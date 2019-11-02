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

#include "Chat.h"


Chat::Chat()
  : m_group(), m_messages(), m_lastMessageTimestamp(), m_unreadMessages( 0 ),
    m_unreadMessageUsersId()
{
}

Chat::Chat( const Chat& c )
{
  (void)operator=( c );
}

Chat& Chat::operator=( const Chat& c )
{
  if( this != &c )
  {
    m_group = c.m_group;
    m_messages = c.m_messages;
    m_lastMessageTimestamp = c.m_lastMessageTimestamp;
    m_unreadMessages = c.m_unreadMessages;
    m_unreadMessageUsersId = c.m_unreadMessageUsersId;
  }
  return *this;
}

bool Chat::isEmpty() const
{
  foreach( ChatMessage cm, m_messages )
  {
    if( cm.type() == ChatMessage::Chat || cm.type() == ChatMessage::FileTransfer || cm.type() == ChatMessage::History || cm.type() == ChatMessage::Voice )
      return false;
  }
  return true;
}

VNumber Chat::privateUserId() const
{
  foreach( VNumber user_id, m_group.usersId() )
  {
    if( user_id != ID_LOCAL_USER )
      return user_id;
  }
  return ID_INVALID;
}

void Chat::clearMessages()
{
  setLastMessageTimestamp( QDateTime() );
  readAllMessages();
  m_messages.clear();
}

void Chat::clearSystemMessages()
{
  QList<ChatMessage>::iterator it = m_messages.begin();
  bool skip_first_system_message = isDefault();
  while( it != m_messages.end() )
  {
    if( skip_first_system_message )
    {
      skip_first_system_message = false;
      ++it;
    }
    else if( it->isSystemActivity() )
      it = m_messages.erase( it );
    else
      ++it;
  }
}

bool Chat::hasSystemMessages() const
{
  int min_system_messages = isDefault() ? 2 : 1;
  int counter = 0;
  foreach( ChatMessage cm, m_messages )
  {
    if( cm.isSystemActivity() )
      counter++;

    if( counter > min_system_messages )
      return true;
  }
  return false;
}

int Chat::chatMessages() const
{
  int chat_messages = 0;
  foreach( ChatMessage cm, m_messages )
  {
    if( cm.type() == ChatMessage::Chat )
      chat_messages++;
  }
  return chat_messages;
}

void Chat::addMessage( const ChatMessage& cm )
{
  if( !cm.isValid() )
    return;

  m_messages.append( cm );

  if( cm.isFromLocalUser() && cm.type() == ChatMessage::Chat )
  {
    foreach( VNumber user_id, m_group.usersId() )
    {
      if( user_id == ID_LOCAL_USER )
        continue;

      if( !m_unreadMessageUsersId.contains( user_id ) )
        m_unreadMessageUsersId.append( user_id );
    }
  }
}

bool Chat::hasMinimumUsersForGroup() const
{
  int chat_members = 0;
  foreach( VNumber user_id, m_group.usersId() )
  {
    if( user_id != ID_LOCAL_USER )
      chat_members++;
  }
  return chat_members >= 2;
}
