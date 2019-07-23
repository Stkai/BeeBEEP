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
#include "Random.h"
#include "Settings.h"
#include "Version.h"


Settings* Settings::mp_instance = Q_NULLPTR;

Settings::Settings()
 : m_localUser( ID_LOCAL_USER )
{
  m_settingsVersion = BEEBEEP_SETTINGS_VERSION;

  /* Default RC start */
  m_enableSaveData = true;
  m_useChatWithAllUsers = true;
  m_useSettingsFileIni = true;
  m_broadcastOnlyToHostsIni = false;
  m_defaultBroadcastPort = DEFAULT_BROADCAST_PORT;
  m_defaultListenerPort = DEFAULT_LISTENER_PORT;
  m_defaultFileTransferPort = DEFAULT_FILE_TRANSFER_PORT;

  m_resetGeometryAtStartup = false;
  m_saveGeometryOnExit = false;
  m_saveDataInDocumentsFolder = false;
  m_disableMenuSettings = false;

  // In windows native dialogs are application modal and the connection goes in timeout...
  // In MacOSX instead it seems to work... I have changed the connection timeout...
  m_useNativeDialogs = true;
#ifdef Q_OS_MAC
  m_useNativeEmoticons = true;
#else
  m_useNativeEmoticons = false;
#endif

#ifdef MAKE_BEEBEEP_PORTABLE
  m_saveDataInUserApplicationFolder = false;
#else
  m_saveDataInUserApplicationFolder = true;
#endif

#ifdef BEEBEEP_USE_MULTICAST_DNS
  #ifdef Q_OS_MAC
    m_useMulticastDns = true;
  #else
    m_useMulticastDns = false;
  #endif
#else
  m_useMulticastDns = false;
#endif

  m_allowMultipleInstances = false;
  m_dataFolderInRC = "";
  m_addAccountNameToDataFolder = false;
  m_preferredSubnets = "";
  m_disableSystemProxyForConnections = true;
  m_useDefaultMulticastGroupAddress = true;
  m_ipMulticastTtl = DEFAULT_IPV4_MULTICAST_TTL_OPTION;
  m_useIPv6 = false;
  m_useHive = true;
  m_checkNewVersionAtStartup = true;
  m_postUsageStatistics = true;
  m_useHostnameForDefaultUsername = false;
  m_useEasyConnection = false;
#ifdef BEEBEEP_DISABLE_FILE_TRANSFER
  m_disableFileTransfer = true;
  m_disableFileSharing = true;
#else
  m_disableFileTransfer = false;
  m_disableFileSharing = false;
#endif
#ifdef BEEBEEP_DISABLE_SEND_MESSAGE
  m_disableSendMessage = true;
#else
  m_disableSendMessage = false;
#endif
  m_startMinimized = false;
  m_signature = "";
  m_useOnlyTextEmoticons = false;
  m_disablePrivateChats = false;
  m_userRecognitionMethod = RecognizeByDefaultMethod;
  m_canAddMembersToGroup = true;
  m_canRemoveMembersFromGroup = true;
  m_allowEditNickname = true;
  m_disableCreateMessage = false;
  m_clearCacheAfterDays = 96;
  /* Default RC end */

  m_emoticonSizeInEdit = 18;

  QFont f = QApplication::font();
  setChatFont( f );
  m_emoticonSizeInMenu = 24;
  m_emoticonInRecentMenu = 48;
  m_confirmOnDownloadFile = false;
  m_promptOnCloseEvent = true;
  m_saveUserList = true;
  m_saveGroupList = true;
  m_localUser.setStatus( User::Online );
  m_localUser.setVersion( version( false, false ) );
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
  m_cacheFolder = defaultCacheFolderPath();

  m_lastSave = QDateTime::currentDateTime();

  m_preventMultipleConnectionsFromSingleHostAddress = true;
  m_acceptConnectionsOnlyFromWorkgroups = false;
  m_maxUserStatusDescriptionInList = 10;

  m_tickIntervalConnectionTimeout = 16;
  m_useReturnToSendMessage = true;
  m_tickIntervalCheckIdle = 10;
  m_tickIntervalCheckNetwork = 5;
  m_tickIntervalBroadcasting = 0;
  m_broadcastToOfflineUsers = false;

  m_chatMessageFilter = QBitArray( static_cast<int>(ChatMessage::NumTypes) );
  for( int i = 0; i < ChatMessage::NumTypes; i++ )
    m_chatMessageFilter.setBit( i, false );

#ifdef Q_OS_WIN
  m_usePreviewFileDialog = true;
#else
  m_usePreviewFileDialog = false;
#endif
  m_previewFileDialogGeometry = "";
  m_previewFileDialogImageSize = 200;
  m_maxUsersToConnectInATick = 25;
  m_showTextInModeRTL = false;
  m_showChatsInOneWindow = false;
  m_maxLogLines = 5000;
  m_enableDefaultChatNotifications = true;

  m_enableFileTransfer = true;
  m_enableFileSharing = false;

  m_iconSourcePath = "";
  m_emoticonSourcePath = "";

  m_maxDaysOfUserInactivity = 31;

  m_shareDesktopCaptureDelay = 2100;

  m_delayConnectionAtStartup = 5000;

  m_maxChatsToOpenAfterSendingMessage = 6;
  m_showUsersOnConnection = false;
  m_showChatsOnConnection = false;
  m_hideEmptyChatsInList = true;

  m_chatActiveWindowOpacityLevel = 100;
  m_chatInactiveWindowOpacityLevel = chatInactiveWindowDefaultOpacityLevel();

  m_useMessageTimestampWithAP = false;
  m_useDarkStyle = false;
  m_chatDefaultUserNameColor = "#000000";

  m_saveMessagesTimestamp = QDateTime::currentDateTime();

#if QT_VERSION >= 0x050000
  m_chatMessagesToShow = 900;
#else
  m_chatMessagesToShow = 400;
#endif

  resetAllColors();
}

void Settings::initFolders( const QString& app_folder )
{
  m_resourceFolder = app_folder;
#if QT_VERSION >= 0x050400
  m_dataFolder = Bee::convertToNativeFolderSeparator( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) );
#elif QT_VERSION >= 0x050000
  m_dataFolder = Bee::convertToNativeFolderSeparator( QString( "%1/%2" )
                   .arg( QStandardPaths::writableLocation( QStandardPaths::DataLocation ) )
                   .arg( programName() ) );
#else
  m_dataFolder = Bee::convertToNativeFolderSeparator( QDesktopServices::storageLocation( QDesktopServices::DataLocation ) );
#endif
  m_cacheFolder = defaultCacheFolderPath();
}

void Settings::resetAllColors()
{
  m_homeBackgroundColor = "#f5f5f5";
  m_defaultChatBackgroundColor = "#f5f5f5";
  m_userListBackgroundColor = "#ffffff";
  m_chatListBackgroundColor = "#ffffff";
  m_groupListBackgroundColor = "#ffffff";
  m_savedChatListBackgroundColor = "#ffffff";
  m_chatBackgroundColor = "#ffffff";
  m_chatDefaultTextColor = "#555555";
  m_chatSystemTextColor = "#808080";
}

void Settings::createApplicationUuid()
{
  if( m_applicationUuid.isEmpty() )
  {
    m_applicationUuid = QUuid::createUuid().toString();
    m_applicationUuidCreationDate = QDate::currentDate();
    qDebug() << "Create new application uuid" << qPrintable( m_applicationUuid );
  }
  else
  {
    qint64 uuid_days_life = m_applicationUuidCreationDate.daysTo( QDate::currentDate() );
#ifdef BEEBEEP_DEBUG
    qDebug() << "Application uuid is created" << uuid_days_life << "days ago";
#endif
    if( uuid_days_life > 31 )
    {
      m_applicationUuid = QUuid::createUuid().toString();
      m_applicationUuidCreationDate = QDate::currentDate();
      qDebug() << "Renew application uuid" << qPrintable( m_applicationUuid ) << "after" << uuid_days_life << "days";
    }
    else
      qDebug() << "Continue to use application uuid" << qPrintable( m_applicationUuid ) << "created in date" << qPrintable( m_applicationUuidCreationDate.toString( "yyyy-MM-dd" ) );
  }
}

QNetworkProxy Settings::systemNetworkProxy( const QNetworkProxyQuery& npq ) const
{
  QList<QNetworkProxy> proxy_list = QNetworkProxyFactory::systemProxyForQuery( npq );
  if( proxy_list.isEmpty() )
    return QNetworkProxy( QNetworkProxy::NoProxy );
  else
    return proxy_list.first();
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
  QString host_name = QHostInfo::localHostName();
  QString account_name = pe.value( "USERNAME", "" );
  if( account_name.isEmpty() )
    account_name = pe.value( "USER", "" );
  return m_useHostnameForDefaultUsername ? host_name.simplified() : account_name.simplified();
}

