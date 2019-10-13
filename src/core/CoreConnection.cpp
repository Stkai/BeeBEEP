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

#include "Avatar.h"
#include "BeeUtils.h"
#include "ChatManager.h"
#include "ColorManager.h"
#include "Connection.h"
#include "Core.h"
#include "FileShare.h"
#include "FirewallManager.h"
#include "IconManager.h"
#include "NetworkManager.h"
#include "Protocol.h"
#include "Settings.h"
#include "UserManager.h"
#ifdef BEEBEEP_USE_SHAREDESKTOP
  #include "ShareDesktop.h"
#endif


Connection* Core::connection( VNumber user_id ) const
{
  foreach( Connection* c, m_connections )
  {
    if( c->userId() == user_id )
      return c;
  }
  return Q_NULLPTR;
}

bool Core::hasConnection( const QHostAddress& sender_ip, int sender_port ) const
{
  foreach( Connection* c, m_connections )
  {
    if( (sender_port == -1 || c->peerPort() == sender_port) && c->peerAddress() == sender_ip )
    {
      if( c->isConnected() || c->isConnecting() )
      {
#ifdef BEEBEEP_DEBUG
        qDebug() << "Connection from" << qPrintable( sender_ip.toString() ) << sender_port << "is already opened";
#endif
        return true;
      }
    }
  }

  return false;
}

void Core::checkNetworkAddress( const NetworkAddress& na )
{
  if( na == Settings::instance().localUser().networkAddress() )
    return;

  newPeerFound( na.hostAddress(), na.hostPort() );
}

Connection* Core::createConnection()
{
  Connection *c = new Connection( this );
  m_connections.append( c );
  return c;
}

void Core::newPeerFound( const QHostAddress& sender_ip, int sender_port )
{
  if( !isConnected() )
    return;

#if QT_VERSION >= 0x050000
  if( sender_ip.isLoopback() && sender_port == mp_listener->serverPort() )
    return;
#else
  if( sender_ip == QHostAddress( "127.0.0.1" ) && sender_port == mp_listener->serverPort() )
    return;
#endif

  if( Settings::instance().preventMultipleConnectionsFromSingleHostAddress() )
  {
    if( hasConnection( sender_ip, -1 ) )
    {
      qWarning() << qPrintable( sender_ip.toString() ) << "is already connected and blocked by prevent multiple connections";
      return;
    }
  }
  else
  {
    if( hasConnection( sender_ip, sender_port ) )
    {
      qDebug() << qPrintable( sender_ip.toString() ) << ":" << sender_port << "is already connected (or connecting)";
      return;
    }
  }

  NetworkAddress na( sender_ip, static_cast<quint16>(sender_port) );
  if( isUserConnected( na ) )
  {
    User u = UserManager::instance().findUserByNetworkAddress( na );
    if( u.isValid() )
      qDebug() << "Skip new peer" << qPrintable( sender_ip.toString() ) << "from connected user" << qPrintable( u.name() );
    else
      qWarning() << qPrintable( sender_ip.toString() ) << "is already connected (but user is not authorized yet)";
    return;
  }

  qDebug() << "Connecting to new peer" << qPrintable( sender_ip.toString() ) << sender_port;

  Connection *c = createConnection();
  setupNewConnection( c );
  c->connectToNetworkAddress( na );
}

void Core::checkNewConnection( qintptr socket_descriptor )
{
  Connection *c = createConnection();
  c->initSocket( socket_descriptor, mp_listener->serverPort() );
  qDebug() << "New connection to port" << mp_listener->serverPort() << "from" << qPrintable( c->networkAddress().toString() );
  setupNewConnection( c );
}

void Core::setupNewConnection( Connection *c )
{
#ifdef BEEBEEP_DEBUG
  if( !c->peerAddress().isNull() )
    qDebug() << "Connecting SIGNAL/SLOT to connection from" << qPrintable( c->networkAddress().toString() );
#endif
  connect( c, SIGNAL( error( QAbstractSocket::SocketError ) ), this, SLOT( setConnectionError( QAbstractSocket::SocketError ) ) );
  connect( c, SIGNAL( disconnected() ), this, SLOT( setConnectionClosed() ) );
  connect( c, SIGNAL( abortRequest() ), this, SLOT( setConnectionClosed() ) );
  connect( c, SIGNAL( authenticationRequested( const QByteArray& ) ), this, SLOT( checkUserAuthentication( const QByteArray& ) ) );
}

