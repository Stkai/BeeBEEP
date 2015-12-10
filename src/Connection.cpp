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

#include "Connection.h"
#include "Protocol.h"
#include "Settings.h"

#undef CONNECTION_PING_PONG_DEBUG


Connection::Connection( QObject *parent )
  : ConnectionSocket( parent )
{
  setSocketOption( QAbstractSocket::KeepAliveOption, 1 );
  connect( this, SIGNAL( dataReceived( const QByteArray& ) ), this, SLOT( parseData( const QByteArray& ) ) );
  connect( this, SIGNAL( tickEvent( int ) ), this, SLOT( onTickEvent( int ) ) );
}

bool Connection::sendMessage( const Message& m )
{
  QByteArray message_data = Protocol::instance().fromMessage( m );
  if( message_data.isEmpty() )
  {
    qWarning() << "Connection has received an invalid message to send";
    return false;
  }

  if( message_data.size() > 524288 )
    qWarning() << "Outgoing message to" << qPrintable( hostAndPort() ) << "is VERY VERY BIG:" << message_data.size() << "bytes";

  return sendData( message_data );
}

void Connection::parseData( const QByteArray& message_data )
{
  if( message_data.size() > 524288 )
    qWarning() << "Incoming message from" << qPrintable( hostAndPort() ) << "is VERY VERY BIG:" << message_data.size() << "bytes";

  Message m = Protocol::instance().toMessage( message_data );
  if( !m.isValid() )
  {
    qWarning() << "Connection from" << qPrintable( hostAndPort() ) << "has received an invalid message data";
    return;
  }

  switch( m.type() )
  {
  case Message::Ping:
#if defined( CONNECTION_PING_PONG_DEBUG )
    qDebug() << "PING received from" << qPrintable( hostAndPort() );
#endif
    sendPong();
    break;
  case Message::Pong:
#if defined( CONNECTION_PING_PONG_DEBUG )
    qDebug() << "PONG received from" << qPrintable( hostAndPort() );
#endif
    break;
  default:
    emit newMessage( userId(), m );
    break;
  }
}

void Connection::setReadyForUse( VNumber user_id )
{
  qDebug() << "Connection is ready for use by the user" << user_id;
  setUserId( user_id );
}

void Connection::sendPing()
{
  int activity_idle = activityIdle();

#if defined( CONNECTION_PING_PONG_DEBUG )
  qDebug() << "Connection" << qPrintable( hostAndPort() ) << "idle=" << activity_idle << "timeout=" << Settings::instance().pongTimeout();
#endif

  if( activity_idle > Settings::instance().pongTimeout() )
  {
    qDebug() << "Connection timeout with" << activity_idle << "ms idle from"  << qPrintable( hostAndPort() );
    emit abortRequest();
    return;
  }

  // -200 takes care of time from here to sendData
  if( activity_idle < (Settings::instance().pingInterval()-200) )
  {
#if defined( CONNECTION_PING_PONG_DEBUG )
    qDebug() << "Ping is not sent because connection" << qPrintable( hostAndPort() ) << "has activity idle too short:" << activity_idle << "<" << Settings::instance().pingInterval();
#endif
    return;
  }

#if defined( CONNECTION_PING_PONG_DEBUG )
  qDebug() << "Sending PING to" << qPrintable( hostAndPort() );
#endif
  if( !sendData( Protocol::instance().pingMessage() ) )
  {
    qWarning() << "Unable to send PING to" << qPrintable( hostAndPort() );
    emit abortRequest();
  }
}

void Connection::sendPong()
{
#if defined( CONNECTION_PING_PONG_DEBUG )
  qDebug() << "Sending PONG to" << qPrintable( hostAndPort() );
#endif
  if( !sendData( Protocol::instance().pongMessage() ) )
    qWarning() << "Unable to send PONG to" << qPrintable( hostAndPort() );
}

void Connection::closeConnection()
{
  if( !peerAddress().isNull() )
    qDebug() << "Connection to" << qPrintable( hostAndPort() ) << "closed";

  abortConnection();
}

void Connection::onTickEvent( int ticks )
{
  if( ticks % 2 == 0 )
    sendPing();
}
