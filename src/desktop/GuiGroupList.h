//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2021 Marco Mastroddi
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
// along with BeeBEEP. If not, see <http://www.gnu.org/licenses/>.
//
// Author: Marco Mastroddi <marco.mastroddi(AT)gmail.com>
//
// $Id$
//
//////////////////////////////////////////////////////////////////////

#ifndef BEEBEEP_GUIGROUPLIST_H
#define BEEBEEP_GUIGROUPLIST_H

#include "GuiGroupItem.h"
#include "ui_GuiGroupList.h"


class GuiGroupList : public QWidget, private Ui::GuiGroupListWidget
{
  Q_OBJECT

public:
  GuiGroupList( QWidget* parent );

  inline void setMainToolTip( const QString& );

  void updateGroups();
  void updateUser( const User& );
  void updateChat( const Chat& );

  void onTickEvent( int );

  void updateBackground();

signals:
  void openChatForGroupRequest( VNumber group_id );
  void createGroupRequest();
  void editGroupRequest( VNumber );
  void showVCardRequest( VNumber );

protected slots:
  void showGroupMenu( const QPoint& );
  void checkItemClicked( QTreeWidgetItem*, int );
  void openGroupChatSelected();
  void editGroupSelected();
  void enableGroupNotification();
  void disableGroupNotification();
  void filterText( const QString& );
  void clearFilter();
  void selectBackgroundColor();

protected:
  void keyReleaseEvent( QKeyEvent* );
  GuiGroupItem* itemFromId( VNumber );

private:
  QAction* mp_actCreateGroup;
  QAction* mp_actEditGroup;
  QAction* mp_actOpenChat;
  QAction* mp_actEnableGroupNotification;
  QAction* mp_actDisableGroupNotification;

  QMenu* mp_contextMenu;

  VNumber m_selectedGroupId;
  VNumber m_groupChatOpened;
  bool m_blockShowChatRequest;

  QString m_filter;

};


// Inline Functions
inline void GuiGroupList::setMainToolTip( const QString& new_value ) { mp_twGroupList->setToolTip( new_value ); }

#endif // BEEBEEP_GUIGROUPLIST_H
