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

#ifndef BEEBEEP_GUIMAIN_H
#define BEEBEEP_GUIMAIN_H


#include "Config.h"
#include "FileTransferPeer.h"
#ifdef BEEBEEP_USE_QXT
 #include "qxtglobalshortcut.h"
#endif
class Chat;
class ChatMessage;
class Group;
class GuiChat;
class GuiChatList;
class GuiFileSharing;
class GuiFloatingChat;
class GuiGroupList;
class GuiHome;
class GuiLog;
class GuiNetworkTest;
class GuiSavedChat;
class GuiSavedChatList;
class GuiScreenShot;
class GuiSystemTray;
class GuiFileTransfer;
class GuiUserList;
class User;
#ifdef BEEBEEP_USE_SHAREDESKTOP
  class GuiShareDesktop;
#endif
#ifdef BEEBEEP_USE_WEBENGINE
  class GuiWebView;
#endif


class GuiMain : public QMainWindow
{
  Q_OBJECT

public:
  GuiMain( QWidget* parent = Q_NULLPTR );

public slots:
  void loadSession();
  void checkWindowFlagsAndShow();
  void startCore();
  void stopCore();
  void restartCore();
  void forceShutdown();
  void hideToTrayIcon();
  void setInIdle();
  void exitFromIdle();
  void showMessage( const QString&, int );
  void showUp();
  void raiseOnTop();
  void onTickEvent( int );
  void saveSession( QSessionManager& );
  void onWakeUpRequest();
  void onSleepRequest();
  void disconnectFromNetwork();
  void loadStyle();

private slots:
  void onCoreConnected();
  void onCoreDisconnected();
  void showAbout();
  void showLicense();
  void showAboutQt();
  void onUserChanged( const User& );
  void showWritingUser( const User&, VNumber );
  void onUserRemoved( const User& );
  void onNewChatMessage( const Chat&, const ChatMessage& );
  void onChatChanged( const Chat& );
  void onChatRemoved( const Chat& );
  void sendMessage( VNumber, const QString&, bool );
  void showTipOfTheDay();
  void showFaq();
  void showFactOfTheDay();
  void searchUsers();
  void settingsChanged();
  void settingsChanged( QAction* );
  void showNextChat();
  void statusSelected();
  void changeStatusDescription();
  void sendFileFromChat( VNumber, const QString& );
  void sendFilesFromChat( VNumber, const QStringList& );
  void sendFile( VNumber );
  void sendFile( const QString& );
  void downloadFolder( const User&, const QString&, const QList<FileInfo>& );
  void downloadFile( const User&, const FileInfo& );
  void downloadSharedFile( VNumber, VNumber );
  void downloadSharedFiles( const QList<SharedFileInfo>& );
  void selectDownloadDirectory();
  void changeVCard();
  void showVCard( VNumber );
  void changeUserColor( VNumber, const QString& );
  void showPluginHelp();
  void showPluginManager();
  void showChat( VNumber );
  bool showWizard();
  void trayIconClicked( QSystemTrayIcon::ActivationReason );
  void trayMessageClicked();
  void addToShare( const QString& );
  void removeFromShare( const QString& );
  void openUrl( const QUrl& );
  void openUrlFromChat( const QUrl&, VNumber );
  void selectBeepFile();
  void testBeepFile();
  void playBeep();
  void playBuzz();
  void editGroupChat( VNumber );
  void createGroupChat();
  void showSavedChatSelected( const QString& );
  void removeSavedChat( const QString& );
  void linkSavedChat( const QString& );
  void checkNewVersion();
  void openWebSite();
  void openDownloadPluginPage();
  void openDonationPage();
  void openDeveloperWebSite();
  void openHelpPage();
  void clearChat( VNumber );
  void clearSystemMessagesInChat( VNumber );
  void removeChat( VNumber );
  void showSharesForUser( const User& );
  void selectLanguage();
  void showLocalUserVCard();
  void showAddUser();
  void showChatSettingsMenu();
  void sendBroadcastMessage();
  void enableBroadcastAction();
  void checkUserSelected( VNumber );
  void showConnectionStatusChanged( const User& );
  void changeAvatarSizeInList();
  void toggleUserFavorite( VNumber );
  void removeUserFromList( VNumber );
  void openDataFolder();
  void openResourceFolder();
  void openDownloadFolder();
  void readAllMessagesInChat( VNumber );
  void recentlyUsedUserStatusSelected();
  void clearRecentlyUsedUserStatus();
  void loadSavedChatsCompleted();
  void editShortcuts();
  void minimizeAllChats();
  void showAllChats();
  void sendHelpMessage();
  void sendHelpMessageToUser( VNumber );
  void selectDictionatyPath();
  void onNetworkInterfaceDown();
  void onNetworkInterfaceUp();
  void onChatReadByUser( const Chat&, const User& );
  void askSaveGeometryAndState();
  void askResetGeometryAndState();
  void onChangeSettingOnExistingFile( QAction* );
#ifdef BEEBEEP_USE_SHAREDESKTOP
  void onShareDesktopImageAvailable( const User&, const QImage&, const QString&, QRgb );
  void onShareDesktopCloseEvent( VNumber );
  void onShareDesktopDeleteRequest( VNumber );
  void onShareDesktopRequestFromChat( VNumber, bool );
  void onShareDesktopUpdate( const User& );
  void sendScreenshotToChat( VNumber );
#endif
  void startExternalApplicationFromActionData();
  void onFileTransferProgress( VNumber, const User&, const FileInfo&, FileSizeType, qint64 );
  void onFileTransferMessage( VNumber, const User&, const FileInfo&, const QString&, FileTransferPeer::TransferState );
  void sendBuzzToUser( VNumber );
  void showBuzzFromUser( const User&, VNumber );
  void showHelpRequestFromUser( const User&, VNumber );
  void showHelpAnswerFromUser( const User&, VNumber );
  void removeFloatingChatFromList( VNumber );
  void showFileSharingWindow();
  void onFileSharingWindowClosed();
  void showScreenShotWindow();
  void onScreenShotWindowClosed();
  void showLogWindow();
  void onLogWindowClosed();
  void onMainTabChanged( int );
  void showWorkgroups();
  void showRefusedChats();
  void updateLocalStatusMessage();
  void onStatusBarMessageChanged( const QString& );
  void selectIconSourcePath();
  void selectEmoticonSourcePath();
  void onNewsAvailable( const QString& );
  void showDefaultServerPortInMenu();
  void createMessage();
  void updateChatFont();
  void updateChatColors();
  void resetAllColors();
  void onHideEmptyChatsRequest();
  void updateUser( const User& );
  void onApplicationFocusChanged( QWidget*, QWidget* );
#ifdef BEEBEEP_USE_WEBENGINE
  void showWebView();
  void onNewsLoad( bool );
#endif
  void showNetworkTest();
  void onNetworkTestWindowClosed();
#ifdef BEEBEEP_USE_VOICE_CHAT
  void sendVoiceMessageToChat( VNumber, const QString&, qint64 );
  void showVoiceEncoderSettings();
#endif
  void resumeFileTransfer( VNumber user_id, const FileInfo& );
  void changeEmoticonSizeInChat();
  void selectFirstChat();

protected:
  void keyPressEvent( QKeyEvent* );
  void closeEvent( QCloseEvent* );
  void changeEvent( QEvent* );
  bool promptConnectionPassword();
  bool checkAllChatMembersAreConnected( const QList<VNumber>& );
  void showAlertForMessage( const Chat&, const ChatMessage& );
  GuiFloatingChat* createFloatingChat( const Chat& );
  void setFileTransferEnabled( bool );
  void setFileSharingEnabled( bool );
  void saveGeometryAndState();
  void restoreGeometryAndState();
  void resetGeometryAndState();
  void setMinimumWidthForStyle();

private:
  void setupChatConnections( GuiChat* );
  void createActions();
  void createMenus();
  void createToolAndMenuBars();
  void createMainWidgets();
  void updateWindowTitle();
  void initGuiItems();
  void updadePluginMenu();
  void updateStatusIcon();
  void sendFile( const User& );
  bool askToDownloadFile( const User&, const FileInfo&, const QString&, bool );
  void checkAutoStartOnBoot( bool );
  void checkViewActions();
  bool sendFile( const User&, const QString&, VNumber chat_id );
  bool openWebUrl( const QString& );
  QStringList checkFilePath( const QString& );
  bool isAudioDeviceAvailable() const;
  GuiFloatingChat* floatingChat( VNumber ) const;
  QWidget* activeWindow();
  void setChatMessagesToShowInAction( QAction* );
  void setMaxInactivityDaysInAction( QAction* );
  void setChatInactiveWindowOpacityLevelInAction( QAction* );
  void loadUserStatusRecentlyUsed();
  void setUserStatusSelected( int );
  void initShortcuts();
  void updateShortcuts();
  void updateEmoticons();
  void updateNewMessageAction();
  void updateTabTitles();
  QString tabToolTip( int );
  void showRestartConnectionAlertMessage();
  void showRestartApplicationAlertMessage();
  void showCheckSaveChatMessages();
  void setChatMaxLinesToSaveInAction( QAction* );
  void setClearCacheAfterDaysInAction( QAction* );
  void setClearPartiallyDownloadedFilesAfterDaysInAction( QAction* );
  void setMaxQueuedDownloadsInAction( QAction* );
  bool isFileTransferInProgress();

private:
  QTabWidget* mp_tabMain;
  GuiFileTransfer* mp_fileTransfer;
  GuiUserList* mp_userList;
  GuiChatList* mp_chatList;
  GuiSavedChatList* mp_savedChatList;
  GuiGroupList* mp_groupList;
  GuiScreenShot* mp_screenShot;
  GuiLog* mp_log;
  QList<GuiFloatingChat*> m_floatingChats;
  GuiFileSharing* mp_fileSharing;
  GuiHome* mp_home;
#ifdef BEEBEEP_USE_SHAREDESKTOP
  QList<GuiShareDesktop*> m_desktops;
#endif
  QMenu* mp_menuMain;
  QMenu* mp_menuInfo;
  QMenu* mp_menuChat;
  QMenu* mp_menuSettings;
  QMenu* mp_menuStatus;
  QMenu* mp_menuPlugins;
  QMenu* mp_menuTrayIcon;
  QMenu* mp_menuUserList;
  QMenu* mp_menuUserStatusList;
  QMenu* mp_menuInterfaceSettings;
  QMenu* mp_menuStartupSettings;
  QMenu* mp_menuCloseSettings;
  QMenu* mp_menuUsersSettings;
  QMenu* mp_menuChatSettings;
  QMenu* mp_menuFileTransferSettings;
  QMenu* mp_menuNotificationSettings;
  QMenu* mp_menuTrayIconSettings;
  QMenu* mp_menuExistingFile;
  QMenu* mp_menuConnectionSettings;
  QMenu* mp_menuChatColorSettings;
  QMenu* mp_menuChatSaveSettings;

