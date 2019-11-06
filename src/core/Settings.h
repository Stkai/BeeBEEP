//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2019 Marco Mastroddi
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

  inline const QString& currentFilePath() const;
  void setDefaultFolders();
  bool enableSaveData() const;
  inline bool rcFileExists() const;

  QString currentSettingsFilePath() const;
  QString operatingSystem( bool use_long_name ) const;
  QString operatingSystemIconPath() const;
  QString accountNameFromSystemEnvinroment() const;
  void createLocalUser( const QString& user_name );
  QString createLocalUserHash();
  bool setDataFolder();
  bool searchDataFolder();
  bool createDefaultRcFile();
  bool createDefaultHostsFile();

  inline bool firstTime() const;
  inline void setFirstTime( bool );

  enum UserRecognitionMethods { RecognizeByDefaultMethod, RecognizeByAccountAndDomain, RecognizeByAccount,
                                RecognizeByNickname, NumUserRecognitionMethods };
  void setUserRecognitionMethod( int );
  inline int userRecognitionMethod() const;
  inline bool userRecognitionUsesDefaultMethod() const;

  inline bool allowMultipleInstances() const;
  inline void setCheckNewVersionAtStartup( bool );
  inline bool checkNewVersionAtStartup() const;
  inline void setPostUsageStatistics( bool );
  inline bool postUsageStatistics() const;
  inline bool canPostUsageStatistics() const;

  inline const QDate& settingsCreationDate() const;
  inline const QString& dataFolder() const;
  inline const QString& resourceFolder() const;
  inline const QString& cacheFolder() const;
  QString defaultSettingsFilePath() const;
  QString defaultBeepFilePath();
  QString defaultPluginFolderPath() const;
  QString defaultLanguageFolderPath() const;
  QString defaultHostsFilePath() const;

  bool isDevelopmentVersion( const QString& v = QString::null ) const;
  QString version( bool build_version, bool qt_version, bool debug_info ) const;
  int protocolVersion() const;
  inline int dataStreamVersion( bool in_load_event ) const;
  QString hunspellVersion() const;
  QString programName() const;
  QString organizationName() const;
  QString organizationDomain() const;
  QString officialWebSite() const;
  QString pluginWebSite() const;
  QString checkVersionWebSite() const;
  QString donationWebSite() const;
  QString helpWebSite() const;
  QString languageWebSite() const;
  QString newsWebSite() const;
  QString httpUserAgent() const;
  QString lastVersionUrl() const;
  QString downloadWebSite() const;
  QString gaTrackingId() const;
  QString gaUrl() const;
  QString gaEventVersion() const;
  QString developerWebSite() const;
  QString tipsWebSite() const;
  QString factWebSite() const;
  QString faqWebSite() const;

  QString guiCustomListStyleSheet( const QString& background_color, const QString& background_image_path ) const;

  inline void setUseHive( bool );
  inline bool useHive() const;

  inline bool disablePrivateChats() const;
  inline bool disableFileTransfer() const;
  inline bool disableFileSharing() const;
  inline bool disableDesktopSharing() const;
  inline bool disableSendMessage() const;
  inline bool disableVoiceMessages() const;
  inline bool useCompactDataSaving() const;
  inline bool useEasyConnection() const;
  inline bool useUserFullName() const;
  inline bool appendHostNameToUserName() const;

  inline bool canAddMembersToGroup() const;
  inline bool canRemoveMembersFromGroup() const;
  inline bool disableCreateMessage() const;

  inline bool disableMenuSettings() const;

  inline bool checkUserConnectedFromDatagramIp() const;

  QString dnsRecord() const;
  inline bool useMulticastDns() const;
  inline void setUseMulticastDns( bool );

  inline const User& localUser() const;
  inline void setLocalUser( const User& );
  void setLocalUserHost( const QHostAddress&, quint16 );
  inline void setLocalUserStatus( User::Status );
  inline bool isLocalHardwareAddressToSkip( const QString& ) const;

  inline bool useIPv6() const;
  QHostAddress hostAddressToListen();
  inline const QHostAddress& multicastGroupAddress() const;
  QHostAddress defaultMulticastGroupAddress() const;
  inline void setIpMulticastTtl( int );
  inline int ipMulticastTtl() const;
  inline int defaultBroadcastPort() const;
  inline int defaultListenerPort() const;
  inline int defaultFileTransferPort() const;
  inline int pongTimeout() const;
  inline int writingTimeout() const;
  inline int fileTransferConfirmTimeout() const;
  inline int fileTransferBufferSize() const;
  inline int trayMessageTimeout() const;
  inline int tickIntervalConnectionTimeout() const;
  inline int tickIntervalCheckIdle() const;
  inline int tickIntervalCheckNetwork() const;
  inline void setMaxUsersToConnectInATick( int );
  inline int maxUsersToConnectInATick() const;
  inline void setTickIntervalBroadcasting( int );
  inline int tickIntervalBroadcasting() const;

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
  inline bool useOnlyTextEmoticons() const;

  inline bool chatWithAllUsersIsEnabled() const;
  inline QString defaultChatName() const;
  inline QString defaultChatPrivateId() const;
  inline int chatMessageHistorySize() const;
  inline const QString& chatFontColor() const;
  inline void setChatFontColor( const QString& );
  inline const QFont& chatFont() const;
  void setChatFont( const QFont& );
  inline void setDefaultChatBackgroundColor( const QString& );
  inline const QString& defaultChatBackgroundColor() const;
  inline void setChatBackgroundColor( const QString& );
  inline const QString& chatBackgroundColor() const;
  inline void setChatDefaultTextColor( const QString& );
  inline const QString& chatDefaultTextColor() const;
  inline void setChatSystemTextColor( const QString& );
  inline const QString& chatSystemTextColor() const;
  inline bool chatCompact() const;
  inline void setChatCompact( bool );
  inline bool chatShowMessageTimestamp() const;
  inline void setChatShowMessageTimestamp( bool );
  inline bool homeShowMessageTimestamp() const;
  inline void setHomeShowMessageTimestamp( bool );
  inline void setHomeBackgroundColor( const QString& );
  inline const QString& homeBackgroundColor() const;
  inline void setUserListBackgroundColor( const QString& );
  inline const QString& userListBackgroundColor() const;
  inline void setChatListBackgroundColor( const QString& );
  inline const QString& chatListBackgroundColor() const;
  inline void setGroupListBackgroundColor( const QString& );
  inline const QString& groupListBackgroundColor() const;
  inline void setSavedChatListBackgroundColor( const QString& );
  inline const QString& savedChatListBackgroundColor() const;
  inline void setChatUseHtmlTags( bool );
  inline bool chatUseHtmlTags() const;
  inline void setChatUseClickableLinks( bool );
  inline bool chatUseClickableLinks() const;
  inline void setChatMessageFilter( const QBitArray& );
  inline const QBitArray& chatMessageFilter() const;
  inline bool showOnlyOnlineUsers() const;
  inline void setShowOnlyOnlineUsers( bool );
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
  inline bool raiseMainWindowOnNewMessageArrived() const;
  inline void setRaiseMainWindowOnNewMessageArrived( bool );
  inline bool alwaysShowFileTransferProgress() const;
  inline void setalwaysShowFileTransferProgress( bool );
  inline bool alwaysOpenChatOnNewMessageArrived() const;
  inline void setAlwaysOpenChatOnNewMessageArrived( bool );
  inline bool showChatToolbar() const;
  inline void setShowChatToolbar( bool );
  inline bool showOnlyMessagesInDefaultChat() const;
  inline void setShowOnlyMessagesInDefaultChat( bool );
  inline bool showVCardOnRightClick() const;
  inline void setShowVCardOnRightClick( bool );
  int defaultChatMessagesToShow() const;
  inline int chatMessagesToShow() const;
  inline void setChatMessagesToShow( int );
  inline const QSize& mainBarIconSize() const;
  inline void setAvatarIconSize( const QSize& );
  inline const QSize& avatarIconSize() const;
  inline void setGuiGeometry( const QByteArray& );
  inline const QByteArray& guiGeometry() const;
  inline void setGuiState( const QByteArray& );
  inline const QByteArray& guiState() const;
  inline void setShowEmoticonMenu( bool );
  inline bool showEmoticonMenu() const;
  inline void setShowPresetMessages( bool );
  inline bool showPresetMessages() const;
  inline bool isNotificationDisabledForGroup( const QString& ) const;
  void setNotificationEnabledForGroup( const QString&, bool );
  inline void setUseNativeEmoticons( bool );
  inline bool useNativeEmoticons() const;
  inline int imagePreviewHeight() const;
  inline void setFloatingChatGeometry( const QByteArray& );
  inline void setFloatingChatState( const QByteArray& );
  inline void setFloatingChatSplitterState( const QByteArray& );
  inline const QByteArray& floatingChatGeometry() const;
  inline const QByteArray& floatingChatState() const;
  inline const QByteArray& floatingChatSplitterState() const;
  inline void setCreateMessageGeometry( const QByteArray& );
  inline const QByteArray& createMessageGeometry() const;
  inline void setFileSharingGeometry( const QByteArray& );
  inline const QByteArray& fileSharingGeometry() const;
  inline void setShowUserStatusBackgroundColor( bool );
  inline bool showUserStatusBackgroundColor() const;
  inline void setShowUserStatusDescription( bool );
  inline bool showUserStatusDescription() const;
  inline void setUseReturnToSendMessage( bool );
  inline bool useReturnToSendMessage() const;
  inline void setChatUseYourNameInsteadOfYou( bool );
  inline bool chatUseYourNameInsteadOfYou() const;
  inline void setChatClearAllReadMessages( bool );
  inline bool chatClearAllReadMessages() const;
  inline void setUserSortingMode( int );
  inline int userSortingMode() const;
  inline void setSortUsersAscending( bool );
  inline bool sortUsersAscending() const;
  inline void setShowChatsInOneWindow( bool );
  inline bool showChatsInOneWindow() const;
  inline void setShowChatToolbar();
  inline void setChatUseColoredUserNames( bool );
  inline bool chatUseColoredUserNames() const;
  inline const QString& chatDefaultUserNameColor() const;
  inline void setSaveGeometryOnExit( bool );
  inline bool saveGeometryOnExit() const;
  inline int maxChatsToOpenAfterSendingMessage() const;
  inline void setShowUsersOnConnection( bool );
  inline bool showUsersOnConnection() const;
  inline void setShowChatsOnConnection( bool );
  inline bool showChatsOnConnection() const;
  inline void setHideEmptyChatsInList( bool );
  inline bool hideEmptyChatsInList() const;
  inline void setEnableMaximizeButton( bool );
  inline bool enableMaximizeButton() const;
  inline int chatActiveWindowOpacityLevel() const;
  inline void setChatInactiveWindowOpacityLevel( int );
  inline int chatInactiveWindowOpacityLevel() const;
  inline int chatInactiveWindowDefaultOpacityLevel() const;
  inline void setEnableDefaultChatNotifications( bool );
  inline bool enableDefaultChatNotifications() const;
  inline void setDownloadInUserFolder( bool );
  inline bool downloadInUserFolder() const;
  inline void setShowUsersInWorkgroups( bool );
  inline bool showUsersInWorkgroups() const;
  inline void setOpenChatWhenSendNewMessage( bool );
  inline bool openChatWhenSendNewMessage() const;
  inline void setSendNewMessageIndividually( bool );
  inline bool sendNewMessageIndividually() const;
  inline bool usePreviewFileDialog() const;
  inline void setPreviewFileDialogGeometry( const QByteArray& );
  inline const QByteArray& previewFileDialogGeometry();
  inline int previewFileDialogImageSize() const;
  inline void setChatQuoteBackgroundColor( const QString& );
  inline const QString& chatQuoteBackgroundColor() const;
  inline void setChatQuoteTextColor( const QString& );
  inline const QString& chatQuoteTextColor() const;
  enum ChatOnSendingMessageTypes { SkipOnSendingMessage, MinimizeChatOnSendingMessage,
                                   CloseChatOnSendingMessage, NumChatOnSendingMessageTypes };
  inline void setChatOnSendingMessage( int );
  inline int chatOnSendingMessage() const;

  inline bool allowEditNickname() const;

  QByteArray hash( const QString& ) const;
  QString currentHash() const;
  QString simpleHash( const QString& ) const;

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

  inline void setAskChangeUserAtStartup( bool );
  inline bool askChangeUserAtStartup() const;

  inline bool logToFile() const;
  inline void setLogToFile( bool );
  inline int maxLogLines() const;

  inline const QString& language() const;
  inline void setLanguage( const QString& );

  inline const QString& lastDirectorySelected() const;
  inline void setLastDirectorySelected( const QString& );
  void setLastDirectorySelectedFromFile( const QString & );

  inline const QString& downloadDirectory() const;
  inline void setDownloadDirectory( const QString& );
  QString downloadDirectoryForUser( const User& ) const;

  inline bool beepOnNewMessageArrived() const;
  inline void setBeepOnNewMessageArrived( bool );
  inline const QString& beepFilePath() const;
  inline void setBeepFilePath( const QString& );
  inline bool disableBeepInUserStatusBusy() const;
  inline void setDisableBeepInUserStatusBusy( bool );
  inline bool beepInActiveWindowAlso() const;
  inline void setBeepInActiveWindowAlso( bool );

  inline bool automaticFileName() const;
  inline void setAutomaticFileName( bool );
  inline bool overwriteExistingFiles() const;
  inline void setOverwriteExistingFiles( bool );
  inline bool resumeFileTransfer() const;
  inline void setResumeFileTransfer( bool );

  inline const QStringList& broadcastAddressesInFileHosts() const;
  inline const QHostAddress& localHostAddressForced() const;
  inline void setLocalSubnetForced( const QString& );
  inline const QString& localSubnetForced() const;
  inline bool broadcastOnlyToHostsIni() const;
  inline bool useOnlyMulticast() const;
  inline void setPreventMultipleConnectionsFromSingleHostAddress( bool );
  inline bool preventMultipleConnectionsFromSingleHostAddress();
  inline const QString& preferredSubnets() const;
  inline void setDisableSystemProxyForConnections( bool );
  inline bool disableSystemProxyForConnections() const;
  inline void setUseDefaultMulticastGroupAddress( bool );
  inline bool useDefaultMulticastGroupAddress() const;
  inline void setBroadcastToOfflineUsers( bool );
  inline bool broadcastToOfflineUsers() const;
  inline void setBroadcastToLocalSubnetAlways( bool );
  inline bool broadcastToLocalSubnetAlways() const;

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
  inline void setShowChatMessageOnTray( bool );
  inline bool showChatMessageOnTray() const;
  inline int textSizeInChatMessagePreviewOnTray() const;
  inline void setShowFileTransferCompletedOnTray( bool );
  inline bool showFileTransferCompletedOnTray() const;

  inline const QStringList& localShare() const;
  inline void setLocalShare( const QStringList& );
  inline bool hasLocalSharePath( const QString& ) const;
  inline bool enableFileTransfer() const;
  inline void setEnableFileTransfer( bool );
  inline bool enableFileSharing() const;
  inline void setEnableFileSharing( bool );

  inline int maxFileShared() const;

  QString savedChatsFilePath() const;
  inline bool chatAutoSave() const;
  inline void setChatAutoSave( bool );
  inline int chatMaxLineSaved() const;
  inline void setChatMaxLineSaved( int );
  QString unsentMessagesFilePath() const;
  inline bool chatSaveUnsentMessages() const;
  inline void setChatSaveUnsentMessages( bool );
  inline bool chatSaveFileTransfers() const;
  inline void setChatSaveFileTransfers( bool );
  inline bool chatSaveSystemMessages() const;
  inline void setChatSaveSystemMessages( bool );

  inline bool autoUserAway() const;
  inline void setAutoUserAway( bool );

  inline bool confirmOnDownloadFile() const;
  inline void setConfirmOnDownloadFile( bool );
  inline int maxSimultaneousDownloads() const;
  inline void setMaxQueuedDownloads( int );
  inline int maxQueuedDownloads() const;

  inline QStringList pluginSettings( const QString& ) const;
  inline void setPluginSettings( const QString&, const QStringList& );
  inline bool pluginHasSettings( const QString& ) const;

  void addTemporaryFilePath( const QString& );
  void clearTemporaryFiles();

  bool addStartOnSystemBoot();
  bool removeStartOnSystemBoot();
  bool hasStartOnSystemBoot() const;

  inline void setSaveGroupList( bool );
  inline bool saveGroupList() const;
  inline void setGroupList( const QStringList& );
  inline const QStringList& groupList() const;

  inline void setNetworkAddressList( const QStringList& );
  inline const QStringList& networkAddressList() const;
  inline void setUserStatusList( const QStringList& );
  inline const QStringList& userStatusList() const;
  inline int maxUserStatusDescriptionInList() const;
  inline void setMaxDaysOfUserInactivity( int );
  inline int maxDaysOfUserInactivity() const;
  inline void setRemoveInactiveUsers( bool );
  inline bool removeInactiveUsers() const;

  inline int clearCacheAfterDays() const;
  inline void setClearCacheAfterDays( int );
  inline int removePartiallyDownloadedFilesAfterDays() const;
  inline void setRemovePartiallyDownloadedFilesAfterDays( int );
  QString partiallyDownloadedFileExtension() const;

  inline void setSaveUserList( bool );
  inline bool saveUserList() const;
  inline void setUserList( const QStringList& );
  inline const QStringList& userList() const;

  inline void setAcceptConnectionsOnlyFromWorkgroups( bool );
  inline bool acceptConnectionsOnlyFromWorkgroups() const;

  inline void setShortcuts( const QStringList& );
  inline const QStringList& shortcuts() const;
  inline void setUseShortcuts( bool );
  inline bool useShortcuts() const;

  inline void setResetGeometryAtStartup( bool );
  inline bool resetGeometryAtStartup() const;
  inline void setShowMinimizedAtStartup( bool );
  inline bool showMinimizedAtStartup() const;

  inline void setPromptOnCloseEvent( bool );
  inline bool promptOnCloseEvent() const;

  inline void setUseSpellChecker( bool );
  inline bool useSpellChecker() const;
  inline void setUseWordCompleter( bool );
  inline bool useWordCompleter() const;
  inline void setDictionaryPath( const QString& );
  inline const QString& dictionaryPath() const;

  void createApplicationUuid();
  inline const QString& applicationUuid() const;
  inline void setStatsPostDate( const QDate& );
  inline const QDate& statsPostDate() const;

  inline void setUseShareBox( bool );
  inline bool useShareBox() const;
  inline void setShareBoxPath( const QString& );
  inline const QString& shareBoxPath() const;

  inline void setPresetMessages( const QMap<QString,QVariant>& );
  inline const QMap<QString,QVariant>& presetMessages() const;

  inline void setShowTextInModeRTL( bool );
  inline bool showTextInModeRTL() const;

  inline void setPlayBuzzSound( bool );
  inline bool playBuzzSound() const;

  inline void setUseMessageTimestampWithAP( bool );
  inline bool useMessageTimestampWithAP() const;

  inline void setVoiceMessageMaxDuration( int );
  inline int voiceMessageMaxDuration() const;
  inline void setUseVoicePlayer( bool );
  inline bool useVoicePlayer() const;
  inline void setVoiceInputDeviceName( const QString& );
  inline const QString& voiceInputDeviceName() const;
  inline void setVoiceFileMessageContainer( const QString& );
  inline const QString& voiceFileMessageContainer() const;
  inline void setVoiceCodec( const QString& );
  inline const QString& voiceCodec() const;
  inline void setVoiceSampleRate( int );
  inline int voiceSampleRate() const;
  inline void setVoiceBitRate( int );
  inline int voiceBitRate() const;
  inline void setVoiceChannels( int );
  inline int voiceChannels() const;
  inline void setVoiceEncodingMode( int );
  inline int voiceEncodingMode() const;
  inline void setVoiceEncodingQuality( int );
  inline int voiceEncodingQuality() const;
  inline void setUseCustomVoiceEncoderSettings( bool );
  inline bool useCustomVoiceEncoderSettings() const;
  inline void setUseSystemVoiceEncoderSettings( bool );
  inline bool useSystemVoiceEncoderSettings() const;

  void loadRcFile();
  void clearNativeSettings();
  void load();
  void save();
  inline const QDateTime& lastSave() const;

  inline bool useNativeDialogs() const;
  inline void setUseNativeDialogs( bool );

  inline bool useLowDelayOptionOnSocket() const;

  inline void setRefusedChats( const QStringList& );
  inline const QStringList& refusedChats() const;

  inline void setIconSourcePath( const QString& );
  inline const QString& iconSourcePath() const;
  inline void setEmoticonSourcePath( const QString& );
  inline const QString& emoticonSourcePath() const;

  inline void setEnableShareDesktop( bool );
  inline bool enableShareDesktop() const;
  inline void setShareDesktopCaptureDelay( int );
  inline int shareDesktopCaptureDelay() const;
  inline void setShareDesktopFitToScreen( bool );
  inline bool shareDesktopFitToScreen() const;
  inline void setShareDesktopImageType( const QString& );
  inline const QString& shareDesktopImageType() const;
  inline void setShareDesktopImageQuality( int );
  inline int shareDesktopImageQuality() const;

  inline void setDelayConnectionAtStartup( int );
  inline int delayConnectionAtStartup() const;

  inline void setSendOfflineMessagesToDefaultChat( bool );
  inline bool sendOfflineMessagesToDefaultChat() const;

  inline void setUseDarkStyle( bool );
  inline bool useDarkStyle() const;
  inline QString defaultListBackgroundColor() const;
  inline QString defaultSystemBackgroundColor() const;

  void resetAllColors();

  QString autoresponderName() const;
  inline void setSaveMessagesTimestamp( const QDateTime& );
  inline const QDateTime& saveMessagesTimestamp() const;

  QNetworkProxy systemNetworkProxy( const QNetworkProxyQuery& ) const;

  inline bool disableConnectionSocketEncryption() const;
  inline bool allowNotEncryptedConnectionsAlso() const;
  inline bool allowEncryptedConnectionsAlso() const;
  inline bool disableConnectionSocketDataCompression() const;

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
      mp_instance = Q_NULLPTR;
    }
  }

