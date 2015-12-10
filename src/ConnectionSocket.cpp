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

#include "ConnectionSocket.h"
#include "Protocol.h"
#include "Settings.h"


#undef CONNECTION_SOCKET_IO_DEBUG
const int SECURE_LEVEL_2_PROTO_VERSION = 60;


ConnectionSocket::ConnectionSocket( QObject* parent )
  : QTcpSocket( parent ), m_blockSize( 0 ), m_isHelloSent( false ), m_userId( ID_INVALID ), m_protoVersion( 1 ),
    m_cipherKey( "" ), m_publicKey1( "" ), m_publicKey2( "" ), m_hostAndPort( "" ), m_latestActivityDateTime(),
    m_timerTickId( 0 ), m_tickCounter( 0 ), m_isAborted( false )
{
  connect( this, SIGNAL( connected() ), this, SLOT( sendQuestionHello() ) );
  connect( this, SIGNAL( readyRead() ), this, SLOT( readBlock() ) );
}

void ConnectionSocket::initSocket( qintptr socket_descriptor )
{
  m_isAborted = false;
  setSocketDescriptor( socket_descriptor );
  m_hostAndPort = QString( "%1:%2" ).arg( peerAddress().toString() ).arg( peerPort() );
  startTimerTick();
}

void ConnectionSocket::connectToNetworkAddress( const QHostAddress& host_address, int host_port )
{
  m_isAborted = false;
  m_hostAndPort = QString( "%1:%2" ).arg( host_address.toString() ).arg( host_port );
  if( !startTimerTick() )
    qWarning() << "Unable to start event timer for connection:" << m_hostAndPort;
  connectToHost( host_address, host_port );
}

bool ConnectionSocket::startTimerTick()
{
  stopTimerTick();
  m_timerTickId = startTimer( TICK_INTERVAL );
  return m_timerTickId != 0;
}

void ConnectionSocket::stopTimerTick()
{
  if( m_timerTickId != 0 )
  {
    killTimer( m_timerTickId );
    m_timerTickId = 0;
    m_tickCounter = 0;
  }
}

void ConnectionSocket::abortConnection()
{
  m_isAborted = true;
  stopTimerTick();
  abort();
}

void ConnectionSocket::closeConnection()
{
  m_isAborted = true;
  stopTimerTick();
  if( isOpen() )
  {
    flushAll();
    close();
  }
}

const QByteArray& ConnectionSocket::cipherKey() const
{
  return m_cipherKey.isEmpty() ? Settings::instance().password() : m_cipherKey;
}

bool ConnectionSocket::createCipherKey( const QString& public_key )
{
  if( m_publicKey1.isEmpty() )
  {
    m_publicKey1 = public_key;
  }
  else if( m_publicKey2.isEmpty() )
  {
    m_publicKey2 = public_key;
  }
  else
  {
    qWarning() << "Encryption handshake error. Too many public key arrived";
    return false;
  }

  if( m_publicKey1.isEmpty() || m_publicKey2.isEmpty() )
    return false;

  m_cipherKey = Protocol::instance().createCipherKey( m_publicKey1, m_publicKey2 );
  m_publicKey1 = "";
  m_publicKey2 = "";
  return true;
}

void ConnectionSocket::readBlock()
{
  m_latestActivityDateTime = QDateTime::currentDateTime();
  // QByteArray
  // If the byte array is null: 0xFFFFFFFF (quint32)
  // Otherwise: the array size (quint32) followed by the array bytes, i.e. size bytes
  QDataStream data_stream( this );

  if( m_protoVersion > SECURE_LEVEL_2_PROTO_VERSION )
    data_stream.setVersion( DATASTREAM_VERSION_2 );
  else
    data_stream.setVersion( DATASTREAM_VERSION_1 );

  if( m_blockSize == 0 )
  {
    if( m_protoVersion > SECURE_LEVEL_2_PROTO_VERSION )
    {
      if( bytesAvailable() < (int)sizeof(DATA_BLOCK_SIZE_32))
        return;
      DATA_BLOCK_SIZE_32 block_size_32;
      data_stream >> block_size_32;
      m_blockSize = block_size_32 - sizeof(DATA_BLOCK_SIZE_32); // bytearray serialize format
    }
    else
    {
      if( bytesAvailable() < (int)sizeof(DATA_BLOCK_SIZE_16))
        return;
      DATA_BLOCK_SIZE_16 block_size_16;
      data_stream >> block_size_16;
      m_blockSize = block_size_16 - sizeof(DATA_BLOCK_SIZE_32); // bytearray serialize format
    }
  }

#if defined( CONNECTION_SOCKET_IO_DEBUG )
  qDebug() << "ConnectionSocket read from" << m_hostAndPort << "the block size:" << m_blockSize;
#endif

  if( bytesAvailable() < m_blockSize )
    return;

  QByteArray byte_array_read;
  data_stream >> byte_array_read;

  if( byte_array_read.size() != (int)m_blockSize )
  {
    qWarning() << "ConnectionSocket read an invalid block size from" << m_hostAndPort << ":"
               << byte_array_read.size() << "bytes read and" << m_blockSize << "bytes aspected";
  }

  m_blockSize = 0;

  QByteArray decrypted_byte_array = Protocol::instance().decryptByteArray( byte_array_read, cipherKey() );

#if defined( CONNECTION_SOCKET_IO_DEBUG )
  qDebug() << "ConnectionSocket read from" << m_hostAndPort << "the byte array:" << decrypted_byte_array;
#endif

  if( m_userId == ID_INVALID )
    checkHelloMessage( decrypted_byte_array );
  else
    emit dataReceived( decrypted_byte_array );

  if( isConnected() && !m_isAborted && bytesAvailable() )
    QTimer::singleShot( 0, this, SLOT( readBlock() ) );
}

