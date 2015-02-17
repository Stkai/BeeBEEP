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

#ifndef BEEBEEP_GUIGROUPLIST_H
#define BEEBEEP_GUIGROUPLIST_H

#include "GuiGroupItem.h"


class GuiGroupList : public QTreeWidget
{
  Q_OBJECT

public:
  GuiGroupList( QWidget* parent = 0 );

  virtual QSize sizeHint() const;

  void loadGroups();
  void updateGroup( VNumber );
  void updateUser( const User& );

signals:
  void openChatForGroupRequest( VNumber group_id );
  void createGroupRequest();
  void editGroupRequest( VNumber );
  void showVCardRequest( VNumber );
  void removeGroupRequest( VNumber );

protected slots:
  void showGroupMenu( const QPoint& );
  void checkItemDoubleClicked( QTreeWidgetItem*, int );
  void openGroupChatSelected();
  void editGroupSelected();
  void removeGroupSelected();

private:
  GuiGroupItem* itemFromId( VNumber );

  QAction* mp_actCreateGroup;
  QAction* mp_actEditGroup;
  QAction* mp_actOpenChat;
  QAction* mp_actRemoveGroup;

  VNumber m_selectedGroupId;
  VNumber m_selectedChatId;

};


#endif // BEEBEEP_GUIGROUPLIST_H
