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

#ifndef BEEBEEP_SETTINGS_H
#define BEEBEEP_SETTINGS_H

#include "Config.h"
#include "NetworkAccount.h"
#include "User.h"


class Settings
{
// Singleton Object
  static Settings* mp_instance;

public:
  inline bool firstTime() const;
  inline void setFirstTime( bool );

  QString version( bool ) const;
  int protoVersion() const;
  QString programName() const;

  inline const User& localUser() const;
  inline void setLocalUser( const User& );
  void setLocalUserHost( const QHostAddress&, int );

  inline int broadcastPort() const;
  inline int broadcastInterval() const;
  inline int pingInterval() const;
  inline int pongTimeout() const;
  inline int writingTimeout() const;
  inline int fileTransferConfirmTimeout() const;
  inline int fileTransferBufferSize() const;
  inline int trayMessageTimeout() const;

  inline const QString& logPath() const;
  inline void setLogPath( const QString& );
  inline const QString& pluginPath() const;
  inline void setPluginPath( const QString& );
  inline const QString& localePath() const;
  inline void setLocalePath( const QString& );

  inline int chatMessageHistorySize() const;
  inline const QString& chatFontColor() const;
  inline void setChatFontColor( const QString& );
  inline const QFont& chatFont() const;
  inline void setChatFont( const QFont& );
  inline bool chatCompact() const;
  inline void setChatCompact( bool );
  inline bool chatAddNewLineToMessage() const;
  inline void setChatAddNewLineToMessage( bool );
  inline bool chatShowMessageTimestamp() const;
  inline void setChatShowMessageTimestamp( bool );
  inline void setChatUseHtmlTags( bool );
  inline bool chatUseHtmlTags() const;
  inline void setChatUseClickableLinks( bool );
  inline bool chatUseClickableLinks() const;
  inline const QString& chatSaveDirectory() const;
  inline void setChatSaveDirectory( const QString& );
  inline bool showOnlyOnlineUsers() const;
  inline void setShowOnlyOnlineUsers( bool );
  inline bool showUserColor() const;
  inline void setShowUserColor( bool );
  inline bool showEmoticons() const;
  inline void setShowEmoticons( bool );

  inline const QSize& mainBarIconSize() const;
  inline void setGuiGeometry( const QByteArray& );
  inline const QByteArray& guiGeometry() const;
  inline void setGuiState( const QByteArray& );
  inline const QByteArray& guiState() const;

  QByteArray hash( const QString& ) const;
  inline const QString& hash() const;
  void setPassword( const QString& );
  inline const QByteArray& password() const;
  inline QString defaultPassword() const;

  inline bool debugMode() const;
  inline void setDebugMode( bool );
  inline bool logToFile() const;
  inline void setLogToFile( bool );

  inline const QString& language() const;
  inline void setLanguage( const QString& );

  inline const QString& lastDirectorySelected() const;
  inline void setLastDirectorySelected( const QString& );

  inline const QString& downloadDirectory() const;
  inline void setDownloadDirectory( const QString& );

  inline bool beepOnNewMessageArrived() const;
  inline void setBeepOnNewMessageArrived( bool );

  inline bool showTipsOfTheDay() const;
  inline void setShowTipsOfTheDay( bool );

  inline bool automaticFileName() const;
  inline void setAutomaticFileName( bool );

  inline bool useNetworkProxy() const;
  inline bool networkProxyUseAuthentication() const;
  inline void setNetworkProxyUseAuthentication( bool );
  inline const QNetworkProxy& networkProxy() const;
  inline void setNetworkProxy( const QNetworkProxy& );

  inline const QList<NetworkAccount>& networkAccounts() const;
  void setNetworkAccount( const NetworkAccount& );
  NetworkAccount networkAccount( const QString& ) const;

  inline const QStringList& broadcastAddresses() const;
  inline void setBroadcastAddresses( const QStringList& );

  inline bool minimizeInTray() const;
  inline void setMinimizeInTray( bool );

  inline const QStringList& localShare() const;
  inline void setLocalShare( const QStringList& );
  inline bool fileShare() const;
  inline void setFileShare( bool );
  inline int maxFileShared() const;

  void load();
  void save();

  static Settings& instance()
  {
    if( !mp_instance )
      mp_instance = new Settings();
    return *mp_instance;
  }

  static void close()
  {
    if( mp_instance )
    {
      delete mp_instance;
      mp_instance = NULL;
    }
  }

protected:
  Settings();