void ConnectionSocket::flushAll()
{
  int prevent_loop = 0;
  while( bytesAvailable() && prevent_loop < (MAX_NUM_OF_LOOP_IN_CONNECTON_SOCKECT * 10) )
  {
    prevent_loop++;
    readBlock();
  }
  flush();
}

QByteArray ConnectionSocket::serializeData( const QByteArray& bytes_to_send )
{
  QByteArray data_block;
  QDataStream data_stream( &data_block, QIODevice::WriteOnly );

  if( m_protoVersion > SECURE_LEVEL_2_PROTO_VERSION )
  {
    data_stream.setVersion( DATASTREAM_VERSION_2 );
    data_stream << (DATA_BLOCK_SIZE_32)0;

    if( bytes_to_send.size() > DATA_BLOCK_SIZE_32_LIMIT )
    {
      qWarning() << "Unable to send a message to" << m_hostAndPort << "because exceeded the limit of 32bit block data... truncated to max size";
      QByteArray bytes_to_send_truncated = bytes_to_send;
      bytes_to_send_truncated.truncate( DATA_BLOCK_SIZE_32_LIMIT );
      data_stream << bytes_to_send_truncated;
    }
    else
      data_stream << bytes_to_send;

    data_stream.device()->seek( 0 );
    data_stream << (DATA_BLOCK_SIZE_32)(data_block.size() - sizeof(DATA_BLOCK_SIZE_32));
  }
  else
  {
    data_stream.setVersion( DATASTREAM_VERSION_1 );
    data_stream << (DATA_BLOCK_SIZE_16)0;

    if( bytes_to_send.size() > DATA_BLOCK_SIZE_16_LIMIT )
    {
      qWarning() << "Unable to send a message to" << m_hostAndPort << "because exceeded the limit of 16bit block data... truncated to max size";
      QByteArray bytes_to_send_truncated = bytes_to_send;
      bytes_to_send_truncated.truncate( DATA_BLOCK_SIZE_16_LIMIT );
      data_stream << bytes_to_send_truncated;
    }
    else
      data_stream << bytes_to_send;

    data_stream.device()->seek( 0 );
    data_stream << (DATA_BLOCK_SIZE_16)(data_block.size() - sizeof(DATA_BLOCK_SIZE_16));
  }

  return data_block;
}

bool ConnectionSocket::sendData( const QByteArray& byte_array )
{
#if defined( CONNECTION_SOCKET_IO_DEBUG )
  qDebug() << "ConnectionSocket sends to" << m_hostAndPort << "the following data:" << byte_array;
#endif

  QByteArray byte_array_to_send = Protocol::instance().encryptByteArray( byte_array, cipherKey() );

  QByteArray data_serialized = serializeData( byte_array_to_send );

  if( write( data_serialized ) == data_serialized.size() )
  {
#ifdef CONNECTION_SOCKET_IO_DEBUG
    qDebug() << "ConnectionSocket sends" << data_serialized.size() << "bytes to" << m_hostAndPort;
#endif
    // send data is asynchronous and m_latestActivityDateTime is not set
    return true;
  }
  else
  {
    qWarning() << "ConnectionSocket has an I/O error";
    return false;
  }
}

void ConnectionSocket::sendQuestionHello()
{
  m_publicKey1 = Protocol::instance().newMd5Id();
#ifdef BEEBEEP_DEBUG
  qDebug() << "ConnectionSocket is sending pkey1 with shared-key:" << qPrintable( cipherKey() );
#endif
  if( sendData( Protocol::instance().helloMessage( m_publicKey1 ) ) )
  {
    qDebug() << "ConnectionSocket has sent question HELLO to" << m_hostAndPort;
    m_isHelloSent = true;
  }
  else
  {
    qWarning() << "ConnectionSocket is unable to send question HELLO to" << m_hostAndPort;
    emit abortRequest();
  }
}

