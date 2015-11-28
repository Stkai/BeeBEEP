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
#include "ChatMessage.h"
#include "NetworkAddress.h"
#include "Settings.h"
#include "Version.h"


Settings* Settings::mp_instance = NULL;


Settings::Settings()
 : m_localUser( ID_LOCAL_USER )
{
  m_settingsVersion = BEEBEEP_SETTINGS_VERSION;

  /* Default RC start */
  m_useChatWithAllUsers = true;
  m_useSettingsFileIni = true;
  m_broadcastOnlyToHostsIni = false;
  m_defaultBroadcastPort = DEFAULT_BROADCAST_PORT;
  m_defaultListenerPort = DEFAULT_LISTENER_PORT;
  m_defaultFileTransferPort = DEFAULT_FILE_TRANSFER_PORT;
  m_saveDataInDocumentsFolder = false;
  m_resetGeometryAtStartup = false;
#ifdef Q_OS_MAC
  m_saveDataInUserApplicationFolder = true;
  m_useNativeEmoticons = true;
#ifdef BEEBEEP_USE_MULTICAST_DNS
  m_useMulticastDns = true;
#else
  m_useMulticastDns = false;
#endif
#else
  m_saveDataInUserApplicationFolder = false;
  m_useNativeEmoticons = false;
  m_useMulticastDns = false;
#endif
  m_allowMultipleInstances = false;
  m_trustNickname = true;
  m_trustSystemAccount = false;
  m_dataFolderInRC = "";
  m_addAccountNameToDataFolder = false;
  m_preferredSubnets = "";
  m_useIPv6 = false;
  /* Default RC end */

  m_emoticonSizeInEdit = 18;

  QFont f = QApplication::font();
  setChatFont( f );
  m_emoticonSizeInMenu = 24;
  m_emoticonInRecentMenu = 30;
  m_confirmOnDownloadFile = false;
  m_promptOnCloseEvent = true;
  m_saveUserList = false;
  m_localUser.setStatus( User::Online );
  m_localUser.setVersion( version( false ) );
  setPassword( defaultPassword() );
  m_resourceFolder = ".";
#if QT_VERSION >= 0x050400
  m_dataFolder = Bee::convertToNativeFolderSeparator( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) );
#elif QT_VERSION >= 0x050000
  m_dataFolder = Bee::convertToNativeFolderSeparator( QString( "%1/%2" )
                   .arg( QStandardPaths::writableLocation( QStandardPaths::DataLocation ) )
                   .arg( programName() ) );
#else
  m_dataFolder = Bee::convertToNativeFolderSeparator( QDesktopServices::storageLocation( QDesktopServices::DataLocation ) );
#endif
  m_lastSave = QDateTime::currentDateTime();

  m_preventMultipleConnectionsFromSingleHostAddress = false;
  m_alwaysOpenNewFloatingChat = false;
  m_acceptConnectionsOnlyFromWorkgroups = false;
  m_maxUserStatusInList = 10;

  m_connectionTimeout = 5000;
  m_useReturnToSendMessage = true;

}

void Settings::setChatFont( const QFont& new_value )
{
  m_chatFont = new_value;
  QFontMetrics fm( m_chatFont );
  m_emoticonSizeInChat = qMax( 24, qMin( 32, fm.height() ) );
  m_emoticonSizeInEdit = qMax( 16, qMin( 32, fm.height() ) );
  if( m_emoticonSizeInChat % 2 )
    m_emoticonSizeInChat++;
  if( m_emoticonSizeInEdit % 2 )
    m_emoticonSizeInEdit++;
}

QString Settings::accountNameFromSystemEnvinroment() const
{
  QProcessEnvironment pe = QProcessEnvironment::systemEnvironment();
  QString account_name = pe.value( "USERNAME", "" );
  if( account_name.isEmpty() )
    account_name = pe.value( "USER", "" );
  return account_name.simplified();
}

void Settings::createLocalUser()
{
  QString sName = accountNameFromSystemEnvinroment();
  if( sName.isEmpty() )
  {
    qWarning() << "USERNAME and USER variabile not found in system environment";
    sName = QString( "Bee%1" ).arg( QTime::currentTime().toString( "hmszzz" ) );
  }
  m_localUser.setAccountName( sName.toLower() );
  if( m_localUser.name().isEmpty() )
    m_localUser.setName( sName );
  m_localUser.setQtVersion( qtMajorVersion() );
  qDebug() << "User name:" << m_localUser.name();
  qDebug() << "System Account:" << m_localUser.accountName();
}

void Settings::createSessionId()
{
  QString session_parameters = QString( "%1%2%3%4" ).arg( m_localUser.accountName() ).arg( m_localUser.path() )
          .arg( version( true ) ).arg( QDateTime::currentDateTime().toString( "dd.MM.yyyy-hh:mm:ss.zzz" ) );
  QByteArray id_generated = QCryptographicHash::hash( session_parameters.toUtf8(), QCryptographicHash::Sha1 );
  QString session_id = QString::fromUtf8( id_generated.toHex() );
  qDebug() << "Session ID created:" << session_id;
  m_localUser.setSessionId( session_id );
}

bool Settings::createDefaultRcFile()
{
  QFileInfo rc_file_info = defaultRcFilePath( false );
  if( rc_file_info.exists() )
  {
    qDebug() << "RC default configuration file exists in" << rc_file_info.absoluteFilePath();
    return false;
  }

  QSettings* sets = new QSettings( rc_file_info.absoluteFilePath(), QSettings::IniFormat );
  bool rc_file_created = false;
  if( sets->isWritable() )
  {
    sets->beginGroup( "BeeBEEP" );
    sets->setValue( "UseConfigurationFileIni", m_useSettingsFileIni );
    sets->setValue( "BroadcastOnlyToHostsIni", m_broadcastOnlyToHostsIni );
    sets->setValue( "BroadcastPort", m_defaultBroadcastPort );
    sets->setValue( "DefaultListenerPort", m_defaultListenerPort );
    sets->setValue( "DefaultFileTransferPort", m_defaultFileTransferPort );
    sets->setValue( "SaveDataInDocumentsFolder", m_saveDataInDocumentsFolder );
    sets->setValue( "SaveDataInUserApplicationFolder", m_saveDataInUserApplicationFolder );
    sets->setValue( "AllowMultipleInstances", m_allowMultipleInstances );
    sets->setValue( "DataFolderPath", m_dataFolderInRC );
    sets->setValue( "AddAccountNameToDataFolder", m_addAccountNameToDataFolder );
  #ifdef BEEBEEP_USE_MULTICAST_DNS
    sets->setValue( "UseMulticastDns", m_useMulticastDns );
  #endif
    sets->setValue( "PreventMultipleConnectionsFromSingleHostAddress", m_preventMultipleConnectionsFromSingleHostAddress );
    sets->setValue( "PreferredSubnets", m_preferredSubnets );
    sets->setValue( "UseIPv6", m_useIPv6 );
    if( m_multicastGroupAddress.isNull() )
      sets->setValue( "MulticastGroupAddress", "" );
    else
      sets->setValue( "MulticastGroupAddress", m_multicastGroupAddress.toString() );
    sets->setValue( "EnableChatWithAllUsers", m_useChatWithAllUsers );
    sets->endGroup();
    sets->beginGroup( "Groups" );
    sets->setValue( "TrustNickname", m_trustNickname );
    sets->setValue( "TrustSystemAccount", m_trustSystemAccount );
    sets->endGroup();
    sets->sync();
    qDebug() << "RC default configuration file created in" << sets->fileName();
    rc_file_created = true;
  }
  else
    qWarning() << "Unable to create RC default configuration file in" << sets->fileName();

  sets->deleteLater();
  return rc_file_created;
}

