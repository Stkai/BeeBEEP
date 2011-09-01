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
class GuiChat;
class GuiTransferFile;
class GuiUserList;
class User;


class GuiMain : public QMainWindow
{
  Q_OBJECT

public:
  GuiMain( QWidget* parent = 0 );

public slots:
  void startStopCore();

private slots:
  void showAbout();
  void checkUser( const User& );
  void showWritingUser( const User& );
  void showSelectedChat( VNumber chat_id );
  void showChatMessage( VNumber, const ChatMessage& );
  void sendMessage( VNumber, const QString& );
  void changeUserColor( VNumber );
  void showTipOfTheDay();

  void selectNickname();
  void selectFontColor();
  void selectFont();
  void searchUsers();
  void settingsChanged();
  void emoticonSelected();

  void toggleMenuBar( bool );
  void saveChat();

  void showNextChat();
  void statusSelected();
  void changeStatusDescription();
  void sendFile();
  void sendFile( VNumber );
  void downloadFile( const User&, const FileInfo& );
  void selectDownloadDirectory();
  void changeVCard();

protected:
  void closeEvent( QCloseEvent* );
  void showChat( const Chat& );

private:
  void createActions();
  void createMenus();
  void createToolBars();
  void createStatusBar();
  void createDockWindows();
  void refreshUserList();
  void refreshChat();
  void refreshTitle();
  void startCore();
  void stopCore();
  void initGuiItems();

private:
  GuiChat* mp_defaultChat;
  GuiTransferFile* mp_fileTransfer;
  GuiUserList* mp_userList;
  Core *mp_core;

  QMenu *mp_menuSettings;
  QMenu *mp_menuEmoticons;
  QMenu *mp_menuStatus;

  QToolBar *mp_barMain;

  QAction* mp_actStartStopCore;
  QAction* mp_actSaveChat;
  QAction* mp_actSearch;
  QAction* mp_actQuit;
  QAction* mp_actNickname;
  QAction* mp_actVCard;
  QAction* mp_actFont;
  QAction* mp_actFontColor;
  QAction* mp_actMenuBar;
  QAction* mp_actToolBar;
  QAction* mp_actAbout;
  QAction* mp_actViewUsers;
  QAction* mp_actSendFile;
  QAction* mp_actViewFileTransfer;

};

#endif // BEEBEEP_GUIMAIN_H
