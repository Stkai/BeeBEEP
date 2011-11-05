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
#include "Core.h"
#include "Protocol.h"


void Core::dispatchChatMessageReceived( VNumber from_user_id, const Message& m )
{
  Chat c = m.hasFlag( Message::Private ) ? ChatManager::instance().privateChatForUser( from_user_id ) : ChatManager::instance().defaultChat( false );
  qDebug() << "Message dispatched to chat" << c.id();
  ChatMessage cm( from_user_id, m );
  c.addMessage( cm );
  c.addUnreadMessage();
  c.setLastMessageTimestamp( m.timestamp() );
  qDebug() << "Chat" << c.id() << "has" << c.unreadMessages() << "unread messages";
  ChatManager::instance().setChat( c );
  emit chatMessage( c.id(), cm );
}

void Core::dispatchSystemMessage( VNumber chat_id, VNumber from_user_id, const QString& msg, DispatchType dt )
{
  qDebug() << "Dispatch system message to chat" << chat_id << "from user" << from_user_id << "with type" << (int)dt;
  Message m = Protocol::instance().systemMessage( msg );
  ChatMessage cm( from_user_id, m );

  switch( dt )
  {
  case DispatchToAll:
    dispatchToAllChats( cm );
    break;
  case DispatchToAllChatsWithUser:
    dispatchToAllChatsWithUser( cm, from_user_id );
    break;
  case DispatchToChat:
    dispatchToChat( cm, chat_id );
    break;
  default:
    qWarning() << "Invalid dispatch type found while dispatching a system message";
    dispatchToChat( cm, ID_DEFAULT_CHAT );
  }
}

void Core::dispatchToAllChats( const ChatMessage& cm )
{
  QList<Chat>::iterator it = ChatManager::instance().chatList().begin();
  while( it != ChatManager::instance().chatList().end() )
  {
    (*it).addMessage( cm );
    emit chatMessage( (*it).id(), cm );
    ++it;
  }
}

void Core::dispatchToAllChatsWithUser( const ChatMessage& cm, VNumber user_id )
{
  QList<Chat>::iterator it = ChatManager::instance().chatList().begin();
  while( it != ChatManager::instance().chatList().end() )
  {
    if( (*it).hasUser( user_id ) )
    {
      (*it).addMessage( cm );
      emit chatMessage( (*it).id(), cm );
    }
    ++it;
  }
}

void Core::dispatchToChat( const ChatMessage& cm, VNumber chat_id )
{
  QList<Chat>::iterator it = ChatManager::instance().chatList().begin();
  while( it != ChatManager::instance().chatList().end() )
  {
    if( (*it).id() == chat_id )
    {
      (*it).addMessage( cm );
      emit chatMessage( (*it).id(), cm );
      return;
    }
    ++it;
  }
}
