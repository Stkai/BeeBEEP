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
const QString SETTINGS_FILE_NAME = "beebeep.ini";
const QSettings::Format SETTINGS_FILE_FORMAT = QSettings::IniFormat;


Settings::Settings()
 : m_localUser( ID_LOCAL_USER )
{
  m_localUser.setStatus( User::Online );
  setPassword( defaultPassword() );
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

QByteArray Settings::hash( const QString& string_to_hash ) const
{
  QByteArray hash_pre = string_to_hash.toUtf8() + m_password;
  QByteArray hash_generated = QCryptographicHash::hash( hash_pre, QCryptographicHash::Sha1 );
  return hash_generated.toHex();
}

void Settings::setPassword( const QString& new_value )
{
  m_password = QCryptographicHash::hash( QString( (new_value.isEmpty() || new_value == defaultPassword()) ? "*6475*" : new_value ).toUtf8(), QCryptographicHash::Sha1 ).toHex();
  m_hash = QString::fromUtf8( hash( m_localUser.name() ) );
}

void Settings::setLocalUserHost( const QHostAddress& host_address, int host_port )
{
  if( host_address.toString() == QString( "0.0.0.0" ) )
    m_localUser.setHostAddress( QHostAddress( "127.0.0.1") );
  else
    m_localUser.setHostAddress( host_address );
  m_localUser.setHostPort( host_port );
}

void Settings::setNetworkAccount( const NetworkAccount& na )
{
  if( !na.isValid() )
    return;
  QList<NetworkAccount>::iterator it = m_networkAccounts.begin();
  while( it != m_networkAccounts.end() )
  {
    if( (*it).service() == na.service() )
    {
      (*it) = na;
      qDebug() << "New network account saved for service" << na.service();
      return;
    }
    ++it;
  }
  qDebug() << "New network account added for service" << na.service();
  m_networkAccounts.append( na );
}

NetworkAccount Settings::networkAccount( const QString& account_service ) const
{
  QList<NetworkAccount>::const_iterator it = m_networkAccounts.begin();
  while( it != m_networkAccounts.end() )
  {
    if( (*it).service() == account_service )
      return *it;
    ++it;
  }
  return NetworkAccount();
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
      sTmp = QString( "Bee%1" ).arg( QTime::currentTime().toString( "ss" ) );
    return sTmp;
  }
}

