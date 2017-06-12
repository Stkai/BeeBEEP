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

#ifndef BEEBEEP_GUIUSERLIST_H
#define BEEBEEP_GUIUSERLIST_H

#include "GuiUserItem.h"
#include "ui_GuiUserList.h"
class User;
class Chat;


class GuiUserList : public QWidget, private Ui::GuiUserListWidget
{
  Q_OBJECT

public:
  GuiUserList( QWidget* parent = 0 );

  void setUser( const User&, bool );
  void removeUser( const User& );
  void setUnreadMessages( VNumber private_chat_id, int );
  void setMessages( VNumber private_chat_id, int );
  void updateUsers();
  void updateChat( const Chat& );

  inline void setMenuSettings( QMenu* );
  inline void setContextMenuUsers( QMenu* );

  void onTickEvent( int );

  void clear();

signals:
  void userSelected( VNumber );
  void chatSelected( VNumber );
  void showVCardRequest( VNumber );

protected slots:
  void userItemClicked( QTreeWidgetItem*, int );
  void showUserMenu( const QPoint& );
  void filterText( const QString& );
  void clearFilter();
  void showMenuSettings();
  void onItemEntered( QTreeWidgetItem*, int );

private:
  GuiUserItem* itemFromUserId( VNumber );
  GuiUserItem* itemFromChatId( VNumber );
  void sortUsers();
  void resetList();

  QString m_filter;

  QMenu* mp_menuSettings;
  QMenu* mp_menuUsers;

  bool m_blockShowChatRequest;
  bool m_showOnlyChatMembers;

};


// Inline Functions
inline void GuiUserList::setMenuSettings( QMenu* new_value ) { mp_menuSettings = new_value; }
inline void GuiUserList::setContextMenuUsers( QMenu* new_value ) { mp_menuUsers = new_value; }

#endif // BEEBEEP_GUIUSERLIST_H
