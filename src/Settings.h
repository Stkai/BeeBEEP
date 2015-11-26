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

#ifndef BEEBEEP_SETTINGS_H
#define BEEBEEP_SETTINGS_H

#include "Config.h"
#include "User.h"


class Settings
{
// Singleton Object
  static Settings* mp_instance;

public:
  QSettings* objectSettings() const; // deleteLater instance after call

  QString currentSettingsFilePath() const;
  QString operatingSystem( bool use_long_name ) const;
  QString operatingSystemIconPath() const;
  QString accountNameFromSystemEnvinroment() const;
  void createLocalUser();
  void createSessionId();
  void setResourceFolder();
  bool setDataFolder();
  bool createDefaultRcFile();
  bool createDefaultHostsFile();

  inline bool firstTime() const;
  inline void setFirstTime( bool );

  inline bool allowMultipleInstances() const;
  inline bool trustNickname() const;
  inline bool trustSystemAccount() const;

  inline const QDate& settingsCreationDate() const;
  inline const QString& dataFolder() const;
  inline const QString& resourceFolder() const;
  QString defaultHostsFilePath( bool use_resource_folder ) const;
  QString defaultRcFilePath( bool use_resource_folder ) const;
  QString defaultSettingsFilePath() const;
  QString defaultBeepFilePath( bool use_resource_folder ) const;
  QString defaultPluginFolderPath( bool use_resource_folder ) const;

  QString version( bool ) const;
  int protoVersion() const;
  inline int dataStreamVersion( bool in_load_event ) const;
  QString programName() const;
  QString organizationName() const;
  QString organizationDomain() const;
  QString officialWebSite() const;
  QString pluginWebSite() const;
  QString checkVersionWebSite() const;
  QString donationWebSite() const;
  QString helpWebSite() const;
  QString languageWebSite() const;
  QString httpUserAgent() const;
  QString facebookPage() const;
  inline void setIsFacebookPageLinkClicked( bool );
  inline bool isFacebookPageLinkClicked() const;

  QString dnsRecord() const;
  inline bool useMulticastDns() const;
  inline void setUseMulticastDns( bool );

  inline const User& localUser() const;
  inline void setLocalUser( const User& );
  void setLocalUserHost( const QHostAddress&, int );

  inline bool useIPv6() const;
  QHostAddress hostAddressToListen();
  inline const QHostAddress& multicastGroupAddress() const;
  inline int defaultBroadcastPort() const;
  inline int defaultListenerPort() const;
  inline int defaultFileTransferPort() const;
  inline int broadcastInterval() const;
  inline int broadcastLoopbackInterval() const;
  inline int pingInterval() const;
  inline int pongTimeout() const;
  inline int writingTimeout() const;
  inline int fileTransferConfirmTimeout() const;
  inline int fileTransferBufferSize() const;
  inline int trayMessageTimeout() const;
  inline int connectionTimeout() const;

  inline int userAwayTimeout() const;
  inline void setUserAwayTimeout( int );

  inline const QString& logPath() const;
  inline void setLogPath( const QString& );
  inline QString logFilePath() const;
  inline const QString& pluginPath() const;
  inline void setPluginPath( const QString& );
  inline const QString& languagePath() const;
  inline void setLanguagePath( const QString& );
  QString languageFilePath( const QString&, const QString& ) const;

  inline int emoticonSizeInEdit() const;
  inline int emoticonSizeInChat() const;
  inline int emoticonSizeInMenu() const;
  inline int emoticonInRecentMenu() const;
  inline void setRecentEmoticons( const QStringList& );
  inline const QStringList& recentEmoticons() const;

