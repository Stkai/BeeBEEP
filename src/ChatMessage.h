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

#ifndef BEEBEEP_CHATMESSAGE_H
#define BEEBEEP_CHATMESSAGE_H

#include "Config.h"
#include "Message.h"
class User;


class ChatMessage
{
public:
  enum Type { Header, System, Chat, Connection, UserStatus, UserInfo, FileTransfer, History, Other, NumTypes };

  ChatMessage();
  ChatMessage( const ChatMessage& );
  ChatMessage( VNumber user_id, const Message&, ChatMessage::Type );

  ChatMessage& operator=( const ChatMessage& );

  inline bool isValid() const;
  inline bool isSystem() const;
  inline bool isFromLocalUser() const;

  inline VNumber userId() const;
  inline const Message& message() const;
  inline void setMessage( const Message& );
  inline ChatMessage::Type type() const;

private:
  VNumber m_userId;
  Message m_message;
  Type m_type;

};


// Inline Functions
inline bool ChatMessage::isValid() const { return m_userId != ID_INVALID && m_message.isValid(); }
inline bool ChatMessage::isSystem() const { return m_message.type() == Message::System; }
inline bool ChatMessage::isFromLocalUser() const { return m_userId == ID_LOCAL_USER; }
inline VNumber ChatMessage::userId() const { return m_userId; }
inline const Message& ChatMessage::message() const { return m_message; }
inline void ChatMessage::setMessage( const Message& new_value ) { m_message = new_value; }
inline ChatMessage::Type ChatMessage::type() const { return m_type; }

#endif // BEEBEEP_CHATMESSAGE_H

