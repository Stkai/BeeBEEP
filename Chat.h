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

#ifndef BEEBEEP_CHAT_H
#define BEEBEEP_CHAT_H

#include "Config.h"
#include "ChatMessage.h"


class Chat
{
public:
  Chat();
  Chat( const Chat& );

  Chat& operator=( const Chat& );
  inline bool operator==( const Chat& ) const;
  inline bool isValid() const;
  inline VNumber id() const;
  inline void setId( VNumber );
  inline const QList<VNumber>& usersId() const;
  bool addUser( VNumber );
  inline bool hasUser( VNumber ) const;
  inline bool removeUser( VNumber );
  inline bool isPrivateForUser( VNumber ) const;
  inline const QDateTime& lastMessageTimestamp() const;
  inline void setLastMessageTimestamp( const QDateTime& );
  inline int unreadMessages() const;
  inline void addUnreadMessage();
  inline void readAllMessages();
  inline const QList<ChatMessage> messages() const;
  inline void addMessage( const ChatMessage& );

private:
  VNumber m_id;
  QList<VNumber> m_usersId;
  QList<ChatMessage> m_messages;
  QDateTime m_lastMessageTimestamp;
  int m_unreadMessages;

};


// Inline Functions
inline bool Chat::operator==( const Chat& c ) const { return m_id == c.m_id; }
inline bool Chat::isValid() const { return m_id > 0; }
inline VNumber Chat::id() const { return m_id; }
inline void Chat::setId( VNumber new_value ) { m_id = new_value; }
inline const QList<VNumber>& Chat::usersId() const { return m_usersId; }
inline bool Chat::hasUser( VNumber user_id ) const { return m_usersId.contains( user_id ); }
inline bool Chat::removeUser( VNumber user_id ) { return m_usersId.removeOne( user_id ); }
inline bool Chat::isPrivateForUser( VNumber user_id ) const { return m_usersId.size() == 2 && hasUser( user_id ); }
inline const QDateTime& Chat::lastMessageTimestamp() const { return m_lastMessageTimestamp; }
inline void Chat::setLastMessageTimestamp( const QDateTime& new_value ) { m_lastMessageTimestamp = new_value; }
inline int Chat::unreadMessages() const { return m_unreadMessages; }
inline void Chat::addUnreadMessage() { m_unreadMessages++; }
inline void Chat::readAllMessages() { m_unreadMessages = 0; }
inline const QList<ChatMessage> Chat::messages() const { return m_messages; }
inline void Chat::addMessage( const ChatMessage& cm ) { m_messages.append( cm ); }


#endif // BEEBEEP_CHAT_H
