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

#include "ChatManager.h"
#include "User.h"
#include "Settings.h"

ChatManager* ChatManager::mp_instance = NULL;


ChatManager::ChatManager()
  : m_chats(), m_history(), m_isLoadHistoryCompleted( false )
{
}

Chat ChatManager::chat( VNumber chat_id ) const
{
  if( chat_id != ID_INVALID )
  {
    foreach( Chat c, m_chats )
    {
      if( c.id() == chat_id )
        return c;
    }
    qWarning() << "Unable to find chat with id" << chat_id;
  }
  return Chat();
}

Chat ChatManager::privateChatForUser( VNumber user_id ) const
{
  if( user_id == ID_LOCAL_USER )
    return chat( ID_DEFAULT_CHAT );

  foreach( Chat c, m_chats )
  {
    if( c.isPrivateForUser( user_id ) )
      return c;
  }

  qWarning() << "Unable to find private chat for user id" << user_id;
  return Chat();
}

Chat ChatManager::findChatByPrivateId( const QString& chat_private_id, bool skip_default_chat, VNumber user_id ) const
{
  if( !chat_private_id.isEmpty() )
  {
    foreach( Chat c, m_chats )
    {
      if( skip_default_chat && c.isDefault() )
        continue;

      if( c.privateId() == chat_private_id )
        return c;
    }
#ifdef BEEBEEP_DEBUG
    qWarning() << "Unable to find group chat with private id" << chat_private_id;
#endif
    return Chat();
  }
  else
    return user_id > ID_LOCAL_USER ? privateChatForUser( user_id ) : Chat();
}

void ChatManager::setChat( const Chat& c )
{
  QList<Chat>::iterator it = m_chats.begin();
  while( it != m_chats.end() )
  {
    if( (*it).id() == c.id() )
    {
      (*it) = c;
      return;
    }
    ++it;
  }
  m_chats.append( c );
}

int ChatManager::unreadMessages() const
{
  int unread_messages = 0;
  foreach( Chat c, m_chats )
    unread_messages += c.unreadMessages();
  return unread_messages;
}

bool ChatManager::hasUnreadMessages() const
{
  foreach( Chat c, m_chats )
  {
    if( c.unreadMessages() > 0 )
      return true;
  }
  return false;
}

Chat ChatManager::firstChatWithUnreadMessages() const
{
  bool default_chat_has_unread_messages = false;
  foreach( Chat c, m_chats )
  {
    if( c.unreadMessages() > 0 )
    {
      if( c.isDefault() )
        default_chat_has_unread_messages = true;
      else
        return c;
    }
  }

  if( default_chat_has_unread_messages )
    return chat( ID_DEFAULT_CHAT );
  else
    return Chat();
}

bool ChatManager::hasName( const QString& chat_name ) const
{
  if( Settings::instance().defaultChatName() == chat_name )
    return true;

  foreach( Chat c, m_chats )
  {
    if( c.name().toLower() == chat_name.toLower() )
      return true;
  }
  return false;
}

bool ChatManager::isGroupChat( VNumber chat_id ) const
{
  if( chat_id == ID_DEFAULT_CHAT )
    return false;
  Chat c = chat( chat_id );
  return c.isGroup();
}

QList<Chat> ChatManager::groupChatsForUser( VNumber user_id ) const
{
  QList<Chat> chat_list;
  foreach( Chat c, m_chats )
  {
    if( c.isGroup() && c.usersId().contains( user_id ) )
      chat_list.append( c );
  }
  return chat_list;
}

bool ChatManager::userIsInGroupChat( VNumber user_id ) const
{
  foreach( Chat c, m_chats )
  {
    if( c.isGroup() && c.usersId().contains( user_id ) )
      return true;
  }
  return false;
}

QString ChatManager::findPrivateChatSavedTextWithSameNickname( const QString& chat_name ) const
{
  QString chat_user_nick = User::nameFromPath( chat_name );

  foreach( QString key_name, m_history )
  {
    if( chat_user_nick == User::nameFromPath( key_name ) )
      return key_name;
  }

  return QString();
}

void ChatManager::updateChatSavedText( const QString& old_chat_name, const QString& new_chat_name, bool add_to_new )
{
#ifdef BEEBEEP_DEBUG
  qDebug() << "Copy the chat history with name" << old_chat_name << "to" << new_chat_name;
#endif
  QString chat_text_old = m_history.take( old_chat_name );
  if( add_to_new && chatHasSavedText( new_chat_name ) )
  {
    chat_text_old.append( "<br />" );
    chat_text_old.append( chatSavedText( new_chat_name ) );
  }

  m_history.insert( new_chat_name, chat_text_old );
}

void ChatManager::changePrivateChatNameAfterUserNameChanged( VNumber user_id, const QString& user_new_path )
{
  Chat c = privateChatForUser( user_id );
  if( !c.isValid() )
    return;

  QString old_chat_name = c.name();
  c.setName( user_new_path );
  setChat( c );
#ifdef BEEBEEP_DEBUG
  qDebug() << "The chat with name" << old_chat_name << "is changed to" << c.name();
#endif

  if( !chatHasSavedText( c.name() ) && chatHasSavedText( old_chat_name ) )
    updateChatSavedText( old_chat_name, c.name(), false );
}

void ChatManager::autoLinkSavedChatByNickname( const Chat& c )
{
  if( !chatHasSavedText( c.name() ) )
  {
    QString chat_same_nickname = ChatManager::instance().findPrivateChatSavedTextWithSameNickname( c.name() );
    if( chat_same_nickname.isEmpty() )
      return;
    ChatManager::instance().updateChatSavedText( chat_same_nickname, c.name(), false );
  }
}

void ChatManager::addSavedChats( const QMap<QString, QString>& saved_chats )
{
  m_history = saved_chats;
  m_isLoadHistoryCompleted = true;
  if( Settings::instance().autoLinkSavedChatByNickname() )
  {
    foreach( Chat c, m_chats )
      autoLinkSavedChatByNickname( c );
  }
}

QStringList ChatManager::chatNamesToStringList( bool add_default_chat ) const
{
  QStringList sl;

  foreach( Chat c, m_chats )
  {
    if( c.isDefault() && !add_default_chat )
      continue;
    sl << c.name();
  }
  return sl;
}

Chat ChatManager::groupChatForUsers( const QList<VNumber>& user_list ) const
{
  foreach( Chat c, m_chats )
  {
    if( c.isGroup() && c.hasUsers( user_list ) )
      return c;
  }
  return Chat();
}

int ChatManager::savedChatSize( const QString& chat_name ) const
{
  if( chatHasSavedText( chat_name ) )
  {
    return chatSavedText( chat_name ).split( "<br />", QString::SkipEmptyParts ).size();
  }
  else
    return 0;
}

int ChatManager::countNotEmptyChats() const
{
  int num_chats = 0;
  foreach( Chat c, m_chats )
  {
    if( !c.isEmpty() )
      num_chats++;
  }
  return num_chats;
}
