//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2019 Marco Mastroddi
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
// along with BeeBEEP. If not, see <http://www.gnu.org/licenses/>.
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
  connect( this, SIGNAL( pingRequest() ), this, SLOT( sendPing() ) );
}

bool Connection::sendMessage( const Message& m )
{
  QByteArray message_data = Protocol::instance().fromMessage( m, protocolVersion() );
  if( message_data.size() > 524288 )
    qWarning() << "Outgoing message to" << qPrintable( networkAddress().toString() ) << "is VERY VERY BIG:" << message_data.size() << "bytes";

#ifdef BEEBEEP_DEBUG
  if( !isReadyForUse() )
    qDebug() << qPrintable( networkAddress().toString() ) << "is sending this message:" << message_data;
#endif

  if( !sendData( message_data ) )
  {
    qWarning() << "Unable to send message type" << m.type() << "and size" << m.text().size() << "to user" << userId();
    return false;
  }
  else
    return true;
}

void Connection::parseData( const QByteArray& message_data )
{
  if( message_data.size() > 524288 )
    qWarning() << "Incoming message from" << qPrintable( networkAddress().toString() ) << "is VERY VERY BIG:" << message_data.size() << "bytes";

  Message m = Protocol::instance().toMessage( message_data, protocolVersion() );
  if( !m.isValid() )
  {
    qWarning() << "Connection from" << qPrintable( networkAddress().toString() ) << "has received an invalid message data";
    return;
  }

  switch( m.type() )
  {
  case Message::Ping:
#if defined( CONNECTION_PING_PONG_DEBUG )
    qDebug() << "PING received from" << qPrintable( networkAddress().toString() );
#endif
    sendPong();
    break;
  case Message::Pong:
#if defined( CONNECTION_PING_PONG_DEBUG )
    qDebug() << "PONG received from" << qPrintable( networkAddress().toString() );
#endif
    break;
  default:
    emit newMessage( userId(), m );
    break;
  }
}

void Connection::setReadyForUse( VNumber user_id )
{
  setUserId( user_id );
}

void Connection::sendPing()
{
  if( !isReadyForUse() )
    return;

  int activity_idle = activityIdle();

#if defined( CONNECTION_PING_PONG_DEBUG )
  qDebug() << "Connection" << qPrintable( networkAddress().toString() ) << "idle=" << activity_idle << "timeout=" << Settings::instance().pongTimeout();
#endif

  if( activity_idle > Settings::instance().pongTimeout() )
  {
    qDebug() << "Connection timeout with" << activity_idle << "ms idle from" << qPrintable( networkAddress().toString() );
    emit abortRequest();
    return;
  }

  if( activity_idle < PING_INTERVAL_IDLE )
  {
#if defined( CONNECTION_PING_PONG_DEBUG )
    qDebug() << "Ping is not sent because connection" << qPrintable( networkAddress().toString() ) << "has activity idle too short:" << activity_idle << "<" << PING_INTERVAL_IDLE;
#endif
    return;
  }

#if defined( CONNECTION_PING_PONG_DEBUG )
  qDebug() << "Sending PING to" << qPrintable( networkAddress().toString() );
#endif
  if( !sendData( Protocol::instance().pingMessage() ) )
  {
    qWarning() << "Unable to send PING to" << qPrintable( networkAddress().toString() );
    emit abortRequest();
  }
}

void Connection::sendPong()
{
#if defined( CONNECTION_PING_PONG_DEBUG )
  qDebug() << "Sending PONG to" << qPrintable( networkAddress().toString() );
#endif
  if( !sendData( Protocol::instance().pongMessage() ) )
    qWarning() << "Unable to send PONG to" << qPrintable( networkAddress().toString() );
}
