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

#ifndef BEEBEEP_GUIMAIN_H
#define BEEBEEP_GUIMAIN_H


#include "Config.h"
#include "FileInfo.h"
#ifdef BEEBEEP_USE_QXT
 #include "qxtglobalshortcut.h"
#endif
class Core;
class Chat;
class ChatMessage;
class GameInterface;
class GuiChat;
class GuiChatList;
class GuiEmoticons;
class GuiFloatingChat;
class GuiGroupList;
class GuiLog;
class GuiHome;
class GuiSavedChat;
class GuiSavedChatList;
class GuiScreenShot;
class GuiShareBox;
class GuiShareLocal;
class GuiShareNetwork;
#ifdef BEEBEEP_USE_SHAREDESKTOP
  class GuiShareDesktop;
#endif
class GuiSystemTray;
class GuiTransferFile;
class GuiUserList;
class User;


class GuiMain : public QMainWindow
{
  Q_OBJECT

public:
  GuiMain( QWidget* parent = 0 );
  void checkWindowFlagsAndShow();

  void loadSession();

public slots:
  void startStopCore();
  void forceShutdown();
  void hideToTrayIcon();
  void setInIdle();
  void exitFromIdle();
  void showMessage( const QString&, int );
  void showUp();
  void onTickEvent( int );
  void saveSession( QSessionManager& );

private slots:
  void startCore();
  void stopCore();
  void showAbout();
  void showLicense();
  void updateUser( const User& );
  void showWritingUser( const User&, VNumber );
  void showChatMessage( VNumber, const ChatMessage& );
  void sendMessage( VNumber, const QString& );
  void showTipOfTheDay();
  void showFactOfTheDay();
  void searchUsers();
  void settingsChanged();
  void showNextChat();
  void statusSelected();
  void changeStatusDescription();
  void sendFileFromChat( VNumber, const QString& );
  void sendFile( VNumber );
  void sendFile( const QString& );
  void downloadFolder( const User&, const QString&, const QList<FileInfo>& );
  void downloadFile( const User&, const FileInfo& );
  void downloadSharedFile( VNumber, VNumber );
  void downloadSharedFiles( const QList<SharedFileInfo>& );
  void selectDownloadDirectory();
  void changeVCard();
  void showVCard( VNumber, bool );
  void changeUserColor( VNumber );
  void showPluginHelp();
  void showPluginManager();
  void showCurrentChat();
  void showDefaultChat();
  void showChat( VNumber );
  void showWizard();
  void trayIconClicked( QSystemTrayIcon::ActivationReason );
  void trayMessageClicked();
  void raiseChatView();
  void raiseLocalShareView();
  void raiseNetworkShareView();
  void raisePluginView();
  void raiseLogView();
  void raiseScreenShotView();
  void raiseHomeView();
  void raiseShareBoxView();
  void addToShare( const QString& );
  void removeFromShare( const QString& );
  void openUrl( const QUrl& );
  void selectBeepFile();
  void testBeepFile();
  void playBeep();
  void editGroupFromChat( VNumber );
  void createChat();
  void createGroup();
  void editGroup( VNumber );
  void showSavedChatSelected( const QString& );
  void removeSavedChat( const QString& );
  void linkSavedChat( const QString& );
  void checkNewVersion();
  void openWebSite();
  void openDownloadPluginPage();
  void openDonationPage();
  void openHelpPage();
  void openFacebookPage();
  void clearChat( VNumber );
  void checkGroup( VNumber );
  void checkChat( VNumber );
  void leaveGroupChat( VNumber );
  void removeGroup( VNumber );
  void removeChat( VNumber );
  void showChatForGroup( VNumber );
  void showSharesForUser( const User& );
  void selectLanguage();
  void showLocalUserVCard();
  void showAddUser();
  void showChatSettingsMenu();
  void emoticonMenuVisibilityChanged( bool );
  void sendBroadcastMessage();
  void enableBroadcastAction();
  void checkUserSelected( VNumber );
  void showConnectionStatusChanged( const User& );
  void changeAvatarSizeInList();
  void toggleUserFavorite( VNumber );
  void createGroupFromChat( VNumber );
  void removeUserFromList( VNumber );
  void openDataFolder();
  void openResourceFolder();
  void detachChat( VNumber );
  void attachChat( VNumber );
  void readAllMessagesInChat( VNumber );
  void recentlyUsedUserStatusSelected();
  void clearRecentlyUsedUserStatus();
  void loadSavedChatsCompleted();
  void editShortcuts();
  void onApplicationFocusChanged( QWidget*, QWidget* );
  void minimizeAllChats();
  void showAllChats();
  void selectDictionatyPath();
  void onNetworkInterfaceDown();
  void onNetworkInterfaceUp();
  void onChatReadByUser( VNumber chat_id, VNumber user_id );
  void saveGeometryAndState();
  void toggleVisibilityEmoticonPanel();
  void onChangeSettingBeepOnNewMessage( QAction* );
  void onChangeSettingOnExistingFile( QAction* );
  void onShareBoxRequest( VNumber, const QString& );
  void onShareBoxDownloadRequest( VNumber, const FileInfo&, const QString& );
  void onShareBoxUploadRequest( VNumber, const FileInfo&, const QString& );
#ifdef BEEBEEP_USE_SHAREDESKTOP
  void onShareDesktopImageAvailable( const User&, const QPixmap& );
  void onShareDesktopCloseEvent( VNumber );
#endif
  void startExternalApplicationFromActionData();
  void onFileTransferCompleted( VNumber, const User&, const FileInfo& );

protected:
  void keyPressEvent( QKeyEvent* );
  void closeEvent( QCloseEvent* );
  void changeEvent( QEvent* );
  bool promptConnectionPassword();
  void raiseOnTop();
  void checkChatToolbar();
  void raiseView( QWidget*, VNumber, const QString& );
  bool checkAllChatMembersAreConnected( const QList<VNumber>& );
  void closeFloatingChat( VNumber );
  bool reloadChat( VNumber );
  bool chatIsVisible( VNumber );
  void showAlertForMessage( VNumber, const ChatMessage&, bool* chat_window_is_created );
  GuiChat* guiChat( VNumber );

private:
  void updateMainIcon();
  void setupChatConnections( GuiChat* );
  void createActions();
  void createMenus();
  void createToolAndMenuBars();
  void createDockWindows();
  void createStackedWidgets();
  void createPluginWindows();
  void refreshUserList();
  void refreshTitle( const User& );
  void initGuiItems();
  void updadePluginMenu();
  void updateStatusIcon();
  void sendFile( const User& );
  bool askToDownloadFile( const User&, const FileInfo&, const QString&, bool );
  void setGameInPauseMode();
  void checkAutoStartOnBoot( bool );
  void checkViewActions();
  QMenu* gameMenu( GameInterface* );
  bool sendFile( const User&, const QString&, VNumber chat_id );
  void sendFiles( const User&, const QStringList&, VNumber chat_id );
  bool openWebUrl( const QString& );
  QStringList checkFilePath( const QString& );
  void showVCard( const User&, bool ensure_visible );
  bool isAudioDeviceAvailable() const;
  void showDefaultServerPortInMenu();
  void applyFlagStaysOnTop();
  bool floatingChatExists( VNumber ) const;
  GuiFloatingChat* floatingChat( VNumber ) const;
  QWidget* activeChatWindow();
  void setChatMessagesToShowInAction( QAction* );
  void loadUserStatusRecentlyUsed();
  void setUserStatusSelected( int );
  void initShortcuts();
  void updateShortcuts();
  void updateEmoticons();
  void updateNewMessageAction();
  QList<GuiChat*> guiChatList() const;

private:
  QStackedWidget* mp_stackedWidget;
  GuiChat* mp_chat;
  GuiTransferFile* mp_fileTransfer;
  GuiUserList* mp_userList;
  GuiChatList* mp_chatList;
  GuiSavedChatList* mp_savedChatList;
  GuiGroupList* mp_groupList;
  GuiShareLocal* mp_shareLocal;
  GuiShareNetwork* mp_shareNetwork;
  GuiLog* mp_logView;
  GuiSavedChat* mp_savedChat;
  GuiScreenShot* mp_screenShot;
  GuiHome* mp_home;
  GuiEmoticons* mp_emoticonsWidget;
  Core *mp_core;
  QList<GuiFloatingChat*> m_floatingChats;
#ifdef BEEBEEP_USE_SHAREDESKTOP
  QList<GuiShareDesktop*> m_desktops;
#endif
  QMenu* mp_menuMain;
  QMenu* mp_menuInfo;
  QMenu* mp_menuChat;
  QMenu* mp_menuSettings;
  QMenu* mp_menuStatus;
  QMenu* mp_menuPlugins;
  QMenu* mp_menuView;
  QMenu* mp_menuTrayIcon;
  QMenu* mp_menuUserList;
  QMenu* mp_menuUserStatusList;

