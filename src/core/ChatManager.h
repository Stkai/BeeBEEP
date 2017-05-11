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

#ifndef BEEBEEP_CHATMANAGER_H
#define BEEBEEP_CHATMANAGER_H

#include "Chat.h"
#include "ChatRecord.h"


class ChatManager
{
// Singleton Object
  static ChatManager* mp_instance;
  friend class Core;

public:
  inline Chat defaultChat() const;
  Chat chat( VNumber ) const;
  Chat privateChatForUser( VNumber user_id ) const;
  Chat findChatByName( const QString& ) const;
  Chat findChatByPrivateId( const QString& chat_private_id, bool skip_default_chat, VNumber user_id ) const;
  Chat firstChatWithUnreadMessages() const;
  Chat findGroupChatByUsers( const QList<VNumber>& ) const;

  void setChat( const Chat& );
  inline const QList<Chat>& constChatList() const;
  QStringList chatNamesToStringList( bool add_default_chat ) const;
  inline bool removeChat( const Chat& );

  bool isChatEmpty( const Chat&, bool check_also_history ) const;
  int countNotEmptyChats( bool check_also_history ) const;

  inline bool hasName( const QString& ) const;
  int unreadMessages() const;
  bool isGroupChat( VNumber ) const;
  bool hasUnreadMessages() const;
  QList<Chat> chatsWithUser( VNumber ) const;

  QList<Chat> groupChatsWithUser( VNumber ) const;
  bool userIsInGroupChat( VNumber ) const;

  void addSavedChats( const QMap<QString, QString>& );
  inline QString chatSavedText( const QString& ) const;
  inline bool chatHasSavedText( const QString& ) const;
  inline void removeSavedTextFromChat( const QString& );
  inline bool isLoadHistoryCompleted() const;
  void updateChatSavedText( const QString& old_chat_name, const QString& new_chat_name, bool add_to_new );
  inline const QMap<QString, QString>& constHistoryMap() const;
  int savedChatSize( const QString& ) const;

  void changePrivateChatNameAfterUserNameChanged( VNumber user_id, const QString& new_chat_name );

  void addToRefusedChat( const ChatRecord& );
  bool isChatRefused( const QString& chat_private_id ) const;
  inline const QList<ChatRecord>& refusedChats() const;


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
  QMap<QString, QString> m_history;
  bool m_isLoadHistoryCompleted;
  QList<ChatRecord> m_refusedChats;

};


// Inline Function
inline Chat ChatManager::defaultChat() const { return chat( ID_DEFAULT_CHAT ); }
inline const QList<Chat>& ChatManager::constChatList() const { return m_chats; }
inline QList<Chat>& ChatManager::chatList() { return m_chats; }
inline bool ChatManager::hasName( const QString& chat_name ) const { return findChatByName( chat_name ).isValid(); }
inline QString ChatManager::chatSavedText( const QString& chat_name ) const { return m_history.value( chat_name ); }
inline bool ChatManager::chatHasSavedText( const QString& chat_name ) const { return m_history.contains( chat_name ); }
inline void ChatManager::removeSavedTextFromChat( const QString& chat_name ) { m_history.remove( chat_name ); }
inline bool ChatManager::isLoadHistoryCompleted() const { return m_isLoadHistoryCompleted; }
inline const QMap<QString, QString>& ChatManager::constHistoryMap() const { return m_history; }
inline bool ChatManager::removeChat( const Chat& c ) { return m_chats.removeOne( c ); }
inline const QList<ChatRecord>& ChatManager::refusedChats() const { return m_refusedChats; }

#endif // BEEBEEP_CHATMANAGER_H
