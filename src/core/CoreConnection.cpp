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
#include "NetworkManager.h"
#include "Protocol.h"
#include "Settings.h"
#include "UserManager.h"


Connection* Core::connection( VNumber user_id ) const
{
  foreach( Connection* c, m_connections )
  {
    if( c->userId() == user_id )
      return c;
  }
  return 0;
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
        qDebug() << "Connection from" << sender_ip.toString() << sender_port << "is already open";
#endif
        return true;
      }
    }
  }

  User u = UserManager::instance().findUserByHostAddressAndPort( sender_ip, sender_port );
  if( u.isValid() && isUserConnected( u.id() ) )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "User from" << qPrintable( sender_ip.toString() ) << sender_port << "is already connected";
#endif
    return true;
  }

  return false;
}

void Core::checkUserRecord( const UserRecord& ur )
{
  if( ur.networkAddress() == Settings::instance().localUser().networkAddress() )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Skip local user record:" << qPrintable( ur.networkAddress().toString() );
#endif
    return;
  }

  newPeerFound( ur.networkAddress().hostAddress(), ur.networkAddress().hostPort() );
}

void Core::newPeerFound( const QHostAddress& sender_ip, int sender_port )
{
  if( !isConnected() )
    return;

  if( hasConnection( sender_ip, sender_port ) )
    return;

  qDebug() << "Connecting to new peer" << qPrintable( sender_ip.toString() ) << sender_port;

  Connection *c = new Connection( this );
  setupNewConnection( c );
  c->connectToNetworkAddress( NetworkAddress( sender_ip, sender_port ) );
}

