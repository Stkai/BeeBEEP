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
{
  m_localUser = Protocol::instance().createLocalUser();
  m_localUser.setStatus( User::Online );
  setPassword( defaultPassword() );
}

QString Settings::getUserName( const QProcessEnvironment& pe )
{
#if defined( BEEBEEP_DEBUG )
  return QString( "Bee%1" ).arg( QTime::currentTime().toString( "ss" ) );
#else
  QString sTmp = pe.value( "USERNAME" );
  if( sTmp.isNull() )
    sTmp = pe.value( "USER" );
  if( sTmp.isNull() )
    sTmp = QString( "Bee%1" ).arg( QTime::currentTime().toString( "ss" ) );
  return sTmp;
#endif
}

void Settings::checkSystemEnvinroment()
{
  QProcessEnvironment pe = QProcessEnvironment::systemEnvironment();
  QString sName = getUserName( pe );
  m_localUser.setName( sName );
  if( m_localUser.nickname().isEmpty() )
    m_localUser.setNickname( sName );
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

void Settings::load( bool check_environment_also )
{
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
  m_showUserIp = sets.value( "ShowAddressIp", false ).toBool();
  m_showUserNickname = sets.value( "ShowNickname", true ).toBool();
  m_localUser.setNickname( sets.value( "LocalNickname", "" ).toString() );
  m_localUser.setStatus( sets.value( "LocalLastStatus", m_localUser.status() ).toInt() );
  m_localUser.setStatusDescription( sets.value( "LocalLastStatusDescription", m_localUser.statusDescription() ).toString() );
  sets.endGroup();
  sets.beginGroup( "Geometry" );
  m_guiGeometry = sets.value( "MainWindow", "" ).toByteArray();
#ifdef Q_OS_SYMBIAN
  m_userListWidth = sets.value( "UserListWidth", 110 ).toInt();
#else
  m_userListWidth = sets.value( "UserListWidth", 140 ).toInt();
#endif
  sets.endGroup();
  sets.beginGroup( "Gui" );
  m_showMenuBar = sets.value( "ShowMenuBar", false ).toBool();
  m_showToolBar = sets.value( "ShowToolBar", true ).toBool();
  m_mainBarIconSize = sets.value( "MainBarIconSize", QSize( 24, 24 ) ).toSize();
#if defined( BEEBEEP_DEBUG )
  m_debugMode = sets.value( "DebugMode", true ).toBool();
#else
  m_debugMode = sets.value( "DebugMode", false ).toBool();
#endif
  m_language = sets.value( "Language", QLocale::system().name() ).toString();
  if( m_language.size() > 2 )
    m_language.resize( 2 );
  m_lastDirectorySelected = sets.value( "LastDirectorySelected", QDir::homePath() ).toString();
  m_downloadDirectory = sets.value( "DownloadDirectory", QDir::homePath() ).toString();
  m_logPath = sets.value( "LogPath", "." ).toString();
  sets.endGroup();

  sets.beginGroup( "Tools" );
  m_showTipsOfTheDay = sets.value( "ShowTipsOfTheDay", true ).toBool();
  sets.endGroup();

  if( check_environment_also )
    checkSystemEnvinroment();
}

void Settings::save()
{
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
  sets.setValue( "LocalNickname", m_localUser.nickname() );
  sets.setValue( "LocalLastStatus", m_localUser.status() );
  sets.setValue( "LocalLastStatusDescription", m_localUser.statusDescription() );
  sets.setValue( "ShowAddressIp", m_showUserIp );
  sets.setValue( "ShowNickname", m_showUserNickname );
  sets.endGroup();
  sets.beginGroup( "Geometry" );
  sets.setValue( "MainWindow", m_guiGeometry );
  sets.setValue( "UserListWidth", m_userListWidth );
  sets.endGroup();
  sets.beginGroup( "Gui" );
  sets.setValue( "ShowMenuBar", m_showMenuBar );
  sets.setValue( "ShowToolBar", m_showToolBar );
  sets.setValue( "MainBarIconSize", m_mainBarIconSize );
  sets.setValue( "DebugMode", m_debugMode );
  sets.setValue( "Language", m_language );
  sets.setValue( "LastDirectorySelected", m_lastDirectorySelected );
  sets.setValue( "DownloadDirectory", m_downloadDirectory );
  sets.setValue( "LogPath", m_logPath );
  sets.endGroup();
  sets.beginGroup( "Tools" );
  sets.setValue( "ShowTipsOfTheDay", m_showTipsOfTheDay );
  sets.endGroup();
  sets.sync();
}
