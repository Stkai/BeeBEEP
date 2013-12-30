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

int Chat::addUsers( const QList<VNumber>& user_list )
{
  int num = 0;
  foreach( VNumber user_id, user_list )
  {
    if( addUser( user_id ) )
      num++;
  }
  return num;
}

bool Chat::isEmpty() const
{
  foreach( ChatMessage cm, m_messages )
  {
    if( !cm.isSystem() && !cm.isFromLocalUser() )
      return false;
  }
  return true;
}