  QToolBar* mp_barMain;
  QToolBar* mp_barChat;
  QToolBar* mp_barShareNetwork;
  QToolBar* mp_barShareLocal;
  QToolBar* mp_barScreenShot;
  QToolBar* mp_barLog;
  QToolBar* mp_barGames;

  QAction* mp_actStartStopCore;
  QAction* mp_actConfigureNetwork;
  QAction* mp_actBroadcast;
  QAction* mp_actQuit;
  QAction* mp_actVCard;
  QAction* mp_actToolBar;
  QAction* mp_actChatBar;
  QAction* mp_actAbout;
  QAction* mp_actViewUsers;
  QAction* mp_actViewFileTransfer;
  QAction* mp_actViewChats;
  QAction* mp_actViewSavedChats;
  QAction* mp_actViewGroups;
  QAction* mp_actViewShareLocal;
  QAction* mp_actViewShareNetwork;
  QAction* mp_actViewDefaultChat;
  QAction* mp_actViewScreenShot;
  QAction* mp_actViewLog;
  QAction* mp_actViewHome;
  QAction* mp_actViewNewMessage;
  QAction* mp_actPromptPassword;
  QAction* mp_actConfirmDownload;

  GuiShareBox* mp_shareBox;
  QAction* mp_actViewShareBox;

