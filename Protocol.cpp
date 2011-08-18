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

#include "Protocol.h"
#include "Settings.h"
#include "Rijndael.h"

Protocol* Protocol::mp_instance = NULL;
const QChar PROTOCOL_FIELD_SEPARATOR = QChar::ParagraphSeparator;  // 0x2029
const QChar DATA_FIELD_SEPARATOR = QChar::LineSeparator; // 0x2028


Protocol::Protocol()
  : m_id( ID_START ), m_writingMessage( Message::User, ID_WRITING_MESSAGE, "*" )
{
  m_writingMessage.addFlag( Message::Private );
  m_writingMessage.addFlag( Message::Writing );
}

QString Protocol::messageHeader( Message::Type mt ) const
{
  switch( mt )
  {
  case Message::Beep:   return "BEE-BEEP";
  case Message::Ping:   return "BEE-PING";
  case Message::Pong:   return "BEE-PONG";
  case Message::Chat:   return "BEE-CHAT";
  case Message::Hello:  return "BEE-CIAO";
  case Message::System: return "BEE-SYST";
  case Message::User:   return "BEE-USER";
  case Message::File:   return "BEE-FILE";
  default:              return "BEE-BOOH";
  }
}

Message::Type Protocol::messageType( const QString& msg_type ) const
{
  if( msg_type == "BEE-BEEP" )
    return Message::Beep;
  else if( msg_type == "BEE-PING" )
    return Message::Ping;
  else if( msg_type == "BEE-PONG" )
    return Message::Pong;
  else if( msg_type == "BEE-USER")
    return Message::User;
  else if( msg_type == "BEE-CHAT")
    return Message::Chat;
  else if( msg_type == "BEE-CIAO")
    return Message::Hello;
  else if( msg_type == "BEE-SYST")
    return Message::System;
  else if( msg_type == "BEE-FILE" )
    return Message::File;
  else
    return Message::Undefined;
}

QByteArray Protocol::fromMessage( const Message& m ) const
{
  if( !m.isValid() )
    return "";
  QStringList sl;
  sl << messageHeader( m.type() );
  sl << QString::number( m.id() );
  sl << QString::number( m.text().size() );
  sl << QString::number( m.flags() );
  sl << m.data();
  sl << m.timestamp().toString( Qt::ISODate );
  sl << m.text();
  QByteArray byte_array = sl.join( PROTOCOL_FIELD_SEPARATOR ).toUtf8();
  while( byte_array.size() % ENCRYPTED_DATA_BLOCK_SIZE )
    byte_array.append( ' ' );
  return byte_array;
}

Message Protocol::toMessage( const QByteArray& byte_array_data ) const
{
  QString message_data = QString::fromUtf8( byte_array_data );
  Message m;
  QStringList sl = message_data.split( PROTOCOL_FIELD_SEPARATOR, QString::KeepEmptyParts );
  if( sl.size() < 7 )
  {
    qWarning() << "Invalid message fields:" << message_data;
    return m;
  }
  m.setType( messageType( sl.first() ) );
  if( !m.isValid() )
  {
    qWarning() << "Invalid message type:" << message_data;
    return m;
  }
  sl.removeFirst();
  bool ok = false;
  int msg_id = sl.first().toInt( &ok );
  if( !ok )
  {
    qWarning() << "Invalid message id:" << message_data;
    m.setType( Message::Undefined );
    return m;
  }
  m.setId( msg_id );
  sl.removeFirst();
  int msg_size = sl.first().toInt( &ok );
  if( !ok )
  {
    qWarning() << "Invalid message size:" << message_data;
    m.setType( Message::Undefined );
    return m;
  }
  sl.removeFirst();
  int msg_flags = sl.first().toInt( &ok );
  if( !ok )
  {
    qWarning() << "Invalid message flags:" << message_data;
    m.setType( Message::Undefined );
    return m;
  }
  m.setFlags( msg_flags );
  sl.removeFirst();
  m.setData( sl.first() );
  sl.removeFirst();
  m.setTimestamp( QDateTime::fromString( sl.first(), Qt::ISODate ) );
  if( !m.timestamp().isValid() )
  {
    qWarning() << "Invalid message timestamp:" << message_data;
    m.setType( Message::Undefined );
    return m;
  }
  sl.removeFirst();
  QString msg_txt;
  if( sl.size() > 1 )
    msg_txt = sl.join( PROTOCOL_FIELD_SEPARATOR );
  else if( sl.size() == 1 )
    msg_txt = sl.first();
  else
    msg_txt = "";

  if( msg_txt.size() > msg_size )
    msg_txt.resize( msg_size ); // to prevent spaces added for encryption
  m.setText( msg_txt );

  return m;
}

