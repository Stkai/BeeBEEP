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
class GuiChat;
class GuiChatList;
class GuiFileSharing;
class GuiFloatingChat;
class GuiGroupList;
class GuiHome;
class GuiLog;
class GuiSavedChat;
class GuiSavedChatList;
class GuiScreenShot;
class GuiSystemTray;
class GuiTransferFile;
class GuiUserList;
class User;


class GuiMain : public QMainWindow
{
  Q_OBJECT

public:
  GuiMain( QWidget* parent = 0 );

public slots:
  void loadSession();
  void checkWindowFlagsAndShow();
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
  void onUserChanged( const User& );
  void showWritingUser( const User&, VNumber );
  void onNewChatMessage( const Chat&, const ChatMessage& );
  void onChatChanged( const Chat& );
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
  void showVCard( VNumber );
  void changeUserColor( VNumber, const QString& );
  void showPluginHelp();
  void showPluginManager();
  void showChat( VNumber );
  void showWizard();
  void trayIconClicked( QSystemTrayIcon::ActivationReason );
  void trayMessageClicked();
  void addToShare( const QString& );
  void removeFromShare( const QString& );
  void openUrl( const QUrl& );
  void selectBeepFile();
  void testBeepFile();
  void playBeep();
  void editChat( VNumber );
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
  void leaveGroupChat( VNumber );
  void removeGroup( VNumber );
  void removeChat( VNumber );
  void showChatForGroup( VNumber );
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
  void createGroupFromChat( VNumber );
  void removeUserFromList( VNumber );
  void openDataFolder();
  void openResourceFolder();
  void readAllMessagesInChat( VNumber );
  void recentlyUsedUserStatusSelected();
  void clearRecentlyUsedUserStatus();
  void loadSavedChatsCompleted();
  void editShortcuts();
  void minimizeAllChats();
  void showAllChats();
  void selectDictionatyPath();
  void onNetworkInterfaceDown();
  void onNetworkInterfaceUp();
  void onChatReadByUser( const Chat&, const User& );
  void saveGeometryAndState();
  void onChangeSettingOnExistingFile( QAction* );
  void onShareBoxRequest( VNumber, const QString& );
  void onShareBoxDownloadRequest( VNumber, const FileInfo&, const QString& );
  void onShareBoxUploadRequest( VNumber, const FileInfo&, const QString& );
#ifdef BEEBEEP_USE_SHAREDESKTOP
  void onShareDesktopImageAvailable( const User&, const QPixmap& );
  void onShareDesktopCloseEvent( VNumber );
#endif
  void startExternalApplicationFromActionData();
  void onFileTransferProgress( VNumber, const User&, const FileInfo&, FileSizeType );
  void onFileTransferMessage( VNumber, const User&, const FileInfo&, const QString& );
  void onFileTransferCompleted( VNumber, const User&, const FileInfo& );
  void sendBuzzToUser( VNumber );
  void showBuzzFromUser( const User& );
  void removeFloatingChatFromList( VNumber );
  void showFileSharingWindow();
  void onFileSharingWindowClosed();
  void showScreenShotWindow();
  void onScreenShotWindowClosed();
  void showLogWindow();
  void onLogWindowClosed();

protected:
  void keyPressEvent( QKeyEvent* );
  void closeEvent( QCloseEvent* );
  void changeEvent( QEvent* );
  bool promptConnectionPassword();
  bool checkAllChatMembersAreConnected( const QList<VNumber>& );
  void showAlertForMessage( const Chat&, const ChatMessage& );
  GuiFloatingChat* createFloatingChat( const Chat& );
  void closeFloatingChat( VNumber );

private:
  void setupChatConnections( GuiChat* );
  void createActions();
  void createMenus();
  void createToolAndMenuBars();
  void createMainWidgets();
  void refreshUserList();
  void updateWindowTitle();
  void initGuiItems();
  void updadePluginMenu();
  void updateStatusIcon();
  void sendFile( const User& );
  bool askToDownloadFile( const User&, const FileInfo&, const QString&, bool );
  void checkAutoStartOnBoot( bool );
  void checkViewActions();
  bool sendFile( const User&, const QString&, VNumber chat_id );
  void sendFiles( const User&, const QStringList&, VNumber chat_id );
  bool openWebUrl( const QString& );
  QStringList checkFilePath( const QString& );
  bool isAudioDeviceAvailable() const;
  void showDefaultServerPortInMenu();
  GuiFloatingChat* floatingChat( VNumber ) const;
  QWidget* activeWindow() const;
  void setChatMessagesToShowInAction( QAction* );
  void loadUserStatusRecentlyUsed();
  void setUserStatusSelected( int );
  void initShortcuts();
  void updateShortcuts();
  void updateEmoticons();
  void updateNewMessageAction();
  void updateTabTitles();

private:
  QTabWidget* mp_tabMain;
  GuiTransferFile* mp_fileTransfer;
  GuiUserList* mp_userList;
  GuiChatList* mp_chatList;
  GuiSavedChatList* mp_savedChatList;
  GuiGroupList* mp_groupList;
  GuiScreenShot* mp_screenShot;
  GuiLog* mp_log;
  Core *mp_core;
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
  QMenu* mp_menuStartupSettings;
  QMenu* mp_menuCloseSettings;
  QMenu* mp_menuChatSettings;
  QMenu* mp_menuFileTransferSettings;
  QMenu* mp_menuSoundSettings;
  QMenu* mp_menuTrayIconSettings;

  QToolBar* mp_barMain;

  QAction* mp_actStartStopCore;
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
  QAction* mp_actViewHome;
  QAction* mp_actViewScreenShot;
  QAction* mp_actViewLog;

  QAction* mp_actCreateGroup;
  QAction* mp_actCreateGroupChat;

  QActionGroup* mp_actGroupExistingFile;
  QAction* mp_actOverwriteExistingFile;
  QAction* mp_actGenerateAutomaticFilename;
  QAction* mp_actAskToDoOnExistingFile;

  QAction* mp_actBeepOnNewMessage;

  QMenu* mp_menuNetworkStatus;
  QAction* mp_actHostAddress;
  QAction* mp_actPortBroadcast;
  QAction* mp_actPortListener;
  QAction* mp_actPortFileTransfer;
#ifdef BEEBEEP_USE_MULTICAST_DNS
  QAction* mp_actMulticastDns;
#endif

  QDockWidget* mp_dockFileTransfers;
  QDockWidget* mp_dockHome;

  GuiSystemTray* mp_trayIcon;
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
