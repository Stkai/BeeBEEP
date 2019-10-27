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
#include "NetworkManager.h"
#include "Protocol.h"
#include "Settings.h"

#undef CONNECTION_SOCKET_IO_DEBUG
#undef CONNECTION_SOCKET_IO_DEBUG_VERBOSE


ConnectionSocket::ConnectionSocket( QObject* parent )
  : QTcpSocket( parent ), m_blockSize( 0 ), m_isHelloSent( false ), m_userId( ID_INVALID ), m_protoVersion( 1 ),
    m_cipherKey( "" ), m_publicKey1( "" ), m_publicKey2( "" ), m_networkAddress(), m_latestActivityDateTime(),
    m_checkConnectionTimeout( false ), m_tickCounter( 0 ), m_isAborted( false ), m_datastreamVersion( 0 ),
    m_isTestConnection( false ), m_serverPort( 0 ), m_isEncrypted( true ), m_isCompressed( false )
{
  if( Settings::instance().useLowDelayOptionOnSocket() )
    setSocketOption( QAbstractSocket::LowDelayOption, 1 );
  if( Settings::instance().disableSystemProxyForConnections() )
    setProxy( QNetworkProxy::NoProxy );
  m_pingByteArraySize = Protocol::instance().pingMessage().size() + 10;

  connect( this, SIGNAL( connected() ), this, SLOT( sendQuestionHello() ) );
  connect( this, SIGNAL( readyRead() ), this, SLOT( readBlock() ) );
  connect( this, SIGNAL( bytesWritten( qint64 ) ), this, SLOT( onBytesWritten( qint64 ) ) );
}

void ConnectionSocket::initSocket( qintptr socket_descriptor, quint16 server_port )
{
  m_isAborted = false;
  setSocketDescriptor( socket_descriptor );
  m_networkAddress.setHostAddress( peerAddress() );
  m_networkAddress.setHostPort( peerPort() );
  m_serverPort = server_port;
  m_tickCounter = 0;
  m_checkConnectionTimeout = false;
  m_isEncrypted = !Settings::instance().disableConnectionSocketEncryption();
  m_isCompressed = false;
#ifdef BEEBEEP_DEBUG
  qDebug() << "Connection socket initializes peer with network address" << qPrintable( m_networkAddress.toString() ) << "and server port" << m_serverPort;
#endif
}

void ConnectionSocket::connectToNetworkAddress( const NetworkAddress& network_address )
{
  m_isAborted = false;
  m_networkAddress = network_address;
  m_tickCounter = 0;
  m_checkConnectionTimeout = true;
  m_serverPort = 0;
  m_isEncrypted = !Settings::instance().disableConnectionSocketEncryption();;
  m_isCompressed = false;
  connectToHost( network_address.hostAddress(), network_address.hostPort() );
}

void ConnectionSocket::abortConnection()
{
  m_isAborted = true;
  m_userId = ID_INVALID;
  abort();
}

void ConnectionSocket::closeConnection()
{
  if( isOpen() )
  {
    flushAll();
    close();
  }
  m_userId = ID_INVALID;
  m_isAborted = true;
}

void ConnectionSocket::useCompression( bool compression_enabled )
{
  m_isCompressed = compression_enabled;
  if( !Settings::instance().disableConnectionSocketDataCompression() && !m_isCompressed )
    qWarning() << "ConnectionSocket disables compression for address peer" << qPrintable( m_networkAddress.toString() );
}