  void loadBroadcastAddresses();

private:
  User m_localUser;
  int m_broadcastPort;
  int m_broadcastInterval;
  int m_pingInterval;
  int m_pongTimeout;
  int m_writingTimeout;
  int m_fileTransferConfirmTimeout;
  int m_fileTransferBufferSize;
  int m_trayMessageTimeout;

  QString m_logPath;
  QString m_pluginPath;
  QString m_localePath;

  int m_chatMessageHistorySize;
  QString m_chatFontColor;
  QFont m_chatFont;
  bool m_chatCompact;
  bool m_chatAddNewLineToMessage;
  bool m_chatShowMessageTimestamp;
  bool m_chatUseHtmlTags;
  bool m_chatUseClickableLinks;
  QString m_chatSaveDirectory;
  bool m_showOnlyOnlineUsers;
  bool m_showUserColor;
  bool m_showEmoticons;

  QByteArray m_guiGeometry;
  QByteArray m_guiState;
  QSize m_mainBarIconSize;

  QByteArray m_password;
  QString m_hash;

  bool m_debugMode;
  bool m_logToFile;

  QString m_language;

  QString m_lastDirectorySelected;
  QString m_downloadDirectory;

  bool m_beepOnNewMessageArrived;

  bool m_showTipsOfTheDay;
  bool m_automaticFileName;

  QNetworkProxy m_networkProxy;
  bool m_networkProxyUseAuthentication;

  QList<NetworkAccount> m_networkAccounts;

  QStringList m_broadcastAddresses;

  bool m_firstTime;

  bool m_minimizeInTray;

  QStringList m_localShare;
  bool m_fileShare;
  int m_maxFileShared;

};