  inline QString defaultChatName() const;
  inline int chatMessageHistorySize() const;
  inline const QString& chatFontColor() const;
  inline void setChatFontColor( const QString& );
  inline const QFont& chatFont() const;
  void setChatFont( const QFont& );
  inline void setDefaultChatBackgroundColor( const QString& );
  inline const QString& defaultChatBackgroundColor() const;
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
  inline void setChatMessageFilter( const QBitArray& );
  inline const QBitArray& chatMessageFilter() const;
  inline bool showOnlyOnlineUsers() const;
  inline void setShowOnlyOnlineUsers( bool );
  inline bool showUserColor() const;
  inline void setShowUserColor( bool );
  inline bool showUserPhoto() const;
  inline void setShowUserPhoto( bool );
  inline bool showEmoticons() const;
  inline void setShowEmoticons( bool );
  inline bool showMessagesGroupByUser() const;
  inline void setShowMessagesGroupByUser( bool );
  inline bool stayOnTop() const;
  inline void setStayOnTop( bool );
  inline bool raiseOnNewMessageArrived() const;
  inline void setRaiseOnNewMessageArrived( bool );
  inline bool showChatToolbar() const;
  inline void setShowChatToolbar( bool );
  inline bool showOnlyMessagesInDefaultChat() const;
  inline void setShowOnlyMessagesInDefaultChat( bool );
  inline bool showVCardOnRightClick() const;
  inline void setShowVCardOnRightClick( bool );
  inline bool chatMaxMessagesToShow() const;
  inline void setChatMaxMessagesToShow( bool );
  inline int chatMessagesToShow() const;
  inline void setChatMessagesToShow( int );
  inline const QSize& mainBarIconSize() const;
  inline void setAvatarIconSize( const QSize& );
  inline const QSize& avatarIconSize() const;
  inline void setGuiGeometry( const QByteArray& );
  inline const QByteArray& guiGeometry() const;
  inline void setGuiState( const QByteArray& );
  inline const QByteArray& guiState() const;
  inline void setChatSplitterState( const QByteArray& );
  inline const QByteArray& chatSplitterState() const;
  inline void setShowEmoticonMenu( bool );
  inline bool showEmoticonMenu() const;
  inline bool isNotificationDisabledForGroup( const QString& ) const;
  void setNotificationEnabledForGroup( const QString&, bool );
  inline void setUseNativeEmoticons( bool );
  inline bool useNativeEmoticons() const;
  inline void setShowImagePreview( bool );
  inline bool showImagePreview() const;
  inline int imagePreviewHeight() const;
  inline void setFloatingChatGeometry( const QByteArray& );
  inline void setFloatingChatState( const QByteArray& );
  inline void setFloatingChatSplitterState( const QByteArray& );
  inline const QByteArray& floatingChatGeometry() const;
  inline const QByteArray& floatingChatState() const;
  inline const QByteArray& floatingChatSplitterState() const;
  inline void setShowUserStatusBackgroundColor( bool );
  inline bool showUserStatusBackgroundColor() const;
  inline void setUseReturnToSendMessage( bool );
  inline bool useReturnToSendMessage() const;

  QByteArray hash( const QString& ) const;
  QString currentHash() const;

  void setPassword( const QString& );
  inline const QByteArray& password() const;
  inline QString defaultPassword() const;
  inline void setUseDefaultPassword( bool );
  inline bool useDefaultPassword() const;
  inline void setSavePassword( bool );
  inline bool savePassword() const;
  inline const QString& passwordBeforeHash() const;
  inline void setAskPasswordAtStartup( bool );
  inline bool askPasswordAtStartup() const;
  bool askPassword() const;

  inline bool logToFile() const;
  inline void setLogToFile( bool );

  inline const QString& language() const;
  inline void setLanguage( const QString& );

  inline const QString& lastDirectorySelected() const;
  inline void setLastDirectorySelected( const QString& );
  void setLastDirectorySelectedFromFile( const QString & );

  inline const QString& downloadDirectory() const;
  inline void setDownloadDirectory( const QString& );

  inline bool beepOnNewMessageArrived() const;
  inline void setBeepOnNewMessageArrived( bool );
  inline bool beepAlwaysOnNewMessageArrived() const;
  inline void setBeepAlwaysOnNewMessageArrived( bool );
  inline const QString& beepFilePath() const;
  inline void setBeepFilePath( const QString& );

  inline bool showTipsOfTheDay() const;
  inline void setShowTipsOfTheDay( bool );

  inline bool automaticFileName() const;
  inline void setAutomaticFileName( bool );

  inline const QStringList& broadcastAddressesInFileHosts() const;
  inline const QStringList& broadcastAddressesInSettings() const;
  int setBroadcastAddressesInSettings( const QStringList& );
  inline const QHostAddress& localHostAddressForced() const;
  inline const QString& localSubnetForced() const;
  inline bool broadcastOnlyToHostsIni() const;
  inline bool parseBroadcastAddresses() const;
  inline void setParseBroadcastAddresses( bool );
  inline bool addExternalSubnetAutomatically() const;
  inline void setAddExternalSubnetAutomatically( bool );
  bool addSubnetToBroadcastAddress( const QHostAddress& );
  inline void SetPreventMultipleConnectionsFromSingleHostAddress( bool );
  inline bool preventMultipleConnectionsFromSingleHostAddress();
  inline const QString& preferredSubnets() const;

  inline bool keyEscapeMinimizeInTray() const;
  inline void setKeyEscapeMinimizeInTray( bool );
  inline bool minimizeInTray() const;
  inline void setMinimizeInTray( bool );
  inline bool loadOnTrayAtStartup() const;
  inline void setLoadOnTrayAtStartup( bool );
  inline bool showNotificationOnTray() const;
  inline void setShowNotificationOnTray( bool );
  inline bool showOnlyMessageNotificationOnTray() const;
  inline void setShowOnlyMessageNotificationOnTray( bool );

