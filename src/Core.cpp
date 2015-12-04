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
  connect( mp_broadcaster, SIGNAL( udpPortBlocked() ), this, SLOT( showBroadcasterUdpError() ) );
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
    qWarning() << "User" << Settings::instance().localUser().accountName() << "cannot save settings in path:" << sets->fileName();
    dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER,
                           tr( "%1 User %2 cannot save settings in path: %3" ).arg( Bee::iconToHtml( ":/images/warning.png", "*E*" ) )
                                                                              .arg( Settings::instance().localUser().accountName() )
                                                                              .arg( Bee::convertToNativeFolderSeparator( sets->fileName() ) ),
                           DispatchToChat, ChatMessage::Other );

    settings_can_be_saved = false;
  }
  else
    qDebug() << "Settings will be saved in path:" << qPrintable( sets->fileName() );

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
    QTimer::singleShot( 100, this, SLOT( sendBroadcastMessage() ) );
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
  saveChatMessages();
  Settings::instance().save();
}

bool Core::updateBroadcastAddresses()
{
  if( mp_broadcaster->updateAddresses() > 0 )
  {
    QString sHtmlMsg = tr( "%1 %2 will search users in these IP addresses: %3" )
                .arg( Bee::iconToHtml( ":/images/search-users.png", "*B*" ), Settings::instance().programName(),
                      Settings::instance().broadcastAddressesInSettings().join( ", " ) );
    dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER, sHtmlMsg, DispatchToChat, ChatMessage::Connection );
    return true;
  }
  else
    return false;
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
    mp_broadcaster->sendBroadcastDatagram();
    QTimer::singleShot( 500, this, SLOT( sendHelloToHostsInSettings() ) );
  }
  else
    dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER,
                         tr( "%1 You are not connected to %2 Network." ).arg( Bee::iconToHtml( ":/images/red-ball.png", "*E*" ),
                                                                             Settings::instance().programName() ), DispatchToChat, ChatMessage::Connection );
}

void Core::showBroadcasterUdpError()
{
  dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER,
                         tr( "%1 %2 has found a filter on UDP port %3. Please check your firewall settings." )
                         .arg( Bee::iconToHtml( ":/images/broadcast.png", "*B*" ) )
                         .arg( Settings::instance().programName() )
                         .arg( Settings::instance().defaultBroadcastPort() ),
                         DispatchToChat, ChatMessage::Connection );

  dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER,
                          tr( "%1 You cannot reach %2 Network." ).arg( Bee::iconToHtml( ":/images/red-ball.png", "*E*" ),
                            Settings::instance().programName() ), DispatchToChat, ChatMessage::Connection );
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
    return;

  if( Settings::instance().addSubnetToBroadcastAddress( user_host_address ) )
  {
    QString sHtmlMsg = QString( "%1 %2 %3" )
                           .arg( Bee::iconToHtml( ":/images/broadcast.png", "*B*" ) )
                           .arg( u.path() )
                           .arg( tr( "is connected from external network (the new subnet is added to your broadcast address list)." ) );

    dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), sHtmlMsg, DispatchToAllChatsWithUser, ChatMessage::Connection );
    updateBroadcastAddresses();
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

  UserRecord ur;
  User u;
  int user_contacted = 0;
  foreach( QString user_path, Settings::instance().userPathList() )
  {
    ur = Protocol::instance().loadUserRecord( user_path );
    if( ur.isValid() )
    {
      u = UserManager::instance().findUserByHostAddressAndPort( ur.hostAddress(), ur.hostPort() );
      if( !u.isValid() || !isUserConnected( u.id() ) )
      {
    #ifdef BEEBEEP_DEBUG
        qDebug() << "Contacting manually added host" << ur.hostAddress().toString() << ur.hostPort();
    #endif
        dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER,
                               tr( "%1 Contacting %2 ..." )
                               .arg( Bee::iconToHtml( ":/images/broadcast.png", "*B*" ) )
                               .arg( ur.hostAddressAndPort() ),
                               DispatchToChat, ChatMessage::Connection );
        checkUserRecord( ur );
        user_contacted++;
      }
    }
    else
      qWarning() << "Invalid host address found in settings:" << user_path;
  }

  qDebug() << user_contacted << "hosts manually added contacted";
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
