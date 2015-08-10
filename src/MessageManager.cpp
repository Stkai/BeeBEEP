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
// $Id: AudioManager.cpp 435 2015-07-31 17:34:50Z mastroddi $
//
//////////////////////////////////////////////////////////////////////

#include "MessageManager.h"


MessageManager* MessageManager::mp_instance = NULL;

MessageManager::MessageManager()
  : m_messagesToSend()
{
}

void MessageManager::addMessageToSend( VNumber to_user_id, VNumber chat_id, const Message& m )
{
  MessageRecord mr( to_user_id, chat_id, m );
  m_messagesToSend.append( mr );
}

QList<MessageRecord> MessageManager::takeMessagesToSend( VNumber user_id )
{
  QList<MessageRecord> message_list;
  QList<MessageRecord>::iterator it = m_messagesToSend.begin();
  while( it != m_messagesToSend.end() )
  {
    if( (*it).toUserId() == user_id )
    {
      message_list.append( *it );
      it = m_messagesToSend.erase( it );
    }
    else
      ++it;
  }
  return message_list;
}
