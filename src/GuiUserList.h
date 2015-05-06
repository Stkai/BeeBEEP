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
class User;


class GuiUserList : public QTreeWidget
{
  Q_OBJECT

public:
  GuiUserList( QWidget* parent = 0 );

  virtual QSize sizeHint() const;

  void setUser( const User& );
  void removeUser( const User& );
  void setUnreadMessages( VNumber private_chat_id, int );
  void updateUsers( bool );
  bool nextUserWithUnreadMessages();

  void setDefaultChatConnected( bool );

signals:
  void chatSelected( VNumber chat_id );
  void menuToShow( VNumber user_id );

protected slots:
  void userItemClicked( QTreeWidgetItem*, int );
  void showUserMenu( const QPoint& );

private:
  GuiUserItem* itemFromUserId( VNumber );
  GuiUserItem* itemFromChatId( VNumber );
  void sortUsers();
  void resetList();

  QColor m_defaultForegroundColor;

};


#endif // BEEBEEP_GUIUSERLIST_H
