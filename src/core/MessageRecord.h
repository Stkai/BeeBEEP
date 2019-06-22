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

#ifndef BEEBEEP_MESSAGERECORD_H
#define BEEBEEP_MESSAGERECORD_H

#include "Config.h"
#include "Message.h"


class MessageRecord
{
public:
  MessageRecord();
  MessageRecord( const MessageRecord& );
  MessageRecord( VNumber to_user_id, VNumber chat_id, const Message& );

  MessageRecord& operator=( const MessageRecord& );

  inline bool isValid() const;

  inline VNumber toUserId() const;
  inline VNumber chatId() const;
  inline const Message& message() const;

private:
  VNumber m_toUserId;
  VNumber m_chatId;
  Message m_message;

};


// Inline Functions
inline bool MessageRecord::isValid() const { return m_toUserId != ID_INVALID && m_chatId != ID_INVALID && m_message.isValid(); }
inline VNumber MessageRecord::toUserId() const { return m_toUserId; }
inline VNumber MessageRecord::chatId() const { return m_chatId; }
inline const Message& MessageRecord::message() const { return m_message; }

#endif // BEEBEEP_CHATMESSAGE_H