void ConnectionSocket::useEncryption( bool encryption_enabled )
{
  if( m_isEncrypted )
  {
    if( !encryption_enabled )
    {
      qWarning() << "ConnectionSocket disables encryption for address peer" << qPrintable( m_networkAddress.toString() );
      m_isEncrypted = false;
    }
  }
  else
  {
    if( encryption_enabled )
    {
      qWarning() << "ConnectionSocket enables encryption for address peer" << qPrintable( m_networkAddress.toString() );
      m_isEncrypted = true;
    }
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
#ifdef BEEBEEP_DEBUG
    qDebug() << "Encryption handshake key 1";
#endif
    m_publicKey1 = public_key;
  }
  else if( m_publicKey2.isEmpty() )
  {
    m_publicKey2 = public_key;
#ifdef BEEBEEP_DEBUG
    qDebug() << "Encryption handshake key 2";
#endif
  }
  else
  {
    qWarning() << "Encryption handshake error. Too many public key arrived from" << qPrintable( m_networkAddress.toString() );
    return false;
  }

  if( m_publicKey1.isEmpty() || m_publicKey2.isEmpty() )
    return false;

#ifdef BEEBEEP_DEBUG
  qDebug() << "Encryption handshake completed with" << qPrintable( m_networkAddress.toString() );
#endif

  m_cipherKey = Protocol::instance().createCipherKey( m_publicKey1, m_publicKey2, m_datastreamVersion );
  m_publicKey1 = "";
  m_publicKey2 = "";
  return true;
}

qint64 ConnectionSocket::readBlock()
{
  if( m_isAborted )
    return 0;

  m_latestActivityDateTime = QDateTime::currentDateTime();
  qint64 bytes_available = bytesAvailable();

  if( bytes_available == 0 )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "ConnectionSocket from" << qPrintable( m_networkAddress.toString() ) << "is empty... wait for more bytes";
#endif
    return 0;
  }

  // QByteArray
  // If the byte array is null: 0xFFFFFFFF (quint32)
  // Otherwise: the array size (quint32) followed by the array bytes, i.e. size bytes
  QDataStream data_stream( this );
  data_stream.setByteOrder( QDataStream::BigEndian );

  if( m_datastreamVersion > 0 )
  {
    data_stream.setVersion( m_datastreamVersion );
  }
  else
  {
    if( m_protoVersion > SECURE_LEVEL_2_PROTO_VERSION )
      data_stream.setVersion( DATASTREAM_VERSION_2 );
    else
      data_stream.setVersion( DATASTREAM_VERSION_1 );
  }

#ifdef BEEBEEP_DEBUG
  if( isConnecting() )
    qDebug() << "Connection" << qPrintable( m_networkAddress.toString() ) << "uses datastream version:" << data_stream.version();
#endif

  if( m_blockSize == 0 )
  {
    if( m_protoVersion > SECURE_LEVEL_2_PROTO_VERSION )
    {
      if( static_cast<unsigned long>(bytes_available) < sizeof(DATA_BLOCK_SIZE_32) )
      {
#ifdef BEEBEEP_DEBUG
        qDebug() << "ConnectionSocket from" << qPrintable( m_networkAddress.toString() ) << "has only" << bytes_available << "bytes... wait for more";
#endif
        return 0;
      }
      DATA_BLOCK_SIZE_32 block_size_32;
      data_stream >> block_size_32;
      m_blockSize = block_size_32 - sizeof(DATA_BLOCK_SIZE_32); // bytearray serialize format must be 32 bytes
    }
    else
    {
      if( static_cast<unsigned long>(bytes_available) < sizeof(DATA_BLOCK_SIZE_16) )
      {
#ifdef BEEBEEP_DEBUG
        qDebug() << "ConnectionSocket from" << qPrintable( m_networkAddress.toString() ) << "has only" << bytes_available << "bytes... wait for more";
#endif
        return 0;
      }
      DATA_BLOCK_SIZE_16 block_size_16;
      data_stream >> block_size_16;
      m_blockSize = block_size_16 - sizeof(DATA_BLOCK_SIZE_32); // bytearray serialize format must be 32 bytes (not 16!!!)
    }
  }

#if defined( CONNECTION_SOCKET_IO_DEBUG )
  qDebug() << "ConnectionSocket read from" << qPrintable( m_networkAddress.toString() ) << "the block size:" << m_blockSize;