void Settings::load()
{
  qDebug() << "Creating local user and loading settings";
  QSettings sets( SETTINGS_FILE_NAME, SETTINGS_FILE_FORMAT );

  sets.beginGroup( "Chat" );
  m_chatFont.fromString( sets.value( "Font", QApplication::font().toString() ).toString() );
  m_chatFontColor = sets.value( "FontColor", QColor( Qt::black ).name() ).toString();
  m_chatCompact = sets.value( "CompactMessage", true ).toBool();
  m_chatAddNewLineToMessage = sets.value( "AddNewLineAfterMessage", false ).toBool();
  m_chatShowMessageTimestamp = sets.value( "ShowMessageTimestamp", false ).toBool();
  m_chatSaveDirectory = sets.value( "SaveDirectory", "." ).toString();
  m_beepOnNewMessageArrived = sets.value( "BeepOnNewMessageArrived", true ).toBool();
  m_chatUseHtmlTags = sets.value( "UseHtmlTags", false ).toBool();
  m_chatUseClickableLinks = sets.value( "UseClickableLinks", true ).toBool();
  m_chatMessageHistorySize = sets.value( "MessageHistorySize", 10 ).toInt();
  sets.endGroup();

  sets.beginGroup( "User" );
  m_localUser.setName( sets.value( "LocalName", "" ).toString() ); // For Backward compatibility (FIXME!!!)
  m_localUser.setColor( sets.value( "LocalColor", "#000000" ).toString() );
  m_localUser.setStatus( sets.value( "LocalLastStatus", m_localUser.status() ).toInt() );
  m_localUser.setStatusDescription( sets.value( "LocalLastStatusDescription", m_localUser.statusDescription() ).toString() );
  m_showOnlyOnlineUsers = sets.value( "ShowOnlyOnlineUsers", true ).toBool();
  m_showUserColor = sets.value( "ShowUserNameColor", true ).toBool();
  sets.endGroup();

  sets.beginGroup( "VCard" );
  VCard vc;
  vc.setNickName( sets.value( "NickName", m_localUser.name() ).toString() );
  vc.setFirstName( sets.value( "FirstName", "" ).toString() );
  vc.setLastName( sets.value( "LastName", "" ).toString() );
  QDate dt = sets.value( "Birthday", QDate() ).toDate();
  if( dt.isValid() )
    vc.setBirthday( dt );
  vc.setEmail( sets.value( "Email", "" ).toString() );
  QPixmap pix = sets.value( "Photo", QPixmap() ).value<QPixmap>();
  if( !pix.isNull() )
    vc.setPhoto( pix );
  m_localUser.setVCard( vc );
  sets.endGroup();

  sets.beginGroup( "Gui" );
  m_guiGeometry = sets.value( "MainWindowGeometry", "" ).toByteArray();
  m_guiState = sets.value( "MainWindowState", "" ).toByteArray();
  m_mainBarIconSize = sets.value( "MainBarIconSize", QSize( 24, 24 ) ).toSize();
  m_language = sets.value( "Language", QLocale::system().name() ).toString();
  if( m_language.size() > 2 )
    m_language.resize( 2 );
  m_lastDirectorySelected = sets.value( "LastDirectorySelected", QDesktopServices::storageLocation( QDesktopServices::DocumentsLocation ) ).toString();
  m_downloadDirectory = sets.value( "DownloadDirectory",QDesktopServices::storageLocation( QDesktopServices::DocumentsLocation ) ).toString();
  m_logPath = sets.value( "LogPath", "." ).toString();
  m_pluginPath = sets.value( "PluginPath", "." ).toString();
  m_localePath = sets.value( "LocalePath", "." ).toString();
  sets.endGroup();

  sets.beginGroup( "Tools" );
#if defined( BEEBEEP_DEBUG )
  m_debugMode = sets.value( "DebugMode", true ).toBool();
#else
  m_debugMode = sets.value( "DebugMode", false ).toBool();
#endif
  m_showTipsOfTheDay = sets.value( "ShowTipsOfTheDay", true ).toBool();
  m_automaticFileName = sets.value( "AutomaticFileName", true ).toBool();
  sets.endGroup();

  sets.beginGroup( "Misc" );
  m_broadcastPort = sets.value( "BroadcastPort", 36475 ).toInt();
  m_broadcastInterval = qMax( sets.value( "BroadcastInterval", 35000 ).toInt(), 1000 );
  m_localUser.setHostPort( sets.value( "ListenerPort", 6475 ).toInt() );
  m_pingInterval = qMax( sets.value( "PingInterval", 31000 ).toInt(), 1000 );
  m_pongTimeout = qMax( sets.value( "PongTimeout", 98000 ).toInt(), 1000 );
  m_writingTimeout = qMax( sets.value( "WritingTimeout", 3000 ).toInt(), 1000 );
  m_fileTransferConfirmTimeout = qMax( sets.value( "FileTransferConfirmTimeout", 30000 ).toInt(), 1000 );
  m_fileTransferBufferSize = qMax( sets.value( "FileTransferBufferSize", 65456 ).toInt(), 2048 );
  int mod_buffer_size = m_fileTransferBufferSize % ENCRYPTED_DATA_BLOCK_SIZE; // For a corrected encryption
  if( mod_buffer_size > 0 )
    m_fileTransferBufferSize -= mod_buffer_size;
  sets.endGroup();

  sets.beginGroup( "Network");
  m_networkProxy.setType( (QNetworkProxy::ProxyType)sets.value( "ProxyType", QNetworkProxy::DefaultProxy ).toInt() );
  QString proxy_address = sets.value( "ProxyAddress", "" ).toString();
  if( proxy_address.isEmpty() )
    m_networkProxy.setType( QNetworkProxy::DefaultProxy );
  else
    m_networkProxy.setHostName( proxy_address );
  m_networkProxy.setPort( sets.value( "ProxyPort", 0 ).toInt() );
  m_networkProxyUseAuthentication = sets.value( "ProxyUseAuthentication", false ).toBool();
  m_networkProxy.setUser( sets.value( "ProxyUser", "" ).toString() );
  m_networkProxy.setPassword( Protocol::simpleDecrypt( sets.value( "ProxyPassword", "" ).toString() ) );
  sets.endGroup();

  qDebug() << "Loading network accounts";
  sets.beginGroup( "NetworkAccount" );
  int account_number = sets.value( "Accounts", 0 ).toInt();
  qDebug() << account_number << "accounts found";
  if( account_number > 0 )
  {
    int account_index = 1;
    QString account_data = "";
    while( account_index <= account_number )
    {
      account_data = sets.value( QString( "Account%1" ).arg( account_index ), QString( "" ) ).toString();
      if( account_data.size() > 0 )
      {
        NetworkAccount na;
        if( na.fromString( account_data ) )
        {
          setNetworkAccount( na );
          qDebug() << "Account loaded for service" << na.service();
        }
        else
          qWarning() << "Error occurred when loading account data";
      }
      account_index++;
    }
  }
  sets.endGroup();

  if( m_localUser.name().isEmpty() )
  {
    QString sName = GetUserNameFromSystemEnvinroment();
    m_localUser.setName( sName );
  }

  if( m_localUser.bareJid().isEmpty() )
    m_localUser.setBareJid( QString( "%1@localhost" ).arg( m_localUser.name() ) );

  qDebug() << "Local user:" << m_localUser.path();
}

