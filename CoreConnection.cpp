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
  if( !hasConnection( sender_ip, -1 ) ) // Check it: before the sender port is not checked and it was passed -1
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
    qDebug() << "Connection from" << c->peerAddress().toString() << c->peerPort() << "has an error:" << c->errorString() << "-" << (int)se;
    closeConnection( c );
  }
  else
    qWarning() << "Connection error" << se << "occurred but the object caller is invalid";
}

void Core::setConnectionClosed()
{
  ConnectionSocket* c = qobject_cast<ConnectionSocket*>( sender() );
  if( c )
  {
    qDebug() << "Setting connection from" << c->peerAddress().toString() << c->peerPort() << "closed";
    closeConnection( (Connection*)c );
  }
  else
    qWarning() << "Connection closed but the object caller is invalid";
}

void Core::closeConnection( Connection *c )
{
  qDebug() << "Removing connection pointer from connection list";
  int number_of_connection_pointers = m_connections.removeAll( c );
  if( number_of_connection_pointers <= 0 )
  {
    qDebug() << "Connection pointer is not present";
    return;
  }

  if( number_of_connection_pointers != 1 )
    qWarning() << number_of_connection_pointers << "pointers of a single connection found in connection list";

  qDebug() << "Closing connection for user" << c->userId();
  User u = m_users.find( c->userId() );
  if( u.isValid() )
  {
    qDebug() << "User" << u.path() << "goes offline";
    u.setStatus( User::Offline );
    m_users.setUser( u );
    QString sHtmlMsg = tr( "%1 %2 has left." ).arg( Bee::iconToHtml( ":/images/red-ball.png", "*X*" ), u.path() );
    dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), sHtmlMsg, DispatchToAllChatsWithUser );
    emit userChanged( u );
  }
  else
    qWarning() << "User" << c->userId() << "not found while closing connection";

  Chat default_chat = defaultChat( false );
  if( default_chat.removeUser( u.id() ) )
    setChat( default_chat );

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

  User u = Protocol::instance().createUser( m, c->peerAddress() );
  if( !u.isValid() )
  {
    qWarning() << "Unable to create a new user from the message arrived from:" << c->peerAddress().toString() << c->peerPort();
    c->abort();
    c->deleteLater();
    return;
  }

  User user_found = m_users.find( u.path() );
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

  m_users.setUser( u );
  emit userChanged( u );

  QString sHtmlMsg = tr( "%1 %2 has %3." ).arg( Bee::iconToHtml( ":/images/green-ball.png", "*U*" ),
                                                u.path(), (user_found.isValid() ? tr( "reconnected" ) : tr( "joined")) );
  dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), sHtmlMsg, DispatchToAllChatsWithUser );
}
