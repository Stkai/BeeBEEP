//////////////////////////////////////////////////////////////////////
//
// This file is part of BeeBEEP.
//
// BeeBEEP is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License,
// or (at your option) any later version.
//
// BeeBEEP is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with BeeBEEP.  If not, see <http://www.gnu.org/licenses/>.
//
// Author: Marco Mastroddi <marco.mastroddi(AT)gmail.com>
//
// $Id$
//
//////////////////////////////////////////////////////////////////////

#include "BeeUtils.h"
#include "Connection.h"
#include "Core.h"
#include "Broadcaster.h"
#include "FileShare.h"
#include "SaveChatList.h"
#include "Settings.h"
#include "NetworkManager.h"
#include "Protocol.h"
#include "UserManager.h"
#include "Updater.h"
#include "GAnalytics.h"
#ifdef BEEBEEP_USE_MULTICAST_DNS
  #include "MDnsManager.h"
#endif


Core::Core( QObject* parent )
 : QObject( parent ), m_connections()
{
  qDebug() << "Core created";
  createDefaultChat();

  mp_listener = new Listener( this );
  qDebug() << "Listener created";
  mp_broadcaster = new Broadcaster( this );
  qDebug() << "Broadcaster created";
  mp_fileTransfer = new FileTransfer( this );
  qDebug() << "FileTransfer created";
  m_shareListToBuild = 0;

#ifdef BEEBEEP_USE_MULTICAST_DNS
  mp_mDns = new MDnsManager( this );
  connect( mp_mDns, SIGNAL( serviceRegistered() ), this, SLOT( sendMulticastingMessage() ) );
  connect( mp_mDns, SIGNAL( newUserFound( const UserRecord& ) ), this, SLOT( checkUserRecord( const UserRecord& ) ) );
#endif

  connect( mp_broadcaster, SIGNAL( newPeerFound( const QHostAddress&, int ) ), this, SLOT( newPeerFound( const QHostAddress&, int ) ) );
  //connect( mp_broadcaster, SIGNAL( udpPortBlocked() ), this, SLOT( showBroadcasterUdpError() ) );
  connect( mp_listener, SIGNAL( newConnection( Connection* ) ), this, SLOT( checkNewConnection( Connection* ) ) );
  connect( mp_fileTransfer, SIGNAL( listening() ), this, SLOT( fileTransferServerListening() ) );
  connect( mp_fileTransfer, SIGNAL( userConnected( VNumber, const QHostAddress&, const Message& ) ), this, SLOT( validateUserForFileTransfer( VNumber, const QHostAddress&, const Message& ) ) );
  connect( mp_fileTransfer, SIGNAL( progress( VNumber, VNumber, const FileInfo&, FileSizeType ) ), this, SLOT( checkFileTransferProgress( VNumber, VNumber, const FileInfo&, FileSizeType ) ) );
  connect( mp_fileTransfer, SIGNAL( message( VNumber, VNumber, const FileInfo&, const QString& ) ), this, SLOT( checkFileTransferMessage( VNumber, VNumber, const FileInfo&, const QString& ) ) );
}

bool Core::checkSavingPaths()
{
  bool settings_can_be_saved = true;
  bool chats_can_be_saved = true;

  QSettings* sets = Settings::instance().objectSettings();
  sets->deleteLater();
  if( !sets->isWritable() )
  {
    qWarning() << "User" << Settings::instance().localUser().accountName() << "cannot save settings in path:" << qPrintable( Bee::convertToNativeFolderSeparator( sets->fileName() ) );
    dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER,
                           tr( "%1 User %2 cannot save settings in path: %3" ).arg( Bee::iconToHtml( ":/images/warning.png", "*E*" ) )
                                                                              .arg( Settings::instance().localUser().accountName() )
                                                                              .arg( Bee::convertToNativeFolderSeparator( sets->fileName() ) ),
                           DispatchToChat, ChatMessage::Other );

    settings_can_be_saved = false;
  }
  else
    qDebug() << "Settings will be saved in path:" << qPrintable( Bee::convertToNativeFolderSeparator( sets->fileName() ) );

  if( Settings::instance().chatAutoSave() )
  {
    if( !SaveChatList::canBeSaved() )
    {
      qWarning() << "User" << Settings::instance().localUser().accountName() << "cannot save chat messages in path:" << qPrintable( Settings::instance().savedChatsFilePath() );
      dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER,
                             tr( "%1 User %2 cannot save chat messages in path: %3" ).arg( Bee::iconToHtml( ":/images/warning.png", "*E*" ) )
                                                                                .arg( Settings::instance().localUser().accountName() )
                                                                                .arg( Settings::instance().savedChatsFilePath() ),
                             DispatchToChat, ChatMessage::Other );

      chats_can_be_saved = false;
    }
    else
      qDebug() << "Chat messages will be saved in path:" << qPrintable( Settings::instance().savedChatsFilePath() );
  }
  else
    qDebug() << "Save chat messages options is disabled";

  return settings_can_be_saved && chats_can_be_saved;
}