#endif

  if( bytes_available < m_blockSize )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "ConnectionSocket from" << qPrintable( m_networkAddress.toString() ) << "has" << bytes_available << "and wait for" << (m_blockSize-bytes_available) << "more bytes, total" << m_blockSize;
#endif
    return 0;
  }

  QByteArray byte_array_read;
#if QT_VERSION >= 0x050700
  data_stream.startTransaction();
#endif
  data_stream >> byte_array_read;

#if QT_VERSION >= 0x050700
  if( !data_stream.commitTransaction() )
  {
    data_stream.rollbackTransaction();
#ifdef BEEBEEP_DEBUG
    qDebug() << "ConnectionSocket from" << qPrintable( m_networkAddress.toString() ) << "cannot complete transaction and unable to read" << bytes_available;
#endif
    return 0;
  }
#endif

  if( byte_array_read.size() < 1 )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "ConnectionSocket from" << qPrintable( m_networkAddress.toString() ) << "has" << bytes_available << "but it is busy and read 0 bytes";
#endif
    return 0;
  }

  unsigned int byte_array_read_size = static_cast<unsigned int>( byte_array_read.size() );

  if( byte_array_read_size != m_blockSize )
  {
    qWarning() << "ConnectionSocket read an invalid block size from" << qPrintable( m_networkAddress.toString() ) << ":"
               << byte_array_read_size << "bytes read and" << m_blockSize << "bytes aspected";
  }

  m_blockSize = 0;
  QByteArray decrypted_byte_array;

  if( isKeysHandshakeCompleted() && !isEncrypted() )
    decrypted_byte_array = byte_array_read;
  else
    decrypted_byte_array = Protocol::instance().decryptByteArray( byte_array_read, cipherKey(), m_protoVersion );

#if defined( CONNECTION_SOCKET_IO_DEBUG_VERBOSE )
  qDebug() << "ConnectionSocket reads from" << qPrintable( m_networkAddress.toString() ) << "the byte array:" << decrypted_byte_array;
#endif

  if( m_userId == ID_INVALID )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "HELLO received from" << qPrintable( m_networkAddress.toString() );
#endif
    checkHelloMessage( decrypted_byte_array );
  }
  else
  {
    if( isCompressed() )
    {
      QByteArray uncompressed_byte_array = qUncompress( decrypted_byte_array );
      if( uncompressed_byte_array.isEmpty() )
      {
        qWarning() << "ConnectionSocket foun an invalid compressed data from" << qPrintable( m_networkAddress.toString() );
        emit dataReceived( decrypted_byte_array );
      }
      else
        emit dataReceived( uncompressed_byte_array );
    }
    else
      emit dataReceived( decrypted_byte_array );
  }

  return byte_array_read_size;
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
  data_stream.setByteOrder( QDataStream::BigEndian );

  if( m_protoVersion > SECURE_LEVEL_2_PROTO_VERSION )
  {
    if( m_datastreamVersion > 0 )
      data_stream.setVersion( m_datastreamVersion );
    else
      data_stream.setVersion( DATASTREAM_VERSION_2 );

    data_stream << static_cast<DATA_BLOCK_SIZE_32>(0);

    if( bytes_to_send.size() > DATA_BLOCK_SIZE_32_LIMIT )
    {
      qWarning() << "Unable to send a message to" << qPrintable( m_networkAddress.toString() ) << "because exceeded the limit of 32bit block data... truncated to max size";
      QByteArray bytes_to_send_truncated = bytes_to_send;
      bytes_to_send_truncated.truncate( DATA_BLOCK_SIZE_32_LIMIT );
      data_stream << bytes_to_send_truncated;
    }
    else
      data_stream << bytes_to_send;

    data_stream.device()->seek( 0 );
    data_stream << static_cast<DATA_BLOCK_SIZE_32>(static_cast<unsigned int>(data_block.size()) - sizeof(DATA_BLOCK_SIZE_32));
  }
  else
  {
    data_stream.setVersion( DATASTREAM_VERSION_1 );
    data_stream << static_cast<DATA_BLOCK_SIZE_16>(0);

    if( bytes_to_send.size() > DATA_BLOCK_SIZE_16_LIMIT )
    {
      qWarning() << "Unable to send a message to" << qPrintable( m_networkAddress.toString() ) << "because exceeded the limit of 16bit block data... truncated to max size";
      QByteArray bytes_to_send_truncated = bytes_to_send;
      bytes_to_send_truncated.truncate( DATA_BLOCK_SIZE_16_LIMIT );
      data_stream << bytes_to_send_truncated;
    }
    else
      data_stream << bytes_to_send;

    data_stream.device()->seek( 0 );
    data_stream << static_cast<DATA_BLOCK_SIZE_16>(static_cast<unsigned int>(data_block.size()) - sizeof(DATA_BLOCK_SIZE_16));
  }

  return data_block;
}

