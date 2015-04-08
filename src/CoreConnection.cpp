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
#include "ChatManager.h"
#include "ColorManager.h"
#include "Connection.h"
#include "Core.h"
#include "FileShare.h"
#include "Protocol.h"
#include "Settings.h"
#include "UserManager.h"


Connection* Core::connection( VNumber user_id )
{
  QList<Connection*>::iterator it = m_connections.begin();
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
      return true;
    ++it;
  }
  return false;
}

void Core::newPeerFound( const QHostAddress& sender_ip, int sender_port )
{
  if( hasConnection( sender_ip, sender_port ) )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Connection from" << sender_ip.toString() << sender_ip << "is already open";
#endif
    return;
  }

  Connection *c = new Connection( this );
  setNewConnection( c );
  c->connectToHost( sender_ip, sender_port );
}

void Core::setNewConnection( Connection *c )
{
#ifdef BEEBEEP_DEBUG
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
    qWarning() << "Connection from" << c->peerAddress().toString() << c->peerPort() << "has an error:" << c->errorString() << "-" << (int)se;
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
  if( number_of_connection_pointers <= 0 )
    qDebug() << "Connection pointer is not present (or already removed from list)";
  else if( number_of_connection_pointers != 1 )
    qDebug() << number_of_connection_pointers << "pointers of a single connection found in connection list";
  else
    qDebug() << "Connection pointer removed from connection list";

  User u = UserManager::instance().userList().find( c->userId() );
  if( u.isValid() )
  {
    qDebug() << "User" << u.path() << "goes offline";
    u.setStatus( User::Offline );
    UserManager::instance().setUser( u );
    showUserStatusChanged( u );

    Chat default_chat = ChatManager::instance().defaultChat();
    if( default_chat.removeUser( u.id() ) )
      ChatManager::instance().setChat( default_chat );

    FileShare::instance().removeFromNetwork( c->userId() );
    if( u.isValid() )
      emit fileShareAvailable( u );
  }
  else
    qWarning() << "User" << c->userId() << "not found while closing connection";

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
    qDebug() << u.path() << ": authentication completed";

  User user_found;

  if( !u.sessionId().isEmpty() )
  {
    user_found = UserManager::instance().findUserBySessionId( u.sessionId() );
    if( user_found.isValid() && user_found.isConnected() )
    {
      qWarning() << u.path() << "is already connected with path" << user_found.path();
      closeConnection( c );
      return;
    }
  }

  bool user_reconnect = false;
  bool user_path_changed = false;

  if( !user_found.isValid() )
    user_found = UserManager::instance().findUserByPath( u.path() );
  if( !user_found.isValid() && Settings::instance().trustSystemAccount() )
    user_found = UserManager::instance().findUserByAccountName( u.accountName() );

  if( user_found.isValid() )
  {
    if( u.path() != user_found.path() )
    {
      user_path_changed = true;
      qDebug() << "On connection old user found" << user_found.path() << "and associated to" << u.path();
    }
    else
      qDebug() << "User" << u.path() << "reconnected";
    u.setId( user_found.id() );
    u.setColor( user_found.color() );
    user_reconnect = true;
  }
  else
  {
    u.setColor( ColorManager::instance().unselectedQString() );
    qDebug() << "New user connected:" << u.path() << "with color" << u.color();
  }

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

  UserManager::instance().setUser( u );
  qDebug() << "User" << u.path() << "added with id" << u.id();
  if( user_path_changed )
    ChatManager::instance().changePrivateChatNameAfterUserNameChanged( user_found.id(), u.path() );

  sHtmlMsg = QString( "%1 " ).arg( Bee::iconToHtml( ":/images/network-connected.png", "*C*" ) );
  sHtmlMsg += tr( "%1 (%2) is connected to %3 network." ).arg( u.name(), u.accountPath(), Settings::instance().programName() );
  dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), sHtmlMsg, DispatchToAllChatsWithUser );

  if( !u.statusDescription().isEmpty() || u.status() > User::Online )
    showUserStatusChanged( u );
  else
    emit userChanged( u );

  if( !Settings::instance().localUser().vCard().hasOnlyNickName() )
  {
    if( c->protoVersion() > 1 )
    {
      qDebug() << "Sending my VCard to" << u.path();
      c->sendData( Protocol::instance().localVCardMessage() );
    }
  }

  if( user_reconnect )
    checkGroupChatAfterUserReconnect( u );

}