  QAction* mp_actCreateGroup;
  QAction* mp_actCreateGroupChat;

  QActionGroup* mp_actGroupExistingFile;
  QAction* mp_actOverwriteExistingFile;
  QAction* mp_actGenerateAutomaticFilename;
  QAction* mp_actAskToDoOnExistingFile;

  QActionGroup* mp_actGroupBeepOnNewMessage;
  QAction* mp_actBeepOnNewMessage;
  QAction* mp_actAlwaysBeepOnNewMessage;
  QAction* mp_actNeverBeepOnNewMessage;

  QMenu* mp_menuNetworkStatus;
  QAction* mp_actHostAddress;
  QAction* mp_actPortBroadcast;
  QAction* mp_actPortListener;
  QAction* mp_actPortFileTransfer;
#ifdef BEEBEEP_USE_MULTICAST_DNS
  QAction* mp_actMulticastDns;
#endif
  QDockWidget* mp_dockUserList;
  QDockWidget* mp_dockGroupList;
  QDockWidget* mp_dockSavedChatList;
  QDockWidget* mp_dockChatList;
  QDockWidget* mp_dockFileTransfers;
  QDockWidget* mp_dockEmoticons;

  GuiSystemTray* mp_trayIcon;

  QMap<QString, QMenu*> m_mapGameMenu;
  QMenu* mp_defaultGameMenu;

  int m_lastUserStatus;

  bool m_forceShutdown;
  bool m_autoConnectOnInterfaceUp;

  QShortcut* mp_scMinimizeAllChats;
  QShortcut* mp_scShowNextUnreadMessage;
#ifdef BEEBEEP_USE_QXT
  QxtGlobalShortcut* mp_scShowAllChats;
#endif
  QWidget* mp_lastActiveWindow;

  bool m_prevActivatedState;
};

#endif // BEEBEEP_GUIMAIN_H
