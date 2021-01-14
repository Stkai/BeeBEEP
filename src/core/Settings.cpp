//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2021 Marco Mastroddi
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
// along with BeeBEEP. If not, see <http://www.gnu.org/licenses/>.
//
// Author: Marco Mastroddi <marco.mastroddi(AT)gmail.com>
//
// $Id$
//
//////////////////////////////////////////////////////////////////////

#include "BeeApplication.h"
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
  m_currentFilePath = "";

  /* Default RC start */
  m_enableSaveData = true;
  m_useChatWithAllUsers = true;
  m_useSettingsFileIni = true;
  m_broadcastOnlyToHostsIni = false;
  m_allowOnlyHostAddressesFromHostsIni = false;
  m_useOnlyMulticast = false;
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
  m_useFontEmoticons = false;
#if QT_VERSION >= 0x050000 && !defined( BEEBEEP_FOR_RASPBERRY_PI )
  m_useHiResEmoticons = true;
#else
  m_useHiResEmoticons = false;
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
  m_addNicknameToDataFolder = false;
  m_preferredSubnets = "";
  m_disableSystemProxyForConnections = true;
  m_useDefaultMulticastGroupAddress = true;
  m_ipMulticastTtl = DEFAULT_IPV4_MULTICAST_TTL_OPTION;
  m_useIPv6 = false;
  m_useHive = false;
  m_checkNewVersionAtStartup = true;
  m_postUsageStatistics = true;
  m_useHostnameForDefaultUsername = false;
  m_useEasyConnection = false;
  m_useUserFullName = false;
  m_appendHostNameToUserName = false;
  m_useCompactDataSaving = true;

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
#ifdef BEEBEEP_USE_SHAREDESKTOP
  m_disableDesktopSharing = false;
#else
  m_disableDesktopSharing = true;
#endif
#ifdef BEEBEEP_USE_VOICE_CHAT
  m_disableVoiceMessages = false;
#else
  m_disableVoiceMessages = true;
#endif

  m_startMinimized = false;
  m_signature = "";
  m_useOnlyTextEmoticons = false;
  m_disablePrivateChats = false;
  m_disableMulticast = false;
  m_userRecognitionMethod = RecognizeByDefaultMethod;
  m_canAddMembersToGroup = true;
  m_canRemoveMembersFromGroup = true;
  m_allowEditNickname = true;
  m_disableCreateMessage = false;
  m_clearCacheAfterDays = 96;
  m_removePartiallyDownloadedFilesAfterDays = 5;

  m_checkUserConnectedFromDatagramIp = false;
  m_skipLocalHardwareAddresses = QStringList();

  m_disableConnectionSocketEncryption = false;
  m_allowNotEncryptedConnectionsAlso = false;
  m_allowEncryptedConnectionsAlso = true;
  m_disableConnectionSocketDataCompression = false;

  m_allowedFileExtensionsInFileTransfer = QStringList();

  m_rcFileExists = false;

  m_connectionKeyExchangeMethod = ConnectionKeyExchangeAuto;
  /* Default RC end */

  m_emoticonSizeInEdit = 16;
  setChatFont( QApplication::font(), true );
  m_emoticonSizeInMenu = 24;
  m_emoticonInRecentMenu = 48;
  m_confirmOnDownloadFile = false;
  m_resetMinimumWidthForStyle = true;
  m_onExistingFileAction = GenerateNewFileName;
  m_resumeFileTransfer = true;
  m_promptOnCloseEvent = true;
  m_saveUserList = true;
  m_saveGroupList = true;
  m_localUser.setStatus( User::Online );
  m_localUser.setVersion( version( false, false, false ) );
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

  m_tickIntervalConnectionTimeout = TICK_INTERVAL_CONNECTION_TIMEOUT;
  m_useReturnToSendMessage = true;
  m_tickIntervalCheckIdle = 13;
  m_tickIntervalCheckNetwork = 5;
  m_tickIntervalBroadcasting = 0;
  m_broadcastToOfflineUsers = false;
  m_broadcastToLocalSubnetAlways = true;

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

  m_tickIntervalChatAutoSave = 61;

  m_useMessageTimestampWithAP = false;
  m_useDarkStyle = false;

  m_saveMessagesTimestamp = QDateTime::currentDateTime();

  m_voiceMessageMaxDuration = 120;
  m_useVoicePlayer = true;
  m_useCustomVoiceEncoderSettings = false;
  m_useSystemVoiceEncoderSettings = false;

  m_keepModificationDateOnFileTransferred = true;

  m_allowedFileExtensionsInFileTransfer = QStringList();

  resetAllColors();
}

QStringList Settings::resourceFolders() const
{
  QStringList system_folders;
  system_folders.append( dataFolder() );
  system_folders.append( resourceFolder() );
#ifdef Q_OS_MAC
  QDir mac_plugin_dir( QApplication::applicationDirPath() );
  if( mac_plugin_dir.cdUp() && mac_plugin_dir.cd( "PlugIns" ) )
    system_folders.append( Bee::convertToNativeFolderSeparator( mac_plugin_dir.absolutePath() ) );
#endif
#ifdef Q_OS_UNIX
  system_folders.append( QLatin1String( "/etc/beebeep/" ) );
  system_folders.append( QLatin1String( "/usr/local/etc/beebeep/" ) );
  system_folders.append( QLatin1String( "/usr/lib/beebeep/" ) );
  system_folders.append( QLatin1String( "/usr/local/lib/beebeep/" ) );
#endif
  system_folders.removeDuplicates();
  return system_folders;
}

QStringList Settings::dataFolders() const
{
  QStringList data_folders;
  data_folders.append( dataFolder() );
#if QT_VERSION >= 0x050400
  data_folders.append( Bee::convertToNativeFolderSeparator( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) ) );
#elif QT_VERSION >= 0x050000
  data_folders.append( Bee::convertToNativeFolderSeparator( QString( "%1/%2" ).arg( QStandardPaths::writableLocation( QStandardPaths::DataLocation ) ).arg( programName() ) ) );
#else
  data_folders.append( Bee::convertToNativeFolderSeparator( QDesktopServices::storageLocation( QDesktopServices::DataLocation ) ) );
#endif
#ifdef Q_OS_UNIX
  data_folders.append( QLatin1String( "/usr/share/beebeep/" ) );
  data_folders.append( QLatin1String( "/usr/local/share/beebeep/" ) );
#endif
  data_folders.append( cacheFolder() );
  data_folders.removeDuplicates();
  return data_folders;
}

QString Settings::findFileInFolders( const QString& file_name, const QStringList& folder_list, bool return_folder_path ) const
{
  foreach( QString folder_path, folder_list )
  {
    if( folder_path.trimmed().isEmpty() )
      continue;
    folder_path = Bee::convertToNativeFolderSeparator( folder_path );
    QString file_path = folder_path;
    if( !file_path.endsWith( Bee::nativeFolderSeparator() ) )
      file_path.append( Bee::nativeFolderSeparator() );
    file_path += file_name;
    QFileInfo fi( file_path );
    if( fi.exists() )
    {
      if( !fi.isReadable() )
        qWarning() << "Skip file" << qPrintable( file_path ) << "because is not readable";
      else
        return return_folder_path ? folder_path : file_path;
    }
  }
  return QString();
}

void Settings::setDefaultFolders()
{
  QString app_folder = Bee::convertToNativeFolderSeparator( QApplication::applicationDirPath() );
  qDebug() << "Applicaction folder:" << qPrintable( app_folder );

#ifdef Q_OS_MAC
  QDir macos_dir( app_folder );
  if( macos_dir.cdUp() && macos_dir.cd( "Resources" ) )
    m_resourceFolder = Bee::convertToNativeFolderSeparator( macos_dir.absolutePath() );
  else
    m_resourceFolder = app_folder;
#else
  m_resourceFolder = app_folder;
#endif
  qDebug() << "Resource folder:" << qPrintable( m_resourceFolder );

  if( !Bee::folderIsWriteable( m_resourceFolder, false ) )
  {
#if QT_VERSION >= 0x050400
    m_dataFolder = Bee::convertToNativeFolderSeparator( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) );
#elif QT_VERSION >= 0x050000
    m_dataFolder = Bee::convertToNativeFolderSeparator( QString( "%1/%2" )
                     .arg( QStandardPaths::writableLocation( QStandardPaths::DataLocation ) )
                     .arg( programName() ) );
#else
    m_dataFolder = Bee::convertToNativeFolderSeparator( QDesktopServices::storageLocation( QDesktopServices::DataLocation ) );
#endif
  }
  else
    m_dataFolder = m_resourceFolder;
  m_cacheFolder = defaultCacheFolderPath();
}

QString Settings::defaultListBackgroundColor() const
{
  return m_useDarkStyle ? Bee::colorDarkGrey().name() : QLatin1String( "#ffffff" );
}

QString Settings::defaultSystemBackgroundColor() const
{
  return m_useDarkStyle ? Bee::colorBlack().name() : QLatin1String( "#e0e0e0" );
}