void ConnectionSocket::onBytesWritten( qint64 bytes_written )
{
  // This function is useful for large byte array data to prevent connection timeout.
  // To recognize disconnection, ping message must be skipped
  if( bytes_written > m_pingByteArraySize )
    m_latestActivityDateTime = QDateTime::currentDateTime();
}

bool ConnectionSocket::sendData( const QByteArray& byte_array )
{
#if defined( CONNECTION_SOCKET_IO_DEBUG_VERBOSE )
  qDebug() << "ConnectionSocket is sending to" << qPrintable( m_networkAddress.toString() ) << "the following data:" << byte_array;
#endif
  QByteArray byte_array_to_send;

  if( isCompressed() )
  {
    byte_array_to_send = qCompress( byte_array );
#ifdef BEEBEEP_DEBUG
    qDebug() << "ConnectionSocket compress data to sent from" << byte_array.size() << "to" << byte_array_to_send.size() << "bytes";
#endif
  }
  else
    byte_array_to_send = byte_array;

  if( isEncrypted() )
    byte_array_to_send = Protocol::instance().encryptByteArray( byte_array_to_send, cipherKey(), m_protoVersion );

  QByteArray data_serialized = serializeData( byte_array_to_send );

  if( write( data_serialized ) == data_serialized.size() )
  {
#ifdef CONNECTION_SOCKET_IO_DEBUG
    qDebug() << "ConnectionSocket sends" << data_serialized.size() << "bytes to" << qPrintable( m_networkAddress.toString() );
#endif
    flush();
    return true;
  }
  else
  {
    qWarning() << "ConnectionSocket to"  << qPrintable( m_networkAddress.toString() ) << "has an I/O error";
    return false;
  }
}

void ConnectionSocket::sendQuestionHello()
{
  if( m_isTestConnection )
  {
    if( sendData( Protocol::instance().testQuestionMessage( m_networkAddress ) ) )
      qDebug() << "Connection TEST request sent to" << qPrintable( m_networkAddress.toString() );
  }
  else
  {
    m_checkConnectionTimeout = false;
    m_publicKey1 = Protocol::instance().newMd5Id();
#ifdef CONNECTION_SOCKET_IO_DEBUG
    qDebug() << "ConnectionSocket is sending pkey1 with shared-key:" << qPrintable( m_publicKey1 );
#endif
    if( sendData( Protocol::instance().helloMessage( m_publicKey1, isEncrypted(), !Settings::instance().disableConnectionSocketDataCompression() ) ) )
    {
#ifdef BEEBEEP_DEBUG
      qDebug() << "ConnectionSocket sent question HELLO to" << qPrintable( m_networkAddress.toString() );
#endif
      m_isHelloSent = true;
    }
    else
    {
      qWarning() << "ConnectionSocket is unable to send question HELLO to" << qPrintable( m_networkAddress.toString() );
      emit abortRequest();
    }
  }
}

