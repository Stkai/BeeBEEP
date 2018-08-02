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

#include "GuiConfig.h"
#include "GuiGroupList.h"
#include "ChatManager.h"
#include "IconManager.h"
#include "Settings.h"
#include "UserManager.h"


GuiGroupList::GuiGroupList( QWidget* parent )
  : QWidget( parent )
{
  setObjectName( "GuiGroupList" );
  setupUi( this );

  mp_twGroupList->setColumnCount( 1 );
  mp_twGroupList->setRootIsDecorated( true );
  mp_twGroupList->setSortingEnabled( true );
  mp_twGroupList->setObjectName( "GuiCustomList" );
  mp_twGroupList->setContextMenuPolicy( Qt::CustomContextMenu );
  mp_twGroupList->setMouseTracking( true );
  mp_twGroupList->setSortingEnabled( true );
  mp_twGroupList->setIconSize( Settings::instance().avatarIconSize() );
  mp_twGroupList->setHeaderHidden( true );
  updateBackground();

#if QT_VERSION >= 0x040700
  mp_leFilter->setPlaceholderText( tr( "Search group" ) );
#endif

  mp_contextMenu = new QMenu( parent );

  m_selectedGroupId = ID_INVALID;
  m_groupChatOpened = ID_INVALID;
  m_blockShowChatRequest = false;
  m_filter = "";

  mp_actCreateGroup = new QAction( IconManager::instance().icon( "group-add.png" ), tr( "Create new group chat" ), this );
  connect( mp_actCreateGroup, SIGNAL( triggered() ), this, SIGNAL( createGroupRequest() ) );

  mp_actEditGroup = new QAction( IconManager::instance().icon( "group-edit.png" ), tr( "Edit group chat" ), this );
  connect( mp_actEditGroup, SIGNAL( triggered() ), this, SLOT( editGroupSelected() ) );

  mp_actOpenChat = new QAction( IconManager::instance().icon( "chat.png" ), tr( "Open chat" ), this );
  connect( mp_actOpenChat, SIGNAL( triggered() ), this, SLOT( openGroupChatSelected() ) );

  mp_actEnableGroupNotification = new QAction( IconManager::instance().icon( "notification-disabled.png" ), tr( "Enable notifications" ), this );
  connect( mp_actEnableGroupNotification, SIGNAL( triggered() ), this, SLOT( enableGroupNotification() ) );

  mp_actDisableGroupNotification = new QAction( IconManager::instance().icon( "notification-enabled.png" ), tr( "Disable notifications" ), this );
  connect( mp_actDisableGroupNotification, SIGNAL( triggered() ), this, SLOT( disableGroupNotification() ) );

  mp_pbClearFilter->setIcon( IconManager::instance().icon( "clear.png" ) );

  connect( mp_twGroupList, SIGNAL( customContextMenuRequested( const QPoint& ) ), this, SLOT( showGroupMenu( const QPoint& ) ) );
  connect( mp_twGroupList, SIGNAL( itemClicked( QTreeWidgetItem*, int ) ), this, SLOT( checkItemClicked( QTreeWidgetItem*, int ) ), Qt::QueuedConnection );
  connect( mp_leFilter, SIGNAL( textChanged( const QString& ) ), this, SLOT( filterText( const QString& ) ) );
  connect( mp_pbClearFilter, SIGNAL( clicked() ), this, SLOT( clearFilter() ) );
}

void GuiGroupList::updateGroups()
{
  mp_twGroupList->setIconSize( Settings::instance().avatarIconSize() );
  if( mp_twGroupList->topLevelItemCount() > 0 )
    mp_twGroupList->clear();

  foreach( Chat c, ChatManager::instance().constChatList() )
  {
    if( c.isGroup() )
    {
      if( !m_filter.isEmpty() )
      {
        if( !c.name().contains( m_filter, Qt::CaseInsensitive ) )
          continue;
      }

      GuiGroupItem* group_item = new GuiGroupItem( mp_twGroupList );
      group_item->init( c.id(), true );
      group_item->updateChat( c );
    }
  }

  mp_twGroupList->sortItems( 0, Qt::AscendingOrder );
}

void GuiGroupList::updateUser( const User& u )
{
  GuiGroupItem* item;
  QTreeWidgetItemIterator it( mp_twGroupList );
  while( *it )
  {
    item = (GuiGroupItem*)(*it);
    if( item->itemId() == u.id() )
      item->updateUser( u );
    ++it;
  }
  mp_twGroupList->sortItems( 0, Qt::AscendingOrder );
}

void GuiGroupList::updateChat( const Chat& c )
{
  if( !c.isGroup() )
    return;

  GuiGroupItem* group_item = itemFromId( c.id() );
  if( !group_item )
  {
    group_item = new GuiGroupItem( mp_twGroupList );
    group_item->init( c.id(), true );
  }
  group_item->updateChat( c );
  mp_twGroupList->sortItems( 0, Qt::AscendingOrder );
}

