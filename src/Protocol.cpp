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

#include "BeeUtils.h"
#include "Protocol.h"
#include "Random.h"
#include "Rijndael.h"
#include "Settings.h"
#include "UserManager.h"
#include "EmoticonManager.h"
#include "PluginManager.h"


Protocol* Protocol::mp_instance = NULL;
const QChar PROTOCOL_FIELD_SEPARATOR = QChar::ParagraphSeparator;  // 0x2029
const QChar DATA_FIELD_SEPARATOR = QChar::LineSeparator; // 0x2028


Protocol::Protocol()
  : m_id( ID_START ), m_writingMessage(), m_fileShareListMessage( "" ), m_fileShareRequestMessage( "" )
{
  Message writing_message( Message::User, ID_WRITING_MESSAGE, "*" );
  writing_message.addFlag( Message::Private );
  writing_message.addFlag( Message::UserWriting );
  m_writingMessage = fromMessage( writing_message );

  Message file_share_request_message( Message::Share, ID_SHARE_MESSAGE, "" );
  file_share_request_message.addFlag( Message::Request );
  m_fileShareRequestMessage = fromMessage( file_share_request_message );
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
  case Message::Group:  return "BEE-GROU";
  case Message::Folder: return "BEE-FOLD";
  case Message::Read:   return "BEE-READ";
  case Message::Hive:   return "BEE-HIVE";
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
  else if( msg_type == "BEE-READ" )
    return Message::Read;
  else if( msg_type == "BEE-CIAO")
    return Message::Hello;
  else if( msg_type == "BEE-SYST")
    return Message::System;
  else if( msg_type == "BEE-FILE" )
    return Message::File;
  else if( msg_type == "BEE-FSHR" )
    return Message::Share;
  else if( msg_type == "BEE-GROU" )
    return Message::Group;
  else if( msg_type == "BEE-FOLD" )
    return Message::Folder;
  else if( msg_type == "BEE-HIVE" )
    return Message::Hive;
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
  m.setType( messageType( sl.takeFirst() ) );
  if( !m.isValid() )
  {
    qWarning() << "Invalid message type:" << message_data;
    return m;
  }

  VNumber msg_id = Bee::qVariantToVNumber( sl.takeFirst() );
  if( msg_id == ID_INVALID )
  {
    qWarning() << "Invalid message id:" << message_data;
    m.setType( Message::Undefined );
    return m;
  }
  m.setId( msg_id );

  bool ok = false;
  int msg_size = sl.takeFirst().toInt( &ok );
  if( !ok )
  {
    qWarning() << "Invalid message size:" << message_data;
    m.setType( Message::Undefined );
    return m;
  }

  int msg_flags = sl.takeFirst().toInt( &ok );
  if( !ok )
  {
    qWarning() << "Invalid message flags:" << message_data;
    m.setType( Message::Undefined );
    return m;
  }
  m.setFlags( msg_flags );

  m.setData( sl.takeFirst() );

  QDateTime dt_timestamp = QDateTime::fromString( sl.takeFirst(), Qt::ISODate );
  if( !dt_timestamp.isValid() )
  {
    qWarning() << "Invalid message timestamp:" << message_data;
    m.setType( Message::Undefined );
    return m;
  }
  else
    m.setTimestamp( dt_timestamp.toLocalTime() ); // sometimes people in vpn are in different timezone

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

QByteArray Protocol::broadcastMessage( const QHostAddress& to_host_address ) const
{
  Message m( Message::Beep, ID_BEEP_MESSAGE, QString::number( Settings::instance().localUser().hostPort() ) );
  QStringList sl;
  sl << to_host_address.toString();
  m.setData( sl.join( DATA_FIELD_SEPARATOR ) );
  return fromMessage( m );
}

QHostAddress Protocol::hostAddressFromBroadcastMessage( const Message& m ) const
{
  QHostAddress host_address;

  if( m.data().isEmpty() )
    return host_address;

  QStringList sl = m.data().split( DATA_FIELD_SEPARATOR );

  if( !sl.isEmpty() )
    host_address = QHostAddress( sl.takeFirst() );

  return host_address;
}

int Protocol::protoVersion( const Message& m ) const
{
  return m.id() <= ID_HELLO_MESSAGE ? 1 : m.id();
}

QString Protocol::publicKey( const Message& m ) const
{
  QStringList data_list = m.text().split( DATA_FIELD_SEPARATOR );
  return data_list.size() >= 6 ? data_list.at( 5 ) : QString();
}

QByteArray Protocol::helloMessage( const QString& public_key ) const
{
  QStringList data_list;
  data_list << QString::number( Settings::instance().localUser().hostPort() );
  data_list << Settings::instance().localUser().name();
  data_list << QString::number( Settings::instance().localUser().status() );
  data_list << Settings::instance().localUser().statusDescription();
  data_list << Settings::instance().localUser().accountName();
  data_list << public_key;
  data_list << Settings::instance().version( false );
  data_list << Settings::instance().localUser().sessionId();
  data_list << Settings::instance().localUser().color();
  if( Settings::instance().workgroups().isEmpty() )
    data_list << QString( "" );
  else
    data_list << Settings::instance().workgroups().join( ", " );
  data_list << Settings::instance().localUser().qtVersion();
  Message m( Message::Hello, Settings::instance().protoVersion(), data_list.join( DATA_FIELD_SEPARATOR ) );
  m.setData( Settings::instance().currentHash() );
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
  QByteArray byte_array = QByteArray::fromBase64( s.toLatin1() ); // base64 uses latin1 chars
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
  data_list << Settings::instance().localUser().color();
  data_list << vc.phoneNumber();
  data_list << vc.info();
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
  vc.setPhoto( stringToPixmap( m.text() ) );

  if( sl.size() >= 6 )
  {
    QString user_color = sl.at( 5 );
#if QT_VERSION >= 0x040700
    if( user_color != QString( "#000000" ) && QColor::isValidColor( user_color ) )
#else
    if( user_color != QString( "#000000" ) && QColor( user_color ).isValid() )
#endif
      u->setColor( user_color );
  }

  if( sl.size() >= 7 )
    vc.setPhoneNumber( sl.at( 6 ) );

  if( sl.size() >= 8 )
    vc.setInfo( sl.at( 7 ) );

  if( sl.size() > 8 )
    qWarning() << "VCARD message contains more data. Skip it";

  u->setVCard( vc );

  return true;
}

QStringList Protocol::workgroupsFromHelloMessage( const Message& hello_message ) const
{
  QStringList sl = hello_message.text().split( DATA_FIELD_SEPARATOR, QString::KeepEmptyParts );
  if( sl.size() < 10 )
    return QStringList();

  QString workgroups = sl.at( 9 );
  if( workgroups.isEmpty() )
    return QStringList();
  else
    return workgroups.split( ", ", QString::SkipEmptyParts );
}

bool Protocol::acceptConnectionFromWorkgroup( const Message& hello_message ) const
{
  if( Settings::instance().workgroups().isEmpty() )
    return true;

  QStringList workgroups = workgroupsFromHelloMessage( hello_message );
  if( workgroups.isEmpty() )
    return Settings::instance().workgroups().isEmpty();

  foreach( QString workgroup, workgroups )
  {
    if( Settings::instance().workgroups().contains( workgroup, Qt::CaseInsensitive ) )
      return true;
  }
  return false;
}

User Protocol::createUser( const Message& hello_message, const QHostAddress& peer_address )
{
  /* Read User Field Data */
  QStringList sl = hello_message.text().split( DATA_FIELD_SEPARATOR, QString::KeepEmptyParts );
  bool ok = false;

  if( sl.size() < 4 )
  {
    qWarning() << "HELLO message has not 4 (at least) field data but" << sl.size();
    qWarning() << "Skip this HELLO:" << sl.join( DATA_FIELD_SEPARATOR );
    return User();
  }

  int listener_port = sl.takeFirst().toInt( &ok );
  if( !ok )
  {
    qWarning() << "HELLO has an invalid Listener port";
    return User();
  }

  QString user_name = sl.takeFirst();
  /* Auth */
  if( hello_message.data().toUtf8() != Settings::instance().hash( user_name ) )
  {
    qWarning() << "HELLO message has an invalid password";
    return User();
  }

  int user_status = sl.takeFirst().toInt( &ok );
  if( !ok )
    user_status = User::Online;

  if( user_status == User::Offline )
  {
    qWarning() << "HELLO message has an user in offline status but it will be changed";
    user_status = User::Online;
  }

  QString user_status_description = sl.takeFirst();

  QString user_account_name = "";
  if( !sl.isEmpty() )
    user_account_name = sl.takeFirst();

  // skip public_key at 5
  if( !sl.isEmpty() )
    sl.takeFirst();

  QString user_version = "";
  if( !sl.isEmpty() )
    user_version = sl.takeFirst();

  QString user_session_id = "";
  if( !sl.isEmpty() )
    user_session_id = sl.takeFirst();

  QString user_color( "#000000" );
  if( !sl.isEmpty() )
    user_color = sl.takeFirst();

  // skip workgroups at 9
  if( !sl.isEmpty() )
    sl.takeFirst();

  QString user_qt_version = Settings::instance().localUser().qtVersion();
  if( !sl.isEmpty() )
    user_qt_version = sl.takeFirst();

  /* Skip other data */
  if( !sl.isEmpty() )
    qWarning() << "HELLO message contains more data. Skip it";

  /* Create User */
  User u( newId() );
  u.setName( user_name );
  u.setHostAddress( peer_address );
  u.setHostPort( listener_port );
  u.setStatus( user_status );
  u.setStatusDescription( user_status_description );
  u.setAccountName( user_account_name );
  u.setVersion( user_version );
  u.setSessionId( user_session_id );
  u.setColor( user_color );
  u.setQtVersion( user_qt_version );
  return u;
}

User Protocol::createTemporaryUser( const QString& user_path, const QString& account_name )
{
  QString user_name = User::nameFromPath( user_path );
  if( user_name.isNull() || user_name.isEmpty() )
    return User();

  QHostAddress user_address;
  int user_port = 0;

  QString host_port = user_path;
  host_port.remove( 0, user_name.size() + 1 ); // remove name and @
  bool ok = false;
  QStringList sl = host_port.split( ":" );
  if( sl.size() > 2 ) // ipv6 address
  {
    user_port = sl.last().toInt( &ok );
    if( !ok )
      return User();
    sl.removeLast();
    user_address = QHostAddress( sl.join( ":" ) );
  }
  else if( sl.size() == 2 )
  {
    user_address = QHostAddress( sl.first() );
    user_port = sl.last().toInt( &ok );
    if( !ok )
      return User();
  }
  else
    return User();

  return createTemporaryUser( user_name, account_name, user_address, user_port );
}

User Protocol::createTemporaryUser( const QString& user_name, const QString& user_account_name, const QHostAddress& user_address, int user_port )
{
  User u;
  u.setName( user_name );
  u.setHostAddress( user_address );
  u.setHostPort( user_port );
  if( !user_account_name.isEmpty() )
    u.setAccountName( user_account_name );
  u.setStatus( User::Offline );
  u.setId( newId() );
  return u;
}

QString Protocol::saveUser( const User& u ) const
{
  UserRecord ur;
  ur.setName( u.name() );
  ur.setAccount( u.accountName() );
  ur.setHostAddress( u.hostAddress() );
  ur.setHostPort( u.hostPort() );
  ur.setFavorite( u.isFavorite() );
  return saveUserRecord( ur, false );
}

User Protocol::loadUser( const QString& s )
{
  UserRecord ur = loadUserRecord( s );
  if( !ur.isValid() )
    return User();

  User u = createTemporaryUser( ur.name(), ur.account(), ur.hostAddress(), ur.hostPort() );
  u.setIsFavorite( ur.isFavorite() );

  return u;
}

QString Protocol::saveUserRecord( const UserRecord& ur, bool compact_fields ) const
{
  QStringList sl;
  sl << ur.hostAddress().toString();
  sl << QString::number( ur.hostPort() );
  if( !compact_fields )
  {
    sl << ur.comment();
    sl << ur.name();
    sl << ur.account();
    if( ur.isFavorite() )
      sl << QString( "*" );
    else
      sl << QString( "" );
  }
  return sl.join( DATA_FIELD_SEPARATOR );
}

UserRecord Protocol::loadUserRecord( const QString& s ) const
{
  QStringList sl = s.split( DATA_FIELD_SEPARATOR );
  if( sl.size() < 2 )
  {
    qWarning() << "Invalid user record found in data:" << s << "(size error)";
    return UserRecord();
  }

  UserRecord ur;
  ur.setHostAddress( QHostAddress( sl.takeFirst() ) );
  if( ur.hostAddress().isNull() )
  {
    qWarning() << "Invalid user record found in data:" << s << "(host address error)";
    return UserRecord();
  }
  bool ok = false;
  int host_port = sl.takeFirst().toInt( &ok, 10 );
  if( !ok || host_port < 1 || host_port > 65535 )
  {
    qWarning() << "Invalid user record found in data:" << s << "(host port error)";
    return UserRecord();
  }
  else
    ur.setHostPort( host_port );

  if( !sl.isEmpty() )
    ur.setComment( sl.takeFirst() );

  if( !sl.isEmpty() )
    ur.setName( sl.takeFirst() );

  if( !sl.isEmpty() )
    ur.setAccount( sl.takeFirst() );

  if( !sl.isEmpty() )
  {
    QString favorite_txt = sl.takeFirst();
    if( favorite_txt == QString( "*" ) )
    {
      qDebug() << "User" << ur.name() << "is in favorite list";
      ur.setFavorite( true );
    }
  }

  return ur;
}

QString Protocol::saveUserStatusRecord( const UserStatusRecord& usr ) const
{
  QStringList sl;
  sl << QString::number( usr.status() );
  sl << usr.statusDescription();
  return sl.join( DATA_FIELD_SEPARATOR );
}

UserStatusRecord Protocol::loadUserStatusRecord( const QString& s ) const
{
  QStringList sl = s.split( DATA_FIELD_SEPARATOR );
  if( sl.size() < 2 )
  {
    qWarning() << "Invalid user status record found in data:" << s << "(size error)";
    return UserStatusRecord();
  }

  bool ok = false;
  int user_status = 0;
  QString user_status_desc = "";

  user_status = sl.takeFirst().toInt( &ok );
  if( !ok || user_status < 0 || user_status >= User::NumStatus )
  {
    qWarning() << "Invalid user status record found in data:" << s << "(status error)";
    return UserStatusRecord();
  }

  user_status_desc = sl.takeFirst();

  UserStatusRecord usr;
  usr.setStatus( user_status );
  usr.setStatusDescription( user_status_desc );
  return usr;
}

Chat Protocol::createChat( const QList<VNumber>& user_list )
{
  Chat c;
  c.setId( newId() );
  if( user_list.size() > 1 )
    c.setPrivateId( newMd5Id() );
  foreach( VNumber user_id, user_list )
    c.addUser( user_id );
  c.addUser( ID_LOCAL_USER );
  return c;
}

Group Protocol::createGroup( const QString& group_name, const QString& group_private_id, const QList<VNumber>& user_list )
{
  Group g;
  g.setId( newId() );
  g.setName( group_name );
  g.setUsers( user_list );
  if( group_private_id.isEmpty() )
    g.setPrivateId( newMd5Id() );
  else
    g.setPrivateId( group_private_id );
  return g;
}

QString Protocol::saveGroup( const Group& g ) const
{
  QStringList sl;
  sl << g.name();
  sl << g.privateId();

  UserList ul = UserManager::instance().userList().fromUsersId( g.usersId() );
  ul.remove( Settings::instance().localUser() );
  sl << QString::number( ul.toList().size() );

  foreach( User u, ul.toList() )
  {
    sl << u.path();
    sl << u.accountName();
  }

  return sl.join( DATA_FIELD_SEPARATOR );
}

Group Protocol::loadGroup( const QString& group_data_saved )
{
  QStringList sl = group_data_saved.split( DATA_FIELD_SEPARATOR );
  if( sl.size() < 5 )
    return Group();

  Group g;
  g.setId( newId() );
  g.setName( sl.takeFirst() );
  g.setPrivateId( sl.takeFirst() );
  bool ok = false;
  int members = sl.takeFirst().toInt( &ok );
  if( !ok )
    return Group();

  UserList member_list;
  QString user_path;
  QString user_account_name;
  User user_found;

  member_list.set( Settings::instance().localUser() );

  for( int i = 0; i < members; i++ )
  {
    if( sl.size() >= 2 )
    {
      user_path = sl.takeFirst();
      user_account_name = sl.takeFirst();
      if( Settings::instance().trustSystemAccount() )
        user_found = UserManager::instance().findUserByAccountName( user_account_name );
      else
        user_found = UserManager::instance().findUserByPath( user_path );

      if( !user_found.isValid() )
        user_found = createTemporaryUser( user_path, user_account_name );

      if( user_found.isValid() )
        UserManager::instance().setUser( user_found );
      else
        return Group();

      member_list.set( user_found );
    }
  }

  g.setUsers( member_list.toUsersId() );

  return g;
}

Message Protocol::groupChatRefuseMessage( const Chat& c )
{
  Message m( Message::Group, newId(), "" );
  m.addFlag( Message::Refused );
  ChatMessageData cmd;
  cmd.setGroupId( c.privateId() );
  cmd.setGroupName( c.name() );
  m.setData( chatMessageDataToString( cmd ) );
  return m;
}

Message Protocol::groupChatRequestMessage( const Chat& c, const User& to_user )
{
  Message m( Message::Group, newId(), "" );
  m.addFlag( Message::Request );
  UserList ul = UserManager::instance().userList().fromUsersId( c.usersId() );
  ul.remove( Settings::instance().localUser() );
  ul.remove( to_user );
  QStringList sl = ul.toStringList( false, false );
  m.setText( sl.join( PROTOCOL_FIELD_SEPARATOR ) );
#ifdef BEEBEEP_DEBUG
  qDebug() << "Members to send group chat request:" << qPrintable( sl.join( ", " ) );
#endif
  ChatMessageData cmd;
  cmd.setGroupId( c.privateId() );
  cmd.setGroupName( c.name() );
  m.setData( chatMessageDataToString( cmd ) );
  return m;
}

QStringList Protocol::userPathsFromGroupRequestMessage( const Message& m ) const
{
  return m.text().split( PROTOCOL_FIELD_SEPARATOR );
}

Message Protocol::fileInfoRefusedToMessage( const FileInfo& fi )
{
  Message m( Message::File, newId(), fi.name() );
  m.addFlag( Message::Refused );
  m.addFlag( Message::Private );
  /* for backward compatibility 0.9.6 */
  QStringList sl;
  sl << QString::number( fi.hostPort() );
  sl << QString::number( fi.size() );
  sl << QString::number( fi.id() );
  sl << QString( "*" );
  m.setData( sl.join( DATA_FIELD_SEPARATOR ) );
  /* end of patch */
  return m;
}

Message Protocol::folderRefusedToMessage( const QString& folder_name )
{
  Message m( Message::Folder, newId(), folder_name );
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
  sl << fi.fileHash();
  sl << fi.shareFolder();
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
    return fi;
  fi.setHostPort( sl.takeFirst().toInt() );
  fi.setSize( Bee::qVariantToVNumber( sl.takeFirst() ) );
  fi.setId( Bee::qVariantToVNumber( sl.takeFirst() ) );
  QString password = sl.takeFirst();
  fi.setPassword( password.toUtf8() );
  if( !sl.isEmpty() )
    fi.setFileHash( sl.takeFirst() );
  else
    fi.setFileHash( fileInfoHashTmp( fi.id(), fi.name(), fi.size() ) );

  if( !sl.isEmpty() )
    fi.setShareFolder( sl.takeFirst() );

  /* Skip other data */
  if( !sl.isEmpty()  )
    qWarning() << "FILEINFO message contains more data. Skip it";

  return fi;
}

FileInfo Protocol::fileInfo( const QFileInfo& fi, const QString& share_folder )
{
  FileInfo file_info = FileInfo( newId(), FileInfo::Upload );
  file_info.setName( fi.fileName() );
  file_info.setPath( fi.absoluteFilePath() );
  file_info.setShareFolder( share_folder );

  if( fi.isFile() )
  {
    file_info.setSuffix( fi.suffix() );
    file_info.setSize( fi.size() );
  }
  else
    file_info.setIsFolder( true );

  QString password_key = QString( "%1%2%3%4%5" )
                            .arg( Random::number( 111111, 999999 ) )
                            .arg( file_info.id() )
                            .arg( Random::number( 111111, 999999 ) )
                            .arg( file_info.path() )
                            .arg( Random::number( 111111, 999999 ) );
  file_info.setPassword( Settings::instance().hash( password_key ) );
  file_info.setFileHash( fileInfoHash( fi ) );
  return file_info;
}

bool Protocol::fileCanBeShared( const QFileInfo& file_info )
{
  if( !file_info.exists() )
  {
    qWarning() << "Path" << file_info.absoluteFilePath() << "not exists and cannot be shared";
    return false;
  }

  if( file_info.isDir() && file_info.fileName().endsWith( "." ) )
  {
    // skip folder . and folder ..
    return false;
  }

  if( !file_info.isReadable() )
  {
    qWarning() << "Path" << file_info.absoluteFilePath() << "is not readable and cannot be shared";
    return false;
  }

  if( file_info.isSymLink() )
  {
    qDebug() << "Path" << file_info.absoluteFilePath() << "is a symbolic link and cannot be shared";
    return false;
  }

  if( file_info.isHidden() )
  {
    qDebug() << "Path" << file_info.absoluteFilePath() << "is hidden and cannot be shared";
    return false;
  }

  return true;
}

int Protocol::countFilesCanBeSharedInPath( const QString& file_path )
{
#ifdef BEEBEEP_DEBUG
  qDebug() << "Protocol checks file path:" << file_path;
#endif
  int num_files = 0;
  QFileInfo file_info( file_path );
  if( fileCanBeShared( file_info ) )
  {
    if( file_info.isDir() )
    {
      QDir dir( file_path );
      QStringList dir_entries = dir.entryList();
      foreach( QString dir_entry, dir_entries )
      {
        if( num_files > Settings::instance().maxQueuedDownloads() )
          break;
        dir_entry = Bee::convertToNativeFolderSeparator( QString( "%1/%2" ).arg( dir.absolutePath() ).arg( dir_entry ) );
        num_files += countFilesCanBeSharedInPath( dir_entry );
      }
    }
    else
      num_files = 1;
  }
#ifdef BEEBEEP_DEBUG
  qDebug() << "Protocol counts" << num_files << "files which can be shared";
#endif
  return num_files;
}

Message Protocol::createFolderMessage( const QString& folder_name, const QList<FileInfo>& file_info_list, int server_port )
{
  QStringList msg_list;
  foreach( FileInfo fi, file_info_list )
  {
    QStringList sl;
    sl << fi.name();
    sl << fi.suffix();
    sl << QString::number( fi.size() );
    sl << QString::number( fi.id() );
    sl << QString::fromUtf8( fi.password() );
    sl << fi.fileHash();
    sl << fi.shareFolder();
    msg_list.append( sl.join( DATA_FIELD_SEPARATOR ) );
  }

  Message m( Message::Folder, newId(), msg_list.join( PROTOCOL_FIELD_SEPARATOR ) );
  msg_list.clear();
  msg_list << QString::number( server_port );
  msg_list << folder_name;
  m.setData( msg_list.join( DATA_FIELD_SEPARATOR ) );
  m.addFlag( Message::Request );
  m.addFlag( Message::Private );

  return m;
}

QList<FileInfo> Protocol::messageFolderToInfoList( const Message& m, const QHostAddress& server_address, QString* pFolderName ) const
{
  QList<FileInfo> file_info_list;
  if( m.type() != Message::Folder )
    return file_info_list;

  QStringList sl = m.data().split( DATA_FIELD_SEPARATOR );
  if( sl.isEmpty() )
  {
    qWarning() << "Folder message received has invalid data";
    return file_info_list;
  }

  int server_port = sl.takeFirst().toInt();
  QString folder_name = sl.takeFirst();
  if( pFolderName )
    *pFolderName = folder_name;

  /* Skip other data */
  if( !sl.isEmpty() )
    qWarning() << "Folder message contains more data. Skip it";

  sl = m.text().split( PROTOCOL_FIELD_SEPARATOR, QString::SkipEmptyParts );

  QStringList::const_iterator it = sl.begin();
  while( it != sl.end() )
  {
    QStringList sl_tmp = (*it).split( DATA_FIELD_SEPARATOR, QString::SkipEmptyParts );

    if( sl_tmp.size() >= 7 )
    {
      FileInfo fi;

      fi.setTransferType( FileInfo::Download );
      fi.setHostAddress( server_address );
      fi.setHostPort( server_port );
      fi.setName( sl_tmp.takeFirst() );
      fi.setSuffix( sl_tmp.takeFirst() );
      fi.setSize( Bee::qVariantToVNumber( sl_tmp.takeFirst() ) );
      fi.setId( Bee::qVariantToVNumber( sl_tmp.takeFirst() ) );
      fi.setPassword( sl_tmp.takeFirst().toUtf8() );
      fi.setFileHash( sl_tmp.takeFirst() );
      fi.setShareFolder( Bee::convertToNativeFolderSeparator( sl_tmp.takeFirst() ) );

      file_info_list.append( fi );
    }

    ++it;
  }

  return file_info_list;
}

void Protocol::createFileShareListMessage( const QMultiMap<QString, FileInfo>& file_info_list, int server_port )
{
  if( server_port <= 0 )
  {
    m_fileShareListMessage = "";
    return;
  }

  QStringList msg_list;
  QMultiMap<QString, FileInfo>::const_iterator it = file_info_list.begin();
  while( it != file_info_list.end() )
  {
    QStringList sl;
    sl << it.value().name();
    sl << it.value().suffix();
    sl << QString::number( it.value().size() );
    sl << QString::number( it.value().id() );
    sl << QString::fromUtf8( it.value().password() );
    sl << it.value().fileHash();
    sl << it.value().shareFolder();
    msg_list.append( sl.join( DATA_FIELD_SEPARATOR ) );
    ++it;
  }

  Message m( Message::Share, ID_SHARE_MESSAGE, msg_list.isEmpty() ? QString( "" ) : msg_list.join( PROTOCOL_FIELD_SEPARATOR ) );
  m.setData( msg_list.isEmpty() ? QString( "0" ) : QString::number( server_port ) );
  m.addFlag( Message::List );

  m_fileShareListMessage = fromMessage( m );
}

QList<FileInfo> Protocol::messageToFileShare( const Message& m, const QHostAddress& server_address ) const
{
  QList<FileInfo> file_info_list;
  if( m.type() != Message::Share )
    return file_info_list;

  QStringList sl = m.data().split( DATA_FIELD_SEPARATOR );
  if( sl.isEmpty() )
    return file_info_list;

  int server_port = sl.takeFirst().toInt();
  /* Skip other data */
  if( !sl.isEmpty() )
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
      fi.setName( sl_tmp.takeFirst() );
      fi.setSuffix( sl_tmp.takeFirst() );
      fi.setSize( Bee::qVariantToVNumber( sl_tmp.takeFirst() ) );
      fi.setId( Bee::qVariantToVNumber( sl_tmp.takeFirst() ) );
      fi.setPassword( sl_tmp.takeFirst().toUtf8() );
      if( !sl_tmp.isEmpty() )
        fi.setFileHash( sl_tmp.takeFirst() );
      else
        fi.setFileHash( fileInfoHashTmp( fi.id(), fi.name(), fi.size() ) );

      if( !sl_tmp.isEmpty() )
        fi.setShareFolder( Bee::convertToNativeFolderSeparator( sl_tmp.takeFirst() ) );

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
  if( sl.isEmpty() )
    return cmd;

  if( !sl.first().isEmpty() )
  {
    QColor c( sl.first() );
    if( !c.isValid() )
    {
      qWarning() << "Invalid text color in Chat Message Data:" << m.data();
      cmd.setTextColor( QColor( Qt::black ) );
    }
    else
      cmd.setTextColor( c );
  }

  sl.removeFirst();
  if( sl.isEmpty() )
    return cmd;
  else
    cmd.setGroupId( sl.takeFirst() );

  if( sl.isEmpty() )
    return cmd;
  else
    cmd.setGroupName( sl.takeFirst() );

  return cmd;
}

QString Protocol::chatMessageDataToString( const ChatMessageData& cmd )
{
  QStringList sl;
  sl << (cmd.textColor().isValid() ? cmd.textColor().name() : "");
  sl << (cmd.groupId().size() > 0 ? cmd.groupId() : "");
  sl << (cmd.groupName().size() > 0 ? cmd.groupName() : "");
  return sl.join( DATA_FIELD_SEPARATOR );
}

Message Protocol::chatMessage( const Chat& c, const QString& msg_txt )
{
  Message m( Message::Chat, newId(), msg_txt );
  ChatMessageData cmd;
  cmd.setTextColor( Settings::instance().chatFontColor() );
  if( c.isGroup() )
  {
    m.addFlag( Message::GroupChat );
    cmd.setGroupId( c.privateId() );
  }
  else
  {
    if( !c.isDefault() )
      m.addFlag( Message::Private );
  }

  m.setData( Protocol::instance().chatMessageDataToString( cmd ) );
  return m;
}

Message Protocol::chatReadMessage( const Chat& c )
{
  Message m = chatMessage( c, "" );
  m.setType( Message::Read );
  return m;
}

QString Protocol::fileInfoHash( const QFileInfo& file_info ) const
{
  QStringList sl;
  sl << file_info.fileName();
  sl << QString::number( file_info.size() );
  sl << file_info.lastModified().toString( "dd.MM.yyyy-hh:mm:ss" );

  QCryptographicHash ch( QCryptographicHash::Sha1 );
  ch.addData( sl.join( "-" ).toUtf8() );
  return QString::fromLatin1( ch.result().toHex() );
}

QString Protocol::fileInfoHashTmp( VNumber file_info_id, const QString& file_info_name, FileSizeType file_info_size ) const
{
  QStringList sl;
  sl << QString::number( file_info_id );
  sl << file_info_name;
  sl << QString::number( file_info_size );

  QCryptographicHash ch( QCryptographicHash::Sha1 );
  ch.addData( sl.join( "-" ).toUtf8() );
  return QString::fromLatin1( ch.result().toHex() );
}

QString Protocol::newMd5Id()
{
  QStringList sl;
  sl << QString::number( newId() );
  sl << QString::number( Random::d100() );
  sl << Settings::instance().localUser().path();
  sl << QString::number( Random::d100() );
  sl << QDateTime::currentDateTime().toString( "dd.MM.yyyy-hh:mm:ss.zzz" );
  sl << QString::number( Random::d100() );

  QCryptographicHash ch( QCryptographicHash::Sha1 );
  ch.addData( sl.join( "=" ).toUtf8() );
  return QString::fromLatin1( ch.result().toHex() );
}

QByteArray Protocol::bytesArrivedConfirmation( int num_bytes ) const
{
  QByteArray byte_array = QByteArray::number( num_bytes );
  while( byte_array.size() % ENCRYPTED_DATA_BLOCK_SIZE )
    byte_array.prepend( '0' );
  return byte_array;
}

Message Protocol::userRecordListToMessage( const QList<UserRecord>& user_record_list )
{
  QStringList msg_list;
  foreach( UserRecord ur, user_record_list )
    msg_list.append( saveUserRecord( ur, true ) );

  Message m( Message::Hive, newId(), msg_list.join( PROTOCOL_FIELD_SEPARATOR ) );
  m.addFlag( Message::List );
  return m;
}

QList<UserRecord> Protocol::messageToUserRecordList( const Message& m ) const
{
  QList<UserRecord> user_record_list;
  if( m.type() != Message::Hive && !m.hasFlag( Message::List ) )
    return user_record_list;

  QStringList sl = m.text().split( PROTOCOL_FIELD_SEPARATOR, QString::SkipEmptyParts );
  UserRecord ur;
  foreach( QString s, sl )
  {
    ur = loadUserRecord( s );
    if( ur.isValid() )
      user_record_list.append( ur );
  }

  return user_record_list;
}

QString Protocol::linkifyText( QString text )
{
  if( !text.contains( QLatin1Char( '.' ) ) )
    return text;
  text.prepend( " " ); // for matching www.miosito.it
  text.replace( QRegExp( "(((f|ht){1}tp(s:|:){1}//)[-a-zA-Z0-9@:%_\\+.,~#?&//=\\(\\)]+)" ), "<a href=\"\\1\">\\1</a>" );
  text.replace( QRegExp( "([\\s()[{}])(www.[-a-zA-Z0-9@:%_\\+.,~#?&//=\\(\\)]+)" ), "\\1<a href=\"http://\\2\">\\2</a>" );
  text.replace( QRegExp( "([_\\.0-9a-z-]+@([0-9a-z][0-9a-z-]+\\.)+[a-z]{2,3})" ), "<a href=\"mailto:\\1\">\\1</a>" );
  text.remove( 0, 1 ); // remove the space added
  return text;
}

QString Protocol::formatHtmlText( const QString& text )
{
  QString text_formatted = "";
  int last_semicolon_index = -1;

  for( int i = 0; i < text.length(); i++ )
  {
    if( text.at( i ) == QLatin1Char( ' ' ) )
    {
      if( (i + 1) < text.length() && text.at( (i+1) ) == QLatin1Char( ' ' ) )
        text_formatted += QLatin1String( "&nbsp;" );
      else
        text_formatted += QLatin1Char( ' ' );
    }
    else if( text.at( i ) == QLatin1Char( '\n' ) )
    {
      // space added to match url after a \n
      text_formatted += QLatin1String( "<br /> " );
    }
    else if( text.at( i ) == QLatin1Char( '<' ) )
    {
      if( Settings::instance().chatUseHtmlTags() )
      {
        if( last_semicolon_index >= 0 )
          text_formatted.replace( last_semicolon_index, 1, QLatin1String( "&lt;" ) );

        last_semicolon_index = text_formatted.size();
        text_formatted += QLatin1Char( '<' );
      }
      else
        text_formatted += QLatin1String( "&lt;" );
    }
    else if( text.at( i ) == QLatin1Char( '>' ) )
    {
      if( Settings::instance().chatUseHtmlTags() )
      {
        text_formatted += QLatin1Char( '>' );
        if( last_semicolon_index >= 0 )
          last_semicolon_index = -1;
      }
      else
        text_formatted += QLatin1String( "&gt;" );
    }
    else if( text.at( i ) == QLatin1Char( '"' ) )
    {
      if( last_semicolon_index >= 0 )
        text_formatted += QLatin1Char( '"' );
      else
        text_formatted += QLatin1String( "&quot;" );
    }
    else if( text.at( i ) == QLatin1Char( '&' ) )
    {
      text_formatted += QLatin1Char( '&' ); // not &amp; for Linkify
    }
    else
      text_formatted += text.at( i );
  }

  if( last_semicolon_index >= 0 )
    text_formatted.replace( last_semicolon_index, 1, QLatin1String( "&lt;" ) );

  text_formatted.replace( QRegExp("(^|\\s|>)_(\\S+)_(<|\\s|$)"), "\\1<u>\\2</u>\\3" );
  text_formatted.replace( QRegExp("(^|\\s|>)\\*(\\S+)\\*(<|\\s|$)"), "\\1<b>\\2</b>\\3" );
  text_formatted.replace( QRegExp("(^|\\s|>)\\/(\\S+)\\/(<|\\s|$)"), "\\1<i>\\2</i>\\3" );

  if( Settings::instance().chatUseClickableLinks() )
    text_formatted = linkifyText( text_formatted );

  if( Settings::instance().showEmoticons() )
    text_formatted = EmoticonManager::instance().parseEmoticons( text_formatted, Settings::instance().emoticonSizeInChat(),
                                                                 Settings::instance().useNativeEmoticons() );

  PluginManager::instance().parseText( &text_formatted, false );

  return text_formatted;
}

/* Encryption */
QByteArray Protocol::createCipherKey( const QString& public_key_1, const QString& public_key_2 ) const
{
  QString public_key = public_key_1 + public_key_2;
  QCryptographicHash ch( QCryptographicHash::Sha1 );
  ch.addData( public_key.toUtf8() );
  return ch.result().toHex();
}

QList<QByteArray> Protocol::splitByteArray( const QByteArray& byte_array, int num_chars ) const
{
  QList<QByteArray> array_list;

  if( byte_array.isEmpty() )
    return array_list;

  QByteArray tmp = "";

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
#ifdef BEEBEEP_DEBUG
    qDebug() << "Protocol splits byte array in" << array_list.size() << "parts but some chars remains out:" << tmp;
#endif
    array_list.append( tmp );
  }

  return array_list;
}

