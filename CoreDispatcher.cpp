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

#include "Core.h"


void Core::dispatchChatMessageReceived( VNumber from_user_id, const Message& m )
{
  Chat c = m.hasFlag( Message::Private ) ? privateChatForUser( from_user_id ) : defaultChat();
  ChatMessage cm( u, m );
  c.addMessage( cm );
  c.addUnreadMessage();
  c.setLastMessageTimestamp( m.timestamp() );
  setChat( c );
  emit chatMessage( c.id(), cm );
}

void Core::dispatchSystemMessage( VNumber chat_id, VNumber from_user_id, const QString& msg )
{
  Message m = Protocol::instance().systemMessage( msg );
  ChatMessage cm( from_user_id, m );
  if( chat_id == ID_ALL_CHATS )
  {
    dispatchToAllChats( cm );
    return;
  }

  Chat c = privateChatForUser( from_user_id );
  c.addMessage( cm );
  setChat( c );
  emit chatMessage( c.id(), cm );

  if( from_user_id == chat_id )
    return;

  c = defaultChat();
  c.addMessage( cm );
  setChat( c );
  emit chatMessage( c.id(), cm );
}

void Core::dispatchToAllChats( const ChatMessage& cm )
{
  QList<Chat>::iterator it = m_chats.begin();
  while( it != m_chats.end() )
  {
    (*it).addMessage( cm );
    emit chatMessage( (*it).id(), cm );
    ++it;
  }
}
