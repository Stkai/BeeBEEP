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

#include "ChatManager.h"
#include "Core.h"
#include "Protocol.h"
#include "UserManager.h"


Chat Core::findChatFromMessageData( VNumber from_user_id, const Message& m )
{
  Chat c;
  ChatMessageData cmd = Protocol::instance().dataFromChatMessage( m );
  if( m.hasFlag( Message::Private ) )
  {
    c = ChatManager::instance().privateChatForUser( from_user_id );
  }
  else if( m.hasFlag( Message::GroupChat ) )
  {
    c = ChatManager::instance().findChatByPrivateId( cmd.groupId(), true, ID_INVALID );
    if( !c.isValid() )
    {
      Group g = UserManager::instance().findGroupByPrivateId( cmd.groupId() );
      if( g.isValid() )
      {
        qDebug() << "New message arrived for your group" << g.name() << "but chat does not exists";
        createGroupChat( g.name(), g.usersId(), g.privateId(), false );
        c = ChatManager::instance().findChatByPrivateId( cmd.groupId(), true, ID_INVALID );
      }
    }
  }
  else
    c = ChatManager::instance().defaultChat();

  return c;
}

void Core::dispatchChatMessageReceived( VNumber from_user_id, const Message& m )
{
  Chat c = findChatFromMessageData( from_user_id, m );

  if( !c.isValid() )
  {
    qWarning() << "Invalid message received from" << from_user_id;
    return;
  }

  if( !c.usersId().contains( from_user_id ) )
  {
    qWarning() << "User" << from_user_id << "is not present in the chat" << c.id() << c.name() << "... drop message:";
    qWarning() << m.text();
    return;
  }

  if( !c.usersId().contains( ID_LOCAL_USER ) )
  {
    qWarning() << "You are not in the chat" << c.id() << c.name() << "... drop message:";
    qWarning() << m.text();
    return;
  }

#ifdef BEEBEEP_DEBUG
  qDebug() << "Message dispatched to chat" << c.id();
#endif
  ChatMessage cm( from_user_id, m, ChatMessage::Chat );
  c.addMessage( cm );
  c.addUnreadMessage();
  c.setLastMessageTimestamp( m.timestamp() );
#ifdef BEEBEEP_DEBUG
  qDebug() << "Chat" << c.id() << "has" << c.unreadMessages() << "unread messages";
#endif
  ChatManager::instance().setChat( c );
  emit chatMessage( c.id(), cm );
}

void Core::dispatchChatMessageReadReceived( VNumber from_user_id, const Message& m )
{
  Chat c = findChatFromMessageData( from_user_id, m );
  if( !c.isValid() )
  {
    qWarning() << "Invalid chat message read received from" << from_user_id;
    return;
  }

#ifdef BEEBEEP_DEBUG
  qDebug() << "User" << from_user_id << "has read messages inc chat" << c.name();
#endif

  c.setReadMessagesByUser( from_user_id );
  ChatManager::instance().setChat( c );
  emit chatReadByUser( c.id(), from_user_id );
}

void Core::dispatchSystemMessage( VNumber chat_id, VNumber from_user_id, const QString& msg, DispatchType dt, ChatMessage::Type cmt )
{
  Message m = Protocol::instance().systemMessage( msg );
  ChatMessage cm( from_user_id, m, cmt );

  if( chat_id == ID_INVALID )
    chat_id = ID_DEFAULT_CHAT;

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
  case DispatchToDefaultAndPrivateChat:
    dispatchToDefaultAndPrivateChat( cm, from_user_id );
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
      emit chatMessage( chat_id, cm );
      return;
    }
    ++it;
  }
}

void Core::dispatchToDefaultAndPrivateChat( const ChatMessage& cm, VNumber user_id )
{
  emit chatMessage( ID_DEFAULT_CHAT, cm );
  Chat c = ChatManager::instance().privateChatForUser( user_id );
  if( c.isValid() )
    emit chatMessage( c.id(), cm );
}