void Settings::createLocalUser( const QString& user_name )
{
  m_localUser = User( ID_LOCAL_USER );

  QString account_name = accountNameFromSystemEnvinroment();
  if( account_name.isEmpty() )
  {
    qWarning() << "USERNAME and USER variabile not found in system environment";
    account_name = QString( "Bee %1" ).arg( QTime::currentTime().toString( "zzzsmh" ) );
  }
  m_localUser.setAccountName( account_name.toLower() );
  if( user_name.isEmpty() )
    m_localUser.setName( account_name );
  else
    m_localUser.setName( user_name );

  m_localUser.setQtVersion( qtMajorVersion() );
  m_localUser.setProtocolVersion( protoVersion() );
  m_localUser.setDomainName( QHostInfo::localDomainName() );
  m_localUser.setHash( Settings::instance().createLocalUserHash() );

  qDebug() << "User name:" << qPrintable( m_localUser.name() );
  qDebug() << "System account:" << qPrintable( m_localUser.accountName() );
  qDebug() << "Local domain name:" << qPrintable( m_localUser.domainName() );
  qDebug() << "Local host name:" << qPrintable( QHostInfo::localHostName() );
#ifdef BEEBEEP_DEBUG
  qDebug() << "Local user hash:" << qPrintable( m_localUser.hash() );
#endif
}

QString Settings::createLocalUserHash()
{
  QString hash_parameters = QString( "%1%2%3%4%5%6%7" )
                              .arg( QString::number( Random::number( 6475, 36475 ) ) )
                              .arg( m_localUser.accountName() ).arg( m_localUser.name() )
                              .arg( QString::number( Random::number( 6475, 36475 ) ) )
                              .arg( m_localUser.domainName() ).arg( version( true, true ) )
                              .arg( QDateTime::currentDateTime().toString( "dd.MM.yyyy-hh:mm:ss.zzz" ) );
  QString local_user_hash = simpleHash( hash_parameters );
#ifdef BEEBEEP_DEBUG
  qDebug() << "Local user HASH created:" << qPrintable( local_user_hash );
#endif
  return local_user_hash;
}

bool Settings::createDefaultRcFile()
{
  QFileInfo rc_file_info = defaultRcFilePath( false );
  if( rc_file_info.exists() )
  {
    qDebug() << "RC default configuration file exists in" << qPrintable( rc_file_info.absoluteFilePath() );
    return false;
  }

  QSettings* sets = new QSettings( rc_file_info.absoluteFilePath(), QSettings::IniFormat );
  bool rc_file_created = false;
  if( sets->isWritable() )
  {
    sets->beginGroup( "BeeBEEP" );
    sets->setValue( "EnableSaveData", m_enableSaveData );
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
    sets->setValue( "UseHive", m_useHive );
    sets->setValue( "UseHostnameForDefaultUsername", m_useHostnameForDefaultUsername );
    sets->setValue( "DisableFileTransfer", m_disableFileTransfer );
    sets->setValue( "DisableFileSharing", m_disableFileSharing );
    sets->setValue( "DisableSendMessage", m_disableSendMessage );
    sets->setValue( "UseEasyConnection", m_useEasyConnection );
    sets->setValue( "StartMinimized", m_startMinimized );
    sets->setValue( "Signature", m_signature );
    sets->setValue( "UseOnlyTextEmoticons", m_useOnlyTextEmoticons );
    sets->setValue( "DisablePrivateChats", m_disablePrivateChats );
    sets->setValue( "UserRecognitionMethod", m_userRecognitionMethod );
    sets->setValue( "AllowAddMembersToGroup", m_canAddMembersToGroup );
    sets->setValue( "AllowRemoveMembersFromGroup", m_canRemoveMembersFromGroup );
    sets->setValue( "AllowEditNickname", m_allowEditNickname );
    sets->setValue( "DisableCreateMessage", m_disableCreateMessage );
    sets->setValue( "DisableMenuSettings", m_disableMenuSettings );
    sets->endGroup();
    sets->sync();
    qDebug() << "RC default configuration file created in" << qPrintable( Bee::convertToNativeFolderSeparator( sets->fileName() ) );
    rc_file_created = true;
  }
  else
    qWarning() << "Unable to create RC default configuration file in" << qPrintable( Bee::convertToNativeFolderSeparator( sets->fileName() ) );

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
#ifdef Q_OS_UNIX
    rc_file_path = QLatin1String( "/etc/beebeep/beebeep.rc" );
    rc_file_info = QFileInfo( rc_file_path );
    qDebug() << "Check for RC file in system path:" << qPrintable( rc_file_path );
    if( !rc_file_info.exists() || !rc_file_info.isReadable() )
    {
#endif
    rc_file_info = QFileInfo( defaultRcFilePath( false ) );
    rc_file_path = Bee::convertToNativeFolderSeparator( rc_file_info.absoluteFilePath() );
    qDebug() << "Check for RC file in custom path:" << qPrintable( rc_file_path );
    if( !rc_file_info.exists() || !rc_file_info.isReadable() )
    {
      qDebug() << "RC file not found:" << qPrintable( programName() ) << "uses default RC configuration";
      return;
    }
#ifdef Q_OS_UNIX
    }
#endif
  }

  QSettings* sets = new QSettings( rc_file_path, QSettings::IniFormat );
  sets->setFallbacksEnabled( false );

  sets->beginGroup( "Groups" );
  bool trust_system_account = sets->value( "TrustSystemAccount", false ).toBool(); // for compatibility
  sets->endGroup();

  sets->beginGroup( "BeeBEEP" );
  m_enableSaveData = sets->value( "EnableSaveData", m_enableSaveData ).toBool();
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
  m_useHive = sets->value( "UseHive", m_useHive ).toBool();
  m_useHostnameForDefaultUsername = sets->value( "UseHostnameForDefaultUsername", m_useHostnameForDefaultUsername ).toBool();
#ifdef BEEBEEP_DISABLE_FILE_TRANSFER
  m_disableFileTransfer = true;
  m_disableFileSharing = true;
#else
  m_disableFileTransfer = sets->value( "DisableFileTransfer", m_disableFileTransfer ).toBool();
  if( m_disableFileTransfer )
    m_disableFileSharing = true;
  else
    m_disableFileSharing = sets->value( "DisableFileSharing", m_disableFileSharing ).toBool();
#endif
  m_disableSendMessage = sets->value( "DisableSendMessage", m_disableSendMessage ).toBool();
  m_useEasyConnection = sets->value( "UseEasyConnection", m_useEasyConnection ).toBool();
  m_startMinimized = sets->value( "StartMinimized", m_startMinimized ).toBool();
  m_signature = sets->value( "Signature", m_signature ).toString();
  m_useOnlyTextEmoticons = sets->value( "UseOnlyTextEmoticons", m_useOnlyTextEmoticons ).toBool();
  m_disablePrivateChats = sets->value( "DisablePrivateChats", m_disablePrivateChats ).toBool();
  int user_recognition_method = sets->value( "UserRecognitionMethod", -1 ).toInt();
  setUserRecognitionMethod( (user_recognition_method < 0 ? (trust_system_account ? RecognizeByAccount : m_userRecognitionMethod) : user_recognition_method) );

  m_canAddMembersToGroup = sets->value( "AllowAddMembersToGroup", m_canAddMembersToGroup ).toBool();
  m_canRemoveMembersFromGroup = sets->value( "AllowRemoveMembersFromGroup", m_canRemoveMembersFromGroup ).toBool();
  m_allowEditNickname = sets->value( "AllowEditNickname", m_allowEditNickname ).toBool();
  m_disableCreateMessage = sets->value( "DisableCreateMessage", m_disableCreateMessage ).toBool();
  m_disableMenuSettings = sets->value( "DisableMenuSettings", m_disableMenuSettings ).toBool();
  m_clearCacheAfterDays = qMax( -1, sets->value( "ClearCacheAfterDays", m_clearCacheAfterDays ).toInt() );
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

void Settings::setUserRecognitionMethod( int new_value )
{
  if( new_value <= 0 || new_value >= NumUserRecognitionMethods )
    m_userRecognitionMethod = RecognizeByDefaultMethod;
  else
    m_userRecognitionMethod = new_value;
}

bool Settings::isDevelopmentVersion( const QString& v ) const
{
  QString s_version;
  if( v.isEmpty() )
    s_version = QString( BEEBEEP_VERSION );
  else
    s_version = v;

  QStringList sl = s_version.split( "." );
  if( sl.isEmpty() )
  {
    qWarning() << "INVALID BEEBEEP VERSION FOUND";
    return true;
  }
  int v_num = sl.last().toInt();
  return v_num % 2 == 1;
}

QString Settings::version( bool qt_version, bool debug_info ) const
{
  QString s_version = QString( BEEBEEP_VERSION );
  if( isDevelopmentVersion( s_version ) || debug_info )
    s_version += QString( "-%1" ).arg( BEEBEEP_BUILD );

  if( debug_info )
  {
    s_version += QString( "-p%2" ).arg( BEEBEEP_PROTO_VERSION );
#ifdef BEEBEEP_DISABLE_FILE_TRANSFER
    s_version += QLatin1String( "-noft" );
#endif
#ifdef BEEBEEP_DISABLE_SEND_MESSAGE
    s_version += QLatin1String( "-nosm" );
#endif
  }

  if( qt_version )
    s_version += QString( "-qt%1" ).arg( qtMajorVersion() );

  return s_version;
}

QString Settings::httpUserAgent() const
{
  return QString( "%1 %2" ).arg( programName() ).arg( version( false, false ) );
}

int Settings::protoVersion() const
{
  return BEEBEEP_PROTO_VERSION;
}

QHostAddress Settings::defaultMulticastGroupAddress() const
{
  return m_useIPv6 ? QHostAddress( QString( DEFAULT_IPV6_MULTICAST_ADDRESS ) ) : QHostAddress( QString( DEFAULT_IPV4_MULTICAST_ADDRESS ) );
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
  return officialWebSite() + QString( BEEBEEP_DOWNLOAD_WEBSITE ) + "#plugin";
}