protected:
  Settings();

  QString qtMajorVersion() const;
  QString qtMajorMinorVersion() const;

  void loadBroadcastAddressesFromFileHosts();
  QHostAddress subnetFromHostAddress( const QHostAddress& ) const;
  QString checkFilePath( const QString& file_path, const QString& default_value );
  QString checkFolderPath( const QString& folder_path, const QString& default_value );
  QString defaultCacheFolderPath() const;
  bool isFileImageInCache( const QString& ) const;
  QString findFileInFolders( const QString&, const QStringList&, bool return_folder_path = false ) const;
  QStringList resourceFolders() const;
  QStringList dataFolders() const;

private:
  QString m_currentFilePath;
  QString m_resourceFolder;
  QString m_dataFolder;
  QString m_cacheFolder;

  // RC
  bool m_useSettingsFileIni;
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
  bool m_checkUserConnectedFromDatagramIp;

  bool m_useIPv6;
  QHostAddress m_multicastGroupAddress;
  bool m_useChatWithAllUsers;
  bool m_useHive;
  bool m_enableSaveData;
  bool m_useHostnameForDefaultUsername;
  bool m_disableFileTransfer;
  bool m_disableSendMessage;
  bool m_useEasyConnection;
  bool m_startMinimized;
  QString m_signature;
  bool m_useOnlyTextEmoticons;
  bool m_disablePrivateChats;
  bool m_disableFileSharing;
  bool m_disableDesktopSharing;
  int m_userRecognitionMethod;
  bool m_useOnlyMulticast;

  bool m_canAddMembersToGroup;
  bool m_canRemoveMembersFromGroup;
  bool m_allowEditNickname;
  bool m_disableCreateMessage;
  bool m_disableMenuSettings;
  bool m_disableVoiceMessages;
  bool m_useCompactDataSaving;
  bool m_useUserFullName;
  bool m_appendHostNameToUserName;

  bool m_disableConnectionSocketEncryption;
  bool m_allowNotEncryptedConnectionsAlso;
  bool m_allowEncryptedConnectionsAlso;
  bool m_disableConnectionSocketDataCompression;

  QStringList m_skipLocalHardwareAddresses;

  bool m_rcFileExists;

  // Ini
  bool m_firstTime;
  QDate m_settingsCreationDate;
  int m_settingsVersion;

  User m_localUser;
  int m_pongTimeout;
  int m_writingTimeout;
  int m_fileTransferConfirmTimeout;
  int m_fileTransferBufferSize;
  int m_trayMessageTimeout;
  int m_userAwayTimeout;
  int m_tickIntervalConnectionTimeout;
  int m_tickIntervalCheckIdle;
  int m_tickIntervalCheckNetwork;
  int m_maxUsersToConnectInATick;
  int m_tickIntervalBroadcasting;

  bool m_disableSystemProxyForConnections;
  bool m_useDefaultMulticastGroupAddress;
  int m_ipMulticastTtl;
  bool m_broadcastToOfflineUsers;
  bool m_broadcastToLocalSubnetAlways;

  QDateTime m_lastSave;
  QString m_logPath;
  QString m_pluginPath;
  QString m_languagePath;
  QString m_lastDirectorySelected;
  QString m_downloadDirectory;

  bool m_logToFile;
  int m_maxLogLines;

  int m_chatMessageHistorySize;
  QString m_chatFontColor;
  QFont m_chatFont;
  QString m_defaultChatBackgroundColor;
  QString m_chatBackgroundColor;
  QString m_chatDefaultTextColor;
  QString m_chatSystemTextColor;
  bool m_chatCompact;
  bool m_chatShowMessageTimestamp;
  bool m_chatShowMessageDatestamp;
  bool m_chatUseHtmlTags;
  bool m_chatUseClickableLinks;
  QBitArray m_chatMessageFilter;
  bool m_showOnlyOnlineUsers;
  bool m_showEmoticons;
  bool m_showMessagesGroupByUser;
  bool m_stayOnTop;
  bool m_alwaysOpenChatOnNewMessageArrived;
  bool m_raiseOnNewMessageArrived;
  bool m_raiseMainWindowOnNewMessageArrived;
  bool m_alwaysShowFileTransferProgress;
  bool m_showUserPhoto;
  bool m_automaticFileName;
  bool m_overwriteExistingFiles;
  bool m_resumeFileTransfer;
  bool m_showChatToolbar;
  bool m_showOnlyMessagesInDefaultChat;
  bool m_showVCardOnRightClick;
  int m_chatMessagesToShow;
  bool m_showEmoticonMenu;
  bool m_useNativeEmoticons;
  int m_imagePreviewHeight;
  bool m_showUserStatusBackgroundColor;
  bool m_showUserStatusDescription;
  bool m_useReturnToSendMessage;
  bool m_chatUseYourNameInsteadOfYou;
  bool m_chatClearAllReadMessages;
  int m_userSortingMode;  //  0 priority( default), 1 by name, 2 by status, 3 by messages
  bool m_sortUsersAscending;
  bool m_showPresetMessages;
  bool m_showTextInModeRTL;
  bool m_playBuzzSound;
  bool m_showChatsInOneWindow;
  bool m_chatUseColoredUserNames;
  QString m_chatDefaultUserNameColor;
  int m_maxChatsToOpenAfterSendingMessage;
  bool m_showUsersOnConnection;
  bool m_showChatsOnConnection;
  bool m_enableMaximizeButton;
  int m_chatActiveWindowOpacityLevel;
  int m_chatInactiveWindowOpacityLevel;
  bool m_enableDefaultChatNotifications;
  bool m_downloadInUserFolder;
  bool m_showUsersInWorkgroups;
  bool m_openChatWhenSendNewMessage;
  bool m_sendNewMessageIndividually;
  QString m_chatQuoteBackgroundColor;
  QString m_chatQuoteTextColor;
  int m_chatOnSendingMessage;

  QByteArray m_guiGeometry;
  QByteArray m_guiState;
  QSize m_mainBarIconSize;
  QSize m_avatarIconSize;
  int m_emoticonSizeInEdit;
  int m_emoticonSizeInChat;
  int m_emoticonSizeInMenu;
  int m_emoticonInRecentMenu;
  QStringList m_recentEmoticons;

  bool m_usePreviewFileDialog;
  QByteArray m_previewFileDialogGeometry;
  int m_previewFileDialogImageSize;

  QByteArray m_floatingChatGeometry;
  QByteArray m_floatingChatState;
  QByteArray m_floatingChatSplitterState;

  QByteArray m_createMessageGeometry;
  QByteArray m_fileSharingGeometry;

  QString m_passwordBeforeHash;
  QByteArray m_password;
  bool m_useDefaultPassword;
  bool m_savePassword;
  bool m_askPasswordAtStartup;
  bool m_askChangeUserAtStartup;
  QString m_language;
  bool m_beepOnNewMessageArrived;
  QString m_beepFilePath;
  QString m_beepDefaultFilePath;
  bool m_disableBeepInUserStatusBusy;
  bool m_beepInActiveWindowAlso;
  QStringList m_broadcastAddressesInFileHosts;
  QHostAddress m_localHostAddressForced;
  QString m_localSubnetForced;
  bool m_useMulticastDns;
  bool m_preventMultipleConnectionsFromSingleHostAddress;

  bool m_keyEscapeMinimizeInTray;
  bool m_minimizeInTray;
  bool m_loadOnTrayAtStartup;
  bool m_showNotificationOnTray;
  bool m_showOnlyMessageNotificationOnTray;
  bool m_showChatMessageOnTray;
  int m_textSizeInChatMessagePreviewOnTray;
  bool m_showFileTransferCompletedOnTray;

  bool m_enableFileTransfer;
  bool m_enableFileSharing;
  QStringList m_localShare;
  int m_maxFileShared;

  bool m_chatAutoSave;
  int m_chatMaxLineSaved;
  bool m_chatSaveUnsentMessages;
  bool m_chatSaveFileTransfers;
  bool m_chatSaveSystemMessages;

  bool m_autoUserAway;

  QMap<QString, QStringList> m_pluginSettings;

  QStringList m_tempFilePathList;

  int m_dataStreamVersion;
  bool m_useLowDelayOptionOnSocket;

  bool m_confirmOnDownloadFile;
  int m_maxSimultaneousDownloads;
  int m_maxQueuedDownloads;

  bool m_saveGroupList;
  QStringList m_groupList;
  QStringList m_networkAddressList;
  QStringList m_groupSilenced;
  QStringList m_userStatusList;
  int m_maxUserStatusDescriptionInList;
  int m_maxDaysOfUserInactivity;
  bool m_removeInactiveUsers;

  int m_clearCacheAfterDays;
  int m_removePartiallyDownloadedFilesAfterDays;

  bool m_saveUserList;
  QStringList m_userList;

  bool m_resetGeometryAtStartup;
  bool m_saveGeometryOnExit;
  bool m_showMinimizedAtStartup;
  bool m_promptOnCloseEvent;

  bool m_acceptConnectionsOnlyFromWorkgroups;

  QStringList m_shortcuts;
  bool m_useShortcuts;

  bool m_useSpellChecker;
  bool m_useWordCompleter;
  QString m_dictionaryPath;

  bool m_checkNewVersionAtStartup;
  bool m_useNativeDialogs;
  bool m_postUsageStatistics;

  bool m_homeShowMessageTimestamp;
  QString m_homeBackgroundColor;
  QString m_userListBackgroundColor;
  QString m_chatListBackgroundColor;
  QString m_groupListBackgroundColor;
  QString m_savedChatListBackgroundColor;

  QString m_applicationUuid;
  QDate m_applicationUuidCreationDate;
  QDate m_statsPostDate;

  bool m_useShareBox;
  QString m_shareBoxPath;

  QMap<QString,QVariant> m_presetMessages;
  QStringList m_refusedChats;

  QString m_iconSourcePath;
  QString m_emoticonSourcePath;

  bool m_enableShareDesktop;
  int m_shareDesktopCaptureDelay;
  bool m_shareDesktopFitToScreen;
  QString m_shareDesktopImageType;
  int m_shareDesktopImageQuality;

  int m_delayConnectionAtStartup;

  bool m_sendOfflineMessagesToDefaultChat;
  bool m_useMessageTimestampWithAP;
  bool m_hideEmptyChatsInList;

  bool m_useDarkStyle;

  QDateTime m_saveMessagesTimestamp;

  int m_voiceMessageMaxDuration;
  bool m_useVoicePlayer;
  QString m_voiceInputDeviceName;
  QString m_voiceFileMessageContainer;
  QString m_voiceCodec;
  int m_voiceSampleRate;
  int m_voiceBitRate;
  int m_voiceChannels;
  int m_voiceEncodingMode;
  int m_voiceEncodingQuality;
  bool m_useCustomVoiceEncoderSettings;
  bool m_useSystemVoiceEncoderSettings;

};