bool Core::start()
{
  qDebug() << "Starting" << Settings::instance().programName() << "core";

  if( !mp_listener->listen( Settings::instance().hostAddressToListen(), Settings::instance().defaultListenerPort() ) )
  {
    qWarning() << "Unable to bind default listener port" << Settings::instance().defaultListenerPort();

    if( !mp_listener->listen( Settings::instance().hostAddressToListen(), Settings::instance().localUser().hostPort() ) )
    {
      qDebug() << "Unable to bind last used listener port" << Settings::instance().localUser().hostPort();
      if( !mp_listener->listen( Settings::instance().hostAddressToListen() ) )
      {
        dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER,
                             tr( "%1 Unable to connect to %2 Network. Please check your firewall settings." )
                               .arg( Bee::iconToHtml( ":/images/network-disconnected.png", "*E*" ),
                                     Settings::instance().programName() ), DispatchToChat, ChatMessage::Connection );
        qWarning() << "Unable to bind a valid listener port";
        return false;
      }
    }
  }

  qDebug() << "Listener binds" << mp_listener->serverAddress().toString() << mp_listener->serverPort();
  NetworkManager::instance().searchLocalHostAddress();

  Settings::instance().setLocalUserHost( NetworkManager::instance().localHostAddress(), mp_listener->serverPort() );
  if( Settings::instance().localUser().sessionId().isEmpty() )
    Settings::instance().createSessionId();

  Settings::instance().createApplicationUuid();

#ifdef BEEBEEP_DEBUG
  qDebug() << "Network password used:" << Settings::instance().passwordBeforeHash();
#endif

  if( !mp_broadcaster->startBroadcasting() )
  {
    dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER,
                           tr( "%1 Unable to broadcast to %2 Network. Please check your firewall settings." )
                             .arg( Bee::iconToHtml( ":/images/network-disconnected.png", "*E*" ),
                                   Settings::instance().programName() ), DispatchToChat, ChatMessage::Connection );
    mp_listener->close();
    return false;
  }
  else
  {
    qDebug() << "Broadcaster starts broadcasting with tcp listener port" << Settings::instance().localUser().hostPort() << "and udp port" << Settings::instance().defaultBroadcastPort();
    QTimer::singleShot( 2000, this, SLOT( sendBroadcastMessage() ) );
  }

  dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER,
                         tr( "%1 You are connected to %2 Network." )
                         .arg( Bee::iconToHtml( ":/images/network-connected.png", "*C*" ),
                               Settings::instance().programName() ), DispatchToAllChatsWithUser, ChatMessage::Connection );

#ifdef BEEBEEP_USE_MULTICAST_DNS
  startDnsMulticasting();
