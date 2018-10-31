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

#ifndef BEEBEEP_GUIREFUSEDCHAT_H
#define BEEBEEP_GUIREFUSEDCHAT_H

#include "ui_GuiRefusedChat.h"
#include "ChatRecord.h"


class GuiRefusedChat : public QDialog, private Ui::GuiRefusedChatDialog
{
  Q_OBJECT

public:
  explicit GuiRefusedChat( QWidget *parent = Q_NULLPTR );

  int loadRefusedChats();

  inline const QList<ChatRecord>& refusedChats() const;

protected slots:
  void saveAndClose();
  void openCustomMenu( const QPoint& );
  void removeRefusedChat();
  void removeAllRefusedChats();

protected:
  void loadRefusedChatsInList();

private:
  QList<ChatRecord> m_refusedChats;
  QMenu* mp_menuContext;

};

#endif // BEEBEEP_GUIREFUSEDCHAT_H