void ConnectionSocket::sendAnswerHello()
{
  m_publicKey2 = Protocol::instance().newMd5Id();
#ifdef CONNECTION_SOCKET_IO_DEBUG
  qDebug() << "ConnectionSocket is sending pkey2 with shared-key:" << qPrintable( m_publicKey2 );
#endif
  if( sendData( Protocol::instance().helloMessage( m_publicKey2, isEncrypted(), !Settings::instance().disableConnectionSocketDataCompression() ) ) )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "ConnectionSocket sent answer HELLO to" << qPrintable( m_networkAddress.toString() );
#endif
    m_isHelloSent = true;
  }
  else
  {
    qWarning() << "ConnectionSocket is unable to send answer HELLO to" << qPrintable( m_networkAddress.toString() );
    emit abortRequest();
  }
}

void ConnectionSocket::checkHelloMessage( const QByteArray& array_data )
{
  Message m = Protocol::instance().toMessage( array_data, m_protoVersion );
  if( !m.isValid() )
  {
    qWarning() << "ConnectionSocket received an invalid HELLO from" << qPrintable( m_networkAddress.toString() );
    emit abortRequest();
    return;
  }

  if( checkTestMessage( m ) )
    return;

  if( m.type() != Message::Hello )
  {
    qWarning() << "ConnectionSocket is waiting for HELLO, but another message type" << m.type() << "is arrived from" << qPrintable( m_networkAddress.toString() );
    emit abortRequest();
    return;
  }

  if( isEncrypted() )
  {
    if( m.hasFlag( Message::EncryptionDisabled ) )
    {
      if( !Settings::instance().allowNotEncryptedConnectionsAlso() )
      {
        qWarning() << "ConnectionSocket does not accept authentication with encryption disabled from" << qPrintable( m_networkAddress.toString() );
        emit abortRequest();
        return;
      }
      else
        useEncryption( false );
    }
  }
  else
  {
    if( !m.hasFlag( Message::EncryptionDisabled ) )
    {
      if( !Settings::instance().allowEncryptedConnectionsAlso() )
      {
        qWarning() << "ConnectionSocket does not accept authentication with encryption enabled from" << qPrintable( m_networkAddress.toString() );
        emit abortRequest();
        return;
      }
      else
        useEncryption( true );
    }
  }

  if( Settings::instance().acceptConnectionsOnlyFromWorkgroups() )
  {
    if( !Protocol::instance().acceptConnectionFromWorkgroup( m ) )
    {
#ifdef BEEBEEP_DEBUG
      qDebug() << "ConnectionSocket drops user of external workgroup from" << qPrintable( m_networkAddress.toString() );
#endif
      emit abortRequest();
      return;
    }
    else
      qDebug() << "ConnectionSocket has accepted user of your workgroup from" << qPrintable( m_networkAddress.toString() );
  }

  if( !m_isHelloSent )
    sendAnswerHello();

  // After sending HELLO to ensure low protocol version compatibility
  m_protoVersion = Protocol::instance().protoVersion( m );

  int peer_datastream_version = Protocol::instance().datastreamVersion( m );
  if( peer_datastream_version > 0 )
  {
    m_datastreamVersion = qMax( 0, static_cast<int>( qMin( peer_datastream_version, Protocol::instance().datastreamMaxVersion() ) ) );
#ifdef BEEBEEP_DEBUG
    qDebug() << "ConnectionSocket uses handshaked datastream version" << m_datastreamVersion;
#endif
  }
  else
    m_datastreamVersion = 0;

  if( m_protoVersion != Settings::instance().protoVersion() )
  {
    qWarning() << "Protocol version from" << qPrintable( m_networkAddress.toString() ) << "is"
               << (m_protoVersion > Settings::instance().protoVersion() ? "newer" : "older") << "than yours";
    if( m_protoVersion > Settings::instance().protoVersion() )
    {
      m_protoVersion = Settings::instance().protoVersion();
      qWarning() << "Your old protocol version" << m_protoVersion << "is used with" << qPrintable( m_networkAddress.toString() );
    }
    else
      qWarning() << "Old protocol version" << m_protoVersion << "is used with" << qPrintable( m_networkAddress.toString() );
  }

  if( m_protoVersion > SECURE_LEVEL_2_PROTO_VERSION )
  {
    QString public_key = Protocol::instance().publicKey( m );
    if( !public_key.isEmpty() )
    {
      if( !createCipherKey( public_key ) )
      {
        qWarning() << "ConnectionSocket has not shared a public key to negotiate encryption with" << qPrintable( m_networkAddress.toString() );
        emit abortRequest();
        return;
      }
      else
      {
        if( isEncrypted() )
        {
          if( m_protoVersion < SECURE_LEVEL_3_PROTO_VERSION )
            qWarning() << "Old encryption level 2 is activated with" << qPrintable( m_networkAddress.toString() );
          else
            qDebug() << "Encryption level 3 is activated with" << qPrintable( m_networkAddress.toString() );
        }
        else
          qDebug() << "ConnectionSocket has completed the initial negotiation with" << qPrintable( m_networkAddress.toString() );
      }
    }
    else
      qWarning() << "Remote host" << qPrintable( m_networkAddress.toString() ) << "has not shared a public key to negotiate encryption";
  }

  if( m_protoVersion >= DATA_COMPRESSED_PROTO_VERSION && m.hasFlag( Message::Compressed ) && !Settings::instance().disableConnectionSocketDataCompression() )
    useCompression( true );
  else
    useCompression( false );

#ifdef BEEBEEP_DEBUG
  qDebug() << "ConnectionSocket request an authentication for" << qPrintable( m_networkAddress.toString() );
#endif
  emit authenticationRequested( array_data );
}