#endif

  if( Settings::instance().acceptConnectionsOnlyFromWorkgroups() && !Settings::instance().workgroups().isEmpty() )
  {
    dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER,
                           tr( "%1 You have selected to join only in these workgroups: %2" )
                           .arg( Bee::iconToHtml( ":/images/group.png", "*C*" ) ).arg( Settings::instance().workgroups().join( ", " ) ),
                           DispatchToAllChatsWithUser, ChatMessage::Connection );
    qDebug() << "Protocol accepts connections only from these workgroups:" << qPrintable( Settings::instance().workgroups().join( ", " ) );
  }

  if( Settings::instance().fileTransferIsEnabled() )
    startFileTransferServer();
  else
    qWarning() << "File transfer is disabled";

  if( Settings::instance().showTipsOfTheDay() )
  {
    showTipOfTheDay();
    showFactOfTheDay();
  }

  if( Settings::instance().localUser().status() == User::Offline )
  {
    User u = Settings::instance().localUser();
    u.setStatus( User::Online );
    Settings::instance().setLocalUser( u );
  }

  showUserStatusChanged( Settings::instance().localUser() );
  showUserVCardChanged( Settings::instance().localUser() );

  qDebug() << "Local user path:" << Settings::instance().localUser().path();

  checkSavingPaths();

  if( Settings::instance().checkNewVersionAtStartup() )
    QTimer::singleShot( 0, this, SLOT( checkNewVersion() ) );

  if( Settings::instance().canPostUsageStatistics() )
    QTimer::singleShot( 5000, this, SLOT( postUsageStatistics() ) );

  return true;
}

#ifdef BEEBEEP_USE_MULTICAST_DNS
void Core::startDnsMulticasting()
{
  if( mp_mDns->isActive() )
    return;

  if( Settings::instance().useMulticastDns() )
  {
    if( mp_mDns->start( Settings::instance().programName(),
                        Settings::instance().dnsRecord(),
                        Settings::instance().localUser().hostAddress().toString(),
                        mp_listener->serverPort() ) )
    {
      dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER,
                             tr( "%1 Zero Configuration started with service name: %2" )
                               .arg( Bee::iconToHtml( ":/images/mdns.png", "*C*" ), Settings::instance().dnsRecord() ),
                             DispatchToChat, ChatMessage::Connection );
    }
  }
}

void Core::stopDnsMulticasting()
{
  if( !mp_mDns->isActive() )
    return;

  if( mp_mDns->stop() )
  {
    dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER,
                             tr( "%1 Zero Configuration service closed." )
                               .arg( Bee::iconToHtml( ":/images/mdns.png", "*C*" ) ),
                             DispatchToChat, ChatMessage::Connection );
  }
}

bool Core::dnsMulticastingIsActive() const
{
  return mp_mDns->isActive();
}

#endif

void Core::stop()
{
  qDebug() << "Closing core...";
  mp_broadcaster->stopBroadcasting();

#ifdef BEEBEEP_USE_MULTICAST_DNS
  stopDnsMulticasting();
#endif

  stopFileTransferServer();

  mp_listener->close();

  foreach( Connection* c, m_connections )
    closeConnection( c );

  m_connections.clear();

  dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER,
                         tr( "%1 You are disconnected from %2 Network.")
                           .arg( Bee::iconToHtml( ":/images/network-disconnected.png", "*D*" ),
                           Settings::instance().programName() ), DispatchToAllChatsWithUser,
                           ChatMessage::Connection );

  checkSavingPaths();
  saveUsersAndGroups();
  Settings::instance().save();
  qDebug() << "Core closed";
}


void Core::sendMulticastingMessage()
{
  if( !Settings::instance().useMulticastDns() )
    return;

#ifdef BEEBEEP_USE_MULTICAST_DNS
  if( mp_mDns->isActive() )
  {
    if( mp_mDns->browseForService() )
    {
      dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER,
                               tr( "%1 Zero Configuration is browsing network for service: %2" )
                                 .arg( Bee::iconToHtml( ":/images/mdns.png", "*C*" ), Settings::instance().dnsRecord() ),
                               DispatchToChat, ChatMessage::Connection );
    }
    else
    {
      dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER,
                               tr( "%1 Zero Configuration cannot browse network for service: %2" )
                                 .arg( Bee::iconToHtml( ":/images/mdns.png", "*C*" ), Settings::instance().dnsRecord() ),
                               DispatchToChat, ChatMessage::Connection );
    }
  }
