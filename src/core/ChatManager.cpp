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
#include "GuiChatMessage.h"
#include "User.h"
#include "Settings.h"

ChatManager* ChatManager::mp_instance = Q_NULLPTR;


ChatManager::ChatManager()
  : m_chats(), m_history(), m_isLoadHistoryCompleted( false ), m_refusedChats()
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
    qWarning() << "Unable to find chat" << chat_id;
  }
  return Chat();
}

Chat ChatManager::privateChatForUser( VNumber user_id ) const
{
  if( user_id == ID_INVALID || user_id == ID_LOCAL_USER )
    return Chat();

  foreach( Chat c, m_chats )
  {
    if( c.isPrivateForUser( user_id ) )
      return c;
  }

  qWarning() << "Unable to find private chat for user" << user_id;
  return Chat();
}

Chat ChatManager::findChatByName( const QString& chat_name ) const
{
  if( !chat_name.isEmpty() )
  {
    foreach( Chat c, m_chats )
    {
      if( c.name().toLower() == chat_name.toLower() )
        return c;
    }
#ifdef BEEBEEP_DEBUG
    qWarning() << "Unable to find group chat" << chat_name;
#endif
  }
#ifdef BEEBEEP_DEBUG
  else
    qWarning() << "Unable to find group chat with empty name";
#endif
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

QList<Chat> ChatManager::chatsWithUser( VNumber user_id ) const
{
  if( user_id == ID_LOCAL_USER )
    return m_chats;
  QList<Chat> chat_list;
  foreach( Chat c, m_chats )
  {
    if( c.hasUser( user_id ) )
      chat_list.append( c );
  }
  return chat_list;
}

bool ChatManager::isGroupChat( VNumber chat_id ) const
{
  if( chat_id == ID_DEFAULT_CHAT )
    return false;
  Chat c = chat( chat_id );
  return c.isGroup();
}

QList<Chat> ChatManager::groupChatsWithUser( VNumber user_id ) const
{
  QList<Chat> chat_list;
  foreach( Chat c, m_chats )
  {
    if( c.isGroup() && c.hasUser( user_id ) )
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

void ChatManager::updateChatSavedText( const QString& old_chat_name, const QString& new_chat_name, bool add_to_new )
{
#ifdef BEEBEEP_DEBUG
  qDebug() << "Copy the chat history with name" << old_chat_name << "to" << new_chat_name;
#endif
  QString chat_text_old = m_history.take( old_chat_name );
  if( add_to_new && chatHasSavedText( new_chat_name ) )
  {
    chat_text_old.append( "<br>" );
    chat_text_old.append( chatSavedText( new_chat_name ) );
  }

  m_history.insert( new_chat_name, chat_text_old );
}

void ChatManager::changePrivateChatNameAfterUserNameChanged( VNumber user_id, const QString& new_chat_name )
{
  Chat c = privateChatForUser( user_id );
  if( !c.isValid() )
    return;

  QString old_chat_name = c.name();
  c.setName( new_chat_name );
  setChat( c );
#ifdef BEEBEEP_DEBUG
  qDebug() << "The chat with name" << old_chat_name << "is changed to" << c.name();
#endif

  if( !chatHasSavedText( c.name() ) && chatHasSavedText( old_chat_name ) )
    updateChatSavedText( old_chat_name, c.name(), false );
}

void ChatManager::addSavedChats( const QMap<QString, QString>& saved_chats )
{
  m_history = saved_chats;
  m_isLoadHistoryCompleted = true;
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

QString ChatManager::chatName( VNumber chat_id ) const
{
  foreach( Chat c, m_chats )
  {
    if( c.id() == chat_id )
      return c.name();
  }
  return QString( "" );
}

Chat ChatManager::findGroupChatByUsers( const QList<VNumber>& user_list ) const
{
  foreach( Chat c, m_chats )
  {
    if( c.isGroup() && c.usersId().size() == user_list.size() && c.hasUsers( user_list ) )
      return c;
  }
  return Chat();
}

int ChatManager::savedChatSize( const QString& chat_name ) const
{
  if( chatHasSavedText( chat_name ) )
  {
    return chatSavedText( chat_name ).split( "<br>", QString::SkipEmptyParts ).size();
  }
  else
    return 0;
}

bool ChatManager::isChatEmpty( const Chat& c, bool check_also_history ) const
{
  if( c.isEmpty() )
  {
    if( check_also_history)
      return !chatHasSavedText( c.name() );
    else
      return true;
  }
  else
    return false;
}

int ChatManager::countNotEmptyChats( bool check_also_history ) const
{
  int num_chats = 0;
  foreach( Chat c, m_chats )
  {
    if( !isChatEmpty( c, check_also_history ) )
      num_chats++;
  }
  return num_chats;
}

int ChatManager::countGroupChats() const
{
  int num_chats = 0;
  foreach( Chat c, m_chats )
  {
    if( c.isGroup() )
      num_chats++;
  }
  return num_chats;
}

void ChatManager::addToRefusedChat( const ChatRecord& cr )
{
  if( !m_refusedChats.contains( cr ) )
    m_refusedChats.append( cr );
}

bool ChatManager::isChatRefused( const QString& chat_private_id ) const
{
  ChatRecord cr( "", chat_private_id );
  if( cr.isValid() )
    return m_refusedChats.contains( cr );
  else
    return false;
}

bool ChatManager::setChatToSavedChats( const Chat& c )
{
  QString saved_chat_text = GuiChatMessage::chatToHtml( c, !Settings::instance().chatSaveFileTransfers(),
                                                        !Settings::instance().chatSaveSystemMessages(),
                                                        true, true, true, Settings::instance().useCompactDataSaving() );
  if( saved_chat_text.isEmpty() )
    return false;
  if( chatHasSavedText( c.name() ) )
    saved_chat_text.prepend( chatSavedText( c.name() ) );
  m_history.insert( c.name(), saved_chat_text );
  return true;
}

QString ChatManager::chatSavedText( const QString& chat_name, int max_lines, int *missed_lines ) const
{
  if( missed_lines )
    *missed_lines = 0;
  QString saved_text = m_history.value( chat_name );
  if( max_lines < 0 )
   return saved_text;

  QStringList sl = saved_text.split( "<br>" );
  if( sl.size() <= max_lines )
    return saved_text;

  saved_text = "";
  QStringListIterator it( sl );
  it.toBack();
  int line_counter = 0;
  while( it.hasPrevious() )
  {
    line_counter++;
    if( line_counter >= max_lines )
      break;
    saved_text.prepend( QString( "%1%2").arg( it.previous() ).arg( "<br>" ) );
  }
  if( missed_lines )
    *missed_lines = qMax( 0, sl.size() - max_lines);
  return saved_text;
}

