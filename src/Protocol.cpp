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

#include "BeeUtils.h"
#include "Protocol.h"
#include "Random.h"
#include "Rijndael.h"
#include "Settings.h"



Protocol* Protocol::mp_instance = NULL;
const QChar PROTOCOL_FIELD_SEPARATOR = QChar::ParagraphSeparator;  // 0x2029
const QChar DATA_FIELD_SEPARATOR = QChar::LineSeparator; // 0x2028


Protocol::Protocol()
  : m_id( ID_START ), m_writingMessage(), m_localFileShareMessage( "" )
{
  Message writing_message( Message::User, ID_WRITING_MESSAGE, "*" );
  writing_message.addFlag( Message::Private );
  writing_message.addFlag( Message::UserWriting );
  m_writingMessage = fromMessage( writing_message );
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
  case Message::Share:  return "BEE-FSHR";
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
  else if( msg_type == "BEE-FSHR" )
    return Message::Share;
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
    qWarning() << "Invalid number of fields in message:" << message_data;
    return m;
  }
  m.setType( messageType( sl.first() ) );
  if( !m.isValid() )
  {
    qWarning() << "Invalid message type:" << message_data;
    return m;
  }
  sl.removeFirst();
  VNumber msg_id = Bee::qVariantToVNumber( sl.first() );
  if( msg_id == ID_INVALID )
  {
    qWarning() << "Invalid message id:" << message_data;
    m.setType( Message::Undefined );
    return m;
  }
  m.setId( msg_id );
  sl.removeFirst();
  bool ok = false;
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
  Message m( Message::Beep, ID_BEEP_MESSAGE, QString::number( Settings::instance().localUser().hostPort() ) );
  return fromMessage( m );
}

int Protocol::protoVersion( const Message& m ) const
{
  return m.id() <= ID_HELLO_MESSAGE ? 1 : m.id();
}

QByteArray Protocol::helloMessage() const
{
  QStringList data_list;
  data_list << QString::number( Settings::instance().localUser().hostPort() );
  data_list << Settings::instance().localUser().name();
  data_list << QString::number( Settings::instance().localUser().status() );
  data_list << Settings::instance().localUser().statusDescription();
  data_list << Settings::instance().localUser().bareJid();
  Message m( Message::Hello, Settings::instance().protoVersion(), data_list.join( DATA_FIELD_SEPARATOR ) );
  m.setData( Settings::instance().hash() );
  return fromMessage( m );
}

Message Protocol::userStatusMessage( int user_status, const QString& user_status_description ) const
{
  Message m( Message::User, ID_USER_MESSAGE, user_status_description );
  m.addFlag( Message::UserStatus );
  m.setData( QString::number( user_status ) );
  return m;
}

QByteArray Protocol::localUserStatusMessage() const
{
  return fromMessage( userStatusMessage( Settings::instance().localUser().status(), Settings::instance().localUser().statusDescription() ) );
}

