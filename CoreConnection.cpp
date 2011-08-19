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
// Author: Marco Mastroddi (marco.mastroddi(AT)gmail.com)
//
// $Id$
//
//////////////////////////////////////////////////////////////////////

#include "BeeUtils.h"
#include "ColorManager.h"
#include "Connection.h"
#include "Core.h"
#include "Protocol.h"
#include "Settings.h"


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
  if( !hasConnection( sender_ip, sender_port ) ) // Check it: before the sender port is not checked and it was passed -1
  {
    Connection *c = new Connection( this );
    setNewConnection( c );
    c->connectToHost( sender_ip, sender_port );
  }
}

void Core::setNewConnection( Connection *c )
{
  qDebug() << "Connecting SIGNAL/SLOT to connection from" << c->peerAddress().toString() << c->peerPort();
  connect( c, SIGNAL( error( QAbstractSocket::SocketError ) ), this, SLOT( setConnectionError( QAbstractSocket::SocketError ) ) );
  connect( c, SIGNAL( disconnected() ), this, SLOT( setConnectionClosed() ) );
  connect( c, SIGNAL( authenticationRequested( const Message& ) ), this, SLOT( checkUserAuthentication( const Message& ) ) );
}

void Core::setConnectionReadyForUse( Connection* c )
{
  if( hasConnection( c->peerAddress(), c->peerPort() ) )
  {
    qWarning() << "Connection from" << c->peerAddress().toString() << c->peerPort() << "is, already, ready for use";
    return;
  }
  qDebug() << "Connection from" << c->peerAddress().toString() << c->peerPort() << "is ready for use";
  connect( c, SIGNAL( newMessage( VNumber, const Message& ) ), this, SLOT( parseMessage( VNumber, const Message& ) ) );
  m_connections.append( c );
}

void Core::setConnectionError( QAbstractSocket::SocketError se )
{
  Connection* c = qobject_cast<Connection*>( sender() );
  if( c )
  {
    qDebug() << "Connection from" << c->peerAddress() << c->peerPort() << "has an error:" << c->errorString();
    closeConnection( c );
  }
  else
    qWarning() << "Connection error" << se << "occurred but the object caller is invalid";
}

void Core::setConnectionClosed()
{
  Connection* c = qobject_cast<Connection*>( sender() );
  if( c )
  {
    qDebug() << "Connection from" << c->peerAddress() << c->peerPort() << "is closed";
    closeConnection( c );
  }
  else
    qWarning() << "Connection closed but the object caller is invalid";
}

void Core::closeConnection( Connection *c )
{
  User u = user( c->userId() );
  if( u.isValid() )
  {
    u.setStatus( User::Offline );
    setUser( u );
    emit userChanged( u );
    QString sHtmlMsg = tr( "%1 %2 has left." ).arg( Bee::iconToHtml( ":/images/red-ball.png", "*X*" ), u.path() );
    dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), sHtmlMsg, DispatchToAllChatsWithUser );
  }
  else
    qWarning() << "User" << c->userId() << "not found while closing connection";

  Chat default_chat = defaultChat( false );
  if( default_chat.removeUser( u.id() ) )
    setChat( default_chat );

  if( m_connections.removeAll( c ) != 1 )
    qWarning() << "Various pointers of a single connection found in connection list";

  c->deleteLater();
}

void Core::checkUserAuthentication( const Message& m )
{
  Connection* c = qobject_cast<Connection*>( sender() );
  if( !c )
  {
    qWarning() << "Unable to check authentication for an invalid connection object";
    return;
  }

  User u = Protocol::instance().createUser( m, c->peerAddress(), c->peerPort() );
  if( !u.isValid() )
  {
    qWarning() << "Unable to create a new user from the message arrived from:" << c->peerAddress().toString() << c->peerPort();
    c->abort();
    c->deleteLater();
    return;
  }

  User user_found = user( u.path() );
  if( user_found.isValid() )
  {
    u.setId( user_found.id() );
    u.setColor( user_found.color() );
    qDebug() << "User" << u.path() << "reconnected";
  }
  else
  {
    u.setColor( ColorManager::instance().unselectedQString() );
    qDebug() << "New user connected:" << u.path();
    createPrivateChat( u );
  }

  qDebug() << "Adding user" << u.path() << "to default chat";
  Chat default_chat = defaultChat( false );
  if( default_chat.addUser( u.id() ) )
    setChat( default_chat );

  c->setUserAuthenticated( true );
  c->setReadyForUse( u.id() );
  setConnectionReadyForUse( c );

  setUser( u );
  emit userChanged( u );

  QString sHtmlMsg = tr( "%1 %2 has joined." ).arg( Bee::iconToHtml( ":/images/green-ball.png", "*U*" ), Settings::instance().showUserNickname() ? u.nickname() : u.name() );
  dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), sHtmlMsg, DispatchToAllChatsWithUser );
}