int ConnectionSocket::fileTransferBufferSize() const
{
  return m_protoVersion > SECURE_LEVEL_2_PROTO_VERSION ? Settings::instance().fileTransferBufferSize() : qMin( static_cast<int>(65456), Settings::instance().fileTransferBufferSize() );
}

void ConnectionSocket::checkConnectionTimeout( int ticks )
{
  if( !m_checkConnectionTimeout )
    return;

  if( ticks < Settings::instance().tickIntervalConnectionTimeout() )
    return;

  if( m_isTestConnection )
  {
    qDebug() << "Connection TEST timeout for" << qPrintable( m_networkAddress.toString() );
    emit connectionTestCompleted( QObject::tr( "The connection to %1 was not successful." ).arg( m_networkAddress.toString() ) );
  }
  else
    qDebug() << "Connection timeout for" << qPrintable( m_networkAddress.toString() ) << ":" << ticks << "ticks";
  disconnectFromHost();
  emit disconnected();
}

int ConnectionSocket::activityIdle() const
{
  if( m_isAborted )
    return 0;

  if( !m_latestActivityDateTime.isValid() )
    return 0;

  if( bytesAvailable() > 0 )
    return TICK_INTERVAL; // force a read block

#if QT_VERSION < 0x040700
  qint64 idle_time = m_latestActivityDateTime.time().msecsTo( QDateTime::currentDateTime().time() );
#else
  qint64 idle_time = m_latestActivityDateTime.msecsTo( QDateTime::currentDateTime() );
#endif

  if( idle_time < 2147483647 )
    return static_cast<int>(idle_time);
  else
    return 2147483647;
}

