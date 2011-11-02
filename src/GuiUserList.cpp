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

#include "GuiUserList.h"
#include "User.h"


GuiUserList::GuiUserList( QWidget* parent )
  : QTreeWidget( parent )
{
  setObjectName( "GuiUserList" );

  setContextMenuPolicy( Qt::CustomContextMenu );
  setMouseTracking( true );

  setColumnCount( 1 );
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
  GuiUserItem* item;
  QTreeWidgetItemIterator it( this );
  while( *it )
  {
    item = (GuiUserItem*)(*it);
    item->updateItem();
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
  if( item )
  {
    item->setUnreadMessages( n );
    item->updateItem();
  }
  else
    qWarning() << "Unable to set unread messages in chat" << chat_id;
}

void GuiUserList::setUser( const User& u, VNumber private_chat_id, int unread_messages )
{
  GuiUserItem* item = itemFromUserId( u.id() );
  if( !item )
  {
    qDebug() << "Create new user item in GuiUserList";
    item = new GuiUserItem( this );
  }

  item->setUserId( u.id() );
  item->setChatId( private_chat_id );
  item->setUnreadMessages( unread_messages );
  item->updateItem();
  sortUsers();
}

void GuiUserList::removeUser( const User& u, bool erase )
{
  GuiUserItem* item = itemFromUserId( u.id() );
  if( item )
  {
    item->updateItem();
    if( erase )
    {
      qDebug() << "Delete user item from GuiUserList";
      QTreeWidgetItem* root_item = invisibleRootItem();
      if( root_item )
      {
        root_item->removeChild( (QTreeWidgetItem*)item );
        delete item;
      }
    }
    else
      sortUsers();
  }
  else
    qWarning() << "Unable to set user" << u.id() << "offline in GuiUserList";
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
