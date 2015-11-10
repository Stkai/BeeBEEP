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
#include "Protocol.h"
#include "Settings.h"
#include "UserManager.h"


Connection* Core::connection( VNumber user_id ) const
{
  QList<Connection*>::const_iterator it = m_connections.begin();
  while( it != m_connections.end() )
  {
    if( (*it)->userId() == user_id )
      return *it;
    ++it;
  }
  return 0;
}

bool Core::hasConnection( const QHostAddress& sender_ip, int sender_port ) const
{
  QList<Connection*>::const_iterator it = m_connections.begin();
  while( it != m_connections.end() )
  {
    if( (sender_port == -1 || (*it)->peerPort() == sender_port) && (*it)->peerAddress() == sender_ip )
    {
      if( (*it)->isConnected() )
      {
#ifdef BEEBEEP_DEBUG
        qDebug() << "Connection from" << sender_ip.toString() << sender_port << "is already open";
#endif
        return true;
      }
    }
    ++it;
  }

  User u = UserManager::instance().findUserByHostAddressAndPort( sender_ip, sender_port );
  if( u.isValid() && isUserConnected( u.id() ) )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "User from" << sender_ip.toString() << sender_port << "is already connected";
#endif
    return true;
  }

  return false;
}

void Core::checkUserRecord( const UserRecord& ur )
{
  if( ur.hostAddress() == Settings::instance().localUser().hostAddress() &&
      ur.hostPort() == Settings::instance().localUser().hostPort() )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Skip local user record" << ur.hostAddressAndPort();
#endif
    return;
  }

  newPeerFound( ur.hostAddress(), ur.hostPort() );
}

void Core::newPeerFound( const QHostAddress& sender_ip, int sender_port )
{
  if( hasConnection( sender_ip, sender_port ) )
    return;

  qDebug() << "Connecting to new peer" << sender_ip.toString() << sender_port;

  Connection *c = new Connection( this );
  setupNewConnection( c );
  c->connectToHost( sender_ip, sender_port );
}