void ConnectionSocket::sendAnswerHello()
{
  m_publicKey2 = Protocol::instance().newMd5Id();
#ifdef BEEBEEP_DEBUG
  qDebug() << "ConnectionSocket is sending pkey2 with shared-key:" << qPrintable( cipherKey() );
#endif
  if( sendData( Protocol::instance().helloMessage( m_publicKey2 ) ) )
  {
    qDebug() << "ConnectionSocket has sent answer HELLO to" << m_hostAndPort;
    m_isHelloSent = true;
  }
  else
  {
    qWarning() << "ConnectionSocket is unable to send answer HELLO to" << m_hostAndPort;
    emit abortRequest();
  }
}

void ConnectionSocket::checkHelloMessage( const QByteArray& array_data )
{
  Message m = Protocol::instance().toMessage( array_data );
  if( !m.isValid() )
  {
    qWarning() << "ConnectionSocket has received an invalid HELLO from" << m_hostAndPort;
    emit abortRequest();
    return;
  }

  if( m.type() != Message::Hello )
  {
    qWarning() << "ConnectionSocket is waiting for HELLO, but another message type" << m.type() << "is arrived from" << m_hostAndPort;
    emit abortRequest();
    return;
  }

  if( Settings::instance().acceptConnectionsOnlyFromWorkgroups() )
  {
    if( !Protocol::instance().acceptConnectionFromWorkgroup( m ) )
    {
      qWarning() << "ConnectionSocket drops user of external workgroup from" << m_hostAndPort;
      emit abortRequest();
      return;
    }
    else
      qDebug() << "ConnectionSocket has accepted user of your workgroup from" << m_hostAndPort;
  }

  if( !m_isHelloSent )
    sendAnswerHello();

  // After sending HELLO to ensure low protocol version compatibility
  m_protoVersion = Protocol::instance().protoVersion( m );

  if( m_protoVersion != Settings::instance().protoVersion() )
  {
    qWarning() << "Protocol version from" << m_hostAndPort << "is"
               << (m_protoVersion > Settings::instance().protoVersion() ? "newer" : "older") << "than yours";
    if( m_protoVersion > Settings::instance().protoVersion() )
    {
      m_protoVersion = Settings::instance().protoVersion();
      qWarning() << "Your old protocol version" << m_protoVersion << "is used with" << m_hostAndPort;
    }
    else
      qWarning() << "Old protocol version" << m_protoVersion << "is used with" << m_hostAndPort;
  }

  if( m_protoVersion > SECURE_LEVEL_2_PROTO_VERSION )
  {
    QString public_key = Protocol::instance().publicKey( m );
    if( !public_key.isEmpty() )
    {
      if( !createCipherKey( public_key ) )
      {
        qWarning() << "You have not shared a public key for encryption";
        emit abortRequest();
        return;
      }
      else
        qDebug() << "Encryption level 2 is activated with" << m_hostAndPort;

    }
    else
      qWarning() << "Remote host" << m_hostAndPort << "has not shared a public key for encryption";
  }

  qDebug() << "ConnectionSocket request an authentication for" << m_hostAndPort;
  emit authenticationRequested( m );
}

int ConnectionSocket::fileTransferBufferSize() const
{
  return m_protoVersion > SECURE_LEVEL_2_PROTO_VERSION ? Settings::instance().fileTransferBufferSize() : qMin( (int)65456, Settings::instance().fileTransferBufferSize() );
}

void ConnectionSocket::checkConnectionTimeout( int ticks )
{
  if( ticks < Settings::instance().tickIntervalConnectionTimeout() )
    return;

  stopTimerTick();
  qDebug() << "Connection timeout for" << m_hostAndPort;
  disconnectFromHost();
  emit disconnected();
}

int ConnectionSocket::activityIdle() const
{
  if( !m_latestActivityDateTime.isValid() )
    return 0;

  quint64 idle_time = m_latestActivityDateTime.msecsTo( QDateTime::currentDateTime() );

  if( idle_time < 2147483647 )
    return (int)idle_time;
  else
    return 2147483647;
}

void ConnectionSocket::timerEvent( QTimerEvent* timer_event )
{
  if( timer_event->timerId() != m_timerTickId )
  {
    QTcpSocket::timerEvent( timer_event );
    return;
  }

  m_tickCounter++;

  if( isConnecting() )
  {
    checkConnectionTimeout( m_tickCounter );
    return;
  }

  if( m_tickCounter > 31536000 )
  {
    // 1 year is passed ... it is time to close!
    qWarning() << "A year in uptime is passed. It is time to close and restart connection" << m_hostAndPort;
    emit abortRequest();
    return;
  }

  if( activityIdle() >= TICK_INTERVAL && bytesAvailable() )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Bytes available in tick interval and read socket is forced for" << m_hostAndPort;
#endif
    readBlock();
    return;
  }

  emit tickEvent( m_tickCounter );
}
