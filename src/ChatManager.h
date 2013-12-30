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

#ifndef BEEBEEP_CHATMANAGER_H
#define BEEBEEP_CHATMANAGER_H

#include "Chat.h"


class ChatManager
{
// Singleton Object
  static ChatManager* mp_instance;
  friend class Core;

public:
  inline Chat defaultChat( bool read_all_messages  );
  Chat chat( VNumber ) const;
  Chat chat( VNumber chat_id, bool read_all_messages );
  Chat privateChatForUser( VNumber user_id ) const;
  Chat groupChat( const QString& ) const;

  void setChat( const Chat& );
  inline const QList<Chat>& chatList() const;

  bool hasName( const QString& ) const;
  int unreadMessages() const;

  static ChatManager& instance()
  {
    if( !mp_instance )
      mp_instance = new ChatManager();
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
  ChatManager();

  inline QList<Chat>& chatList();

private:
  QList<Chat> m_chats;

};


// Inline Function
inline Chat ChatManager::defaultChat( bool read_all_messages ) { return chat( ID_DEFAULT_CHAT, read_all_messages ); }
inline const QList<Chat>& ChatManager::chatList() const { return m_chats; }
inline QList<Chat>& ChatManager::chatList() { return m_chats; }


#endif // BEEBEEP_CHATMANAGER_H