void Settings::save()
{
  qDebug() << "Saving settings";
  QSettings sets( SETTINGS_FILE_NAME, SETTINGS_FILE_FORMAT );
  sets.beginGroup( "Version" );
  sets.setValue( "Program", version( true ) );
  sets.setValue( "Proto", protoVersion() );
  sets.endGroup();
  sets.beginGroup( "Chat" );
  sets.setValue( "Font", m_chatFont.toString() );
  sets.setValue( "FontColor", m_chatFontColor );
  sets.setValue( "CompactMessage", m_chatCompact );
  sets.setValue( "AddNewLineAfterMessage", m_chatAddNewLineToMessage );
  sets.setValue( "ShowMessageTimestamp", m_chatShowMessageTimestamp );
  sets.setValue( "SaveDirectory", m_chatSaveDirectory );
  sets.setValue( "BeepOnNewMessageArrived", m_beepOnNewMessageArrived );
  sets.setValue( "UseHtmlTags", m_chatUseHtmlTags );
  sets.setValue( "UseClickableLinks", m_chatUseClickableLinks );
  sets.setValue( "MessageHistorySize", m_chatMessageHistorySize );
  sets.endGroup();
  sets.beginGroup( "User" );
  sets.setValue( "LocalColor", m_localUser.color() );
  sets.setValue( "LocalLastStatus", m_localUser.status() );
  sets.setValue( "LocalLastStatusDescription", m_localUser.statusDescription() );
  sets.setValue( "ShowOnlyOnlineUsers", m_showOnlyOnlineUsers );
  sets.setValue( "ShowUserNameColor", m_showUserColor );
  sets.endGroup();
  sets.beginGroup( "VCard" );
  sets.setValue( "NickName", m_localUser.vCard().nickName() );
  sets.setValue( "FirstName", m_localUser.vCard().firstName() );
  sets.setValue( "LastName", m_localUser.vCard().lastName() );
  sets.setValue( "Birthday", m_localUser.vCard().birthday() );
  sets.setValue( "Email", m_localUser.vCard().email() );
  sets.setValue( "Photo", m_localUser.vCard().photo() );
  sets.endGroup();
  sets.beginGroup( "Gui" );
  sets.setValue( "MainWindowGeometry", m_guiGeometry );
  sets.setValue( "MainWindowState", m_guiState );
  sets.setValue( "MainBarIconSize", m_mainBarIconSize );
  sets.setValue( "Language", m_language );
  sets.setValue( "LastDirectorySelected", m_lastDirectorySelected );
  sets.setValue( "DownloadDirectory", m_downloadDirectory );
  sets.setValue( "LogPath", m_logPath );
  sets.setValue( "PluginPath", m_pluginPath );
  sets.setValue( "LocalePath", m_localePath );
  sets.endGroup();
  sets.beginGroup( "Tools" );
  sets.setValue( "DebugMode", m_debugMode );
  sets.setValue( "ShowTipsOfTheDay", m_showTipsOfTheDay );
  sets.setValue( "AutomaticFileName", m_automaticFileName );
  sets.endGroup();
  sets.beginGroup( "Misc" );
  sets.setValue( "BroadcastPort", m_broadcastPort );
  sets.setValue( "BroadcastInterval", m_broadcastInterval );
  sets.setValue( "ListenerPort", m_localUser.hostPort() );
  sets.setValue( "PingInterval", m_pingInterval );
  sets.setValue( "PongTimeout", m_pongTimeout );
  sets.setValue( "WritingTimeout", m_writingTimeout );
  sets.setValue( "FileTransferConfirmTimeout", m_fileTransferConfirmTimeout );
  sets.setValue( "FileTransferBufferSize", m_fileTransferBufferSize );
  sets.endGroup();
  sets.beginGroup( "Network");
  sets.setValue( "ProxyType", (int)m_networkProxy.type() );
  sets.setValue( "ProxyAddress", m_networkProxy.hostName() );
  sets.setValue( "ProxyPort", m_networkProxy.port() );
  sets.setValue( "ProxyUseAuthentication", m_networkProxyUseAuthentication );
  sets.setValue( "ProxyUser", m_networkProxy.user() );
  sets.setValue( "ProxyPassword", Protocol::simpleEncrypt( m_networkProxy.password() ) );
  sets.endGroup();

  if( m_networkAccounts.size() > 0 )
  {
    sets.beginGroup( "NetworkAccount" );
    sets.setValue( "Accounts", m_networkAccounts.size() );
    int account_index = 1;
    QList<NetworkAccount>::const_iterator it = m_networkAccounts.begin();
    while( it != m_networkAccounts.end() )
    {
      sets.setValue( QString( "Account%1" ).arg( account_index ), (*it).toString() );
      ++it;
      account_index++;
    }
    sets.endGroup();
  }

  sets.sync();
  qDebug() << "Settings saved";
}