// Inline Functions
inline const User& Settings::localUser() const { return m_localUser; }
inline void Settings::setLocalUser( const User& new_value ) { m_localUser = new_value; }
inline int Settings::broadcastPort() const { return m_broadcastPort; }
inline int Settings::broadcastInterval() const { return m_broadcastInterval; }
inline int Settings::pingInterval() const { return m_pingInterval; }
inline int Settings::pongTimeout() const { return m_pongTimeout; }
inline int Settings::writingTimeout() const { return m_writingTimeout; }
inline int Settings::fileTransferConfirmTimeout() const { return m_fileTransferConfirmTimeout; }
inline int Settings::fileTransferBufferSize() const { return m_fileTransferBufferSize; }
inline int Settings::trayMessageTimeout() const  { return m_trayMessageTimeout; }
inline const QString& Settings::logPath() const { return m_logPath; }
inline void Settings::setLogPath( const QString& new_value ) { m_logPath = new_value; }
inline const QString& Settings::pluginPath() const { return m_pluginPath; }
inline void Settings::setPluginPath( const QString& new_value ) { m_pluginPath = new_value; }
inline const QString& Settings::localePath() const { return m_localePath; }
inline void Settings::setLocalePath( const QString& new_value ) { m_localePath = new_value; }
inline const QSize& Settings::mainBarIconSize() const { return m_mainBarIconSize; }
inline int Settings::chatMessageHistorySize() const { return m_chatMessageHistorySize; }
inline const QString& Settings::chatFontColor() const { return m_chatFontColor; }
inline void Settings::setChatFontColor( const QString& new_value ) { m_chatFontColor = new_value; }
inline const QFont& Settings::chatFont() const { return m_chatFont; }
inline void Settings::setChatFont( const QFont& new_value ) { m_chatFont = new_value; }
inline bool Settings::chatCompact() const { return m_chatCompact; }
inline void Settings::setChatCompact( bool new_value ) { m_chatCompact = new_value; }
inline bool Settings::chatAddNewLineToMessage() const { return m_chatAddNewLineToMessage; }
inline void Settings::setChatAddNewLineToMessage( bool new_value ) { m_chatAddNewLineToMessage = new_value; }
inline bool Settings::chatShowMessageTimestamp() const { return m_chatShowMessageTimestamp; }
inline void Settings::setChatShowMessageTimestamp( bool new_value ) { m_chatShowMessageTimestamp = new_value; }
inline void Settings::setChatUseHtmlTags( bool new_value ) { m_chatUseHtmlTags = new_value; }
inline bool Settings::chatUseHtmlTags() const { return m_chatUseHtmlTags; }
inline void Settings::setChatUseClickableLinks( bool new_value ) { m_chatUseClickableLinks = new_value; }
inline bool Settings::chatUseClickableLinks() const { return m_chatUseClickableLinks; }
inline const QString& Settings::chatSaveDirectory() const { return m_chatSaveDirectory; }
inline void Settings::setChatSaveDirectory( const QString& new_value ) { m_chatSaveDirectory = new_value; }
inline bool Settings::showOnlyOnlineUsers() const { return m_showOnlyOnlineUsers; }
inline void Settings::setShowOnlyOnlineUsers( bool new_value ) { m_showOnlyOnlineUsers = new_value; }
inline bool Settings::showEmoticons() const { return m_showEmoticons; }
inline void Settings::setShowEmoticons( bool new_value ) { m_showEmoticons = new_value; }
inline void Settings::setGuiGeometry( const QByteArray& new_value ) { m_guiGeometry = new_value; }
inline const QByteArray& Settings::guiGeometry() const { return m_guiGeometry; }
inline void Settings::setGuiState( const QByteArray& new_value ) { m_guiState = new_value; }
inline const QByteArray& Settings::guiState() const { return m_guiState; }
inline const QString& Settings::hash() const { return m_hash; }
inline const QByteArray& Settings::password() const { return m_password; }
inline QString Settings::defaultPassword() const { return "*"; }
inline bool Settings::debugMode() const { return m_debugMode; }
inline void Settings::setDebugMode( bool new_value ) { m_debugMode = new_value; }
inline bool Settings::logToFile() const { return m_logToFile; }
inline void Settings::setLogToFile( bool new_value ) { m_logToFile = new_value; }
inline const QString& Settings::language() const { return m_language; }
inline void Settings::setLanguage( const QString& new_value ) { m_language = new_value; }
inline const QString& Settings::lastDirectorySelected() const { return m_lastDirectorySelected; }
inline void Settings::setLastDirectorySelected( const QString& new_value ) { m_lastDirectorySelected = new_value; }
inline const QString& Settings::downloadDirectory() const { return m_downloadDirectory; }
inline void Settings::setDownloadDirectory( const QString& new_value ) { m_downloadDirectory = new_value; }
inline bool Settings::beepOnNewMessageArrived() const { return m_beepOnNewMessageArrived; }
inline void Settings::setBeepOnNewMessageArrived( bool new_value ) { m_beepOnNewMessageArrived = new_value; }
inline bool Settings::showTipsOfTheDay() const { return m_showTipsOfTheDay; }
inline void Settings::setShowTipsOfTheDay( bool new_value ) { m_showTipsOfTheDay = new_value; }
inline void Settings::setShowUserColor( bool new_value ) { m_showUserColor = new_value; }
inline bool Settings::showUserColor() const { return m_showUserColor; }
inline bool Settings::automaticFileName() const { return m_automaticFileName; }
inline void Settings::setAutomaticFileName( bool new_value ) { m_automaticFileName = new_value; }
inline bool Settings::useNetworkProxy() const { return m_networkProxy.type() != QNetworkProxy::NoProxy; }
inline bool Settings::networkProxyUseAuthentication() const { return m_networkProxyUseAuthentication; }
inline void Settings::setNetworkProxyUseAuthentication( bool new_value ) { m_networkProxyUseAuthentication = new_value; }
inline const QNetworkProxy& Settings::networkProxy() const { return m_networkProxy; }
inline void Settings::setNetworkProxy( const QNetworkProxy& new_value ) { m_networkProxy = new_value; }
inline const QList<NetworkAccount>& Settings::networkAccounts() const { return m_networkAccounts; }
inline const QStringList& Settings::broadcastAddresses() const { return m_broadcastAddresses; }
inline void Settings::setBroadcastAddresses( const QStringList& new_value ) { m_broadcastAddresses = new_value; }
inline void Settings::setFirstTime( bool new_value ) { m_firstTime = new_value; }
inline bool Settings::firstTime() const { return m_firstTime; }
inline bool Settings::minimizeInTray() const { return m_minimizeInTray; }
inline void Settings::setMinimizeInTray( bool new_value ) { m_minimizeInTray = new_value; }
inline const QStringList& Settings::localShare() const { return m_localShare; }
inline void Settings::setLocalShare( const QStringList& new_value ) { m_localShare = new_value; }
inline bool Settings::fileShare() const { return m_fileShare; }
inline void Settings::setFileShare( bool new_value ) { m_fileShare = new_value; }
inline int Settings::maxFileShared() const { return m_maxFileShared; }


#endif // BEEBEEP_SETTINGS_H