void Core::checkNewConnection( Connection *c )
{
  // Has connection never return true because peer port is always different.
  // It comes from Listener. If I want to prevent multiple users from single
  // ip, I can pass -1 to peer_port and check only host address

  qDebug() << "New connection from" << c->peerAddress().toString() << c->peerPort();

  if( Settings::instance().preventMultipleConnectionsFromSingleHostAddress() )
  {
    if( hasConnection( c->peerAddress(), -1 ) )
    {
#ifdef BEEBEEP_DEBUG
      qDebug() << c->peerAddress().toString() << "is already connected and blocked by prevent multiple connections";
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
    qDebug() << "Connecting SIGNAL/SLOT to connection from" << c->peerAddress().toString() << c->peerPort();
#endif
  connect( c, SIGNAL( error( QAbstractSocket::SocketError ) ), this, SLOT( setConnectionError( QAbstractSocket::SocketError ) ) );
  connect( c, SIGNAL( disconnected() ), this, SLOT( setConnectionClosed() ) );
  connect( c, SIGNAL( abortRequest() ), this, SLOT( setConnectionClosed() ) );
  connect( c, SIGNAL( authenticationRequested( const Message& ) ), this, SLOT( checkUserAuthentication( const Message& ) ) );
}

void Core::addConnectionReadyForUse( Connection* c )
{
#ifdef BEEBEEP_DEBUG
  qDebug() << "Connection from" << c->peerAddress().toString() << c->peerPort() << "is ready for use";
#endif
  connect( c, SIGNAL( newMessage( VNumber, const Message& ) ), this, SLOT( parseMessage( VNumber, const Message& ) ) );
  m_connections.append( c );
}

void Core::setConnectionError( QAbstractSocket::SocketError se )
{
  Connection* c = qobject_cast<Connection*>( sender() );
  if( c )
  {
    if( c->userId() != ID_INVALID )
    {
      qWarning() << "Connection from" << c->peerAddress().toString() << c->peerPort() << "has an error:" << c->errorString() << "-" << (int)se;
    }
    else
    {
      if( !c->peerAddress().isNull() )
        qWarning() << "Connection from" << c->peerAddress().toString() << c->peerPort() << "has refused password:" << c->errorString() << "-" << (int)se;
    }
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
  int number_of_connection_pointers = m_connections.removeAll( c );

#ifdef BEEBEEP_DEBUG
  if( !c->peerAddress().isNull() )
  {
    if( number_of_connection_pointers <= 0 )
      qDebug() << "Connection pointer is not present (or already removed from list)";
    else if( number_of_connection_pointers != 1 )
      qDebug() << number_of_connection_pointers << "pointers of a single connection found in connection list";
    else
      qDebug() << "Connection pointer removed from connection list";
  }
#else
  if( number_of_connection_pointers > 1 )
    qWarning() << number_of_connection_pointers << "similar connections found in list and removed";
#endif


  if( c->userId() != ID_INVALID )
  {
    User u = UserManager::instance().findUser( c->userId() );
    if( u.isValid() )
    {
      qDebug() << "User" << u.path() << "goes offline";
      u.setStatus( User::Offline );
      UserManager::instance().setUser( u );
      showUserStatusChanged( u );

      userConnectionStatusChanged( u );

      Chat default_chat = ChatManager::instance().defaultChat();
      if( default_chat.removeUser( u.id() ) )
        ChatManager::instance().setChat( default_chat );

      FileShare::instance().removeFromNetwork( c->userId() );
      if( u.isValid() )
        emit fileShareAvailable( u );

      QString sHtmlMsg = QString( "%1 " ).arg( Bee::iconToHtml( ":/images/network-disconnected.png", "*C*" ) );
      sHtmlMsg += tr( "%1 (%2) is disconnected from %3 network." ).arg( u.name(), u.accountPath(), Settings::instance().programName() );
      dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), sHtmlMsg, DispatchToChat, ChatMessage::UserInfo );

    }
    else
      qWarning() << "User" << c->userId() << "not found while closing connection";
  }

  c->disconnect();
  c->closeConnection();
  //do not delete the object. Can cause crash. See c->readblock( ... )
  //c->deleteLater();
}

void Core::checkUserAuthentication( const Message& m )
{
  Connection* c = qobject_cast<Connection*>( sender() );
  if( !c )
  {
    qWarning() << "Unable to check authentication for an invalid connection object";
    return;
  }

  QString sHtmlMsg;
  User u = Protocol::instance().createUser( m, c->peerAddress() );
  if( !u.isValid() )
  {
    qWarning() << "Unable to create a new user (invalid protocol or password) from the message arrived from:" << c->peerAddress().toString() << c->peerPort();
    closeConnection( c );
    return;
  }
  else
    qDebug() << u.path() << "has completed the authentication";

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
      qDebug() << "User with account" << u.accountName() << "and path" << u.path() << "is recognized to be Local";
      closeConnection( c );
      return;
    }

    if( isUserConnected( user_found.id() ) )
    {
      qDebug() << "User with account" << u.accountName() << "and path" << u.path() << "is already connected with account name" << user_found.accountName() << "path" << user_found.path();
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

  if( u.color() == QString( "#000000" ) || !QColor::isValidColor( u.color() ) )
  {
    if( user_found.isValid() && user_found.color() != QString( "#000000" ) )
      u.setColor( user_found.color() );
    else
      u.setColor( ColorManager::instance().unselectedQString() );
  }

  UserManager::instance().setUser( u );
  qDebug() << "User" << u.path() << "added with id" << u.id() << "and color" << u.color();

  Chat default_chat = ChatManager::instance().defaultChat();
  if( default_chat.addUser( u.id() ) )
  {
    qDebug() << "Adding user" << u.path() << "to default chat";
    ChatManager::instance().setChat( default_chat );
  }

  if( !ChatManager::instance().privateChatForUser( u.id() ).isValid() )
    createPrivateChat( u );

  c->setReadyForUse( u.id() );
  addConnectionReadyForUse( c );

  if( user_path_changed )
    ChatManager::instance().changePrivateChatNameAfterUserNameChanged( user_found.id(), u.path() );

  sHtmlMsg = QString( "%1 " ).arg( Bee::iconToHtml( ":/images/network-connected.png", "*C*" ) );
  sHtmlMsg += tr( "%1 (%2) is connected to %3 network." ).arg( u.name(), u.accountPath(), Settings::instance().programName() );
  dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), sHtmlMsg, DispatchToChat, ChatMessage::UserInfo );

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

  checkUserHostAddress( u );
  checkOfflineMessagesForUser( u );
}
