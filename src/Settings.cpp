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

#include "Settings.h"
#include "Protocol.h"
#include "Version.h"


Settings* Settings::mp_instance = NULL;


Settings::Settings()
 : m_localUser( ID_LOCAL_USER )
{
  m_useSettingsFileIni = true;
  m_trustNickname = true;
  m_trustSystemAccount = false;
  m_confirmOnDownloadFile = true;
  m_localUser.setStatus( User::Online );
  m_localUser.setVersion( version( false ) );
  setPassword( defaultPassword() );
  m_defaultFolder = ".";
  m_lastSave = QDateTime::currentDateTime();
}

QString Settings::version( bool complete ) const
{
  return complete ? QString( "%1 (b%2p%3)" ).arg( BEEBEEP_VERSION ).arg( BEEBEEP_BUILD ).arg( BEEBEEP_PROTO_VERSION ) : QString( BEEBEEP_VERSION );
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

QString Settings::checkVersionWebSite() const
{
  QString os_type = "windows";
#ifdef Q_OS_LINUX
  os_type = "linux";
#endif
#ifdef Q_OS_MAC
  os_type = "macosx";
#endif
  return officialWebSite() + QString( "%1?beebeep-version=%2&beebeep-os=%3" ).arg( QString( BEEBEEP_CHECK_VERSION_WEBSITE ) ).arg( QString( BEEBEEP_VERSION ) ).arg( os_type );
}

QString Settings::languageFilePath( const QString& language_folder, const QString& language_selected ) const
{
  return QString( "%1/%2_%3.qm" ).arg( language_folder, Settings::instance().programName().toLower(), language_selected );
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

QHostAddress Settings::searchLocalHostAddress() const
{
  QList<QNetworkInterface> interface_list = QNetworkInterface::allInterfaces();
  QList<QHostAddress> address_ipv6_list;
  QList<QHostAddress> address_ipv4_list;
  QList<QHostAddress> address_list;

  // Collect the list
  foreach( QNetworkInterface if_net, interface_list )
  {
    if( (if_net.flags() & QNetworkInterface::IsUp) &&
         (if_net.flags() & QNetworkInterface::IsRunning) &&
         (if_net.flags() & ~QNetworkInterface::IsLoopBack) )
    {
       address_list = if_net.allAddresses();
       foreach( QHostAddress host_address, address_list )
       {
         qDebug() << "Check host address:" << host_address.toString();
         if( host_address != QHostAddress::LocalHost && host_address != QHostAddress::LocalHostIPv6 )
         {
           if( host_address.toString().contains( ":" ) )
             address_ipv6_list.append( host_address );
           else
             address_ipv4_list.append( host_address );
         }
       }
    }
  }

  // check forced ip
  if( !m_localHostAddressForced.isNull() )
  {
    if( !address_ipv4_list.isEmpty() )
    {
      foreach( QHostAddress host_address, address_ipv4_list )
      {
        if( host_address == m_localHostAddressForced )
        {
          qDebug() << "Local host address IPV4 selected:" << host_address.toString() << "(forced IP found)";
          return host_address;
        }
      }
    }

    if( !address_ipv6_list.isEmpty() )
    {
      foreach( QHostAddress host_address, address_ipv6_list )
      {
        if( host_address == m_localHostAddressForced )
        {
          qDebug() << "Local host address IPV6 selected:" << host_address.toString() << "(forced IP found)";
          return host_address;
        }
      }
    }
  }

  // check forced subnet
  if( !m_localSubnetForced.isEmpty() )
  {
    QPair<QHostAddress, int> subnet_forced = QHostAddress::parseSubnet( m_localSubnetForced );

    if( !address_ipv4_list.isEmpty() )
    {
      foreach( QHostAddress host_address, address_ipv4_list )
      {
        if( host_address.isInSubnet( subnet_forced ) )
        {
          qDebug() << "Local host address IPV4 selected:" << host_address.toString() << "(forced SUBNET found)";
          return host_address;
        }
      }
    }

    if( !address_ipv6_list.isEmpty() )
    {
      foreach( QHostAddress host_address, address_ipv6_list )
      {
        if( host_address.isInSubnet( subnet_forced ) )
        {
          qDebug() << "Local host address IPV6 selected:" << host_address.toString() << "(forced SUBNET found)";
          return host_address;
        }
      }
    }
  }

  if( !address_ipv4_list.isEmpty() )
  {
    qDebug() << "Local host address IPV4 selected:" << address_ipv4_list.first().toString();
    return address_ipv4_list.first();
  }

  if( !address_ipv6_list.isEmpty() )
  {
    qDebug() << "Local host address IPV6 selected:" << address_ipv6_list.first().toString();
    return address_ipv6_list.first();
  }

  qDebug() << "Local host address not found";
  return QHostAddress( "127.0.0.1" );
}

void Settings::setLocalUserHost( const QHostAddress& host_address, int host_port )
{
  if( host_address.toString() == QString( "0.0.0.0" ) )
    m_localUser.setHostAddress( QHostAddress( "127.0.0.1") );
  else
    m_localUser.setHostAddress( host_address );
  m_localUser.setHostPort( host_port );
}

void Settings::loadBroadcastAddresses()
{
  QFile file( defaultHostsFilePath( false ) );
  if( !file.open( QIODevice::ReadOnly ) )
  {
    qWarning() << "File hosts" << file.fileName() << "not found. Check the default one";
    file.setFileName( defaultHostsFilePath( true ) );
    if( !file.open( QIODevice::ReadOnly ) )
    {
      qWarning() << "Default file host not found";
      return;
    }
  }

  qDebug() << "Reading file hosts" << file.fileName();
  QString address_string;
  QString line_read;
  char c;

  while( !file.atEnd() )
  {
    line_read = file.readLine();
    if( line_read.size() > 0 )
    {
      address_string = line_read.simplified();

      if( address_string.size() > 0 )
      {
        c = address_string.at( 0 ).toLatin1();
        if( c == '#' || c == '/' || c == '*' )
          continue;

        if( QHostAddress( address_string ).isNull() )
        {
          qWarning() << "Invalid broadcast address found:" << address_string;
          continue;
        }

        if( m_broadcastAddresses.contains( address_string ) )
        {
          qDebug() << "Broadcast address is already in list";
          continue;
        }

        qDebug() << "Adding broadcast address:" << address_string;
        m_broadcastAddresses << address_string;
      }
    }
  }
}

namespace
{
  QString GetUserNameFromSystemEnvinroment()
  {
    qDebug() << "Checking local user system environment";
    QProcessEnvironment pe = QProcessEnvironment::systemEnvironment();
    QString sTmp = pe.value( "USERNAME" );
    if( sTmp.isNull() )
      sTmp = pe.value( "USER" );
    if( sTmp.isNull() )
      sTmp = QString( "Bee%1" ).arg( QTime::currentTime().toString( "hmszzz" ) );
    return sTmp;
  }
}

void Settings::loadPreConf()
{
  QSettings* sets = new QSettings( defaultRcFilePath( true ), QSettings::IniFormat );
  if( sets->allKeys().isEmpty() )
  {
    qDebug() << "Creating pre-configuration file";
    sets->deleteLater();
    sets = new QSettings( defaultRcFilePath( false ), QSettings::IniFormat );

    sets->beginGroup( "BeeBEEP" );
    sets->setValue( "UseConfigurationFileIni", true );
    sets->endGroup();
    sets->beginGroup( "Groups" );
    sets->setValue( "TrustNickname", true );
    sets->setValue( "TrustSystemAccount", false );
    sets->endGroup();
    sets->sync();
    qDebug() << "Pre-configuration file created in" << sets->fileName();
  }
  else
  {
    sets->beginGroup( "BeeBEEP" );
    m_useSettingsFileIni = sets->value( "UseConfigurationFileIni", true ).toBool();
    sets->endGroup();
    sets->beginGroup( "Groups" );
    m_trustNickname = sets->value( "TrustNickname", true ).toBool();
    m_trustSystemAccount = sets->value( "TrustSystemAccount", false ).toBool();
    sets->endGroup();
  }

  qDebug() << "Read initial settings from" << sets->fileName();
  sets->deleteLater();
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
  m_dataStreamVersion = sets->value( "DataStream", (int)DATASTREAM_VERSION_1 ).toInt();
  m_settingsCreationDate = sets->value( "BeeBang", QDate() ).toDate();
  if( m_settingsCreationDate.isNull() )
    m_settingsCreationDate = QDate::currentDate();
  sets->endGroup();

  sets->beginGroup( "Chat" );
  m_chatFont.fromString( sets->value( "Font", QApplication::font().toString() ).toString() );
  m_chatFontColor = sets->value( "FontColor", QColor( Qt::black ).name() ).toString();
  m_chatCompact = sets->value( "CompactMessage", true ).toBool();
  m_chatAddNewLineToMessage = sets->value( "AddNewLineAfterMessage", false ).toBool();
  m_chatShowMessageTimestamp = sets->value( "ShowMessageTimestamp", false ).toBool();
  m_beepOnNewMessageArrived = sets->value( "BeepOnNewMessageArrived", true ).toBool();
  m_chatUseHtmlTags = sets->value( "UseHtmlTags", false ).toBool();
  m_chatUseClickableLinks = sets->value( "UseClickableLinks", true ).toBool();
  m_chatMessageHistorySize = sets->value( "MessageHistorySize", 10 ).toInt();
  m_showEmoticons = sets->value( "ShowEmoticons", true ).toBool();
  m_showMessagesGroupByUser = sets->value( "ShowMessagesGroupByUsers", true ).toBool();
  m_autoLinkSavedChatByNickname = sets->value( "AutoLinkSavedChatByNickname", true ).toBool();
  sets->endGroup();

  sets->beginGroup( "User" );
  m_localUser.setName( sets->value( "LocalName", "" ).toString() ); // For Backward compatibility, if empty the name is set after
  m_localUser.setColor( sets->value( "LocalColor", "#000000" ).toString() );
  m_localUser.setStatus( sets->value( "LocalLastStatus", m_localUser.status() ).toInt() );
  m_localUser.setStatusDescription( sets->value( "LocalLastStatusDescription", m_localUser.statusDescription() ).toString() );
  m_showOnlyOnlineUsers = sets->value( "ShowOnlyOnlineUsers", true ).toBool();
  m_showUserColor = sets->value( "ShowUserNameColor", true ).toBool();
  m_autoUserAway = sets->value( "AutoAwayStatus", true ).toBool();
  m_userAwayTimeout = qMax( sets->value( "UserAwayTimeout", 10 ).toInt(), 1 ); // minutes
  m_useDefaultPassword = sets->value( "UseDefaultPassword", false ).toBool();
  m_askPasswordAtStartup = sets->value( "AskPasswordAtStartup", true ).toBool();
  m_savePassword = sets->value( "SavePassword", false ).toBool();
  if( m_savePassword )
    m_passwordBeforeHash = Protocol::instance().simpleEncryptDecrypt( sets->value( "EncPwd", "" ).toString() );
  else
    m_passwordBeforeHash = "";
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
  m_guiGeometry = sets->value( "MainWindowGeometry", "" ).toByteArray();
  m_guiState = sets->value( "MainWindowState", "" ).toByteArray();
  m_mainBarIconSize = sets->value( "MainBarIconSize", QSize( 24, 24 ) ).toSize();
  m_language = sets->value( "Language", QLocale::system().name() ).toString();
  if( m_language.size() > 2 )
    m_language.resize( 2 );
#if QT_VERSION >= 0x050000
  m_lastDirectorySelected = sets->value( "LastDirectorySelected", QStandardPaths::writableLocation( QStandardPaths::DocumentsLocation ) ).toString();
  m_downloadDirectory = sets->value( "DownloadDirectory", QStandardPaths::writableLocation( QStandardPaths::DownloadLocation ) ).toString();
#else
  m_lastDirectorySelected = sets->value( "LastDirectorySelected", QDesktopServices::storageLocation( QDesktopServices::DocumentsLocation ) ).toString();
  m_downloadDirectory = sets->value( "DownloadDirectory", QDesktopServices::storageLocation( QDesktopServices::DocumentsLocation ) ).toString();
#endif
  m_logPath = sets->value( "LogPath", defaultFolder() ).toString();
  m_pluginPath = sets->value( "PluginPath", defaultFolder() ).toString();
  m_languagePath = sets->value( "LanguagePath", defaultFolder() ).toString();
  m_minimizeInTray = sets->value( "MinimizeInTray", false ).toBool();
  m_stayOnTop = sets->value( "StayOnTop", false ).toBool();
  m_raiseOnNewMessageArrived = sets->value( "RaiseOnNewMessageArrived", false ).toBool();
  m_beepFilePath = sets->value( "BeepFilePath", defaultBeepFilePath() ).toString();
  m_loadOnTrayAtStartup = sets->value( "LoadOnTrayAtStartup", false ).toBool();
  m_showNotificationOnTray = sets->value( "ShowNotificationOnTray", true ).toBool();
  m_chatSaveDirectory = sets->value( "ChatSaveDirectory", defaultFolder() ).toString();
  m_chatAutoSave = sets->value( "ChatAutoSave", false ).toBool();
  m_chatMaxLineSaved = sets->value( "ChatMaxLineSaved", 3000 ).toInt();
  sets->endGroup();

  sets->beginGroup( "Tools" );
  m_logToFile = sets->value( "LogToFile", false ).toBool();
  m_showTipsOfTheDay = sets->value( "ShowTipsOfTheDay", true ).toBool();
  m_automaticFileName = sets->value( "AutomaticFileName", true ).toBool();
  sets->endGroup();

  sets->beginGroup( "Misc" );
  m_broadcastPort = sets->value( "BroadcastPort", 36475 ).toInt();
  m_broadcastInterval = sets->value( "BroadcastInterval", 0 ).toInt();
  m_localUser.setHostPort( sets->value( "ListenerPort", 6475 ).toInt() );
  m_pingInterval = qMax( sets->value( "PingInterval", 31000 ).toInt(), 1000 );
  m_pongTimeout = qMax( sets->value( "PongTimeout", 98000 ).toInt(), 1000 );
  m_writingTimeout = qMax( sets->value( "WritingTimeout", 3000 ).toInt(), 1000 );
  m_fileTransferConfirmTimeout = qMax( sets->value( "FileTransferConfirmTimeout", 30000 ).toInt(), 1000 );
  m_fileTransferBufferSize = qMax( sets->value( "FileTransferBufferSize", 65456 ).toInt(), 2048 );
  int mod_buffer_size = m_fileTransferBufferSize % ENCRYPTED_DATA_BLOCK_SIZE; // For a corrected encryption
  if( mod_buffer_size > 0 )
    m_fileTransferBufferSize -= mod_buffer_size;
  m_trayMessageTimeout = qMax( sets->value( "SystemTrayMessageTimeout", 2000 ).toInt(), 100 );
  sets->endGroup();

  sets->beginGroup( "Network");
   m_broadcastAddresses = sets->value( "BroadcastAddresses", QStringList() ).toStringList();
  QString local_host_address = sets->value( "LocalHostAddressForced", "" ).toString();
  if( !local_host_address.isEmpty() )
    m_localHostAddressForced = QHostAddress( local_host_address );
  m_localSubnetForced = sets->value( "LocalSubnetForced", "" ).toString();
  sets->endGroup();
  loadBroadcastAddresses();

  sets->beginGroup( "FileShare" );
  m_fileTransferIsEnabled = sets->value( "FileTransferIsEnabled", true ).toBool();
  m_maxFileShared = qMax( 0, sets->value( "MaxFileShared", MAX_NUM_FILE_SHARED ).toInt() );
  QStringList local_share = sets->value( "ShareList", QStringList() ).toStringList();
  if( !local_share.isEmpty() )
  {
    foreach( QString share_path, local_share )
      m_localShare.append( QDir::toNativeSeparators( share_path ) );
  }
  else
    m_localShare = local_share;
  sets->endGroup();

  sets->beginGroup( "Group" );
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

  QString sName = GetUserNameFromSystemEnvinroment().simplified();
  m_localUser.setAccountName( sName.toLower() );
  if( m_localUser.name().isEmpty() )
    m_localUser.setName( sName );

  qDebug() << "Local user:" << m_localUser.path();
  qDebug() << "Account name:" << m_localUser.accountName();

  m_lastSave = QDateTime::currentDateTime();

  sets->deleteLater();
}

void Settings::save()
{
  qDebug() << "Saving settings";
  QSettings *sets = objectSettings();

  sets->beginGroup( "Version" );
  sets->setValue( "Program", version( true ) );
  sets->setValue( "Proto", protoVersion() );
  sets->setValue( "Settings", BEEBEEP_SETTINGS_VERSION );
  sets->setValue( "DataStream", (int)dataStreamVersion( false ) );
  sets->setValue( "BeeBang", m_settingsCreationDate );
  sets->endGroup();
  sets->beginGroup( "Chat" );
  sets->setValue( "Font", m_chatFont.toString() );
  sets->setValue( "FontColor", m_chatFontColor );
  sets->setValue( "CompactMessage", m_chatCompact );
  sets->setValue( "AddNewLineAfterMessage", m_chatAddNewLineToMessage );
  sets->setValue( "ShowMessageTimestamp", m_chatShowMessageTimestamp );
  sets->setValue( "BeepOnNewMessageArrived", m_beepOnNewMessageArrived );
  sets->setValue( "UseHtmlTags", m_chatUseHtmlTags );
  sets->setValue( "UseClickableLinks", m_chatUseClickableLinks );
  sets->setValue( "MessageHistorySize", m_chatMessageHistorySize );
  sets->setValue( "ShowEmoticons", m_showEmoticons );
  sets->setValue( "ShowMessagesGroupByUsers", m_showMessagesGroupByUser );
  sets->setValue( "AutoLinkSavedChatByNickname", m_autoLinkSavedChatByNickname );
  sets->endGroup();
  sets->beginGroup( "User" );
  sets->setValue( "LocalColor", m_localUser.color() );
  sets->setValue( "LocalLastStatus", (int)(m_localUser.status() == User::Offline ? User::Online : m_localUser.status()) );
  sets->setValue( "LocalLastStatusDescription", m_localUser.statusDescription() );
  sets->setValue( "ShowOnlyOnlineUsers", m_showOnlyOnlineUsers );
  sets->setValue( "ShowUserNameColor", m_showUserColor );
  sets->setValue( "AutoAwayStatus", m_autoUserAway );
  sets->setValue( "UserAwayTimeout", m_userAwayTimeout ); // minutes
  sets->setValue( "UseDefaultPassword", m_useDefaultPassword );
  sets->setValue( "AskPasswordAtStartup", m_askPasswordAtStartup );
  if( m_savePassword )
  {
    sets->setValue( "SavePassword", true );
    sets->setValue( "EncPwd", Protocol::instance().simpleEncryptDecrypt( m_passwordBeforeHash ) );
  }
  else
  {
    sets->remove( "SavePassword" );
    sets->remove( "EncPwd" );
  }
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
  sets->setValue( "MainBarIconSize", m_mainBarIconSize );
  sets->setValue( "Language", m_language );
  sets->setValue( "LastDirectorySelected", m_lastDirectorySelected );
  sets->setValue( "DownloadDirectory", m_downloadDirectory );
  sets->setValue( "LogPath", m_logPath );
  sets->setValue( "PluginPath", m_pluginPath );
  sets->setValue( "LanguagePath", m_languagePath );
  sets->setValue( "MinimizeInTray", m_minimizeInTray );
  sets->setValue( "StayOnTop", m_stayOnTop );
  sets->setValue( "BeepFilePath", m_beepFilePath );
  sets->setValue( "RaiseOnNewMessageArrived", m_raiseOnNewMessageArrived );
  sets->setValue( "LoadOnTrayAtStartup", m_loadOnTrayAtStartup );
  sets->setValue( "ShowNotificationOnTray", m_showNotificationOnTray );
  sets->setValue( "ChatSaveDirectory", m_chatSaveDirectory );
  sets->setValue( "ChatAutoSave", m_chatAutoSave );
  sets->setValue( "ChatMaxLineSaved", m_chatMaxLineSaved );
  sets->endGroup();
  sets->beginGroup( "Tools" );
  sets->setValue( "LogToFile", m_logToFile );
  sets->setValue( "ShowTipsOfTheDay", m_showTipsOfTheDay );
  sets->setValue( "AutomaticFileName", m_automaticFileName );
  sets->endGroup();
  sets->beginGroup( "Misc" );
  sets->setValue( "BroadcastPort", m_broadcastPort );
  sets->setValue( "BroadcastInterval", m_broadcastInterval );
  sets->setValue( "ListenerPort", m_localUser.hostPort() );
  sets->setValue( "PingInterval", m_pingInterval );
  sets->setValue( "PongTimeout", m_pongTimeout );
  sets->setValue( "WritingTimeout", m_writingTimeout );
  sets->setValue( "FileTransferConfirmTimeout", m_fileTransferConfirmTimeout );
  sets->setValue( "FileTransferBufferSize", m_fileTransferBufferSize );
  sets->setValue( "SystemTrayMessageTimeout", m_trayMessageTimeout );
  sets->endGroup();
  sets->beginGroup( "Network");
  sets->setValue( "BroadcastAddresses", m_broadcastAddresses );
  if( !m_localHostAddressForced.isNull() )
    sets->setValue( "LocalHostAddressForced", m_localHostAddressForced.toString() );
  else
    sets->setValue( "LocalHostAddressForced", QString( "" ) );
  sets->setValue( "LocalSubnetForced", m_localSubnetForced );
  sets->endGroup();
  sets->beginGroup( "FileShare" );
  sets->setValue( "FileTransferIsEnabled", m_fileTransferIsEnabled );
  sets->setValue( "MaxFileShared", m_maxFileShared );
  sets->setValue( "ShareList", m_localShare );
  sets->endGroup();

  if( !m_groupList.isEmpty() )
  {
    sets->beginGroup( "Group" );
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
    qDebug() << "Settings saved" << sets->fileName();
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
    qDebug() << "Temporary file added:" << file_path;
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
        qWarning() << "Unable to remove temporary file:" << file_path;
    }
  }

  m_tempFilePathList.clear();
}

void Settings::addStartOnSystemBoot()
{
#ifdef Q_OS_WIN
  QString program_path = qApp->applicationFilePath().replace( "/", "\\" );
  QSettings sets( "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat );
  sets.setValue( programName(), program_path );
  sets.sync();
#endif
}

void Settings::removeStartOnSystemBoot()
{
#ifdef Q_OS_WIN
  QSettings sets( "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat );
  sets.remove( programName() );
  sets.sync();
#endif
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

bool Settings::createDefaultFolder()
{
  QString default_folder = QApplication::applicationDirPath();

  QFileInfo current_folder( default_folder );
  if( current_folder.isWritable() )
  {
    m_defaultFolder = default_folder;
    qDebug() << "Current folder" << current_folder.absolutePath() << "is writeable";
    return true;
  }
  else
    qWarning() << "Current" << Settings::instance().programName() << "folder is not writeable";

  QString data_folder = QLatin1String( "beebeep-data" );
  QString root_folder;

#if QT_VERSION >= 0x050000
  root_folder = QStandardPaths::writableLocation( QStandardPaths::DocumentsLocation );
#else
  root_folder = QDesktopServices::storageLocation( QDesktopServices::DocumentsLocation );
#endif
  default_folder = QDir::toNativeSeparators( QString( "%1/%2" ).arg( root_folder, data_folder ) );

  QDir folder( default_folder );
  if( !folder.exists() )
  {
    qWarning() << "Default folder root not found:" << folder.absolutePath();
    folder.cdUp();
    if( !folder.mkdir( data_folder ) )
    {
      qWarning() << "Unable to create folder" << data_folder << "in" << folder.absolutePath() ;
      m_defaultFolder = root_folder;
      return false;
    }

    qDebug() << "Folder" << data_folder << "created in" << root_folder;
  }

  QFileInfo folder_info( default_folder );
  if( !folder_info.isWritable() )
  {
    qWarning() << "Default folder" << folder_info.absoluteFilePath() << "is not writeable";
    m_defaultFolder = root_folder;
    return false;
  }

  qDebug() << "Default folder:" << default_folder;
  m_defaultFolder = default_folder;
  return true;
}

QString Settings::defaultHostsFilePath( bool current_dir ) const
{
  return current_dir ? QLatin1String( "beehosts.ini" ) : QDir::toNativeSeparators( QString( "%1/%2" ).arg( defaultFolder() ).arg( QLatin1String( "beehosts.ini" ) ) );
}

QString Settings::defaultRcFilePath( bool current_dir ) const
{
  return current_dir ? QLatin1String( "beebeep.rc" ) : QDir::toNativeSeparators( QString( "%1/%2" ).arg( defaultFolder() ).arg( QLatin1String( "beebeep.rc" ) ) );
}

QString Settings::defaultSettingsFilePath() const
{
  return QDir::toNativeSeparators( QString( "%1/%2" ).arg( defaultFolder() ).arg( QLatin1String( "beebeep.ini" ) ) );
}

QString Settings::defaultBeepFilePath() const
{
 return QDir::toNativeSeparators( QString( "%1/%2" ).arg( defaultFolder() ).arg( QLatin1String( "beep.wav" ) ) );
}