#ifdef BEEBEEP_DEBUG
  else
    qDebug() << "Unable to send multicast message because MDnsManager is not active";
#endif

#endif
}

void Core::sendBroadcastMessage()
{
  if( isConnected() )
  {
    dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER,
                           tr( "%1 Broadcasting to the %2 Network..." ).arg( Bee::iconToHtml( ":/images/broadcast.png", "*B*" ),
                                                                            Settings::instance().programName() ), DispatchToChat, ChatMessage::Connection );
    QTimer::singleShot( 0, mp_broadcaster, SLOT( sendBroadcastDatagram() ) );
  }
  else
    dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER,
                         tr( "%1 You are not connected to %2 Network." ).arg( Bee::iconToHtml( ":/images/red-ball.png", "*E*" ),
                                                                             Settings::instance().programName() ), DispatchToChat, ChatMessage::Connection );
}

void Core::showBroadcasterUdpError()
{
  QString html_msg = tr( "%1 %2 has found a filter on UDP port %3. Please check your firewall settings." )
                       .arg( Bee::iconToHtml( ":/images/broadcast.png", "*B*" ) )
                       .arg( Settings::instance().programName() )
                       .arg( Settings::instance().defaultBroadcastPort() );
  html_msg += QString( " %1." ).arg( tr( "View the log messages for more informations" ) );
  dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER, html_msg, DispatchToChat, ChatMessage::Connection );
}

bool Core::isConnected() const
{
  return mp_listener->isListening();
}

void Core::checkUserHostAddress( const User& u )
{
  if( !Settings::instance().addExternalSubnetAutomatically() )
    return;

  QHostAddress user_host_address = NetworkManager::instance().broadcastSubnetFromIPv4HostAddress( u.hostAddress() );

  if( user_host_address.isNull() )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Invalid broadcast address for user" << u.path();
#endif
    return;
  }

  if( user_host_address == Settings::instance().localUser().hostAddress() )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Skip local user address for user" << u.path();
#endif
    return;
  }

  if( user_host_address == NetworkManager::instance().localBroadcastAddress() )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Skip subnet in the base broadcast for user" << u.path();
#endif
    return;
  }

  if( Settings::instance().addSubnetToBroadcastAddress( user_host_address ) )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "External broadcast address" << user_host_address << "is added for user" << u.path();
#endif
    QString sHtmlMsg = QString( "%1 %2 %3" )
                           .arg( Bee::iconToHtml( ":/images/broadcast.png", "*B*" ) )
                           .arg( u.path() )
                           .arg( tr( "is connected from external network (the new subnet is added to your broadcast address list)." ) );

    dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), sHtmlMsg, DispatchToAllChatsWithUser, ChatMessage::Connection );
  }
}

void Core::sendHelloToHostsInSettings()
{
  if( Settings::instance().userPathList().isEmpty() )
    return;

  dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER,
                         tr( "%1 Checking %2 more addresses..." )
                         .arg( Bee::iconToHtml( ":/images/broadcast.png", "*B*" ) )
                         .arg( Settings::instance().userPathList().size() ),
                         DispatchToChat, ChatMessage::Connection );

  QTimer::singleShot( 0, mp_broadcaster, SLOT( sendBroadcastDatagram() ) );
}

int Core::fileTransferPort() const
{
  if( mp_fileTransfer->isListening() )
    return mp_fileTransfer->serverPort();
  else
    return Settings::instance().defaultFileTransferPort();
}

void Core::checkNetworkInterface()
{
  if( !m_connections.isEmpty() )
  {
    int max_activity_idle = Settings::instance().tickIntervalCheckNetwork() * 1000;
    foreach( Connection* c, m_connections )
    {
      if( c->isConnected() && c->activityIdle() < max_activity_idle )
        return;
    }
  }

  if( !NetworkManager::instance().isMainInterfaceUp() )
  {
    if( isConnected() )
    {
      qWarning() << "Network interface is gone down";
      dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER,
                             tr( "%1 Network interface %2 is gone down.")
                               .arg( Bee::iconToHtml( ":/images/network-disconnected.png", "*D*" ),
                               NetworkManager::instance().localInterfaceHardwareAddress() ), DispatchToChat,
                               ChatMessage::Connection );
      emit networkInterfaceIsDown();
    }
  }
  else
  {
    if( !isConnected() )
      emit networkInterfaceIsUp();
  }
}

