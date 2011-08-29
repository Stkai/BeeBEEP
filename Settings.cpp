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

QString Settings::version() const
{
  return QString( BEEBEEP_VERSION );
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
  m_localUser.setHostAddress( host_address );
  m_localUser.setHostPort( host_port );
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
  sets.endGroup();
  sets.beginGroup( "User" );
  m_localUser.setName( sets.value( "LocalName", "" ).toString() );
  m_localUser.setStatus( sets.value( "LocalLastStatus", m_localUser.status() ).toInt() );
  m_localUser.setStatusDescription( sets.value( "LocalLastStatusDescription", m_localUser.statusDescription() ).toString() );
  m_localUser.setHostPort( sets.value( "LocalListenerPort", LISTENER_DEFAULT_PORT ).toInt() );
  m_broadcastPort = sets.value( "LocalBroadcastPort", BROADCAST_DEFAULT_PORT ).toInt();
  m_showOnlyUsername = sets.value( "ShowOnlyName", true ).toBool();
  m_showUserColor = sets.value( "ShowColors", true ).toBool();
  sets.endGroup();
  sets.beginGroup( "Gui" );
  m_guiGeometry = sets.value( "MainWindowGeometry", "" ).toByteArray();
  m_guiState = sets.value( "MainWindowState", "" ).toByteArray();
  m_showMenuBar = sets.value( "ShowMenuBar", true ).toBool();
  m_mainBarIconSize = sets.value( "MainBarIconSize", QSize( 24, 24 ) ).toSize();
  m_language = sets.value( "Language", QLocale::system().name() ).toString();
  if( m_language.size() > 2 )
    m_language.resize( 2 );
  m_lastDirectorySelected = sets.value( "LastDirectorySelected", QDesktopServices::storageLocation( QDesktopServices::DocumentsLocation ) ).toString();
  m_downloadDirectory = sets.value( "DownloadDirectory",QDesktopServices::storageLocation( QDesktopServices::DocumentsLocation ) ).toString();
  m_logPath = sets.value( "LogPath", "." ).toString();
  sets.endGroup();

  sets.beginGroup( "Tools" );
#if defined( BEEBEEP_DEBUG )
  m_debugMode = sets.value( "DebugMode", true ).toBool();
#else
  m_debugMode = sets.value( "DebugMode", false ).toBool();
#endif
  m_showTipsOfTheDay = sets.value( "ShowTipsOfTheDay", true ).toBool();
  sets.endGroup();

  if( m_localUser.name().isEmpty() )
  {
    QString sName = GetUserNameFromSystemEnvinroment();
    m_localUser.setName( sName );
  }
  qDebug() << "Local user:" << m_localUser.path();
}

void Settings::save()
{
  qDebug() << "Saving settings";
  QSettings sets( SETTINGS_FILE_NAME, SETTINGS_FILE_FORMAT );
  sets.beginGroup( "Chat" );
  sets.setValue( "Font", m_chatFont.toString() );
  sets.setValue( "FontColor", m_chatFontColor );
  sets.setValue( "CompactMessage", m_chatCompact );
  sets.setValue( "AddNewLineAfterMessage", m_chatAddNewLineToMessage );
  sets.setValue( "ShowMessageTimestamp", m_chatShowMessageTimestamp );
  sets.setValue( "SaveDirectory", m_chatSaveDirectory );
  sets.setValue( "BeepOnNewMessageArrived", m_beepOnNewMessageArrived );
  sets.endGroup();
  sets.beginGroup( "User" );
  sets.setValue( "LocalName", m_localUser.name() );
  sets.setValue( "LocalLastStatus", m_localUser.status() );
  sets.setValue( "LocalLastStatusDescription", m_localUser.statusDescription() );
  sets.setValue( "LocalListenerPort", m_localUser.hostPort() );
  sets.setValue( "LocalBroadcastPort", m_broadcastPort );
  sets.setValue( "ShowOnlyName", m_showOnlyUsername );
  sets.setValue( "ShowColors", m_showUserColor );
  sets.endGroup();
  sets.beginGroup( "Gui" );
  sets.setValue( "MainWindowGeometry", m_guiGeometry );
  sets.setValue( "MainWindowState", m_guiState );
  sets.setValue( "ShowMenuBar", m_showMenuBar );
  sets.setValue( "MainBarIconSize", m_mainBarIconSize );
  sets.setValue( "Language", m_language );
  sets.setValue( "LastDirectorySelected", m_lastDirectorySelected );
  sets.setValue( "DownloadDirectory", m_downloadDirectory );
  sets.setValue( "LogPath", m_logPath );
  sets.endGroup();
  sets.beginGroup( "Tools" );
  sets.setValue( "DebugMode", m_debugMode );
  sets.setValue( "ShowTipsOfTheDay", m_showTipsOfTheDay );
  sets.endGroup();
  sets.sync();
  qDebug() << "Settings saved";
}
