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

#ifndef BEEBEEP_GUICHATLIST_H
#define BEEBEEP_GUICHATLIST_H

#include "GuiChatItem.h"
#include "ui_GuiChatList.h"


class GuiChatList : public QWidget, private Ui::GuiChatListWidget
{
  Q_OBJECT

public:
  GuiChatList( QWidget* parent = 0 );

  void reloadChatList();

signals:
  void chatSelected( VNumber chat_id );
  void chatToClear( VNumber chat_id );
  void chatToRemove( VNumber chat_id );
  void createNewChatRequest();

public slots:
  void updateChat( const Chat& );
  void updateUser( const User& );
  void onTickEvent( int );

protected slots:
  void chatClicked( QTreeWidgetItem*, int );
  void showChatMenu( const QPoint& );
  void openChatSelected();
  void clearChatSelected();
  void removeChatSelected();
  void filterText( const QString& );
  void clearFilter();

private:
  GuiChatItem* itemFromChatId( VNumber );

  QMenu* mp_menuContext;
  VNumber m_chatSelected;
  QAction* mp_actClear;
  QAction* mp_actDelete;

  VNumber m_chatOpened;
  bool m_blockShowChatRequest;

  QString m_filter;
};


#endif // BEEBEEP_GUICHATLIST_H