bool Core::saveChatMessages()
{
  if( !Settings::instance().enableSaveData() )
  {
    qDebug() << "Skip saving chat messages because you have disabled this option in RC file";
    return false;
  }

  if( !Settings::instance().chatAutoSave() )
  {
    qDebug() << "Save chat messages option is disabled";
    return false;
  }

  if( !SaveChatList::canBeSaved() )
  {
    qWarning() << "Chat messages can not be saved in path:" << qPrintable( Settings::instance().savedChatsFilePath() );
    return false;
  }

  SaveChatList scl;
  scl.save();
  qDebug() << "Chat messages are saved in path:" << qPrintable( Settings::instance().savedChatsFilePath() );
  return true;
}

void Core::checkNewVersion()
{
  qDebug() << "Checking for new version...";
  Updater* updater = new Updater( this );
  connect( updater, SIGNAL( jobCompleted() ), this, SLOT( onUpdaterJobCompleted() ) );
  QTimer::singleShot( 0, updater, SLOT( checkForNewVersion() ) );
}

void Core::onUpdaterJobCompleted()
{
  Updater *updater = qobject_cast<Updater*>( sender() );
  if( !updater )
  {
    qWarning() << "Core received a signal from invalid Updater instance";
    return;
  }

  QString latest_version = updater->versionAvailable();
  QString download_url = updater->downloadUrl().isEmpty() ? Settings::instance().downloadWebSite() : updater->downloadUrl();

  updater->deleteLater();

  if( latest_version.isEmpty() )
    return;

  QString my_version = Settings::instance().version( false, false );

  if( my_version >= latest_version )
  {
    qDebug() << "Latest version on the website is" << qPrintable( latest_version ) << "(update is not needed)";
    return;
  }

  qDebug() << "Latest version on website is" << qPrintable( latest_version ) << "(update is recommended)";

  QString html_msg = QString( "%1 <b>%2</b>. <a href=""%3"">%4</a>." ).arg( Bee::iconToHtml( ":/images/update.png", "*!*" ),
                                                           tr( "New version is available" ), download_url,
                                                           tr( "Click here to download" ) );

  dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER, html_msg, DispatchToChat, ChatMessage::Other );
}

void Core::checkBroadcastInterval()
{
  if( isConnected() )
    mp_broadcaster->enableBroadcastTimer( Settings::instance().broadcastInterval() > 0 );
}

void Core::postUsageStatistics()
{
  GAnalytics* ga = new GAnalytics( this );
#ifdef BEEBEEP_DEBUG
  qDebug() << qPrintable( ga->objectName() ) << "created";
#endif
  connect( ga, SIGNAL( jobFinished() ), this, SLOT( onPostUsageStatisticsJobCompleted() ) );
  QTimer::singleShot( 0, ga, SLOT( doPost() ) );
}

void Core::onPostUsageStatisticsJobCompleted()
{
  GAnalytics *ga = qobject_cast<GAnalytics*>( sender() );
  if( !ga )
  {
#ifdef BEEBEEP_DEBUG
    qWarning() << "Core received a signal from invalid GAnalytics instance";
#endif
    return;
  }

#ifdef BEEBEEP_DEBUG
  qDebug() << qPrintable( ga->objectName() ) << "will be cleared";
#endif

  ga->deleteLater();
}

void Core::onTickEvent( int ticks )
{
  if( ticks % 15 == 0 && isConnected() && Settings::instance().autoSearchUsersWhenListIsEmpty() && connectedUsers() == 0 )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Auto search for users is enabled. It's time to check..." << ticks;
#endif
    QTimer::singleShot( 0, mp_broadcaster, SLOT( sendBroadcastDatagram() ) );
  }
  else
    mp_broadcaster->onTickEvent( ticks );
}