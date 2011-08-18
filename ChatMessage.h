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

#ifndef BEEBEEP_CHATMESSAGE_H
#define BEEBEEP_CHATMESSAGE_H

#include "Config.h"
#include "Message.h"
class User;


class ChatMessage
{
public:
  ChatMessage();
  ChatMessage( const ChatMessage& );
  ChatMessage( VNumber user_id, const Message& );

  ChatMessage& operator=( const ChatMessage& );

  inline bool isValid() const;
  inline bool isSystem() const;
  inline bool isFromLocal() const;
  inline void setLocal( bool );
  inline const Message& message() const;
  inline void setMessage( const Message& );

private:
  VNumber m_userId;
  Message m_message;

};


// Inline Functions

inline bool ChatMessage::isValid() const { return m_userId > 0 && m_message.isValid(); }
inline bool ChatMessage::isSystem() const { return m_message.type() == Message::System; }
inline bool ChatMessage::isLocal() const { return m_userId == ID_LOCAL_USER; }
inline const Message& ChatMessage::message() const { return m_message; }
inline void ChatMessage::setMessage( const Message& new_value ) { m_message = new_value; }


#endif // BEEBEEP_CHATMESSAGE_H