void Settings::loadRcFile()
{
  QFileInfo rc_file_info( defaultRcFilePath( true ) );
  QString rc_file_path = Bee::convertToNativeFolderSeparator( rc_file_info.absoluteFilePath() );
  qDebug() << "Check for RC file in current path:" << qPrintable( rc_file_path );
  if( !rc_file_info.exists() || !rc_file_info.isReadable() )
  {
    rc_file_info = QFileInfo( defaultRcFilePath( false ) );
    rc_file_path = Bee::convertToNativeFolderSeparator( rc_file_info.absoluteFilePath() );
    qDebug() << "Check for RC file in custom path:" << qPrintable( rc_file_path );

    if( !rc_file_info.exists() || !rc_file_info.isReadable() )
    {
      qDebug() << "RC file not found:" << qPrintable( programName() ) << "uses default RC configuration";
      return;
    }
  }

  QSettings* sets = new QSettings( rc_file_path, QSettings::IniFormat );
  sets->beginGroup( "BeeBEEP" );
  m_useSettingsFileIni = sets->value( "UseConfigurationFileIni", m_useSettingsFileIni ).toBool();
  m_broadcastOnlyToHostsIni = sets->value( "BroadcastOnlyToHostsIni", m_broadcastOnlyToHostsIni ).toBool();
  m_defaultBroadcastPort = sets->value( "BroadcastPort", m_defaultBroadcastPort ).toInt();
  m_defaultListenerPort = sets->value( "DefaultListenerPort", m_defaultListenerPort ).toInt();
  m_defaultFileTransferPort = sets->value( "DefaultFileTransferPort", m_defaultFileTransferPort ).toInt();
  m_saveDataInDocumentsFolder = sets->value( "SaveDataInDocumentsFolder", m_saveDataInDocumentsFolder ).toBool();
  m_saveDataInUserApplicationFolder = sets->value( "SaveDataInUserApplicationFolder", m_saveDataInUserApplicationFolder ).toBool();
  m_allowMultipleInstances = sets->value( "AllowMultipleInstances", m_allowMultipleInstances ).toBool();
  m_dataFolderInRC = Bee::convertToNativeFolderSeparator( sets->value( "DataFolderPath", m_dataFolderInRC ).toString() );
  m_addAccountNameToDataFolder = sets->value( "AddAccountNameToDataFolder", m_addAccountNameToDataFolder ).toBool();
#ifdef BEEBEEP_USE_MULTICAST_DNS
  m_useMulticastDns = sets->value( "UseMulticastDns", m_useMulticastDns ).toBool();
#endif
  m_preventMultipleConnectionsFromSingleHostAddress = sets->value( "PreventMultipleConnectionsFromSingleHostAddress", m_preventMultipleConnectionsFromSingleHostAddress ).toBool();
  m_preferredSubnets = sets->value( "PreferredSubnets", m_preferredSubnets ).toString();
  m_useIPv6 = sets->value( "UseIPv6", m_useIPv6 ).toBool();
  QString multicast_group_address = sets->value( "MulticastGroupAddress", "" ).toString();
  if( multicast_group_address.isEmpty() )
    m_multicastGroupAddress = QHostAddress();
  else
    m_multicastGroupAddress = QHostAddress( multicast_group_address );
  m_useChatWithAllUsers = sets->value( "EnableChatWithAllUsers", m_useChatWithAllUsers ).toBool();
  sets->endGroup();
  sets->beginGroup( "Groups" );
  m_trustNickname = sets->value( "TrustNickname", m_trustNickname ).toBool();
  m_trustSystemAccount = sets->value( "TrustSystemAccount", m_trustSystemAccount ).toBool();
  sets->endGroup();

  QStringList key_list = sets->allKeys();
  foreach( QString key, key_list )
    qDebug() << "RC read ->" << qPrintable( key ) << "=" << qPrintable( sets->value( key ).toString() );

  qDebug() << "RC configuration file read";

  sets->deleteLater();
}

bool Settings::createDefaultHostsFile()
{
  QStringList sl;

  sl << "# This is a sample HOSTS file used by BeeBEEP.";
  sl << "#";
  sl << "# This file contains the IP addresses (something like 10.0.0.123)";
  sl << "# or subnet (10.0.0.255 means that BeeBEEP tries to connect every";
  sl << "# client from the IP 10.0.0.1 to IP 10.0.0.254).";
  sl << "# Each entry should be kept on an individual line.";
  sl << "#";
  sl << "# Additionally, comments (such as these) may be inserted";
  sl << "# on individual line denoted by a '#', '*' or '/' symbol.";
  sl << "#";
  sl << "# For example:";
  sl << "#";
  sl << "# 10.184.9.132";
  sl << "# 192.168.2.17";
  sl << "# 10.184.5.255";
  sl << "# 10.2.4.255";
  sl << "# 2001:db8:1f70::999:de8:7648:6e8";
  sl << "#";
  sl << "# From version 3.0.1 you can also add port to the host addresses";
  sl << "# (ipv4 and ipv6 supported). For example:";
  sl << "#";
  sl << "# 10.184.9.132:6475";
  sl << "# [2001:db8:1f70::999:de8:7648:6e8]:6475";
  sl << "#";
  sl << " ";

  QFile file_host_ini( defaultHostsFilePath( false ) );
  if( file_host_ini.exists() )
  {
    qDebug() << "HOSTS default configuration file exists in" << file_host_ini.fileName();
    return false;
  }

  if( file_host_ini.open( QIODevice::WriteOnly ) )
  {
    QTextStream ts( &file_host_ini );
    foreach( QString line, sl )
      ts << line << endl;
    file_host_ini.close();
    qDebug() << "HOSTS default configuration file created in" << file_host_ini.fileName();
    return true;
  }
  else
  {
    qWarning() << "Unable to create the HOSTS default configuration file in" << file_host_ini.fileName();
    return false;
  }
}

QString Settings::version( bool complete ) const
{
  return complete ? QString( "%1 (b%2p%3)" ).arg( BEEBEEP_VERSION ).arg( BEEBEEP_BUILD ).arg( BEEBEEP_PROTO_VERSION ) : QString( BEEBEEP_VERSION );
}

QString Settings::httpUserAgent() const
{
  return QString( "%1 %2" ).arg( programName() ).arg( version( false ) );
}

int Settings::protoVersion() const
{
  return BEEBEEP_PROTO_VERSION;
}

QString Settings::programName() const
{
  return QString( BEEBEEP_NAME );
}

QString Settings::organizationName() const
{
  return QString( BEEBEEP_ORGANIZATION );
}

QString Settings::organizationDomain() const
{
  return QString( BEEBEEP_ORGANIZATION_DOMAIN );
}

QString Settings::officialWebSite() const
{
  return QString( BEEBEEP_WEBSITE );
}

QString Settings::pluginWebSite() const
{
  return officialWebSite() + QString( BEEBEEP_PLUGIN_WEBSITE );
}

QString Settings::donationWebSite() const
{
  return officialWebSite() + QString( BEEBEEP_DONATE_WEBSITE );
}

QString Settings::helpWebSite() const
{
  return officialWebSite() + QString( BEEBEEP_HELP_WEBSITE );
}

QString Settings::languageWebSite() const
{
  return officialWebSite() + QString( BEEBEEP_LANGUAGE_WEBSITE );
}

QString Settings::operatingSystem( bool use_long_name  ) const
{
  QString os_name_long = "Unknown OS";
  QString os_name_short = "unknown";
#ifdef Q_OS_WIN
  os_name_long = "MS Windows";
  os_name_short = "Windows";
#endif
#ifdef Q_OS_LINUX
  os_name_long = "Linux";
  os_name_short = "Linux";
#endif
#ifdef Q_OS_MAC
  os_name_long = "MacOS X";
  os_name_short = "MacOSX";
#endif
#ifdef Q_OS_OS2
  os_name_long = "OS/2";
  os_name_short = "OS2";
#endif

  return use_long_name ? os_name_long : os_name_short;
}