GuiGroupItem* GuiGroupList::itemFromId( VNumber item_id )
{
  GuiGroupItem* item;
  QTreeWidgetItemIterator it( mp_twGroupList );
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

  if( m_blockShowChatRequest )
  {
    m_blockShowChatRequest = false;
    return;
  }

  GuiGroupItem* group_item = (GuiGroupItem*)item;
  if( group_item->isGroup() )
    emit openChatForGroupRequest( group_item->itemId() );
}

void GuiGroupList::showGroupMenu( const QPoint& p )
{
  QTreeWidgetItem* item = mp_twGroupList->itemAt( p );
  mp_contextMenu->clear();

  if( !item )
  {
    if( UserManager::instance().userList().toList().size() < 2 )
    {
      mp_contextMenu->addAction( IconManager::instance().icon( "info.png" ), tr( "Please wait for two or more users" ) );
      mp_contextMenu->addSeparator();
    }

    mp_contextMenu->addAction( mp_actCreateGroup );
    mp_actCreateGroup->setEnabled( UserManager::instance().userList().toList().size() > 1 );
    mp_contextMenu->addSeparator();
    mp_contextMenu->addAction( IconManager::instance().icon( "background-color.png" ), tr( "Change background color" ) + QString("..."), this, SLOT( selectBackgroundColor() ) );
    mp_contextMenu->exec( QCursor::pos() );
    return;
  }

  m_blockShowChatRequest = true;

  GuiGroupItem* group_item = (GuiGroupItem*)item;

  if( group_item->isGroup() )
  {
    m_selectedGroupId = group_item->itemId();
    mp_contextMenu->addAction( mp_actOpenChat );
    mp_contextMenu->setDefaultAction( mp_actOpenChat );
    mp_contextMenu->addSeparator();
    mp_contextMenu->addAction( mp_actEditGroup );
    mp_contextMenu->addSeparator();
    Chat c = ChatManager::instance().chat( m_selectedGroupId );
    if( Settings::instance().isNotificationDisabledForGroup( c.privateId() ) )
      mp_contextMenu->addAction( mp_actEnableGroupNotification );
    else
      mp_contextMenu->addAction( mp_actDisableGroupNotification );

    mp_contextMenu->addSeparator();
    mp_contextMenu->addAction( IconManager::instance().icon( "background-color.png" ), tr( "Change background color" ) + QString("..."), this, SLOT( selectBackgroundColor() ) );
    mp_contextMenu->exec( QCursor::pos() );
  }
  else
  {
    emit showVCardRequest( group_item->itemId() );
  }

  mp_twGroupList->clearSelection();
}

void GuiGroupList::openGroupChatSelected()
{
  if( m_selectedGroupId != ID_INVALID )
  {
    mp_twGroupList->clearSelection();
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

void GuiGroupList::enableGroupNotification()
{
  if( m_selectedGroupId != ID_INVALID )
  {
    Chat c = ChatManager::instance().chat( m_selectedGroupId );
    if( !c.isValid() )
    {
      qWarning() << "Invalid id" << m_selectedGroupId << "found in enable group notification";
      return;
    }
    qDebug() << "Enable notification for group chat:" << qPrintable( c.name() );
    Settings::instance().setNotificationEnabledForGroup( c.privateId(), true );
    m_selectedGroupId = ID_INVALID;
  }
}

void GuiGroupList::disableGroupNotification()
{
  if( m_selectedGroupId != ID_INVALID )
  {
    Chat c = ChatManager::instance().chat( m_selectedGroupId );
    if( !c.isValid() )
    {
      qWarning() << "Invalid id" << m_selectedGroupId << "found in disable group notification";
      return;
    }
    qDebug() << "Disable notification for group:" << c.name();
    Settings::instance().setNotificationEnabledForGroup( c.privateId(), false );
    m_selectedGroupId = ID_INVALID;
  }
}

void GuiGroupList::filterText( const QString& txt )
{
  QString new_filter = txt.trimmed().toLower();
  if( m_filter == new_filter )
    return;

  m_filter = new_filter;
  updateGroups();
}

void GuiGroupList::clearFilter()
{
  mp_leFilter->setText( "" );
  mp_leFilter->setFocus();
}

void GuiGroupList::selectBackgroundColor()
{
  QColor c = Bee::selectColor( this, Settings::instance().groupListBackgroundColor() );
  if( c.isValid() )
  {
    Settings::instance().setGroupListBackgroundColor( c.name() );
    updateBackground();
  }
}

void GuiGroupList::updateBackground()
{
  QString w_stylesheet = QString( BEE_GUICUSTOMLIST_STYLESHEET ).arg( Settings::instance().groupListBackgroundColor(),
                                                   IconManager::instance().iconPath( "group-list.png" ) );
  mp_twGroupList->setStyleSheet( w_stylesheet );
}

