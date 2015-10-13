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

class GuiFloatingChat : public QMainWindow
{
  Q_OBJECT

public:
  GuiFloatingChat( QWidget* parent = 0 );

  bool setChatId( VNumber );
  inline VNumber chatId() const;
  inline GuiChat* guiChat() const;
  void checkWindowFlagsAndShow();
  void showUserWriting( VNumber, const QString& );
  void raiseOnTop();
  inline bool chatIsVisible() const;

signals:
  void attachChatRequest( VNumber );
  void readAllMessages( VNumber );

protected:
  void closeEvent( QCloseEvent* );
  void applyFlagStaysOnTop();

private slots:
  void onApplicationFocusChanged( QWidget*, QWidget* );

private:
  GuiChat* mp_chat;
  QToolBar* mp_barChat;
  QDockWidget* mp_dockEmoticons;
  GuiEmoticons* mp_emoticonsWidget;

  bool m_chatIsVisible;
  bool m_prevActivatedState;
};


// Inline Functions
inline VNumber GuiFloatingChat::chatId() const { return mp_chat->chatId(); }
inline GuiChat* GuiFloatingChat::guiChat() const { return mp_chat; }
inline bool GuiFloatingChat::chatIsVisible() const { return m_chatIsVisible; }


#endif // BEEBEEP_GUIFLOATINGCHAT_H
