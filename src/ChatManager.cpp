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

ChatManager* ChatManager::mp_instance = NULL;


ChatManager::ChatManager()
  : m_chats(), m_history(), m_isLoadHistoryCompleted( false )
{
}

Chat ChatManager::chat( VNumber chat_id, bool read_all_messages )
{
  QList<Chat>::iterator it = m_chats.begin();
  while( it != m_chats.end() )
  {
    if( chat_id == (*it).id() )
    {
      if( read_all_messages )
        (*it).readAllMessages();
      return *it;
    }
    ++it;
  }
  qWarning() << "Unable to find chat with id" << chat_id;
  return Chat();
}

Chat ChatManager::privateChatForUser( VNumber user_id ) const
{
  if( user_id == ID_LOCAL_USER )
    return chat( ID_DEFAULT_CHAT );
  QList<Chat>::const_iterator it = m_chats.begin();
  while( it != m_chats.end() )
  {
    if( (*it).isPrivateForUser( user_id ) )
      return *it;
    ++it;
  }
  qWarning() << "Unable to find private chat for user id" << user_id;
  return Chat();
}

Chat ChatManager::chat( VNumber chat_id ) const
{
  QList<Chat>::const_iterator it = m_chats.begin();
  while( it != m_chats.end() )
  {
    if( chat_id == (*it).id() )
      return *it;
    ++it;
  }
  qWarning() << "Unable to find chat with id" << chat_id;
  return Chat();
}

Chat ChatManager::groupChat( const QString& chat_id ) const
{
  QList<Chat>::const_iterator it = m_chats.begin();
  while( it != m_chats.end() )
  {
    if( chat_id == (*it).privateId() )
      return *it;
    ++it;
  }
  qWarning() << "Unable to find group chat with private id" << chat_id;
  return Chat();
}


void ChatManager::setChat( const Chat& c )
{
  QList<Chat>::iterator it = m_chats.begin();
  while( it != m_chats.end() )
  {
    if( (*it).id() == c.id() )
    {
      (*it) = c;
      return;
    }
    ++it;
  }
  m_chats.append( c );
}

int ChatManager::unreadMessages() const
{
  int unread_messages = 0;
  QList<Chat>::const_iterator it = m_chats.begin();
  while( it != m_chats.end() )
  {
    unread_messages += (*it).unreadMessages();
    ++it;
  }
  return unread_messages;
}

bool ChatManager::hasName( const QString& chat_name ) const
{
  foreach( Chat c, m_chats )
  {
    if( c.name() == chat_name )
      return true;
  }
  return false;
}

bool ChatManager::isGroupChat( VNumber chat_id ) const
{
  if( chat_id == ID_DEFAULT_CHAT )
    return false;
  Chat c = chat( chat_id );
  return c.isGroup();
}

QList<Chat> ChatManager::groupChatForUser( VNumber user_id ) const
{
  QList<Chat> chat_list;
  foreach( Chat c, m_chats )
  {
    if( c.isGroup() && c.usersId().contains( user_id ) )
      chat_list.append( c );
  }
  return chat_list;
}

