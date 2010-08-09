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

#include "ChatMessage.h"
#include "User.h"


ChatMessage::ChatMessage()
  : m_username( "" ), m_nickname( "" ), m_message()
{
}

ChatMessage::ChatMessage( const ChatMessage& cm )
{
  (void)operator=( cm );
}

ChatMessage::ChatMessage( const User& u, const Message& m )
  : m_username( u.name() ), m_nickname( u.nickname() ), m_message( m )
{
}

ChatMessage& ChatMessage::operator=( const ChatMessage& cm )
{
  if( this != &cm )
  {
    m_username = cm.m_username;
    m_nickname = cm.m_nickname;
    m_message = cm.m_message;
  }
  return *this;
}

