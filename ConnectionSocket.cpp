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


ConnectionSocket::ConnectionSocket( QObject* parent )
  : QTcpSocket( parent ), m_blockSize( 0 ), m_isHelloSent( false ), m_userId( ID_INVALID )
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
    qDebug() << "ConnectionSocket read from" << peerAddress().toString() << peerPort() << "the block size:" << m_blockSize;
  }

  if( bytesAvailable() < m_blockSize )
    return;

  QByteArray byte_array_read;
  data_stream >> byte_array_read;

  m_blockSize = 0;

  QByteArray decrypted_byte_array = Protocol::instance().decryptByteArray( byte_array_read );

  qDebug() << "ConnectionSocket read from" << peerAddress().toString() << peerPort() << "the byte array:" << decrypted_byte_array;

  if( m_userId == ID_INVALID )
    checkHelloMessage( decrypted_byte_array );
  else
    emit dataReceived( decrypted_byte_array );
}

bool ConnectionSocket::sendData( const QByteArray& byte_array )
{
  qDebug() << "ConnectionSocket sends to" << peerAddress().toString() << peerPort() << "the following data:" << byte_array;

  QByteArray byte_array_to_send = Protocol::instance().encryptByteArray( byte_array );

  QByteArray data_block;
  QDataStream data_stream( &data_block, QIODevice::WriteOnly );
  data_stream.setVersion( DATASTREAM_VERSION );
  data_stream << (DATA_BLOCK_SIZE)0;
  data_stream << byte_array_to_send;
  data_stream.device()->seek( 0 );
  data_stream << (DATA_BLOCK_SIZE)(data_block.size() - sizeof(DATA_BLOCK_SIZE));
  qDebug() << "ConnectionSocket sends to" << peerAddress().toString() << peerPort() << "the block size:" << (DATA_BLOCK_SIZE)(data_block.size() - sizeof(DATA_BLOCK_SIZE));

  if( write( data_block ) == data_block.size() )
  {
    qDebug() << "ConnectionSocket sends" << data_block.size() << "bytes to" << peerAddress().toString() << peerPort();
    return true;
  }
  else
  {
    qDebug() << "ConnectionSocket has an I/O error";
    return false;
  }
}

void ConnectionSocket::sendHello()
{
  qDebug() << "ConnectionSocket is sending HELLO to" << peerAddress().toString() << peerPort();

  if( sendData( Protocol::instance().helloMessage() ) )
  {
    qDebug() << "ConnectionSocket has sent HELLO to" << peerAddress().toString() << peerPort();
    m_isHelloSent = true;
  }
  else
    qWarning() << "ConnectionSocket is unable to send HELLO to" << peerAddress().toString() << peerPort();
}

void ConnectionSocket::checkHelloMessage( const QByteArray& array_data )
{
  Message m = Protocol::instance().toMessage( array_data );
  if( !m.isValid() )
  {
    qWarning() << "ConnectionSocket has received an invalid HELLO from:" << peerAddress().toString() << peerPort();
    abort();
    return;
  }

  if( m.type() != Message::Hello )
  {
    qWarning() << "ConnectionSocket is waiting for HELLO, but another message type" << m.type() << "is arrived from:" << peerAddress().toString() << peerPort();
    abort();
    return;
  }

  if( !m_isHelloSent )
    sendHello();

  qDebug() << "ConnectionSocket request an authentication";
  emit authenticationRequested( m );
}