QByteArray Protocol::pingMessage() const
{
  Message m( Message::Ping, ID_PING_MESSAGE, "*" );
  return fromMessage( m );
}

QByteArray Protocol::pongMessage() const
{
  Message m( Message::Pong, ID_PONG_MESSAGE, "*" );
  return fromMessage( m );
}

QByteArray Protocol::broadcastMessage() const
{
  Message m( Message::Beep, ID_BEEP_MESSAGE, QString::number( UserManager::instance().localUser().hostPort() ) );
  return fromMessage( m );
}

QByteArray Protocol::helloMessage() const
{
  QStringList data_list;
  data_list << UserManager::instance().localUser().name();
  data_list << UserManager::instance().localUser().nickname();
  Message m( Message::Hello, ID_HELLO_MESSAGE, data_list.join( DATA_FIELD_SEPARATOR ) );
  m.setData( Settings::instance().hash() );
  return fromMessage( m );
}

QByteArray Protocol::localUserStatusMessage() const
{
  Message m( Message::User, ID_STATUS_MESSAGE, UserManager::instance().localUser().statusDescription() );
  m.addFlag( Message::Status );
  m.setData( QString::number( UserManager::instance().localUser().status() ) );
  return fromMessage( m );
}

bool Protocol::changeUserStatusFromMessage( User* u, const Message& m ) const
{
  int user_status = m.data().toInt();
  QString user_status_description = m.text();
  if( u->status() != user_status || u->statusDescription() != user_status_description )
  {
    u->setStatus( user_status );
    u->setStatusDescription( user_status_description );
    return true;
  }
  return false;
}

User Protocol::createUser( const Message& hello_message, const QHostAddress& host_address, int host_port )
{
  /* Read User Field Data */
  QStringList sl = hello_message.text().split( DATA_FIELD_SEPARATOR, QString::KeepEmptyParts );
  if( sl.size() < 2 )
    return User();
  QString user_name = sl.at( 0 );
  sl.removeFirst();
  QString user_nickname = sl.size() > 1 ? sl.join( DATA_FIELD_SEPARATOR ) : sl.at( 0 );
  /* Auth */
  if( hello_message.data().toUtf8() != Settings::instance().hash( user_name ) )
    return User();
  /* Create User */
  User u( newId() );
  u.setName( user_name );
  u.setNickname( user_nickname );
  u.setHostAddress( host_address );
  u.setHostPort( host_port );
  u.setStatus( User::Online );
  return u;
}

Message Protocol::fileInfoToMessage( const FileInfo& fi )
{
  Message m( Message::File, newId(), fi.name() );
  QStringList sl;
  sl << QString::number( fi.hostPort() );
  sl << QString::number( fi.size() );
  sl << QString::number( fi.id() );
  sl << QString::fromUtf8( fi.password() );
  m.setData( sl.join( DATA_FIELD_SEPARATOR ) );
  m.addFlag( Message::Private );
  return m;
}

FileInfo Protocol::fileInfoFromMessage( const Message& m )
{
  FileInfo fi( 0, FileInfo::Download );
  fi.setName( m.text() );
  QStringList sl = m.data().split( DATA_FIELD_SEPARATOR );
  if( sl.size() < 4 )
    return FileInfo( 0, FileInfo::Download );
  fi.setHostPort( sl.at( 0 ).toInt() );
  sl.removeFirst();
  fi.setSize( sl.at( 0 ).toULongLong() ); // FIXME ???
  sl.removeFirst();
  fi.setId( sl.at( 0 ).toULongLong() ); // FIXME ???
  sl.removeFirst();
  QString password = sl.size() > 1 ? sl.join( DATA_FIELD_SEPARATOR ) : sl.at( 0 );
  fi.setPassword( password.toUtf8() );
  return fi;
}

/* Encryption */

