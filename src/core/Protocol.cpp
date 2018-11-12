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
#include "ColorManager.h"
#include "EmoticonManager.h"
#include "PluginManager.h"
#include "Protocol.h"
#include "Random.h"
#include "Rijndael.h"
#include "Settings.h"
#include "UserManager.h"

Protocol* Protocol::mp_instance = Q_NULLPTR;
const QChar PROTOCOL_FIELD_SEPARATOR = QChar::ParagraphSeparator;  // 0x2029
const QChar DATA_FIELD_SEPARATOR = QChar::LineSeparator; // 0x2028


Protocol::Protocol()
  : m_id( ID_START ), m_fileShareListMessage( Message::Share, ID_SHARE_MESSAGE, "" )
{
  m_id += static_cast<VNumber>(Random::d100());
#if QT_VERSION == 0x050603 && defined Q_OS_MAC
  // Fixed a protocol bug in OsX Legacy version
  m_datastreamMaxVersion = 12;
#else
  QDataStream ds;
  m_datastreamMaxVersion = ds.version();
#endif
  qDebug() << "Protocol has detected latest datastream version:" << m_datastreamMaxVersion;
}

QString Protocol::messageHeader( Message::Type mt ) const
{
  switch( mt )
  {
  case Message::Beep:     return "BEE-BEEP";
  case Message::Ping:     return "BEE-PING";
  case Message::Pong:     return "BEE-PONG";
  case Message::Chat:     return "BEE-CHAT";
  case Message::Buzz:     return "BEE-BUZZ";
  case Message::Hello:    return "BEE-CIAO";
  case Message::System:   return "BEE-SYST";
  case Message::User:     return "BEE-USER";
  case Message::File:     return "BEE-FILE";
  case Message::Share:    return "BEE-FSHR";
  case Message::Group:    return "BEE-GROU";
  case Message::Folder:   return "BEE-FOLD";
  case Message::Read:     return "BEE-READ";
  case Message::Hive:     return "BEE-HIVE";
  case Message::ShareBox: return "BEE-SBOX";
  case Message::ShareDesktop : return "BEE-DESK";
  default:                return "BEE-BOOH";
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
  else if( msg_type == "BEE-BUZZ" )
    return Message::Buzz;
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
  else if( msg_type == "BEE-SBOX" )
    return Message::ShareBox;
  else if( msg_type == "BEE-HIVE" )
    return Message::Hive;
  else if( msg_type == "BEE-DESK" )
    return Message::ShareDesktop;
  else
    return Message::Undefined;
}

QByteArray Protocol::fromMessage( const Message& m, int proto_version ) const
{
  if( !m.isValid() )
    return "";
  QStringList sl;
  sl << messageHeader( m.type() );
  sl << QString::number( m.id() );
  sl << QString::number( m.text().size() );
  sl << QString::number( m.flags() );
  sl << m.data();
  if( proto_version < UTC_TIMESTAMP_PROTO_VERSION )
    sl << m.timestamp().toString( Qt::ISODate );
  else
    sl << m.timestamp().toUTC().toString( Qt::ISODate );
  sl << m.text();
  QByteArray byte_array = sl.join( PROTOCOL_FIELD_SEPARATOR ).toUtf8();
  while( byte_array.size() % ENCRYPTED_DATA_BLOCK_SIZE )
    byte_array.append( ' ' );
  return byte_array;
}

Message Protocol::toMessage( const QByteArray& byte_array_data, int proto_version ) const
{
  QString message_data = QString::fromUtf8( byte_array_data );
  Message m;
  QStringList sl = message_data.split( PROTOCOL_FIELD_SEPARATOR, QString::KeepEmptyParts );
  if( sl.size() < 7 )
  {
    qWarning() << "Invalid number of fields in message:" << message_data.simplified();
    return m;
  }

  m.setType( messageType( sl.takeFirst() ) );
  if( !m.isValid() )
  {
    qWarning() << "Invalid message type:" << message_data.simplified();
    return m;
  }

  VNumber msg_id = Bee::qVariantToVNumber( sl.takeFirst() );
  if( msg_id == ID_INVALID )
  {
    qWarning() << "Invalid message id:" << message_data.simplified();
    m.setType( Message::Undefined );
    return m;
  }
  m.setId( msg_id );

  bool ok = false;
  int msg_size = sl.takeFirst().toInt( &ok );
  if( !ok )
  {
    qWarning() << "Invalid message size:" << message_data.simplified();
    m.setType( Message::Undefined );
    return m;
  }

  int msg_flags = sl.takeFirst().toInt( &ok );
  if( !ok )
  {
    qWarning() << "Invalid message flags:" << message_data.simplified();
    m.setType( Message::Undefined );
    return m;
  }
  m.setFlags( msg_flags );

  m.setData( sl.takeFirst() );

  QDateTime dt_timestamp = QDateTime::fromString( sl.takeFirst(), Qt::ISODate );
  if( !dt_timestamp.isValid() )
  {
    qWarning() << "Invalid message timestamp:" << message_data.simplified();
    m.setType( Message::Undefined );
    return m;
  }
  else
  {
    if( proto_version < UTC_TIMESTAMP_PROTO_VERSION )
    {
      m.setTimestamp( dt_timestamp.toLocalTime() );
    }
    else
    {
      dt_timestamp.setTimeSpec( Qt::UTC );
      m.setTimestamp( dt_timestamp.toLocalTime() );
    }
  }

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
  return fromMessage( m, 1 );
}

QByteArray Protocol::pongMessage() const
{
  Message m( Message::Pong, ID_PONG_MESSAGE, "*" );
  return fromMessage( m, 1 );
}

QByteArray Protocol::broadcastMessage( const QHostAddress& to_host_address ) const
{
  Message m( Message::Beep, ID_BEEP_MESSAGE, QString::number( Settings::instance().localUser().networkAddress().hostPort() ) );
  QStringList sl;
  sl << to_host_address.toString();
  m.setData( sl.join( DATA_FIELD_SEPARATOR ) );
  return fromMessage( m, 1 );
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
  int proto_version = static_cast<int>( m.id() <= ID_HELLO_MESSAGE ? 1 : m.id() );
  return proto_version;
}

QString Protocol::publicKey( const Message& m ) const
{
  QStringList data_list = m.text().split( DATA_FIELD_SEPARATOR );
  return data_list.size() >= 6 ? data_list.at( 5 ) : QString();
}

int Protocol::datastreamVersion( const Message& m ) const
{
  QStringList data_list = m.text().split( DATA_FIELD_SEPARATOR );

  int datastream_version = 0;
  if( data_list.size() >= 12 )
  {
    bool ok = false;
    datastream_version = data_list.at( 11 ).toInt( &ok );
    if( !ok )
      datastream_version = 0;
  }

  return datastream_version;
}

QByteArray Protocol::helloMessage( const QString& public_key ) const
{
  QStringList data_list;
  data_list << QString::number( Settings::instance().localUser().networkAddress().hostPort() );
  data_list << Settings::instance().localUser().name();
  data_list << QString::number( Settings::instance().localUser().status() );
  data_list << Settings::instance().localUser().statusDescription();
  data_list << Settings::instance().localUser().accountName();
  data_list << public_key;
  data_list << Settings::instance().version( false, false );
  data_list << Settings::instance().localUser().hash();
  data_list << Settings::instance().localUser().color();
  if( Settings::instance().workgroups().isEmpty() )
    data_list << QString( "" );
  else
    data_list << Settings::instance().workgroups().join( ", " );
  data_list << Settings::instance().localUser().qtVersion();
  data_list << QString::number( m_datastreamMaxVersion );
  if( Settings::instance().localUser().statusChangedIn().isValid() )
    data_list << Settings::instance().localUser().statusChangedIn().toString( Qt::ISODate );
  else
    data_list << QString( "" );
  data_list << Settings::instance().localUser().domainName();
  Message m( Message::Hello, static_cast<VNumber>(Settings::instance().protoVersion()), data_list.join( DATA_FIELD_SEPARATOR ) );
  m.setData( Settings::instance().currentHash() );
  return fromMessage( m, 1 );
}

Message Protocol::writingMessage( const QString& chat_private_id ) const
{
  Message writing_message( Message::User, ID_WRITING_MESSAGE, "*" );
  writing_message.addFlag( Message::Private );
  writing_message.addFlag( Message::UserWriting );
  if( !chat_private_id.isEmpty() )
    writing_message.setData( chat_private_id );
  return writing_message;
}

Message Protocol::userStatusMessage( int user_status, const QString& user_status_description ) const
{
  Message m( Message::User, ID_USER_MESSAGE, user_status_description );
  m.addFlag( Message::UserStatus );
  m.setData( QString::number( user_status ) );
  return m;
}

bool Protocol::changeUserStatusFromMessage( User* u, const Message& m ) const
{
  int user_status = m.data().toInt();
  QString user_status_description = m.text();
  bool status_changed = false;
  if( u->status() != user_status  )
  {
    status_changed = true;
    u->setStatus( user_status );
    u->setStatusChangedIn( QDateTime::currentDateTime() );
  }

  if( u->statusDescription() != user_status_description )
  {
    status_changed = true;
    u->setStatusDescription( user_status_description );
  }

  return status_changed;
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

Message Protocol::localVCardMessage() const
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
  return m;
}

bool Protocol::changeVCardFromMessage( User* u, const Message& m ) const
{
  QStringList sl = m.data().split( DATA_FIELD_SEPARATOR );
  if( sl.size() < 5 )
    return false;

  VCard vc;
  vc.setNickName( sl.takeFirst() );
  vc.setFirstName( sl.takeFirst() );
  vc.setLastName( sl.takeFirst() );
  vc.setBirthday( QDate::fromString( sl.takeFirst(), Qt::ISODate ) );
  vc.setEmail( sl.takeFirst() );
  vc.setPhoto( stringToPixmap( m.text() ) );

  if( !sl.isEmpty() )
  {
    QString user_color = sl.takeFirst();
    if( ColorManager::instance().isValidColor( user_color ) )
    {
      u->setColor( user_color );
      ColorManager::instance().setColorSelected( user_color );
    }
  }

  if( !sl.isEmpty() )
    vc.setPhoneNumber( sl.takeFirst() );

  if( !sl.isEmpty()  )
    vc.setInfo( sl.takeFirst() );

  u->setVCard( vc );

  return true;
}

QStringList Protocol::workgroupsFromHelloMessage( const Message& hello_message ) const
{
  QStringList sl = hello_message.text().split( DATA_FIELD_SEPARATOR );
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

User Protocol::recognizeUser( const User& u, int user_recognition_method ) const
{
  User user_found;
  if( user_recognition_method == Settings::RecognizeByAccountAndDomain )
  {
    user_found = UserManager::instance().findUserByAccountNameAndDomainName( u.accountName(), u.domainName() );
    if( !user_found.isValid() )
      qDebug() << "User not found in list with account path" << qPrintable( u.accountPath() );
  }
  else if( user_recognition_method == Settings::RecognizeByAccount )
  {
    user_found = UserManager::instance().findUserByAccountName( u.accountName() );
    if( !user_found.isValid() )
      qDebug() << "User not found in list with account name" << qPrintable( u.accountName() );
  }
  else if( user_recognition_method == Settings::RecognizeByNickname )
  {
    user_found = UserManager::instance().findUserByNickname( u.name() );
    if( !user_found.isValid() )
    {
      user_found = UserManager::instance().findUserByHash( u.hash() );
      if( !user_found.isValid() )
        qDebug() << "User not found in list with nickname" << qPrintable( u.name() ) << "and hash" << qPrintable( u.hash() );
    }
  }
  else
    qWarning() << "Invalid user recognition method found" << user_recognition_method;

  return user_found;
}

User Protocol::recognizeUser( const UserRecord& ur, int user_recognition_method ) const
{
  User user_tmp( ID_INVALID, ur );
  return recognizeUser( user_tmp, user_recognition_method );
}

User Protocol::createUser( const Message& hello_message, const QHostAddress& peer_address )
{
  /* Read User Field Data */
  QStringList sl = hello_message.text().split( DATA_FIELD_SEPARATOR );
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

  QString user_hash = "";
  if( !sl.isEmpty() )
    user_hash = sl.takeFirst();

  QString user_color( "#000000" );
  if( !sl.isEmpty() )
    user_color = sl.takeFirst();

  /* skip workgroups */
  if( !sl.isEmpty() )
    sl.takeFirst();

  QString user_qt_version = Settings::instance().localUser().qtVersion();
  if( !sl.isEmpty() )
    user_qt_version = sl.takeFirst();

  /* Skip datastream version */
  if( !sl.isEmpty() )
    sl.takeFirst();

  QDateTime status_changed_in = QDateTime::currentDateTime();
  if( !sl.isEmpty() )
  {
    /* User status changed in datetime */
    QString s_datetime = sl.takeFirst();
    if( !s_datetime.isEmpty() )
      status_changed_in = QDateTime::fromString( s_datetime, Qt::ISODate );
  }

  QString user_domain_name = "";
  if( !sl.isEmpty() )
    user_domain_name = sl.takeFirst();

  /* Create User */
  User u( newId() );
  u.setName( user_name );
  u.setNetworkAddress( NetworkAddress( peer_address, listener_port ) );
  u.setStatus( user_status );
  u.setStatusChangedIn( status_changed_in );
  u.setStatusDescription( user_status_description );
  u.setAccountName( user_account_name.isEmpty() ? user_name : user_account_name );
  u.setDomainName( user_domain_name );
  u.setVersion( user_version );
  u.setHash( user_hash.isEmpty() ? newMd5Id() : user_hash );
  u.setColor( user_color );
  u.setQtVersion( user_qt_version );
  return u;
}

User Protocol::createTemporaryUser( const UserRecord& ur )
{
  User u( newId(), ur );
  if( !ur.networkAddressIsValid() )
    u.setNetworkAddress( NetworkAddress( QHostAddress::LocalHost, DEFAULT_LISTENER_PORT ) );
  if( ur.account().isEmpty() )
    u.setAccountName( u.name().toLower() );
  if( ur.hash().isEmpty() )
    u.setHash( newMd5Id() );
  u.setStatus( User::Offline );
  u.setIsFavorite( ur.isFavorite() );
  if( ur.birthday().isValid() )
  {
    VCard vc = u.vCard();
    vc.setBirthday( ur.birthday() );
    u.setVCard( vc );
  }
#ifdef BEEBEEP_DEBUG
  qDebug() << "Temporary user" << u.id() << qPrintable( u.path() ) << "created with account" << qPrintable( u.accountName() ) << "and hash" << qPrintable( u.hash() );
#endif
  return u;
}

QString Protocol::saveUserRecord( const UserRecord& ur, bool add_extras ) const
{
  QStringList sl;
  sl << ur.networkAddress().hostAddress().toString();
  sl << QString::number( ur.networkAddress().hostPort() );
  if( add_extras )
  {
    sl << ur.networkAddress().info();
    sl << ur.name();
    sl << ur.account();
    if( ur.isFavorite() )
      sl << QString( "*" );
    else
      sl << QString( "" );
    sl << ur.color();
    sl << ur.hash();
    sl << ur.domainName();
    sl << ur.lastConnection().toString( Qt::ISODate );
    sl << ur.birthday().toString( Qt::ISODate );
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

  QHostAddress user_host_address = QHostAddress( sl.takeFirst() );
  if( user_host_address.isNull() )
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

  NetworkAddress na( user_host_address, host_port );

  if( !sl.isEmpty() )
    na.setInfo( sl.takeFirst() );

  ur.setNetworkAddress( na );

  if( !sl.isEmpty() )
    ur.setName( sl.takeFirst() );

  if( !sl.isEmpty() )
    ur.setAccount( sl.takeFirst() );

  if( !sl.isEmpty() )
  {
    QString favorite_txt = sl.takeFirst();
    if( favorite_txt == QString( "*" ) )
    {
      qDebug() << "User" << qPrintable( ur.name() ) << "is in favorite list";
      ur.setFavorite( true );
    }
  }

  if( !sl.isEmpty() )
  {
    ur.setColor( sl.takeFirst() );
    qDebug() << "User" << qPrintable( ur.name() ) << "has color saved:" << qPrintable( ur.color() );
  }

  if( !sl.isEmpty() )
  {
    ur.setHash( sl.takeFirst() );
 #ifdef BEEBEEP_DEBUG
    qDebug() << "User" << qPrintable( ur.name() ) << "has hash saved:" << qPrintable( ur.hash() );
 #endif
  }

  if( !sl.isEmpty() )
  {
    ur.setDomainName( sl.takeFirst() );
    qDebug() << "User" << qPrintable( ur.name() ) << "has domain name saved:" << qPrintable( ur.domainName() );
  }

  if( !sl.isEmpty() )
  {
    ur.setLastConnection( QDateTime::fromString( sl.takeFirst(), Qt::ISODate ) );
    qDebug() << "User" << qPrintable( ur.name() ) << "has last connection date saved:" << qPrintable( ur.lastConnection().toString( Qt::ISODate ) );
  }

  if( !sl.isEmpty() )
  {
    ur.setBirthday( QDate::fromString( sl.takeFirst(), Qt::ISODate ) );
    qDebug() << "User" << qPrintable( ur.name() ) << "has birthday saved:" << qPrintable( ur.birthday().toString( Qt::ISODate ) );
  }

  return ur;
}

QString Protocol::saveUser( const User& u ) const
{
  UserRecord ur;
  ur.setName( u.name() );
  ur.setAccount( u.accountName() );
  ur.setNetworkAddress( u.networkAddress() );
  ur.setFavorite( u.isFavorite() );
  ur.setColor( u.color() );
  ur.setHash( u.hash() );
  ur.setDomainName( u.domainName() );
  if( u.lastConnection().isValid() )
    ur.setLastConnection( u.lastConnection() );
  else
    ur.setLastConnection( QDateTime::currentDateTime() );
  ur.setBirthday( u.vCard().birthday() );
  return saveUserRecord( ur, true );
}

User Protocol::loadUser( const QString& s )
{
  UserRecord ur = loadUserRecord( s );
  if( ur.name().isEmpty() || !ur.networkAddressIsValid() )
    return User();

  User u = createTemporaryUser( ur );
  if( !u.lastConnection().isValid() )
    u.setLastConnection( QDateTime::currentDateTime() );
  if( ColorManager::instance().isValidColor( ur.color() ) )
    ColorManager::instance().setColorSelected( ur.color() );
  return u;
}

QString Protocol::saveNetworkAddress( const NetworkAddress& na ) const
{
  QStringList sl;
  sl << na.hostAddress().toString();
  sl << QString::number( na.hostPort() );
  sl << na.info();
  return sl.join( DATA_FIELD_SEPARATOR );
}

NetworkAddress Protocol::loadNetworkAddress( const QString& s ) const
{
  QStringList sl = s.split( DATA_FIELD_SEPARATOR );
  if( sl.size() < 2 )
  {
    qWarning() << "Invalid network address found in data:" << s << "(size error)";
    return NetworkAddress();
  }

  QHostAddress user_host_address = QHostAddress( sl.takeFirst() );
  if( user_host_address.isNull() )
  {
    qWarning() << "Invalid network address found in data:" << s << "(host address error)";
    return NetworkAddress();
  }

  bool ok = false;
  int host_port = sl.takeFirst().toInt( &ok, 10 );
  if( !ok || host_port < 1 || host_port > 65535 )
  {
    qWarning() << "Invalid network address found in data:" << s << "(host port error)";
    return NetworkAddress();
  }

  NetworkAddress na( user_host_address, host_port );
  if( !sl.isEmpty() )
    na.setInfo( sl.takeFirst() );

  return na;
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

Chat Protocol::createDefaultChat()
{
  Group g;
  g.setId( ID_DEFAULT_CHAT );
  g.setName( Settings::instance().defaultChatName() );
  g.setPrivateId( Settings::instance().defaultChatPrivateId() );
  return createChat( g, Group::DefaultChat );
}

Chat Protocol::createPrivateChat( const User& u )
{
  Group g;
  g.setName( u.name() );
  g.addUser( u.id() );
  return createChat( g, Group::PrivateChat );
}

Chat Protocol::createChat( const Group& g, Group::ChatType chat_type )
{
  Group g_to_check = g;
  if( g_to_check.chatType() != chat_type )
    g_to_check.setChatType( chat_type );
  if( !g_to_check.isValid() )
    g_to_check.setId( newId() );
  if( g_to_check.chatType() == Group::GroupChat && g_to_check.privateId().isEmpty() )
    g_to_check.setPrivateId( newMd5Id() );
  g_to_check.addUser( ID_LOCAL_USER );
  Chat c;
  c.setGroup( g_to_check );
  return c;
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
    sl << u.name();
    sl << u.accountName();
    sl << u.hash();
    sl << u.domainName();
    sl << ""; // for future use
  }

  sl << g.lastModified().toString( Qt::ISODate );

  return sl.join( DATA_FIELD_SEPARATOR );
}

Group Protocol::loadGroup( const QString& group_data_saved )
{
  QStringList sl = group_data_saved.split( DATA_FIELD_SEPARATOR, QString::KeepEmptyParts );
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
  QString user_nickname = "";
  QString user_account_name = "";
  QString user_hash = "";
  QString user_domain = "";

  member_list.set( Settings::instance().localUser() );

  bool read_user_extra_fields = sl.size() > (members*2);

  for( int i = 0; i < members; i++ )
  {
    if( sl.size() >= 2 )
    {
      user_nickname = User::nameFromPath( sl.takeFirst() );
      user_account_name = sl.takeFirst();
      user_hash = "";
      user_domain = "";

      if( read_user_extra_fields )
      {
        if( !sl.isEmpty() )
          user_hash = sl.takeFirst();

        if( !sl.isEmpty() )
          user_domain = sl.takeFirst();

        if( !sl.isEmpty() )
          sl.takeFirst(); // for future use
      }

      UserRecord ur( user_nickname, user_account_name, user_hash, user_domain );
      ur.setDomainName( user_domain );
      User u = recognizeUser( ur, Settings::instance().userRecognitionMethod() );

      if( !u.isValid() )
      {
        u = createTemporaryUser( ur );
        qDebug() << "Group chat" << qPrintable( g.name() ) << "has created temporary user" << u.id() << qPrintable( u.name() );
        UserManager::instance().setUser( u );
      }

      member_list.set( u );
    }
  }

  g.setUsers( member_list.toUsersId() );

  if( !sl.isEmpty() )
  {
    g.setLastModified( QDateTime::fromString( sl.takeFirst(), Qt::ISODate ) );
#ifdef BEEBEEP_DEBUG
    qDebug() << "Group chat" << qPrintable( g.name() ) << "has last modified field" << qPrintable( g.lastModified().toString( Qt::ISODate ) );
#endif
  }

  g.setChatType( Group::GroupChat );
  return g;
}

Message Protocol::groupChatRefuseMessage( const Chat& c )
{
  ChatMessageData cmd;
  cmd.setGroupId( c.privateId() );
  cmd.setGroupName( c.name() );
  return groupChatRefuseMessage( cmd );
}

Message Protocol::groupChatRefuseMessage( const ChatMessageData& cmd )
{
  Message m( Message::Group, newId(), "" );
  m.addFlag( Message::Refused );
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

  QStringList sl;
  sl << QString::number( ul.size() );
  foreach( User u, ul.toList() )
  {
    sl << u.name();
    sl << u.accountName();
    sl << u.hash();
    sl << u.domainName();
  }

  if( ul.size() >= 1 )
    m.setText( sl.join( PROTOCOL_FIELD_SEPARATOR ) );

  ChatMessageData cmd;
  cmd.setGroupId( c.privateId() );
  cmd.setGroupName( c.name() );
  cmd.setGroupLastModified( c.lastModified() );
  m.setData( chatMessageDataToString( cmd ) );
  return m;
}

Message Protocol::groupChatRemoveUserMessage( const Chat& c )
{
  Message m( Message::Group, newId(), "" );
  m.addFlag( Message::Delete );
  ChatMessageData cmd;
  cmd.setGroupId( c.privateId() );
  cmd.setGroupName( c.name() );
  cmd.setGroupLastModified( c.lastModified() );
  m.setData( chatMessageDataToString( cmd ) );
  return m;
}

QList<UserRecord> Protocol::userRecordsFromGroupRequestMessage( const Message& m ) const
{
  QList<UserRecord> user_records;
  if( m.text().isEmpty() )
    return user_records;
  QStringList sl = m.text().split( PROTOCOL_FIELD_SEPARATOR );
  if( sl.isEmpty() )
    return user_records;

  bool ok = false;
  int members = sl.takeFirst().toInt( &ok );
  if( !ok )
    return user_records;

  for( int i = 0; i < members; i++ )
  {
    if( sl.size() >= 4 )
    {
      UserRecord ur;
      ur.setName( sl.takeFirst() );
      ur.setAccount( sl.takeFirst() );
      ur.setHash( sl.takeFirst() );
      ur.setDomainName( sl.takeFirst() );
      user_records.append( ur );
    }
    else
    {
      user_records.clear();
      return user_records;
    }
  }

  return user_records;
}

Message Protocol::groupChatRequestMessage_obsolete( const Chat& c, const User& to_user )
{
  Message m( Message::Group, newId(), "" );
  m.addFlag( Message::Request );
  UserList ul = UserManager::instance().userList().fromUsersId( c.usersId() );
  ul.remove( Settings::instance().localUser() );
  ul.remove( to_user );
  QStringList sl;
  foreach( User u, ul.toList() )
    sl << u.path();
  if( !sl.isEmpty() )
    m.setText( sl.join( PROTOCOL_FIELD_SEPARATOR ) );
  ChatMessageData cmd;
  cmd.setGroupId( c.privateId() );
  cmd.setGroupName( c.name() );
  m.setData( chatMessageDataToString( cmd ) );
  return m;
}

QStringList Protocol::userPathsFromGroupRequestMessage_obsolete( const Message& m ) const
{
  return m.text().isEmpty() ? QStringList() : m.text().split( PROTOCOL_FIELD_SEPARATOR );
}

Message Protocol::fileInfoRefusedToMessage( const FileInfo& fi )
{
  Message m = fileInfoToMessage( fi );
  m.addFlag( Message::Refused );
  m.addFlag( Message::Private );
  return m;
}

Message Protocol::folderRefusedToMessage( const QString& folder_name, const QString& chat_private_id )
{
  Message m( Message::Folder, newId(), folder_name );
  m.addFlag( Message::Refused );
  m.addFlag( Message::Private );
  m.setData( chat_private_id );
  return m;
}

Message Protocol::fileInfoToMessage( const FileInfo& fi )
{
  Message m( Message::File, newId(), fi.name() );
  QStringList sl;
  sl << QString::number( fi.networkAddress().hostPort() );
  sl << QString::number( fi.size() );
  sl << QString::number( fi.id() );
  sl << QString::fromUtf8( fi.password() );
  sl << fi.fileHash();
  sl << fi.shareFolder();
  if( fi.isInShareBox() )
    sl << QString( "1" );
  else
    sl << QString( "0" );
  sl << fi.chatPrivateId();
  if( fi.lastModified().isValid() )
    sl << fi.lastModified().toString( Qt::ISODate );
  else
    sl << QString( "" );
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

  if( !sl.isEmpty() )
    fi.setIsInShareBox( sl.takeFirst() == QString( "1" ) );

  if( !sl.isEmpty() )
    fi.setChatPrivateId( sl.takeFirst() );

  if( !sl.isEmpty() )
  {
    if( !sl.first().isEmpty() )
    {
      QDateTime dt_last_modified = QDateTime::fromString( sl.takeFirst(), Qt::ISODate );
      if( dt_last_modified.isValid() )
        fi.setLastModified( dt_last_modified );
    }
  }

  return fi;
}

FileInfo Protocol::fileInfo( const QFileInfo& fi, const QString& share_folder, bool to_share_box, const QString& chat_private_id )
{
  FileInfo file_info = FileInfo( newId(), FileInfo::Upload );
  file_info.setName( fi.fileName() );
  file_info.setPath( Bee::convertToNativeFolderSeparator( fi.absoluteFilePath() ) );
  file_info.setShareFolder( share_folder );
  file_info.setIsInShareBox( to_share_box );

  if( fi.isFile() )
  {
    file_info.setSuffix( fi.suffix() );
    file_info.setSize( static_cast<FileSizeType>( fi.size() ) );
  }
  else
    file_info.setIsFolder( true );

  if( to_share_box )
  {
    file_info.setPassword( Settings::instance().hash( file_info.path() ) );
    file_info.setFileHash( QString::fromLatin1( file_info.password() ) );
  }
  else
  {
    file_info.setFileHash( fileInfoHash( fi ) );
    QString password_key = QString( "%1%2%3%4%5%6" )
                            .arg( Random::number( 111111, 999999 ) )
                            .arg( file_info.id() )
                            .arg( Random::number( 111111, 999999 ) )
                            .arg( file_info.path() )
                            .arg( Random::number( 111111, 999999 ) )
                            .arg( file_info.size() );
    file_info.setPassword( Settings::instance().hash( password_key ) );
  }

  file_info.setLastModified( fi.lastModified() );
  file_info.setChatPrivateId( chat_private_id );

  return file_info;
}

bool Protocol::fileCanBeShared( const QFileInfo& file_info )
{
  if( !file_info.exists() )
  {
    qWarning() << "Path" << qPrintable( file_info.absoluteFilePath() ) << "not exists and cannot be shared";
    return false;
  }

  if( file_info.isDir() && file_info.fileName().endsWith( "." ) )
  {
    // skip folder . and folder ..
    return false;
  }

  if( !file_info.isReadable() )
  {
    qWarning() << "Path" << qPrintable( file_info.absoluteFilePath() ) << "is not readable and cannot be shared";
    return false;
  }

  if( file_info.isSymLink() )
  {
    qDebug() << "Path" << qPrintable( file_info.absoluteFilePath() ) << "is a symbolic link and cannot be shared";
    return false;
  }

  if( file_info.isHidden() )
  {
    qDebug() << "Path" << qPrintable( file_info.absoluteFilePath() ) << "is hidden and cannot be shared";
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
    sl.append( fi.chatPrivateId().isEmpty() ? QString( "" ) : fi.chatPrivateId() );
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

  sl = m.text().split( PROTOCOL_FIELD_SEPARATOR, QString::SkipEmptyParts );

  QStringList::const_iterator it = sl.begin();
  while( it != sl.end() )
  {
    QStringList sl_tmp = (*it).split( DATA_FIELD_SEPARATOR );

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
      if( !sl_tmp.isEmpty() )
        fi.setChatPrivateId( sl_tmp.takeFirst() );
      file_info_list.append( fi );
    }

    ++it;
  }

  return file_info_list;
}

Message Protocol::fileShareRequestMessage() const
{
  Message file_share_request_message( Message::Share, ID_SHARE_MESSAGE, "" );
  file_share_request_message.addFlag( Message::Request );
  return file_share_request_message;
}

void Protocol::createFileShareListMessage( const QMultiMap<QString, FileInfo>& file_info_list, int server_port )
{
  QStringList msg_list;

  if( server_port > 0 )
  {
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
  }

  Message m( Message::Share, ID_SHARE_MESSAGE, msg_list.isEmpty() ? QString( "" ) : msg_list.join( PROTOCOL_FIELD_SEPARATOR ) );
  m.setData( msg_list.isEmpty() ? QString( "0" ) : QString::number( server_port ) );
  m.addFlag( Message::List );

  m_fileShareListMessage = m;
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

  sl = m.text().split( PROTOCOL_FIELD_SEPARATOR, QString::SkipEmptyParts );

  QStringList::const_iterator it = sl.begin();
  while( it != sl.end() )
  {
    QStringList sl_tmp = (*it).split( DATA_FIELD_SEPARATOR );

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

Message Protocol::shareBoxRequestPathList( const QString& folder_name, bool set_create_flag )
{
  Message m( Message::ShareBox, ID_SHAREBOX_MESSAGE, "" );
  QStringList msg_data;
  msg_data << QString::number( 0 );
  msg_data << folder_name;
  m.setData( msg_data.join( DATA_FIELD_SEPARATOR ) );
  if( set_create_flag )
    m.addFlag( Message::Create );
  else
    m.addFlag( Message::Request );
  return m;
}

Message Protocol::refuseToShareBoxPath( const QString& folder_name, bool set_create_flag )
{
  Message m( Message::ShareBox, ID_SHAREBOX_MESSAGE, "" );
  QStringList msg_data;
  msg_data << QString::number( 0 );
  msg_data << folder_name;
  m.setData( msg_data.join( DATA_FIELD_SEPARATOR ) );
  m.addFlag( Message::Refused );
  if( set_create_flag )
    m.addFlag( Message::Create );
  else
    m.addFlag( Message::Request );
  return m;
}

Message Protocol::acceptToShareBoxPath( const QString& folder_name, const QList<FileInfo>& file_info_list, int server_port )
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
    sl << QString( "" ); // shareFolder;
    sl << fi.lastModified().toString( Qt::ISODate );
    if( fi.isFolder() )
      sl << QString( "1" );
    else
      sl << QString( "" );
    msg_list.append( sl.join( DATA_FIELD_SEPARATOR ) );
  }

  Message m( Message::ShareBox, ID_SHAREBOX_MESSAGE, msg_list.join( PROTOCOL_FIELD_SEPARATOR ) );
  msg_list.clear();
  msg_list << QString::number( server_port );
  msg_list << folder_name;
  m.setData( msg_list.join( DATA_FIELD_SEPARATOR ) );
  m.addFlag( Message::List );
  return m;
}

QString Protocol::folderNameFromShareBoxMessage( const Message& m ) const
{
  QStringList sl = m.data().split( DATA_FIELD_SEPARATOR );
  if( sl.size() < 2 )
    return "";
  else
    return Bee::convertToNativeFolderSeparator( sl.at( 1 ) );
}

QList<FileInfo> Protocol::messageToShareBoxFileList( const Message& m, const QHostAddress& server_address ) const
{
  QList<FileInfo> file_info_list;
  if( m.type() != Message::ShareBox )
    return file_info_list;

  QStringList sl = m.data().split( DATA_FIELD_SEPARATOR );
  if( sl.size() < 2 )
    return file_info_list;

  int server_port = sl.takeFirst().toInt();
  QString folder_name = Bee::convertToNativeFolderSeparator( sl.takeFirst() );
  QString s_tmp;

  sl = m.text().split( PROTOCOL_FIELD_SEPARATOR, QString::SkipEmptyParts );
  QStringList::const_iterator it = sl.begin();
  while( it != sl.end() )
  {
    QStringList sl_tmp = (*it).split( DATA_FIELD_SEPARATOR );
    if( sl_tmp.size() >= 9 )
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
      s_tmp = sl_tmp.takeFirst();
      if( s_tmp.isEmpty() )
        fi.setShareFolder( folder_name );
      else
        fi.setShareFolder( Bee::convertToNativeFolderSeparator( s_tmp ) );
      fi.setLastModified( QDateTime::fromString( sl_tmp.takeFirst(), Qt::ISODate ) );
      s_tmp = sl_tmp.takeFirst();
      fi.setIsFolder( !s_tmp.isEmpty() );
      fi.setIsInShareBox( true );
      file_info_list.append( fi );
    }

    ++it;
  }

  return file_info_list;
}

#ifdef BEEBEEP_USE_SHAREDESKTOP
  Message Protocol::refuseToViewDesktopShared() const
  {
    Message m( Message::ShareDesktop, ID_SHAREDESKTOP_MESSAGE, "" );
    m.addFlag( Message::Refused );
    return m;
  }

  Message Protocol::readImageFromDesktopShared() const
  {
    Message m( Message::ShareDesktop, ID_SHAREDESKTOP_MESSAGE, "" );
    m.addFlag( Message::Request );
    return m;
  }


  Message Protocol::shareDesktopImageDataToMessage( const ShareDesktopData& sdd ) const
  {
    Message m( Message::ShareDesktop, ID_SHAREDESKTOP_MESSAGE, QString::fromLatin1( sdd.imageData().toBase64() ) );
    m.addFlag( Message::Private );
    QStringList sl_data;
    sl_data << sdd.imageType();
    if( sdd.isCompressed() )
      sl_data << QString( "9" );
    else
      sl_data << QString( "" );
    sl_data << QString::number( sdd.diffColor() );
    sl_data << QLatin1String( "base64" );
    m.setData( sl_data.join( DATA_FIELD_SEPARATOR ) );
    return m;
  }

  ShareDesktopData Protocol::imageDataFromShareDesktopMessage( const Message& m ) const
  {
    ShareDesktopData sdd;
    if( m.text().isEmpty() )
    {
#ifdef BEEBEEP_DEBUG
      qDebug() << "Share desktop message is arrived with empty image";
#endif
      return sdd;
    }

    QString img_type = Settings::instance().shareDesktopImageType();
    QRgb diff_color = qRgba( 0, 0, 0, 0 );
    bool use_compression = true;
    QString image_codec = "base64";
    if( !m.data().isEmpty() )
    {
      QStringList sl_data = m.data().split( DATA_FIELD_SEPARATOR, QString::KeepEmptyParts );
      if( sl_data.size() >= 4 )
      {
        img_type = sl_data.takeFirst();
        use_compression = sl_data.takeFirst().isEmpty() ? false : true;
        bool ok = false;
        unsigned int diff_color_tmp = sl_data.takeFirst().toUInt( &ok );
        if( ok )
          diff_color = diff_color_tmp;
        image_codec = sl_data.takeFirst();
      }
      else
        qWarning() << "Invalid image data found in share desktop message (default values used)";
    }
    else
      qWarning() << "Empty image data found in share desktop message (default values used)";

    sdd.setImageType( img_type );
    sdd.setDiffColor( diff_color );
    sdd.setIsCompressed( use_compression );
    sdd.setImageData( image_codec == "base64" ? QByteArray::fromBase64( m.text().toLatin1() ) : m.text().toLatin1() );
    return sdd;
  }
#endif

ChatMessageData Protocol::dataFromChatMessage( const Message& m ) const
{
  ChatMessageData cmd;
  if( m.data().isEmpty() )
    return cmd;
  QStringList sl = m.data().split( DATA_FIELD_SEPARATOR );
  if( sl.isEmpty() )
    return cmd;

  if( !sl.first().isEmpty() )
  {
    QColor c( sl.first() );
    if( !c.isValid() )
    {
      qWarning() << "Invalid text color in Chat Message Data:" << qPrintable( m.data() );
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

  if( sl.isEmpty() )
    return cmd;

  QString s_group_last_modified = sl.takeFirst();
  if( !s_group_last_modified.isEmpty() )
    cmd.setGroupLastModified( QDateTime::fromString( s_group_last_modified, Qt::ISODate ) );

  return cmd;
}

QString Protocol::chatMessageDataToString( const ChatMessageData& cmd ) const
{
  QStringList sl;
  sl << (cmd.textColor().isValid() ? cmd.textColor().name() : "");
  sl << (cmd.groupId().size() > 0 ? cmd.groupId() : "");
  sl << (cmd.groupName().size() > 0 ? cmd.groupName() : "");
  sl << (cmd.groupLastModified().isValid() ? cmd.groupLastModified().toString( Qt::ISODate ) : "");
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

  m.setData( chatMessageDataToString( cmd ) );
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
  return Settings::instance().simpleHash( sl.join( QString::number( Random::d100() ) ) );
}

QString Protocol::fileInfoHashTmp( VNumber file_info_id, const QString& file_info_name, FileSizeType file_info_size ) const
{
  QStringList sl;
  sl << QString::number( file_info_id );
  sl << file_info_name;
  sl << QString::number( file_info_size );
  return Settings::instance().simpleHash( sl.join( QString::number( Random::d100() ) ) );
}

QString Protocol::newMd5Id()
{
  QStringList sl;
  sl << QString::number( newId() );
  sl << QString::number( Random::d100() );
  sl << Settings::instance().localUser().path();
  sl << QHostInfo::localHostName();
  sl << QString::number( Random::d100() );
  sl << QDateTime::currentDateTime().toString( "dd.MM.yyyy-hh:mm:ss.zzz" );
  sl << QString::number( Random::d100() );
  return Settings::instance().simpleHash( sl.join( QString::number( Random::d100() ) ) );
}

QByteArray Protocol::bytesArrivedConfirmation( int num_bytes ) const
{
  QByteArray byte_array = QByteArray::number( num_bytes );
  while( byte_array.size() % ENCRYPTED_DATA_BLOCK_SIZE )
    byte_array.prepend( '0' );
  return byte_array;
}

Message Protocol::userRecordListToHiveMessage( const QList<UserRecord>& user_record_list )
{
  QStringList msg_list;
  foreach( UserRecord ur, user_record_list )
    msg_list.append( saveUserRecord( ur, false ) );

  Message m( Message::Hive, newId(), msg_list.join( PROTOCOL_FIELD_SEPARATOR ) );
  m.addFlag( Message::List );
  return m;
}

QList<UserRecord> Protocol::hiveMessageToUserRecordList( const Message& m ) const
{
  QList<UserRecord> user_record_list;
  if( m.type() != Message::Hive && !m.hasFlag( Message::List ) )
    return user_record_list;

  QStringList sl = m.text().split( PROTOCOL_FIELD_SEPARATOR, QString::SkipEmptyParts );
  UserRecord ur;
  foreach( QString s, sl )
  {
    ur = loadUserRecord( s );
    if( ur.networkAddressIsValid() )
      user_record_list.append( ur );
  }

  return user_record_list;
}

QString Protocol::saveChatRecord( const ChatRecord& cr ) const
{
  QStringList sl;
  sl << cr.name();
  sl << cr.privateId();
  return sl.join( DATA_FIELD_SEPARATOR );
}

ChatRecord Protocol::loadChatRecord( const QString& s ) const
{
  if( s.isEmpty() )
    return ChatRecord();

  ChatRecord cr;
  QStringList sl = s.split( DATA_FIELD_SEPARATOR );
  if( !sl.isEmpty() )
    cr.setName( sl.takeFirst() );
  if( !sl.isEmpty() )
    cr.setPrivateId( sl.takeFirst() );
  return cr;
}

QString Protocol::linkifyText( QString text )
{
#ifdef Q_OS_WIN
  // linkify windows network path
  if( text.contains( "\\\\" ) )
  {
    int index_backslash = text.simplified().indexOf( "\\\\" );
    QString pre_text = "";
    if( index_backslash > 0 )
    {
      pre_text = text.section( "\\\\", 0, 0 );
      if( !pre_text.isEmpty() )
        text.remove( 0, pre_text.size() );
    }

    QUrl url_to_add = QUrl::fromLocalFile( text.simplified() );
#if QT_VERSION >= 0x050000
    text = QString( "<a href=\"%1\">%2</a>" ).arg( url_to_add.url() ).arg( text.simplified() );
#else
    text = QString( "<a href=\"%1\">%2</a>" ).arg( url_to_add.toString() ).arg( text.simplified() );
#endif
    if( !pre_text.isEmpty() )
      text.prepend( pre_text );

#ifdef BEEBEEP_DEBUG
    qDebug() << "Linkified text:" << qPrintable( text );
#endif
  }
#endif

  if( !text.contains( QLatin1Char( '.' ) ) )
    return text;
  text.prepend( " " ); // for matching www.miosito.it
  text.replace( QRegExp( "(((f|ht){1}tp(s:|:){1}//)[-a-zA-Z0-9@:%_\\+.,~#?!&//=\\(\\)]+)" ), "<a href=\"\\1\">\\1</a>" );
  text.replace( QRegExp( "([\\s()[{}])(www.[-a-zA-Z0-9@:%_\\+.,~#?!&//=\\(\\)]+)" ), "\\1<a href=\"http://\\2\">\\2</a>" );
  text.replace( QRegExp( "([_\\.0-9a-z-]+@([0-9a-z][0-9a-z-]+\\.)+[a-z]{2,3})" ), "<a href=\"mailto:\\1\">\\1</a>" );
  text.remove( 0, 1 ); // remove the space added

  return text;
}

QString Protocol::formatHtmlText( const QString& text )
{
  QString text_formatted = "";
  int last_semicolon_index = -1;
  bool there_is_a_space_before_it = false;

  for( int i = 0; i < text.length(); i++ )
  {
    if( text.at( i ) == QLatin1Char( ' ' ) )
    {
      if( there_is_a_space_before_it )
        text_formatted += QLatin1String( "&nbsp; " ); // space added for emoticons recognize
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

    there_is_a_space_before_it = text.at( i ) == QLatin1Char( ' ' );
  }

  if( last_semicolon_index >= 0 )
    text_formatted.replace( last_semicolon_index, 1, QLatin1String( "&lt;" ) );

  text_formatted.replace( QRegExp("(^|\\s|>)_(\\S+)_(<|\\s|$)"), "\\1<u>\\2</u>\\3" );
  text_formatted.replace( QRegExp("(^|\\s|>)\\*(\\S+)\\*(<|\\s|$)"), "\\1<b>\\2</b>\\3" );
  text_formatted.replace( QRegExp("(^|\\s|>)\\/(\\S+)\\/(<|\\s|$)"), "\\1<i>\\2</i>\\3" );

  if( Settings::instance().chatUseClickableLinks() )
    text_formatted = linkifyText( text_formatted );

  if( Settings::instance().showEmoticons() )
    text_formatted = EmoticonManager::instance().parseEmoticons( text_formatted, Settings::instance().emoticonSizeInChat(), Settings::instance().useNativeEmoticons() );

  PluginManager::instance().parseText( &text_formatted, false );

  return text_formatted;
}

/* Encryption */
QByteArray Protocol::createCipherKey( const QString& public_key_1, const QString& public_key_2, int data_stream_version ) const
{
  QString public_key = public_key_1 + public_key_2;
#if QT_VERSION < 0x050000
  Q_UNUSED( data_stream_version );
  QCryptographicHash ch( QCryptographicHash::Sha1 );
#else
  QCryptographicHash ch( data_stream_version < 13 ? QCryptographicHash::Sha1 : QCryptographicHash::Sha3_256 );
#endif
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

void Protocol::hexToUnsignedChar(  const QByteArray& hex_byte_array, unsigned char* out_string, unsigned int len_out_string ) const
{
  // Thanks to Christophe David
  const char* hex_string = hex_byte_array.data();
  unsigned int i = 0;
  unsigned int j = 0;
  bool msb = true;
  while( j < len_out_string )
  {
    if( hex_string[i] >= '0' && hex_string[i] <='9' )
    {
      if( msb )
      {
        out_string[j] = int(hex_string[i++] - '0')*16;
        msb = false;
      }
      else
      {
        out_string[j++] |= int(hex_string[i++] - '0');
        msb = true;
      }
    }
    else if( toupper(hex_string[i]) >= 'A' && toupper(hex_string[i]) <= 'F' )
    {
      if( msb )
      {
        out_string[j] = (int(toupper(hex_string[i++]) - 'A') + 10) * 16;
        msb = false;
      }
      else
      {
        out_string[j++] |= (int(toupper(hex_string[i++]) - 'A')+10);
        msb = true;
      }
    }
    else
    {
      // whatever it is and aspecially a '\x00'
      if( i < strlen( hex_string ) )
        i++;

      if( msb ) // lsb can not be decoded, we keep it to 0 else we fill data with 0
        out_string[j++] = 0;
      else
        msb = true;
    }
  }
}

QByteArray Protocol::encryptByteArray( const QByteArray& text_to_encrypt, const QByteArray& cipher_key, int proto_version ) const
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

  if( proto_version < SECURE_LEVEL_3_PROTO_VERSION )
  {
    // What the hell...
    for( unsigned int i = 0; i < sizeof( key ); i++ )
      key[ i ] = static_cast<unsigned int>(cipher_key.size()) < i ? static_cast<unsigned char>( cipher_key.at( static_cast<int>(i) ) ) : 0;
  }
  else
  {
    hexToUnsignedChar( cipher_key, key, KEYLENGTH(ENCRYPTION_KEYBITS) );
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

QByteArray Protocol::decryptByteArray( const QByteArray& text_to_decrypt, const QByteArray& cipher_key, int proto_version ) const
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

  if( proto_version < SECURE_LEVEL_3_PROTO_VERSION )
  {
    // What the hell...
    for( unsigned int i = 0; i < sizeof( key ); i++ )
      key[ i ] = static_cast<unsigned int>(cipher_key.size()) < i ? static_cast<unsigned char>( cipher_key.at( static_cast<int>(i) ) ) : 0;
  }
  else
  {
    hexToUnsignedChar( cipher_key, key, KEYLENGTH(ENCRYPTION_KEYBITS) );
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
