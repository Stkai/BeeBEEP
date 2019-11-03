//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2019 Marco Mastroddi
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

#include "GuiChatItem.h"
#include "Chat.h"
#include "ChatManager.h"
#include "IconManager.h"
#include "MessageManager.h"
#include "Settings.h"
#include "UserManager.h"


GuiChatItem::GuiChatItem( QTreeWidget* parent )
 : QTreeWidgetItem( parent )
{
  setData( 0, ChatId, 0 );
  setData( 0, ChatName, "" );
  setData( 0, ChatIsGroup, false );
  setData( 0, ChatUnreadMessages, 0 );
  setData( 0, ChatMessages, 0 );
  setData( 0, ChatLastMessageTimestamp, QDateTime( QDate( 1975, 4, 6), QTime( 7, 15 ) ) );
}

bool GuiChatItem::operator<( const QTreeWidgetItem& item ) const
{
  if( chatId() == ID_DEFAULT_CHAT )
    return false;

  if( Bee::qVariantToVNumber( item.data( 0, GuiChatItem::ChatId ) ) == ID_DEFAULT_CHAT )
    return true;

  int chat_messages = unreadMessages();
  int other_messages = item.data( 0, GuiChatItem::ChatUnreadMessages ).toInt();
  if( chat_messages != other_messages )
    return chat_messages < other_messages;

  int chat_online_users = onlineUsers();
  int other_online_users = item.data( 0, GuiChatItem::ChatOnlineUsers ).toInt();
  if( chat_online_users != other_online_users )
    return chat_online_users < other_online_users;

  chat_messages = data( 0, GuiChatItem::ChatMessages ).toInt();
  other_messages = item.data( 0, GuiChatItem::ChatMessages ).toInt();
  if( chat_messages != other_messages )
    return chat_messages < other_messages;

  QDateTime chat_timestamp = lastMessageTimestamp();
  QDateTime other_timestamp = item.data( 0, GuiChatItem::ChatLastMessageTimestamp ).toDateTime();

  if( chat_timestamp != other_timestamp )
    return chat_timestamp < other_timestamp;

  return data( 0, GuiChatItem::ChatName ).toString().toLower() < item.data( 0, GuiChatItem::ChatName ).toString().toLower();
}

QString GuiChatItem::defaultChatName()
{
  return QObject::tr( "All users" );
}

bool GuiChatItem::updateItem( const Chat& c )
{
  QString chat_name;
  QString tool_tip;
  m_defaultIcon = QIcon();
  int unsent_messages = MessageManager::instance().countMessagesToSendInChatId( c.id() );
  int online_users = 0;

  if( c.isDefault() )
  {
    chat_name = defaultChatName();
    tool_tip = QObject::tr( "Open chat with all users" );
    setData( 0, ChatName, " " );
    m_defaultIcon = IconManager::instance().icon( "default-chat-online.png" );
  }
  else
  {
    UserList user_list = UserManager::instance().userList().fromUsersId( c.usersId() );
    QStringList sl;
    foreach( User u, user_list.toList() )
    {
      if( !u.isLocal() && u.isValid() )
      {
        QString user_name = Bee::userNameToShow( u, false );
        sl.append( user_name );
        if( c.isPrivateForUser( u.id() ) )
          m_defaultIcon = Bee::avatarForUser( u, Settings::instance().avatarIconSize(), Settings::instance().showUserPhoto() );
        if( u.isStatusConnected() )
          online_users++;
      }
    }

    if( c.isGroup() )
    {
      chat_name = c.name();
      m_defaultIcon = IconManager::instance().icon( "group.png" );
    }
    else
      chat_name = sl.isEmpty() ? c.name() : sl.first();

    tool_tip = QObject::tr( "Open chat with %1" ).arg( chat_name );

    setData( 0, ChatName, chat_name );
    setIsGroup( c.isGroup() );
  }

  if( unsent_messages > 0 )
    tool_tip += QString( "\n[%1 %2]" ).arg( unsent_messages ).arg( QObject::tr( "unsent messages" ) );

  if( c.unreadMessages() > 0 )
    chat_name.prepend( QString( "(%1) " ).arg( c.unreadMessages() ) );

  chat_name += " ";
  setText( 0, c.isDefault() ? chat_name.toUpper() : chat_name );
  setToolTip( 0, tool_tip );
  setData( 0, ChatUnreadMessages, c.unreadMessages() );
  setData( 0, ChatMessages, c.messages().size() + ChatManager::instance().savedChatSize( c.name() ) );
  setData( 0, ChatOnlineUsers, online_users );
  if( c.lastMessageTimestamp().isValid() )
    setData( 0, ChatLastMessageTimestamp, c.lastMessageTimestamp() );
  if( m_defaultIcon.isNull() )
    m_defaultIcon = IconManager::instance().icon( "chat.png" );

  if( !c.isDefault() )
  {
    if( !Settings::instance().localUser().isStatusConnected() || online_users == 0 )
      m_defaultIcon = Bee::convertToGrayScale( m_defaultIcon, Settings::instance().avatarIconSize() );
  }

  setIcon( 0, m_defaultIcon );
  onTickEvent( 2 );

  return true;
}

void GuiChatItem::onTickEvent( int ticks )
{
  if( unreadMessages() > 0 )
  {
    if( ticks % 2 == 0 )
      setIcon( 0, IconManager::instance().icon( "beebeep-message.png" ) );
    else
      setIcon( 0, m_defaultIcon );
  }
}
