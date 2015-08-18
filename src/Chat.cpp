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
    m_lastMessageTimestamp(), m_unreadMessages( 0 ), m_privateId( "" )
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
    if( !cm.isSystem() )
      return false;
  }
  return true;
}

QDateTime Chat::dateTimeStarted() const
{
  if( m_messages.isEmpty() )
    return QDateTime::currentDateTime();
  else
    return m_messages.first().timestamp();
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
  if( m_usersId.size() != user_list.size() )
    return false;
  foreach( VNumber user_id, m_usersId )
  {
    if( !user_list.contains( user_id ) )
      return false;
  }
  return true;
}

void Chat::clearMessages()
{
  setLastMessageTimestamp( QDateTime::currentDateTime() );
  readAllMessages();

  ChatMessage cm_intro;
  if( isGroup() )
  {
    cm_intro = m_messages.first();
    m_messages.removeFirst();
    ChatMessage cm_intro_group = m_messages.first();
    m_messages.clear();
    addMessage( cm_intro );
    addMessage( cm_intro_group );
  }
  else
  {
    cm_intro = m_messages.first();
    m_messages.clear();
    addMessage( cm_intro );
  }
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
