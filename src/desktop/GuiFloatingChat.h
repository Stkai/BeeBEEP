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

#ifndef BEEBEEP_GUIFLOATINGCHAT_H
#define BEEBEEP_GUIFLOATINGCHAT_H

#include "GuiChat.h"
class GuiEmoticons;
class GuiPresetMessageList;

class GuiFloatingChat : public QMainWindow
{
  Q_OBJECT

public:
  GuiFloatingChat( QWidget* parent = Q_NULLPTR );

  bool setChat( const Chat& );
  inline GuiChat* guiChat() const;
  void setWindowFlagsAndGeometry();
  inline bool chatIsVisible() const;
  void setMainIcon( bool with_message );
  void updateChat( const Chat& );
  void updateUser( const User& );
  void setFocusInChat();
  void updateActions( bool is_connected, int connected_users );
  void setChatReadByUser( const Chat&, const User& );
  void showChatMessage( const Chat&, const ChatMessage& );
  void updateEmoticons();
  inline void setChatToolbarVisible( bool );

  void onTickEvent( int );

  inline void setSaveGeometryDisabled( bool );

public slots:
  void showUp();
  void raiseOnTop();

signals:
  void readAllMessages( VNumber );
  void chatIsAboutToClose( VNumber );
  void showVCardRequest( VNumber );
  void updateChatColorsRequest();

protected:
  void closeEvent( QCloseEvent* );
  void keyPressEvent( QKeyEvent* );
  void updateChatTitle( const Chat& );
  void updateChatMembers( const Chat& );
  void updateChatMember( const Chat&, const User& );

private slots:
  void onApplicationFocusChanged( QWidget*, QWidget* );
  void saveGeometryAndState();
  void toggleVisibilityEmoticonPanel();
  void toggleVisibilityPresetMessagesPanel();
  void onGroupMemberActionTriggered();
  void showGroupMenu();

private:
  GuiChat* mp_chat;
  QToolBar* mp_barChat;
  QToolBar* mp_barMembers;
  QDockWidget* mp_dockEmoticons;
  QDockWidget* mp_dockPresetMessageList;
  GuiEmoticons* mp_emoticonsWidget;
  GuiPresetMessageList* mp_presetMessageListWidget;

  bool m_chatIsVisible;
  bool m_prevActivatedState;

  QIcon m_mainWindowIcon;
  QAction* mp_actGroupMenu;
  QAction* mp_actSaveWindowGeometry;

};

// Inline Functions
inline GuiChat* GuiFloatingChat::guiChat() const { return mp_chat; }
inline bool GuiFloatingChat::chatIsVisible() const { return m_chatIsVisible; }
inline void GuiFloatingChat::setChatToolbarVisible( bool new_value ) { mp_barChat->setVisible( new_value ); }
inline void GuiFloatingChat::setSaveGeometryDisabled( bool new_value ) { mp_actSaveWindowGeometry->setDisabled( new_value ); }

#endif // BEEBEEP_GUIFLOATINGCHAT_H
