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

#include "ChatManager.h"
#include "GuiUserList.h"
#include "Settings.h"
#include "UserManager.h"


GuiUserList::GuiUserList( QWidget* parent )
  : QTreeWidget( parent )
{
  setObjectName( "GuiUserList" );

  setContextMenuPolicy( Qt::CustomContextMenu );
  setMouseTracking( true );

  header()->hide();
  setRootIsDecorated( false );
  setSortingEnabled( true );

  connect( this, SIGNAL( itemDoubleClicked( QTreeWidgetItem*, int ) ), this, SLOT( userDoubleClicked( QTreeWidgetItem*, int ) ) );
  connect( this, SIGNAL( customContextMenuRequested( const QPoint& ) ), this, SLOT( showUserMenu( const QPoint& ) ) );
}

void GuiUserList::sortUsers()
{
  sortItems( 0, Qt::AscendingOrder );
}

QSize GuiUserList::sizeHint() const
{
  return QSize( 140, 300 );
}

void GuiUserList::updateUsers()
{
  clear();
  setUser( Settings::instance().localUser() );
  QList<User>::const_iterator it = UserManager::instance().userList().toList().begin();
  while( it != UserManager::instance().userList().toList().end() )
  {
    setUser( *it );
    ++it;
  }
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
  item->updateItem();
}

void GuiUserList::setUser( const User& u )
{
  GuiUserItem* item = itemFromUserId( u.id() );
  bool user_item_created = false;
  if( !item )
  {
    if( u.isConnected() )
    {
      item = new GuiUserItem( this );
      item->setUserId( u.id() );
      user_item_created = true;
    }
    else
      return;
  }

  if( !u.isConnected() && Settings::instance().showOnlyOnlineUsers() )
  {
    removeUser( u );
    return;
  }

  Chat c = ChatManager::instance().privateChatForUser( u.id() );
  item->setChatId( c.id() );
  item->setUnreadMessages( c.unreadMessages() );
  item->updateItem();
  if( !user_item_created )
    sortUsers();
}

void GuiUserList::removeUser( const User& u )
{
  GuiUserItem* item = itemFromUserId( u.id() );
  if( item )
  {
    qDebug() << "Delete user item from GuiUserList";
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
}

void GuiUserList::userDoubleClicked( QTreeWidgetItem* item, int )
{
  if( !item )
    return;

  GuiUserItem* user_item = (GuiUserItem*)item;
  emit chatSelected( user_item->chatId() );
}

bool GuiUserList::nextUserWithUnreadMessages()
{
  GuiUserItem* item;
  QTreeWidgetItemIterator it( this );
  while( *it )
  {
    item = (GuiUserItem*)(*it);
    if( item->unreadMessages() > 0 )
    {
      emit chatSelected( item->chatId() );
      return true;
    }
    ++it;
  }
  return false;
}
