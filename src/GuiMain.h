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
class GuiLog;
class GuiSavedChat;
class GuiSavedChatList;
class GuiScreenShot;
class GuiSessionManager;
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
  void hideToTrayIcon();
  void setInIdle();
  void exitFromIdle();

private slots:
  void showAbout();
  void showLicense();
  void checkUser( const User& );
  void showWritingUser( const User& );
  void showChatMessage( VNumber, const ChatMessage& );
  void sendMessage( VNumber, const QString& );
  void showTipOfTheDay();
  void selectFontColor();
  void selectFont();
  void searchUsers();
  void settingsChanged();
  void emoticonSelected();
  void saveChat();
  void showNextChat();
  void statusSelected();
  void changeStatusDescription();
  void sendFile();
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
  void showChat( VNumber );
  void sendBroadcastMessage();
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
  void addUserToGroup();
  void createGroup();
  void loadSessionCompleted();
  void showSavedChatSelected( const QString& );
  void removeSavedChat( const QString& );
  void linkSavedChat( const QString& );
  void checkNewVersion();
  void openWebSite();
  void openDownloadPluginPage();

protected:
  void closeEvent( QCloseEvent* );
  void changeEvent( QEvent* );
  bool showAlert();
  void raiseOnTop();
  bool promptConnectionPassword();

private:
  void createActions();
  void createMenus();
  void createToolAndMenuBars();
  void createStatusBar();
  void createDockWindows();
  void createStackedWidgets();
  void createPluginWindows();
  void refreshUserList();
  void refreshChat();
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

private:
  QStackedWidget* mp_stackedWidget;
  GuiChat* mp_defaultChat;
  GuiTransferFile* mp_fileTransfer;
  GuiUserList* mp_userList;
  GuiChatList* mp_chatList;
  GuiSavedChatList* mp_savedChatList;
  GuiShareLocal* mp_shareLocal;
  GuiShareNetwork* mp_shareNetwork;
  GuiLog* mp_logView;
  GuiSavedChat* mp_savedChat;
  GuiScreenShot* mp_screenShot;
  Core *mp_core;

  QMenu *mp_menuMain;
  QMenu *mp_menuInfo;
  QMenu *mp_menuSettings;
  QMenu *mp_menuChat;
  QMenu *mp_menuEmoticons;
  QMenu *mp_menuStatus;
  QMenu *mp_menuPlugins;
  QMenu *mp_menuView;

  QToolBar *mp_barMain;
  QToolBar *mp_barPlugins;

  QAction* mp_actStartStopCore;
  QAction* mp_actSaveChat;
  QAction* mp_actSearch;
  QAction* mp_actQuit;
  QAction* mp_actVCard;
  QAction* mp_actFont;
  QAction* mp_actFontColor;
  QAction* mp_actToolBar;
  QAction* mp_actPluginBar;
  QAction* mp_actAbout;
  QAction* mp_actViewUsers;
  QAction* mp_actSendFile;
  QAction* mp_actViewFileTransfer;
  QAction* mp_actViewChats;
  QAction* mp_actViewSavedChats;
  QAction* mp_actViewShareLocal;
  QAction* mp_actViewShareNetwork;
  QAction* mp_actViewDefaultChat;
  QAction* mp_actBeepOnNewMessage;
  QAction* mp_actGroupAdd;
  QAction* mp_actCreateGroup;
  QAction* mp_actPromptPassword;

  QDockWidget* mp_dockUserList;

  GuiSystemTray* mp_trayIcon;

  GuiSessionManager* mp_sessionManager;

  QMap<QString, QMenu*> m_mapGameMenu;

  int m_lastUserStatus;

};

#endif // BEEBEEP_GUIMAIN_H