QString Settings::operatingSystemIconPath() const
{
  QString os_icon_path = "Unknown OS";

#ifdef Q_OS_WIN
  os_icon_path = ":/images/windows.png";
#endif
#ifdef Q_OS_LINUX
  os_icon_path = ":/images/linux.png";
#endif
#ifdef Q_OS_MAC
  os_icon_path = ":/images/macosx.png";
#endif
#ifdef Q_OS_OS2
  os_icon_path = ":/images/os2.png";
#endif
  return os_icon_path;
}

QString Settings::checkVersionWebSite() const
{
  QString os_type = operatingSystem( false ).toLower();
  return officialWebSite() + QString( "%1?beebeep-version=%2&beebeep-os=%3" ).arg( QString( BEEBEEP_CHECK_VERSION_WEBSITE ) ).arg( QString( BEEBEEP_VERSION ) ).arg( os_type );
}

QString Settings::languageFilePath( const QString& language_folder, const QString& language_selected ) const
{
  return QString( "%1/%2_%3.qm" ).arg( language_folder, Settings::instance().programName().toLower(), language_selected );
}

QString Settings::dnsRecord() const
{
  return QString( BEEBEEP_DNS_RECORD );
}

QString Settings::facebookPage() const
{
  return QString( BEEBEEP_PAGE_ON_FACEBOOK );
}

QByteArray Settings::hash( const QString& string_to_hash ) const
{
  QByteArray hash_pre = string_to_hash.toUtf8() + m_password;
  QByteArray hash_generated = QCryptographicHash::hash( hash_pre, QCryptographicHash::Sha1 );
  return hash_generated.toHex();
}

void Settings::setPassword( const QString& new_value )
{
  m_passwordBeforeHash = new_value;
  m_password = QCryptographicHash::hash( QString( (new_value.isEmpty() || new_value == defaultPassword()) ? "*6475*" : new_value ).toUtf8(), QCryptographicHash::Sha1 ).toHex();
}

QString Settings::currentHash() const
{
  return QString::fromUtf8( hash( m_localUser.name() ) );
}

QHostAddress Settings::hostAddressToListen()
{
#if QT_VERSION >= 0x050000
  return m_useIPv6 ? QHostAddress::Any : QHostAddress::AnyIPv4;
#else
  return m_useIPv6 ? QHostAddress::AnyIPv6 : QHostAddress::Any;
#endif
}

bool Settings::addBroadcastAddressInSettings( const QString& host_address )
{
  if( m_broadcastAddressesInFileHosts.contains( host_address ) )
    return false;

  if( m_broadcastAddressesInSettings.contains( host_address ) )
    return false;

  m_broadcastAddressesInSettings.append( host_address );
  return true;
}

int Settings::setBroadcastAddressesInSettings( const QStringList& address_list )
{
  int num_addresses = 0;
  m_broadcastAddressesInSettings.clear();
  if( address_list.isEmpty() )
    return num_addresses;

  foreach( QString sa, address_list )
  {
    if( addBroadcastAddressInSettings( sa ) )
      num_addresses++;
  }

  return num_addresses;
}

void Settings::setLocalUserHost( const QHostAddress& host_address, int host_port )
{
  if( host_address.toString() == QString( "0.0.0.0" ) )
    m_localUser.setHostAddress( QHostAddress( "127.0.0.1") );
  else
    m_localUser.setHostAddress( host_address );
  m_localUser.setHostPort( host_port );
}

void Settings::loadBroadcastAddressesFromFileHosts()
{
  if( !m_broadcastAddressesInFileHosts.isEmpty() )
    m_broadcastAddressesInFileHosts.clear();

  QFile file( defaultHostsFilePath( true ) );
  if( !file.open( QIODevice::ReadOnly ) )
  {
    qDebug() << "File HOSTS not found in current path:" << qPrintable( file.fileName() );
    file.setFileName( defaultHostsFilePath( false ) );
    if( !file.open( QIODevice::ReadOnly ) )
    {
      qDebug() << "File HOSTS not found in custom path:" << qPrintable( file.fileName() );
      return;
    }
  }

  qDebug() << "Reading HOSTS from file" << file.fileName();
  QString address_string;
  QString line_read;
  int num_lines = 0;
  char c;
  int hosts_found = 0;

  while( !file.atEnd() )
  {
    num_lines++;
    line_read = file.readLine();
    if( line_read.size() > 0 )
    {
      address_string = line_read.simplified();

      if( address_string.size() > 0 )
      {
        c = address_string.at( 0 ).toLatin1();
        if( c == '#' || c == '/' || c == '*' )
          continue;

        NetworkAddress na = NetworkAddress::fromString( address_string );
        if( !na.isValid() )
        {
          qWarning() << "Invalid broadcast address found in line" << num_lines << ":" << address_string;
          continue;
        }

        if( m_broadcastAddressesInFileHosts.contains( address_string ) )
        {
          qDebug() << "Broadcast address is already in list";
          continue;
        }

        qDebug() << "Adding broadcast address:" << address_string;
        m_broadcastAddressesInFileHosts << address_string;
        hosts_found++;
      }
    }
  }

  qDebug() << "HOSTS file read:" << hosts_found << "IP addresses found";
  file.close();
}

QString Settings::currentSettingsFilePath() const
{
  if( m_useSettingsFileIni )
    return defaultSettingsFilePath();
  else
    return QLatin1String( "Native OS Settings" );
}

QSettings* Settings::objectSettings() const
{
  QSettings *sets;

  if( m_useSettingsFileIni )
    sets = new QSettings( defaultSettingsFilePath(), QSettings::IniFormat );
  else
    sets = new QSettings( QSettings::NativeFormat, QSettings::UserScope, organizationName(), programName() );

  if( !sets->isWritable() )
    qWarning() << sets->fileName() << "is not a writable path. Settings cannot be saved.";

  return sets;
}