QString Settings::downloadWebSite() const
{
  return officialWebSite() + QString( BEEBEEP_DOWNLOAD_WEBSITE );
}

QString Settings::donationWebSite() const
{
  return officialWebSite() + QString( BEEBEEP_DONATE_WEBSITE );
}

QString Settings::helpWebSite() const
{
  QString web_url = officialWebSite() + QString( BEEBEEP_HELP_WEBSITE );
  if( m_language == "it" )
    web_url += QString( "?lang=it_IT" );
  else
    web_url += QString( "?lang=en_US" );
  return web_url;
}

QString Settings::languageWebSite() const
{
  return officialWebSite() + QString( BEEBEEP_LANGUAGE_WEBSITE );
}

QString Settings::lastVersionUrl() const
{
  return officialWebSite() + QString( BEEBEEP_LAST_VERSION_FILE );
}

QString Settings::newsWebSite() const
{
  return officialWebSite() + QString( BEEBEEP_NEWS_WEBSITE );
}

QString Settings::developerWebSite() const
{
  QString web_url = QString::fromLatin1( QByteArray::fromBase64( "aHR0cHM6Ly93d3cubWFyY29tYXN0cm9kZGkuY29tL2Fib3V0X21lLnBocD9sYW5nPQ==" ) );
  if( m_language == "it" )
    web_url += QString( "it_IT" );
  else
    web_url += QString( "en_US" );
  return web_url;
}

QString Settings::tipsWebSite() const
{
  QString web_url = officialWebSite() + QString( BEEBEEP_TIPS_WEBSITE );
  if( m_language == "it" )
    web_url += QString( "?lang=it_IT" );
  else
    web_url += QString( "?lang=en_US" );
  return web_url;
}

QString Settings::factWebSite() const
{
  QString web_url = officialWebSite() + QString( BEEBEEP_FACT_WEBSITE );
  if( m_language == "it" )
    web_url += QString( "?lang=it_IT" );
  else
    web_url += QString( "?lang=en_US" );
  return web_url;
}

QString Settings::faqWebSite() const
{
  QString web_url = officialWebSite() + QString( BEEBEEP_FAQ_WEBSITE );
  if( m_language == "it" )
    web_url += QString( "?lang=it_IT" );
  else
    web_url += QString( "?lang=en_US" );
  return web_url;
}

QString Settings::hunspellVersion() const
{
  return QString( HUNSPELL_VERSION );
}

QString Settings::operatingSystem( bool use_long_name ) const
{
  QString os_name_long = "Unknown OS";
  QString os_name_short = "unknown";
#ifdef Q_OS_WIN
  os_name_long = "Microsoft Windows";
  os_name_short = "Windows";
#endif
#ifdef Q_OS_LINUX
  os_name_long = "Linux";
  os_name_short = "Linux";
#endif
#ifdef Q_OS_MAC
  os_name_long = "Apple MacOSX";
  os_name_short = "MacOSX";
#endif
#ifdef Q_OS_OS2
  os_name_long = "eComStation";
  os_name_short = "eCS";
#endif
#ifdef BEEBEEP_FOR_RASPBERRY_PI
  os_name_long = "Raspberry PI";
  os_name_short = "PI";
#endif

  return use_long_name ? os_name_long : os_name_short;
}

QString Settings::operatingSystemIconPath() const
{
  QString os_icon_path = "beebeep.png";

#ifdef Q_OS_WIN
  os_icon_path = "windows.png";
#endif
#ifdef Q_OS_LINUX
  os_icon_path = "linux.png";
#endif
#ifdef Q_OS_MAC
  os_icon_path = "macosx.png";
#endif
#ifdef Q_OS_OS2
  os_icon_path = "os2.png";
#endif
#ifdef BEEBEEP_FOR_RASPBERRY_PI
  os_icon_path = "raspberry-pi.png";
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
  return Bee::convertToNativeFolderSeparator( QString( "%1/%2_%3.qm" ).arg( language_folder, Settings::instance().programName().toLower(), language_selected ) );
}

QString Settings::dnsRecord() const
{
  return QString( BEEBEEP_DNS_RECORD );
}

QString Settings::gaTrackingId() const
{
  return QString( BEEBEEP_GA_TRACKING_ID );
}

QString Settings::gaUrl() const
{
  return QString( BEEBEEP_GA_URL );
}

QString Settings::gaEventVersion() const
{
  return QString( BEEBEEP_GA_EVENT_VERSION );
}

QByteArray Settings::hash( const QString& string_to_hash ) const
{
  QByteArray hash_pre = string_to_hash.toUtf8() + m_password;
  if( !m_signature.isEmpty() )
    hash_pre += m_signature.toUtf8();
  QByteArray hash_generated = QCryptographicHash::hash( hash_pre, QCryptographicHash::Sha1 );
  return hash_generated.toHex();
}

QString Settings::simpleHash( const QString& string_to_hash ) const
{
#if QT_VERSION >= 0x050000
  QByteArray hash_generated = QCryptographicHash::hash( string_to_hash.toUtf8(), QCryptographicHash::Sha256 );
#else
  QByteArray hash_generated = QCryptographicHash::hash( string_to_hash.toUtf8(), QCryptographicHash::Sha1 );
#endif
  return QString::fromLatin1( hash_generated.toHex() );
}

