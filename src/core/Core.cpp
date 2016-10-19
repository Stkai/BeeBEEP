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
#ifdef BEEBEEP_USE_SHAREDESKTOP
  #include "ShareDesktop.h"
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

#ifdef BEEBEEP_USE_SHAREDESKTOP
  mp_shareDesktop = new ShareDesktop( this );
  qDebug() << "ShareDesktop created";
#endif

  connect( mp_broadcaster, SIGNAL( newPeerFound( const QHostAddress&, int ) ), this, SLOT( newPeerFound( const QHostAddress&, int ) ) );
  connect( mp_listener, SIGNAL( newConnection( qintptr ) ), this, SLOT( checkNewConnection( qintptr ) ) );
  connect( mp_fileTransfer, SIGNAL( listening() ), this, SLOT( fileTransferServerListening() ) );
  connect( mp_fileTransfer, SIGNAL( progress( VNumber, VNumber, const FileInfo&, FileSizeType ) ), this, SLOT( checkFileTransferProgress( VNumber, VNumber, const FileInfo&, FileSizeType ) ) );
  connect( mp_fileTransfer, SIGNAL( message( VNumber, VNumber, const FileInfo&, const QString& ) ), this, SLOT( checkFileTransferMessage( VNumber, VNumber, const FileInfo&, const QString& ) ) );
  connect( mp_fileTransfer, SIGNAL( completed( VNumber, VNumber, const FileInfo& ) ), this, SLOT( onFileTransferCompleted( VNumber, VNumber, const FileInfo& ) ) );
#ifdef BEEBEEP_USE_SHAREDESKTOP
  connect( mp_shareDesktop, SIGNAL( shareDesktopDataReady( const QByteArray& ) ), this, SLOT( onShareDesktopDataReady( const QByteArray& ) ) );
#endif
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

    if( !mp_listener->listen( Settings::instance().hostAddressToListen(), Settings::instance().localUser().networkAddress().hostPort() ) )
    {
      qDebug() << "Unable to bind last used listener port" << Settings::instance().localUser().networkAddress().hostPort();
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

  if( !mp_broadcaster->startBroadcastServer() )
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
    qDebug() << "Broadcaster starts broadcasting with tcp listener port" << Settings::instance().localUser().networkAddress().hostPort() << "and udp port" << Settings::instance().defaultBroadcastPort();
    QTimer::singleShot( 5000, this, SLOT( sendBroadcastMessage() ) );
  }

  dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER,
                         tr( "%1 You are connected to %2 Network." )
                         .arg( Bee::iconToHtml( ":/images/network-connected.png", "*C*" ),
                               Settings::instance().programName() ), DispatchToChat, ChatMessage::Connection );

#ifdef BEEBEEP_USE_MULTICAST_DNS
  QTimer::singleShot( 12000, this, SLOT( startDnsMulticasting() ) );
#endif

  if( Settings::instance().acceptConnectionsOnlyFromWorkgroups() && !Settings::instance().workgroups().isEmpty() )
  {
    dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER,
                           tr( "%1 You have selected to join only in these workgroups: %2" )
                           .arg( Bee::iconToHtml( ":/images/group.png", "*C*" ) ).arg( Settings::instance().workgroups().join( ", " ) ),
                           DispatchToChat, ChatMessage::Connection );
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
  emit userChanged( Settings::instance().localUser() );

  qDebug() << "Local user path:" << Settings::instance().localUser().path();

  checkSavingPaths();

  if( Settings::instance().checkNewVersionAtStartup() )
    QTimer::singleShot( 1000, this, SLOT( checkNewVersion() ) );

  if( Settings::instance().canPostUsageStatistics() )
    QTimer::singleShot( 7000, this, SLOT( postUsageStatistics() ) );

  return true;
}

#ifdef BEEBEEP_USE_MULTICAST_DNS
void Core::startDnsMulticasting()
{
  if( dnsMulticastingIsActive() )
    return;

  if( Settings::instance().useMulticastDns() )
  {
    if( mp_mDns->start( Settings::instance().programName(),
                        Settings::instance().dnsRecord(),
                        Settings::instance().localUser().networkAddress().hostAddress().toString(),
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
  if( !dnsMulticastingIsActive() )
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
  qDebug() << "Stopping network core...";
  mp_broadcaster->stopBroadcasting();

#ifdef BEEBEEP_USE_MULTICAST_DNS
  stopDnsMulticasting();
#endif

#ifdef BEEBEEP_USE_SHAREDESKTOP
  if( mp_shareDesktop->isActive() )
    mp_shareDesktop->stop();
 #endif

  stopFileTransferServer();

  mp_listener->close();

  foreach( Connection* c, m_connections )
    closeConnection( c );

  m_connections.clear();

  dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER,
                         tr( "%1 You are disconnected from %2 Network.")
                           .arg( Bee::iconToHtml( ":/images/network-disconnected.png", "*D*" ),
                           Settings::instance().programName() ), DispatchToChat,
                           ChatMessage::Connection );

  checkSavingPaths();
  saveUsersAndGroups();
  saveChatMessages();
  Settings::instance().save();
  qDebug() << "Network core stopped";
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
    QTimer::singleShot( 0, mp_broadcaster, SLOT( sendBroadcast() ) );
  }
  else
    dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER,
                         tr( "%1 You are not connected to %2 Network." ).arg( Bee::iconToHtml( ":/images/red-ball.png", "*E*" ),
                                                                             Settings::instance().programName() ), DispatchToChat, ChatMessage::Connection );
}

bool Core::isConnected() const
{
  return mp_listener->isListening();
}

int Core::fileTransferPort() const
{
  if( mp_fileTransfer->isActive() )
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
      return;
    }

    if( NetworkManager::instance().isMainInterfaceUnavailable() )
    {
#ifdef BEEBEEP_DEBUG
      qDebug() << "Main network interface is not available. Searching...";
#endif
      if( NetworkManager::instance().searchLocalHostAddress() )
        QTimer::singleShot( 0, this, SLOT( checkNetworkInterface() ) );
      else
        qWarning() << "Network iterface not found. Please check your connection";
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

  SaveChatList scl;
  if( scl.save() )
  {
    qDebug() << "Chat messages are saved in path:" << qPrintable( Settings::instance().savedChatsFilePath() );
    return true;
  }
  else
    return false;
}

void Core::checkNewVersion()
{
  if( !NetworkManager::instance().isMainInterfaceUp() )
    return;

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

void Core::postUsageStatistics()
{
  if( !NetworkManager::instance().isMainInterfaceUp() )
    return;

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
  if( isConnected() )
  {
    mp_broadcaster->onTickEvent( ticks );
    if( Settings::instance().tickIntervalBroadcasting() > 0 && (ticks % Settings::instance().tickIntervalBroadcasting() == 0) )
      mp_broadcaster->setNewBroadcastRequested( true );
  }

  if( Protocol::instance().currentId() >= Protocol::instance().maxId() )
  {
    QString html_msg = QString( "%1 <b>%2</b>." ).arg( Bee::iconToHtml( ":/images/warning.png", "*!*" ),
                                                       tr( "Max ID is reached. Please close and restart the application." ) );
    dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER, html_msg, DispatchToChat, ChatMessage::Other );
  }
}
