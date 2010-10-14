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
#include "EmoticonManager.h"
#include "Rijndael.h"

Protocol* Protocol::mp_instance = NULL;
const QString PROTOCOL_FIELD_SEPARATOR = "|";
const QString HELLO_FIELD_SEPARATOR = ",";
const QString DATA_FIELD_SEPARATOR = ",";


Protocol::Protocol()
  : m_id( ID_START ), m_writingMessage( Message::User, ID_WRITING_MESSAGE, "*" )
{
  m_writingMessage.addFlag( Message::Private );
  m_writingMessage.addFlag( Message::Writing );
}

int Protocol::newId()
{
  if( m_id > 2147483600 )
    m_id = ID_START;
  return ++m_id;
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
  else
    return Message::Undefined;
}

QString Protocol::fromMessage( const Message& m ) const
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
  return sl.join( PROTOCOL_FIELD_SEPARATOR );
}

Message Protocol::toMessage( const QString& message_data ) const
{
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

QString Protocol::pingMessage() const
{
  Message m( Message::Ping, ID_PING_MESSAGE, "*" );
  return fromMessage( m );
}

QString Protocol::pongMessage() const
{
  Message m( Message::Pong, ID_PONG_MESSAGE, "*" );
  return fromMessage( m );
}

QString Protocol::broadcastMessage() const
{
  int listener_port = Settings::instance().listenerPort();
  if( listener_port <= 0 )
    return "";
  Message m( Message::Beep, ID_BEEP_MESSAGE, QString::number( listener_port ) );
  return fromMessage( m );
}

QString Protocol::helloMessage() const
{
  QStringList data_list;
  data_list << Settings::instance().localUser().name();
  data_list << Settings::instance().localUser().nickname();
  Message m( Message::Hello, ID_HELLO_MESSAGE, data_list.join( HELLO_FIELD_SEPARATOR ) );
  m.setData( Settings::instance().hash() );
  return fromMessage( m );
}

Message Protocol::userStatusToMessage( const User& u )
{
  Message m( Message::User, ID_STATUS_MESSAGE, u.statusDescription() );
  m.addFlag( Message::Status );
  m.setData( QString::number( u.status() ) );
  return m;
}

User Protocol::userStatusFromMessage( User u, const Message& m )
{
  if( m.type() == Message::User && m.hasFlag( Message::Status ) )
  {
    int user_status = m.data().toInt();
    QString user_status_description = m.text();
    if( u.status() != user_status || u.statusDescription() != user_status_description )
    {
      u.setStatus( user_status );
      u.setStatusDescription( user_status_description );
      return u;
    }
  }
  return User();
}

User Protocol::createUser( const Message& hello_message )
{
  /* Read User Field Data */
  QStringList sl = hello_message.text().split( HELLO_FIELD_SEPARATOR, QString::KeepEmptyParts );
  if( sl.size() < 2 )
    return User();
  QString sUserName = sl.at( 0 );
  sl.removeFirst();
  QString sNickName = sl.size() > 1 ? sl.join( HELLO_FIELD_SEPARATOR ) : sl.at( 0 );
  /* Auth */
  if( hello_message.data() != Settings::instance().hash( sUserName ) )
    return User();
  /* Create User */
  User u( newId() );
  u.setName( sUserName.trimmed() );
  u.setNickname( sNickName.trimmed() );
  return u;
}

namespace
{
  QStringList SplitString( const QString& s, int num_chars, int* num_chars_to_fill_used = NULL, const QChar& char_to_fill = ' ' )
  {
    QStringList string_list;

    if( s.isEmpty() || s.isNull() )
      return string_list;

    QString string_tmp = "";

    for( int i = 0; i < s.size(); i++ )
    {
      string_tmp.append( s.at( i ) );
      if( string_tmp.size() == num_chars )
      {
        string_list.append( string_tmp );
        string_tmp = "";
      }
    }
    int num_chars_to_fill = 0;
    if( !string_tmp.isEmpty() )
    {
      while( string_tmp.size() < num_chars )
      {
        string_tmp.append( char_to_fill );
        num_chars_to_fill++;
      }
      string_list.append( string_tmp );
    }
    if( num_chars_to_fill_used )
      *num_chars_to_fill_used = num_chars_to_fill;
    return string_list;
  }
}

#define KEYBITS 256

QString Protocol::encrypt( const QString& txt, int* num_chars_used_to_fill ) const
{
  unsigned long rk[RKLENGTH(KEYBITS)];
  unsigned char key[KEYLENGTH(KEYBITS)];
  unsigned int i;
  int nrounds;

  if( txt.isNull() || txt.isEmpty() )
    return "";

  QByteArray password = Settings::instance().password();
  for( i = 0; i < sizeof( key ); i++ )
  {
    key[ i ] = (unsigned int)password.size() < i ? (char)password[ i ] : 0;
  }

  nrounds = rijndaelSetupEncrypt( rk, key, KEYBITS );

  QStringList string_list = SplitString( txt, 16, num_chars_used_to_fill );
  unsigned char plaintext[16];
  unsigned char ciphertext[16];
  QString encrypted_string = "";

  foreach( QString s, string_list )
  {
    for( i = 0; i < sizeof( plaintext ); i++ )
      plaintext[ i ] = s.at( i ).unicode();

    rijndaelEncrypt( rk, nrounds, plaintext, ciphertext );

    for( i = 0; i < sizeof( ciphertext ); i++ )
      encrypted_string.append( QChar( ciphertext[ i ] ) );
  }

  return encrypted_string;
}

QString Protocol::decrypt( const QString& txt_encrypted ) const
{
  unsigned long rk[RKLENGTH(KEYBITS)];
  unsigned char key[KEYLENGTH(KEYBITS)];
  unsigned int i;
  int nrounds;

  if( txt_encrypted.isNull() || txt_encrypted.isEmpty() )
    return "";

  QByteArray password = Settings::instance().password();
  for( i = 0; i < sizeof( key ); i++ )
  {
    key[ i ] = (unsigned int)password.size() < i ? (char)password[ i ] : 0;
  }

  nrounds = rijndaelSetupDecrypt( rk, key, KEYBITS );

  QStringList string_list = SplitString( txt_encrypted, 16 );
  unsigned char plaintext[16];
  unsigned char ciphertext[16];
  QString decrypted_string = "";

  foreach( QString s, string_list )
  {
    for( i = 0; i < sizeof( ciphertext ); i++ )
      ciphertext[ i ] =  s.at( i ).unicode();

    rijndaelDecrypt( rk, nrounds, ciphertext, plaintext );

    for( i = 0; i < sizeof( plaintext ); i++ )
      decrypted_string.append( QChar( plaintext[ i ] ) );
  }

  return decrypted_string;
}
