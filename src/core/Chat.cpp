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

#include "Chat.h"


Chat::Chat()
  : m_id( ID_INVALID ), m_name( "" ), m_usersId(), m_messages(),
    m_lastMessageTimestamp(), m_unreadMessages( 0 ), m_privateId( "" ),
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
    m_id = c.m_id;
    m_name = c.m_name;
    m_usersId = c.m_usersId;
    m_messages = c.m_messages;
    m_lastMessageTimestamp = c.m_lastMessageTimestamp;
    m_unreadMessages = c.m_unreadMessages;
    m_privateId = c.m_privateId;
    m_unreadMessageUsersId = c.m_unreadMessageUsersId;
  }
  return *this;
}

bool Chat::addUser( VNumber user_id )
{
  if( hasUser( user_id ) )
    return false;
  m_usersId.append( user_id );
  return true;
}

bool Chat::isEmpty() const
{
  foreach( ChatMessage cm, m_messages )
  {
    if( cm.type() == ChatMessage::Chat || cm.type() == ChatMessage::FileTransfer || cm.type() == ChatMessage::History )
      return false;
  }
  return true;
}

VNumber Chat::privateUserId() const
{
  foreach( VNumber user_id, m_usersId )
  {
    if( user_id != ID_LOCAL_USER )
      return user_id;
  }
  return ID_INVALID;
}

bool Chat::hasUsers( const QList<VNumber>& user_list )
{
  foreach( VNumber user_id, user_list )
  {
    if( !hasUser( user_id ) )
      return false;
  }
  return true;
}

void Chat::clearMessages()
{
  setLastMessageTimestamp( QDateTime() );
  readAllMessages();
  m_messages.clear();
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
    foreach( VNumber user_id, m_usersId )
    {
      if( user_id == ID_LOCAL_USER )
        continue;

      if( !m_unreadMessageUsersId.contains( user_id ) )
        m_unreadMessageUsersId.append( user_id );
    }
  }
}