// Inline Functions
inline const QString& Settings::currentFilePath() const { return m_currentFilePath; }
inline bool Settings::enableSaveData() const { return m_enableSaveData; }
inline const QString& Settings::resourceFolder() const { return m_resourceFolder; }
inline const QString& Settings::dataFolder() const { return m_dataFolder; }
inline const QString& Settings::cacheFolder() const { return m_cacheFolder; }
inline const User& Settings::localUser() const { return m_localUser; }
inline void Settings::setLocalUser( const User& new_value ) { m_localUser = new_value; }
inline void Settings::setLocalUserStatus( User::Status new_value ) { m_localUser.setStatus( new_value ); }
inline int Settings::userRecognitionMethod() const { return m_userRecognitionMethod == RecognizeByDefaultMethod ? RecognizeByNickname : m_userRecognitionMethod; }
inline bool Settings::userRecognitionUsesDefaultMethod() const { return m_userRecognitionMethod == RecognizeByDefaultMethod; }
inline bool Settings::chatWithAllUsersIsEnabled() const { return m_useChatWithAllUsers; }
inline bool Settings::allowMultipleInstances() const { return m_allowMultipleInstances; }
inline void Settings::setUseHive( bool new_value ) { m_useHive = new_value; }
inline bool Settings::useHive() const { return m_useHive; }
inline int Settings::defaultBroadcastPort() const { return m_defaultBroadcastPort; }
inline int Settings::defaultListenerPort() const { return m_defaultListenerPort; }
inline int Settings::defaultFileTransferPort() const { return m_defaultFileTransferPort; }
inline int Settings::pongTimeout() const { return m_pongTimeout; }
inline int Settings::writingTimeout() const { return m_writingTimeout; }
inline int Settings::fileTransferConfirmTimeout() const { return m_fileTransferConfirmTimeout; }
inline int Settings::fileTransferBufferSize() const { return m_fileTransferBufferSize; }
inline int Settings::trayMessageTimeout() const  { return m_trayMessageTimeout; }
inline int Settings::userAwayTimeout() const { return m_userAwayTimeout; }
inline void Settings::setUserAwayTimeout( int new_value ) { m_userAwayTimeout = new_value; }
inline void Settings::setMaxUsersToConnectInATick( int new_value ) { m_maxUsersToConnectInATick = new_value; }
inline int Settings::maxUsersToConnectInATick() const { return m_maxUsersToConnectInATick; }
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
inline QString Settings::defaultChatPrivateId() const { return QString::fromLatin1( QCryptographicHash::hash( defaultChatName().toLatin1() , QCryptographicHash::Sha1 ).toHex() ); }
inline int Settings::chatMessageHistorySize() const { return m_chatMessageHistorySize; }
inline const QString& Settings::chatFontColor() const { return m_chatFontColor; }
inline void Settings::setChatFontColor( const QString& new_value ) { m_chatFontColor = new_value; }
inline const QFont& Settings::chatFont() const { return m_chatFont; }
inline void Settings::setDefaultChatBackgroundColor( const QString& new_value ) { m_defaultChatBackgroundColor = new_value; }
inline const QString& Settings::defaultChatBackgroundColor() const { return m_defaultChatBackgroundColor; }
inline bool Settings::chatCompact() const { return m_chatCompact; }
inline void Settings::setChatCompact( bool new_value ) { m_chatCompact = new_value; }
inline bool Settings::chatShowMessageTimestamp() const { return m_chatShowMessageTimestamp; }
inline void Settings::setChatShowMessageTimestamp( bool new_value ) { m_chatShowMessageTimestamp = new_value; }
inline bool Settings::homeShowMessageTimestamp() const { return m_homeShowMessageTimestamp; }
inline void Settings::setHomeShowMessageTimestamp( bool new_value ) { m_homeShowMessageTimestamp = new_value; }
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
inline bool Settings::useUserFullName() const { return m_useUserFullName; }
inline bool Settings::appendHostNameToUserName() const { return m_appendHostNameToUserName; }
inline bool Settings::showEmoticons() const { return m_showEmoticons; }
inline void Settings::setShowEmoticons( bool new_value ) { m_showEmoticons = new_value; }
inline void Settings::setShowEmoticonMenu(bool new_value ) { m_showEmoticonMenu = new_value; }
inline bool Settings::showEmoticonMenu() const { return m_showEmoticonMenu; }
inline bool Settings::useOnlyTextEmoticons() const { return m_useOnlyTextEmoticons; }
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
inline void Settings::setAskChangeUserAtStartup( bool new_value ) { m_askChangeUserAtStartup = new_value; }
inline bool Settings::askChangeUserAtStartup() const { return m_askChangeUserAtStartup; }
inline bool Settings::logToFile() const { return m_logToFile; }
inline void Settings::setLogToFile( bool new_value ) { m_logToFile = new_value; }
inline int Settings::maxLogLines() const { return m_maxLogLines; }
inline const QString& Settings::language() const { return m_language; }
inline void Settings::setLanguage( const QString& new_value ) { m_language = new_value; }
inline const QString& Settings::lastDirectorySelected() const { return m_lastDirectorySelected; }
inline void Settings::setLastDirectorySelected( const QString& new_value ) { m_lastDirectorySelected = new_value; }
inline const QString& Settings::downloadDirectory() const { return m_downloadDirectory; }
inline void Settings::setDownloadDirectory( const QString& new_value ) { m_downloadDirectory = new_value; }
inline bool Settings::beepOnNewMessageArrived() const { return m_beepOnNewMessageArrived; }
inline void Settings::setBeepOnNewMessageArrived( bool new_value ) { m_beepOnNewMessageArrived = new_value; }
inline const QString& Settings::beepFilePath() const { return m_beepFilePath; }
inline void Settings::setBeepFilePath( const QString& new_value ) { m_beepFilePath = new_value; }
inline bool Settings::showVCardOnRightClick() const { return m_showVCardOnRightClick; }
inline void Settings::setShowVCardOnRightClick( bool new_value ) { m_showVCardOnRightClick = new_value; }
inline bool Settings::showMessagesGroupByUser() const { return m_showMessagesGroupByUser; }
inline void Settings::setShowMessagesGroupByUser( bool new_value ) { m_showMessagesGroupByUser = new_value; }
inline bool Settings::stayOnTop() const { return m_stayOnTop; }
inline void Settings::setStayOnTop( bool new_value ) { m_stayOnTop = new_value; }
inline bool Settings::raiseOnNewMessageArrived() const { return m_raiseOnNewMessageArrived; }
inline void Settings::setRaiseOnNewMessageArrived( bool new_value ) { m_raiseOnNewMessageArrived = new_value; }
inline bool Settings::raiseMainWindowOnNewMessageArrived() const { return m_raiseMainWindowOnNewMessageArrived; }
inline void Settings::setRaiseMainWindowOnNewMessageArrived( bool new_value ) { m_raiseMainWindowOnNewMessageArrived = new_value; }
inline bool Settings::alwaysShowFileTransferProgress() const { return m_alwaysShowFileTransferProgress; }
inline void Settings::setalwaysShowFileTransferProgress( bool new_value ) { m_alwaysShowFileTransferProgress = new_value; }
inline bool Settings::alwaysOpenChatOnNewMessageArrived() const { return m_alwaysOpenChatOnNewMessageArrived; }
inline void Settings::setAlwaysOpenChatOnNewMessageArrived( bool new_value ) { m_alwaysOpenChatOnNewMessageArrived = new_value; }
inline bool Settings::automaticFileName() const { return m_automaticFileName; }
inline void Settings::setAutomaticFileName( bool new_value ) { m_automaticFileName = new_value; }
inline bool Settings::overwriteExistingFiles() const { return m_overwriteExistingFiles; }
inline void Settings::setOverwriteExistingFiles( bool new_value ) { m_overwriteExistingFiles = new_value; }
inline bool Settings::resumeFileTransfer() const { return m_resumeFileTransfer; }
inline void Settings::setResumeFileTransfer( bool new_value ) { m_resumeFileTransfer = new_value; }
inline bool Settings::showChatToolbar() const { return m_showChatToolbar; }
inline void Settings::setShowChatToolbar( bool new_value ) { m_showChatToolbar = new_value; }
inline const QStringList& Settings::broadcastAddressesInFileHosts() const { return m_broadcastAddressesInFileHosts; }
inline const QHostAddress& Settings::localHostAddressForced() const { return m_localHostAddressForced; }
inline void Settings::setLocalSubnetForced( const QString& new_value ) { m_localSubnetForced = new_value; }
inline const QString& Settings::localSubnetForced() const { return m_localSubnetForced; }
inline bool Settings::broadcastOnlyToHostsIni() const { return m_broadcastOnlyToHostsIni; }
inline bool Settings::useOnlyMulticast() const { return m_useOnlyMulticast; }
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
inline void Settings::setShowChatMessageOnTray( bool new_value ) { m_showChatMessageOnTray = new_value; }
inline bool Settings::showChatMessageOnTray() const { return m_showChatMessageOnTray; }
inline const QStringList& Settings::localShare() const { return m_localShare; }
inline void Settings::setLocalShare( const QStringList& new_value ) { m_localShare = new_value; }
inline bool Settings::hasLocalSharePath( const QString& share_path ) const { return m_localShare.contains( share_path ); }
inline bool Settings::enableFileTransfer() const { return m_enableFileTransfer; }
inline void Settings::setEnableFileTransfer( bool new_value ) { m_enableFileTransfer = new_value; }
inline bool Settings::enableFileSharing() const { return m_enableFileSharing; }
inline void Settings::setEnableFileSharing( bool new_value ) { m_enableFileSharing = new_value; }
inline int Settings::maxFileShared() const { return m_maxFileShared; }
inline bool Settings::chatAutoSave() const { return m_chatAutoSave; }
inline void Settings::setChatAutoSave( bool new_value ) { m_chatAutoSave = new_value; }
inline int Settings::chatMaxLineSaved() const { return m_chatMaxLineSaved > 100 ? m_chatMaxLineSaved: 100; }
inline void Settings::setChatMaxLineSaved( int new_value ) { m_chatMaxLineSaved = new_value; }
inline bool Settings::chatSaveUnsentMessages() const { return m_chatSaveUnsentMessages; }
inline void Settings::setChatSaveUnsentMessages( bool new_value ) { m_chatSaveUnsentMessages = new_value; }
inline bool Settings::autoUserAway() const { return m_autoUserAway; }
inline void Settings::setAutoUserAway( bool new_value ) { m_autoUserAway = new_value; }
inline QStringList Settings::pluginSettings( const QString& plugin_name ) const { return m_pluginSettings.value( plugin_name ); }
inline void Settings::setPluginSettings( const QString& plugin_name, const QStringList& plugin_settings ) { m_pluginSettings.insert( plugin_name, plugin_settings ); }
inline bool Settings::pluginHasSettings( const QString& plugin_name ) const { return m_pluginSettings.contains( plugin_name ); }
inline int Settings::dataStreamVersion( bool in_load_event ) const { return in_load_event ? m_dataStreamVersion : LAST_DATASTREAM_VERSION; }
inline bool Settings::confirmOnDownloadFile() const { return m_confirmOnDownloadFile; }
inline void Settings::setConfirmOnDownloadFile( bool new_value ) { m_confirmOnDownloadFile = new_value; }
inline int Settings::maxSimultaneousDownloads() const { return m_maxSimultaneousDownloads; }
inline void Settings::setMaxQueuedDownloads( int new_value ) { m_maxQueuedDownloads = new_value; }
inline int Settings::maxQueuedDownloads() const { return m_maxQueuedDownloads; }
inline const QDate& Settings::settingsCreationDate() const { return m_settingsCreationDate; }
inline void Settings::setSaveGroupList( bool new_value ) { m_saveGroupList = new_value; }
inline bool Settings::saveGroupList() const { return m_saveGroupList; }
inline void Settings::setGroupList( const QStringList& new_value ) { m_groupList = new_value; }
inline const QStringList& Settings::groupList() const { return m_groupList; }
inline const QDateTime& Settings::lastSave() const { return m_lastSave; }
inline void Settings::setNetworkAddressList( const QStringList& new_value ) { m_networkAddressList = new_value; }
inline const QStringList& Settings::networkAddressList() const { return m_networkAddressList; }
inline void Settings::setUserStatusList( const QStringList& new_value ) { m_userStatusList = new_value; }
inline const QStringList& Settings::userStatusList() const { return m_userStatusList; }
inline void Settings::setSaveUserList( bool new_value ) { m_saveUserList = new_value; }
inline bool Settings::saveUserList() const { return m_saveUserList; }
inline void Settings::setUserList( const QStringList& new_value ) { m_userList = new_value; }
inline const QStringList& Settings::userList() const { return m_userList; }
inline void Settings::setSaveGeometryOnExit( bool new_value ) { m_saveGeometryOnExit = new_value; }
inline bool Settings::saveGeometryOnExit() const { return m_saveGeometryOnExit; }
inline void Settings::setResetGeometryAtStartup( bool new_value ) { m_resetGeometryAtStartup = new_value; }
inline bool Settings::resetGeometryAtStartup() const { return m_resetGeometryAtStartup; }
inline void Settings::setShowMinimizedAtStartup( bool new_value ) { m_showMinimizedAtStartup = new_value; }
inline bool Settings::showMinimizedAtStartup() const { return m_showMinimizedAtStartup; }
inline int Settings::chatMessagesToShow() const { return m_chatMessagesToShow; }
inline void Settings::setChatMessagesToShow( int new_value ) { m_chatMessagesToShow = new_value; }
inline int Settings::emoticonSizeInEdit() const { return m_emoticonSizeInEdit; }
inline int Settings::emoticonSizeInChat() const { return m_emoticonSizeInChat; }
inline int Settings::emoticonSizeInMenu() const { return m_emoticonSizeInMenu; }
inline int Settings::emoticonInRecentMenu() const { return m_emoticonInRecentMenu; }
inline void Settings::setRecentEmoticons( const QStringList& new_value ) { m_recentEmoticons = new_value; }
inline const QStringList& Settings::recentEmoticons() const { return m_recentEmoticons; }
inline bool Settings::isNotificationDisabledForGroup( const QString& group_id ) const { return group_id.isEmpty() ? false : m_groupSilenced.contains( group_id ); }
inline void Settings::setUseNativeEmoticons( bool new_value ) { m_useNativeEmoticons = new_value; }
inline bool Settings::useNativeEmoticons() const { return m_useNativeEmoticons; }
inline int Settings::imagePreviewHeight() const { return m_imagePreviewHeight; }
inline void Settings::setPromptOnCloseEvent( bool new_value ) { m_promptOnCloseEvent = new_value; }
inline bool Settings::promptOnCloseEvent() const { return m_promptOnCloseEvent; }
inline void Settings::setPreventMultipleConnectionsFromSingleHostAddress( bool new_value ) { m_preventMultipleConnectionsFromSingleHostAddress = new_value; }
inline bool Settings::preventMultipleConnectionsFromSingleHostAddress() { return m_preventMultipleConnectionsFromSingleHostAddress; }
inline void Settings::setFloatingChatGeometry( const QByteArray& new_value ) { m_floatingChatGeometry = new_value; }
inline void Settings::setFloatingChatState( const QByteArray& new_value ) { m_floatingChatState = new_value; }
inline void Settings::setFloatingChatSplitterState( const QByteArray& new_value ) { m_floatingChatSplitterState = new_value; }
inline const QByteArray& Settings::floatingChatGeometry() const { return m_floatingChatGeometry; }
inline const QByteArray& Settings::floatingChatState() const { return m_floatingChatState; }
inline const QByteArray& Settings::floatingChatSplitterState() const { return m_floatingChatSplitterState; }
inline void Settings::setCreateMessageGeometry( const QByteArray& new_value ) { m_createMessageGeometry = new_value; }
inline const QByteArray& Settings::createMessageGeometry() const { return m_createMessageGeometry; }
inline void Settings::setFileSharingGeometry( const QByteArray& new_value ) { m_fileSharingGeometry = new_value; }
inline const QByteArray& Settings::fileSharingGeometry() const { return m_fileSharingGeometry; }
inline void Settings::setAcceptConnectionsOnlyFromWorkgroups( bool new_value ) { m_acceptConnectionsOnlyFromWorkgroups = new_value; }
inline bool Settings::acceptConnectionsOnlyFromWorkgroups() const { return m_acceptConnectionsOnlyFromWorkgroups; }
inline int Settings::maxUserStatusDescriptionInList() const { return m_maxUserStatusDescriptionInList; }
inline void Settings::setMaxDaysOfUserInactivity( int new_value ) { m_maxDaysOfUserInactivity = new_value; }
inline int Settings::maxDaysOfUserInactivity() const { return m_maxDaysOfUserInactivity; }
inline void Settings::setRemoveInactiveUsers( bool new_value ) { m_removeInactiveUsers = new_value; }
inline bool Settings::removeInactiveUsers() const { return m_removeInactiveUsers; }
inline void Settings::setShowUserStatusBackgroundColor( bool new_value ) { m_showUserStatusBackgroundColor = new_value; }
inline bool Settings::showUserStatusBackgroundColor() const { return m_showUserStatusBackgroundColor; }
inline void Settings::setShowUserStatusDescription( bool new_value ) { m_showUserStatusDescription = new_value; }
inline bool Settings::showUserStatusDescription() const { return m_showUserStatusDescription; }
inline void Settings::setShortcuts( const QStringList& new_value ) { m_shortcuts = new_value; }
inline const QStringList& Settings::shortcuts() const { return m_shortcuts; }
inline void Settings::setUseShortcuts( bool new_value ) { m_useShortcuts = new_value; }
inline bool Settings::useShortcuts() const { return m_useShortcuts; }
inline int Settings::tickIntervalConnectionTimeout() const { return m_tickIntervalConnectionTimeout; }
inline const QString& Settings::preferredSubnets() const { return m_preferredSubnets; }
inline void Settings::setDisableSystemProxyForConnections( bool new_value ) { m_disableSystemProxyForConnections = new_value; }
inline bool Settings::disableSystemProxyForConnections() const { return m_disableSystemProxyForConnections; }
inline void Settings::setUseDefaultMulticastGroupAddress( bool new_value ) { m_useDefaultMulticastGroupAddress = new_value; }
inline bool Settings::useDefaultMulticastGroupAddress() const { return m_useDefaultMulticastGroupAddress; }
inline bool Settings::useIPv6() const { return m_useIPv6; }
inline const QHostAddress& Settings::multicastGroupAddress() const { return m_multicastGroupAddress; }
inline void Settings::setIpMulticastTtl( int new_value ) { m_ipMulticastTtl = new_value; }
inline int Settings::ipMulticastTtl() const { return m_ipMulticastTtl; }
inline void Settings::setUseReturnToSendMessage( bool new_value ) { m_useReturnToSendMessage = new_value; }
inline bool Settings::useReturnToSendMessage() const { return m_useReturnToSendMessage; }
inline void Settings::setUseSpellChecker( bool new_value ) { m_useSpellChecker = new_value; }
inline bool Settings::useSpellChecker() const { return m_useSpellChecker; }
inline void Settings::setDictionaryPath( const QString& new_value ) { m_dictionaryPath = new_value; }
inline const QString& Settings::dictionaryPath() const { return m_dictionaryPath; }
inline void Settings::setUseWordCompleter( bool new_value ) { m_useWordCompleter = new_value; }
inline bool Settings::useWordCompleter() const { return m_useWordCompleter; }
inline int Settings::tickIntervalCheckIdle() const { return m_tickIntervalCheckIdle; }
inline int Settings::tickIntervalCheckNetwork() const { return m_tickIntervalCheckNetwork; }
inline void Settings::setTickIntervalBroadcasting( int new_value ) { m_tickIntervalBroadcasting = new_value; }
inline int Settings::tickIntervalBroadcasting() const { return m_tickIntervalBroadcasting; }
inline void Settings::setCheckNewVersionAtStartup( bool new_value ) { m_checkNewVersionAtStartup = new_value; }
inline bool Settings::checkNewVersionAtStartup() const { return m_checkNewVersionAtStartup; }
inline void Settings::setPostUsageStatistics( bool new_value ) { m_postUsageStatistics = new_value; }
inline bool Settings::postUsageStatistics() const { return m_postUsageStatistics; }
inline void Settings::setChatUseYourNameInsteadOfYou( bool new_value ) { m_chatUseYourNameInsteadOfYou = new_value; }
inline bool Settings::chatUseYourNameInsteadOfYou() const { return m_chatUseYourNameInsteadOfYou; }
inline void Settings::setChatClearAllReadMessages( bool new_value ) { m_chatClearAllReadMessages = new_value; }
inline bool Settings::chatClearAllReadMessages() const { return m_chatClearAllReadMessages; }
inline void Settings::setUseNativeDialogs( bool new_value ) { m_useNativeDialogs = new_value; }
inline bool Settings::useNativeDialogs() const { return m_useNativeDialogs; }
inline const QString& Settings::applicationUuid() const { return m_applicationUuid; }
inline bool Settings::usePreviewFileDialog() const { return m_usePreviewFileDialog; }
inline void Settings::setPreviewFileDialogGeometry( const QByteArray& new_value ) { m_previewFileDialogGeometry = new_value; }
inline const QByteArray& Settings::previewFileDialogGeometry() { return m_previewFileDialogGeometry; }
inline int Settings::previewFileDialogImageSize() const { return m_previewFileDialogImageSize; }
inline void Settings::setStatsPostDate( const QDate& new_value ) { m_statsPostDate = new_value; }
inline const QDate& Settings::statsPostDate() const { return m_statsPostDate; }
inline bool Settings::canPostUsageStatistics() const { return m_postUsageStatistics && m_statsPostDate != QDate::currentDate(); }
inline int Settings::textSizeInChatMessagePreviewOnTray() const { return m_textSizeInChatMessagePreviewOnTray; }
inline void Settings::setShowFileTransferCompletedOnTray( bool new_value ) { m_showFileTransferCompletedOnTray = new_value; }
inline bool Settings::showFileTransferCompletedOnTray() const { return m_showFileTransferCompletedOnTray; }
inline bool Settings::useLowDelayOptionOnSocket() const { return m_useLowDelayOptionOnSocket; }
inline bool Settings::disableFileTransfer() const { return m_disableFileTransfer; }
inline bool Settings::disableFileSharing() const { return m_disableFileSharing; }
inline bool Settings::disableDesktopSharing() const { return m_disableDesktopSharing; }
inline bool Settings::disableSendMessage() const { return m_disableSendMessage; }
inline bool Settings::disableVoiceMessages() const { return m_disableFileTransfer || m_disableVoiceMessages; }
inline bool Settings::useCompactDataSaving() const { return m_useCompactDataSaving; }
inline bool Settings::useEasyConnection() const { return m_useEasyConnection; }
inline void Settings::setUseShareBox( bool new_value ) { m_useShareBox = new_value; }
inline bool Settings::useShareBox() const { return m_useShareBox; }
inline void Settings::setShareBoxPath( const QString& new_value ) { m_shareBoxPath = new_value; }
inline const QString& Settings::shareBoxPath() const { return m_shareBoxPath; }
inline void Settings::setUserSortingMode( int new_value ) { m_userSortingMode = new_value; }
inline int Settings::userSortingMode() const { return m_userSortingMode; }
inline void Settings::setSortUsersAscending( bool new_value ) { m_sortUsersAscending = new_value; }
inline bool Settings::sortUsersAscending() const { return m_sortUsersAscending; }
inline void Settings::setPresetMessages( const QMap<QString,QVariant>& new_value ) { m_presetMessages = new_value; }
inline const QMap<QString,QVariant>& Settings::presetMessages() const { return m_presetMessages; }
inline void Settings::setShowPresetMessages( bool new_value ) { m_showPresetMessages = new_value; }
inline bool Settings::showPresetMessages() const { return m_showPresetMessages; }
inline void Settings::setShowTextInModeRTL( bool new_value ) { m_showTextInModeRTL = new_value; }
inline bool Settings::showTextInModeRTL() const { return m_showTextInModeRTL; }
inline void Settings::setPlayBuzzSound( bool new_value ) { m_playBuzzSound = new_value; }
inline bool Settings::playBuzzSound() const { return m_playBuzzSound; }
inline bool Settings::disablePrivateChats() const { return m_disablePrivateChats; }
inline void Settings::setShowChatsInOneWindow( bool new_value ) { m_showChatsInOneWindow = new_value; }
inline bool Settings::showChatsInOneWindow() const { return m_showChatsInOneWindow; }
inline void Settings::setHomeBackgroundColor( const QString& new_value ) { m_homeBackgroundColor = new_value; }
inline const QString& Settings::homeBackgroundColor() const { return m_homeBackgroundColor; }
inline void Settings::setUserListBackgroundColor( const QString& new_value ) { m_userListBackgroundColor = new_value; }
inline const QString& Settings::userListBackgroundColor() const { return m_userListBackgroundColor; }
inline void Settings::setChatListBackgroundColor( const QString& new_value ) { m_chatListBackgroundColor = new_value; }
inline const QString& Settings::chatListBackgroundColor() const { return m_chatListBackgroundColor; }
inline void Settings::setGroupListBackgroundColor( const QString& new_value ) { m_groupListBackgroundColor = new_value; }
inline const QString& Settings::groupListBackgroundColor() const { return m_groupListBackgroundColor; }
inline void Settings::setSavedChatListBackgroundColor( const QString& new_value ) { m_savedChatListBackgroundColor = new_value; }
inline const QString& Settings::savedChatListBackgroundColor() const { return m_savedChatListBackgroundColor; }
inline void Settings::setRefusedChats( const QStringList& new_value ) { m_refusedChats = new_value; }
inline const QStringList& Settings::refusedChats() const { return m_refusedChats; }
inline void Settings::setIconSourcePath( const QString& new_value ) { m_iconSourcePath = new_value; }
inline const QString& Settings::iconSourcePath() const { return m_iconSourcePath; }
inline void Settings::setEmoticonSourcePath( const QString& new_value ) { m_emoticonSourcePath = new_value; }
inline const QString& Settings::emoticonSourcePath() const { return m_emoticonSourcePath; }
inline void Settings::setEnableShareDesktop( bool new_value ) { m_enableShareDesktop = new_value; }
inline bool Settings::enableShareDesktop() const { return m_enableShareDesktop; }
inline void Settings::setShareDesktopCaptureDelay( int new_value ) { m_shareDesktopCaptureDelay = new_value; }
inline int Settings::shareDesktopCaptureDelay() const { return m_shareDesktopCaptureDelay; }
inline void Settings::setShareDesktopFitToScreen( bool new_value ) { m_shareDesktopFitToScreen = new_value; }
inline bool Settings::shareDesktopFitToScreen() const { return m_shareDesktopFitToScreen; }
inline void Settings::setShareDesktopImageType( const QString& new_value ) { m_shareDesktopImageType = new_value; }
inline const QString& Settings::shareDesktopImageType() const { return m_shareDesktopImageType; }
inline void Settings::setShareDesktopImageQuality( int new_value ) { m_shareDesktopImageQuality = new_value; }
inline int Settings::shareDesktopImageQuality() const { return m_shareDesktopImageQuality; }
inline void Settings::setDelayConnectionAtStartup( int new_value ) { m_delayConnectionAtStartup = new_value; }
inline int Settings::delayConnectionAtStartup() const { return m_delayConnectionAtStartup; }
inline void Settings::setSendOfflineMessagesToDefaultChat( bool new_value ) { m_sendOfflineMessagesToDefaultChat = new_value; }
inline bool Settings::sendOfflineMessagesToDefaultChat() const { return m_sendOfflineMessagesToDefaultChat; }
inline void Settings::setChatUseColoredUserNames( bool new_value ) { m_chatUseColoredUserNames = new_value; }
inline bool Settings::chatUseColoredUserNames() const { return m_chatUseColoredUserNames; }
inline const QString& Settings::chatDefaultUserNameColor() const { return m_chatDefaultUserNameColor; }
inline int Settings::maxChatsToOpenAfterSendingMessage() const { return m_maxChatsToOpenAfterSendingMessage; }
inline void Settings::setShowUsersOnConnection( bool new_value ) { m_showUsersOnConnection = new_value; if( m_showUsersOnConnection ) m_showChatsOnConnection = false; }
inline bool Settings::showUsersOnConnection() const { return m_showUsersOnConnection; }
inline void Settings::setShowChatsOnConnection( bool new_value ) { m_showChatsOnConnection = new_value; if( m_showChatsOnConnection ) m_showUsersOnConnection = false; }
inline bool Settings::showChatsOnConnection() const { return m_showChatsOnConnection; }
inline void Settings::setHideEmptyChatsInList( bool new_value ) { m_hideEmptyChatsInList = new_value; }
inline bool Settings::hideEmptyChatsInList() const { return m_hideEmptyChatsInList; }
inline bool Settings::canAddMembersToGroup() const { return m_canAddMembersToGroup; }
inline bool Settings::canRemoveMembersFromGroup() const { return m_canRemoveMembersFromGroup; }
inline bool Settings::disableCreateMessage() const { return m_disableCreateMessage; }
inline void Settings::setEnableMaximizeButton( bool new_value ) { m_enableMaximizeButton = new_value; }
inline bool Settings::enableMaximizeButton() const { return m_enableMaximizeButton; }
inline bool Settings::allowEditNickname() const { return m_allowEditNickname; }
inline int Settings::chatActiveWindowOpacityLevel() const { return m_chatActiveWindowOpacityLevel; }
inline void Settings::setChatInactiveWindowOpacityLevel( int new_value ) { m_chatInactiveWindowOpacityLevel = new_value; }
inline int Settings::chatInactiveWindowOpacityLevel() const { return m_chatInactiveWindowOpacityLevel; }
inline int Settings::chatInactiveWindowDefaultOpacityLevel() const { return 85; }
inline void Settings::setChatBackgroundColor( const QString& new_value ) { m_chatBackgroundColor = new_value; }
inline const QString& Settings::chatBackgroundColor() const { return m_chatBackgroundColor; }
inline void Settings::setChatDefaultTextColor( const QString& new_value ) { m_chatDefaultTextColor = new_value; }
inline const QString& Settings::chatDefaultTextColor() const { return m_chatDefaultTextColor; }
inline void Settings::setChatSystemTextColor( const QString& new_value ) { m_chatSystemTextColor = new_value; }
inline const QString& Settings::chatSystemTextColor() const { return m_chatSystemTextColor; }
inline void Settings::setEnableDefaultChatNotifications( bool new_value ) { m_enableDefaultChatNotifications = new_value; }
inline bool Settings::enableDefaultChatNotifications() const { return m_enableDefaultChatNotifications; }
inline bool Settings::disableMenuSettings() const { return m_disableMenuSettings; }
inline void Settings::setUseMessageTimestampWithAP( bool new_value ) { m_useMessageTimestampWithAP = new_value; }
inline bool Settings::useMessageTimestampWithAP() const { return m_useMessageTimestampWithAP; }
inline void Settings::setBroadcastToOfflineUsers( bool new_value ) { m_broadcastToOfflineUsers = new_value; }
inline bool Settings::broadcastToOfflineUsers() const { return m_broadcastToOfflineUsers; }
inline void Settings::setBroadcastToLocalSubnetAlways( bool new_value ) { m_broadcastToLocalSubnetAlways = new_value; }
inline bool Settings::broadcastToLocalSubnetAlways() const { return m_broadcastToLocalSubnetAlways; }
inline void Settings::setUseDarkStyle( bool new_value ) { m_useDarkStyle = new_value; }
inline bool Settings::useDarkStyle() const { return m_useDarkStyle; }
inline void Settings::setSaveMessagesTimestamp( const QDateTime& new_value ) { m_saveMessagesTimestamp = new_value; }
inline const QDateTime& Settings::saveMessagesTimestamp() const { return m_saveMessagesTimestamp; }
inline bool Settings::chatSaveFileTransfers() const { return m_chatSaveFileTransfers; }
inline void Settings::setChatSaveFileTransfers( bool new_value ) { m_chatSaveFileTransfers = new_value; }
inline bool Settings::chatSaveSystemMessages() const { return m_chatSaveSystemMessages; }
inline void Settings::setChatSaveSystemMessages( bool new_value ) { m_chatSaveSystemMessages = new_value; }
inline int Settings::clearCacheAfterDays() const { return m_clearCacheAfterDays; }
inline void Settings::setClearCacheAfterDays( int new_value ) { m_clearCacheAfterDays = new_value; }
inline int Settings::removePartiallyDownloadedFilesAfterDays() const { return m_removePartiallyDownloadedFilesAfterDays; }
inline void Settings::setRemovePartiallyDownloadedFilesAfterDays( int new_value ) { m_removePartiallyDownloadedFilesAfterDays = new_value; }
inline QString Settings::defaultListBackgroundColor() const { return QLatin1String( "#ffffff" ); }
inline QString Settings::defaultSystemBackgroundColor() const { return QLatin1String( "#f5f5f5" ); }
inline void Settings::setDownloadInUserFolder( bool new_value ) { m_downloadInUserFolder = new_value; }
inline bool Settings::downloadInUserFolder() const { return m_downloadInUserFolder; }
inline void Settings::setShowUsersInWorkgroups( bool new_value ) { m_showUsersInWorkgroups = new_value; }
inline bool Settings::showUsersInWorkgroups() const { return m_showUsersInWorkgroups; }
inline void Settings::setOpenChatWhenSendNewMessage( bool new_value ) { m_openChatWhenSendNewMessage = new_value; }
inline bool Settings::openChatWhenSendNewMessage() const { return m_openChatWhenSendNewMessage; }
inline void Settings::setSendNewMessageIndividually( bool new_value ) { m_sendNewMessageIndividually = new_value; }
inline bool Settings::sendNewMessageIndividually() const { return m_sendNewMessageIndividually; }
inline void Settings::setChatQuoteBackgroundColor( const QString& new_value ) { m_chatQuoteBackgroundColor = new_value; }
inline const QString& Settings::chatQuoteBackgroundColor() const { return m_chatQuoteBackgroundColor; }
inline void Settings::setChatQuoteTextColor( const QString& new_value ) { m_chatQuoteTextColor = new_value; }
inline const QString& Settings::chatQuoteTextColor() const { return m_chatQuoteTextColor; }
inline bool Settings::checkUserConnectedFromDatagramIp() const { return m_checkUserConnectedFromDatagramIp; }
inline bool Settings::isLocalHardwareAddressToSkip( const QString& hw_value ) const { return hw_value.isEmpty() ? true : (m_skipLocalHardwareAddresses.isEmpty() ? false : m_skipLocalHardwareAddresses.contains( hw_value, Qt::CaseInsensitive )); }
inline bool Settings::rcFileExists() const { return m_rcFileExists; }
inline void Settings::setChatOnSendingMessage( int new_value ) { m_chatOnSendingMessage = new_value; }
inline int Settings::chatOnSendingMessage() const { return m_chatOnSendingMessage; }
inline void Settings::setVoiceMessageMaxDuration( int new_value ) { m_voiceMessageMaxDuration = new_value; }
inline int Settings::voiceMessageMaxDuration() const { return m_voiceMessageMaxDuration;}
inline void Settings::setUseVoicePlayer( bool new_value ) { m_useVoicePlayer = new_value; }
inline bool Settings::useVoicePlayer() const { return m_useVoicePlayer; }
inline void Settings::setVoiceInputDeviceName( const QString& new_value ) { m_voiceInputDeviceName = new_value; }
inline const QString& Settings::voiceInputDeviceName() const { return m_voiceInputDeviceName; }
inline void Settings::setVoiceFileMessageContainer( const QString& new_value ) { m_voiceFileMessageContainer = new_value; }
inline const QString& Settings::voiceFileMessageContainer() const { return m_voiceFileMessageContainer; }
inline void Settings::setVoiceCodec( const QString& new_value ) { m_voiceCodec = new_value; }
inline const QString& Settings::voiceCodec() const { return m_voiceCodec; }
inline void Settings::setVoiceSampleRate( int new_value ) { m_voiceSampleRate = new_value; }
inline int Settings::voiceSampleRate() const { return m_voiceSampleRate; }
inline void Settings::setVoiceBitRate( int new_value ) { m_voiceBitRate = new_value; }
inline int Settings::voiceBitRate() const { return m_voiceBitRate; }
inline void Settings::setVoiceChannels( int new_value ) { m_voiceChannels = new_value; }
inline int Settings::voiceChannels() const { return m_voiceChannels; }
inline void Settings::setVoiceEncodingMode( int new_value ) { m_voiceEncodingMode = new_value; }
inline int Settings::voiceEncodingMode() const { return m_voiceEncodingMode; }
inline void Settings::setVoiceEncodingQuality( int new_value ) { m_voiceEncodingQuality = new_value; }
inline int Settings::voiceEncodingQuality() const { return m_voiceEncodingQuality; }
inline void Settings::setUseCustomVoiceEncoderSettings( bool new_value ) { m_useCustomVoiceEncoderSettings = new_value; }
inline bool Settings::useCustomVoiceEncoderSettings() const { return m_useCustomVoiceEncoderSettings; }
inline void Settings::setUseSystemVoiceEncoderSettings( bool new_value ) { m_useSystemVoiceEncoderSettings = new_value; }
inline bool Settings::useSystemVoiceEncoderSettings() const { return m_useSystemVoiceEncoderSettings; }
inline bool Settings::disableConnectionSocketEncryption() const { return m_disableConnectionSocketEncryption; }
inline bool Settings::disableConnectionSocketDataCompression() const { return m_disableConnectionSocketDataCompression; }
inline bool Settings::allowNotEncryptedConnectionsAlso() const { return m_allowNotEncryptedConnectionsAlso; }
inline bool Settings::allowEncryptedConnectionsAlso() const { return m_allowEncryptedConnectionsAlso; }
inline bool Settings::disableBeepInUserStatusBusy() const { return m_disableBeepInUserStatusBusy; }
inline void Settings::setDisableBeepInUserStatusBusy( bool new_value ) { m_disableBeepInUserStatusBusy = new_value; }
inline bool Settings::beepInActiveWindowAlso() const { return m_beepInActiveWindowAlso; }
inline void Settings::setBeepInActiveWindowAlso( bool new_value ) { m_beepInActiveWindowAlso = new_value; }


#endif // BEEBEEP_SETTINGS_
