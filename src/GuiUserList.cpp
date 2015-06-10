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

#include "ChatManager.h"
#include "GuiUserList.h"
#include "GuiConfig.h"
#include "Settings.h"
#include "UserManager.h"


GuiUserList::GuiUserList( QWidget* parent )
  : QTreeWidget( parent )
{
  setObjectName( "GuiUserList" );

  setContextMenuPolicy( Qt::CustomContextMenu );
  setRootIsDecorated( false );
  setSortingEnabled( true );
  setColumnCount( 1 );

  m_chatOpened = ID_INVALID;

  setHeaderHidden( true );
  resetList();

  connect( this, SIGNAL( itemClicked( QTreeWidgetItem*, int ) ), this, SLOT( userItemClicked( QTreeWidgetItem*, int ) ) );
  connect( this, SIGNAL( customContextMenuRequested( const QPoint& ) ), this, SLOT( showUserMenu( const QPoint& ) ) );
}

void GuiUserList::sortUsers()
{
  sortItems( 0, Qt::AscendingOrder );
}

QSize GuiUserList::sizeHint() const
{
  return QSize( BEE_DOCK_WIDGET_SIZE_HINT_WIDTH, BEE_DOCK_WIDGET_SIZE_HINT_HEIGHT );
}

void GuiUserList::resetList()
{
  if( topLevelItemCount() > 0 )
    clear();
  if( Settings::instance().showUserPhoto() )
    setIconSize( Settings::instance().avatarIconSize() );
  else
    setIconSize( QSize( 16, 16 ) );
}

void GuiUserList::updateUsers( bool is_connected )
{
  resetList();
  setUser( Settings::instance().localUser() );
  setDefaultChatConnected( is_connected );
  foreach( User u, UserManager::instance().userList().toList() )
    setUser( u );
}

GuiUserItem* GuiUserList::itemFromUserId( VNumber user_id )
{
  GuiUserItem* item;
  QTreeWidgetItemIterator it( this );
  while( *it )
  {
    item = (GuiUserItem*)(*it);
    if( item->userId() == user_id )
      return item;
    ++it;
  }
  return 0;
}

GuiUserItem* GuiUserList::itemFromChatId( VNumber chat_id )
{
  GuiUserItem* item;
  QTreeWidgetItemIterator it( this );
  while( *it )
  {
    item = (GuiUserItem*)(*it);
    if( item->chatId() == chat_id )
      return item;
    ++it;
  }
  return 0;
}

void GuiUserList::setUnreadMessages( VNumber chat_id, int n )
{
  GuiUserItem* item = itemFromChatId( chat_id );
  if( !item )
    return;

  item->setUnreadMessages( n );
  item->updateUser();
}

void GuiUserList::setUser( const User& u )
{
  GuiUserItem* item = itemFromUserId( u.id() );
  bool item_is_created = false;
  if( !item )
  {
    if( !u.isConnected() && Settings::instance().showOnlyOnlineUsers() )
      return;

    item = new GuiUserItem( this );
    item->setUserId( u.id() );
    item_is_created = true;
  }
  else
  {
    if( !u.isConnected() && Settings::instance().showOnlyOnlineUsers() )
    {
      removeUser( u );
      return;
    }
  }

  Chat c = ChatManager::instance().privateChatForUser( u.id() );
  item->setChatId( c.id() );
  item->setUnreadMessages( c.unreadMessages() );
  item->updateUser( u );

  if( item_is_created )
    item->setChatOpened( m_chatOpened );
  sortUsers();
}

void GuiUserList::removeUser( const User& u )
{
  GuiUserItem* item = itemFromUserId( u.id() );
  if( item )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Delete user item from GuiUserList";
#endif
    QTreeWidgetItem* root_item = invisibleRootItem();
    if( root_item )
    {
      root_item->removeChild( (QTreeWidgetItem*)item );
      delete item;
    }
  }
}

void GuiUserList::showUserMenu( const QPoint& p )
{
  QTreeWidgetItem* item = itemAt( p );
  if( !item )
    return;

  GuiUserItem* user_item = (GuiUserItem*)item;
  emit menuToShow( user_item->userId() );
  clearSelection();
}

void GuiUserList::userItemClicked( QTreeWidgetItem* item, int )
{
  if( !item )
    return;

  GuiUserItem* user_item = (GuiUserItem*)item;
  emit chatSelected( user_item->chatId() );
  clearSelection();
}

void GuiUserList::setDefaultChatConnected( bool yes )
{
  GuiUserItem* item = itemFromChatId( ID_DEFAULT_CHAT );
  if( !item )
    return;
  item->setIcon( 0, QIcon( (yes ? ":/images/default-chat-online" : ":/images/default-chat-offline" ) ) );
}

void GuiUserList::setChatOpened( VNumber chat_id )
{
  m_chatOpened = chat_id;

  if( chat_id == ID_INVALID )
    return;

  Chat c = ChatManager::instance().chat( chat_id );
  if( !c.isValid() )
    return;

  GuiUserItem* item;
  QTreeWidgetItemIterator it( this );
  while( *it )
  {
    item = (GuiUserItem*)(*it);
    if( c.isPrivate() || c.isDefault() )
      item->setChatOpened( item->chatId() == chat_id );
    else if( item->chatId() == ID_DEFAULT_CHAT )
      item->setChatOpened( false );
    else
      item->setChatOpened( c.hasUser( item->userId() ) );
    ++it;
  }
}