  inline const QStringList& localShare() const;
  inline void setLocalShare( const QStringList& );
  inline bool hasLocalSharePath( const QString& ) const;
  inline bool fileTransferIsEnabled() const;
  inline void setFileTransferIsEnabled( bool );
  inline int maxFileShared() const;

  inline const QString& chatSaveDirectory() const;
  inline void setChatSaveDirectory( const QString& );
  QString savedChatsFilePath() const;
  inline bool chatAutoSave() const;
  inline void setChatAutoSave( bool );
  inline int chatMaxLineSaved() const;
  inline void setChatMaxLineSaved( int );

  inline bool autoUserAway() const;
  inline void setAutoUserAway( bool );
  inline bool autoLinkSavedChatByNickname() const;
  inline void setAutoLinkSavedChatByNickname( bool );

  inline bool confirmOnDownloadFile() const;
  inline void setConfirmOnDownloadFile( bool );
  inline int maxSimultaneousDownloads() const;
  inline int maxQueuedDownloads() const;

  inline QStringList pluginSettings( const QString& ) const;
  inline void setPluginSettings( const QString&, const QStringList& );
  inline bool pluginHasSettings( const QString& ) const;

  void addTemporaryFilePath( const QString& );
  void clearTemporaryFile();

  bool addStartOnSystemBoot();
  bool removeStartOnSystemBoot();
  bool hasStartOnSystemBoot() const;

  inline void setGroupList( const QStringList& );
  inline const QStringList& groupList() const;

  inline void setUserPathList( const QStringList& );
  inline const QStringList& userPathList() const;
  inline void setUserStatusList( const QStringList& );
  inline const QStringList& userStatusList() const;
  inline int maxUserStatusInList() const;

  inline void setSaveUserList( bool );
  inline bool saveUserList() const;
  inline void setUserList( const QStringList& );
  inline const QStringList& userList() const;

  inline void setAcceptConnectionsOnlyFromWorkgroups( bool );
  inline bool acceptConnectionsOnlyFromWorkgroups() const;
  inline void setWorkgroups( const QStringList& );
  inline const QStringList& workgroups() const;

  inline void setShortcuts( const QStringList& );
  inline const QStringList& shortcuts() const;
  inline void setUseShortcuts( bool );
  inline bool useShortcuts() const;

  inline void setShowHomeAsDefaultPage( bool );
  inline bool showHomeAsDefaultPage() const;
  inline void setResetGeometryAtStartup( bool );
  inline bool resetGeometryAtStartup() const;
  inline void setShowMinimizedAtStartup( bool );
  inline bool showMinimizedAtStartup() const;

  inline void setPromptOnCloseEvent( bool );
  inline bool promptOnCloseEvent() const;
  inline void setAlwaysOpenNewFloatingChat( bool );
  inline bool alwaysOpenNewFloatingChat() const;

  inline void setUseSpellChecker( bool );
  inline bool useSpellChecker() const;
  inline void setDictionaryPath( const QString& );
  inline const QString& dictionaryPath() const;

  void loadRcFile();
  void clearNativeSettings();
  void load();
  void save();
  inline const QDateTime& lastSave() const;

  QString simpleEncrypt( const QString& );
  QString simpleDecrypt( const QString& );

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

  void loadBroadcastAddressesFromFileHosts();
  QHostAddress subnetFromHostAddress( const QHostAddress& ) const;
  bool addBroadcastAddressInSettings( const QString& );

private:
  QString m_resourceFolder;
  QString m_dataFolder;

  // RC
  bool m_useSettingsFileIni;
  bool m_trustNickname;
  bool m_trustSystemAccount;
  bool m_broadcastOnlyToHostsIni;
  int m_defaultBroadcastPort;
  int m_defaultListenerPort;
  int m_defaultFileTransferPort;
  bool m_saveDataInDocumentsFolder;
  bool m_saveDataInUserApplicationFolder;
  bool m_allowMultipleInstances;
  QString m_dataFolderInRC;
  bool m_addAccountNameToDataFolder;
  QString m_preferredSubnets;
  bool m_useIPv6;
  QHostAddress m_multicastGroupAddress;

  // Ini
  bool m_firstTime;
  QDate m_settingsCreationDate;
  int m_settingsVersion;

  User m_localUser;
  int m_broadcastInterval;
  int m_broadcastLoopbackInterval;
  int m_pingInterval;
  int m_pongTimeout;
  int m_writingTimeout;
  int m_fileTransferConfirmTimeout;
  int m_fileTransferBufferSize;
  int m_trayMessageTimeout;
  int m_userAwayTimeout;
  int m_connectionTimeout;

  QDateTime m_lastSave;
  bool m_logToFile;
  QString m_logPath;
  QString m_pluginPath;
  QString m_languagePath;
  QString m_lastDirectorySelected;
  QString m_downloadDirectory;

