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

#include "GuiGroupList.h"
#include "GuiConfig.h"
#include "ChatManager.h"
#include "UserManager.h"


GuiGroupList::GuiGroupList( QWidget* parent )
  : QTreeWidget( parent )
{
  setObjectName( "GuiGroupList" );

  setColumnCount( 1 );
  header()->hide();
  setRootIsDecorated( true );
  setSortingEnabled( true );

  setContextMenuPolicy( Qt::CustomContextMenu );
  setMouseTracking( true );

  m_selectedGroupId = ID_INVALID;
  m_groupChatOpened = ID_INVALID;

  mp_actCreateGroup = new QAction( QIcon( ":/images/group-add.png" ), tr( "Create group" ), this );
  connect( mp_actCreateGroup, SIGNAL( triggered() ), this, SIGNAL( createGroupRequest() ) );

  mp_actEditGroup = new QAction( QIcon( ":/images/group-edit.png" ), tr( "Edit group" ), this );
  connect( mp_actEditGroup, SIGNAL( triggered() ), this, SLOT( editGroupSelected() ) );

  mp_actOpenChat = new QAction( QIcon( ":/images/chat.png" ), tr( "Open chat" ), this );
  connect( mp_actOpenChat, SIGNAL( triggered() ), this, SLOT( openGroupChatSelected() ) );

  mp_actRemoveGroup = new QAction( QIcon( ":/images/group-remove.png" ), tr( "Delete group" ), this );
  connect( mp_actRemoveGroup, SIGNAL( triggered() ), this, SLOT( removeGroupSelected() ) );


  connect( this, SIGNAL( itemClicked( QTreeWidgetItem*, int ) ), this, SLOT( checkItemClicked( QTreeWidgetItem*, int ) ) );
  connect( this, SIGNAL( customContextMenuRequested( const QPoint& ) ), this, SLOT( showGroupMenu( const QPoint& ) ) );
}

QSize GuiGroupList::sizeHint() const
{
  return QSize( BEE_DOCK_WIDGET_SIZE_HINT_WIDTH, BEE_DOCK_WIDGET_SIZE_HINT_HEIGHT );
}

void GuiGroupList::loadGroups()
{
  if( topLevelItemCount() > 0 )
    clear();

  foreach( Group g, UserManager::instance().groups() )
  {
    GuiGroupItem* group_item = new GuiGroupItem( this );
    group_item->init( g.id(), true );
    group_item->setChatOpened( g.id() == m_groupChatOpened );
    group_item->updateGroup( g );
  }
}

void GuiGroupList::updateGroup( VNumber group_id )
{
  Group g = UserManager::instance().group( group_id );
  if( !g.isValid() )
    return;

  GuiGroupItem* group_item = itemFromId( group_id );
  if( !group_item )
  {
    group_item = new GuiGroupItem( this );
    group_item->init( group_id, true );
    group_item->setChatOpened( group_id == m_groupChatOpened );
  }
  group_item->updateGroup( g );
  sortItems( 0, Qt::AscendingOrder );
}

GuiGroupItem* GuiGroupList::itemFromId( VNumber item_id )
{
  GuiGroupItem* item;
  QTreeWidgetItemIterator it( this );
  while( *it )
  {
    item = (GuiGroupItem*)(*it);
    if( item->itemId() == item_id )
      return item;
    ++it;
  }
  return 0;
}

void GuiGroupList::checkItemClicked( QTreeWidgetItem* item, int )
{
  if( !item )
    return;

  GuiGroupItem* group_item = (GuiGroupItem*)item;
  if( group_item->isGroup() )
    emit openChatForGroupRequest( group_item->itemId() );
  clearSelection();
}

void GuiGroupList::showGroupMenu( const QPoint& p )
{
  QTreeWidgetItem* item = itemAt( p );
  if( !item )
  {
    QMenu menu;
    if( UserManager::instance().userList().toList().size() < 2 )
      menu.addAction( QIcon( ":/images/group-remove.png" ), tr( "Waiting for two or more connected user" ) );
    else
      menu.addAction( mp_actCreateGroup );
    menu.exec( QCursor::pos() );
    return;
  }

  GuiGroupItem* group_item = (GuiGroupItem*)item;

  if( group_item->isGroup() )
  {
    m_selectedGroupId = group_item->itemId();
    QMenu menu;
    menu.addAction( mp_actOpenChat );
    menu.setDefaultAction( mp_actOpenChat );
    menu.addSeparator();
    menu.addAction( mp_actEditGroup );
    menu.addSeparator();
    menu.addAction( mp_actRemoveGroup );
    menu.exec( QCursor::pos() );
  }
  else
  {
    emit showVCardRequest( group_item->itemId(), true );
  }
  clearSelection();
}

void GuiGroupList::openGroupChatSelected()
{
  if( m_selectedGroupId != ID_INVALID )
  {
    emit openChatForGroupRequest( m_selectedGroupId );
    m_selectedGroupId = ID_INVALID;
  }
}

void GuiGroupList::editGroupSelected()
{
  if( m_selectedGroupId != ID_INVALID )
  {
    emit editGroupRequest( m_selectedGroupId );
    m_selectedGroupId = ID_INVALID;
  }
}

void GuiGroupList::removeGroupSelected()
{
  if( m_selectedGroupId != ID_INVALID )
  {
    emit removeGroupRequest( m_selectedGroupId );
    m_selectedGroupId = ID_INVALID;
  }
}

void GuiGroupList::updateUser( const User& u )
{
  GuiGroupItem* item;
  QTreeWidgetItemIterator it( this );
  while( *it )
  {
    item = (GuiGroupItem*)(*it);
    if( item->itemId() == u.id() )
      item->updateUser( u );
    ++it;
  }
  sortItems( 0, Qt::AscendingOrder );
}

void GuiGroupList::updateChat( VNumber chat_id )
{
  GuiGroupItem* item;
  QTreeWidgetItemIterator it( this );
  while( *it )
  {
    item = (GuiGroupItem*)(*it);
    if( item->updateChat( chat_id ) )
    {
      sortItems( 0, Qt::AscendingOrder );
      return;
    }
    ++it;
  }
}

void GuiGroupList::setChatOpened( VNumber chat_id )
{
  m_groupChatOpened = ID_INVALID;

  if( chat_id != ID_INVALID )
  {
    Chat c = ChatManager::instance().chat( chat_id );
    if( c.isValid() && c.isGroup() )
    {
      Group g = UserManager::instance().findGroupByPrivateId( c.privateId() );
      if( g.isValid() )
        m_groupChatOpened = g.id();
    }
  }

  GuiGroupItem* item;
  QTreeWidgetItemIterator it( this );
  while( *it )
  {
    item = (GuiGroupItem*)(*it);
    item->setChatOpened( item->itemId() == m_groupChatOpened );
    ++it;
  }
}