QByteArray Protocol::encryptByteArray( const QByteArray& text_to_encrypt, const QByteArray& cipher_key ) const
{
  unsigned long rk[ RKLENGTH(ENCRYPTION_KEYBITS) ];
  unsigned char key[ KEYLENGTH(ENCRYPTION_KEYBITS) ];
  unsigned int i;
  int nrounds;

  if( text_to_encrypt.isEmpty() )
    return QByteArray();

  if( cipher_key.isEmpty() )
  {
    qWarning() << "Unable to encrypt data with an empty cipher key";
    return text_to_encrypt;
  }

  for( i = 0; i < sizeof( key ); i++ )
  {
    key[ i ] = (unsigned int)cipher_key.size() < i ? static_cast<unsigned char>( cipher_key.at( i ) ) : 0;
  }

  nrounds = rijndaelSetupEncrypt( rk, key, ENCRYPTION_KEYBITS );

  QList<QByteArray> byte_array_list = splitByteArray( text_to_encrypt, ENCRYPTED_DATA_BLOCK_SIZE );

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

QByteArray Protocol::decryptByteArray( const QByteArray& text_to_decrypt, const QByteArray& cipher_key ) const
{
  unsigned long rk[RKLENGTH(ENCRYPTION_KEYBITS)];
  unsigned char key[KEYLENGTH(ENCRYPTION_KEYBITS)];
  unsigned int i;
  int nrounds;

  if( text_to_decrypt.isEmpty() )
    return QByteArray();

  if( cipher_key.isEmpty() )
  {
    qWarning() << "Unable to decrypt data with an empty cipher key";
    return text_to_decrypt;
  }

  for( i = 0; i < sizeof( key ); i++ )
  {
    key[ i ] = (unsigned int)cipher_key.size() < i ? static_cast<unsigned char>( cipher_key.at( i ) ) : 0;
  }

  nrounds = rijndaelSetupDecrypt( rk, key, ENCRYPTION_KEYBITS );

  QList<QByteArray> byte_array_list = splitByteArray( text_to_decrypt, ENCRYPTED_DATA_BLOCK_SIZE );

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
