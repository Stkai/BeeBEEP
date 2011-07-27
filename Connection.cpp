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
#include "Protocol.h"
#include "Settings.h"


Connection::Connection( QObject *parent )
  : QTcpSocket( parent ), m_user(), m_state( WaitingForHello ), m_isHelloMessageSent( false )
{
  m_pingTimer.setInterval( PING_INTERVAL );
  connect( this, SIGNAL( readyRead() ), this, SLOT( readData() ) );
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
  QString message_data = Protocol::instance().fromMessage( m );
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

  if( writeMessageData( message_data ) )
  {
#if defined( BEEBEEP_DEBUG )
    qDebug() << "Data sent";
#endif
    return true;
  }
  else
  {
#if defined( BEEBEEP_DEBUG )
    qDebug() << "Unable to send data";
#endif
    return false;
  }
}

void Connection::readData()
{
  if( !canReadLine() )
    return;
#if defined( BEEBEEP_DEBUG )
  qDebug() << "New data available";
#endif
  QString line = "";
  if( Settings::instance().useEncryption() )
  {
    QTextStream ts( this );
    ts.setCodec( "UTF-16");
    QString encrypted_data = ts.readLine( MAX_BUFFER_SIZE );
#if defined( BEEBEEP_DEBUG )
  qDebug() << "Decrypt data:" << encrypted_data;
#endif
    encrypted_data.replace( "-NEWLINE-", "\n" );
    line = Protocol::instance().decrypt( encrypted_data );
  }
  else
  {
    line = QString::fromUtf8( readLine( MAX_BUFFER_SIZE ) );
    line.replace( "-NEWLINE-", "\n" );
    line.chop( 1 ); // remove last '\n'
  }

  if( line.size() <= Protocol::instance().messageMinimumSize() )
  {
    qWarning() << "Invalid message data size:" << line;
    return;
  }

#if defined( BEEBEEP_DEBUG )
  qDebug() << "Message data:" << line;
#endif
  Message m = Protocol::instance().toMessage( line );
  if( !m.isValid() )
  {
    qWarning() << "Skip message cause error occcurred:" << line;
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
    qDebug() << "User" << m_user.name() << " is writing";
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

void Connection::sendPing()
{
  if( m_pongTime.elapsed() > PONG_TIMEOUT )
  {
    abort();
    return;
  }
  writeMessageData( Protocol::instance().pingMessage() );
}

void Connection::sendPong()
{
  writeMessageData( Protocol::instance().pongMessage() );
}

void Connection::sendHello()
{
#if defined( BEEBEEP_DEBUG )
  qDebug() << "Sending Hello to" << m_user.name();
#endif
  if( writeMessageData( Protocol::instance().helloMessage() ) )
  {
#if defined( BEEBEEP_DEBUG )
    qDebug() << "Hello sent to" << m_user.name();
#endif
    m_isHelloMessageSent = true;
  }
  else
    qWarning() << "Unable to send Hello to" << m_user.name();
}

bool Connection::sendLocalUserStatus()
{
#if defined( BEEBEEP_DEBUG )
    qDebug() << "Sending status to" << m_user.name();
#endif
  return sendMessage( Protocol::instance().userStatusToMessage( Settings::instance().localUser() ) );
}

bool Connection::writeMessageData( const QString& message_data )
{
#if defined( BEEBEEP_DEBUG )
  qDebug() << "Writing on socket:" << message_data;
#endif
  if( Settings::instance().useEncryption() )
  {
    QTextStream ts( this );
    ts.setCodec( "UTF-16");

#if defined( BEEBEEP_DEBUG )
    int num_spaces = 0;
    QString encrypted_data = Protocol::instance().encrypt( message_data, &num_spaces );
    QString tmp = Protocol::instance().decrypt( encrypted_data );
    tmp.chop( num_spaces );
    if( tmp != message_data )
    {
      qWarning() << "Error occurred in writeMessageData - encrypt";
      qWarning() << "Data:" << message_data;
      qWarning() << "Encrypted:" << encrypted_data;
      qWarning() << "Decrypted:" << tmp;
      return false;
    }
#else
    QString encrypted_data = Protocol::instance().encrypt( message_data );
#endif
    encrypted_data.replace( "\n", "-NEWLINE-" );
    ts << encrypted_data;
    ts << "\n";
    ts.flush();
#if defined( BEEBEEP_DEBUG )
    qDebug() << "Encrypted data wrote on socket";
#endif
    return true;
  }
  else
  {
    QByteArray byte_array = message_data.toUtf8();
    byte_array.replace( "\n", "-NEWLINE-" );
    byte_array += '\n';
    if( write( byte_array ) == byte_array.size() )
    {
#if defined( BEEBEEP_DEBUG )
      qDebug() << "Data wrote on socket";
#endif
      return true;
    }
    else
    {
#if defined( BEEBEEP_DEBUG )
      qDebug() << "Unable to write data on socket";
#endif
      return false;
    }
  }
}