void Core::addConnectionReadyForUse( Connection* c, const User& u )
{
#ifdef BEEBEEP_DEBUG
  qDebug() << "Connection from" << qPrintable( c->networkAddress().toString() ) << "is ready for use by" << qPrintable( u.path() );
#endif
  c->setReadyForUse( u.id() );
  connect( c, SIGNAL( newMessage( VNumber, const Message& ) ), this, SLOT( parseMessage( VNumber, const Message& ) ) );
}

void Core::setConnectionError( QAbstractSocket::SocketError se )
{
  Connection* c = qobject_cast<Connection*>( sender() );
  if( c )
  {
    qWarning() << "Connection from" << qPrintable( c->networkAddress().toString() ) << "has an error:" << c->errorString() << "-" << static_cast<int>(se);
    closeConnection( c );
  }
  else
    qWarning() << "Connection error" << se << "occurred but the object caller is invalid";
}

void Core::setConnectionClosed()
{
  Connection* c = qobject_cast<Connection*>( sender() );
  if( c )
    closeConnection( c );
  else
    qWarning() << "Connection closed but the object caller is invalid";
}

void Core::closeConnection( Connection *c )
{
  if( !m_connections.removeOne( c ) )
    return;

  if( c->userId() != ID_INVALID )
  {
    User u = UserManager::instance().findUser( c->userId() );
    if( u.isValid() )
    {
      qDebug() << "User" << qPrintable( u.path() ) << "goes offline";
      u.setStatus( User::Offline );
      u.setLastConnection( QDateTime::currentDateTime() );
      UserManager::instance().setUser( u );
      UserManager::instance().removeNewConnectedUserId( u.id() );

      emit userChanged( u );
      emit userConnectionStatusChanged( u );

      Chat default_chat = ChatManager::instance().defaultChat();
      if( default_chat.removeUser( u.id() ) )
        ChatManager::instance().setChat( default_chat );

      FileShare::instance().removeFromNetwork( u.id() );
      emit fileShareAvailable( u );

      if( isConnected() )
        mp_fileTransfer->removeFilesToUser( u.id() );

#ifdef BEEBEEP_USE_SHAREDESKTOP
      if( mp_shareDesktop->isActive() )
        stopShareDesktop( u.id() );
#endif
    }
    else
      qWarning() << "User" << c->userId() << "not found while closing connection";
  }

#ifdef BEEBEEP_DEBUG
  qDebug() << "Deleting connection" << qPrintable( c->networkAddress().toString() );
#endif
  c->disconnect();
  c->abortConnection();
  c->deleteLater();
}