  QToolBar* mp_barMain;

  QAction* mp_actConnect;
  QAction* mp_actDisconnect;
  QAction* mp_actConfigureNetwork;
  QAction* mp_actBroadcast;
  QAction* mp_actQuit;
  QAction* mp_actVCard;
  QAction* mp_actMainToolBar;
  QAction* mp_actAbout;
  QAction* mp_actViewFileTransfer;
  QAction* mp_actViewNewMessage;
  QAction* mp_actPromptPassword;
  QAction* mp_actConfirmDownload;
  QAction* mp_actViewFileSharing;
  QAction* mp_actViewScreenShot;
  QAction* mp_actViewLog;
  QAction* mp_actAddUsers;
  QAction* mp_actCreateGroupChat;
  QAction* mp_actEnableFileSharing;
  QAction* mp_actSelectDownloadFolder;
  QAction* mp_actSaveWindowGeometry;
  QActionGroup* mp_actGroupExistingFile;
  QAction* mp_actResumeFileTransfer;
  QAction* mp_actBeepOnNewMessage;
  QActionGroup* mp_actGroupRecognizeUsers;
  QAction* mp_actSelectEmoticonSourcePath;
  QAction* mp_actRemoveInactiveUsers;
  QAction* mp_actChangeStatusDescription;
  QAction* mp_actCreateMessage;
  QAction* mp_actSaveUserList;
  QAction* mp_actSaveGroupList;
  QAction* mp_actSaveFileTransferMessages;
  QAction* mp_actSaveSystemMessages;
  QAction* mp_actSetAutoAway;
  QAction* mp_actEditWorkgroups;
  QAction* mp_actEditAvatarIconSize;
  QAction* mp_actShowOnlineUsersOnly;
  QAction* mp_actShowUserFullName;
  QAction* mp_actShowUserFirstNameFirstInFullName;
  QActionGroup* mp_actGroupOnSendingMessage;

