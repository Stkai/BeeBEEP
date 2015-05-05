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
class Core;
class Chat;
class ChatMessage;
class FileInfo;
class GameInterface;
class GuiChat;
class GuiChatList;
class GuiGroupList;
class GuiLog;
class GuiSavedChat;
class GuiSavedChatList;
class GuiScreenShot;
class GuiShareLocal;
class GuiShareNetwork;
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
  void saveSession();

public slots:
  void startStopCore();
  void quitCore();
  void hideToTrayIcon();
  void setInIdle();
  void exitFromIdle();
  void showMessage( const QString&, int );
  void showUp();

private slots:
  void showAbout();
  void showLicense();
  void checkUser( const User& );
  void showWritingUser( const User& );
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
  void downloadFile( const User&, const FileInfo& );
  void downloadSharedFile( VNumber, VNumber );
  void selectDownloadDirectory();
  void changeVCard();
  void showUserMenu( VNumber );
  void changeUserColor( VNumber );
  void showPluginHelp();
  void showPluginManager();
  void showCurrentChat();
  void showChat( VNumber );
  void showWizard();
  void showFromTrayIcon();
  void forceExit();
  void trayIconClicked( QSystemTrayIcon::ActivationReason );
  void trayMessageClicked();
  void raiseChatView();
  void raiseLocalShareView();
  void raiseNetworkShareView();
  void raisePluginView();
  void raiseLogView();
  void raiseScreenShotView();
  void addToShare( const QString& );
  void removeFromShare( const QString& );
  void openUrl( const QUrl& );
  void selectBeepFile();
  void testBeepFile();
  void playBeep();
  void addUserToGroupChat();
  void createGroupChat();
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

protected:
  void closeEvent( QCloseEvent* );
  void changeEvent( QEvent* );
  bool showAlert();
  bool promptConnectionPassword();
  void raiseOnTop();

private:
  void createActions();
  void createMenus();
  void createToolAndMenuBars();
  void createStatusBar();
  void createDockWindows();
  void createStackedWidgets();
  void createPluginWindows();
  void refreshUserList();
  void refreshTitle( const User& );
  void startCore();
  void stopCore();
  void initGuiItems();
  void updadePluginMenu();
  void updateStatusIcon();
  void sendFile( const User& );
  bool askToDownloadFile( const User&, const FileInfo& );
  void setGameInPauseMode();
  void checkAutoStartOnBoot( bool );
  void checkViewActions();
  QMenu* gameMenu( GameInterface* );
  bool sendFile( const User&, const QString& );
  bool openWebUrl( const QString& );
  QString checkFilePath( const QString& );
  void showVCard( const User&, bool ensure_visible );
  bool isAudioDeviceAvailable() const;

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
  Core *mp_core;

  QMenu *mp_menuMain;
  QMenu *mp_menuInfo;
  QMenu *mp_menuUsers;
  QMenu *mp_menuChat;
  QMenu *mp_menuSystem;
  QMenu *mp_menuStatus;
  QMenu *mp_menuPlugins;
  QMenu *mp_menuView;

  QToolBar *mp_barMain;
  QToolBar *mp_barChat;
  QToolBar *mp_barPlugins;
  QToolBar *mp_barShareNetwork;
  QToolBar *mp_barShareLocal;
  QToolBar *mp_barScreenShot;

  QAction* mp_actStartStopCore;
  QAction* mp_actConfigureNetwork;
  QAction* mp_actBroadcast;
  QAction* mp_actQuit;
  QAction* mp_actVCard;
  QAction* mp_actToolBar;
  QAction* mp_actChatBar;
  QAction* mp_actPluginBar;
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
  QAction* mp_actBeepOnNewMessage;
  QAction* mp_actPromptPassword;

  QDockWidget* mp_dockUserList;
  QDockWidget* mp_dockGroupList;
  QDockWidget* mp_dockSavedChatList;
  QDockWidget* mp_dockChatList;

  GuiSystemTray* mp_trayIcon;

  QMap<QString, QMenu*> m_mapGameMenu;

  int m_lastUserStatus;

  QSound* mp_sound;

};

#endif // BEEBEEP_GUIMAIN_H
