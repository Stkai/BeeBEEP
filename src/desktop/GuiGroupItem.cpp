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

#include "BeeUtils.h"
#include "ChatManager.h"
#include "GuiGroupItem.h"
#include "Group.h"
#include "IconManager.h"
#include "Settings.h"
#include "UserManager.h"



GuiGroupItem::GuiGroupItem( QTreeWidget* parent )
 : QTreeWidgetItem( parent )
{
  setItemId( ID_INVALID );
  setObjectType( ObjectInvalid );
  m_unreadMessages = 0;
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
  return text( 0 ).toLower() < item.text( 0 ).toLower();
}

void GuiGroupItem::setGroupName( const QString& group_name, int unread_messages )
{
  m_unreadMessages = unread_messages;
  if( m_unreadMessages > 0 )
    setText( 0, QString( "(%1) %2" ).arg( unread_messages ).arg( group_name ) );
  else
    setText( 0, group_name );
  setToolTip( 0, QObject::tr( "Click to send message to group: %1" ).arg( group_name ) );
}

bool GuiGroupItem::updateUser( const User& u )
{
  if( itemId() != u.id() )
    return false;
  if( isGroup() )
    return false;

  setIcon( 0, Bee::avatarForUser( u, Settings::instance().avatarIconSize(), true ) );
  setText( 0, u.name() );
  if( u.isStatusConnected() )
    setToolTip( 0, Bee::toolTipForUser( u, false ) );
  else
    setToolTip( 0, "" );

  return true;
}

bool GuiGroupItem::updateChat( const Chat& c )
{
  if( !c.isValid() )
    return false;
  if( !c.isGroup() )
    return false;
  if( c.id() != itemId() )
    return false;

  setIcon( 0, IconManager::instance().icon( "group.png" ) );
  setGroupName( c.name(), c.unreadMessages() );

  QList<QTreeWidgetItem*> group_children = takeChildren();
  qDeleteAll( group_children );
  UserList user_list = UserManager::instance().userList().fromUsersId( c.usersId() );
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

void GuiGroupItem::onTickEvent( int ticks )
{
  if( m_unreadMessages > 0 )
  {
    if( ticks % 2 == 0 )
      setIcon( 0, IconManager::instance().icon( "beebeep-message.png" ) );
    else
      setIcon( 0, IconManager::instance().icon( "group.png" ) );
  }
}
