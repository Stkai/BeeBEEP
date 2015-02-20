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

#include "GuiGroupItem.h"
#include "Group.h"
#include "UserManager.h"
#include "ChatManager.h"


GuiGroupItem::GuiGroupItem( QTreeWidget* parent )
 : QTreeWidgetItem( parent )
{
  setItemId( ID_INVALID );
  setObjectType( ObjectInvalid );
}

GuiGroupItem::GuiGroupItem( QTreeWidgetItem* parent )
 : QTreeWidgetItem( parent )
{
  setItemId( ID_INVALID );
  setObjectType( ObjectInvalid );
}

void GuiGroupItem::init( VNumber item_id, bool is_group )
{
  setItemId( item_id );
  if( is_group )
    setObjectType( ObjectGroup );
  else
    setObjectType( ObjectUser );
}

bool GuiGroupItem::operator<( const GuiGroupItem& item ) const
{
  QString user_item_name = text( 0 ).toLower();
  QString other_name = item.text( 0 ).toLower();

  return user_item_name > other_name; // correct order
}

void GuiGroupItem::setGroupName( const QString& group_name, int unread_messages )
{
  if( unread_messages > 0 )
    setText( 0, QString( "(%1) %2" ).arg( unread_messages ).arg( group_name ) );
  else
    setText( 0, group_name );
}

bool GuiGroupItem::updateGroup( const Group& g )
{
  if( itemId() != g.id() )
    return false;
  if( !isGroup() )
    return false;

  setIcon( 0, QIcon( ":/images/group.png" ) );
  Chat c = ChatManager::instance().findGroupChatByPrivateId( g.privateId() );
  if( c.isValid() )
    setGroupName( g.name(), c.unreadMessages() );
  else
    setGroupName( g.name(), 0 );

  takeChildren();

  UserList user_list = UserManager::instance().userList().fromUsersId( g.usersId() );
  foreach( User u, user_list.toList() )
  {
    if( !u.isLocal() )
    {
      GuiGroupItem* user_item = new GuiGroupItem( this );
      user_item->init( u.id(), false );
      user_item->updateUser( u );
      addChild( user_item );
    }
  }

  return true;
}

bool GuiGroupItem::updateUser( const User& u )
{
  if( itemId() != u.id() )
    return false;
  if( isGroup() )
    return false;

  setIcon( 0, Bee::userStatusIcon( u.status() ) );
  setText( 0, u.name() );
  setToolTip( 0, u.path() );

  return true;
}

bool GuiGroupItem::updateChat( VNumber chat_id )
{
  Chat c = ChatManager::instance().chat( chat_id );
  if( !c.isValid() )
    return false;
  if( !c.isGroup() )
    return false;

  Group g = UserManager::instance().findGroupByPrivateId( c.privateId() );
  if( !g.isValid() )
    return false;

  if( g.id() == itemId() )
  {
    setGroupName( g.name(), c.unreadMessages() );
    return true;
  }
  else
    return false;
}