void Settings::load()
{
  qDebug() << "Creating local user and loading settings";
  QSettings *sets = objectSettings();

  m_firstTime = sets->allKeys().isEmpty();
  sets->beginGroup( "Version" );
  m_settingsVersion = sets->value( "Settings", BEEBEEP_SETTINGS_VERSION ).toInt();
  m_dataStreamVersion = sets->value( "DataStream", (int)DATASTREAM_VERSION_1 ).toInt();
  m_settingsCreationDate = sets->value( "BeeBang", QDate() ).toDate();
  if( m_settingsCreationDate.isNull() )
    m_settingsCreationDate = QDate::currentDate();
  QString qt_version_in_settings = sets->value( "Qt", qtMajorVersion() ).toString();
  sets->endGroup();

  bool qt_is_compatible = qt_version_in_settings == qtMajorVersion();

  sets->beginGroup( "Chat" );
  m_chatFont.fromString( sets->value( "Font", QApplication::font().toString() ).toString() );
  setChatFont( m_chatFont );
  m_chatFontColor = sets->value( "FontColor", QColor( Qt::black ).name() ).toString();
  m_defaultChatBackgroundColor = sets->value( "DefaultChatBackgroundColor", QColor( Qt::white ).name() ).toString();
  m_chatCompact = sets->value( "CompactMessage", true ).toBool();
  m_chatAddNewLineToMessage = sets->value( "AddNewLineAfterMessage", false ).toBool();
  m_chatShowMessageTimestamp = sets->value( "ShowMessageTimestamp", false ).toBool();
  m_beepOnNewMessageArrived = sets->value( "BeepOnNewMessageArrived", true ).toBool();
  m_beepAlwaysOnNewMessageArrived = sets->value( "BeepAlwaysOnNewMessageArrived", false ).toBool();
  m_chatUseHtmlTags = sets->value( "UseHtmlTags", false ).toBool();
  m_chatUseClickableLinks = sets->value( "UseClickableLinks", true ).toBool();
  m_chatMessageHistorySize = sets->value( "MessageHistorySize", 10 ).toInt();
  m_showEmoticons = sets->value( "ShowEmoticons", true ).toBool();
  m_showMessagesGroupByUser = sets->value( "ShowMessagesGroupByUsers", true ).toBool();
  m_autoLinkSavedChatByNickname = sets->value( "AutoLinkSavedChatByNickname", true ).toBool();
  m_chatMessageFilter = sets->value( "MessageFilter", QBitArray() ).toBitArray();
  if( m_chatMessageFilter.size() < (int)ChatMessage::NumTypes )
    m_chatMessageFilter.resize( (int)ChatMessage::NumTypes );
  m_showOnlyMessagesInDefaultChat = sets->value( "ShowOnlyMessagesInDefaultChat", true ).toBool();
  m_showImagePreview = sets->value( "ShowImagePreview", true ).toBool();
  m_chatMessagesToShow = sets->value( "ChatMessagesToShow", 80 ).toInt();
  m_chatMaxMessagesToShow = sets->value( "ChatMaxMessagesToShow", false ).toBool();
  m_imagePreviewHeight = qMax( 48, sets->value( "ImagePreviewHeight", 160 ).toInt() );
  m_useReturnToSendMessage = sets->value( "UseKeyReturnToSendMessage", m_useReturnToSendMessage ).toBool();
  sets->endGroup();

  sets->beginGroup( "User" );
  m_localUser.setName( sets->value( "LocalName", "" ).toString() ); // For Backward compatibility, if empty the name is set after
  m_localUser.setColor( sets->value( "LocalColor", "#000000" ).toString() );
  m_localUser.setStatus( sets->value( "LocalLastStatus", m_localUser.status() ).toInt() );
  m_localUser.setStatusDescription( sets->value( "LocalLastStatusDescription", m_localUser.statusDescription() ).toString() );
  m_autoUserAway = sets->value( "AutoAwayStatus", true ).toBool();
  m_userAwayTimeout = qMax( sets->value( "UserAwayTimeout", 10 ).toInt(), 1 ); // minutes
  m_useDefaultPassword = sets->value( "UseDefaultPassword", true ).toBool();
  m_askPasswordAtStartup = sets->value( "AskPasswordAtStartup", true ).toBool();
  m_savePassword = sets->value( "SavePassword", false ).toBool();
  QString enc_pass = "";
  if( m_savePassword )
    enc_pass = simpleDecrypt( sets->value( "EncPwd", "" ).toString() );
  setPassword( enc_pass );
  m_saveUserList = sets->value( "SaveUsers", m_saveUserList ).toBool();
  QString user_list = sets->value( "List", "" ).toString();
  if( !user_list.isEmpty() )
    m_userList = simpleDecrypt( user_list ).split( QString( "\n" ) );
  else
    m_userList = QStringList();
  QString user_status_list = sets->value( "StatusList", "" ).toString();
  if( !user_status_list.isEmpty() )
    m_userStatusList = simpleDecrypt( user_status_list ).split( QString( "\n" ) );
  else
    m_userStatusList = QStringList();
  m_maxUserStatusInList = sets->value( "MaxStatusInList", m_maxUserStatusInList ).toInt();
  sets->endGroup();

  sets->beginGroup( "VCard" );
  VCard vc;
  vc.setNickName( sets->value( "NickName", m_localUser.name() ).toString() );
  vc.setFirstName( sets->value( "FirstName", "" ).toString() );
  vc.setLastName( sets->value( "LastName", "" ).toString() );
  QDate dt = sets->value( "Birthday", QDate() ).toDate();
  if( dt.isValid() )
    vc.setBirthday( dt );
  vc.setEmail( sets->value( "Email", "" ).toString() );
  QPixmap pix = sets->value( "Photo", QPixmap() ).value<QPixmap>();
  if( !pix.isNull() )
    vc.setPhoto( pix );
  vc.setPhoneNumber( sets->value( "Phone", "" ).toString() );
  vc.setInfo( sets->value( "Info", "" ).toString() );
  m_localUser.setVCard( vc );
  sets->endGroup();

  sets->beginGroup( "Gui" );
  if( qt_is_compatible )
    m_resetGeometryAtStartup = sets->value( "ResetGeometryAtStartup", m_resetGeometryAtStartup ).toBool();
  else
    m_resetGeometryAtStartup = true;

  if( m_resetGeometryAtStartup )
  {
    m_guiGeometry = "";
    m_guiState = "";
    m_chatSplitterState = "";
    m_floatingChatGeometry = "";
    m_floatingChatState = "";
    m_floatingChatSplitterState = "";
    m_mainBarIconSize = QSize( 24, 24 );
    m_avatarIconSize = QSize( 32, 32 );
  }
  else
  {
    m_guiGeometry = sets->value( "MainWindowGeometry", "" ).toByteArray();
    m_guiState = sets->value( "MainWindowState", "" ).toByteArray();
    m_chatSplitterState = sets->value( "ChatSplitterState", "" ).toByteArray();
    m_floatingChatGeometry = sets->value( "FloatingChatGeometry", "" ).toByteArray();
    m_floatingChatState = sets->value( "FloatingChatState", "" ).toByteArray();
    m_floatingChatSplitterState = sets->value( "FloatingChatSplitterState", "" ).toByteArray();
    m_mainBarIconSize = sets->value( "MainBarIconSize", QSize( 24, 24 ) ).toSize();
    m_avatarIconSize = sets->value( "AvatarIconSize", QSize( 32, 32 ) ).toSize();
  }
  m_language = sets->value( "Language", QLocale::system().name() ).toString();
  if( m_language.size() > 2 )
    m_language.resize( 2 );
#if QT_VERSION >= 0x050000
  m_lastDirectorySelected = Bee::convertToNativeFolderSeparator( sets->value( "LastDirectorySelected", QStandardPaths::writableLocation( QStandardPaths::DocumentsLocation ) ).toString() );
  m_downloadDirectory = Bee::convertToNativeFolderSeparator( sets->value( "DownloadDirectory", QStandardPaths::writableLocation( QStandardPaths::DownloadLocation ) ).toString() );
#else
  m_lastDirectorySelected = Bee::convertToNativeFolderSeparator( sets->value( "LastDirectorySelected", QDesktopServices::storageLocation( QDesktopServices::DocumentsLocation ) ).toString() );
  m_downloadDirectory = Bee::convertToNativeFolderSeparator( sets->value( "DownloadDirectory", QDesktopServices::storageLocation( QDesktopServices::DocumentsLocation ) ).toString() );
#endif
  m_logPath = Bee::convertToNativeFolderSeparator( sets->value( "LogPath", dataFolder() ).toString() );
  m_pluginPath = Bee::convertToNativeFolderSeparator( sets->value( "PluginPath", defaultPluginFolderPath( true ) ).toString() );
  m_languagePath = Bee::convertToNativeFolderSeparator( sets->value( "LanguagePath", m_resourceFolder ).toString() );
  m_keyEscapeMinimizeInTray = sets->value( "KeyEscapeMinimizeInTray", false ).toBool();
  m_minimizeInTray = sets->value( "MinimizeInTray", true ).toBool();
  m_stayOnTop = sets->value( "StayOnTop", false ).toBool();
  m_raiseOnNewMessageArrived = sets->value( "RaiseOnNewMessageArrived", false ).toBool();
  m_beepFilePath = Bee::convertToNativeFolderSeparator( sets->value( "BeepFilePath", defaultBeepFilePath( true ) ).toString() );
  m_loadOnTrayAtStartup = sets->value( "LoadOnTrayAtStartup", false ).toBool();
  m_showNotificationOnTray = sets->value( "ShowNotificationOnTray", true ).toBool();
  m_showOnlyMessageNotificationOnTray = sets->value( "ShowOnlyMessageNotificationOnTray", false ).toBool();
  m_trayMessageTimeout = qMax( sets->value( "ShowNotificationOnTrayTimeout", 2000 ).toInt(), 100 );
  m_chatSaveDirectory = Bee::convertToNativeFolderSeparator( sets->value( "ChatSaveDirectory", dataFolder() ).toString() );
  m_chatAutoSave = sets->value( "ChatAutoSave", true ).toBool();
  m_chatMaxLineSaved = sets->value( "ChatMaxLineSaved", 3000 ).toInt();
  m_showChatToolbar = sets->value( "ShowChatToolbar", true ).toBool();
  m_showHomeAsDefaultPage = sets->value( "ShowHomeAsDefaultPage", true ).toBool();
  m_showTipsOfTheDay = sets->value( "ShowTipsOfTheDay", true ).toBool();
  m_showOnlyOnlineUsers = sets->value( "ShowOnlyOnlineUsers", true ).toBool();
  m_showUserColor = sets->value( "ShowUserNameColor", true ).toBool();
  m_showUserPhoto = sets->value( "ShowUserPhoto", true ).toBool();
  m_showVCardOnRightClick = sets->value( "ShowVCardOnRightClick", true ).toBool();
  m_showEmoticonMenu = sets->value( "ShowEmoticonMenu", false ).toBool();
  m_emoticonSizeInEdit = qMax( m_emoticonSizeInEdit, (int)sets->value( "EmoticonSizeInEdit", m_emoticonSizeInEdit ).toInt() );
  m_emoticonSizeInChat = qMax( m_emoticonSizeInChat, (int)sets->value( "EmoticonSizeInChat", m_emoticonSizeInChat ).toInt() );
  m_emoticonSizeInMenu = sets->value( "EmoticonSizeInMenu", m_emoticonSizeInMenu ).toInt();
  m_emoticonInRecentMenu = sets->value( "EmoticonInRecentMenu", m_emoticonInRecentMenu ).toInt();
  m_recentEmoticons = sets->value( "RecentEmoticons", QStringList() ).toStringList();
  m_useNativeEmoticons = sets->value( "UseNativeEmoticons", m_useNativeEmoticons ).toBool();
  m_showMinimizedAtStartup = sets->value( "ShowMinimizedAtStartup", false ).toBool();
  m_promptOnCloseEvent = sets->value( "PromptOnCloseEvent", m_promptOnCloseEvent ).toBool();
  m_isFacebookPageLinkClicked = sets->value( "FacebookPageLinkClicked", false ).toBool();
  m_alwaysOpenNewFloatingChat = sets->value( "AlwaysOpenNewFloatingChat", m_alwaysOpenNewFloatingChat ).toBool();
  m_showUserStatusBackgroundColor = sets->value( "ShowUserStatusBackgroundColor", false ).toBool();
  m_shortcuts = sets->value( "Shortcuts", QStringList() ).toStringList();
  m_useShortcuts = sets->value( "UseShortcuts", false ).toBool();
  sets->endGroup();

  sets->beginGroup( "Tools" );
  m_logToFile = sets->value( "LogToFile", false ).toBool();
  m_useSpellChecker = sets->value( "UseSpellChecker", false ).toBool();
  m_useWordCompleter = sets->value( "UseWordCompleter", false ).toBool();
  m_dictionaryPath = sets->value( "DictionaryPath", resourceFolder() ).toString();
  sets->endGroup();

  sets->beginGroup( "Misc" );
  m_broadcastInterval = sets->value( "BroadcastInterval", 0 ).toInt();
  m_broadcastLoopbackInterval = sets->value( "BroadcastLoopbackInterval", 2000 ).toInt();
  m_localUser.setHostPort( sets->value( "ListenerPort", DEFAULT_LISTENER_PORT ).toInt() );
  m_pingInterval = qMax( sets->value( "PingInterval", 31000 ).toInt(), 1000 );
  m_pongTimeout = qMax( sets->value( "PongTimeout", 98000 ).toInt(), 3000 );
  m_writingTimeout = qMax( sets->value( "WritingTimeout", 3000 ).toInt(), 1000 );
  int mod_buffer_size = m_fileTransferBufferSize % ENCRYPTED_DATA_BLOCK_SIZE; // For a corrected encryption
  if( mod_buffer_size > 0 )
    m_fileTransferBufferSize -= mod_buffer_size;
  m_connectionTimeout = qMax( sets->value( "ConnectionTimeout", m_connectionTimeout ).toInt(), 1000 );
  sets->endGroup();

  sets->beginGroup( "Network");
  m_broadcastAddressesInSettings = sets->value( "BroadcastAddresses", QStringList() ).toStringList();
  if( !m_broadcastAddressesInSettings.isEmpty() )
    m_broadcastAddressesInSettings.removeDuplicates();
  QString local_host_address = sets->value( "LocalHostAddressForced", "" ).toString();
  if( !local_host_address.isEmpty() )
    m_localHostAddressForced = QHostAddress( local_host_address );
  m_localSubnetForced = sets->value( "LocalSubnetForced", "" ).toString();
  m_parseBroadcastAddresses = sets->value( "ParseBroadcastAddresses", true ).toBool();
  m_addExternalSubnetAutomatically = sets->value( "AddExternalSubnetAutomatically", true ).toBool();
  m_userPathList = sets->value( "UserPathList", QStringList() ).toStringList();
  m_acceptConnectionsOnlyFromWorkgroups = sets->value( "AcceptConnectionsOnlyFromWorkgroups", m_acceptConnectionsOnlyFromWorkgroups ).toBool();
  m_workgroups = sets->value( "Workgroups", QStringList() ).toStringList();
#ifdef BEEBEEP_USE_MULTICAST_DNS
  m_useMulticastDns = sets->value( "UseMulticastDns", m_useMulticastDns ).toBool();
#endif
  sets->endGroup();
  loadBroadcastAddressesFromFileHosts();

  sets->beginGroup( "FileShare" );
  m_fileTransferIsEnabled = sets->value( "FileTransferIsEnabled", true ).toBool();
  m_maxSimultaneousDownloads = sets->value( "MaxSimultaneousDownloads", 3 ).toInt();
  m_maxQueuedDownloads = sets->value( "MaxQueuedDownloads", 200 ).toInt();
  m_fileTransferConfirmTimeout = qMax( sets->value( "FileTransferConfirmTimeout", 30000 ).toInt(), 1000 );
  m_fileTransferBufferSize = qMax( sets->value( "FileTransferBufferSize", 65456 ).toInt(), 2048 );
  m_maxFileShared = qMax( 0, sets->value( "MaxFileShared", MAX_NUM_FILE_SHARED ).toInt() );
  m_automaticFileName = sets->value( "SetAutomaticFileNameOnSave", true ).toBool();
  m_confirmOnDownloadFile = sets->value( "ConfirmOnDownloadFile", m_confirmOnDownloadFile ).toBool();
  QStringList local_share = sets->value( "ShareList", QStringList() ).toStringList();
  if( !local_share.isEmpty() )
  {
    foreach( QString share_path, local_share )
      m_localShare.append( Bee::convertToNativeFolderSeparator( share_path ) );
  }
  else
    m_localShare = local_share;
  sets->endGroup();

  sets->beginGroup( "Group" );
  m_groupSilenced = sets->value( "Silenced", QStringList() ).toStringList();
  m_groupList = sets->value( "List", QStringList() ).toStringList();
  sets->endGroup();

  sets->beginGroup( "Plugin" );
  QStringList key_list = sets->value( "List", QStringList() ).toStringList();
  if( !key_list.isEmpty() )
  {
    QStringList plugin_settings_tmp;
    QStringList::const_iterator it = key_list.constBegin();
    while( it != key_list.constEnd() )
    {
      plugin_settings_tmp = sets->value( *it, QStringList() ).toStringList();
      if( !plugin_settings_tmp.isEmpty() )
        setPluginSettings( *it, plugin_settings_tmp );
      ++it;
    }
  }
  sets->endGroup();

  m_lastSave = QDateTime::currentDateTime();
  sets->deleteLater();
}

