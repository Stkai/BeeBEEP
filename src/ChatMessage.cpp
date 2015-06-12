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

#include "ChatMessage.h"


ChatMessage::ChatMessage()
  : m_userId( 0 ), m_message(), m_type( ChatMessage::Other )
{
}

ChatMessage::ChatMessage( const ChatMessage& cm )
{
  (void)operator=( cm );
}

ChatMessage::ChatMessage( VNumber user_id, const Message& m, ChatMessage::Type cmt )
  : m_userId( user_id ), m_message( m ), m_type( cmt )
{
}

ChatMessage& ChatMessage::operator=( const ChatMessage& cm )
{
  if( this != &cm )
  {
    m_userId = cm.m_userId;
    m_message = cm.m_message;
    m_type = cm.m_type;
  }
  return *this;
}