void Settings::resetAllColors()
{
  m_chatDefaultUserNameColor = m_useDarkStyle ? "#ffffff" : "#000000";
  m_homeBackgroundColor = defaultSystemBackgroundColor();
  m_defaultChatBackgroundColor = defaultSystemBackgroundColor();
  m_userListBackgroundColor = defaultListBackgroundColor();
  m_chatListBackgroundColor = defaultListBackgroundColor();
  m_groupListBackgroundColor = defaultListBackgroundColor();
  m_savedChatListBackgroundColor = defaultListBackgroundColor();
  m_chatFontColor = m_useDarkStyle ? "#ffffff" : "#000000";
  m_chatBackgroundColor = m_useDarkStyle ? Bee::colorDarkGrey().name() : "#ffffff";
  m_chatDefaultTextColor = m_useDarkStyle ? "#ffffff" : "#000000";
  m_chatSystemTextColor = m_useDarkStyle ? Bee::colorGrey().name() : "#555555";
  m_chatQuoteBackgroundColor = Bee::colorGrey().name();
  m_chatQuoteTextColor = Bee::colorBlack().name();
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

void Settings::setChatFont( const QFont& new_value, bool resize_emoticon_size_in_chat )
{
  m_chatFont = new_value;
  QFontMetrics fm( m_chatFont );
  int fm_h = fm.height();
  if( resize_emoticon_size_in_chat )
  {
    m_emoticonSizeInChat = qMax( 24, qMin( 248, fm_h ) );
    if( m_emoticonSizeInChat % 2 )
      m_emoticonSizeInChat++;
  }
  m_emoticonSizeInEdit = qMax( 16, qMin( 48, fm_h ) );
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
  m_localUser.setProtocolVersion( protocolVersion() );
  m_localUser.setDomainName( QHostInfo::localDomainName() );
  m_localUser.setHash( Settings::instance().createLocalUserHash() );
  m_localUser.setLocalHostName( QHostInfo::localHostName() );

  qDebug() << "System - user name:" << qPrintable( m_localUser.name() );
  qDebug() << "System - account:" << qPrintable( m_localUser.accountName() );
  qDebug() << "System - domain name:" << qPrintable( m_localUser.domainName() );
  qDebug() << "System - host name:" << qPrintable( m_localUser.localHostName() );
#ifdef BEEBEEP_DEBUG
  qDebug() << "Local user hash:" << qPrintable( m_localUser.hash() );
#endif

  if( user_name.isEmpty() && m_addNicknameToDataFolder )
    setDataFolder();
}

QString Settings::createLocalUserHash()
{
  QString hash_parameters = QString( "%1%2%3%4%5%6%7%8" )
                              .arg( QString::number( Random::number32( 6475, 36475 ) ) )
                              .arg( m_localUser.accountName() ).arg( m_localUser.name() )
                              .arg( QString::number( Random::number32( 6475, 36475 ) ) )
                              .arg( m_localUser.domainName() ).arg( m_localUser.localHostName() ).arg( version( true, true, true ) )
                              .arg( QDateTime::currentDateTime().toString( "dd.MM.yyyy-hh:mm:ss.zzz" ) );
  QString local_user_hash = simpleHash( hash_parameters );
#ifdef BEEBEEP_DEBUG
  qDebug() << "Local user HASH created:" << qPrintable( local_user_hash );
#endif
  return local_user_hash;
}

bool Settings::createDefaultRcFile()
{
  QFileInfo rc_file_info( dataFolder() + Bee::nativeFolderSeparator() + QLatin1String( "beebeep.rc" ) );
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
    sets->setValue( "AllowOnlyHostAddressesFromHostsIni", m_allowOnlyHostAddressesFromHostsIni );
    sets->setValue( "UseOnlyMulticast", m_useOnlyMulticast );
    sets->setValue( "BroadcastPort", m_defaultBroadcastPort );
    sets->setValue( "DefaultListenerPort", m_defaultListenerPort );
    sets->setValue( "DefaultFileTransferPort", m_defaultFileTransferPort );
    sets->setValue( "SaveDataInDocumentsFolder", m_saveDataInDocumentsFolder );
    sets->setValue( "SaveDataInUserApplicationFolder", m_saveDataInUserApplicationFolder );
    sets->setValue( "AllowMultipleInstances", m_allowMultipleInstances );
    sets->setValue( "DataFolderPath", m_dataFolderInRC );
    sets->setValue( "AddAccountNameToDataFolder", m_addAccountNameToDataFolder );
    sets->setValue( "AddNicknameToDataFolder", m_addNicknameToDataFolder );
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
    sets->setValue( "DisableDesktopSharing", m_disableDesktopSharing );
    sets->setValue( "DisableSendMessage", m_disableSendMessage );
    sets->setValue( "DisableVoiceMessages", m_disableVoiceMessages );
    sets->setValue( "UseEasyConnection", m_useEasyConnection );
    sets->setValue( "UseCompactDataSaving", m_useCompactDataSaving );
    sets->setValue( "StartMinimized", m_startMinimized );
    sets->setValue( "Signature", m_signature );
    sets->setValue( "UseOnlyTextEmoticons", m_useOnlyTextEmoticons );
    sets->setValue( "DisablePrivateChats", m_disablePrivateChats );
    sets->setValue( "DisableMulticast", m_disableMulticast );
    sets->setValue( "UserRecognitionMethod", m_userRecognitionMethod );
    sets->setValue( "AllowAddMembersToGroup", m_canAddMembersToGroup );
    sets->setValue( "AllowRemoveMembersFromGroup", m_canRemoveMembersFromGroup );
    sets->setValue( "AllowEditNickname", m_allowEditNickname );
    sets->setValue( "DisableCreateMessage", m_disableCreateMessage );
    sets->setValue( "DisableMenuSettings", m_disableMenuSettings );
    sets->setValue( "CheckUserConnectedFromDatagramIp", m_checkUserConnectedFromDatagramIp );
    sets->setValue( "SkipLocalHardwareAddresses", m_skipLocalHardwareAddresses.isEmpty() ? QString( "" ) : m_skipLocalHardwareAddresses.join( ", " ) );
    sets->setValue( "DisableEncryptedConnections", m_disableConnectionSocketEncryption );
    sets->setValue( "AllowNotEncryptedConnectionsAlso", m_allowNotEncryptedConnectionsAlso );
    sets->setValue( "AllowEncryptedConnectionsAlso", m_allowEncryptedConnectionsAlso );
    sets->setValue( "UseUserFullName", m_useUserFullName );
    sets->setValue( "AppendHostNameToUserName", m_appendHostNameToUserName );
    sets->setValue( "DisableConnectionSocketDataCompression", m_disableConnectionSocketDataCompression );
    sets->setValue( "AllowedFileExtensionsInFileTransfer", m_allowedFileExtensionsInFileTransfer.isEmpty() ? QString( "" ) : m_allowedFileExtensionsInFileTransfer.join( ", " ) );
    sets->setValue( "ConnectionKeyExchangeMethod", m_connectionKeyExchangeMethod );
    sets->setValue( "TickIntervalChatAutoSave", m_tickIntervalChatAutoSave );
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

QSettings* Settings::objectRcSettings() const
{
  QString rc_file_path = findFileInFolders( QLatin1String( "beebeep.rc" ), resourceFolders() );
  if( rc_file_path.isEmpty() )
    return Q_NULLPTR;
  QSettings* sets = new QSettings( rc_file_path, QSettings::IniFormat );
  sets->setFallbacksEnabled( false );
  return sets;
}

void Settings::loadRcFile()
{
  QSettings *sets = objectRcSettings();
  if( sets )
  {
    qDebug() << "Loading settings from RC configuration file" << qPrintable( sets->fileName() );
    m_rcFileExists = true;
  }
  else
  {
    qDebug() << "RC configuration file not found";
    m_rcFileExists = false;
    return;
  }

  bool ok = false;
  sets->beginGroup( "Groups" );
  bool trust_system_account = sets->value( "TrustSystemAccount", false ).toBool(); // for compatibility
  sets->endGroup();

  sets->beginGroup( "BeeBEEP" );
  m_enableSaveData = sets->value( "EnableSaveData", m_enableSaveData ).toBool();
  m_useSettingsFileIni = sets->value( "UseConfigurationFileIni", m_useSettingsFileIni ).toBool();
  m_broadcastOnlyToHostsIni = sets->value( "BroadcastOnlyToHostsIni", m_broadcastOnlyToHostsIni ).toBool();
  m_allowOnlyHostAddressesFromHostsIni = sets->value( "AllowOnlyHostAddressesFromHostsIni", m_allowOnlyHostAddressesFromHostsIni ).toBool();
  m_useOnlyMulticast = sets->value( "UseOnlyMulticast", m_useOnlyMulticast ).toBool();
  m_defaultBroadcastPort = sets->value( "BroadcastPort", m_defaultBroadcastPort ).toInt();
  m_defaultListenerPort = sets->value( "DefaultListenerPort", m_defaultListenerPort ).toInt();
  m_defaultFileTransferPort = sets->value( "DefaultFileTransferPort", m_defaultFileTransferPort ).toInt();
  m_saveDataInDocumentsFolder = sets->value( "SaveDataInDocumentsFolder", m_saveDataInDocumentsFolder ).toBool();
  m_saveDataInUserApplicationFolder = sets->value( "SaveDataInUserApplicationFolder", m_saveDataInUserApplicationFolder ).toBool();
  m_allowMultipleInstances = sets->value( "AllowMultipleInstances", m_allowMultipleInstances ).toBool();
  m_dataFolderInRC = Bee::convertToNativeFolderSeparator( sets->value( "DataFolderPath", m_dataFolderInRC ).toString() );
  m_addAccountNameToDataFolder = sets->value( "AddAccountNameToDataFolder", m_addAccountNameToDataFolder ).toBool();
  m_addNicknameToDataFolder = sets->value( "AddNicknameToDataFolder", m_addNicknameToDataFolder ).toBool();
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
  m_disableDesktopSharing = sets->value( "DisableDesktopSharing", m_disableDesktopSharing ).toBool();
  m_disableSendMessage = sets->value( "DisableSendMessage", m_disableSendMessage ).toBool();
  m_disableVoiceMessages = sets->value( "DisableVoiceMessages", m_disableVoiceMessages ).toBool();
  m_useEasyConnection = sets->value( "UseEasyConnection", m_useEasyConnection ).toBool();
  m_useCompactDataSaving = sets->value( "UseCompactDataSaving", m_useCompactDataSaving ).toBool();
  m_startMinimized = sets->value( "StartMinimized", m_startMinimized ).toBool();
  m_signature = sets->value( "Signature", m_signature ).toString();
  m_useOnlyTextEmoticons = sets->value( "UseOnlyTextEmoticons", m_useOnlyTextEmoticons ).toBool();
  m_disablePrivateChats = sets->value( "DisablePrivateChats", m_disablePrivateChats ).toBool();
  m_disableMulticast = sets->value( "DisableMulticast", m_disableMulticast ).toBool();
  int user_recognition_method = sets->value( "UserRecognitionMethod", -1 ).toInt();
  setUserRecognitionMethod( (user_recognition_method < 0 ? (trust_system_account ? RecognizeByAccount : m_userRecognitionMethod) : user_recognition_method) );

  m_canAddMembersToGroup = sets->value( "AllowAddMembersToGroup", m_canAddMembersToGroup ).toBool();
  m_canRemoveMembersFromGroup = sets->value( "AllowRemoveMembersFromGroup", m_canRemoveMembersFromGroup ).toBool();
  m_allowEditNickname = sets->value( "AllowEditNickname", m_allowEditNickname ).toBool();
  m_disableCreateMessage = sets->value( "DisableCreateMessage", m_disableCreateMessage ).toBool();
  m_disableMenuSettings = sets->value( "DisableMenuSettings", m_disableMenuSettings ).toBool();
  m_clearCacheAfterDays = qMax( -1, sets->value( "ClearCacheAfterDays", m_clearCacheAfterDays ).toInt() );
  m_removePartiallyDownloadedFilesAfterDays = qMax( -1, sets->value( "RemovePartiallyDownloadedFilesAfterDays", m_removePartiallyDownloadedFilesAfterDays ).toInt() );
  m_checkUserConnectedFromDatagramIp = sets->value( "CheckUserConnectedFromDatagramIp", m_checkUserConnectedFromDatagramIp ).toBool();
  // Remember to use "" for the string in INI files
  QString local_hw_addresses = sets->value( "SkipLocalHardwareAddresses", m_skipLocalHardwareAddresses.join( "," ) ).toString().simplified();
  m_skipLocalHardwareAddresses.clear();
  if( !local_hw_addresses.isEmpty() )
  {
    QStringList local_hw_address_list = local_hw_addresses.split( "," );
    foreach( QString hw_value, local_hw_address_list )
    {
      hw_value = hw_value.trimmed();
      if( !hw_value.isEmpty() )
        m_skipLocalHardwareAddresses.append( hw_value );
    }
    m_skipLocalHardwareAddresses.removeDuplicates();
  }
  m_disableConnectionSocketEncryption = sets->value( "DisableEncryptedConnections", m_disableConnectionSocketEncryption ).toBool();
  m_allowNotEncryptedConnectionsAlso = sets->value( "AllowNotEncryptedConnectionsAlso", m_allowNotEncryptedConnectionsAlso ).toBool();
  m_allowEncryptedConnectionsAlso = sets->value( "AllowEncryptedConnectionsAlso", m_allowEncryptedConnectionsAlso ).toBool();
  m_useUserFullName = sets->value( "UseUserFullName", m_useUserFullName ).toBool();
  m_appendHostNameToUserName = sets->value( "AppendHostNameToUserName", m_appendHostNameToUserName ).toBool();
  m_disableConnectionSocketDataCompression = sets->value( "DisableConnectionSocketDataCompression", m_disableConnectionSocketDataCompression ).toBool();
  // Remember to use "" for the string in INI files
  QString allowed_file_extensions = sets->value( "AllowedFileExtensionsInFileTransfer", m_allowedFileExtensionsInFileTransfer.join( "," ) ).toString().simplified();
  m_allowedFileExtensionsInFileTransfer.clear();
  if( !allowed_file_extensions.trimmed().isEmpty() )
  {
    QStringList allowed_file_extensions_list = allowed_file_extensions.split( "," );
    foreach( QString file_ext, allowed_file_extensions_list )
    {
      file_ext = file_ext.trimmed();
      if( !file_ext.isEmpty() )
      {
        if( file_ext.startsWith( "*" ) )
          file_ext.remove( 0, 1 );
        if( file_ext.startsWith( "." ) )
          file_ext.remove( 0, 1 );
        if( !file_ext.isEmpty() )
          m_allowedFileExtensionsInFileTransfer.append( file_ext.toLower() );
      }
    }
    m_allowedFileExtensionsInFileTransfer.removeDuplicates();
  }

  m_connectionKeyExchangeMethod = sets->value( "ConnectionKeyExchangeMethod", m_connectionKeyExchangeMethod ).toInt( &ok );
  if( !ok || m_connectionKeyExchangeMethod < 0 || m_connectionKeyExchangeMethod >= NumConnectionKeyExchangeMethods )
    m_connectionKeyExchangeMethod = ConnectionKeyExchangeAuto;
  sets->endGroup();

  QStringList key_list = sets->allKeys();
  foreach( QString key, key_list )
  {
    QString sets_value = sets->value( key ).toString();
    if( sets_value.isEmpty() )
      sets_value = sets->value( key ).toStringList().join( ", " );
    qDebug() << "RC read ->" << qPrintable( key ) << "=" << qPrintable( sets_value );
  }
  qDebug() << "RC configuration file read";
  if( !m_skipLocalHardwareAddresses.isEmpty() )
  {
    foreach( QString hw_value, m_skipLocalHardwareAddresses )
      qDebug() << "Skip local hardware address:" << qPrintable( hw_value );
  }
  if( !m_allowedFileExtensionsInFileTransfer.isEmpty() )
    qWarning() << "File transfer allows only these extensions:" << qPrintable( m_allowedFileExtensionsInFileTransfer.join( ", " ).toUpper() );
  delete sets;
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

  QString file_hosts_path = dataFolder() + Bee::nativeFolderSeparator() + QLatin1String( "beehosts.ini" );
  QFile file_hosts_ini( file_hosts_path );
  if( file_hosts_ini.exists() )
  {
    qDebug() << "HOSTS default configuration file exists in" << qPrintable( file_hosts_path );
    return false;
  }

  if( file_hosts_ini.open( QIODevice::WriteOnly ) )
  {
    QTextStream ts( &file_hosts_ini );
    foreach( QString line, sl )
      ts << line << "\n";
    ts.flush();
    file_hosts_ini.close();
    qDebug() << "HOSTS default configuration file created in" << qPrintable( file_hosts_path );
    return true;
  }
  else
  {
    qWarning() << "Unable to create the HOSTS default configuration file in" << qPrintable( file_hosts_path );
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

QString Settings::version( bool build_version, bool qt_version, bool debug_info ) const
{
  QString s_version = QString( BEEBEEP_VERSION );
  if( build_version )
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
  {
    s_version += QString( "-qt%1" ).arg( qtMajorVersion() );
#if defined( Q_PROCESSOR_X86_64 )
    s_version += QString( "-64bit" );
#else
    s_version += QString( "-32bit" );
#endif
  }
  return s_version;
}

QString Settings::httpUserAgent() const
{
  return QString( "%1 %2" ).arg( programName() ).arg( version( false, false, false ) );
}

int Settings::protocolVersion() const
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
  os_name_long = "OS/2";
  os_name_short = "OS2";
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
    m_localUser.setNetworkAddress( NetworkAddress( m_useIPv6 ? QHostAddress::LocalHostIPv6 : QHostAddress::LocalHost, host_port ) );
  else
    m_localUser.setNetworkAddress( NetworkAddress( host_address, host_port ) );
}

QString Settings::defaultHostsFilePath() const
{
  return findFileInFolders( QLatin1String( "beehosts.ini" ), resourceFolders() );
}

void Settings::loadBroadcastAddressesFromFileHosts()
{
  if( !m_broadcastAddressesInFileHosts.isEmpty() )
    m_broadcastAddressesInFileHosts.clear();

  QString hosts_file_path = defaultHostsFilePath();
  if( hosts_file_path.isNull() )
  {
    qDebug() << "File beehosts.ini not found";
    return;
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

QString Settings::checkFolderPath( const QString& folder_path, const QString& default_value, bool check_writable )
{
  if( folder_path.isEmpty() )
    return default_value;

  if( check_writable )
  {
    if( Bee::folderIsWriteable( folder_path, false ) )
      return folder_path;
  }
  else
  {
    if( QFile::exists( folder_path ) )
      return folder_path;
  }

  qWarning() << "The folder" << folder_path << "load in settings is not usable and" << default_value << "is taken by default";
  return default_value;
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
  // remember to delete it
  return sets;
}

void Settings::load()
{
  qDebug() << "Loading settings";
  QSettings *sets = objectSettings();
#ifdef Q_OS_WIN
  if( m_useSettingsFileIni )
    m_currentFilePath = Bee::convertToNativeFolderSeparator( sets->fileName() );
  else
    m_currentFilePath = ""; // registry path
#else
  m_currentFilePath = Bee::convertToNativeFolderSeparator( sets->fileName() );
#endif
  beeApp->setSettingsFilePath( m_currentFilePath );

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
  loadCommonSettings( sets );

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

#if QT_VERSION >= 0x050000
  m_lastDirectorySelected = Bee::convertToNativeFolderSeparator( sets->value( "LastDirectorySelected", QStandardPaths::writableLocation( QStandardPaths::DocumentsLocation ) ).toString() );
  m_downloadDirectory = Bee::convertToNativeFolderSeparator( sets->value( "DownloadDirectory", QStandardPaths::writableLocation( QStandardPaths::DownloadLocation ) ).toString() );
#else
  m_lastDirectorySelected = Bee::convertToNativeFolderSeparator( sets->value( "LastDirectorySelected", QDesktopServices::storageLocation( QDesktopServices::DocumentsLocation ) ).toString() );
  m_downloadDirectory = Bee::convertToNativeFolderSeparator( sets->value( "DownloadDirectory", QDesktopServices::storageLocation( QDesktopServices::DocumentsLocation ) ).toString() );
#endif
  m_downloadDirectory = checkFolderPath( m_downloadDirectory, defaultDownloadFolderPath(), true );
  m_lastDirectorySelected = checkFolderPath( m_lastDirectorySelected, dataFolder(), false );
  m_logPath = checkFolderPath( Bee::convertToNativeFolderSeparator( sets->value( "LogFolderPath", dataFolder() ).toString() ), dataFolder(), true );
  QString plugin_folder_path = defaultPluginFolderPath();
  m_pluginPath = checkFolderPath( Bee::convertToNativeFolderSeparator( sets->value( "PluginPath", plugin_folder_path ).toString() ), plugin_folder_path, false );
  QString language_folder_path = defaultLanguageFolderPath();
  m_languagePath = checkFolderPath( Bee::convertToNativeFolderSeparator( sets->value( "LanguagePath", language_folder_path ).toString() ), language_folder_path, false );
  m_beepFilePath = checkFilePath( Bee::convertToNativeFolderSeparator( sets->value( "BeepFilePath", defaultBeepFilePath() ).toString() ), defaultBeepFilePath() );
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
  qDebug() << "Loading host addresses from file HOSTS";
  Settings::instance().loadBroadcastAddressesFromFileHosts();
  qDebug() << "Loading settings completed";
  delete sets;
}


void Settings::beginCommonGroup( QSettings* system_rc, QSettings* user_ini, const QString& group_name )
{
  if( system_rc )
    system_rc->beginGroup( group_name );
  if( user_ini )
    user_ini->beginGroup( group_name );
}

void Settings::endCommonGroup( QSettings* system_rc, QSettings* user_ini )
{
  if( system_rc )
    system_rc->endGroup();
  if( user_ini )
    user_ini->endGroup();
}

QVariant Settings::commonValue( QSettings* system_rc, QSettings* user_ini, const QString& key, const QVariant& default_value )
{
  if( system_rc && user_ini )
    return system_rc->value( key, user_ini->value( key, default_value ) );
  else if( system_rc )
    return system_rc->value( key, default_value );
  else if( user_ini )
    return user_ini->value( key, default_value );
  else
    return default_value;
}

void Settings::loadCommonSettings( QSettings* user_ini )
{
  qDebug() << "Loading settings eventually overruled by RC file";
  QSettings* system_rc = objectRcSettings();
  beginCommonGroup( system_rc, user_ini, "Chat" );
  QString chat_font_string = commonValue( system_rc, user_ini,  "Font", "" ).toString();
  if( !chat_font_string.isEmpty() )
  {
    QFont f;
    if( f.fromString( chat_font_string ) )
      setChatFont( f, false );
    else
      qWarning() << "Invalid font string found in ChatFont setting value:" << chat_font_string;
  }
  else
    setChatFont( QApplication::font(), false );
  m_chatFontColor = commonValue( system_rc, user_ini, "FontColor", QColor( Qt::black ).name() ).toString();
  m_defaultChatBackgroundColor = commonValue( system_rc, user_ini, "DefaultChatBackgroundColor", m_defaultChatBackgroundColor ).toString();
  m_chatCompact = commonValue( system_rc, user_ini, "CompactMessage", true ).toBool();
  m_chatShowMessageTimestamp = commonValue( system_rc, user_ini, "ShowMessageTimestamp", true ).toBool();
  m_beepOnNewMessageArrived = commonValue( system_rc, user_ini, "BeepOnNewMessageArrived", true ).toBool();
  m_disableBeepInUserStatusBusy = commonValue( system_rc, user_ini, "DisableBeepInUserStatusBusy", false ).toBool();
  m_beepInActiveWindowAlso = commonValue( system_rc, user_ini, "EnableBeepInActiveWindow", false ).toBool();
  m_chatUseHtmlTags = commonValue( system_rc, user_ini, "UseHtmlTags", false ).toBool();
  m_chatUseClickableLinks = commonValue( system_rc, user_ini, "UseClickableLinks", true ).toBool();
  m_chatMessageHistorySize = commonValue( system_rc, user_ini, "MessageHistorySize", 10 ).toInt();
  m_showEmoticons = commonValue( system_rc, user_ini, "ShowEmoticons", true ).toBool();
  m_showMessagesGroupByUser = commonValue( system_rc, user_ini, "ShowMessagesGroupByUsers", true ).toBool();
  m_chatMessageFilter = commonValue( system_rc, user_ini, "MessageFilter", m_chatMessageFilter ).toBitArray();
  if( m_chatMessageFilter.size() < static_cast<int>(ChatMessage::NumTypes) )
    m_chatMessageFilter.resize( static_cast<int>(ChatMessage::NumTypes) );
  m_showOnlyMessagesInDefaultChat = commonValue( system_rc, user_ini, "ShowOnlyMessagesInDefaultChat", true ).toBool();
  m_chatMessagesToShow = commonValue( system_rc, user_ini, "MaxMessagesToShow", defaultChatMessagesToShow() ).toInt();
  m_imagePreviewHeight = qMax( 48, commonValue( system_rc, user_ini, "ImagePreviewHeight", 160 ).toInt() );
  m_useReturnToSendMessage = commonValue( system_rc, user_ini, "UseKeyReturnToSendMessage", m_useReturnToSendMessage ).toBool();
  m_chatUseYourNameInsteadOfYou = commonValue( system_rc, user_ini, "UseYourNameInsteadOfYou", false ).toBool();
  m_chatClearAllReadMessages = commonValue( system_rc, user_ini, "ClearAllReadMessages", false ).toBool();
  m_chatUseColoredUserNames = commonValue( system_rc, user_ini, "UseColoredUserNames", true ).toBool();
  m_chatDefaultUserNameColor = commonValue( system_rc, user_ini, "DefaultUserNameColor", "#000" ).toString();
  m_chatActiveWindowOpacityLevel = qMax( 10, qMin( 100, commonValue( system_rc, user_ini, "ActiveWindowOpacityLevel", m_chatActiveWindowOpacityLevel ).toInt() ) );
  m_chatInactiveWindowOpacityLevel = qMax( 10, qMin( 100, commonValue( system_rc, user_ini, "InactiveWindowOpacityLevel", m_chatInactiveWindowOpacityLevel ).toInt() ) );
  m_chatBackgroundColor = commonValue( system_rc, user_ini, "BackgroundColor", m_chatBackgroundColor ).toString();
  m_chatDefaultTextColor = commonValue( system_rc, user_ini, "DefaultTextColor", m_chatDefaultTextColor ).toString();
  m_chatSystemTextColor = commonValue( system_rc, user_ini, "SystemTextColor", m_chatSystemTextColor ).toString();
  m_enableDefaultChatNotifications = commonValue( system_rc, user_ini, "EnableDefaultChatNotifications", m_enableDefaultChatNotifications ).toBool();
  m_useMessageTimestampWithAP = commonValue( system_rc, user_ini, "UseMessageTimestampWithAP", m_useMessageTimestampWithAP ).toBool();
  m_chatQuoteBackgroundColor = commonValue( system_rc, user_ini, "QuoteBackgroundColor", m_chatQuoteBackgroundColor ).toString();
  m_chatQuoteTextColor = commonValue( system_rc, user_ini, "QuoteTextColor", m_chatQuoteTextColor ).toString();
  m_chatOnSendingMessage = commonValue( system_rc, user_ini, "CloseOnSendingMessage", SkipOnSendingMessage ).toInt();
  if( m_chatOnSendingMessage < 0 || m_chatOnSendingMessage >= NumChatOnSendingMessageTypes )
    m_chatOnSendingMessage = SkipOnSendingMessage;
  endCommonGroup( system_rc, user_ini );

  beginCommonGroup( system_rc, user_ini, "User" );
  setUserRecognitionMethod( commonValue( system_rc, user_ini, "RecognitionMethod", m_userRecognitionMethod ).toInt() );
  m_localUser.setHash( user_ini->value( "LocalHash", m_localUser.hash() ).toString() );
  m_localUser.setName( user_ini->value( "LocalName", m_localUser.name() ).toString() );
  m_localUser.setColor( user_ini->value( "LocalColor", m_localUser.color() ).toString() );
  m_localUser.setStatusDescription( user_ini->value( "LocalLastStatusDescription", m_localUser.statusDescription() ).toString() );
  m_autoUserAway = commonValue( system_rc, user_ini, "AutoAwayStatus", false ).toBool();
  m_userAwayTimeout = qMax( commonValue( system_rc, user_ini, "UserAwayTimeout", 10 ).toInt(), 1 ); // minutes
  if( m_useEasyConnection )
  {
    m_useDefaultPassword = true;
    m_askChangeUserAtStartup = false;
    m_askPasswordAtStartup = false;
  }
  else
  {
    m_useDefaultPassword = user_ini->value( "UseDefaultPassword", true ).toBool();
    m_askChangeUserAtStartup = user_ini->value( "AskChangeUserAtStartup", m_firstTime ).toBool();
    m_askPasswordAtStartup = user_ini->value( "AskPasswordAtStartup", false ).toBool();
  }

  m_savePassword = user_ini->value( "SavePassword", false ).toBool();
  QString enc_pass = "";
  if( m_savePassword )
    enc_pass = simpleDecrypt( user_ini->value( "EncPwd", "" ).toString() );
  setPassword( enc_pass );
  m_saveUserList = commonValue( system_rc, user_ini, "SaveUsers", m_saveUserList ).toBool();
  QString user_list = user_ini->value( "List", "" ).toString();
  if( !user_list.isEmpty() )
    m_userList = simpleDecrypt( user_list ).split( QString( "\n" ) );
  else
    m_userList = QStringList();
  QString user_status_list = user_ini->value( "StatusList", "" ).toString();
  if( !user_status_list.isEmpty() )
    m_userStatusList = simpleDecrypt( user_status_list ).split( QString( "\n" ) );
  else
    m_userStatusList = QStringList();
   m_refusedChats = user_ini->value( "RefusedChats", QStringList() ).toStringList();

  m_maxUserStatusDescriptionInList = commonValue( system_rc, user_ini, "MaxStatusDescriptionInList", m_maxUserStatusDescriptionInList ).toInt();
  m_presetMessages = commonValue( system_rc, user_ini, "PresetMessages", QMap<QString,QVariant>() ).toMap();
  m_maxDaysOfUserInactivity = commonValue( system_rc, user_ini, "MaxDaysOfUserInactivity", m_maxDaysOfUserInactivity ).toInt();
  m_removeInactiveUsers = commonValue( system_rc, user_ini, "RemoveInactiveUsers", true ).toBool();
  endCommonGroup( system_rc, user_ini );

  beginCommonGroup( system_rc, user_ini, "Gui" );
  m_mainBarIconSize = commonValue( system_rc, user_ini, "MainBarIconSize", QSize( 24, 24 ) ).toSize();
  m_avatarIconSize = commonValue( system_rc, user_ini, "AvatarIconSize", QSize( 28, 28 ) ).toSize();
  m_resetGeometryAtStartup = commonValue( system_rc, user_ini, "ResetWindowGeometryAtStartup", m_resetGeometryAtStartup ).toBool();
  m_saveGeometryOnExit = commonValue( system_rc, user_ini, "SaveGeometryOnExit", m_saveGeometryOnExit ).toBool();
  m_language = commonValue( system_rc, user_ini, "Language", QLocale::system().name() ).toString();
  if( m_language.size() > 2 )
    m_language.resize( 2 );
  m_keyEscapeMinimizeInTray = commonValue( system_rc, user_ini, "KeyEscapeMinimizeInTray", true ).toBool();
#ifdef Q_OS_MAC
  m_closeMinimizeInTray = false;
#else
  if( m_settingsVersion < 18 )
    m_closeMinimizeInTray = commonValue( system_rc, user_ini, "MinimizeInTray", true ).toBool();
  else
    m_closeMinimizeInTray = commonValue( system_rc, user_ini, "CloseMinimizeInTray", true ).toBool();
#endif
  m_stayOnTop = commonValue( system_rc, user_ini, "StayOnTop", false ).toBool();
  m_raiseOnNewMessageArrived = commonValue( system_rc, user_ini, "RaiseOnNewMessageArrived", false ).toBool();
  m_raiseMainWindowOnNewMessageArrived = commonValue( system_rc, user_ini, "RaiseMainWindowOnNewMessageArrived", false ).toBool();
  m_alwaysShowFileTransferProgress = commonValue( system_rc, user_ini, "AlwaysShowFileTransferProgress", false ).toBool();
  m_alwaysOpenChatOnNewMessageArrived = commonValue( system_rc, user_ini, "AlwaysOpenChatOnNewMessageArrived", true ).toBool();
  m_loadOnTrayAtStartup = commonValue( system_rc, user_ini, "LoadOnTrayAtStartup", false ).toBool();
  m_showNotificationOnTray = commonValue( system_rc, user_ini, "ShowNotificationOnTray", true ).toBool();
  m_showOnlyMessageNotificationOnTray = commonValue( system_rc, user_ini, "ShowOnlyMessageNotificationOnTray", true ).toBool();
  m_trayMessageTimeout = qMax( commonValue( system_rc, user_ini, "ShowNotificationOnTrayTimeout", 10000 ).toInt(), 1000 );
  m_showChatMessageOnTray = commonValue( system_rc, user_ini, "ShowChatMessageOnTray", false ).toBool();
  m_textSizeInChatMessagePreviewOnTray = commonValue( system_rc, user_ini, "TextSizeInChatMessagePreviewOnTray", 40 ).toInt();
  m_showFileTransferCompletedOnTray = commonValue( system_rc, user_ini, "ShowFileTransferCompletedOnTray", true ).toBool();
  m_chatAutoSave = commonValue( system_rc, user_ini, "ChatAutoSave", true ).toBool();
  m_tickIntervalChatAutoSave = commonValue( system_rc, user_ini, "TickIntervalChatAutoSave", m_tickIntervalChatAutoSave ).toInt();
  m_chatSaveUnsentMessages = commonValue( system_rc, user_ini, "ChatSaveUnsentMessages", true ).toBool();
  m_chatMaxLineSaved = commonValue( system_rc, user_ini, "ChatMaxLineSaved", 9000 ).toInt();
  m_chatSaveFileTransfers = commonValue( system_rc, user_ini, "ChatSaveFileTransfers", m_chatAutoSave ).toBool();
  m_chatSaveSystemMessages = commonValue( system_rc, user_ini, "ChatSaveSystemMessages", false ).toBool();
  m_showChatToolbar = commonValue( system_rc, user_ini, "ShowChatToolbar", true ).toBool();
  m_showOnlyOnlineUsers = commonValue( system_rc, user_ini, "ShowOnlyOnlineUsers", false ).toBool();
  m_showUserPhoto = commonValue( system_rc, user_ini, "ShowUserPhoto", true ).toBool();
  m_showVCardOnRightClick = commonValue( system_rc, user_ini, "ShowVCardOnRightClick", true ).toBool();
  m_showEmoticonMenu = commonValue( system_rc, user_ini, "ShowEmoticonMenu", false ).toBool();
  m_showPresetMessages = commonValue( system_rc, user_ini, "ShowPresetMessages", false ).toBool();
  m_emoticonSizeInEdit = qMax( 12, commonValue( system_rc, user_ini, "EmoticonSizeInEdit", m_emoticonSizeInEdit ).toInt() );
  m_emoticonSizeInChat = qMax( 12, commonValue( system_rc, user_ini, "EmoticonSizeInChat", m_emoticonSizeInChat ).toInt() );
  m_emoticonSizeInMenu = commonValue( system_rc, user_ini, "EmoticonSizeInMenu", m_emoticonSizeInMenu ).toInt();
  m_emoticonInRecentMenu = commonValue( system_rc, user_ini, "EmoticonsInRecentMenu", m_emoticonInRecentMenu ).toInt();
  m_favoriteEmoticons = user_ini->value( "FavoriteEmoticons", QStringList() ).toStringList();
  m_recentEmoticons = user_ini->value( "RecentEmoticons", QStringList() ).toStringList();
  m_useFontEmoticons = commonValue( system_rc, user_ini, "UseFontEmoticons", m_useFontEmoticons ).toBool();
#if QT_VERSION >= 0x050000 && !defined( BEEBEEP_FOR_RASPBERRY_PI )
  m_useHiResEmoticons = commonValue( system_rc, user_ini, "UseHighResolutionEmoticons", m_useHiResEmoticons ).toBool();
#else
  m_useHiResEmoticons = false;
#endif
  m_showMinimizedAtStartup = commonValue( system_rc, user_ini, "ShowMinimizedAtStartup", m_startMinimized ).toBool();
  m_promptOnCloseEvent = commonValue( system_rc, user_ini, "PromptOnCloseEvent", m_promptOnCloseEvent ).toBool();
  m_showUserStatusBackgroundColor = commonValue( system_rc, user_ini, "ShowUserStatusBackgroundColor", false ).toBool();
  m_showUserStatusDescription = commonValue( system_rc, user_ini, "ShowUserStatusDescription", true ).toBool();
  m_shortcuts = commonValue( system_rc, user_ini, "Shortcuts", QStringList() ).toStringList();
  m_useShortcuts = commonValue( system_rc, user_ini, "UseShortcuts", false ).toBool();
  m_useNativeDialogs = commonValue( system_rc, user_ini, "UseNativeFileDialogs", m_useNativeDialogs ).toBool();
  m_homeShowMessageTimestamp = commonValue( system_rc, user_ini, "ShowActivitiesTimestamp", false ).toBool();
  m_homeBackgroundColor = commonValue( system_rc, user_ini, "HomeBackgroundColor", m_homeBackgroundColor ).toString();
  m_userListBackgroundColor = commonValue( system_rc, user_ini, "UserListBackgroundColor", m_userListBackgroundColor ).toString();
  m_chatListBackgroundColor = commonValue( system_rc, user_ini, "ChatListBackgroundColor", m_chatListBackgroundColor ).toString();
  m_groupListBackgroundColor = commonValue( system_rc, user_ini, "GroupListBackgroundColor", m_groupListBackgroundColor ).toString();
  m_savedChatListBackgroundColor = commonValue( system_rc, user_ini, "SavedChatListBackgroundColor", m_savedChatListBackgroundColor ).toString();
  m_usePreviewFileDialog = commonValue( system_rc, user_ini, "UsePreviewFileDialog", m_usePreviewFileDialog ).toBool();
  m_previewFileDialogImageSize = qMax( 100, commonValue( system_rc, user_ini, "PreviewFileDialogImageSize", m_previewFileDialogImageSize ).toInt() );
  m_userSortingMode = qMax( 0, commonValue( system_rc, user_ini, "UserSortingMode", 0 ).toInt() );
  m_sortUsersAscending = commonValue( system_rc, user_ini, "SortUsersAscending", true ).toBool();
  m_showTextInModeRTL = commonValue( system_rc, user_ini, "ShowChatTextInModeRTL", m_showTextInModeRTL ).toBool();
  m_playBuzzSound = commonValue( system_rc, user_ini, "PlayBuzzSound", true ).toBool();
  bool open_chat_in_new_window = commonValue( system_rc, user_ini, "AlwaysOpenNewFloatingChat", !m_showChatsInOneWindow ).toBool();
  m_showChatsInOneWindow = commonValue( system_rc, user_ini, "ShowChatsInOneWindow", !open_chat_in_new_window ).toBool();
  m_iconSourcePath = commonValue( system_rc, user_ini, "IconSourcePath", m_iconSourcePath ).toString();
  m_emoticonSourcePath = commonValue( system_rc, user_ini, "EmoticonSourcePath", m_emoticonSourcePath ).toString();
  m_maxChatsToOpenAfterSendingMessage = commonValue( system_rc, user_ini, "MaxChatsToOpenAfterSendingMessage", m_maxChatsToOpenAfterSendingMessage ).toInt();
  m_showUsersOnConnection = commonValue( system_rc, user_ini, "ShowUsersOnConnection", m_showUsersOnConnection ).toBool();
  m_showChatsOnConnection = commonValue( system_rc, user_ini, "ShowChatsOnConnection", m_showChatsOnConnection ).toBool();
  if( m_showChatsOnConnection && m_showUsersOnConnection )
    m_showUsersOnConnection = false;
  m_hideEmptyChatsInList = commonValue( system_rc, user_ini, "HideEmptyChatsInList", m_hideEmptyChatsInList ).toBool();
  m_enableMaximizeButton = commonValue( system_rc, user_ini, "EnableMaximizeButton", false ).toBool();
#if QT_VERSION > 0x050000
  m_useDarkStyle = user_ini->value( "UseDarkStyle", m_useDarkStyle ).toBool();
#else
  m_useDarkStyle = false;
#endif
  m_showUsersInWorkgroups = commonValue( system_rc, user_ini, "ShowUsersInWorkgroups", false ).toBool();
  m_openChatWhenSendNewMessage = user_ini->value( "OpenChatWhenSendNewMessage", true ).toBool();
  m_sendNewMessageIndividually = user_ini->value( "SendNewMessageIndividually", false ).toBool();
  m_useUserFirstNameFirstInFullName = commonValue( system_rc, user_ini, "ShowUserFirstNameFirstInFullName", useUserFirstNameFirstInFullNameFromLanguage() ).toBool();
  m_resetMinimumWidthForStyle = commonValue( system_rc, user_ini, "ResetMinimumWidthForStyle", m_resetMinimumWidthForStyle ).toBool();
  endCommonGroup( system_rc, user_ini );

  beginCommonGroup( system_rc, user_ini, "Tools" );
  m_logToFile = commonValue( system_rc, user_ini, "LogToFile", false ).toBool();
  m_maxLogLines = commonValue( system_rc, user_ini, "MaxLogLines", m_maxLogLines ).toInt();
  m_useSpellChecker = commonValue( system_rc, user_ini, "UseSpellChecker", true ).toBool();
  m_useWordCompleter = commonValue( system_rc, user_ini, "UseWordCompleter", false ).toBool();
  m_checkNewVersionAtStartup = commonValue( system_rc, user_ini, "SearchForNewVersionAtStartup", m_checkNewVersionAtStartup ).toBool();
  m_postUsageStatistics = commonValue( system_rc, user_ini, "SendAnonymousUsageStatistics", m_postUsageStatistics ).toBool();
  m_dictionaryPath = checkFilePath( user_ini->value( "DictionaryPath", "" ).toString(), "" );
  m_applicationUuid = user_ini->value( "Uuid", "" ).toString();
  m_applicationUuidCreationDate = user_ini->value( "UuidCreationDate", QDate::currentDate() ).toDate();
  m_statsPostDate = user_ini->value( "StatsPostDate", QDate() ).toDate();
  endCommonGroup( system_rc, user_ini );

  beginCommonGroup( system_rc, user_ini, "Misc" );
  m_tickIntervalCheckIdle = qMax( commonValue( system_rc, user_ini, "TickIntervalCheckIdle", m_tickIntervalCheckIdle ).toInt(), 2 );
  m_tickIntervalCheckNetwork = qMax( commonValue( system_rc, user_ini, "TickIntervalCheckNetwork", m_tickIntervalCheckNetwork ).toInt(), 5 );
  m_tickIntervalBroadcasting = qMax( commonValue( system_rc, user_ini, "TickIntervalBroadcasting", m_tickIntervalBroadcasting ).toInt(), 0 );
  NetworkAddress local_user_network_address = m_localUser.networkAddress();
  local_user_network_address.setHostPort( static_cast<quint16>(commonValue( system_rc, user_ini, "ListenerPort", DEFAULT_LISTENER_PORT ).toUInt()) );
  m_localUser.setNetworkAddress( local_user_network_address );
  m_pongTimeout = qMax( commonValue( system_rc, user_ini, "ConnectionActivityTimeout_ms", PONG_DEFAULT_TIMEOUT ).toInt(), 13000 );
  if( m_pongTimeout > 40000 )
    m_pongTimeout = 40000;
  m_writingTimeout = qMax( commonValue( system_rc, user_ini, "WritingTimeout_ms", 3000 ).toInt(), 3000 );
  m_tickIntervalConnectionTimeout = qMax( commonValue( system_rc, user_ini, "TickIntervalConnectionTimeout", m_tickIntervalConnectionTimeout ).toInt(), 5 );
  if( m_settingsVersion < 6 && m_tickIntervalConnectionTimeout < TICK_INTERVAL_CONNECTION_TIMEOUT )
    m_tickIntervalConnectionTimeout = TICK_INTERVAL_CONNECTION_TIMEOUT;
  m_useLowDelayOptionOnSocket = commonValue( system_rc, user_ini, "UseLowDelayOptionOnSocket", false ).toBool();
  m_delayConnectionAtStartup = qMax( 3000, commonValue( system_rc, user_ini, "DelayConnectionAtStartup_ms", m_delayConnectionAtStartup ).toInt() );
  if( m_delayConnectionAtStartup > 50000 )
    m_delayConnectionAtStartup = 50000;
  m_delayContactUsers = qMax( 7000, commonValue( system_rc, user_ini, "DelayContactUsers_ms", DELAY_CONTACT_USERS ).toInt() );
  if( m_delayContactUsers > 30000 )
    m_delayContactUsers = 30000;
  m_sendOfflineMessagesToDefaultChat = commonValue( system_rc, user_ini, "SendOfflineMessagesToDefaultChat", false ).toBool();
  m_saveMessagesTimestamp = user_ini->value( "SaveMessagesTimestamp", QDateTime() ).toDateTime();
  if( m_saveMessagesTimestamp.isNull() )
  {
    qDebug() << "Generating new save messages timestamp";
    m_saveMessagesTimestamp = QDateTime::currentDateTime();
  }
  m_clearCacheAfterDays = qMax( -1, commonValue( system_rc, user_ini, "ClearCacheAfterDays", m_clearCacheAfterDays ).toInt() );
  m_removePartiallyDownloadedFilesAfterDays = qMax( -1, commonValue( system_rc, user_ini, "RemovePartiallyDownloadedFilesAfterDays", m_removePartiallyDownloadedFilesAfterDays ).toInt() );
  endCommonGroup( system_rc, user_ini );

  beginCommonGroup( system_rc, user_ini, "Network");
  QString local_host_address = user_ini->value( "LocalHostAddressForced", "" ).toString();
  if( !local_host_address.isEmpty() )
    m_localHostAddressForced = QHostAddress( local_host_address );
  m_networkAddressList = commonValue( system_rc, user_ini, "UserPathList", QStringList() ).toStringList();
  m_localSubnetForced = commonValue( system_rc, user_ini, "LocalSubnetForced", "" ).toString();
  m_localUser.setWorkgroups( commonValue( system_rc, user_ini, "Workgroups", QStringList() ).toStringList() );
  m_acceptConnectionsOnlyFromWorkgroups = commonValue( system_rc, user_ini, "AcceptConnectionsOnlyFromWorkgroups", m_acceptConnectionsOnlyFromWorkgroups ).toBool();

#ifdef BEEBEEP_USE_MULTICAST_DNS
  m_useMulticastDns = commonValue( system_rc, user_ini, "UseMulticastDns", m_useMulticastDns ).toBool();
#endif
  m_maxUsersToConnectInATick = commonValue( system_rc, user_ini, "MaxUsersToConnectInATick", m_maxUsersToConnectInATick ).toInt();
  m_preventMultipleConnectionsFromSingleHostAddress = commonValue( system_rc, user_ini, "PreventMultipleConnectionsFromSingleHostAddress", m_preventMultipleConnectionsFromSingleHostAddress ).toBool();
  m_useHive = commonValue( system_rc, user_ini, "UseHiveProtocol", m_useHive ).toBool();
  m_disableSystemProxyForConnections = commonValue( system_rc, user_ini, "DisableSystemProxyForConnections", m_disableSystemProxyForConnections ).toBool();
  m_useDefaultMulticastGroupAddress = commonValue( system_rc, user_ini, "UseDefaultMulticastGroupAddress", m_useDefaultMulticastGroupAddress ).toBool();
  m_broadcastToOfflineUsers = commonValue( system_rc, user_ini, "BroadcastToOfflineUsers", m_broadcastToOfflineUsers ).toBool();
  m_broadcastToLocalSubnetAlways = commonValue( system_rc, user_ini, "BroadcastToLocalSubnet", m_broadcastToLocalSubnetAlways ).toBool();
  m_ipMulticastTtl = commonValue( system_rc, user_ini, "IpMulticastTtl", m_ipMulticastTtl ).toInt();
  endCommonGroup( system_rc, user_ini );

  beginCommonGroup( system_rc, user_ini, "FileShare" );
  if( m_disableFileTransfer )
    m_enableFileTransfer = false;
  else
    m_enableFileTransfer = user_ini->value( "EnableFileTransfer", true ).toBool();

  if( m_enableFileTransfer )
  {
    if( m_disableFileSharing )
      m_enableFileSharing = false;
    else
      m_enableFileSharing = user_ini->value( "EnableFileSharing", false ).toBool();

    if( m_enableFileSharing )
      m_useShareBox = user_ini->value( "UseShareBox", false ).toBool();
    else
      m_useShareBox = false;
  }
  else
  {
    m_enableFileSharing = false;
    m_useShareBox = false;
  }

  m_maxFileShared = qMax( 1024, commonValue( system_rc, user_ini, "MaxSharedFiles", 8192 ).toInt() );
  m_shareBoxPath = checkFolderPath( user_ini->value( "ShareBoxPath", "" ).toString(), "", false );
  m_maxSimultaneousDownloads = commonValue( system_rc, user_ini, "MaxSimultaneousDownloads", 3 ).toInt();
  m_maxQueuedDownloads = qMax( 1, commonValue( system_rc, user_ini, "MaxQueuedDownloads", 400 ).toInt() );
  m_fileTransferConfirmTimeout = qMax( commonValue( system_rc, user_ini, "FileTransferConfirmTimeout", 30000 ).toInt(), 1000 );
  m_fileTransferBufferSize = qMax( commonValue( system_rc, user_ini, "FileTransferBufferSize", 65456 ).toInt(), 2048 );
  int mod_buffer_size = m_fileTransferBufferSize % ENCRYPTED_DATA_BLOCK_SIZE; // For a corrected encryption
  if( mod_buffer_size > 0 )
    m_fileTransferBufferSize -= mod_buffer_size;
  if( m_fileTransferBufferSize < 2048 )
    m_fileTransferBufferSize = 2048;
  bool automatic_file_name = commonValue( system_rc, user_ini, "SetAutomaticFileNameOnSave", false ).toBool();
  if( automatic_file_name )
    m_onExistingFileAction = GenerateNewFileName;
  bool overwrite_existing_files = commonValue( system_rc, user_ini, "OverwriteExistingFiles", false ).toBool();
  if( overwrite_existing_files )
    m_onExistingFileAction = OverwriteExistingFile;
  m_onExistingFileAction = commonValue( system_rc, user_ini, "OnExistingFileAction", (int)m_onExistingFileAction ).toInt();
  if( m_onExistingFileAction < 0 || m_onExistingFileAction >= NumOnExistingFileActionTypes )
    m_onExistingFileAction = OverwriteOlderExistingFile;
  m_resumeFileTransfer = commonValue( system_rc, user_ini, "ResumeFileTransfer", m_resumeFileTransfer ).toBool();
  m_confirmOnDownloadFile = commonValue( system_rc, user_ini, "ConfirmOnDownloadFile", m_confirmOnDownloadFile ).toBool();
  m_downloadInUserFolder = commonValue( system_rc, user_ini, "DownloadInUserFolder", false ).toBool();
  m_keepModificationDateOnFileTransferred = commonValue( system_rc, user_ini, "KeepModificationDateOnFileTransferred", m_keepModificationDateOnFileTransferred ).toBool();
  QStringList local_share = user_ini->value( "ShareList", QStringList() ).toStringList();
  if( !local_share.isEmpty() )
  {
    foreach( QString share_path, local_share )
      m_localShare.append( Bee::convertToNativeFolderSeparator( share_path ) );
  }
  else
    m_localShare = local_share;
  endCommonGroup( system_rc, user_ini );

  beginCommonGroup( system_rc, user_ini, "Group" );
  m_saveGroupList = commonValue( system_rc, user_ini, "SaveGroups", m_saveGroupList ).toBool();
  m_groupSilenced = user_ini->value( "Silenced", QStringList() ).toStringList();
  m_groupList = user_ini->value( "List", QStringList() ).toStringList();
  endCommonGroup( system_rc, user_ini );

  beginCommonGroup( system_rc, user_ini, "ShareDesktop" );
  if( m_disableDesktopSharing )
    m_enableShareDesktop = false;
  else
    m_enableShareDesktop = commonValue( system_rc, user_ini, "Enable", m_enableShareDesktop ).toBool();
  m_shareDesktopCaptureDelay = qMax( 1000, commonValue( system_rc, user_ini, "CaptureScreenInterval", m_shareDesktopCaptureDelay ).toInt() );
  m_shareDesktopFitToScreen = commonValue( system_rc, user_ini, "FitToScreen", false ).toBool();
  m_shareDesktopImageType = commonValue( system_rc, user_ini, "ImageType", "png" ).toString();
  m_shareDesktopImageQuality = commonValue( system_rc, user_ini, "ImageQuality", 20 ).toInt();
  endCommonGroup( system_rc, user_ini );

  beginCommonGroup( system_rc, user_ini, "VoiceMessage" );
  m_voiceMessageMaxDuration = qMax( 5, commonValue( system_rc, user_ini, "MaxDuration", m_voiceMessageMaxDuration ).toInt() );
  m_useVoicePlayer = commonValue( system_rc, user_ini, "UseVoicePlayer", m_useVoicePlayer ).toBool();
  m_voiceInputDeviceName = user_ini->value( "VoiceInputDeviceName", QString() ).toString();
  m_voiceFileMessageContainer = commonValue( system_rc, user_ini, "VoiceFileMessageContainer", QString() ).toString();
  m_voiceCodec = commonValue( system_rc, user_ini, "VoiceCodec", QString() ).toString();
  m_voiceSampleRate = commonValue( system_rc, user_ini, "VoiceSampleRate", 0 ).toInt();
  m_voiceBitRate = commonValue( system_rc, user_ini, "VoiceBitRate", 0 ).toInt();
  m_voiceChannels = commonValue( system_rc, user_ini, "VoiceChannels", -1 ).toInt();
  m_voiceEncodingMode = commonValue( system_rc, user_ini, "VoiceEncodingMode", -1 ).toInt();
  m_voiceEncodingQuality = commonValue( system_rc, user_ini, "VoiceEncodingQuality", -1 ).toInt();
  m_useCustomVoiceEncoderSettings = commonValue( system_rc, user_ini, "UseCustomVoiceEncoderSettings", m_useCustomVoiceEncoderSettings ).toBool();
  m_useSystemVoiceEncoderSettings = commonValue( system_rc, user_ini, "UseSystemVoiceEncoderSettings", m_useSystemVoiceEncoderSettings ).toBool();
  endCommonGroup( system_rc, user_ini );
  delete system_rc;
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

  beeApp->setCheckSettingsFilePath( false );
  QSettings *sets = objectSettings();
  sets->clear();

  sets->beginGroup( "Version" );
  sets->setValue( "Program", version( true, false, true ) );
  sets->setValue( "Proto", protocolVersion() );
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
  sets->setValue( "DisableBeepInUserStatusBusy", m_disableBeepInUserStatusBusy );
  sets->setValue( "EnableBeepInActiveWindow", m_beepInActiveWindowAlso );
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
  sets->setValue( "QuoteBackgroundColor", m_chatQuoteBackgroundColor );
  sets->setValue( "QuoteTextColor", m_chatQuoteTextColor );
  sets->setValue( "CloseOnSendingMessage", m_chatOnSendingMessage );
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
  sets->setValue( "CloseMinimizeInTray", m_closeMinimizeInTray );
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
  sets->setValue( "TickIntervalChatAutoSave", m_tickIntervalChatAutoSave );
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
  sets->setValue( "FavoriteEmoticons", m_favoriteEmoticons );
  sets->setValue( "RecentEmoticons", m_recentEmoticons );
  sets->setValue( "UseFontEmoticons", m_useFontEmoticons );
  sets->setValue( "UseHighResolutionEmoticons", m_useHiResEmoticons );
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
  sets->setValue( "ShowUsersInWorkgroups", m_showUsersInWorkgroups );
  sets->setValue( "OpenChatWhenSendNewMessage", m_openChatWhenSendNewMessage );
  sets->setValue( "SendNewMessageIndividually", m_sendNewMessageIndividually );
  sets->setValue( "ShowUserFirstNameFirstInFullName", m_useUserFirstNameFirstInFullName );
  sets->setValue( "ResetMinimumWidthForStyle", m_resetMinimumWidthForStyle );
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
  sets->setValue( "ConnectionActivityTimeout_ms", m_pongTimeout );
  sets->setValue( "WritingTimeout_ms", m_writingTimeout );
  sets->setValue( "DelayContactUsers_ms", m_delayContactUsers );
  sets->setValue( "TickIntervalConnectionTimeout", m_tickIntervalConnectionTimeout );
  sets->setValue( "UseLowDelayOptionOnSocket", m_useLowDelayOptionOnSocket );
  sets->setValue( "TickIntervalBroadcasting", m_tickIntervalBroadcasting );
  sets->setValue( "DelayConnectionAtStartup_ms", m_delayConnectionAtStartup );
  sets->setValue( "SendOfflineMessagesToDefaultChat", m_sendOfflineMessagesToDefaultChat );
  sets->setValue( "SaveMessagesTimestamp", m_saveMessagesTimestamp );
  sets->setValue( "ClearCacheAfterDays", m_clearCacheAfterDays );
  sets->setValue( "RemovePartiallyDownloadedFilesAfterDays", m_removePartiallyDownloadedFilesAfterDays );
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
  sets->setValue( "Workgroups", m_localUser.workgroups() );
  sets->setValue( "MaxUsersToConnectInATick", m_maxUsersToConnectInATick );
  sets->setValue( "UseHiveProtocol", m_useHive );
  sets->setValue( "DisableSystemProxyForConnections", m_disableSystemProxyForConnections );
  sets->setValue( "UseDefaultMulticastGroupAddress", m_useDefaultMulticastGroupAddress );
  sets->setValue( "BroadcastToOfflineUsers", m_broadcastToOfflineUsers );
  sets->setValue( "BroadcastToLocalSubnet", m_broadcastToLocalSubnetAlways );
  sets->setValue( "IpMulticastTtl", m_ipMulticastTtl );
  sets->endGroup();
  sets->beginGroup( "FileShare" );
  sets->setValue( "EnableFileTransfer", m_enableFileTransfer );
  sets->setValue( "EnableFileSharing", m_enableFileSharing );
  sets->setValue( "UseShareBox", m_useShareBox );
  sets->setValue( "MaxSharedFiles", m_maxFileShared );
  sets->setValue( "ShareBoxPath", m_shareBoxPath );
  sets->setValue( "OnExistingFileAction", m_onExistingFileAction );
  sets->setValue( "ResumeFileTransfer", m_resumeFileTransfer );
  sets->setValue( "FileTransferConfirmTimeout", m_fileTransferConfirmTimeout );
  sets->setValue( "FileTransferBufferSize", m_fileTransferBufferSize );
  sets->setValue( "MaxSimultaneousDownloads", m_maxSimultaneousDownloads );
  sets->setValue( "MaxQueuedDownloads", m_maxQueuedDownloads );
  sets->setValue( "ConfirmOnDownloadFile", m_confirmOnDownloadFile );
  sets->setValue( "ShareList", m_localShare );
  sets->setValue( "DownloadInUserFolder", m_downloadInUserFolder );
  sets->setValue( "KeepModificationDateOnFileTransferred", m_keepModificationDateOnFileTransferred );
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

  sets->beginGroup( "VoiceMessage" );
  sets->setValue( "MaxDuration", m_voiceMessageMaxDuration );
  sets->setValue( "UseVoicePlayer", m_useVoicePlayer );
  sets->setValue( "VoiceInputDeviceName", m_voiceInputDeviceName );
  sets->setValue( "VoiceFileMessageContainer", m_voiceFileMessageContainer );
  sets->setValue( "VoiceCodec", m_voiceCodec );
  sets->setValue( "VoiceSampleRate", m_voiceSampleRate );
  sets->setValue( "VoiceBitRate", m_voiceBitRate );
  sets->setValue( "VoiceChannels", m_voiceChannels );
  sets->setValue( "VoiceEncodingMode", m_voiceEncodingMode );
  sets->setValue( "VoiceEncodingQuality", m_voiceEncodingQuality );
  sets->setValue( "UseCustomVoiceEncoderSettings", m_useCustomVoiceEncoderSettings );
  sets->setValue( "UseSystemVoiceEncoderSettings", m_useSystemVoiceEncoderSettings );
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

#ifdef Q_OS_WIN
  if( m_useSettingsFileIni )
    m_currentFilePath = Bee::convertToNativeFolderSeparator( sets->fileName() );
  else
    m_currentFilePath = ""; // registry path
#else
  m_currentFilePath = Bee::convertToNativeFolderSeparator( sets->fileName() );
#endif

  beeApp->setSettingsFilePath( m_currentFilePath );
  beeApp->setCheckSettingsFilePath( true );
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
  if( file_path.isEmpty() )
    return;
  if( !m_tempFilePathList.contains( file_path ) )
  {
    if( QFile::exists( file_path ) )
    {
      m_tempFilePathList.append( file_path );
      qDebug() << "Temporary file added:" << qPrintable( file_path );
    }
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

QString Settings::partiallyDownloadedFileExtension() const
{
#ifdef Q_OS_OS2
  return QString( "par" );
#else
  return QString( "part" );
#endif
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

bool Settings::searchDataFolder()
{
  qDebug() << "Searching data folder...";
  QString data_folder = m_addAccountNameToDataFolder ? Bee::removeInvalidCharactersForFilePath( accountNameFromSystemEnvinroment() ) : QLatin1String( "beebeep-data" );
  QString root_folder;
#ifdef Q_OS_MAC
  bool rc_folder_is_writable = false;
#else
  bool rc_folder_is_writable = Bee::folderIsWriteable( m_resourceFolder, false );
  #ifdef Q_OS_WIN
    QProcessEnvironment pe = QProcessEnvironment::systemEnvironment();
    QString env_program_files = pe.value( "PROGRAMFILES", QLatin1String( ":\\Program File" ) );
    if( !env_program_files.isEmpty() )
      qDebug() << "Checking if BeeBEEP is installed in system folder:" << qPrintable( env_program_files ) << "...";
    if( m_resourceFolder.contains( "Program Files", Qt::CaseInsensitive ) || m_resourceFolder.contains( env_program_files, Qt::CaseInsensitive ) )
    {
      qDebug() << "BeeBEEP is installed in system folder:" << qPrintable( m_resourceFolder );
      qDebug() << "Resource folder is default Windows Program Files and will not be used as data folder";
      rc_folder_is_writable = false;
    }
    else
      qDebug() << "BeeBEEP is installed in custom folder:" << qPrintable( m_resourceFolder );
  #endif
#endif

#if QT_VERSION >= 0x050400
  if( !m_dataFolderInRC.isEmpty() )
    root_folder = m_dataFolderInRC;
  else if( m_saveDataInUserApplicationFolder )
    root_folder = QStandardPaths::writableLocation( QStandardPaths::AppDataLocation );
  else if( m_saveDataInDocumentsFolder )
    root_folder = QStandardPaths::writableLocation( QStandardPaths::DocumentsLocation );
  else
    root_folder = rc_folder_is_writable ? m_resourceFolder : QStandardPaths::writableLocation( QStandardPaths::AppDataLocation );
#elif QT_VERSION >= 0x050000
  if( !m_dataFolderInRC.isEmpty() )
    root_folder = m_dataFolderInRC;
  else if( m_saveDataInUserApplicationFolder )
    root_folder = QString( "%1/%2" ).arg( QStandardPaths::writableLocation( QStandardPaths::DataLocation ) ).arg( programName() );
  else if( m_saveDataInDocumentsFolder )
    root_folder = QStandardPaths::writableLocation( QStandardPaths::DocumentsLocation );
  else
    root_folder = QString( "%1/%2" ).arg( rc_folder_is_writable ? m_resourceFolder : QStandardPaths::writableLocation( QStandardPaths::DataLocation ) ).arg( programName() );
#else
  if( !m_dataFolderInRC.isEmpty() )
    root_folder = m_dataFolderInRC;
  else if( m_saveDataInUserApplicationFolder )
    root_folder = QDesktopServices::storageLocation( QDesktopServices::DataLocation );
  else if( m_saveDataInDocumentsFolder )
    root_folder = QDesktopServices::storageLocation( QDesktopServices::DocumentsLocation );
  else
    root_folder = rc_folder_is_writable ? m_resourceFolder : QDesktopServices::storageLocation( QDesktopServices::DataLocation );
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

  if( !Bee::folderIsWriteable( m_dataFolder, false ) )
  {
    qWarning() << "Data folder" << qPrintable( m_dataFolder ) << "is not writeable";
    return false;
  }
  qDebug() << "Data folder (found):" << qPrintable( m_dataFolder );
  return true;
}

bool Settings::setDataFolder()
{
  if( !rcFileExists() )
  {
    QStringList default_data_folders;
    default_data_folders.append( m_dataFolder );
    if( m_dataFolder != m_resourceFolder )
      default_data_folders.append( m_resourceFolder );
    m_dataFolder = findFileInFolders( "beebeep.ini", default_data_folders, true );
  }
  else
    m_dataFolder = "";

  if( m_dataFolder.isEmpty() )
  {
    if( !searchDataFolder() )
      qWarning() << "Unable to save data. Check your FS permissions";
  }

  if( m_addNicknameToDataFolder )
    m_dataFolder = Bee::convertToNativeFolderSeparator( QString( "%1/%2" ).arg( m_dataFolder, Bee::removeInvalidCharactersForFilePath( m_localUser.name() ) ) );
  qDebug() << "Data folder:" << qPrintable( m_dataFolder );

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

  QStringList folder_list = resourceFolders();
  if( folder_list.size() > 1 )
    qDebug() << "System files will be searched in the following folders:\n -" << qPrintable( resourceFolders().join( "\n - " ) );
  else if( folder_list.size() == 1 )
    qDebug() << "System files will be searched in the following folder:\n -" << qPrintable( folder_list.first() );
  else
    qDebug() << "System files will not be searched";

  folder_list = dataFolders();
  if( folder_list.size() > 1 )
    qDebug() << "Configuration and data files will be searched in the following folders:\n -" << qPrintable( folder_list.join( "\n - " ) );
  else if( folder_list.size() == 1 )
    qDebug() << "Configuration and data files will be searched in the following folder:\n -" << qPrintable( folder_list.first() );
  else
    qDebug() << "Configuration and data files will not be searched";
  return true;
}

QString Settings::defaultDownloadFolderPath() const
{
#if QT_VERSION >= 0x050000
  QString default_download_folder = Bee::convertToNativeFolderSeparator( QStandardPaths::writableLocation( QStandardPaths::DownloadLocation ) );
#else
  QString default_download_folder = Bee::convertToNativeFolderSeparator( QDesktopServices::storageLocation( QDesktopServices::DocumentsLocation ) );
#endif
  if( Bee::folderIsWriteable( default_download_folder, false ) )
    return default_download_folder;

  default_download_folder = Bee::convertToNativeFolderSeparator( QString( "%1/%2" ).arg( dataFolder(), QLatin1String( "download" ) ) );
  if( Bee::folderIsWriteable( default_download_folder, true ) )
    return default_download_folder;
  else
    return dataFolder();
}

QString Settings::defaultCacheFolderPath() const
{
  return Bee::convertToNativeFolderSeparator( QString( "%1/%2" ).arg( dataFolder(), QLatin1String( "cache" ) ) );
}

QString Settings::savedChatsFilePath() const
{
  return Bee::convertToNativeFolderSeparator( QString( "%1/%2" ).arg( dataFolder(), QLatin1String( "beebeep.dat" ) ) );
}

QString Settings::autoSavedChatsFilePath() const
{
  return Bee::convertToNativeFolderSeparator( QString( "%1/%2" ).arg( dataFolder(), QLatin1String( "beebeep.bak" ) ) );
}

QString Settings::unsentMessagesFilePath() const
{
  return Bee::convertToNativeFolderSeparator( QString( "%1/%2" ).arg( dataFolder(), QLatin1String( "beebeep.off" ) ) );
}

QString Settings::defaultSettingsFilePath() const
{
  return Bee::convertToNativeFolderSeparator( QString( "%1/%2" ).arg( dataFolder(), QLatin1String( "beebeep.ini" ) ) );
}

QString Settings::defaultBeepFilePath()
{
  if( m_beepDefaultFilePath.isEmpty() )
  {
    QStringList data_folders;
    data_folders.append( Bee::convertToNativeFolderSeparator( QString( "%1/%2" ).arg( dataFolder(), QLatin1String( "resources" ) ) ) );
    data_folders.append( dataFolders() );
#ifndef Q_OS_MAC
    data_folders.append( Bee::convertToNativeFolderSeparator( QString( "%1/%2" ).arg( resourceFolder(), QLatin1String( "resources" ) ) ) );
#endif
    data_folders.append( resourceFolders() );
    data_folders.removeDuplicates();
    QString beep_file_path = findFileInFolders( QLatin1String( "beep.wav" ), data_folders );
    if( beep_file_path.isNull() )
      m_beepDefaultFilePath = Bee::convertToNativeFolderSeparator( QString( "%1/%2" ).arg( resourceFolder(), QLatin1String( "beep.wav" ) ) );
    else
      m_beepDefaultFilePath = beep_file_path;
  }
  return m_beepDefaultFilePath;
}

QString Settings::defaultPluginFolderPath() const
{
  QStringList data_folders;
  data_folders.append( Bee::convertToNativeFolderSeparator( QString( "%1/%2" ).arg( dataFolder(), QLatin1String( "plugins" ) ) ) );
  data_folders.append( dataFolders() );
#ifndef Q_OS_MAC
  data_folders.append( Bee::convertToNativeFolderSeparator( QString( "%1/%2" ).arg( resourceFolder(), QLatin1String( "plugins" ) ) ) );
#endif
  data_folders.append( resourceFolders() );
  data_folders.removeDuplicates();
  QString test_plugin_file = QLatin1String( "libnumbertextmarker." ) + Bee::pluginFileExtension();
  QString test_plugin_path = findFileInFolders( test_plugin_file, data_folders, true );
  return test_plugin_path.isNull() ? resourceFolder() : test_plugin_path;
}

QString Settings::defaultLanguageFolderPath() const
{
  QStringList data_folders;
  data_folders.append( Bee::convertToNativeFolderSeparator( QString( "%1/%2" ).arg( dataFolder(), QLatin1String( "languages" ) ) ) );
  data_folders.append( dataFolders() );
#ifndef Q_OS_MAC
  data_folders.append( Bee::convertToNativeFolderSeparator( QString( "%1/%2" ).arg( resourceFolder(), QLatin1String( "languages" ) ) ) );
#endif
  data_folders.append( resourceFolders() );
  data_folders.removeDuplicates();
  QString test_language_file = QLatin1String( "beebeep_it.qm" );
  QString test_language_path = findFileInFolders( test_language_file, data_folders, true );
  return test_language_path.isNull() ? resourceFolder() : test_language_path;
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
                  "}" ).arg( background_color, background_image_path );
}

QString Settings::autoresponderName() const
{
  return QT_TRANSLATE_NOOP( "Settings", "Autoresponder" );
}

int Settings::defaultChatMessagesToShow() const
{
#if QT_VERSION >= 0x050000
  return 800;
#else
  return 400;
#endif
}

QString Settings::downloadDirectoryForUser( const User& u ) const
{
  if( Settings::instance().downloadInUserFolder() )
  {
    QString user_name = Bee::removeInvalidCharactersForFilePath( u.name() );
    return Bee::convertToNativeFolderSeparator( QString( "%1/%2" ).arg( downloadDirectory(), user_name ) );
  }
  else
    return downloadDirectory();
}

bool Settings::useUserFirstNameFirstInFullNameFromLanguage() const
{
  if( m_language == "ja" ) // japanese
    return false;
  else
    return true;
}

bool Settings::isFileExtensionAllowedInFileTransfer( const QString& file_ext ) const
{
  if( m_allowedFileExtensionsInFileTransfer.isEmpty() )
    return true;
  if( file_ext.toLower() == partiallyDownloadedFileExtension().toLower() )
    return true;
  return file_ext.isEmpty() ? false : m_allowedFileExtensionsInFileTransfer.contains( file_ext, Qt::CaseInsensitive );
}