void Core::checkUserAuthentication( const QByteArray& auth_byte_array )
{
  Connection* c = qobject_cast<Connection*>( sender() );
  if( !c )
  {
    qWarning() << "Unable to check authentication for an invalid connection object";
    return;
  }

  Message m = Protocol::instance().toMessage( auth_byte_array, c->protoVersion() );
  if( !m.isValid() )
  {
    qWarning() << "Core has received an invalid HELLO from" << qPrintable( c->networkAddress().toString() );
    closeConnection( c );
    return;
  }

  if( m.type() != Message::Hello )
  {
    qWarning() << "Core is waiting for HELLO, but another message type" << m.type() << "is arrived from" << qPrintable( c->networkAddress().toString() );
    closeConnection( c );
    return;
  }

  User u = Protocol::instance().createUser( m, c->peerAddress() );
  if( !u.isValid() )
  {
    qWarning() << "Unable to create a new user (invalid protocol or password) from the message arrived from:" << qPrintable( c->networkAddress().toString() );
    closeConnection( c );
    return;
  }
  else
    qDebug() << qPrintable( u.path() ) << "has completed the authentication from peer" << qPrintable( c->networkAddress().toString() );

  User user_found = Protocol::instance().recognizeUser( u, Settings::instance().userRecognitionMethod() );
  bool user_path_changed = false;

  if( user_found.isValid() )
  {
    QString sAlertMsg;
    user_path_changed = (u.path() != user_found.path());
    if( user_found.isLocal() )
    {
      if( user_path_changed )
      {
 #ifdef BEEBEEP_DEBUG
        qDebug() << "User path changed:" << u.path() << "vs" << user_found.path();
 #endif
        if( Settings::instance().userRecognitionMethod() == Settings::RecognizeByAccountAndDomain )
        {
          sAlertMsg = tr( "%1 Connection closed to user %2 because it uses your account name: %3." )
                          .arg( IconManager::instance().toHtml( "warning.png", "*E*" ), Bee::replaceHtmlSpecialCharacters( u.path() ), u.accountPath() );
          sAlertMsg += QString( " (%1)" ).arg( tr( "domain" ) );
        }
        else if( Settings::instance().userRecognitionMethod() == Settings::RecognizeByAccount )
        {
          sAlertMsg = tr( "%1 Connection closed to user %2 because it uses your account name: %3." )
                          .arg( IconManager::instance().toHtml( "warning.png", "*E*" ), Bee::replaceHtmlSpecialCharacters( u.path() ), u.accountName() );
        }
        else
        {
          if( u.name() == user_found.name() )
          {
            sAlertMsg = tr( "%1 Connection closed to user %2 because it uses your nickname: %3." )
                          .arg( IconManager::instance().toHtml( "warning.png", "*E*" ), Bee::replaceHtmlSpecialCharacters( u.path() ), Bee::replaceHtmlSpecialCharacters( u.name() ) );
          }
          else
          {
            sAlertMsg = tr( "%1 Connection closed to user %2 because it uses your hash code." )
                          .arg( IconManager::instance().toHtml( "warning.png", "*E*" ), Bee::replaceHtmlSpecialCharacters( u.path() ) );
          }
        }
        dispatchSystemMessage( ID_DEFAULT_CHAT, user_found.id(), sAlertMsg, DispatchToDefaultAndPrivateChat, ChatMessage::Connection );
        qWarning() << "User with account" << qPrintable( u.accountPath() ) << "and path" << qPrintable( u.path() ) << "is recognized to be Local";
      }
      else
        qDebug() << "Test connection from localhost to port" << mp_listener->serverPort() << "was successful";

      closeConnection( c );
      return;
    }

    if( isUserConnected( user_found.id() ) )
    {
      if( user_path_changed )
      {
        if( Settings::instance().userRecognitionMethod() == Settings::RecognizeByAccountAndDomain )
        {
          sAlertMsg = tr( "%1 Connection closed to user %2 because it uses same account name of the already connected user %3: %4." )
                        .arg( IconManager::instance().toHtml( "warning.png", "*E*" ), Bee::replaceHtmlSpecialCharacters( u.path() ), Bee::replaceHtmlSpecialCharacters( user_found.path() ), u.accountPath() );
          sAlertMsg += QString( " (%1)" ).arg( tr( "domain" ) );
          qDebug() << "User" << qPrintable( u.path() ) << "is already connected with account (domain)" << qPrintable( user_found.accountPath() );
        }
        else if( Settings::instance().userRecognitionMethod() == Settings::RecognizeByAccount )
        {
          sAlertMsg = tr( "%1 Connection closed to user %2 because it uses same account name of the already connected user %3: %4." )
                        .arg( IconManager::instance().toHtml( "warning.png", "*E*" ), Bee::replaceHtmlSpecialCharacters( u.path() ), Bee::replaceHtmlSpecialCharacters( user_found.path() ), u.accountName() );
          qDebug() << "User" << qPrintable( u.path() ) << "is already connected with account" << qPrintable( user_found.accountPath() );
        }
        else
        {
          if( u.name() == user_found.name() )
          {
            sAlertMsg = tr( "%1 Connection closed to user %2 because it uses same nickname of the already connected user %3: %4." )
                          .arg( IconManager::instance().toHtml( "warning.png", "*E*" ), Bee::replaceHtmlSpecialCharacters( u.path() ), Bee::replaceHtmlSpecialCharacters( user_found.path() ), Bee::userNameToShow( u ) );
            qDebug() << "User" << qPrintable( u.path() ) << "is already connected with the same nickname of" << qPrintable( user_found.path() );
          }
          else
          {
            sAlertMsg = tr( "%1 Connection closed to user %2 because it uses same hash code of the already connected user %3: %4." )
                          .arg( IconManager::instance().toHtml( "warning.png", "*E*" ), Bee::replaceHtmlSpecialCharacters( u.path() ), Bee::replaceHtmlSpecialCharacters( user_found.path() ), Bee::userNameToShow( u ) );
            qDebug() << "User" << qPrintable( u.path() ) << "is already connected with the same hash code of" << qPrintable( user_found.path() );
          }
        }
        dispatchSystemMessage( ID_DEFAULT_CHAT, user_found.id(), sAlertMsg, DispatchToDefaultAndPrivateChat, ChatMessage::Connection );
      }
      else
        qDebug() << "User" << qPrintable( u.path() ) << "is already online with another connection:" << qPrintable( user_found.path() );

      closeConnection( c );
      return;
    }

    if( user_path_changed )
      qDebug() << "On connection old user found" << qPrintable( user_found.path() ) << "and associated to" << qPrintable( u.path() );

    u.setId( user_found.id() );
    u.setIsFavorite( user_found.isFavorite() );
    u.setColor( user_found.color() );
  }
  else
    qDebug() << "New user connected:" << qPrintable( u.path() );

  if( !ColorManager::instance().isValidColor( u.color() ) || u.color() == QString( "#000000" ) )
    u.setColor( ColorManager::instance().unselectedQString() );
  u.setProtocolVersion( c->protoVersion() );
  u.setLastConnection( QDateTime::currentDateTime() );
  UserManager::instance().setUser( u );

#ifdef BEEBEEP_DEBUG
  qDebug() << "User" << qPrintable( u.path() ) << "added with id" << u.id() << "and color" << qPrintable( u.color() );
#endif

  Chat default_chat = ChatManager::instance().defaultChat();
  if( default_chat.addUser( u.id() ) )
    ChatManager::instance().setChat( default_chat );

  Chat private_chat = ChatManager::instance().privateChatForUser( u.id() );
  if( private_chat.isValid() )
  {
    if( user_found.isValid() && u.name() != user_found.name() )
    {
      ChatManager::instance().changePrivateChatNameAfterUserNameChanged( u.id(), u.name() );
      showUserNameChanged( u, user_found.name() );
    }
  }
  else
    createPrivateChat( u );

  addConnectionReadyForUse( c, u );

  emit userChanged( u );
  emit userConnectionStatusChanged( u );
  showMessage( tr( "%1 users connected" ).arg( connectedUsers() ), 3000 );

  if( c->isEncrypted() )
  {
    if( c->protoVersion() < SECURE_LEVEL_3_PROTO_VERSION )
    {
      dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(),
                             tr( "%1 %2 uses old encryption level." ).arg( IconManager::instance().toHtml( "warning.png", "*!*" ), Bee::userNameToShow( u ) ),
                             DispatchToDefaultAndPrivateChat, ChatMessage::Connection );
    }
  }
  else
  {
    dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(),
                           QString( "%1 %2." ).arg( IconManager::instance().toHtml( "encryption-disabled.png", "*!*" ),
                                                    tr( "Connection with %1 has end-to-end encryption disabled" ).arg( Bee::userNameToShow( u ) ) ),
                           DispatchToDefaultAndPrivateChat, ChatMessage::Connection );
  }

  if( !Settings::instance().localUser().vCard().hasOnlyNickName() )
  {
    if( c->protoVersion() > 1 )
    {
#ifdef BEEBEEP_DEBUG
      qDebug() << "Sending my VCard to" << qPrintable( u.path() );
#endif
      c->sendMessage( Protocol::instance().localVCardMessage() );
    }
  }

  UserManager::instance().addNewConnectedUserId( u.id() );
}

int Core::connectedUsers() const
{
  int connected_users = 0;
  if( m_connections.isEmpty() )
    return connected_users;

  foreach( Connection* c, m_connections )
  {
    if( c->isConnected() && c->isReadyForUse() )
      connected_users++;
  }

  return connected_users;
}

void Core::checkConnectionPorts()
{
  if( isConnected() && connectedUsers() == 0 )
  {
    // FIXME: it works always... not useful to check if firewall ports are opened
    NetworkAddress na( NetworkManager::instance().localHostAddress(), mp_listener->serverPort() );
    qDebug() << "Checking connection to localhost" << qPrintable( na.toString() );
    Connection *c = createConnection();
    setupNewConnection( c );
    c->connectToNetworkAddress( na );
  }
}

void Core::checkFirewall()
{
  // FIXME: how to do this?
  FirewallManager::instance().allowApplication( Settings::instance().programName(), QDir::toNativeSeparators( qApp->applicationFilePath() ) );
}
