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

#include "Connection.h"
#include "FileInfo.h"
#include "Protocol.h"
#include "Settings.h"


Connection::Connection( QObject *parent )
  : ConnectionSocket( parent ), m_user(), m_state( WaitingForHello ), m_isHelloMessageSent( false )
{
  m_pingTimer.setInterval( PING_INTERVAL );
  connect( this, SIGNAL( dataReceived( const QByteArray& ) ), this, SLOT( checkData( const QByteArray& ) ) );
  connect( this, SIGNAL( disconnected() ), &m_pingTimer, SLOT( stop() ) );
  connect( &m_pingTimer, SIGNAL( timeout() ), this, SLOT( sendPing() ) );
  connect( this, SIGNAL( connected() ), this, SLOT( sendHello() ) );
}

bool Connection::sendMessage( const Message& m )
{
  if( !m.isValid() )
  {
#if defined( BEEBEEP_DEBUG )
    qDebug() << "Invalid message";
#endif
    return false;
  }
  QByteArray message_data = Protocol::instance().fromMessage( m );
  if( message_data.isEmpty() )
  {
#if defined( BEEBEEP_DEBUG )
    qDebug() << "Empty message data";
#endif
    return false;
  }
#if defined( BEEBEEP_DEBUG )
  qDebug() << "Sending:" << message_data;
#endif

  if( writeData( message_data ) )
  {
#if defined( BEEBEEP_DEBUG )
    qDebug() << "Message sent";
#endif
    return true;
  }
  else
  {
#if defined( BEEBEEP_DEBUG )
    qDebug() << "Unable to send message";
#endif
    return false;
  }
}

void Connection::checkData( const QByteArray& message_data )
{
#if defined( BEEBEEP_DEBUG )
  qDebug() << "Message data:" << message_data;
#endif
  Message m = Protocol::instance().toMessage( message_data );
  if( !m.isValid() )
  {
    qWarning() << "Skip message cause error occcurred:" << message_data;
    return;
  }

  if( m_state == WaitingForHello )
    parseHelloMessage( m );
  else
    parseMessage( m );

}

void Connection::parseMessage( const Message& m )
{
#if defined( BEEBEEP_DEBUG )
   qDebug() << "Parsing message";
#endif
  switch( m.type() )
  {
  case Message::User:
#if defined( BEEBEEP_DEBUG )
    qDebug() << "New status message for" << m_user.name();
#endif
    parseUserMessage( m );
    break;
  case Message::Chat:
#if defined( BEEBEEP_DEBUG )
    qDebug() << "New chat message:" << m.text();
#endif
    emit newMessage( m_user, m );
    break;
  case Message::Ping:
#if defined( BEEBEEP_DEBUG )
    qDebug() << "Ping arrived";
#endif
    sendPong();
#if defined( BEEBEEP_DEBUG )
    qDebug() << "Pong sent";
#endif
    break;
  case Message::Pong:
#if defined( BEEBEEP_DEBUG )
    qDebug() << "Pong arrived";
#endif
    m_pongTime.restart();
    break;
 case Message::File:
#if defined( BEEBEEP_DEBUG )
    qDebug() << "New file message:" << m.text() << m.data();
#endif
    parseFileMessage( m );
    break;

  default:
    qWarning() << "Invalid message type (in Connection):" << m.type();
    break;
  }
}

void Connection::parseHelloMessage( const Message& m )
{
  if( m.type() != Message::Hello )
  {
    qWarning() << "Hello message not arrived from connection:" << peerAddress().toString();
    abort();
    return;
  }

  m_user = Protocol::instance().createUser( m );
  if( !m_user.isValid() )
  {
    qWarning() << "Invalid user from connection:" << peerAddress().toString();
    abort();
    return;
  }

#if defined( BEEBEEP_DEBUG )
  qDebug() << "New user:" << m_user.name();
#endif

  m_user.setHostAddress( peerAddress() );
  m_state = ReadyForUse;

  if( !m_isHelloMessageSent )
    sendHello();

  m_pingTimer.start();
  m_pongTime.start();
  m_user.setStatus( User::Online );
  emit readyForUse();
}

void Connection::parseUserMessage( const Message& m )
{
  if( m.hasFlag( Message::Writing ) )
  {
#if defined( BEEBEEP_DEBUG )
    qDebug() << "User" << m_user.name() << "is writing";
#endif
    emit isWriting( m_user );
    return;
  }
  else if( m.hasFlag( Message::Status ) )
  {
#if defined( BEEBEEP_DEBUG )
    qDebug() << "New status message from user" << m_user.id() << ": [" << m.data() << "]:" << m.text();
#endif
    User u = Protocol::instance().userStatusFromMessage( m_user, m );
    if( u.isValid() )
    {
#if defined( BEEBEEP_DEBUG )
      qDebug() << "Set status for user" << u.name() << ": [" << u.status() << "]:" << u.statusDescription();
#endif
      m_user = u;
      emit newStatus( m_user );
    }
  }
  else
    qWarning() << "Invalid flag found in user message (in Connection)";
}

void Connection::parseFileMessage( const Message& m )
{
  FileInfo fi = Protocol::instance().fileInfoFromMessage( m );
  fi.setHostAddress( peerAddress() );
  if( fi.isValid() )
    emit newFileMessage( m_user, fi );
  else
    qWarning() << "Invalid FileInfo from user" << m_user.id() << ": [" << m.data() << "]:" << m.text();
}

void Connection::sendPing()
{
  if( m_pongTime.elapsed() > PONG_TIMEOUT )
  {
    abort();
    return;
  }
#if defined( BEEBEEP_DEBUG )
  qDebug() << "Sending PING to" << m_user.name();
#endif
  writeData( Protocol::instance().pingMessage() );
}

void Connection::sendPong()
{
#if defined( BEEBEEP_DEBUG )
  qDebug() << "Sending PONG to" << m_user.name();
#endif
  writeData( Protocol::instance().pongMessage() );
}

void Connection::sendHello()
{
#if defined( BEEBEEP_DEBUG )
  qDebug() << "Sending HELLO to" << m_user.name();
#endif
  if( writeData( Protocol::instance().helloMessage() ) )
  {
#if defined( BEEBEEP_DEBUG )
    qDebug() << "HELLO sent to" << m_user.name();
#endif
    m_isHelloMessageSent = true;
  }
  else
    qWarning() << "Unable to send HELLO to" << m_user.name();
}

bool Connection::sendLocalUserStatus()
{
#if defined( BEEBEEP_DEBUG )
    qDebug() << "Sending status to" << m_user.name();
#endif
  return sendMessage( Protocol::instance().userStatusToMessage( Settings::instance().localUser() ) );
}


bool Connection::writeData( const QByteArray& message_data )
{
  QByteArray message_data_filled = message_data;
  while( message_data_filled.size() % ENCRYPTED_DATA_BLOCK_SIZE )
    message_data_filled.append( ' ' );
  return sendData( message_data_filled );
}

