//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2021 Marco Mastroddi
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

#ifndef BEEBEEP_MESSAGEMANAGER_H
#define BEEBEEP_MESSAGEMANAGER_H

#include "Config.h"
#include "MessageRecord.h"


class MessageManager
{
// Singleton Object
  static MessageManager* mp_instance;

public:
  void loadSavedMessagesAuthCode();
  void generateSaveMessagesAuthCode();

  void addMessageToSend( VNumber to_user_id, VNumber chat_id, const Message& );
  QList<MessageRecord> takeMessagesToSendToUserId( VNumber user_id, bool also_sent_messages );
  int countMessagesToSendToUserId( VNumber );
  int countMessagesToSendInChatId( VNumber );

  void addSentMessage( VNumber to_user_id, VNumber chat_id, const Message& );
  bool setMessageReceived( VNumber msg_id );

  inline VNumber nextUserWithUnsentMessages() const;

  void addMessageRecord( const MessageRecord& );
  void addMessageRecords( const QList<MessageRecord>& );

  bool chatMessageCanBeSaved() const;
  bool unsentMessagesCanBeSaved() const;
  bool saveMessages( bool save_unsent_messages_also );

  inline const QString& savedMessagesAuthCode() const;

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
      mp_instance = Q_NULLPTR;
    }
  }

protected:
  MessageManager();
  bool saveUnsentMessages( bool silent_mode );

private:
  QList<MessageRecord> m_messagesToSend;
  QList<MessageRecord> m_sentMessages;
  QString m_savedMessagesAuthCode;

};


// Inline Functions
VNumber MessageManager::nextUserWithUnsentMessages() const { return m_messagesToSend.isEmpty() ? ID_INVALID : m_messagesToSend.first().toUserId(); }
const QString& MessageManager::savedMessagesAuthCode() const { return m_savedMessagesAuthCode; }

#endif // BEEBEEP_MESSAGEMANAGER_H
