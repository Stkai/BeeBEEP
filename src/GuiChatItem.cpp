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

#include "GuiChatItem.h"
#include "ChatManager.h"
#include "UserManager.h"


GuiChatItem::GuiChatItem( QTreeWidget* parent )
  : QTreeWidgetItem( parent )
{
}

bool GuiChatItem::operator<( const QTreeWidgetItem& item ) const
{
  QString user_item_name = data( 0, GuiChatItem::ChatName ).toString().toLower();
  QString other_name = item.data( 0, GuiChatItem::ChatName ).toString().toLower();

  if( chatId() == ID_DEFAULT_CHAT )
    return false;

  if( Bee::qVariantToVNumber( item.data( 0, GuiChatItem::ChatId ) ) == ID_DEFAULT_CHAT )
    return true;

  if( isGroup() && !item.data( 0, GuiChatItem::ChatIsGroup ).toBool() )
    return false;

  if( !isGroup() && item.data( 0, GuiChatItem::ChatIsGroup ).toBool() )
    return true;

  return user_item_name > other_name; // correct order
}

bool GuiChatItem::updateItem()
{
  setIcon( 0, QIcon( ":/images/chat.png" ) );

  QString chat_name;
  QString tool_tip;

  if( chatId() == ID_DEFAULT_CHAT )
  {
    chat_name = QObject::tr( "All Lan Users" );
    tool_tip = QObject::tr( "Open chat with all local users" );
    setData( 0, ChatName, " " );
  }
  else
  {
    Chat c = ChatManager::instance().chat( chatId() );
    if( !c.isValid() )
      return false;

    UserList user_list = UserManager::instance().userList().fromUsersId( c.usersId() );
    QStringList sl;
    foreach( User u, user_list.toList() )
    {
      if( !u.isLocal() )
        sl.append( u.name() );
    }

    chat_name = c.name().isEmpty() ? (sl.size() == 0 ? QObject::tr( "Nobody" ) : sl.join( ", " )) : c.name();

    tool_tip = QObject::tr( "Open chat with %1" ).arg( chat_name );

    if( c.unreadMessages() > 0 )
      chat_name.prepend( QString( "(%1) " ).arg( c.unreadMessages() ) );

    setData( 0, ChatName, chat_name );

    setIsGroup( c.isGroup() );
  }

  chat_name += " ";
  setText( 0, chat_name );
  setToolTip( 0, tool_tip );
  setStatusTip( 0, tool_tip );
  return true;
}
