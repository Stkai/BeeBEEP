//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2019 Marco Mastroddi
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
// along with BeeBEEP. If not, see <http://www.gnu.org/licenses/>.
//
// Author: Marco Mastroddi <marco.mastroddi(AT)gmail.com>
//
// $Id$
//
//////////////////////////////////////////////////////////////////////

#include "MessageRecord.h"


MessageRecord::MessageRecord()
 : m_toUserId( ID_INVALID ), m_chatId( ID_INVALID ), m_message()
{
}

MessageRecord::MessageRecord( const MessageRecord& mr )
{
  (void)operator=( mr );
}

MessageRecord::MessageRecord( VNumber to_user_id, VNumber chat_id, const Message& m )
  : m_toUserId( to_user_id ), m_chatId( chat_id ), m_message( m )
{
}

MessageRecord& MessageRecord::operator=( const MessageRecord& mr )
{
  if( this != &mr )
  {
    m_toUserId = mr.m_toUserId;
    m_chatId = mr.m_chatId;
    m_message = mr.m_message;
  }
  return *this;
}