void ConnectionSocket::onTickEvent( int  )
{
  if( m_isAborted )
    return;

  m_tickCounter++;

  if( isConnecting() )
  {
    checkConnectionTimeout( m_tickCounter );
    return;
  }

  if( m_tickCounter > 31536000 )
  {
    // 1 year is passed ... it is time to close!
    qWarning() << "A year in uptime is passed. It is time to close and restart connection" << qPrintable( m_networkAddress.toString() );
    emit abortRequest();
    return;
  }

  qint64 bytes_available = bytesAvailable();
  if( bytes_available > 0 )
  {
    if( m_blockSize == 0 || bytes_available >= m_blockSize )
    {
#ifdef BEEBEEP_DEBUG
      qDebug() << qPrintable( m_networkAddress.toString() ) << "has" << bytes_available << "bytes available: read forced";
#endif
      if( readBlock() > 0 )
        return;
    }
  }

  if( m_tickCounter % PING_INTERVAL_TICK == 0 )
    emit pingRequest();
}

bool ConnectionSocket::checkTestMessage( const Message& m )
{
  if( m.type() != Message::Test )
    return false;

  if( !m_isTestConnection )
  {
    m_isTestConnection = true;
    qDebug() << "Connection become TEST after request from" << qPrintable( m_networkAddress.toString() );
  }

  if( Protocol::instance().isTestQuestionMessage( m ) )
  {
    NetworkAddress tested_na = Protocol::instance().networkAddressFromTestMessage( m );
    if( tested_na.isHostAddressValid() && tested_na.isHostPortValid() )
    {
      QString answer_msg = "";
      qDebug() << "Connection TEST for" << qPrintable( tested_na.toString() ) << "requested from" << qPrintable( m_networkAddress.toString() );

      if( !NetworkManager::instance().isLocalHostAddress( tested_na.hostAddress() ) )
      {
        qWarning() << "The tested network address" << qPrintable( tested_na.hostAddress().toString() ) << "is not a valid localhost address of your network interfaces.";
        answer_msg += QObject::tr( "The tested network address %1 is not present among the available addresses of the network interface to which the connection has been made." ).arg( tested_na.hostAddress().toString() );
      }
      else
        answer_msg += QObject::tr( "The connection to host address %1 was successful." ).arg( tested_na.hostAddress().toString() );

      answer_msg += QString( "<br>" );
      if( tested_na.hostPort() != m_serverPort )
      {
        qWarning() << "Port tested is" << tested_na.hostPort() << "but answering port is" << m_serverPort;
        answer_msg += QObject::tr( "The port tested by the connection is %1 but the one that answered is %2." ).arg( tested_na.hostPort() ).arg( m_serverPort );
        tested_na.setHostPort( m_serverPort );
      }
      else
        answer_msg += QObject::tr( "The connection to port %1 was successful." ).arg( tested_na.hostPort() );

      if( sendData( Protocol::instance().testAnswerMessage( tested_na, true, answer_msg ) ) )
        qDebug() << "Connection TEST from" << qPrintable( m_networkAddress.toString() ) << "successfully completed";
    }
    else
    {
      qDebug() << "Connection TEST from" << qPrintable( m_networkAddress.toString() ) << "refused";
      sendData( Protocol::instance().testAnswerMessage( tested_na, false, QObject::tr( "Unable to complete the test with an invalid network address.") ) );
    }
  }
  else if( Protocol::instance().isTestAnswerMessage( m ) )
  {
    qDebug() << "Connection TEST to" << qPrintable( m_networkAddress.toString() ) << "completed with answer:" << m.text();
    if( m.hasFlag( Message::Refused ) )
      qDebug() << "Connection TEST to" << qPrintable( m_networkAddress.toString() ) << "refused with answer:" << m.text();
    else
      qDebug() << "Connection TEST to" << qPrintable( m_networkAddress.toString() ) << "completed with answer:" << m.text();
    emit connectionTestCompleted( m.text() );
  }
  else
    qWarning() << "Connection TEST from" << qPrintable( m_networkAddress.toString() )  << "is not supported";

  flush();
  disconnectFromHost();
  emit abortRequest();
  return true;
}