QByteArray Protocol::localUserNameMessage() const
{
  Message m( Message::User, ID_USER_MESSAGE, Settings::instance().localUser().name() );
  m.addFlag( Message::UserName );
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

bool Protocol::changeUserNameFromMessage( User* u, const Message& m ) const
{
  if( m.text().size() < 1 )
    return false;
  u->setName( m.text().trimmed() );
  return true;
}

QString Protocol::pixmapToString( const QPixmap& pix ) const
{
  if( pix.isNull() )
    return "";
  QByteArray byte_array;
  QBuffer buf( &byte_array );
  buf.open( QIODevice::WriteOnly );
  pix.save( &buf, "PNG" );
  return QString( byte_array.toBase64() );
}

QPixmap Protocol::stringToPixmap( const QString& s ) const
{
  QPixmap pix;
  if( s.isEmpty() )
    return pix;
  QByteArray byte_array = QByteArray::fromBase64( s.toAscii() );
  pix.loadFromData( byte_array, "PNG" );
  return pix;
}

QByteArray Protocol::localVCardMessage() const
{
  const VCard& vc = Settings::instance().localUser().vCard();
  Message m( Message::User, ID_USER_MESSAGE, pixmapToString( vc.photo() ) );
  m.addFlag( Message::UserVCard );

  QStringList data_list;
  data_list << vc.nickName();
  data_list << vc.firstName();
  data_list << vc.lastName();
  data_list << vc.birthday().toString( Qt::ISODate );
  data_list << vc.email();
  m.setData( data_list.join( DATA_FIELD_SEPARATOR ) );

  return fromMessage( m );
}

bool Protocol::changeVCardFromMessage( User* u, const Message& m ) const
{
  QStringList sl = m.data().split( DATA_FIELD_SEPARATOR, QString::KeepEmptyParts );
  if( sl.size() < 5 )
    return false;

  VCard vc;
  vc.setNickName( sl.at( 0 ) );
  vc.setFirstName( sl.at( 1 ) );
  vc.setLastName( sl.at( 2 ) );
  vc.setBirthday( QDate::fromString( sl.at( 3 ), Qt::ISODate ) );
  vc.setEmail( sl.at( 4 ) );

  if( sl.size() > 5 )
    qWarning() << "VCARD message contains more data. Skip it";

  vc.setPhoto( stringToPixmap( m.text() ) );

  u->setVCard( vc );
  return true;
}

User Protocol::createUser( const Message& hello_message, const QHostAddress& peer_address )
{
 /* Read User Field Data */
  QStringList sl = hello_message.text().split( DATA_FIELD_SEPARATOR, QString::KeepEmptyParts );
  bool ok = false;

  if( sl.size() < 4 )
  {
    qWarning() << "HELLO message has not 4 field data but" << sl.size();
    return User();
  }

  int listener_port = sl.at( 0 ).toInt( &ok );
  if( !ok )
  {
    qWarning() << "HELLO has an invalid Listener port";
    return User();
  }

  QString user_name = sl.at( 1 );
  /* Auth */
  if( hello_message.data().toUtf8() != Settings::instance().hash( user_name ) )
  {
    qWarning() << "HELLO message use invalid password. Cannot authenticate the user.";
    return User();
  }

  int user_status = sl.at( 2 ).toInt( &ok );
  if( !ok )
    user_status = User::Online;

  QString user_status_description = sl.at( 3 );

  QString user_bare_jid = "";
  if( sl.size() > 4 )
    user_bare_jid = sl.at( 4 );
  else
    user_bare_jid = user_name;

  /* Skip other data */
  if( sl.size() > 5 )
    qWarning() << "HELLO message contains more data. Skip it";

  /* Create User */
  User u( newId() );
  u.setName( user_name );
  u.setHostAddress( peer_address );
  u.setHostPort( listener_port );
  u.setStatus( user_status );
  u.setStatusDescription( user_status_description );
  u.setBareJid( user_bare_jid );
  return u;
}

Chat Protocol::createChat( const QList<VNumber>& user_list )
{
  Chat c;
  c.setId( newId() );
  foreach( VNumber user_id, user_list )
    c.addUser( user_id );
  c.addUser( ID_LOCAL_USER );
  return c;
}

Message Protocol::fileInfoRefusedToMessage( const FileInfo& fi )
{
  Message m( Message::File, newId(), fi.name() );
  m.addFlag( Message::Refused );
  m.addFlag( Message::Private );
  return m;
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
  fi.setSize( Bee::qVariantToVNumber( sl.at( 1 ) ) );
  fi.setId( Bee::qVariantToVNumber( sl.at( 2 ) ) );
  QString password = sl.at( 3 );
  fi.setPassword( password.toUtf8() );

  /* Skip other data */
  if( sl.size() > 4 )
    qWarning() << "FILEINFO message contains more data. Skip it";

  return fi;
}

FileInfo Protocol::fileInfo( const QFileInfo& fi )
{
  FileInfo file_info = FileInfo( newId(), FileInfo::Upload );
  file_info.setName( fi.fileName() );
  file_info.setPath( fi.absoluteFilePath() );
  file_info.setSuffix( fi.suffix() );
  file_info.setSize( fi.size() );
  QString password_key = QString( "%1%2%3%4" ).arg( file_info.id() ).arg( file_info.path() ).arg( QDateTime::currentDateTime().toString() ).arg( Random::number( 111111, 999999 ) );
  file_info.setPassword( Settings::instance().hash( password_key ) );
  return file_info;
}

void Protocol::createLocalFileShareMessage( const QMultiMap<QString, FileInfo>& file_info_list, int server_port )
{
  if( file_info_list.isEmpty() || server_port <= 0 )
  {
    m_localFileShareMessage = "";
    return;
  }

  QStringList msg_list;
  foreach( FileInfo fi, file_info_list )
  {
    QStringList sl;
    sl << fi.name();
    sl << fi.suffix();
    sl << QString::number( fi.size() );
    sl << QString::number( fi.id() );
    sl << QString::fromUtf8( fi.password() );

    msg_list.append( sl.join( DATA_FIELD_SEPARATOR ) );
  }

  Message m( Message::Share, ID_SHARE_MESSAGE, msg_list.join( PROTOCOL_FIELD_SEPARATOR ) );
  m.setData( QString::number( server_port ) );
  m.addFlag( Message::List );

  m_localFileShareMessage = fromMessage( m );
}

QList<FileInfo> Protocol::messageToFileShare( const Message& m, const QHostAddress& server_address ) const
{
  QList<FileInfo> file_info_list;
  if( m.type() != Message::Share )
    return file_info_list;

  QStringList sl = m.data().split( DATA_FIELD_SEPARATOR );
  if( sl.size() <= 0 )
    return file_info_list;

  int server_port = sl.at( 0 ).toInt();
  /* Skip other data */
  if( sl.size() > 1 )
    qWarning() << "FILESHARE message contains more data. Skip it";

  sl = m.text().split( PROTOCOL_FIELD_SEPARATOR, QString::SkipEmptyParts );

  QStringList::const_iterator it = sl.begin();
  while( it != sl.end() )
  {
    QStringList sl_tmp = (*it).split( DATA_FIELD_SEPARATOR, QString::SkipEmptyParts );

    if( sl_tmp.size() >= 5 )
    {
      FileInfo fi;
      fi.setTransferType( FileInfo::Download );
      fi.setHostAddress( server_address );
      fi.setHostPort( server_port );
      fi.setName( sl_tmp.at( 0 ) );
      fi.setSuffix( sl_tmp.at( 1 ) );
      fi.setSize( Bee::qVariantToVNumber( sl_tmp.at( 2 ) ) );
      fi.setId( Bee::qVariantToVNumber( sl_tmp.at( 3 ) ) );
      fi.setPassword( sl_tmp.at( 4 ).toUtf8() );

      file_info_list.append( fi );
    }

    ++it;
  }

  return file_info_list;
}


ChatMessageData Protocol::dataFromChatMessage( const Message& m )
{
  ChatMessageData cmd;
  if( m.data().size() <= 0 )
    return cmd;
  QStringList sl = m.data().split( DATA_FIELD_SEPARATOR );
  if( sl.size() <= 0 )
    return cmd;

  QColor c( sl.at( 0 ) );
  if( c.isValid() )
    cmd.setTextColor( c );
  else
    qWarning() << "Invalid text color in Chat Message Data:" << m.data();

  return cmd;
}

/* Encryption */

namespace
{
  QList<QByteArray> SplitByteArray( const QByteArray& byte_array, int num_chars )
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

QString Protocol::simpleEncrypt( const QString& plain_text )
{
  if( plain_text.size() <= 0 )
    return "";
  char key = 'k';
  QString encrypted_text = "";
  for( int i = 0; i < plain_text.size(); i++ )
    encrypted_text += plain_text.at( i ).toAscii() ^ (int(key) + i) % 255;
  return encrypted_text;
}

QString Protocol::simpleDecrypt( const QString& encrypted_text )
{
  if( encrypted_text.size() <= 0 )
    return "";
  char key = 'k';
  QString plain_text = "";
  for( int i = 0; i < encrypted_text.size(); i++ )
    plain_text += encrypted_text.at( i ).toAscii() ^ (int(key) + i) % 255;
  return plain_text;
}