void Settings::setPassword( const QString& new_value )
{
  m_passwordBeforeHash = new_value;
  QString pwd_tmp = new_value.isEmpty() || new_value == defaultPassword() ? QString( "*6475*" ) : new_value;
  if( !m_signature.isEmpty() )
    pwd_tmp.prepend( m_signature );
  m_password = QCryptographicHash::hash( pwd_tmp.toUtf8(), QCryptographicHash::Sha1 ).toHex();
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

void Settings::setLocalUserHost( const QHostAddress& host_address, quint16 host_port )
{
  if( host_address.isNull() || host_address.toString() == QString( "0.0.0.0" ) )
    m_localUser.setNetworkAddress( NetworkAddress( QHostAddress( "127.0.0.1" ), host_port ) );
  else
    m_localUser.setNetworkAddress( NetworkAddress( host_address, host_port ) );
}

void Settings::loadBroadcastAddressesFromFileHosts()
{
  if( !m_broadcastAddressesInFileHosts.isEmpty() )
    m_broadcastAddressesInFileHosts.clear();

  QFileInfo hosts_file_info( defaultHostsFilePath( true ) );
  QString hosts_file_path = Bee::convertToNativeFolderSeparator( hosts_file_info.absoluteFilePath() );
  qDebug() << "Check for HOSTS file in current path:" << qPrintable( hosts_file_path );
  if( !hosts_file_info.exists() || !hosts_file_info.isReadable() )
  {
#ifdef Q_OS_UNIX
    hosts_file_path = QLatin1String( "/etc/beebeep/beehosts.ini" );
    hosts_file_info = QFileInfo( hosts_file_path );
    qDebug() << "Check for HOSTS file in system path:" << qPrintable( hosts_file_path );
    if( !hosts_file_info.exists() || !hosts_file_info.isReadable() )
    {
#endif
      hosts_file_info = QFileInfo( defaultHostsFilePath( false ) );
      hosts_file_path = Bee::convertToNativeFolderSeparator( hosts_file_info.absoluteFilePath() );
      qDebug() << "Check for HOSTS file in custom path:" << qPrintable( hosts_file_path );
      if( !hosts_file_info.exists() || !hosts_file_info.isReadable() )
      {
        qDebug() << "HOSTS file not found";
        return;
      }
#ifdef Q_OS_UNIX
    }
#endif
  }

  QFile file( hosts_file_path );
  if( !file.open( QIODevice::ReadOnly ) )
  {
    qWarning() << "Cannot open file HOSTS in path" << qPrintable( hosts_file_path );
    return;
  }

  qDebug() << "Reading HOSTS from file" << qPrintable( hosts_file_path );
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
        if( !na.isHostAddressValid() )
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

QString Settings::checkFilePath( const QString& file_path, const QString& default_value )
{
  QFile file( file_path );
  return file.exists() ? file_path : default_value;
}

QString Settings::checkFolderPath( const QString& folder_path, const QString& default_value )
{
  QDir folder( folder_path );
  return folder.exists() ? folder_path : default_value;
}

QSettings* Settings::objectSettings() const
{
  QSettings *sets;

  if( m_useSettingsFileIni )
  {
    sets = new QSettings( defaultSettingsFilePath(), QSettings::IniFormat );
    sets->setFallbacksEnabled( false );
  }
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
  m_dataStreamVersion = sets->value( "DataStream", static_cast<int>(DATASTREAM_VERSION_1) ).toInt();
  m_settingsCreationDate = sets->value( "BeeBang", QDate() ).toDate();
  if( m_settingsCreationDate.isNull() )
    m_settingsCreationDate = QDate::currentDate();
  QString qt_version_in_settings = sets->value( "Qt", qtMajorMinorVersion() ).toString();
  sets->endGroup();

  bool qt_is_compatible = qt_version_in_settings == qtMajorMinorVersion();

  sets->beginGroup( "Chat" );
  m_chatFont.fromString( sets->value( "Font", QApplication::font().toString() ).toString() );
  setChatFont( m_chatFont );
  m_chatFontColor = sets->value( "FontColor", QColor( Qt::black ).name() ).toString();
  m_defaultChatBackgroundColor = sets->value( "DefaultChatBackgroundColor", m_defaultChatBackgroundColor ).toString();
  m_chatCompact = sets->value( "CompactMessage", true ).toBool();
  m_chatShowMessageTimestamp = sets->value( "ShowMessageTimestamp", false ).toBool();
  m_beepOnNewMessageArrived = sets->value( "BeepOnNewMessageArrived", true ).toBool();
  m_chatUseHtmlTags = sets->value( "UseHtmlTags", false ).toBool();
  m_chatUseClickableLinks = sets->value( "UseClickableLinks", true ).toBool();
  m_chatMessageHistorySize = sets->value( "MessageHistorySize", 10 ).toInt();
  m_showEmoticons = sets->value( "ShowEmoticons", true ).toBool();
  m_showMessagesGroupByUser = sets->value( "ShowMessagesGroupByUsers", true ).toBool();
  m_chatMessageFilter = sets->value( "MessageFilter", m_chatMessageFilter ).toBitArray();
  if( m_chatMessageFilter.size() < static_cast<int>(ChatMessage::NumTypes) )
    m_chatMessageFilter.resize( static_cast<int>(ChatMessage::NumTypes) );
  m_showOnlyMessagesInDefaultChat = sets->value( "ShowOnlyMessagesInDefaultChat", true ).toBool();
  m_chatMessagesToShow = sets->value( "MaxMessagesToShow", m_chatMessagesToShow ).toInt();
  m_imagePreviewHeight = qMax( 48, sets->value( "ImagePreviewHeight", 160 ).toInt() );
  m_useReturnToSendMessage = sets->value( "UseKeyReturnToSendMessage", m_useReturnToSendMessage ).toBool();
  m_chatUseYourNameInsteadOfYou = sets->value( "UseYourNameInsteadOfYou", false ).toBool();
  m_chatClearAllReadMessages = sets->value( "ClearAllReadMessages", false ).toBool();
  m_chatUseColoredUserNames = sets->value( "UseColoredUserNames", true ).toBool();
  m_chatDefaultUserNameColor = sets->value( "DefaultUserNameColor", "#000" ).toString();
  m_chatActiveWindowOpacityLevel = qMax( 10, qMin( 100, sets->value( "ActiveWindowOpacityLevel", m_chatActiveWindowOpacityLevel ).toInt() ) );
  m_chatInactiveWindowOpacityLevel = qMax( 10, qMin( 100, sets->value( "InactiveWindowOpacityLevel", m_chatInactiveWindowOpacityLevel ).toInt() ) );
  m_chatBackgroundColor = sets->value( "BackgroundColor", m_chatBackgroundColor ).toString();
  m_chatDefaultTextColor = sets->value( "DefaultTextColor", m_chatDefaultTextColor ).toString();
  m_chatSystemTextColor = sets->value( "SystemTextColor", m_chatSystemTextColor ).toString();
  m_enableDefaultChatNotifications = sets->value( "EnableDefaultChatNotifications", m_enableDefaultChatNotifications ).toBool();
  m_useMessageTimestampWithAP = sets->value( "UseMessageTimestampWithAP", m_useMessageTimestampWithAP ).toBool();
  sets->endGroup();

  sets->beginGroup( "User" );
  if( m_userRecognitionMethod == RecognizeByDefaultMethod )
  {
    int user_recognition_method = sets->value( "RecognitionMethod", m_userRecognitionMethod ).toInt();
    setUserRecognitionMethod( user_recognition_method );
  }
  m_localUser.setHash( sets->value( "LocalHash", m_localUser.hash() ).toString() );
  m_localUser.setName( sets->value( "LocalName", m_localUser.name() ).toString() );
  m_localUser.setColor( sets->value( "LocalColor", m_localUser.color() ).toString() );
  m_localUser.setStatusDescription( sets->value( "LocalLastStatusDescription", m_localUser.statusDescription() ).toString() );
  m_autoUserAway = sets->value( "AutoAwayStatus", false ).toBool();
  m_userAwayTimeout = qMax( sets->value( "UserAwayTimeout", 10 ).toInt(), 1 ); // minutes
  if( m_useEasyConnection )
  {
    m_useDefaultPassword = true;
    m_askChangeUserAtStartup = false;
    m_askPasswordAtStartup = false;
  }
  else
  {
    m_useDefaultPassword = sets->value( "UseDefaultPassword", true ).toBool();
    m_askChangeUserAtStartup = sets->value( "AskChangeUserAtStartup", m_firstTime ).toBool();
    m_askPasswordAtStartup = sets->value( "AskPasswordAtStartup", false ).toBool();
  }

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
  m_maxUserStatusDescriptionInList = sets->value( "MaxStatusDescriptionInList", m_maxUserStatusDescriptionInList ).toInt();
  m_presetMessages = sets->value( "PresetMessages", QMap<QString,QVariant>() ).toMap();
  m_refusedChats = sets->value( "RefusedChats", QStringList() ).toStringList();
  m_maxDaysOfUserInactivity = sets->value( "MaxDaysOfUserInactivity", m_maxDaysOfUserInactivity ).toInt();
  m_removeInactiveUsers = sets->value( "RemoveInactiveUsers", true ).toBool();
  sets->endGroup();

  sets->beginGroup( "VCard" );
  VCard vc;
  vc.setNickName( sets->value( "NickName",  m_localUser.vCard().nickName() ).toString() );
  vc.setFirstName( sets->value( "FirstName", m_localUser.vCard().firstName() ).toString() );
  vc.setLastName( sets->value( "LastName", m_localUser.vCard().lastName() ).toString() );
  QDate dt = sets->value( "Birthday", m_localUser.vCard().birthday() ).toDate();
  if( dt.isValid() )
    vc.setBirthday( dt );
  vc.setEmail( sets->value( "Email", m_localUser.vCard().email() ).toString() );
  QPixmap pix = sets->value( "Photo", m_localUser.vCard().photo() ).value<QPixmap>();
  if( !pix.isNull() )
    vc.setPhoto( pix );
  vc.setPhoneNumber( sets->value( "Phone", m_localUser.vCard().phoneNumber() ).toString() );
  vc.setInfo( sets->value( "Info", m_localUser.vCard().info() ).toString() );
  m_localUser.setVCard( vc );
  sets->endGroup();

  sets->beginGroup( "Gui" );
  m_resetGeometryAtStartup = sets->value( "ResetWindowGeometryAtStartup", m_resetGeometryAtStartup ).toBool();

  if( m_resetGeometryAtStartup || m_settingsVersion < 9 || !qt_is_compatible )
  {
    m_guiGeometry = "";
    m_guiState = "";
    m_floatingChatGeometry = "";
    m_floatingChatState = "";
    m_floatingChatSplitterState = "";
    m_mainBarIconSize = QSize( 24, 24 );
    m_avatarIconSize = QSize( 28, 28 );
    m_previewFileDialogGeometry = "";
    m_createMessageGeometry = "";
    m_fileSharingGeometry = "";
    qDebug() << "The geometry has been reset at startup";
  }
  else
  {
    m_guiGeometry = sets->value( "MainWindowGeometry", "" ).toByteArray();
    m_guiState = sets->value( "MainWindowState", "" ).toByteArray();
    m_floatingChatGeometry = sets->value( "FloatingChatGeometry", "" ).toByteArray();
    m_floatingChatState = sets->value( "FloatingChatState", "" ).toByteArray();
    m_floatingChatSplitterState = sets->value( "FloatingChatSplitterState", "" ).toByteArray();
    m_mainBarIconSize = sets->value( "MainBarIconSize", QSize( 24, 24 ) ).toSize();
    m_avatarIconSize = sets->value( "AvatarIconSize", QSize( 28, 28 ) ).toSize();
    m_previewFileDialogGeometry = sets->value( "PreviewFileDialogGeometry", "" ).toByteArray();
    m_createMessageGeometry = sets->value( "CreateMessageGeometry", "" ).toByteArray();
    m_fileSharingGeometry = sets->value( "FileSharingGeometry", "" ).toByteArray();
  }

#if QT_VERSION == 0x050906
  if( m_settingsVersion < 11 )
  {
    // Bug in restore state for QDockWidgets
    // https://bugreports.qt.io/browse/QTBUG-68939
    m_guiState = "";
    m_floatingChatState = "";
  }
#endif

  m_saveGeometryOnExit = sets->value( "SaveGeometryOnExit", m_saveGeometryOnExit ).toBool();

  m_language = sets->value( "Language", QLocale::system().name() ).toString();
  if( m_language.size() > 2 )
    m_language.resize( 2 );
#if QT_VERSION >= 0x050000
  m_lastDirectorySelected = Bee::convertToNativeFolderSeparator( sets->value( "LastDirectorySelected", QStandardPaths::writableLocation( QStandardPaths::DocumentsLocation ) ).toString() );
  m_lastDirectorySelected = checkFolderPath( m_lastDirectorySelected, Bee::convertToNativeFolderSeparator( QStandardPaths::writableLocation( QStandardPaths::DocumentsLocation ) ) );
  m_downloadDirectory = Bee::convertToNativeFolderSeparator( sets->value( "DownloadDirectory", QStandardPaths::writableLocation( QStandardPaths::DownloadLocation ) ).toString() );
  m_downloadDirectory = checkFolderPath( m_downloadDirectory, Bee::convertToNativeFolderSeparator( QStandardPaths::writableLocation( QStandardPaths::DownloadLocation ) ) );
#else
  m_lastDirectorySelected = Bee::convertToNativeFolderSeparator( sets->value( "LastDirectorySelected", QDesktopServices::storageLocation( QDesktopServices::DocumentsLocation ) ).toString() );
  m_lastDirectorySelected = checkFolderPath( m_lastDirectorySelected, Bee::convertToNativeFolderSeparator( QDesktopServices::storageLocation( QDesktopServices::DocumentsLocation ) ) );
  m_downloadDirectory = Bee::convertToNativeFolderSeparator( sets->value( "DownloadDirectory", QDesktopServices::storageLocation( QDesktopServices::DocumentsLocation ) ).toString() );
  m_downloadDirectory = checkFolderPath( m_downloadDirectory, Bee::convertToNativeFolderSeparator( QDesktopServices::storageLocation( QDesktopServices::DocumentsLocation ) ) );
#endif

  m_logPath = checkFolderPath( Bee::convertToNativeFolderSeparator( sets->value( "LogFolderPath", dataFolder() ).toString() ), dataFolder() );
  m_pluginPath = checkFolderPath( Bee::convertToNativeFolderSeparator( sets->value( "PluginPath", defaultPluginFolderPath( true ) ).toString() ), defaultPluginFolderPath( true ) );
  m_languagePath = checkFolderPath( Bee::convertToNativeFolderSeparator( sets->value( "LanguagePath", resourceFolder() ).toString() ), resourceFolder() );
  m_keyEscapeMinimizeInTray = sets->value( "KeyEscapeMinimizeInTray", false ).toBool();
#ifdef Q_OS_MAC
  m_minimizeInTray = false;
#else
  m_minimizeInTray = sets->value( "MinimizeInTray", true ).toBool();
#endif
  m_stayOnTop = sets->value( "StayOnTop", false ).toBool();
  m_raiseOnNewMessageArrived = sets->value( "RaiseOnNewMessageArrived", false ).toBool();
  m_raiseMainWindowOnNewMessageArrived = sets->value( "RaiseMainWindowOnNewMessageArrived", false ).toBool();
  m_alwaysShowFileTransferProgress = sets->value( "AlwaysShowFileTransferProgress", false ).toBool();
  m_alwaysOpenChatOnNewMessageArrived = sets->value( "AlwaysOpenChatOnNewMessageArrived", true ).toBool();
  m_beepFilePath = checkFilePath( Bee::convertToNativeFolderSeparator( sets->value( "BeepFilePath", defaultBeepFilePath( true ) ).toString() ), defaultBeepFilePath( true ) );
  m_loadOnTrayAtStartup = sets->value( "LoadOnTrayAtStartup", false ).toBool();
  m_showNotificationOnTray = sets->value( "ShowNotificationOnTray", true ).toBool();
  m_showOnlyMessageNotificationOnTray = sets->value( "ShowOnlyMessageNotificationOnTray", true ).toBool();
  m_trayMessageTimeout = qMax( sets->value( "ShowNotificationOnTrayTimeout", 10000 ).toInt(), 1000 );
  m_showChatMessageOnTray = sets->value( "ShowChatMessageOnTray", false ).toBool();
  m_textSizeInChatMessagePreviewOnTray = sets->value( "TextSizeInChatMessagePreviewOnTray", 40 ).toInt();
  m_showFileTransferCompletedOnTray = sets->value( "ShowFileTransferCompletedOnTray", true ).toBool();
  m_chatAutoSave = sets->value( "ChatAutoSave", true ).toBool();
  m_chatSaveUnsentMessages = sets->value( "ChatSaveUnsentMessages", true ).toBool();
  m_chatMaxLineSaved = sets->value( "ChatMaxLineSaved", 9000 ).toInt();
  m_chatSaveFileTransfers = sets->value( "ChatSaveFileTransfers", m_chatAutoSave ).toBool();
  m_chatSaveSystemMessages = sets->value( "ChatSaveSystemMessages", false ).toBool();
  m_showChatToolbar = sets->value( "ShowChatToolbar", true ).toBool();
  m_showOnlyOnlineUsers = sets->value( "ShowOnlyOnlineUsers", false ).toBool();
  m_showUserPhoto = sets->value( "ShowUserPhoto", true ).toBool();
  m_showVCardOnRightClick = sets->value( "ShowVCardOnRightClick", true ).toBool();
  m_showEmoticonMenu = sets->value( "ShowEmoticonMenu", false ).toBool();
  m_showPresetMessages = sets->value( "ShowPresetMessages", false ).toBool();
  m_emoticonSizeInEdit = qMax( 12, sets->value( "EmoticonSizeInEdit", m_emoticonSizeInEdit ).toInt() );
  m_emoticonSizeInChat = qMax( 12, sets->value( "EmoticonSizeInChat", m_emoticonSizeInChat ).toInt() );
  m_emoticonSizeInMenu = sets->value( "EmoticonSizeInMenu", m_emoticonSizeInMenu ).toInt();
  m_emoticonInRecentMenu = sets->value( "EmoticonsInRecentMenu", m_emoticonInRecentMenu ).toInt();
  m_recentEmoticons = sets->value( "RecentEmoticons", QStringList() ).toStringList();
  m_useNativeEmoticons = sets->value( "UseNativeEmoticons", m_useNativeEmoticons ).toBool();
  m_showMinimizedAtStartup = sets->value( "ShowMinimizedAtStartup", m_startMinimized ).toBool();
  m_promptOnCloseEvent = sets->value( "PromptOnCloseEvent", m_promptOnCloseEvent ).toBool();
  m_showUserStatusBackgroundColor = sets->value( "ShowUserStatusBackgroundColor", false ).toBool();
  m_showUserStatusDescription = sets->value( "ShowUserStatusDescription", true ).toBool();
  m_shortcuts = sets->value( "Shortcuts", QStringList() ).toStringList();
  m_useShortcuts = sets->value( "UseShortcuts", false ).toBool();
  m_useNativeDialogs = sets->value( "UseNativeFileDialogs", m_useNativeDialogs ).toBool();
  m_homeShowMessageTimestamp = sets->value( "ShowActivitiesTimestamp", false ).toBool();
  m_homeBackgroundColor = sets->value( "HomeBackgroundColor", m_homeBackgroundColor ).toString();
  m_userListBackgroundColor = sets->value( "UserListBackgroundColor", m_userListBackgroundColor ).toString();
  m_chatListBackgroundColor = sets->value( "ChatListBackgroundColor", m_chatListBackgroundColor ).toString();
  m_groupListBackgroundColor = sets->value( "GroupListBackgroundColor", m_groupListBackgroundColor ).toString();
  m_savedChatListBackgroundColor = sets->value( "SavedChatListBackgroundColor", m_savedChatListBackgroundColor ).toString();
  m_usePreviewFileDialog = sets->value( "UsePreviewFileDialog", m_usePreviewFileDialog ).toBool();
  m_previewFileDialogImageSize = qMax( 100, sets->value( "PreviewFileDialogImageSize", m_previewFileDialogImageSize ).toInt() );
  m_userSortingMode = qMax( 0, sets->value( "UserSortingMode", 0 ).toInt() );
  m_sortUsersAscending = sets->value( "SortUsersAscending", true ).toBool();
  m_showTextInModeRTL = sets->value( "ShowChatTextInModeRTL", m_showTextInModeRTL ).toBool();
  m_playBuzzSound = sets->value( "PlayBuzzSound", true ).toBool();
  bool open_chat_in_new_window = sets->value( "AlwaysOpenNewFloatingChat", !m_showChatsInOneWindow ).toBool();
  m_showChatsInOneWindow = sets->value( "ShowChatsInOneWindow", !open_chat_in_new_window ).toBool();
  m_iconSourcePath = sets->value( "IconSourcePath", m_iconSourcePath ).toString();
  m_emoticonSourcePath = sets->value( "EmoticonSourcePath", m_emoticonSourcePath ).toString();
  m_maxChatsToOpenAfterSendingMessage = sets->value( "MaxChatsToOpenAfterSendingMessage", m_maxChatsToOpenAfterSendingMessage ).toInt();
  m_showUsersOnConnection = sets->value( "ShowUsersOnConnection", m_showUsersOnConnection ).toBool();
  m_showChatsOnConnection = sets->value( "ShowChatsOnConnection", m_showChatsOnConnection ).toBool();
  if( m_showChatsOnConnection && m_showUsersOnConnection )
    m_showUsersOnConnection = false;
  m_hideEmptyChatsInList = sets->value( "HideEmptyChatsInList", m_hideEmptyChatsInList ).toBool();
  m_enableMaximizeButton = sets->value( "EnableMaximizeButton", false ).toBool();
  m_useDarkStyle = sets->value( "UseDarkStyle", m_useDarkStyle ).toBool();
  sets->endGroup();

  sets->beginGroup( "Tools" );
  m_logToFile = sets->value( "LogToFile", false ).toBool();
  m_maxLogLines = sets->value( "MaxLogLines", m_maxLogLines ).toInt();
  m_useSpellChecker = sets->value( "UseSpellChecker", true ).toBool();
  m_useWordCompleter = sets->value( "UseWordCompleter", false ).toBool();
  m_dictionaryPath = checkFilePath( sets->value( "DictionaryPath", "" ).toString(), "" );
  m_checkNewVersionAtStartup = sets->value( "SearchForNewVersionAtStartup", m_checkNewVersionAtStartup ).toBool();
  m_postUsageStatistics = sets->value( "SendAnonymousUsageStatistics", m_postUsageStatistics ).toBool();
  m_applicationUuid = sets->value( "Uuid", "" ).toString();
  m_applicationUuidCreationDate = sets->value( "UuidCreationDate", QDate::currentDate() ).toDate();
  m_statsPostDate = sets->value( "StatsPostDate", QDate() ).toDate();
  sets->endGroup();

  sets->beginGroup( "Misc" );
  m_tickIntervalCheckIdle = qMax( sets->value( "TickIntervalCheckIdle", m_tickIntervalCheckIdle ).toInt(), 2 );
  m_tickIntervalCheckNetwork = qMax( sets->value( "TickIntervalCheckNetwork", m_tickIntervalCheckNetwork ).toInt(), 5 );
  m_tickIntervalBroadcasting = qMax( sets->value( "TickIntervalBroadcasting", m_tickIntervalBroadcasting ).toInt(), 0 );
  NetworkAddress local_user_network_address = m_localUser.networkAddress();
  local_user_network_address.setHostPort( static_cast<quint16>(sets->value( "ListenerPort", DEFAULT_LISTENER_PORT ).toUInt()) );
  m_localUser.setNetworkAddress( local_user_network_address );
  m_pongTimeout = qMax( sets->value( "ConnectionActivityTimeout(ms)", 30000 ).toInt(), 13000 );
  m_writingTimeout = qMax( sets->value( "WritingTimeout", 3000 ).toInt(), 3000 );
  int mod_buffer_size = m_fileTransferBufferSize % ENCRYPTED_DATA_BLOCK_SIZE; // For a corrected encryption
  if( mod_buffer_size > 0 )
    m_fileTransferBufferSize -= mod_buffer_size;
  m_tickIntervalConnectionTimeout = qMax( sets->value( "TickIntervalConnectionTimeout", m_tickIntervalConnectionTimeout ).toInt(), 3 );
  if( m_settingsVersion < 6 && m_tickIntervalConnectionTimeout < 16 )
    m_tickIntervalConnectionTimeout = 16;
  m_useLowDelayOptionOnSocket = sets->value( "UseLowDelayOptionOnSocket", false ).toBool();
  m_delayConnectionAtStartup = qMax( 3000, sets->value( "DelayConnectionAtStartup(ms)", m_delayConnectionAtStartup ).toInt() );
  m_sendOfflineMessagesToDefaultChat = sets->value( "SendOfflineMessagesToDefaultChat", false ).toBool();
  m_saveMessagesTimestamp = sets->value( "SaveMessagesTimestamp", QDateTime() ).toDateTime();
  m_clearCacheAfterDays = qMax( -1, sets->value( "ClearCacheAfterDays", m_clearCacheAfterDays ).toInt() );
  sets->endGroup();

  sets->beginGroup( "Network");
  QString local_host_address = sets->value( "LocalHostAddressForced", "" ).toString();
  if( !local_host_address.isEmpty() )
    m_localHostAddressForced = QHostAddress( local_host_address );
  m_localSubnetForced = sets->value( "LocalSubnetForced", "" ).toString();
  m_networkAddressList = sets->value( "UserPathList", QStringList() ).toStringList();
  m_acceptConnectionsOnlyFromWorkgroups = sets->value( "AcceptConnectionsOnlyFromWorkgroups", m_acceptConnectionsOnlyFromWorkgroups ).toBool();
  m_workgroups = sets->value( "Workgroups", QStringList() ).toStringList();
#ifdef BEEBEEP_USE_MULTICAST_DNS
  m_useMulticastDns = sets->value( "UseMulticastDns", m_useMulticastDns ).toBool();
#endif
  m_maxUsersToConnectInATick = sets->value( "MaxUsersToConnectInATick", m_maxUsersToConnectInATick ).toInt();
  m_preventMultipleConnectionsFromSingleHostAddress = sets->value( "PreventMultipleConnectionsFromSingleHostAddress", m_preventMultipleConnectionsFromSingleHostAddress ).toBool();
  m_useHive = sets->value( "UseHiveProtocol", m_useHive ).toBool();
  m_disableSystemProxyForConnections = sets->value( "DisableSystemProxyForConnections", m_disableSystemProxyForConnections ).toBool();
  m_useDefaultMulticastGroupAddress = sets->value( "UseDefaultMulticastGroupAddress", m_useDefaultMulticastGroupAddress ).toBool();
  m_broadcastToOfflineUsers = sets->value( "BroadcastToOfflineUsers", m_broadcastToOfflineUsers ).toBool();
  m_ipMulticastTtl = sets->value( "IpMulticastTtl", m_ipMulticastTtl ).toInt();
  sets->endGroup();
  loadBroadcastAddressesFromFileHosts();

  sets->beginGroup( "FileShare" );
  if( m_disableFileTransfer )
    m_enableFileTransfer = false;
  else
    m_enableFileTransfer = sets->value( "EnableFileTransfer", true ).toBool();

  if( m_enableFileTransfer )
  {
    if( m_disableFileSharing )
      m_enableFileSharing = false;
    else
      m_enableFileSharing = sets->value( "EnableFileSharing", false ).toBool();

    if( m_enableFileSharing )
      m_useShareBox = sets->value( "UseShareBox", false ).toBool();
    else
      m_useShareBox = false;
  }
  else
  {
    m_enableFileSharing = false;
    m_useShareBox = false;
  }

  m_maxFileShared = qMax( 1024, sets->value( "MaxSharedFiles", 8192 ).toInt() );
  m_shareBoxPath = checkFolderPath( sets->value( "ShareBoxPath", "" ).toString(), "" );
  m_maxSimultaneousDownloads = sets->value( "MaxSimultaneousDownloads", 3 ).toInt();
  m_maxQueuedDownloads = sets->value( "MaxQueuedDownloads", 400 ).toInt();
  m_fileTransferConfirmTimeout = qMax( sets->value( "FileTransferConfirmTimeout", 30000 ).toInt(), 1000 );
  m_fileTransferBufferSize = qMax( sets->value( "FileTransferBufferSize", 65456 ).toInt(), 2048 );

  m_automaticFileName = sets->value( "SetAutomaticFileNameOnSave", true ).toBool();
  m_overwriteExistingFiles = sets->value( "OverwriteExistingFiles", false ).toBool();
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
  m_saveGroupList = sets->value( "SaveGroups", m_saveGroupList ).toBool();
  m_groupSilenced = sets->value( "Silenced", QStringList() ).toStringList();
  m_groupList = sets->value( "List", QStringList() ).toStringList();
  sets->endGroup();

  sets->beginGroup( "ShareDesktop" );
  m_enableShareDesktop = sets->value( "Enable", true ).toBool();
  m_shareDesktopCaptureDelay = qMax( 1000, sets->value( "CaptureScreenInterval", m_shareDesktopCaptureDelay ).toInt() );
  m_shareDesktopFitToScreen = sets->value( "FitToScreen", false ).toBool();
  m_shareDesktopImageType = sets->value( "ImageType", "png" ).toString();
  m_shareDesktopImageQuality = sets->value( "ImageQuality", 20 ).toInt();
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

  if( !m_allowEditNickname && userRecognitionMethod() != Settings::RecognizeByNickname )
  {
    if( m_localUser.name() != m_localUser.accountName() )
    {
      qWarning() << "AllowEditNickname is disabled but nickname changed to" << qPrintable( m_localUser.name() );
      qDebug() << "Restoring system account name:" << qPrintable( m_localUser.accountName() );
      m_localUser.setName( m_localUser.accountName() );
    }
  }

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

QString Settings::qtMajorMinorVersion() const
{
  QString qt_version( qVersion() );
  QStringList sl_version = qt_version.split( "." );
  if( sl_version.isEmpty() )
    return QString( "0" );
  else if( sl_version.size() < 2 )
    return sl_version.at( 0 );
  else
    return QString( "%1.%2" ).arg( sl_version.at( 0 ) ).arg( sl_version.at( 1 ) );
}

void Settings::save()
{
  if( !m_enableSaveData )
  {
    qWarning() << "Skip savings settings because you have disabled it in RC file";
    return;
  }

  QSettings *sets = objectSettings();

  sets->clear();

  sets->beginGroup( "Version" );
  sets->setValue( "Program", version( false, true ) );
  sets->setValue( "Proto", protoVersion() );
  sets->setValue( "Settings", BEEBEEP_SETTINGS_VERSION );
  sets->setValue( "DataStream", dataStreamVersion( false ) );
  sets->setValue( "BeeBang", m_settingsCreationDate );
  sets->setValue( "Qt", qtMajorMinorVersion() );
  sets->endGroup();
  sets->beginGroup( "Chat" );
  sets->setValue( "Font", m_chatFont.toString() );
  sets->setValue( "FontColor", m_chatFontColor );
  sets->setValue( "DefaultChatBackgroundColor", m_defaultChatBackgroundColor );
  sets->setValue( "CompactMessage", m_chatCompact );
  sets->setValue( "ShowMessageTimestamp", m_chatShowMessageTimestamp );
  sets->setValue( "BeepOnNewMessageArrived", m_beepOnNewMessageArrived );
  sets->setValue( "UseHtmlTags", m_chatUseHtmlTags );
  sets->setValue( "UseClickableLinks", m_chatUseClickableLinks );
  sets->setValue( "MessageHistorySize", m_chatMessageHistorySize );
  sets->setValue( "ShowEmoticons", m_showEmoticons );
  sets->setValue( "ShowMessagesGroupByUsers", m_showMessagesGroupByUser );
  sets->setValue( "MessageFilter", m_chatMessageFilter );
  sets->setValue( "ShowOnlyMessagesInDefaultChat", m_showOnlyMessagesInDefaultChat );
  sets->setValue( "MaxMessagesToShow", m_chatMessagesToShow );
  sets->setValue( "ImagePreviewHeight", m_imagePreviewHeight );
  sets->setValue( "UseKeyReturnToSendMessage", m_useReturnToSendMessage );
  sets->setValue( "UseYourNameInsteadOfYou", m_chatUseYourNameInsteadOfYou );
  sets->setValue( "ClearAllReadMessages", m_chatClearAllReadMessages );
  sets->setValue( "UseColoredUserNames", m_chatUseColoredUserNames );
  sets->setValue( "DefaultUserNameColor", m_chatDefaultUserNameColor );
  sets->setValue( "ActiveWindowOpacityLevel", m_chatActiveWindowOpacityLevel  );
  sets->setValue( "InactiveWindowOpacityLevel", m_chatInactiveWindowOpacityLevel );
  sets->setValue( "BackgroundColor", m_chatBackgroundColor );
  sets->setValue( "DefaultTextColor", m_chatDefaultTextColor );
  sets->setValue( "SystemTextColor", m_chatSystemTextColor );
  sets->setValue( "EnableDefaultChatNotifications", m_enableDefaultChatNotifications );
  sets->setValue( "UseMessageTimestampWithAP", m_useMessageTimestampWithAP );
  sets->endGroup();
  sets->beginGroup( "User" );
  if( m_userRecognitionMethod != RecognizeByDefaultMethod )
    sets->setValue( "RecognitionMethod", m_userRecognitionMethod );
  sets->setValue( "LocalHash", m_localUser.hash() );
  sets->setValue( "LocalColor", m_localUser.color() );
  sets->setValue( "LocalLastStatusDescription", m_localUser.statusDescription() );
  sets->setValue( "AutoAwayStatus", m_autoUserAway );
  sets->setValue( "UserAwayTimeout", m_userAwayTimeout ); // minutes
  sets->setValue( "AskChangeUserAtStartup", m_askChangeUserAtStartup );
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

  sets->setValue( "MaxStatusDescriptionInList", m_maxUserStatusDescriptionInList );
  sets->setValue( "PresetMessages", m_presetMessages );
  sets->setValue( "RefusedChats", m_refusedChats );
  sets->setValue( "MaxDaysOfUserInactivity", m_maxDaysOfUserInactivity );
  sets->setValue( "RemoveInactiveUsers", m_removeInactiveUsers );
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
  sets->setValue( "FloatingChatGeometry", m_floatingChatGeometry );
  sets->setValue( "FloatingChatState", m_floatingChatState );
  sets->setValue( "FloatingChatSplitterState", m_floatingChatSplitterState );
  sets->setValue( "CreateMessageGeometry", m_createMessageGeometry );
  sets->setValue( "FileSharingGeometry", m_fileSharingGeometry );
  sets->setValue( "MainBarIconSize", m_mainBarIconSize );
  sets->setValue( "AvatarIconSize", m_avatarIconSize );
  sets->setValue( "Language", m_language );
  sets->setValue( "LastDirectorySelected", m_lastDirectorySelected );
  sets->setValue( "DownloadDirectory", m_downloadDirectory );
  sets->setValue( "LogFolderPath", m_logPath );
  sets->setValue( "PluginPath", m_pluginPath );
  sets->setValue( "LanguagePath", m_languagePath );
  sets->setValue( "KeyEscapeMinimizeInTray", m_keyEscapeMinimizeInTray );
  sets->setValue( "MinimizeInTray", m_minimizeInTray );
  sets->setValue( "StayOnTop", m_stayOnTop );
  sets->setValue( "BeepFilePath", m_beepFilePath );
  sets->setValue( "RaiseOnNewMessageArrived", m_raiseOnNewMessageArrived );
  sets->setValue( "RaiseMainWindowOnNewMessageArrived", m_raiseMainWindowOnNewMessageArrived );
  sets->setValue( "AlwaysShowFileTransferProgress", m_alwaysShowFileTransferProgress );
  sets->setValue( "AlwaysOpenChatOnNewMessageArrived", m_alwaysOpenChatOnNewMessageArrived );
  sets->setValue( "LoadOnTrayAtStartup", m_loadOnTrayAtStartup );
  sets->setValue( "ShowNotificationOnTray", m_showNotificationOnTray );
  sets->setValue( "ShowOnlyMessageNotificationOnTray", m_showOnlyMessageNotificationOnTray );
  sets->setValue( "ShowNotificationOnTrayTimeout", m_trayMessageTimeout );
  sets->setValue( "ShowChatMessageOnTray", m_showChatMessageOnTray );
  sets->setValue( "TextSizeInChatMessagePreviewOnTray", m_textSizeInChatMessagePreviewOnTray );
  sets->setValue( "ShowFileTransferCompletedOnTray", m_showFileTransferCompletedOnTray );
  sets->setValue( "ChatAutoSave", m_chatAutoSave );
  sets->setValue( "ChatMaxLineSaved", m_chatMaxLineSaved );
  sets->setValue( "ChatSaveUnsentMessages", m_chatSaveUnsentMessages );
  sets->setValue( "ChatSaveFileTransfers", m_chatSaveFileTransfers );
  sets->setValue( "ChatSaveSystemMessages", m_chatSaveSystemMessages );
  sets->setValue( "ShowChatToolbar", m_showChatToolbar );
  sets->setValue( "ShowOnlyOnlineUsers", m_showOnlyOnlineUsers );
  sets->setValue( "ShowUserPhoto", m_showUserPhoto );
  sets->setValue( "ShowVCardOnRightClick", m_showVCardOnRightClick );
  sets->setValue( "ResetWindowGeometryAtStartup", m_resetGeometryAtStartup );
  sets->setValue( "SaveGeometryOnExit", m_saveGeometryOnExit );
  sets->setValue( "ShowEmoticonMenu", m_showEmoticonMenu );
  sets->setValue( "ShowPresetMessages", m_showPresetMessages );
  sets->setValue( "EmoticonSizeInEdit", m_emoticonSizeInEdit );
  sets->setValue( "EmoticonSizeInChat", m_emoticonSizeInChat );
  sets->setValue( "EmoticonSizeInMenu", m_emoticonSizeInMenu );
  sets->setValue( "EmoticonsInRecentMenu", m_emoticonInRecentMenu );
  sets->setValue( "RecentEmoticons", m_recentEmoticons );
  sets->setValue( "UseNativeEmoticons", m_useNativeEmoticons );
  sets->setValue( "ShowMinimizedAtStartup", m_showMinimizedAtStartup );
  sets->setValue( "PromptOnCloseEvent", m_promptOnCloseEvent );
  sets->setValue( "ShowUserStatusBackgroundColor", m_showUserStatusBackgroundColor );
  sets->setValue( "ShowUserStatusDescription", m_showUserStatusDescription );
  sets->setValue( "Shortcuts", m_shortcuts );
  sets->setValue( "UseShortcuts", m_useShortcuts );
  sets->setValue( "UseNativeFileDialogs", m_useNativeDialogs );
  sets->setValue( "ShowActivitiesTimestamp", m_homeShowMessageTimestamp );
  sets->setValue( "HomeBackgroundColor", m_homeBackgroundColor );
  sets->setValue( "UserListBackgroundColor", m_userListBackgroundColor );
  sets->setValue( "ChatListBackgroundColor", m_chatListBackgroundColor );
  sets->setValue( "GroupListBackgroundColor", m_groupListBackgroundColor );
  sets->setValue( "SavedChatListBackgroundColor", m_savedChatListBackgroundColor );
  sets->setValue( "UsePreviewFileDialog", m_usePreviewFileDialog );
  sets->setValue( "PreviewFileDialogGeometry", m_previewFileDialogGeometry );
  sets->setValue( "PreviewFileDialogImageSize", m_previewFileDialogImageSize );
  sets->setValue( "UserSortingMode", m_userSortingMode );
  sets->setValue( "SortUsersAscending", m_sortUsersAscending );
  sets->setValue( "ShowChatTextInModeRTL", m_showTextInModeRTL );
  sets->setValue( "PlayBuzzSound", m_playBuzzSound );
  sets->setValue( "ShowChatsInOneWindow", m_showChatsInOneWindow );
  sets->setValue( "IconSourcePath", m_iconSourcePath );
  sets->setValue( "EmoticonSourcePath", m_emoticonSourcePath );
  sets->setValue( "MaxChatsToOpenAfterSendingMessage", m_maxChatsToOpenAfterSendingMessage );
  sets->setValue( "ShowUsersOnConnection", m_showUsersOnConnection );
  sets->setValue( "ShowChatsOnConnection", m_showChatsOnConnection );
  sets->setValue( "HideEmptyChatsInList", m_hideEmptyChatsInList );
  sets->setValue( "EnableMaximizeButton", m_enableMaximizeButton );
  sets->setValue( "UseDarkStyle", m_useDarkStyle );
  sets->endGroup();
  sets->beginGroup( "Tools" );
  sets->setValue( "LogToFile", m_logToFile );
  sets->setValue( "UseSpellChecker", m_useSpellChecker );
  sets->setValue( "UseWordCompleter", m_useWordCompleter );
  sets->setValue( "DictionaryPath", m_dictionaryPath );
  sets->setValue( "SearchForNewVersionAtStartup", m_checkNewVersionAtStartup );
  sets->setValue( "SendAnonymousUsageStatistics", m_postUsageStatistics );
  sets->setValue( "Uuid", m_applicationUuid );
  sets->setValue( "UuidCreationDate", m_applicationUuidCreationDate );
  sets->setValue( "StatsPostDate", m_statsPostDate );
  sets->endGroup();
  sets->beginGroup( "Misc" );
  sets->setValue( "TickIntervalCheckIdle", m_tickIntervalCheckIdle );
  sets->setValue( "TickIntervalCheckNetwork", m_tickIntervalCheckNetwork );
  sets->setValue( "ListenerPort", m_localUser.networkAddress().hostPort() );
  sets->setValue( "ConnectionActivityTimeout(ms)", m_pongTimeout );
  sets->setValue( "WritingTimeout", m_writingTimeout );
  sets->setValue( "TickIntervalConnectionTimeout", m_tickIntervalConnectionTimeout );
  sets->setValue( "UseLowDelayOptionOnSocket", m_useLowDelayOptionOnSocket );
  sets->setValue( "TickIntervalBroadcasting", m_tickIntervalBroadcasting );
  sets->setValue( "DelayConnectionAtStartup(ms)", m_delayConnectionAtStartup );
  sets->setValue( "SendOfflineMessagesToDefaultChat", m_sendOfflineMessagesToDefaultChat );
  sets->setValue( "SaveMessagesTimestamp", m_saveMessagesTimestamp );
  sets->setValue( "ClearCacheAfterDays", m_clearCacheAfterDays );
  sets->endGroup();
  sets->beginGroup( "Network");
#ifdef BEEBEEP_USE_MULTICAST_DNS
  sets->setValue( "UseMulticastDns", m_useMulticastDns );
#endif
  sets->setValue( "PreventMultipleConnectionsFromSingleHostAddress", m_preventMultipleConnectionsFromSingleHostAddress );
  if( !m_localHostAddressForced.isNull() )
    sets->setValue( "LocalHostAddressForced", m_localHostAddressForced.toString() );
  else
    sets->setValue( "LocalHostAddressForced", QString( "" ) );
  sets->setValue( "LocalSubnetForced", m_localSubnetForced );
  sets->setValue( "UserPathList", m_networkAddressList );
  sets->setValue( "AcceptConnectionsOnlyFromWorkgroups", m_acceptConnectionsOnlyFromWorkgroups );
  sets->setValue( "Workgroups", m_workgroups );
  sets->setValue( "MaxUsersToConnectInATick", m_maxUsersToConnectInATick );
  sets->setValue( "UseHiveProtocol", m_useHive );
  sets->setValue( "DisableSystemProxyForConnections", m_disableSystemProxyForConnections );
  sets->setValue( "UseDefaultMulticastGroupAddress", m_useDefaultMulticastGroupAddress );
  sets->setValue( "BroadcastToOfflineUsers", m_broadcastToOfflineUsers );
  sets->setValue( "IpMulticastTtl", m_ipMulticastTtl );
  sets->endGroup();
  sets->beginGroup( "FileShare" );
  sets->setValue( "EnableFileTransfer", m_enableFileTransfer );
  sets->setValue( "EnableFileSharing", m_enableFileSharing );
  sets->setValue( "UseShareBox", m_useShareBox );
  sets->setValue( "MaxSharedFiles", m_maxFileShared );
  sets->setValue( "ShareBoxPath", m_shareBoxPath );
  sets->setValue( "SetAutomaticFileNameOnSave", m_automaticFileName );
  sets->setValue( "OverwriteExistingFiles", m_overwriteExistingFiles );
  sets->setValue( "FileTransferConfirmTimeout", m_fileTransferConfirmTimeout );
  sets->setValue( "FileTransferBufferSize", m_fileTransferBufferSize );
  sets->setValue( "MaxSimultaneousDownloads", m_maxSimultaneousDownloads );
  sets->setValue( "MaxQueuedDownloads", m_maxQueuedDownloads );
  sets->setValue( "ConfirmOnDownloadFile", m_confirmOnDownloadFile );
  sets->setValue( "ShareList", m_localShare );
  sets->endGroup();

  sets->beginGroup( "Group" );
  sets->setValue( "SaveGroups", m_saveGroupList );
  if( m_saveGroupList && !m_groupList.isEmpty() )
  {
    sets->setValue( "Silenced", m_groupSilenced );
    sets->setValue( "List", m_groupList );
  }
  sets->endGroup();

  sets->beginGroup( "ShareDesktop" );
  sets->setValue( "Enable", m_enableShareDesktop );
  sets->setValue( "CaptureScreenInterval", m_shareDesktopCaptureDelay );
  sets->setValue( "FitToScreen", m_shareDesktopFitToScreen );
  sets->setValue( "ImageType", m_shareDesktopImageType );
  sets->setValue( "ImageQuality", m_shareDesktopImageQuality );
  sets->endGroup();

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
    qDebug() << "Settings saved in" << qPrintable( Bee::convertToNativeFolderSeparator( sets->fileName() ) );
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

bool Settings::isFileImageInCache( const QString& file_path ) const
{
  QFileInfo file_info( file_path );
  if( !Bee::isFileTypeImage( file_info.suffix() ) )
    return false;
  QString base_folder_path = Bee::convertToNativeFolderSeparator( file_info.absoluteDir().path() );
  if( base_folder_path == m_cacheFolder )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << qPrintable( file_path ) << "is in cache";
#endif
    return true;
  }
  else
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << qPrintable( file_path ) << "is NOT in cache but in:" << qPrintable( base_folder_path );
#endif
    return false;
  }
}

void Settings::clearTemporaryFiles()
{
  if( m_tempFilePathList.isEmpty() )
    return;

  foreach( QString file_path, m_tempFilePathList )
  {
    if( m_chatSaveFileTransfers && isFileImageInCache( file_path ) )
      continue;

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

  if( !m_saveDataInDocumentsFolder && !m_saveDataInUserApplicationFolder && m_dataFolderInRC.isEmpty() )
  {
    if( Bee::folderIsWriteable( m_dataFolder ) )
    {
      qDebug() << "Data folder:" << qPrintable( m_dataFolder );
      return true;
    }
    else
      qWarning() << "Default data folder" << qPrintable( m_dataFolder ) << "is not writeable";
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
    root_folder = QString( "%1/%2" ).arg( QStandardPaths::writableLocation( QStandardPaths::DataLocation ) ).arg( programName() );
  else if( m_saveDataInDocumentsFolder )
    root_folder = QStandardPaths::writableLocation( QStandardPaths::DocumentsLocation );
  else
    root_folder = QString( "%1/%2" ).arg( QStandardPaths::writableLocation( QStandardPaths::DataLocation ) ).arg( programName() );
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

  if( !Bee::folderIsWriteable( m_dataFolder ) )
  {
    qWarning() << "Data folder" << qPrintable( m_dataFolder ) << "is not writeable";
    return false;
  }
  qDebug() << "Data folder (smart):" << qPrintable( m_dataFolder );

  m_cacheFolder = defaultCacheFolderPath();
  QDir cache_folder( m_cacheFolder );
  if( !cache_folder.exists() )
  {
    if( !cache_folder.mkpath( m_cacheFolder ) )
    {
      qWarning() << "Unable to create cache folder" << qPrintable( cache_folder.absolutePath() ) ;
      m_cacheFolder = m_dataFolder;
    }
    else
      qDebug() << "Cache folder created in" << qPrintable( m_cacheFolder );
  }
  qDebug() << "Cache folder:" << qPrintable( m_cacheFolder );
  return true;
}

QString Settings::defaultCacheFolderPath() const
{
  return Bee::convertToNativeFolderSeparator( QString( "%1/%2" ).arg( dataFolder() ).arg( "cache" ) );
}

QString Settings::savedChatsFilePath() const
{
  return Bee::convertToNativeFolderSeparator( QString( "%1/%2" ).arg( dataFolder() ).arg( "beebeep.dat" ) );
}

QString Settings::unsentMessagesFilePath() const
{
  return Bee::convertToNativeFolderSeparator( QString( "%1/%2" ).arg( dataFolder() ).arg( "beebeep.off" ) );
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

QString Settings::defaultGroupsFilePath( bool use_resource_folder ) const
{
  QString root_folder = use_resource_folder ? resourceFolder() : dataFolder();
  return Bee::convertToNativeFolderSeparator( QString( "%1/%2" ).arg( root_folder ).arg( QLatin1String( "beegroups.ini" ) ) );
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

QString Settings::guiCustomListStyleSheet( const QString& background_color, const QString& background_image_path ) const
{
   return QString( "#GuiCustomList { background: %1 url(%2);"
                   "background-repeat: no-repeat;"
                   "background-position: bottom center;"
                   "background-attachment: fixed;"
                   "padding-bottom: 32px;"
                   "color: black; }" ).arg( background_color ).arg( background_image_path );
}

QString Settings::autoresponderName() const
{
  return QT_TRANSLATE_NOOP( "Settings", "Autoresponder" );
}
