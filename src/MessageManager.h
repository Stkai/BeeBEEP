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
// $Id: NetworkManager.h 407 2015-06-10 16:51:41Z mastroddi $
//
//////////////////////////////////////////////////////////////////////

#ifndef BEEBEEP_MESSAGEMANAGER_H
#define BEEBEEP_MESSAGEMANAGER_H

#include "Config.h"
#include "MessageRecord.h"


class MessageManager
{
// Singleton Object
  static MessageManager* mp_instance;

public:
  void addMessageToSend( VNumber to_user_id, VNumber chat_id, const Message& );
  QList<MessageRecord> takeMessagesToSend( VNumber user_id );

  static MessageManager& instance()
  {
    if( !mp_instance )
      mp_instance = new MessageManager();
    return *mp_instance;
  }

  static void close()
  {
    if( mp_instance )
    {
      delete mp_instance;
      mp_instance = NULL;
    }
  }

protected:
  MessageManager();

private:
  QList<MessageRecord> m_messagesToSend;

};

#endif // BEEBEEP_MESSAGEMANAGER_H
