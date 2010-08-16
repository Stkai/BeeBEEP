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

#ifndef BEEBEEP_GUICHAT_H
#define BEEBEEP_GUICHAT_H

#include "ui_GuiChat.h"
#include "Chat.h"


class GuiChat : public QWidget, private Ui::GuiChatWidget
{
  Q_OBJECT

public:
  GuiChat( QWidget* parent = 0 );
  void addToMyMessage( const QString& ); // For emoticons
  void appendMessage( const QString&, const QString& );
  void setChatFont( const QFont& );
  void setChatFontColor( const QString& );
  void setChat( const Chat& );
  inline const QString& chatName() const;
  void setLastMessageTimestamp( const QDateTime& );
  inline QString toHtml() const;

signals:
  void newMessage( const QString&, const QString& );
  void writing( const QString& );
  void nextChat();

private slots:
  void sendMessage();
  void checkWriting();
  void customContextMenu( const QPoint& );

private:
  QString m_chatName;

};


// Inline Functions

inline const QString& GuiChat::chatName() const { return m_chatName; }
inline QString GuiChat::toHtml() const { return mp_teChat->toHtml(); }


#endif // BEEBEEP_GUICHAT_H
