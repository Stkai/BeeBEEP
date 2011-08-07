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
  : QTcpSocket( parent ), m_blockSize( 0 )
{
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
  if( Settings::instance().useEncryption() )
  {
    QByteArray decrypted_byte_array = Protocol::instance().decryptByteArray( byte_array_read );
#if defined( BEEBEEP_DEBUG )
    qDebug() << "Data decrypted:" << decrypted_byte_array;
#endif
    emit dataReceived( decrypted_byte_array );
  }
  else
    emit dataReceived( byte_array_read );
}

bool ConnectionSocket::sendData( const QByteArray& byte_array )
{
#if defined( BEEBEEP_DEBUG )
  qDebug() << "Socket send a byte array:" << byte_array;
#endif
  QByteArray byte_array_to_send;
  if( Settings::instance().useEncryption() )
  {
    byte_array_to_send = Protocol::instance().encryptByteArray( byte_array );
#if defined( BEEBEEP_DEBUG )
  qDebug() << "Encrypt data:" << byte_array_to_send;
#endif
  }
  else
    byte_array_to_send = byte_array;

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