  int m_chatMessageHistorySize;
  QString m_chatFontColor;
  QFont m_chatFont;
  QString m_defaultChatBackgroundColor;
  bool m_chatCompact;
  bool m_chatAddNewLineToMessage;
  bool m_chatShowMessageTimestamp;
  bool m_chatUseHtmlTags;
  bool m_chatUseClickableLinks;
  QBitArray m_chatMessageFilter;
  bool m_showOnlyOnlineUsers;
  bool m_showUserColor;
  bool m_showEmoticons;
  bool m_showMessagesGroupByUser;
  bool m_stayOnTop;
  bool m_raiseOnNewMessageArrived;
  bool m_showUserPhoto;
  bool m_showTipsOfTheDay;
  bool m_automaticFileName;
  bool m_showChatToolbar;
  bool m_showOnlyMessagesInDefaultChat;
  bool m_showVCardOnRightClick;
  bool m_chatMaxMessagesToShow;
  int m_chatMessagesToShow;
  bool m_showEmoticonMenu;
  bool m_useNativeEmoticons;
  bool m_showImagePreview;
  int m_imagePreviewHeight;
  bool m_showUserStatusBackgroundColor;
  bool m_useReturnToSendMessage;

  QByteArray m_guiGeometry;
  QByteArray m_guiState;
  QSize m_mainBarIconSize;
  QSize m_avatarIconSize;
  QByteArray m_chatSplitterState;
  int m_emoticonSizeInEdit;
  int m_emoticonSizeInChat;
  int m_emoticonSizeInMenu;
  int m_emoticonInRecentMenu;
  QStringList m_recentEmoticons;

  QByteArray m_floatingChatGeometry;
  QByteArray m_floatingChatState;
  QByteArray m_floatingChatSplitterState;

  QString m_passwordBeforeHash;
  QByteArray m_password;
  bool m_useDefaultPassword;
  bool m_savePassword;
  bool m_askPasswordAtStartup;

  QString m_language;
  bool m_beepOnNewMessageArrived;
  bool m_beepAlwaysOnNewMessageArrived;
  QString m_beepFilePath;

  QStringList m_broadcastAddressesInFileHosts;
  QStringList m_broadcastAddressesInSettings;
  QHostAddress m_localHostAddressForced;
  QString m_localSubnetForced;
  bool m_parseBroadcastAddresses;
  bool m_addExternalSubnetAutomatically;
  bool m_useMulticastDns;
  bool m_preventMultipleConnectionsFromSingleHostAddress;

  bool m_keyEscapeMinimizeInTray;
  bool m_minimizeInTray;
  bool m_loadOnTrayAtStartup;
  bool m_showNotificationOnTray;
  bool m_showOnlyMessageNotificationOnTray;

  bool m_fileTransferIsEnabled;
  QStringList m_localShare;
  int m_maxFileShared;

  QString m_chatSaveDirectory;
  bool m_chatAutoSave;
  int m_chatMaxLineSaved;

  bool m_autoUserAway;
  bool m_autoLinkSavedChatByNickname;

  QMap<QString, QStringList> m_pluginSettings;

  QStringList m_tempFilePathList;

  int m_dataStreamVersion;

  bool m_confirmOnDownloadFile;
  int m_maxSimultaneousDownloads;
  int m_maxQueuedDownloads;

  QStringList m_groupList;
  QStringList m_userPathList;
  QStringList m_groupSilenced;
  QStringList m_userStatusList;
  int m_maxUserStatusInList;

  bool m_saveUserList;
  QStringList m_userList;

  bool m_showHomeAsDefaultPage;
  bool m_resetGeometryAtStartup;
  bool m_showMinimizedAtStartup;
  bool m_promptOnCloseEvent;
  bool m_alwaysOpenNewFloatingChat;

  bool m_isFacebookPageLinkClicked;

  bool m_acceptConnectionsOnlyFromWorkgroups;
  QStringList m_workgroups;

  QStringList m_shortcuts;
  bool m_useShortcuts;

  bool m_useSpellChecker;
  QString m_dictionaryPath;

};


