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
#include "GuiUserItem.h"
#include "Group.h"
#include "UserManager.h"


GuiGroupItem::GuiGroupItem( QTreeWidget* parent )
 : QTreeWidgetItem( parent )
{
  setItemId( ID_INVALID );
  setChatId( ID_INVALID );
  setObjectType( ObjectInvalid );
}

GuiGroupItem::GuiGroupItem( QTreeWidgetItem* parent )
 : QTreeWidgetItem( parent )
{
  setItemId( ID_INVALID );
  setChatId( ID_INVALID );
  setObjectType( ObjectInvalid );
}

bool GuiGroupItem::operator<( const QTreeWidgetItem& item ) const
{
  QString user_item_name = text( 0 ).toLower();
  QString other_name = item.text( 0 ).toLower();

  return user_item_name > other_name; // correct order
}

bool GuiGroupItem::updateGroup( const Group& g )
{
  setItemId( g.id() );

  setIcon( 0, QIcon( ":/images/group.png" ) );
  setText( 0, g.name() );

  takeChildren();

  UserList user_list = UserManager::instance().userList().fromUsersId( g.usersId() );
  foreach( User u, user_list.toList() )
  {
    GuiUserItem* user_item = new GuiUserItem( this );
    user_item->setUserId( u.id() );
    user_item->updateUser( u );
    user_item->setUnreadMessages( 0 );
    user_item->setToolTip( 0, "" );
    user_item->setStatusTip( 0, "" );
    user_item->setUnreadMessages( 0 );
  }

  return true;
}

bool GuiGroupItem::updateUser( const User& u )
{
  QTreeWidgetItemIterator it( this );

}