QString Settings::qtMajorVersion() const
{
  QString qt_version( qVersion() );
  QStringList sl_version = qt_version.split( "." );
  if( sl_version.isEmpty() )
    return QString( "0" );
  else
    return sl_version.at( 0 );
}

void Settings::save()
{
  QSettings *sets = objectSettings();

  sets->clear();

  sets->beginGroup( "Version" );
  sets->setValue( "Program", version( true ) );
  sets->setValue( "Proto", protoVersion() );
  sets->setValue( "Settings", BEEBEEP_SETTINGS_VERSION );
  sets->setValue( "DataStream", (int)dataStreamVersion( false ) );
  sets->setValue( "BeeBang", m_settingsCreationDate );
  sets->setValue( "Qt", qtMajorVersion() );
  sets->endGroup();
  sets->beginGroup( "Chat" );
  sets->setValue( "Font", m_chatFont.toString() );
  sets->setValue( "FontColor", m_chatFontColor );
  sets->setValue( "DefaultChatBackgroundColor", m_defaultChatBackgroundColor );
  sets->setValue( "CompactMessage", m_chatCompact );
  sets->setValue( "AddNewLineAfterMessage", m_chatAddNewLineToMessage );
  sets->setValue( "ShowMessageTimestamp", m_chatShowMessageTimestamp );
  sets->setValue( "BeepOnNewMessageArrived", m_beepOnNewMessageArrived );
  sets->setValue( "BeepAlwaysOnNewMessageArrived", m_beepAlwaysOnNewMessageArrived );
  sets->setValue( "UseHtmlTags", m_chatUseHtmlTags );
  sets->setValue( "UseClickableLinks", m_chatUseClickableLinks );
  sets->setValue( "MessageHistorySize", m_chatMessageHistorySize );
  sets->setValue( "ShowEmoticons", m_showEmoticons );
  sets->setValue( "ShowMessagesGroupByUsers", m_showMessagesGroupByUser );
  sets->setValue( "AutoLinkSavedChatByNickname", m_autoLinkSavedChatByNickname );
  sets->setValue( "MessageFilter", m_chatMessageFilter );
  sets->setValue( "ShowOnlyMessagesInDefaultChat", m_showOnlyMessagesInDefaultChat );
  sets->setValue( "ShowImagePreview", m_showImagePreview );
  sets->setValue( "ChatMessagesToShow", m_chatMessagesToShow );
  sets->setValue( "ChatMaxMessagesToShow", m_chatMaxMessagesToShow );
  sets->setValue( "ImagePreviewHeight", m_imagePreviewHeight );
  sets->setValue( "UseKeyReturnToSendMessage", m_useReturnToSendMessage );
  sets->endGroup();
  sets->beginGroup( "User" );
  sets->setValue( "LocalColor", m_localUser.color() );
  sets->setValue( "LocalLastStatus", (int)(m_localUser.status() == User::Offline ? User::Online : m_localUser.status()) );
  sets->setValue( "LocalLastStatusDescription", m_localUser.statusDescription() );
  sets->setValue( "AutoAwayStatus", m_autoUserAway );
  sets->setValue( "UserAwayTimeout", m_userAwayTimeout ); // minutes
  sets->setValue( "UseDefaultPassword", m_useDefaultPassword );
  sets->setValue( "AskPasswordAtStartup", m_askPasswordAtStartup );
  if( m_savePassword )
  {
    sets->setValue( "SavePassword", true );
    sets->setValue( "EncPwd", simpleEncrypt( m_passwordBeforeHash ) );
  }
  else
  {
    sets->remove( "SavePassword" );
    sets->remove( "EncPwd" );
  }

  sets->setValue( "SaveUsers", m_saveUserList );
  if( m_saveUserList )
  {
    // I need a QString to avoid " in file ini
    QString user_list_to_save = simpleEncrypt( m_userList.join( QString( "\n" ) ) );
    sets->setValue( "List", user_list_to_save );
  }
  else
    sets->remove( "List" );

  if( !m_userStatusList.isEmpty() )
  {
    QString user_status_list_to_save = simpleEncrypt( m_userStatusList.join( QString( "\n" ) ) );
    sets->setValue( "StatusList", user_status_list_to_save );
  }
  else
    sets->remove( "StatusList" );

  sets->setValue( "MaxStatusInList", m_maxUserStatusInList );
  sets->endGroup();

  sets->beginGroup( "VCard" );
  sets->setValue( "NickName", m_localUser.vCard().nickName() );
  sets->setValue( "FirstName", m_localUser.vCard().firstName() );
  sets->setValue( "LastName", m_localUser.vCard().lastName() );
  sets->setValue( "Birthday", m_localUser.vCard().birthday() );
  sets->setValue( "Email", m_localUser.vCard().email() );
  sets->setValue( "Photo", m_localUser.vCard().photo() );
  sets->setValue( "Phone", m_localUser.vCard().phoneNumber() );
  sets->setValue( "Info", m_localUser.vCard().info() );
  sets->endGroup();
  sets->beginGroup( "Gui" );
  sets->setValue( "MainWindowGeometry", m_guiGeometry );
  sets->setValue( "MainWindowState", m_guiState );
  sets->setValue( "ChatSplitterState", m_chatSplitterState );
  sets->setValue( "FloatingChatGeometry", m_floatingChatGeometry );
  sets->setValue( "FloatingChatState", m_floatingChatState );
  sets->setValue( "FloatingChatSplitterState", m_floatingChatSplitterState );
  sets->setValue( "MainBarIconSize", m_mainBarIconSize );
  sets->setValue( "AvatarIconSize", m_avatarIconSize );
  sets->setValue( "Language", m_language );
  sets->setValue( "LastDirectorySelected", m_lastDirectorySelected );
  sets->setValue( "DownloadDirectory", m_downloadDirectory );
  sets->setValue( "LogPath", m_logPath );
  sets->setValue( "PluginPath", m_pluginPath );
  sets->setValue( "LanguagePath", m_languagePath );
  sets->setValue( "KeyEscapeMinimizeInTray", m_keyEscapeMinimizeInTray );
  sets->setValue( "MinimizeInTray", m_minimizeInTray );
  sets->setValue( "StayOnTop", m_stayOnTop );
  sets->setValue( "BeepFilePath", m_beepFilePath );
  sets->setValue( "RaiseOnNewMessageArrived", m_raiseOnNewMessageArrived );
  sets->setValue( "LoadOnTrayAtStartup", m_loadOnTrayAtStartup );
  sets->setValue( "ShowNotificationOnTray", m_showNotificationOnTray );
  sets->setValue( "ShowOnlyMessageNotificationOnTray", m_showOnlyMessageNotificationOnTray );
  sets->setValue( "ShowNotificationOnTrayTimeout", m_trayMessageTimeout );
  sets->setValue( "ChatSaveDirectory", m_chatSaveDirectory );
  sets->setValue( "ChatAutoSave", m_chatAutoSave );
  sets->setValue( "ChatMaxLineSaved", m_chatMaxLineSaved );
  sets->setValue( "ShowChatToolbar", m_showChatToolbar );
  sets->setValue( "ShowHomeAsDefaultPage", m_showHomeAsDefaultPage );
  sets->setValue( "ShowTipsOfTheDay", m_showTipsOfTheDay );
  sets->setValue( "ShowOnlyOnlineUsers", m_showOnlyOnlineUsers );
  sets->setValue( "ShowUserNameColor", m_showUserColor );
  sets->setValue( "ShowUserPhoto", m_showUserPhoto );
  sets->setValue( "ShowVCardOnRightClick", m_showVCardOnRightClick );
  sets->setValue( "ResetGeometryAtStartup", m_resetGeometryAtStartup );
  sets->setValue( "ShowEmoticonMenu", m_showEmoticonMenu );
  sets->setValue( "EmoticonSizeInEdit", m_emoticonSizeInEdit );
  sets->setValue( "EmoticonSizeInChat", m_emoticonSizeInChat );
  sets->setValue( "EmoticonSizeInMenu", m_emoticonSizeInMenu );
  sets->setValue( "EmoticonInRecentMenu", m_emoticonInRecentMenu );
  sets->setValue( "RecentEmoticons", m_recentEmoticons );
  sets->setValue( "UseNativeEmoticons", m_useNativeEmoticons );
  sets->setValue( "ShowMinimizedAtStartup", m_showMinimizedAtStartup );
  sets->setValue( "PromptOnCloseEvent", m_promptOnCloseEvent );
  sets->setValue( "FacebookPageLinkClicked", m_isFacebookPageLinkClicked );
  sets->setValue( "AlwaysOpenNewFloatingChat", m_alwaysOpenNewFloatingChat );
  sets->setValue( "ShowUserStatusBackgroundColor", m_showUserStatusBackgroundColor );
  sets->setValue( "Shortcuts", m_shortcuts );
  sets->setValue( "UseShortcuts", m_useShortcuts );
  sets->endGroup();
  sets->beginGroup( "Tools" );
  sets->setValue( "LogToFile", m_logToFile );
  sets->setValue( "UseSpellChecker", m_useSpellChecker );
  sets->setValue( "UseWordCompleter", m_useWordCompleter );
  sets->setValue( "DictionaryPath", m_dictionaryPath );
  sets->endGroup();
  sets->beginGroup( "Misc" );
  sets->setValue( "BroadcastInterval", m_broadcastInterval );
  sets->setValue( "BroadcastLoopbackInterval", m_broadcastLoopbackInterval );
  sets->setValue( "ListenerPort", m_localUser.hostPort() );
  sets->setValue( "PingInterval", m_pingInterval );
  sets->setValue( "PongTimeout", m_pongTimeout );
  sets->setValue( "WritingTimeout", m_writingTimeout );
  sets->setValue( "ConnectionTimeout", m_connectionTimeout );
  sets->endGroup();
  sets->beginGroup( "Network");
#ifdef BEEBEEP_USE_MULTICAST_DNS
  sets->setValue( "UseMulticastDns", m_useMulticastDns );
#endif
  sets->setValue( "PreventMultipleConnectionsFromSingleHostAddress", m_preventMultipleConnectionsFromSingleHostAddress );
  sets->setValue( "BroadcastAddresses", m_broadcastAddressesInSettings );
  if( !m_localHostAddressForced.isNull() )
    sets->setValue( "LocalHostAddressForced", m_localHostAddressForced.toString() );
  else
    sets->setValue( "LocalHostAddressForced", QString( "" ) );
  sets->setValue( "LocalSubnetForced", m_localSubnetForced );
  sets->setValue( "ParseBroadcastAddresses", m_parseBroadcastAddresses );
  sets->setValue( "AddExternalSubnetAutomatically", m_addExternalSubnetAutomatically );
  sets->setValue( "UserPathList", m_userPathList );
  sets->setValue( "AcceptConnectionsOnlyFromWorkgroups", m_acceptConnectionsOnlyFromWorkgroups );
  sets->setValue( "Workgroups", m_workgroups );
  sets->endGroup();
  sets->beginGroup( "FileShare" );
  sets->setValue( "FileTransferIsEnabled", m_fileTransferIsEnabled );
  sets->setValue( "SetAutomaticFileNameOnSave", m_automaticFileName );
  sets->setValue( "MaxFileShared", m_maxFileShared );
  sets->setValue( "FileTransferConfirmTimeout", m_fileTransferConfirmTimeout );
  sets->setValue( "FileTransferBufferSize", m_fileTransferBufferSize );
  sets->setValue( "MaxSimultaneousDownloads", m_maxSimultaneousDownloads );
  sets->setValue( "MaxQueuedDownloads", m_maxQueuedDownloads );
  sets->setValue( "ConfirmOnDownloadFile", m_confirmOnDownloadFile );
  sets->setValue( "ShareList", m_localShare );
  sets->endGroup();

  if( !m_groupList.isEmpty() )
  {
    sets->beginGroup( "Group" );
    sets->setValue( "Silenced", m_groupSilenced );
    sets->setValue( "List", m_groupList );
    sets->endGroup();
  }

  if( !m_pluginSettings.isEmpty() )
  {
    sets->beginGroup( "Plugin" );
    QStringList key_list( m_pluginSettings.keys() );
    sets->setValue( "List", key_list );
    QMap<QString, QStringList>::const_iterator it = m_pluginSettings.constBegin();
    while( it !=  m_pluginSettings.constEnd() )
    {
      sets->setValue( it.key(), it.value() );
      ++it;
    }
    sets->endGroup();
  }

  if( sets->isWritable() )
  {
    sets->sync();
    qDebug() << "Settings saved in" << qPrintable( sets->fileName() );
    m_lastSave = QDateTime::currentDateTime();
  }
  sets->deleteLater();
}