  QMenu* mp_menuNetworkStatus;
  QAction* mp_actHostAddress;
  QAction* mp_actPortBroadcast;
  QAction* mp_actPortListener;
  QAction* mp_actPortFileTransfer;
#ifdef BEEBEEP_USE_MULTICAST_DNS
  QAction* mp_actMulticastDns;
#endif
  QAction* mp_actMulticastGroupAddress;
  QAction* mp_actEncryptedConnectionByDefault;
  QAction* mp_actCompressedDataByDefault;

  QAction* mp_actHelpRequest;

  QDockWidget* mp_dockFileTransfers;

  GuiSystemTray* mp_trayIcon;
  int m_lastUserStatus;

  bool m_forceShutdown;
  bool m_userSelectOffline;

  QShortcut* mp_scMinimizeAllChats;
  QShortcut* mp_scShowNextUnreadMessage;
#ifdef BEEBEEP_USE_QXT
  QxtGlobalShortcut* mp_scShowAllChats;
  QxtGlobalShortcut* mp_scSendHelpMessage;
  QxtGlobalShortcut* mp_scSelectFirstChat;
#endif
  QWidget* mp_lastActiveWindow;

  bool m_prevActivatedState;
  int m_unreadActivities;
  bool m_coreIsConnecting;
  bool m_changeTabToUserListOnFirstConnected;

#ifdef BEEBEEP_USE_WEBENGINE
  GuiWebView* mp_webView;
  QAction* mp_actWebView;
#endif

  GuiNetworkTest* mp_networkTest;

#ifdef BEEBEEP_USE_VOICE_CHAT
  QMenu* mp_menuVoiceMessage;
#endif

  bool m_useFusionStyle;
};

#endif // BEEBEEP_GUIMAIN_H
