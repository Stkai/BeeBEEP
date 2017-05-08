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
  inline const QString& name() const;
  inline void setName( const QString& );
  inline const QList<VNumber>& usersId() const;
  bool addUser( VNumber );
  inline bool hasUser( VNumber ) const;
  bool hasUsers( const QList<VNumber>& );
  inline bool removeUser( VNumber );
  inline bool isPrivateForUser( VNumber ) const;
  inline bool isPrivate() const;
  VNumber privateUserId() const;
  inline const QDateTime& lastMessageTimestamp() const;
  inline void setLastMessageTimestamp( const QDateTime& );
  inline int unreadMessages() const;
  int chatMessages() const;
  inline void addUnreadMessage();
  inline void readAllMessages();
  inline const QList<ChatMessage> messages() const;
  void addMessage( const ChatMessage& );
  void clearMessages();
  inline const QString& privateId() const;
  inline void setPrivateId( const QString& );
  inline const QList<VNumber>& unreadMessageUsersId() const;
  inline void setReadMessagesByUser( VNumber );
  inline bool userHasReadMessages( VNumber ) const;

  bool isEmpty() const;
  bool isDefault() const;
  inline bool isGroup() const;

private:
  VNumber m_id;
  QString m_name;
  QList<VNumber> m_usersId;
  QList<ChatMessage> m_messages;
  QDateTime m_lastMessageTimestamp;
  int m_unreadMessages;
  QString m_privateId;
  QList<VNumber> m_unreadMessageUsersId;

};


// Inline Functions
inline bool Chat::operator==( const Chat& c ) const { return m_id == c.m_id; }
inline bool Chat::isValid() const { return m_id != ID_INVALID; }
inline bool Chat::isDefault() const { return m_id == ID_DEFAULT_CHAT; }
inline VNumber Chat::id() const { return m_id; }
inline void Chat::setId( VNumber new_value ) { m_id = new_value; }
inline const QString& Chat::name() const { return m_name; }
inline void Chat::setName( const QString& new_value ) { m_name = new_value; }
inline const QList<VNumber>& Chat::usersId() const { return m_usersId; }
inline bool Chat::hasUser( VNumber user_id ) const { return isDefault() ? true : m_usersId.contains( user_id ); }
inline bool Chat::removeUser( VNumber user_id ) { return m_usersId.removeOne( user_id ); }
inline bool Chat::isPrivateForUser( VNumber user_id ) const { return !isDefault() && !isGroup() && m_usersId.size() == 2 && hasUser( user_id ); }
inline bool Chat::isPrivate() const { return !isDefault() && !isGroup() && m_usersId.size() == 2 && hasUser( ID_LOCAL_USER ); }
inline const QDateTime& Chat::lastMessageTimestamp() const { return m_lastMessageTimestamp; }
inline void Chat::setLastMessageTimestamp( const QDateTime& new_value ) { m_lastMessageTimestamp = new_value; }
inline int Chat::unreadMessages() const { return m_unreadMessages; }
inline void Chat::addUnreadMessage() { m_unreadMessages++; }
inline void Chat::readAllMessages() { m_unreadMessages = 0; }
inline const QList<ChatMessage> Chat::messages() const { return m_messages; }
inline const QString& Chat::privateId() const { return m_privateId; }
inline void Chat::setPrivateId( const QString& new_value ) { m_privateId = new_value; }
inline bool Chat::isGroup() const { return !m_privateId.isEmpty() && !isDefault(); }
inline void Chat::setReadMessagesByUser( VNumber user_id ) { m_unreadMessageUsersId.removeOne( user_id ); }
inline bool Chat::userHasReadMessages( VNumber user_id ) const { return !m_unreadMessageUsersId.contains( user_id ); }

#endif // BEEBEEP_CHAT_H