bool Settings::askPassword() const
{
  if( askPasswordAtStartup() )
    return true;

  if( useDefaultPassword() )
    return false;

  if( !savePassword() )
    return true;

  if( m_passwordBeforeHash.isEmpty() )
    return true;

  return false;
}

void Settings::setLastDirectorySelectedFromFile( const QString& file_path )
{
  QFileInfo file_info( file_path );
  setLastDirectorySelected( file_info.absoluteDir().absolutePath() );
}

void Settings::addTemporaryFilePath( const QString& file_path )
{
  if( !m_tempFilePathList.contains( file_path ) )
  {
    m_tempFilePathList.append( file_path );
    qDebug() << "Temporary file added:" << qPrintable( file_path );
  }
}

void Settings::clearTemporaryFile()
{
  if( m_tempFilePathList.isEmpty() )
    return;

  foreach( QString file_path, m_tempFilePathList )
  {
    if( QFile::exists( file_path ) )
    {
      if( !QFile::remove( file_path ) )
        qWarning() << "Unable to remove temporary file:" << qPrintable( file_path );
      else
        qDebug() << "Temporary file removed:" << qPrintable( file_path );
    }
  }

  m_tempFilePathList.clear();
}

void Settings::setNotificationEnabledForGroup( const QString& group_id, bool enable_notification )
{
  if( group_id.isEmpty() )
  {
    qWarning() << "Empty private group id in set notification enabled";
    return;
  }

  if( enable_notification )
  {
    m_groupSilenced.removeOne( group_id );
  }
  else
  {
    if( !m_groupSilenced.contains( group_id ) )
      m_groupSilenced.append( group_id );
  }
}