namespace
{
  QList<QByteArray> SplitByteArray( const QByteArray& byte_array, int num_chars, char fill_char = '\0' )
  {
    QList<QByteArray> array_list;

    if( byte_array.isEmpty() || byte_array.isNull() )
      return array_list;

    QByteArray tmp;

    for( int i = 0; i < byte_array.size(); i++ )
    {
      tmp += byte_array.at( i );
      if( tmp.size() == num_chars )
      {
        array_list.append( tmp );
        tmp = "";
      }
    }

    if( !tmp.isEmpty() )
    {
      qDebug() << "Not encrypted string:" << tmp;
      array_list.append( tmp );
    }

    return array_list;
  }
}

QByteArray Protocol::encryptByteArray( const QByteArray& byte_array ) const
{
  unsigned long rk[ RKLENGTH(ENCRYPTION_KEYBITS) ];
  unsigned char key[ KEYLENGTH(ENCRYPTION_KEYBITS) ];
  unsigned int i;
  int nrounds;

  if( byte_array.isNull() || byte_array.isEmpty() )
    return QByteArray();

  QByteArray password = Settings::instance().password();
  for( i = 0; i < sizeof( key ); i++ )
  {
    key[ i ] = (unsigned int)password.size() < i ? static_cast<unsigned char>( password[ i ] ) : 0;
  }

  nrounds = rijndaelSetupEncrypt( rk, key, ENCRYPTION_KEYBITS );

  QList<QByteArray> byte_array_list = SplitByteArray( byte_array, ENCRYPTED_DATA_BLOCK_SIZE );

  unsigned char plaintext[ ENCRYPTED_DATA_BLOCK_SIZE ];
  unsigned char ciphertext[ ENCRYPTED_DATA_BLOCK_SIZE ];

  memset( ciphertext, 0, sizeof( ciphertext ) );
  memset( plaintext, 0, sizeof( plaintext ) );

  QByteArray encrypted_byte_array;

  foreach( QByteArray ba, byte_array_list )
  {
    if( ba.size() == sizeof( plaintext ) )
    {
      for( i = 0; i < sizeof( plaintext ); i++ )
        plaintext[ i ] = static_cast<unsigned char>( ba[ i ] );

      rijndaelEncrypt( rk, nrounds, plaintext, ciphertext );

      for( i = 0; i < sizeof( ciphertext ); i++ )
        encrypted_byte_array.append( static_cast<char>( ciphertext[ i ] ) );

      memset( ciphertext, 0, sizeof( ciphertext ) );
      memset( plaintext, 0, sizeof( plaintext ) );
    }
    else
      encrypted_byte_array += ba;

  }

  return encrypted_byte_array;
}

QByteArray Protocol::decryptByteArray( const QByteArray& byte_array_encrypted ) const
{
  unsigned long rk[RKLENGTH(ENCRYPTION_KEYBITS)];
  unsigned char key[KEYLENGTH(ENCRYPTION_KEYBITS)];
  unsigned int i;
  int nrounds;

  if( byte_array_encrypted.isNull() || byte_array_encrypted.isEmpty() )
    return QByteArray();

  QByteArray password = Settings::instance().password();
  for( i = 0; i < sizeof( key ); i++ )
  {
    key[ i ] = (unsigned int)password.size() < i ? static_cast<unsigned char>( password[ i ] ) : 0;
  }

  nrounds = rijndaelSetupDecrypt( rk, key, ENCRYPTION_KEYBITS );

  QList<QByteArray> byte_array_list = SplitByteArray( byte_array_encrypted, ENCRYPTED_DATA_BLOCK_SIZE );

  unsigned char plaintext[ ENCRYPTED_DATA_BLOCK_SIZE ];
  unsigned char ciphertext[ ENCRYPTED_DATA_BLOCK_SIZE ];

  memset( ciphertext, 0, sizeof( ciphertext ) );
  memset( plaintext, 0, sizeof( plaintext ) );

  QByteArray decrypted_byte_array;

  foreach( QByteArray ba, byte_array_list )
  {
    if( ba.size() == sizeof( ciphertext ) )
    {
      for( i = 0; i < sizeof( ciphertext ); i++ )
        ciphertext[ i ] = static_cast<unsigned char>( ba[ i ] );

      rijndaelDecrypt( rk, nrounds, ciphertext, plaintext );

      for( i = 0; i < sizeof( plaintext ); i++ )
        decrypted_byte_array.append( static_cast<char>( plaintext[ i ] ) );

      memset( ciphertext, 0, sizeof( ciphertext ) );
      memset( plaintext, 0, sizeof( plaintext ) );
    }
    else
      decrypted_byte_array += ba;
  }

  return decrypted_byte_array;
}