// Inline Functions
inline const QString& Settings::resourceFolder() const { return m_resourceFolder; }
inline const QString& Settings::dataFolder() const { return m_dataFolder; }
inline const User& Settings::localUser() const { return m_localUser; }
inline void Settings::setLocalUser( const User& new_value ) { m_localUser = new_value; }
inline bool Settings::allowMultipleInstances() const { return m_allowMultipleInstances; }
inline bool Settings::trustNickname() const { return m_trustNickname; }
inline bool Settings::trustSystemAccount() const { return m_trustSystemAccount; }
inline int Settings::defaultBroadcastPort() const { return m_defaultBroadcastPort; }
inline int Settings::defaultListenerPort() const { return m_defaultListenerPort; }
inline int Settings::defaultFileTransferPort() const { return m_defaultFileTransferPort; }
inline int Settings::broadcastInterval() const { return m_broadcastInterval; }
inline int Settings::broadcastLoopbackInterval() const { return m_broadcastLoopbackInterval; }
inline int Settings::pingInterval() const { return m_pingInterval; }
inline int Settings::pongTimeout() const { return m_pongTimeout; }
inline int Settings::writingTimeout() const { return m_writingTimeout; }
inline int Settings::fileTransferConfirmTimeout() const { return m_fileTransferConfirmTimeout; }
inline int Settings::fileTransferBufferSize() const { return m_fileTransferBufferSize; }
inline int Settings::trayMessageTimeout() const  { return m_trayMessageTimeout; }
inline int Settings::userAwayTimeout() const { return m_userAwayTimeout; }
inline void Settings::setUserAwayTimeout( int new_value ) { m_userAwayTimeout = new_value; }
inline const QString& Settings::logPath() const { return m_logPath; }
inline void Settings::setLogPath( const QString& new_value ) { m_logPath = new_value; }
inline QString Settings::logFilePath() const { return QDir::toNativeSeparators( QString( "%1/%2.log" ).arg( m_logPath, programName().toLower() ) ); }
inline const QString& Settings::pluginPath() const { return m_pluginPath; }
inline void Settings::setPluginPath( const QString& new_value ) { m_pluginPath = new_value; }
inline const QString& Settings::languagePath() const { return m_languagePath; }
inline void Settings::setLanguagePath( const QString& new_value ) { m_languagePath = new_value; }
inline const QSize& Settings::mainBarIconSize() const { return m_mainBarIconSize; }
inline void Settings::setAvatarIconSize( const QSize& new_value ) { m_avatarIconSize = new_value; }
inline const QSize& Settings::avatarIconSize() const { return m_avatarIconSize; }
inline QString Settings::defaultChatName() const { return QString( "* BeeBEEP Chat *" ); }
inline int Settings::chatMessageHistorySize() const { return m_chatMessageHistorySize; }
inline const QString& Settings::chatFontColor() const { return m_chatFontColor; }
inline void Settings::setChatFontColor( const QString& new_value ) { m_chatFontColor = new_value; }
inline const QFont& Settings::chatFont() const { return m_chatFont; }
inline void Settings::setDefaultChatBackgroundColor( const QString& new_value ) { m_defaultChatBackgroundColor = new_value; }
inline const QString& Settings::defaultChatBackgroundColor() const { return m_defaultChatBackgroundColor; }
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
inline void Settings::setChatMessageFilter( const QBitArray& new_value ) { m_chatMessageFilter = new_value; }
inline const QBitArray& Settings::chatMessageFilter() const { return m_chatMessageFilter; }
inline bool Settings::showOnlyOnlineUsers() const { return m_showOnlyOnlineUsers; }
inline void Settings::setShowOnlyOnlineUsers( bool new_value ) { m_showOnlyOnlineUsers = new_value; }
inline bool Settings::showUserPhoto() const { return m_showUserPhoto; }
inline void Settings::setShowUserPhoto( bool new_value ) { m_showUserPhoto = new_value; }
inline bool Settings::showEmoticons() const { return m_showEmoticons; }
inline void Settings::setShowEmoticons( bool new_value ) { m_showEmoticons = new_value; }
inline void Settings::setShowEmoticonMenu(bool new_value ) { m_showEmoticonMenu = new_value; }
inline bool Settings::showEmoticonMenu() const { return m_showEmoticonMenu; }
inline bool Settings::showOnlyMessagesInDefaultChat() const { return m_showOnlyMessagesInDefaultChat; }
inline void Settings::setShowOnlyMessagesInDefaultChat( bool new_value ) { m_showOnlyMessagesInDefaultChat = new_value; }
inline void Settings::setGuiGeometry( const QByteArray& new_value ) { m_guiGeometry = new_value; }
inline const QByteArray& Settings::guiGeometry() const { return m_guiGeometry; }
inline void Settings::setGuiState( const QByteArray& new_value ) { m_guiState = new_value; }
inline const QByteArray& Settings::guiState() const { return m_guiState; }
inline const QByteArray& Settings::password() const { return m_password; }
inline QString Settings::defaultPassword() const { return "*"; }
inline void Settings::setUseDefaultPassword( bool new_value ) { m_useDefaultPassword = new_value; }
inline bool Settings::useDefaultPassword() const { return m_useDefaultPassword; }
inline void Settings::setSavePassword( bool new_value ) { m_savePassword = new_value; }
inline bool Settings::savePassword() const { return m_savePassword; }
inline const QString& Settings::passwordBeforeHash() const { return m_passwordBeforeHash; }
inline void Settings::setAskPasswordAtStartup( bool new_value ) { m_askPasswordAtStartup = new_value; }
inline bool Settings::askPasswordAtStartup() const { return m_askPasswordAtStartup; }
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
inline bool Settings::beepAlwaysOnNewMessageArrived() const { return m_beepAlwaysOnNewMessageArrived; }
inline void Settings::setBeepAlwaysOnNewMessageArrived( bool new_value ) { m_beepAlwaysOnNewMessageArrived = new_value; }
inline const QString& Settings::beepFilePath() const { return m_beepFilePath; }
inline void Settings::setBeepFilePath( const QString& new_value ) { m_beepFilePath = new_value; }
inline bool Settings::showTipsOfTheDay() const { return m_showTipsOfTheDay; }
inline void Settings::setShowTipsOfTheDay( bool new_value ) { m_showTipsOfTheDay = new_value; }
inline void Settings::setShowUserColor( bool new_value ) { m_showUserColor = new_value; }
inline bool Settings::showUserColor() const { return m_showUserColor; }
inline bool Settings::showVCardOnRightClick() const { return m_showVCardOnRightClick; }
inline void Settings::setShowVCardOnRightClick( bool new_value ) { m_showVCardOnRightClick = new_value; }
inline bool Settings::showMessagesGroupByUser() const { return m_showMessagesGroupByUser; }
inline void Settings::setShowMessagesGroupByUser( bool new_value ) { m_showMessagesGroupByUser = new_value; }
inline bool Settings::stayOnTop() const { return m_stayOnTop; }
inline void Settings::setStayOnTop( bool new_value ) { m_stayOnTop = new_value; }
inline bool Settings::raiseOnNewMessageArrived() const { return m_raiseOnNewMessageArrived; }
inline void Settings::setRaiseOnNewMessageArrived( bool new_value ) { m_raiseOnNewMessageArrived = new_value; }
inline bool Settings::automaticFileName() const { return m_automaticFileName; }
inline void Settings::setAutomaticFileName( bool new_value ) { m_automaticFileName = new_value; }
inline bool Settings::showChatToolbar() const { return m_showChatToolbar; }
inline void Settings::setShowChatToolbar( bool new_value ) { m_showChatToolbar = new_value; }
inline const QStringList& Settings::broadcastAddressesInFileHosts() const { return m_broadcastAddressesInFileHosts; }
inline const QStringList& Settings::broadcastAddressesInSettings() const { return m_broadcastAddressesInSettings; }
inline bool Settings::parseBroadcastAddresses() const { return m_parseBroadcastAddresses; }
inline void Settings::setParseBroadcastAddresses( bool new_value ) { m_parseBroadcastAddresses = new_value; }
inline const QHostAddress& Settings::localHostAddressForced() const { return m_localHostAddressForced; }
inline const QString& Settings::localSubnetForced() const { return m_localSubnetForced; }
inline bool Settings::broadcastOnlyToHostsIni() const { return m_broadcastOnlyToHostsIni; }
inline bool Settings::addExternalSubnetAutomatically() const { return m_addExternalSubnetAutomatically; }
inline void Settings::setAddExternalSubnetAutomatically( bool new_value ) { m_addExternalSubnetAutomatically = new_value; }
inline bool Settings::useMulticastDns() const { return m_useMulticastDns; }
inline void Settings::setUseMulticastDns( bool new_value ) { m_useMulticastDns = new_value; }
inline void Settings::setFirstTime( bool new_value ) { m_firstTime = new_value; }
inline bool Settings::firstTime() const { return m_firstTime; }
inline bool Settings::keyEscapeMinimizeInTray() const { return m_keyEscapeMinimizeInTray; }
inline void Settings::setKeyEscapeMinimizeInTray( bool new_value ) { m_keyEscapeMinimizeInTray = new_value; }
inline bool Settings::minimizeInTray() const { return m_minimizeInTray; }
inline void Settings::setMinimizeInTray( bool new_value ) { m_minimizeInTray = new_value; }
inline bool Settings::loadOnTrayAtStartup() const { return m_loadOnTrayAtStartup; }
inline void Settings::setLoadOnTrayAtStartup( bool new_value ) { m_loadOnTrayAtStartup = new_value; }
inline bool Settings::showNotificationOnTray() const { return m_showNotificationOnTray; }
inline void Settings::setShowNotificationOnTray( bool new_value ) { m_showNotificationOnTray = new_value; }
inline bool Settings::showOnlyMessageNotificationOnTray() const { return m_showOnlyMessageNotificationOnTray; }
inline void Settings::setShowOnlyMessageNotificationOnTray( bool new_value ) { m_showOnlyMessageNotificationOnTray = new_value; }
inline const QStringList& Settings::localShare() const { return m_localShare; }
inline void Settings::setLocalShare( const QStringList& new_value ) { m_localShare = new_value; }
inline bool Settings::hasLocalSharePath( const QString& share_path ) const { return m_localShare.contains( share_path ); }
inline bool Settings::fileTransferIsEnabled() const { return m_fileTransferIsEnabled; }
inline void Settings::setFileTransferIsEnabled( bool new_value ) { m_fileTransferIsEnabled = new_value; }
inline int Settings::maxFileShared() const { return m_maxFileShared; }
inline const QString& Settings::chatSaveDirectory() const { return m_chatSaveDirectory; }
inline void Settings::setChatSaveDirectory( const QString& new_value ) { m_chatSaveDirectory = new_value; }
inline bool Settings::chatAutoSave() const { return m_chatAutoSave; }
inline void Settings::setChatAutoSave( bool new_value ) { m_chatAutoSave = new_value; }
inline int Settings::chatMaxLineSaved() const { return m_chatMaxLineSaved; }
inline void Settings::setChatMaxLineSaved( int new_value ) { m_chatMaxLineSaved = new_value; }
inline bool Settings::autoUserAway() const { return m_autoUserAway; }
inline void Settings::setAutoUserAway( bool new_value ) { m_autoUserAway = new_value; }
inline bool Settings::autoLinkSavedChatByNickname() const { return m_autoLinkSavedChatByNickname; }
inline void Settings::setAutoLinkSavedChatByNickname( bool new_value ) { m_autoLinkSavedChatByNickname = new_value; }
inline QStringList Settings::pluginSettings( const QString& plugin_name ) const { return m_pluginSettings.value( plugin_name ); }
inline void Settings::setPluginSettings( const QString& plugin_name, const QStringList& plugin_settings ) { m_pluginSettings.insert( plugin_name, plugin_settings ); }
inline bool Settings::pluginHasSettings( const QString& plugin_name ) const { return m_pluginSettings.contains( plugin_name ); }
inline int Settings::dataStreamVersion( bool in_load_event ) const { return in_load_event ? m_dataStreamVersion : LAST_DATASTREAM_VERSION; }
inline bool Settings::confirmOnDownloadFile() const { return m_confirmOnDownloadFile; }
inline void Settings::setConfirmOnDownloadFile( bool new_value ) { m_confirmOnDownloadFile = new_value; }
inline int Settings::maxSimultaneousDownloads() const { return m_maxSimultaneousDownloads; }
inline int Settings::maxQueuedDownloads() const { return m_maxQueuedDownloads; }
inline const QDate& Settings::settingsCreationDate() const { return m_settingsCreationDate; }
inline void Settings::setGroupList( const QStringList& new_value ) { m_groupList = new_value; }
inline const QStringList& Settings::groupList() const { return m_groupList; }
inline const QDateTime& Settings::lastSave() const { return m_lastSave; }
inline void Settings::setUserPathList( const QStringList& new_value ) { m_userPathList = new_value; }
inline const QStringList& Settings::userPathList() const { return m_userPathList; }
inline void Settings::setUserStatusList( const QStringList& new_value ) { m_userStatusList = new_value; }
inline const QStringList& Settings::userStatusList() const { return m_userStatusList; }
inline void Settings::setSaveUserList( bool new_value ) { m_saveUserList = new_value; }
inline bool Settings::saveUserList() const { return m_saveUserList; }
inline void Settings::setUserList( const QStringList& new_value ) { m_userList = new_value; }
inline const QStringList& Settings::userList() const { return m_userList; }
inline void Settings::setShowHomeAsDefaultPage( bool new_value ) { m_showHomeAsDefaultPage = new_value; }
inline bool Settings::showHomeAsDefaultPage() const { return m_showHomeAsDefaultPage; }
inline void Settings::setResetGeometryAtStartup( bool new_value ) { m_resetGeometryAtStartup = new_value; }
inline bool Settings::resetGeometryAtStartup() const { return m_resetGeometryAtStartup; }
inline void Settings::setShowMinimizedAtStartup( bool new_value ) { m_showMinimizedAtStartup = new_value; }
inline bool Settings::showMinimizedAtStartup() const { return m_showMinimizedAtStartup; }
inline void Settings::setChatSplitterState( const QByteArray& new_value ) { m_chatSplitterState = new_value; }
inline const QByteArray& Settings::chatSplitterState() const { return m_chatSplitterState; }
inline int Settings::chatMessagesToShow() const { return m_chatMessagesToShow; }
inline void Settings::setChatMessagesToShow( int new_value ) { m_chatMessagesToShow = new_value; }
inline bool Settings::chatMaxMessagesToShow() const { return m_chatMaxMessagesToShow; }
inline void Settings::setChatMaxMessagesToShow( bool new_value ) { m_chatMaxMessagesToShow = new_value; }
inline int Settings::emoticonSizeInEdit() const { return m_emoticonSizeInEdit; }
inline int Settings::emoticonSizeInChat() const { return m_emoticonSizeInChat; }
inline int Settings::emoticonSizeInMenu() const { return m_emoticonSizeInMenu; }
inline int Settings::emoticonInRecentMenu() const { return m_emoticonInRecentMenu; }
inline void Settings::setRecentEmoticons( const QStringList& new_value ) { m_recentEmoticons = new_value; }
inline const QStringList& Settings::recentEmoticons() const { return m_recentEmoticons; }
inline bool Settings::isNotificationDisabledForGroup( const QString& group_id ) const { return group_id.isEmpty() ? false : m_groupSilenced.contains( group_id ); }
inline void Settings::setUseNativeEmoticons( bool new_value ) { m_useNativeEmoticons = new_value; }
inline bool Settings::useNativeEmoticons() const { return m_useNativeEmoticons; }
inline void Settings::setShowImagePreview( bool new_value ) { m_showImagePreview = new_value; }
inline bool Settings::showImagePreview() const { return m_showImagePreview; }
inline int Settings::imagePreviewHeight() const { return m_imagePreviewHeight; }
inline void Settings::setPromptOnCloseEvent( bool new_value ) { m_promptOnCloseEvent = new_value; }
inline bool Settings::promptOnCloseEvent() const { return m_promptOnCloseEvent; }
inline void Settings::setIsFacebookPageLinkClicked( bool new_value ) { m_isFacebookPageLinkClicked = new_value; }
inline bool Settings::isFacebookPageLinkClicked() const { return m_isFacebookPageLinkClicked; }
inline void Settings::SetPreventMultipleConnectionsFromSingleHostAddress( bool new_value ) { m_preventMultipleConnectionsFromSingleHostAddress = new_value; }
inline bool Settings::preventMultipleConnectionsFromSingleHostAddress() { return m_preventMultipleConnectionsFromSingleHostAddress; }
inline void Settings::setFloatingChatGeometry( const QByteArray& new_value ) { m_floatingChatGeometry = new_value; }
inline void Settings::setFloatingChatState( const QByteArray& new_value ) { m_floatingChatState = new_value; }
inline void Settings::setFloatingChatSplitterState( const QByteArray& new_value ) { m_floatingChatSplitterState = new_value; }
inline const QByteArray& Settings::floatingChatGeometry() const { return m_floatingChatGeometry; }
inline const QByteArray& Settings::floatingChatState() const { return m_floatingChatState; }
inline const QByteArray& Settings::floatingChatSplitterState() const { return m_floatingChatSplitterState; }
inline void Settings::setAlwaysOpenNewFloatingChat( bool new_value ) { m_alwaysOpenNewFloatingChat = new_value; }
inline bool Settings::alwaysOpenNewFloatingChat() const { return m_alwaysOpenNewFloatingChat; }
inline void Settings::setAcceptConnectionsOnlyFromWorkgroups( bool new_value ) { m_acceptConnectionsOnlyFromWorkgroups = new_value; }
inline bool Settings::acceptConnectionsOnlyFromWorkgroups() const { return m_acceptConnectionsOnlyFromWorkgroups; }
inline void Settings::setWorkgroups( const QStringList& new_value ) { m_workgroups = new_value; }
inline const QStringList& Settings::workgroups() const { return m_workgroups; }
inline int Settings::maxUserStatusInList() const { return m_maxUserStatusInList; }
inline void Settings::setShowUserStatusBackgroundColor( bool new_value ) { m_showUserStatusBackgroundColor = new_value; }
inline bool Settings::showUserStatusBackgroundColor() const { return m_showUserStatusBackgroundColor; }
inline void Settings::setShortcuts( const QStringList& new_value ) { m_shortcuts = new_value; }
inline const QStringList& Settings::shortcuts() const { return m_shortcuts; }
inline void Settings::setUseShortcuts( bool new_value ) { m_useShortcuts = new_value; }
inline bool Settings::useShortcuts() const { return m_useShortcuts; }
inline int Settings::connectionTimeout() const { return m_connectionTimeout; }
inline const QString& Settings::preferredSubnets() const { return m_preferredSubnets; }
inline bool Settings::useIPv6() const { return m_useIPv6; }
inline const QHostAddress& Settings::multicastGroupAddress() const { return m_multicastGroupAddress; }
inline void Settings::setUseReturnToSendMessage( bool new_value ) { m_useReturnToSendMessage = new_value; }
inline bool Settings::useReturnToSendMessage() const { return m_useReturnToSendMessage; }
inline void Settings::setUseSpellChecker( bool new_value ) { m_useSpellChecker = new_value; }
inline bool Settings::useSpellChecker() const { return m_useSpellChecker; }
inline void Settings::setDictionaryPath( const QString& new_value ) { m_dictionaryPath = new_value; }
inline const QString& Settings::dictionaryPath() const { return m_dictionaryPath; }

#endif // BEEBEEP_SETTINGS_H