bool Settings::addStartOnSystemBoot()
{
#ifdef Q_OS_WIN
  QString program_path = Bee::convertToNativeFolderSeparator( qApp->applicationFilePath() );
  QSettings sets( "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat );
  if( sets.isWritable() )
  {
    sets.setValue( programName(), program_path );
    sets.sync();
    return true;
  }
  qWarning() << "Unable to add auto start in registry key:" << qPrintable( sets.fileName() );
#endif
  return false;
}

bool Settings::removeStartOnSystemBoot()
{
#ifdef Q_OS_WIN
  QSettings sets( "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat );
  if( sets.isWritable() )
  {
    sets.remove( programName() );
    sets.sync();
    return true;
  }
  qWarning() << "Unable to remove auto start from registry key:" << sets.fileName();
#endif
  return false;
}

bool Settings::hasStartOnSystemBoot() const
{
#ifdef Q_OS_WIN
  QString program_path = qApp->applicationFilePath().replace( "/", "\\" );
  QSettings sets( "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat );
  return sets.value( programName(), "" ).toString() == program_path;
#else
  return false;
#endif
}

void Settings::clearNativeSettings()
{
  QSettings sets( QSettings::NativeFormat, QSettings::UserScope, organizationName(), programName() );
  if( !sets.allKeys().isEmpty() )
    sets.clear();
}

void Settings::setResourceFolder()
{
  m_resourceFolder = Bee::convertToNativeFolderSeparator( QApplication::applicationDirPath() );

#ifdef Q_OS_MAC
  QDir macos_dir( m_resourceFolder );
  macos_dir.cdUp();
  macos_dir.cd( "Resources" );
  m_resourceFolder = macos_dir.absolutePath();
#endif

  qDebug() << "Resource folder:" << qPrintable( m_resourceFolder );
}

bool Settings::setDataFolder()
{
  m_dataFolder = Bee::convertToNativeFolderSeparator( m_resourceFolder );

  QFileInfo data_file_info( m_dataFolder );

  if( data_file_info.isWritable() && !m_saveDataInDocumentsFolder && !m_saveDataInUserApplicationFolder && m_dataFolderInRC.isEmpty() )
  {
    qDebug() << "Data folder:" << qPrintable( m_dataFolder );
    return true;
  }

  QString data_folder = m_addAccountNameToDataFolder ? accountNameFromSystemEnvinroment() : QLatin1String( "beebeep-data" );
  QString root_folder;

#if QT_VERSION >= 0x050400
  if( !m_dataFolderInRC.isEmpty() )
    root_folder = m_dataFolderInRC;
  else if( m_saveDataInUserApplicationFolder )
    root_folder = QStandardPaths::writableLocation( QStandardPaths::AppDataLocation );
  else if( m_saveDataInDocumentsFolder )
    root_folder = QStandardPaths::writableLocation( QStandardPaths::DocumentsLocation );
  else
    root_folder = QStandardPaths::writableLocation( QStandardPaths::AppDataLocation );
#elif QT_VERSION >= 0x050000
  if( !m_dataFolderInRC.isEmpty() )
    root_folder = m_dataFolderInRC;
  else if( m_saveDataInUserApplicationFolder )
    root_folder = Bee::convertToNativeFolderSeparator( QString( "%1/%2" ).arg( QStandardPaths::writableLocation( QStandardPaths::DataLocation ) ).arg( programName() ) );
  else if( m_saveDataInDocumentsFolder )
    root_folder = QStandardPaths::writableLocation( QStandardPaths::DocumentsLocation );
  else
    root_folder = Bee::convertToNativeFolderSeparator( QString( "%1/%2" ).arg( QStandardPaths::writableLocation( QStandardPaths::DataLocation ) ).arg( programName() ) );
#else
  if( !m_dataFolderInRC.isEmpty() )
    root_folder = m_dataFolderInRC;
  else if( m_saveDataInUserApplicationFolder )
    root_folder = QDesktopServices::storageLocation( QDesktopServices::DataLocation );
  else if( m_saveDataInDocumentsFolder )
    root_folder = QDesktopServices::storageLocation( QDesktopServices::DocumentsLocation );
  else
    root_folder = QDesktopServices::storageLocation( QDesktopServices::DataLocation );
#endif

  if( m_addAccountNameToDataFolder || m_saveDataInDocumentsFolder )
    m_dataFolder = Bee::convertToNativeFolderSeparator( QString( "%1/%2" ).arg( root_folder, data_folder ) );
  else
    m_dataFolder = Bee::convertToNativeFolderSeparator( root_folder );

  QDir folder( m_dataFolder );
  if( !folder.exists() )
  {
    qWarning() << "Data folder not found in" << qPrintable( folder.absolutePath() );
    if( !folder.mkpath( m_dataFolder ) )
    {
      qWarning() << "Unable to create data folder" << qPrintable( folder.absolutePath() ) ;
      m_dataFolder = root_folder;
    }
    else
      qDebug() << "Data folder created in" << qPrintable( m_dataFolder );
  }

  QFileInfo folder_info( m_dataFolder );
  if( !folder_info.isWritable() )
  {
    qWarning() << "Data folder" << qPrintable( folder_info.absoluteFilePath() ) << "is not writeable";
    return false;
  }

  qDebug() << "Data folder is" << qPrintable(  m_dataFolder );
  return true;
}

QString Settings::savedChatsFilePath() const
{
  return Bee::convertToNativeFolderSeparator( QString( "%1/%2" ).arg( m_chatSaveDirectory ).arg( "beebeep.dat" ) );
}

QString Settings::defaultHostsFilePath( bool use_resource_folder ) const
{
  QString root_folder = use_resource_folder ? resourceFolder() : dataFolder();
  return Bee::convertToNativeFolderSeparator( QString( "%1/%2" ).arg( root_folder ).arg( QLatin1String( "beehosts.ini" ) ) );
}

QString Settings::defaultRcFilePath( bool use_resource_folder ) const
{
  QString root_folder = use_resource_folder ? resourceFolder() : dataFolder();
  return Bee::convertToNativeFolderSeparator( QString( "%1/%2" ).arg( root_folder ).arg( QLatin1String( "beebeep.rc" ) ) );
}

QString Settings::defaultSettingsFilePath() const
{
  return Bee::convertToNativeFolderSeparator( QString( "%1/%2" ).arg( dataFolder() ).arg( QLatin1String( "beebeep.ini" ) ) );
}

QString Settings::defaultBeepFilePath( bool use_resource_folder ) const
{
  QString root_folder = use_resource_folder ? resourceFolder() : dataFolder();
  return Bee::convertToNativeFolderSeparator( QString( "%1/%2" ).arg( root_folder ).arg( QLatin1String( "beep.wav" ) ) );
}

QString Settings::defaultPluginFolderPath( bool use_resource_folder ) const
{
#ifdef Q_OS_MAC
  QString macx_plugins_path = resourceFolder();
  if( macx_plugins_path.endsWith( "Contents/Resources", Qt::CaseInsensitive ) )
  {
     QDir macx_plugins_dir( macx_plugins_path );
     if( macx_plugins_dir.cdUp() )
     {
       if( macx_plugins_dir.cd( "PlugIns" ) )
         macx_plugins_path = macx_plugins_dir.absolutePath();
     }
  }
  return use_resource_folder ? macx_plugins_path : dataFolder();
#else
  return use_resource_folder ? resourceFolder() : dataFolder();
#endif
}

bool Settings::addSubnetToBroadcastAddress( const QHostAddress& ext_subnet )
{
  if( ext_subnet.isNull() )
    return false;
  else
    return addBroadcastAddressInSettings( ext_subnet.toString() );
}

QString Settings::simpleEncrypt( const QString& text_to_encrypt )
{
  if( text_to_encrypt.size() <= 0 )
    return QLatin1String( "" );

  QByteArray byte_array_encrypted = text_to_encrypt.toUtf8().toBase64();

  return QString::fromLatin1( byte_array_encrypted );
}

QString Settings::simpleDecrypt( const QString& text_to_decrypt )
{
  if( text_to_decrypt.size() <= 0 )
    return QLatin1String( "" );

  QString text_decrypted = "";

  if( m_settingsVersion < 5 )
  {
    // Old Decryption
    char key = 'k';
    foreach( QChar c, text_to_decrypt )
      text_decrypted += c.toLatin1() ^ key;
  }
  else
  {
    QByteArray byte_array_to_decrypt = text_to_decrypt.toLatin1();
    text_decrypted = QString::fromUtf8( QByteArray::fromBase64( byte_array_to_decrypt ) );
  }

  return text_decrypted;
}