void Core::checkNewConnection( qintptr socket_descriptor )
{
  // Has connection never return true because peer port is always different.
  // It comes from Listener. If I want to prevent multiple users from single
  // ip, I can pass -1 to peer_port and check only host address

  Connection *c = new Connection( this );
  c->initSocket( socket_descriptor );
  qDebug() << "New connection from" << qPrintable( c->networkAddress().toString() );

  if( Settings::instance().preventMultipleConnectionsFromSingleHostAddress() )
  {
    if( hasConnection( c->peerAddress(), -1 ) )
    {
#ifdef BEEBEEP_DEBUG
      qDebug() << qPrintable( c->peerAddress().toString() ) << "is already connected and blocked by prevent multiple connections";
#endif
      closeConnection( c );
      return;
    }
  }

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

void Core::addConnectionReadyForUse( Connection* c )
{
#ifdef BEEBEEP_DEBUG
  qDebug() << "Connection from" << qPrintable( c->networkAddress().toString() ) << "is ready for use";
#endif
  connect( c, SIGNAL( newMessage( VNumber, const Message& ) ), this, SLOT( parseMessage( VNumber, const Message& ) ) );
  m_connections.append( c );
}

void Core::setConnectionError( QAbstractSocket::SocketError se )
{
  Connection* c = qobject_cast<Connection*>( sender() );
  if( c )
  {
    if( !c->peerAddress().isNull() )
      qWarning() << "Connection from" << qPrintable( c->networkAddress().toString() ) << "has refused connection:" << c->errorString() << "-" << (int)se;
    else
      qWarning() << "Connection from" << qPrintable( c->networkAddress().toString() ) << "has an error:" << c->errorString() << "-" << (int)se;
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
  m_connections.removeOne( c );

  if( c->userId() != ID_INVALID )
  {
    User u = UserManager::instance().findUser( c->userId() );
    if( u.isValid() )
    {
      qDebug() << "User" << u.path() << "goes offline";
      u.setStatus( User::Offline );
      UserManager::instance().setUser( u );
      showUserStatusChanged( u );

      emit userConnectionStatusChanged( u );

      Chat default_chat = ChatManager::instance().defaultChat();
      if( default_chat.removeUser( u.id() ) )
        ChatManager::instance().setChat( default_chat );

      FileShare::instance().removeFromNetwork( c->userId() );
      if( u.isValid() )
        emit fileShareAvailable( u );
    }
    else
      qWarning() << "User" << c->userId() << "not found while closing connection";
  }

  c->disconnect();
  c->closeConnection();
  c->deleteLater();

  if( isConnected() && m_connections.isEmpty() )
    QTimer::singleShot( 0, this, SLOT( checkNetworkInterface() ) );
}

void Core::checkUserAuthentication( const QByteArray& auth_byte_array )
{
  Connection* c = qobject_cast<Connection*>( sender() );
  if( !c )
  {
    qWarning() << "Unable to check authentication for an invalid connection object";
    return;
  }

  Message m = Protocol::instance().toMessage( auth_byte_array );
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
    qDebug() << qPrintable( u.path() ) << "has completed the authentication";

  bool user_path_changed = false;

  User user_found = UserManager::instance().findUserBySessionId( u.sessionId() );
  if( !user_found.isValid() )
  {
    if( Settings::instance().trustSystemAccount() )
    {
      user_found = UserManager::instance().findUserByAccountName( u.accountName() );
      if( user_found.isValid() )
        qDebug() << "User found in list with account name:" << u.accountName();
    }
    else
    {
      user_found = UserManager::instance().findUserByPath( u.path() );
      if( user_found.isValid() )
        qDebug() << "User found in list with path:" << u.path();
    }
  }

  if( user_found.isValid() )
  {
    if( user_found.isLocal() )
    {
#ifdef BEEBEEP_DEBUG
      qDebug() << "User with account" << u.accountName() << "and path" << u.path() << "is recognized to be Local";
#endif
      closeConnection( c );
      return;
    }

    if(  isUserConnected( user_found.id() ) )
    {
#ifdef BEEBEEP_DEBUG
      qDebug() << "User with account" << u.accountName() << "and path" << u.path() << "is already connected with account name" << user_found.accountName() << "path" << user_found.path();
#endif
      c->setUserId( ID_INVALID );
      closeConnection( c );
      return;
    }

    if( u.path() != user_found.path() )
    {
      user_path_changed = true;
      qDebug() << "On connection old user found" << user_found.path() << "and associated to" << u.path();
    }
    else
      qDebug() << "User" << u.path() << "reconnected";

    u.setId( user_found.id() );
    u.setIsFavorite( user_found.isFavorite() );
  }
  else
  {
    qDebug() << "New user is connected from" << u.path();
  }

#if QT_VERSION < 0x040700
  if( u.color() == QString( "#000000" ) || !QColor( u.color() ).isValid() )
#else
  if( u.color() == QString( "#000000" ) || !QColor::isValidColor( u.color() ) )
#endif
  {
    if( user_found.isValid() && user_found.color() != QString( "#000000" ) )
      u.setColor( user_found.color() );
    else
      u.setColor( ColorManager::instance().unselectedQString() );
  }

  u.setProtocolVersion( c->protoVersion() );
  UserManager::instance().setUser( u );
#ifdef BEEBEEP_DEBUG
  qDebug() << "User" << u.path() << "added with id" << u.id() << "and color" << u.color();
#endif

  Chat default_chat = ChatManager::instance().defaultChat();
  if( default_chat.addUser( u.id() ) )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Adding user" << u.path() << "to default chat";
#endif
    ChatManager::instance().setChat( default_chat );
  }

  if( !ChatManager::instance().privateChatForUser( u.id() ).isValid() )
    createPrivateChat( u );

  c->setReadyForUse( u.id() );
  addConnectionReadyForUse( c );

  if( user_path_changed )
    ChatManager::instance().changePrivateChatNameAfterUserNameChanged( user_found.id(), u.path() );

  showUserStatusChanged( u );

  emit userConnectionStatusChanged( u );

  if( !Settings::instance().localUser().vCard().hasOnlyNickName() )
  {
    if( c->protoVersion() > 1 )
    {
#ifdef BEEBEEP_DEBUG
      qDebug() << "Sending my VCard to" << u.path();
#endif
      c->sendData( Protocol::instance().localVCardMessage() );
    }
  }

  if( user_found.isValid() )
    checkGroupChatAfterUserReconnect( u );

  checkOfflineMessagesForUser( u );
  if( Settings::instance().useHive() && u.protocolVersion() >= HIVE_PROTO_VERSION )
    sendLocalConnectedUsersTo( u );
}
