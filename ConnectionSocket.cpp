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

#include "ConnectionSocket.h"
#include "Protocol.h"
#include "Settings.h"


ConnectionSocket::ConnectionSocket( QObject* parent )
  : QTcpSocket( parent ), m_user(), m_blockSize( 0 ), m_isHelloSent( false )
{
  connect( this, SIGNAL( connected() ), this, SLOT( sendHello() ) );
  connect( this, SIGNAL( readyRead() ), this, SLOT( readBlock() ) );
}

void ConnectionSocket::readBlock()
{
  QDataStream data_stream( this );
  data_stream.setVersion( DATASTREAM_VERSION );
  if( m_blockSize == 0 )
  {
    if( bytesAvailable() < (int)sizeof(DATA_BLOCK_SIZE))
      return;
    data_stream >> m_blockSize;
#if defined( BEEBEEP_DEBUG )
    qDebug() << "Socket read the block size:" << m_blockSize;
#endif
  }

  if( bytesAvailable() < m_blockSize )
    return;

  QByteArray byte_array_read;
  data_stream >> byte_array_read;

#if defined( BEEBEEP_DEBUG )
  qDebug() << "Socket read a byte array:" << byte_array_read;
#endif

  m_blockSize = 0;

  QByteArray decrypted_byte_array = Protocol::instance().decryptByteArray( byte_array_read );

  if( !m_user.isValid() )
    checkHello( decrypted_byte_array );
  else
    emit dataReceived( decrypted_byte_array );
}

bool ConnectionSocket::sendData( const QByteArray& byte_array )
{
#if defined( BEEBEEP_DEBUG )
  qDebug() << "Socket send a byte array:" << byte_array;
#endif
  QByteArray byte_array_to_send = Protocol::instance().encryptByteArray( byte_array );
#if defined( BEEBEEP_DEBUG )
  qDebug() << "Encrypt data:" << byte_array_to_send;
#endif

  QByteArray data_block;
  QDataStream data_stream( &data_block, QIODevice::WriteOnly );
  data_stream.setVersion( DATASTREAM_VERSION );
  data_stream << (DATA_BLOCK_SIZE)0;
  data_stream << byte_array_to_send;
  data_stream.device()->seek( 0 );
  data_stream << (DATA_BLOCK_SIZE)(data_block.size() - sizeof(DATA_BLOCK_SIZE));

#if defined( BEEBEEP_DEBUG )
  qDebug() << "Send block size:" << (DATA_BLOCK_SIZE)(data_block.size() - sizeof(DATA_BLOCK_SIZE));
  qDebug() << "Send data:" << data_block;
  if( write( data_block ) == data_block.size() )
  {
    qDebug() << "Socket send" << data_block.size() << "bytes";
    return true;
  }
  else
  {
    qDebug() << "ConnectionSocket I/O error";
    return false;
  }
#else
  return( write( data_block ) == data_block.size() );
#endif
}

void ConnectionSocket::sendHello()
{
#if defined( BEEBEEP_DEBUG )
  qDebug() << "Sending HELLO to" << peerAddress().toString();
#endif
  if( sendData( Protocol::instance().helloMessage() ) )
  {
#if defined( BEEBEEP_DEBUG )
    qDebug() << "HELLO sent to" << peerAddress().toString();
#endif
    m_isHelloSent = true;
  }
  else
    qWarning() << "Unable to send HELLO to" <<  peerAddress().toString();
}

void ConnectionSocket::checkHello( const QByteArray& array_data )
{
  Message m = Protocol::instance().toMessage( array_data );
  if( !m.isValid() )
  {
    qWarning() << "Invalid hello message arrived from connection:" << peerAddress().toString();
    abort();
    return;
  }

  if( m.type() != Message::Hello )
  {
    qWarning() << "Hello message not arrived from connection:" << peerAddress().toString();
    abort();
    return;
  }

  m_user = Protocol::instance().createUser( m, peerAddress() );
  if( !m_user.isValid() )
  {
    qWarning() << "Invalid user from connection:" << peerAddress().toString();
    abort();
    return;
  }
#if defined( BEEBEEP_DEBUG )
  qDebug() << "New user:" << m_user.path();
#endif
  m_user.setStatus( User::Online );

  if( !m_isHelloSent )
    sendHello();

  emit userAuthenticated();
}
