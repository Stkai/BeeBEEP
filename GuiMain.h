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
class BeeBeep;
class ChatMessage;
class GuiChat;
class GuiUserList;
class User;


class GuiMain : public QMainWindow
{
  Q_OBJECT

public:
  GuiMain( QWidget* parent = 0 );

public slots:
  void startStopBeeBeep();

private slots:
  void showAbout();
  void startBeeBeep();
  void stopBeeBeep();
  void newUser( const User& );
  void removeUser( const User& );
  void selectNickname();
  void selectFontColor();
  void selectFont();
  void searchUsers();
  void settingsChanged();
  void emoticonSelected();
  void chatSelected( int, const QString& );
  void showMessage( const QString&, const ChatMessage& );
  void sendMessage( const QString&, const QString& );
  void toggleMenuBar( bool );
  void saveChat();
  void showWritingUser( const User& );
  void showNewUserStatus( const User& );
  void showNextChat();
  void statusSelected();
  void changeStatusDescription();
  void sendFile();

protected:
  void closeEvent( QCloseEvent* );
  void refreshUserList();
  void refreshChat();
  void refreshTitle();
  void createActions();
  void createMenus();
  void createToolBars();
  void createStatusBar();
  void createDockWindows();

private:
  GuiChat* mp_defaultChat;
  GuiUserList* mp_userList;
  BeeBeep *mp_beeBeep;

  QMenu *mp_menuSettings;
  QMenu *mp_menuSettingsIcon;
  QMenu *mp_menuEmoticons;
  QMenu *mp_menuStatus;

  QToolBar *mp_barMain;

  QAction* mp_actStartStopBeeBeep;
  QAction* mp_actSaveChat;
  QAction* mp_actSearch;
  QAction* mp_actQuit;
  QAction* mp_actNickname;
  QAction* mp_actFont;
  QAction* mp_actFontColor;
  QAction* mp_actMenuBar;
  QAction* mp_actToolBar;
  QAction* mp_actAbout;
  QAction* mp_actViewUsers;
  QAction* mp_actSendFile;


};

#endif // BEEBEEP_GUIMAIN_H
