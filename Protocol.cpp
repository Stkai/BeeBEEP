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
  : m_id( 10 )
{
}

QString Protocol::messageHeader( Message::Type mt ) const
{
  switch( mt )
  {
  case Message::Beep:  return "MKM-BEEP";
  case Message::Ping:  return "MKM-PING";
  case Message::Pong:  return "MKM-PONG";
  case Message::Chat:  return "MKM-CHAT";
  case Message::Hello: return "MKM-CIAO";
  case Message::File:  return "MKM-FILE";
  default:             return "";
  }
}

Message::Type Protocol::messageType( const QString& msg_type ) const
{
  if( msg_type == "MKM-BEEP" )
    return Message::Beep;
  else if( msg_type == "MKM-PING" )
    return Message::Ping;
  else if( msg_type == "MKM-PONG" )
    return Message::Pong;
  else if( msg_type == "MKM-CHAT")
    return Message::Chat;
  else if( msg_type == "MKM-CIAO")
    return Message::Hello;
  else if( msg_type == "MKM-FILE")
    return Message::File;
  else
    return Message::Undefined;
}

QString Protocol::fromMessage( const Message& m ) const
{
  if( !m.isValid() )
    return "";
  QStringList sl;
  sl << messageHeader( m.type() );
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
  if( sl.size() < 6 )
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
  Message m( Message::Ping, "*" );
  return fromMessage( m );
}

QString Protocol::pongMessage() const
{
  Message m( Message::Pong, "*" );
  return fromMessage( m );
}

QString Protocol::broadcastMessage() const
{
  int listener_port = Settings::instance().listenerPort();
  if( listener_port <= 0 )
    return "";
  Message m( Message::Beep, QString::number( listener_port ) );
  return fromMessage( m );
}

Message Protocol::createSendFileMessage( const QFileInfo& file_info ) const
{
  Message m( Message::File, "?" );
  QStringList data_list;
  data_list << file_info.fileName();
  data_list << QString::number( file_info.size() );
  data_list << file_info.suffix();
  m.setData( data_list.join( DATA_FIELD_SEPARATOR ) );
  m.addFlag( Message::Private );
  return m;
}

Message Protocol::createSendFileMessageAnswer( const QString& file_data, bool file_accepted ) const
{
  QString sAnswer = file_accepted ? "yes" : "no";
  Message m( Message::File, sAnswer );
  m.setData( file_data );
  m.addFlag( Message::Private );
  return m;
}

QString Protocol::helloMessage() const
{
  QStringList data_list;
  data_list << Settings::instance().localUser().name();
  data_list << Settings::instance().localUser().nickname();
  Message m( Message::Hello, data_list.join( HELLO_FIELD_SEPARATOR ) );
  m.setData( Settings::instance().hash() );
  return fromMessage( m );
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
  User u( ++m_id );
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

QString Protocol::encrypt( const QString& txt ) const
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

  QStringList string_list = SplitString( txt, 16 );
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

  return encrypted_string.replace( "\n", "-NEWLINE-" );
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

  QString txt = txt_encrypted;
  txt.replace( "-NEWLINE-", "\n" );

  QStringList string_list = SplitString( txt, 16 );
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
