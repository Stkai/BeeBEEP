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

#include "BeeApplication.h"
#include "BeeUtils.h"
#include "BuildSavedChatList.h"
#include "ChatManager.h"
#include "Connection.h"
#include "Core.h"
#include "IconManager.h"
#include "MessageManager.h"
#include "Protocol.h"
#include "PluginManager.h"
#include "Random.h"
#include "Settings.h"
#include "UserManager.h"



void Core::createDefaultChat()
{
  qDebug() << "Creating default chat";
  Chat c = Protocol::instance().createDefaultChat();
  qDebug() << "Default chat private id:" << qPrintable( c.privateId() );
  addChatHeader( &c );
  QString sHtmlMsg;

  if( QDate::currentDate().month() == 4 && QDate::currentDate().day() == 6 )
  {
    int my_age = QDate::currentDate().year() - 1975;
    sHtmlMsg = QString( "%1 <b>%2</b>" ).arg( IconManager::instance().toHtml( "birthday.png", "*!*" ),
                                              tr( "Happy birthday to Marco Mastroddi: %1 years old today! Cheers!!!" ).arg( my_age ) );
    c.addMessage( ChatMessage( ID_SYSTEM_MESSAGE, Protocol::instance().systemMessage( sHtmlMsg ), ChatMessage::Other ) );
  }

  if( QDate::currentDate().month() == 1 && QDate::currentDate().day() >= 1 && QDate::currentDate().day() <= 3 )
  {
    QString new_year_icons;
    if( Settings::instance().useNativeEmoticons() )
      new_year_icons = QString( "%1&nbsp;&nbsp;%2&nbsp;&nbsp;%3" ).arg( QString::fromUtf8( "🎆" ), QString::fromUtf8( "🎆" ), QString::fromUtf8( "🎆" ) );
    else
      new_year_icons = QString( "%1&nbsp;&nbsp;%2&nbsp;&nbsp;%3" ).arg( IconManager::instance().toHtml( ":/emojis/objects/1f386.png", "*!*" ), IconManager::instance().toHtml( ":/emojis/objects/1f386.png", "*!*" ), IconManager::instance().toHtml( ":/emojis/objects/1f386.png", "*!*" ) );
    sHtmlMsg = QString( "%1&nbsp;&nbsp;&nbsp;<font color=red><b>%2</b></font>&nbsp;&nbsp;&nbsp;%3" ).arg( new_year_icons, tr( "Happy New Year!" ), new_year_icons );
    c.addMessage( ChatMessage( ID_SYSTEM_MESSAGE, Protocol::instance().systemMessage( sHtmlMsg ), ChatMessage::Other ) );
  }

  if( Settings::instance().localUser().isBirthDay() )
  {
    sHtmlMsg = QString( "%1 <b>%2</b>" ).arg( IconManager::instance().toHtml( "birthday.png", "*!*" ), tr( "Happy Birthday to you!" ) );
    c.addMessage( ChatMessage( ID_SYSTEM_MESSAGE, Protocol::instance().systemMessage( sHtmlMsg ), ChatMessage::Other ) );
  }

  ChatManager::instance().setChat( c );
  emit chatChanged( c );
}

void Core::createPrivateChat( const User& u )
{
  qDebug() << "Creating private chat room for user" << qPrintable( u.name() );
  Chat c = Protocol::instance().createPrivateChat( u );
  addChatHeader( &c );
  ChatManager::instance().setChat( c );
  emit chatChanged( c );
}

int Core::checkGroupChatAfterUserReconnect( const User& u )
{
  QList<Chat> chat_list = ChatManager::instance().groupChatsWithUser( u.id() );
  if( chat_list.isEmpty() )
    return 0;

  foreach( Chat c, chat_list )
  {
    UserList ul = UserManager::instance().userList().fromUsersId( c.usersId() );
    sendGroupChatRequestMessage( c, ul, u );
  }

  return chat_list.size();
}

Chat Core::createGroupChat( const User& u, const Group& g, bool broadcast_message )
{
  QString sHtmlMsg;

  Chat c = Protocol::instance().createChat( g, Group::GroupChat );
  qDebug() << "Creating group chat named" << qPrintable( c.name() ) << "with private id" << qPrintable( c.privateId() ) << "by user" << qPrintable( u.name() );
  addChatHeader( &c );

  if( g.privateId().isEmpty() ) // to prevent creation message on load from settings
  {
    sHtmlMsg = tr( "%1 You have created group chat %2." ).arg( IconManager::instance().toHtml( "group-create.png", "*G*" ), QString( "<b>%1</b>" ).arg( Bee::removeHtmlTags( c.name() ) ) );
    c.addMessage( ChatMessage( ID_SYSTEM_MESSAGE, Protocol::instance().systemMessage( sHtmlMsg ), ChatMessage::System ) );
    c.setLastModifiedToNow();
  }

  if( !u.isLocal() )
  {
    sHtmlMsg = tr( "%1 %2 has added you to the group chat %3." ).arg( IconManager::instance().toHtml( "group-add.png", "*G*" ), Bee::replaceHtmlSpecialCharacters( u.name() ), QString( "<b>%1</b>" ).arg( Bee::replaceHtmlSpecialCharacters( c.name() ) ) );
    c.addMessage( ChatMessage( ID_SYSTEM_MESSAGE, Protocol::instance().systemMessage( sHtmlMsg ), ChatMessage::System ) );
  }

  if( !sHtmlMsg.isEmpty() )
    dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), sHtmlMsg, DispatchToChat, ChatMessage::System );

  ChatManager::instance().setChat( c );
  emit chatChanged( c );

  if( broadcast_message && isConnected() )
  {
    UserList ul = UserManager::instance().userList().fromUsersId( c.usersId() );
    sendGroupChatRequestMessage( c, ul, User() );
  }

  return c;
}

bool Core::changeGroupChat( const User& u, const Group& g )
{
  Chat c = ChatManager::instance().chat( g.id() );
  if( !c.isValid() )
  {
    qWarning() << "Unable to change chat" << qPrintable( g.name() ) << "with id" << g.id() << "because it is not in the list";
    return false;
  }

  UserList group_new_members = UserManager::instance().userList().fromUsersId( g.usersId() );
  UserList group_old_members = UserManager::instance().userList().fromUsersId( c.usersId() );
  UserList group_removed_members;
  QStringList user_added_string_list;
  QStringList user_removed_string_list;
  QStringList user_string_list;
  QString sHtmlMsg;
  bool chat_changed = false;

  if( c.name() != g.name() )
  {
    if( u.isLocal() )
      sHtmlMsg = tr( "%1 You have changed the group name from %2 to %3." ).arg( IconManager::instance().toHtml( "group.png", "*G*" ), c.name(), QString( "<b>%1</b>" ).arg( Bee::replaceHtmlSpecialCharacters( g.name() ) ) );
    else
      sHtmlMsg = tr( "%1 %2 has changed the group name from %3 to %4." ).arg( IconManager::instance().toHtml( "group.png", "*G*" ), Bee::replaceHtmlSpecialCharacters( u.name() ), Bee::replaceHtmlSpecialCharacters( c.name() ), QString( "<b>%1</b>" ).arg( Bee::replaceHtmlSpecialCharacters( g.name() ) ) );
    c.addMessage( ChatMessage( ID_SYSTEM_MESSAGE, Protocol::instance().systemMessage( sHtmlMsg ), ChatMessage::System ) );
    chat_changed = true;
    if( ChatManager::instance().chatHasSavedText( c.name() ) )
      ChatManager::instance().updateChatSavedText( c.name(), g.name(), false );
    c.setName( g.name() );
  }

  foreach( User old_user, group_old_members.toList() )
  {
    if( !old_user.isLocal() )
    {
      if( !group_new_members.has( old_user.id() ) )
      {
        if( u.isLocal() )
        {
          sHtmlMsg = tr( "%1 %2 will be informed of your changes." ).arg( IconManager::instance().toHtml( "group-remove.png", "*G*" ) ).arg( Bee::replaceHtmlSpecialCharacters( old_user.name() ) );
          c.addMessage( ChatMessage( ID_SYSTEM_MESSAGE, Protocol::instance().systemMessage( sHtmlMsg ), ChatMessage::System ) );
          group_removed_members.set( old_user );
        }

        if( c.removeUser( old_user.id() ) )
          user_removed_string_list << Bee::replaceHtmlSpecialCharacters( old_user.name() );
      }
    }
  }

  if( user_removed_string_list.size() > 0 )
  {
    if( u.isLocal() )
      sHtmlMsg = tr( "%1 You have removed members: %2." ).arg( IconManager::instance().toHtml( "group-remove.png", "*G*" ), Bee::stringListToTextString( user_removed_string_list ) );
    else
      sHtmlMsg = tr( "%1 %2 has removed members: %3." ).arg( IconManager::instance().toHtml( "group-remove.png", "*G*" ), Bee::replaceHtmlSpecialCharacters( u.name() ), Bee::stringListToTextString( user_removed_string_list ) );
    c.addMessage( ChatMessage( ID_SYSTEM_MESSAGE, Protocol::instance().systemMessage( sHtmlMsg ), ChatMessage::System ) );

    sHtmlMsg = tr( "%1 This kind of change can be temporary if the user exists and does not leave the group spontaneously." ).arg( IconManager::instance().toHtml( "group-remove.png", "*G*" ) );
    c.addMessage( ChatMessage( ID_SYSTEM_MESSAGE, Protocol::instance().systemMessage( sHtmlMsg ), ChatMessage::System ) );
  }

  foreach( User u, group_new_members.toList() )
  {
    if( !u.isLocal() )
    {
      if( c.addUser( u.id() ) )
        user_added_string_list << Bee::replaceHtmlSpecialCharacters( u.name() );
      user_string_list.append( Bee::replaceHtmlSpecialCharacters( u.name() ) );
    }
  }

  if( user_added_string_list.size() > 0 )
  {
    if( u.isLocal() )
      sHtmlMsg = tr( "%1 You have added members: %2." ).arg( IconManager::instance().toHtml( "group-add.png", "*G*" ), Bee::stringListToTextString( user_added_string_list ) );
    else
      sHtmlMsg = tr( "%1 %2 has added members: %3." ).arg( IconManager::instance().toHtml( "group-add.png", "*G*" ), Bee::replaceHtmlSpecialCharacters( u.name() ), Bee::stringListToTextString( user_added_string_list ) );
    c.addMessage( ChatMessage( ID_SYSTEM_MESSAGE, Protocol::instance().systemMessage( sHtmlMsg ), ChatMessage::System ) );
  }

  if( user_removed_string_list.size() > 0 || user_added_string_list.size() > 0 )
  {
    chat_changed = true;
    sHtmlMsg = tr( "%1 Chat with %2." ).arg( IconManager::instance().toHtml( "group.png", "*G*" ), Bee::stringListToTextString( user_string_list ) );
    c.addMessage( ChatMessage( ID_SYSTEM_MESSAGE, Protocol::instance().systemMessage( sHtmlMsg ), ChatMessage::Header ) );
  }

  if( chat_changed )
  {
    if( !u.isLocal() )
    {
      if( g.lastModified().isValid() )
        c.setLastModified( g.lastModified() );
    }
    else
      c.setLastModifiedToNow();
    ChatManager::instance().setChat( c );
    qDebug() << "Group chat" << qPrintable( c.name() ) << "changed by" << qPrintable( u.name() ) << "with timestamp" << qPrintable( c.lastModified().toString( Qt::ISODate ) ) ;
    emit chatChanged( c );

    if( u.isLocal() && isConnected() )
    {
      sendGroupChatRequestMessage( c, group_new_members, User() );
      Message group_remove_user_message = Protocol::instance().groupChatRemoveUserMessage( c );
      foreach( User removed_member, group_removed_members.toList() )
        sendMessageToLocalNetwork( removed_member, group_remove_user_message );
    }
  }
  else
  {
    if( g.lastModified().isValid() )
    {
      if( c.lastModified().isNull() || g.lastModified() > c.lastModified() )
      {
        c.setLastModified( g.lastModified() );
        ChatManager::instance().setChat( c );
      }
    }
  }

#ifdef BEEBEEP_DEBUG
  if( !chat_changed )
    qDebug() << qPrintable( u.name() ) << "has request to update chat" << qPrintable( c.name() ) << "but there is nothing to change";
#endif
  return chat_changed;
}

bool Core::removeUserFromGroupChat( const User& u, const QString& chat_private_id )
{
  Chat c = ChatManager::instance().findChatByPrivateId( chat_private_id, true, ID_INVALID );
  if( !c.isValid() )
    return true;
  if( !c.isGroup() )
    return false;

  if( u.isLocal() )
  {
    qDebug() << "Removing local user from group chat" << qPrintable( c.name() );
    return removeChat( c.id(), true );
  }
  else
  {
    QString sHtmlMsg = tr( "%1 %2 has left the group chat %3." ).arg( IconManager::instance().toHtml( "group-remove.png", "*G*" ), Bee::replaceHtmlSpecialCharacters( u.name() ), QString( "<b>%1</b>" ).arg( Bee::replaceHtmlSpecialCharacters( c.name() ) ) );
    c.removeUser( u.id() );
    c.setLastModifiedToNow();
    c.addMessage( ChatMessage( ID_SYSTEM_MESSAGE, Protocol::instance().systemMessage( sHtmlMsg ), ChatMessage::System ) );
    ChatManager::instance().setChat( c );
    dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), sHtmlMsg, DispatchToChat, ChatMessage::System );
    emit chatChanged( c );
    return true;
  }
}

bool Core::removeLocalUserFromGroupChatByOther( const User& other_user, const QString& chat_private_id )
{
  if( other_user.isLocal() )
    return false;

  Chat c = ChatManager::instance().findChatByPrivateId( chat_private_id, true, ID_INVALID );
  if( !c.isValid() )
    return false;
  if( !c.isGroup() )
    return false;
  QString sHtmlMsg = tr( "%1 %2 wants to remove you from the group chat %3. If you agree please leave the group." ).arg( IconManager::instance().toHtml( "group-remove.png", "*G*" ), Bee::replaceHtmlSpecialCharacters( other_user.name() ), QString( "<b>%1</b>" ).arg( Bee::replaceHtmlSpecialCharacters( c.name() ) ) );
  c.addMessage( ChatMessage( ID_SYSTEM_MESSAGE, Protocol::instance().systemMessage( sHtmlMsg ), ChatMessage::System ) );
  ChatManager::instance().setChat( c );
  dispatchSystemMessage( ID_DEFAULT_CHAT, other_user.id(), sHtmlMsg, DispatchToChat, ChatMessage::System );
  emit chatChanged( c );
  return true;
}

bool Core::removeChat( VNumber chat_id, bool save_chat_messages )
{
  Chat c = ChatManager::instance().chat( chat_id );
  if( !c.isValid() )
    return true;

  if( c.isDefault() )
    return false;

  if( save_chat_messages && ChatManager::instance().setChatToSavedChats( c ) )
  {
    dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER, tr( "%1 %2 is added to saved chats." )
                                                      .arg( IconManager::instance().toHtml( "saved-chat.png", "*H*" ), QString( "<b>%1</b>" ).arg( Bee::replaceHtmlSpecialCharacters( c.name() ) ) ),
                           DispatchToChat, ChatMessage::System );
  }

  clearMessagesInChat( c.id(), !save_chat_messages );

  if( c.isPrivate() )
  {
    if( !removeOfflineUser( c.privateUserId() ) )
      return false;
  }

  ChatManager::instance().removeChat( c );
  emit chatRemoved( c );
  if( c.isGroup() )
  {
    dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER, tr( "%1 You have left group chat %2." )
                           .arg( IconManager::instance().toHtml( "group-remove.png", "*G*" ) ).arg( QString( "<b>%1</b>" ).arg( Bee::replaceHtmlSpecialCharacters( c.name() ) ) ),
                           DispatchToChat, ChatMessage::System );
    qDebug() << "You have left group chat" << qPrintable( c.name() );
    Settings::instance().setNotificationEnabledForGroup( c.privateId(), false ); // reset notification in settings
    sendRefuseMessageToGroupChat( c );
    ChatManager::instance().addToRefusedChat( ChatRecord( c.name(), c.privateId() ) );
  }
  else
    qDebug() << "You have left private chat with" << qPrintable( c.name() );

  return true;
}

int Core::archiveAllChats()
{
  QList<Chat> chat_list = ChatManager::instance().constChatList();
  if( chat_list.isEmpty() )
    return 0;

  int chat_count = 0;

  foreach( Chat c, chat_list )
  {
    if( c.isDefault() )
      continue;
    else
      chat_count++;

    if( ChatManager::instance().setChatToSavedChats( c ) )
    {
      dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER, tr( "%1 Chat with %2 is archived." )
                                                               .arg( IconManager::instance().toHtml( "saved-chat.png", "*H*" ), QString( "<b>%1</b>" ).arg( Bee::replaceHtmlSpecialCharacters( c.name() ) ) ),
                             DispatchToChat, ChatMessage::System );
      qDebug() << "Chat archived:" << c.name();
    }

    if( c.isGroup() )
    {
      ChatManager::instance().removeChat( c );
      emit chatRemoved( c );
    }
    else
    {
      if( !c.isEmpty() )
        clearMessagesInChat( c.id(), false );
    }
  }
  return chat_count;
}

int Core::sendChatMessage( VNumber chat_id, const QString& msg, bool is_important )
{
  if( !isConnected() )
  {
    dispatchSystemMessage( chat_id, ID_LOCAL_USER, tr( "Unable to send the message: you are not connected." ), DispatchToChat, ChatMessage::Other );
    return 0;
  }

  if( chat_id == ID_DEFAULT_CHAT && !Settings::instance().chatWithAllUsersIsEnabled() )
  {
    dispatchSystemMessage( chat_id, ID_LOCAL_USER, tr( "Unable to send the message: this chat is disabled." ), DispatchToChat, ChatMessage::Other );
    return 0;
  }

  if( msg.isEmpty() )
    return 0;

  Chat c = ChatManager::instance().chat( chat_id );
  if( !c.isValid() )
  {
    qWarning() << "Invalid chat Id in Core::sendChatMessage";
    return 0;
  }

  QString msg_to_send = msg;
  if( !Settings::instance().chatUseHtmlTags() )
  {
    msg_to_send.replace( QLatin1Char( '<' ), QLatin1String( "&lt;" ) );
    msg_to_send.replace( "&lt;3", "<3" ); // hearth emoticon
  }

  PluginManager::instance().parseText( &msg_to_send, true );

  Message m = Protocol::instance().chatMessage( c, msg_to_send );
  if( is_important )
    m.setImportant();

  int messages_sent = 0;

  ChatMessage cm( ID_LOCAL_USER, m, ChatMessage::Chat );
  dispatchToChat( cm, chat_id );

  if( chat_id == ID_DEFAULT_CHAT && !Settings::instance().sendOfflineMessagesToDefaultChat() )
  {
    foreach( Connection *user_connection, m_connections )
    {
      if( user_connection->sendMessage( m ) )
        messages_sent += 1;
      else
        dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER, tr( "Unable to send the message to %1." )
                               .arg( UserManager::instance().findUser( user_connection->userId() ).path() ),
                               DispatchToChat, ChatMessage::Other );
    }

    if( messages_sent == 0 )
      dispatchSystemMessage( chat_id, ID_LOCAL_USER, tr( "Nobody has received the message." ), DispatchToChat, ChatMessage::Other );
  }
  else
    messages_sent = sendMessageToChat( c, m );

  return messages_sent;
}

int Core::sendMessageToChat( const Chat& c, const Message& m )
{
  int messages_sent = 0;
  UserList user_list = c.id() == ID_DEFAULT_CHAT ? UserManager::instance().userList() : UserManager::instance().userList().fromUsersId( c.usersId() );
  QStringList offline_users;
  foreach( User u, user_list.toList() )
  {
    if( u.isLocal() )
      continue;

    if( !sendMessageToLocalNetwork( u, m ) )
    {
      MessageManager::instance().addMessageToSend( u.id(), c.id(), m );
      offline_users.append( Bee::replaceHtmlSpecialCharacters( u.name() ) );
    }
    else
      messages_sent += 1;
  }

  if( !offline_users.isEmpty() )
  {
    QString sys_msg;
    if( m.hasFlag( Message::VoiceMessage ) )
      sys_msg = tr( "The voice message will be delivered to %1." );
    else
      sys_msg = tr( "The message will be delivered to %1." );

    dispatchSystemMessage( c.id(), ID_LOCAL_USER, QString( "%1 %2" )
                           .arg( IconManager::instance().toHtml( "unsent-message.png", "*m*" ) )
                           .arg( sys_msg.arg( Bee::stringListToTextString( offline_users, 3 ) ) ),
                           DispatchToChat, ChatMessage::Other );
  }

  return messages_sent;
}

bool Core::sendChatAutoResponderMessageToUser( const Chat& c, const QString& msg, VNumber user_id )
{
  if( !isConnected() )
    return false;

  User u = UserManager::instance().findUser( user_id );
  if( !u.isValid() )
  {
    qWarning() << "Invalid user" << user_id << "found in Core::sendChatAutoResponderMessageToUser(...)";
    return false;
  }

  if( u.isLocal() )
  {
    qWarning() << "You have tried to send autoresponder message to yourself:" << qPrintable( msg );
    return false;
  }

  QString msg_to_send;

  if( u.protocolVersion() < AUTORESPONDER_MESSAGE_PROTO_VERSION )
    msg_to_send = QString( "[%1] %2" ).arg( Settings::instance().autoresponderName(), msg );
  else
    msg_to_send = msg;

  Message m = Protocol::instance().chatMessage( c, msg_to_send );
  m.addFlag( Message::Auto );
  return sendMessageToLocalNetwork( u, m );
}

void Core::sendWritingMessage( VNumber chat_id )
{
  if( !isConnected() )
    return;

  if( chat_id == ID_DEFAULT_CHAT && !Settings::instance().chatWithAllUsersIsEnabled() )
    return;

  Chat from_chat = ChatManager::instance().chat( chat_id );
  Message m = Protocol::instance().writingMessage( from_chat.privateId() );
  foreach( VNumber user_id,  from_chat.usersId() )
  {
    if( user_id == ID_LOCAL_USER )
      continue;
    Connection* c = connection( user_id );
    if( !c )
      continue;
    c->sendMessage( m );
  }
}

void Core::sendGroupChatRequestMessage( const Chat& group_chat, const UserList& user_list, const User& to_user )
{
  Message group_message;
  foreach( User u, user_list.toList() )
  {
    if( u.isLocal() )
      continue;

    if( to_user.isValid() && u != to_user )
      continue;

    if( u.protocolVersion() < NEW_GROUP_PROTO_VERSION )
      group_message = Protocol::instance().groupChatRequestMessage_obsolete( group_chat, u );
    else
      group_message = Protocol::instance().groupChatRequestMessage( group_chat, u );

#ifdef BEEBEEP_DEBUG
    qDebug() << "Send group chat request message from" << qPrintable( group_chat.name() ) << "to" << qPrintable( u.name() );
#endif

    sendMessageToLocalNetwork( u, group_message );
  }
}

void Core::sendRefuseMessageToGroupChat( const Chat& group_chat )
{
  Message group_refuse_message = Protocol::instance().groupChatRefuseMessage( group_chat );
  UserList user_list = UserManager::instance().userList().fromUsersId( group_chat.usersId() );

  foreach( User u, user_list.toList() )
  {
    if( u.isLocal() )
      continue;

    if( !sendMessageToLocalNetwork( u, group_refuse_message ) )
      dispatchSystemMessage( group_chat.id(), ID_LOCAL_USER, tr( "%1 %2 cannot be informed that you have left the group." )
                             .arg( IconManager::instance().toHtml( "group-remove.png", "*G*" ) ).arg( Bee::replaceHtmlSpecialCharacters( u.name() ) ),
                             DispatchToChat, ChatMessage::System );
  }
}

bool Core::sendMessageToLocalNetwork( const User& to_user, const Message& m )
{
  Connection* c = connection( to_user.id() );
  if( !c )
  {
    if( to_user.isStatusConnected() )
      qWarning() << "Unable to find connection socket for user" << to_user.id() << qPrintable( to_user.name() );
    return false;
  }

  return c->sendMessage( m );
}

void Core::buildSavedChatList()
{
  if( !Settings::instance().enableSaveData() )
  {
    qWarning() << "Skip loading chat messages because you have disabled this option in RC file";
    return;
  }

  BuildSavedChatList *bscl = new BuildSavedChatList;
  connect( bscl, SIGNAL( listCompleted() ), this, SLOT( addListToSavedChats() ) );
  if( beeApp )
    beeApp->addJob( bscl );
  QMetaObject::invokeMethod( bscl, "buildList", Qt::QueuedConnection );
}

void Core::addListToSavedChats()
{
  BuildSavedChatList *bscl = qobject_cast<BuildSavedChatList*>( sender() );
  if( !bscl )
  {
    qWarning() << "Core received a signal from invalid BuildSavedChatList instance";
    return;
  }

  ChatManager::instance().addSavedChats( bscl->savedChats() );
  QString loading_status = QString( "%1 saved chats are added to history (elapsed time: %2)" )
                           .arg( bscl->savedChats().size() )
                           .arg( Bee::timeToString( bscl->elapsedTime() ) );
  qDebug() << qPrintable( loading_status );
  if( bscl->savedChats().size() > 0 )
  {
    // it must be > and not >= to bypass Qt4 and Qt5 problem of different SHA1 code generated (fixed in the SAVE_MESSAGE_AUTH_CODE_PROTO_VERSION+1 version)
    if( bscl->protocolVersion() > SAVE_MESSAGE_AUTH_CODE_PROTO_VERSION && Settings::instance().saveMessagesTimestamp().isValid() && bscl->savedChatsAuthCode() != MessageManager::instance().saveMessagesAuthCode() )
    {
      qWarning() << "Incorrect autorization code found in saved chats file";
      dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER, QString( "%1 %2" )
                             .arg( IconManager::instance().toHtml( "warning.png", "*!*" ) )
                             .arg( tr( "Saved chats had an incorrect authorization code.") ),
                             DispatchToChat, ChatMessage::System );
    }
    loading_status = tr( "%1 saved chats are added to history" ).arg( bscl->savedChats().size() );
    dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER, QString( "%1 %2." ).arg( IconManager::instance().toHtml( "saved-chat.png", "*H*" ), loading_status ),
                           DispatchToChat, ChatMessage::System );
  }

  if( bscl->unsentMessages().size() > 0 )
  {
    if( bscl->protocolVersion() > SAVE_MESSAGE_AUTH_CODE_PROTO_VERSION && Settings::instance().saveMessagesTimestamp().isValid() && bscl->unsentMessagesAuthCode() != MessageManager::instance().saveMessagesAuthCode() )
    {
      qWarning() << "Incorrect autorization code found in offline messages file";
      dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER, QString( "%1 %2" )
                             .arg( IconManager::instance().toHtml( "warning.png", "*!*" ) )
                             .arg( tr( "Offline messages still to be sent had an incorrect authorization code and will not be sent.") ),
                             DispatchToChat, ChatMessage::System );
    }
    else
    {
      dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER, QString( "%1 %2" )
                             .arg( IconManager::instance().toHtml( "unsent-message.png", "*m*" ) )
                             .arg( tr( "%1 offline messages will be sent as soon as possible." ).arg( bscl->unsentMessages().size() ) ),
                             DispatchToChat, ChatMessage::System );
      foreach( MessageRecord mr, bscl->unsentMessages() )
      {
        QString msg_txt;

        if( mr.isVoiceMessage() )
        {
          FileInfo saved_fi = Protocol::instance().fileInfoFromMessage( mr.message() );
          msg_txt = saved_fi.name();
        }
        else
        {
          msg_txt = mr.message().text();
          if( msg_txt.size() > 120 )
          {
             msg_txt.resize( 120 );
             msg_txt.append( "..." );
          }
        }

        User to_user = UserManager::instance().findUser( mr.toUserId() );
        Chat to_chat = ChatManager::instance().chat( mr.chatId() );

        if( !to_user.isValid() || !to_chat.isValid() )
        {
          qWarning() << "Offline" << (mr.isVoiceMessage() ? "voice message" : "message" ) << "to user" << qPrintable( QString( "%1" ).arg( to_user.isValid() ? to_user.name() : QString( "%1 (unknown)").arg( mr.toUserId() ) ) )
                     << "and to chat" << qPrintable( QString( "%1" ).arg( to_chat.isValid() ? to_chat.name() : QString( "%1 (unknown)").arg( mr.chatId() ) ) )
                     << "will not be sent:" << qPrintable( msg_txt );
        }
        else
        {
          if( mr.isVoiceMessage() )
          {
            QString file_path = QString( "%1%2%3" ).arg( Settings::instance().cacheFolder() ).arg( QDir::separator() ).arg( msg_txt );
            QUrl file_url = QUrl::fromLocalFile( file_path );
#ifdef BEEBEEP_USE_VOICE_CHAT
            file_url.setScheme( FileInfo::urlSchemeVoiceMessage() );
#endif
            msg_txt = QString( "<a href=\"%1\">%2</a>" ).arg( file_url.toString(), tr( "voice message" ) );
          }
        }

        dispatchSystemMessage( mr.chatId(), ID_LOCAL_USER, QString( "%1 %2: &quot;%3&quot; [%4]" )
                               .arg( IconManager::instance().toHtml( "unsent-message.png", "*m*" ) )
                               .arg( tr( "Offline message will be sent to %1" ).arg( Bee::replaceHtmlSpecialCharacters( to_user.name() ) ) )
                               .arg( msg_txt )
                               .arg( mr.message().timestamp().toString( "yyyy-MM-dd hh:mm:ss" ) ),
                               DispatchToChat, ChatMessage::Other );
        MessageManager::instance().addMessageRecord( mr );
      }
    }
  }
  if( beeApp )
    beeApp->removeJob( bscl );
  bscl->deleteLater();
  emit savedChatListAvailable();
}

bool Core::clearMessagesInChat( VNumber chat_id, bool clear_history )
{
  Chat c = ChatManager::instance().chat( chat_id );
  if( !c.isValid() )
  {
    qWarning() << "Invalid chat" << chat_id << "in Core::clearMessagesInChat(...)";
    return false;
  }

  if( clear_history )
    ChatManager::instance().removeSavedTextFromChat( c.name() );
  c.clearMessages();
  addChatHeader( &c );
  ChatManager::instance().setChat( c );
  emit chatChanged( c );
  return true;
}

bool Core::clearSystemMessagesInChat( VNumber chat_id )
{
  Chat c = ChatManager::instance().chat( chat_id );
  if( !c.isValid() )
  {
    qWarning() << "Invalid chat" << chat_id << "in Core::clearSystemMessagesInChat(...)";
    return false;
  }
  c.clearSystemMessages();
  ChatManager::instance().setChat( c );
  emit chatChanged( c );
  return true;
}

int Core::checkOfflineMessagesForUser( const User& u )
{
  QList<MessageRecord> message_list = MessageManager::instance().takeMessagesToSendToUserId( u.id() );
  if( message_list.isEmpty() )
    return 0;

  QList<VNumber> chat_list;
  QList<MessageRecord> unsent_messages;
  int messages_sent = 0;
  bool user_is_online = false;

  Connection* c = connection( u.id() );
  if( c && c->isValid() && c->isConnected() )
  {
    user_is_online = true;
    foreach( MessageRecord mr, message_list )
    {
      if( mr.isVoiceMessage() )
      {
        if( !mp_fileTransfer->isActive() )
        {
#ifdef BEEBEEP_DEBUG
          qDebug() << "Unable to send offline voice message to" << qPrintable( u.path() ) << "... (file transfer is disabled) Try again later";
#endif
          unsent_messages.append( mr );
          continue;
        }

        FileInfo fi = Protocol::instance().fileInfoFromMessage( mr.message() );
        QString file_path = QString( "%1%2%3" ).arg( Settings::instance().cacheFolder() ).arg( QDir::separator() ).arg( fi.name() );
        QFileInfo fi_voice( file_path );
        if( fi_voice.exists() )
        {
          FileInfo file_info = mp_fileTransfer->addFile( fi_voice, fi.shareFolder(), fi.isInShareBox(), fi.chatPrivateId(), fi.contentType() );
          mr.setMessage( Protocol::instance().fileInfoToMessage( file_info ) );
        }
        else
        {
          qWarning() << "Cannot send voice message to" << qPrintable( u.path() ) << "because the file because the file no longer exists:" << qPrintable( file_path );
          continue;
        }
      }

      if( c->sendMessage( mr.message() ) )
      {
        if( !chat_list.contains( mr.chatId() ) )
          chat_list.append( mr.chatId() );
        messages_sent++;
      }
      else
      {
#ifdef BEEBEEP_DEBUG
        qDebug() << "Unable to send offline message to" << qPrintable( u.path() ) << "... Try again later";
#endif
        unsent_messages.append( mr );
      }
    }
  }

  if( !user_is_online )
    unsent_messages = message_list;

  if( messages_sent > 0 )
  {
    foreach( VNumber ci, chat_list )
    {
      dispatchSystemMessage( ci, u.id(), QString( "%1 %2" )
                             .arg( IconManager::instance().toHtml( "unsent-message.png", "*m*" ) )
                             .arg( tr( "Offline messages sent to %2." ).arg( Bee::replaceHtmlSpecialCharacters( u.name() ) ) ),
                             DispatchToChat, ChatMessage::Other );
    }
    qDebug() << messages_sent << "offline messages sent to" << qPrintable( u.path() ) << "-" << unsent_messages.size() << "messages left";
    emit offlineMessageSentToUser( u );
  }

  if( !unsent_messages.isEmpty() )
    MessageManager::instance().addMessageRecords( unsent_messages );
  return messages_sent;
}

bool Core::readAllMessagesInChat( VNumber chat_id )
{
  Chat c = ChatManager::instance().chat( chat_id );
  if( !c.isValid() )
  {
  #ifdef BEEBEEP_DEBUG
    qWarning() << "Invalid chat found in readAllMessagesInChat" << chat_id;
  #endif
    return false;
  }

  if( c.unreadMessages() > 0 )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Read all chat messages in" << chat_id << qPrintable( c.name() );
#endif
    c.readAllMessages();
    if( Settings::instance().chatClearAllReadMessages() )
    {
#ifdef BEEBEEP_DEBUG
     qDebug() << "Clear all messages in" << chat_id << qPrintable( c.name() );
#endif
      c.clearMessages();
      addChatHeader( &c );
    }
    ChatManager::instance().setChat( c );
    emit chatChanged( c );
    sendLocalUserHasReadChatMessage( c );
    return true;
  }
  else
    return false;
}

void Core::sendLocalUserHasReadChatMessage( const Chat& c )
{
  if( !isConnected() )
    return;

  Message m = Protocol::instance().chatReadMessage( c );
  if( !m.isValid() )
    return;

  Connection* user_connection;
  foreach( VNumber to_user_id, c.usersId() )
  {
    if( to_user_id == ID_LOCAL_USER )
      continue;

    user_connection = connection( to_user_id );
    if( user_connection )
    {
      if( user_connection->protoVersion() > 62 )
      {
#ifdef BEEBEEP_DEBUG
        qDebug() << "Sending read chat message to" << to_user_id;
#endif
        user_connection->sendMessage( m );
      }
#ifdef BEEBEEP_DEBUG
      else
        qDebug() << "Unable to send read chat message to" << to_user_id << "because the user has an old protocol version" << user_connection->protoVersion();
#endif
    }
  }
}

void Core::sendBuzzToUser( VNumber user_id )
{
  User u = UserManager::instance().userList().find( user_id );
  if( !u.isValid() )
  {
    qWarning() << "Unable to send a buzz to invalid user id" << user_id;
    return;
  }

  if( sendMessageToLocalNetwork( u, Protocol::instance().buzzMessage() ) )
  {
    QString sys_msg = tr( "%1 You have buzzed %2." ).arg( IconManager::instance().toHtml( "bell.png", "*Z*" ), Bee::replaceHtmlSpecialCharacters( u.name() ) );
    Chat c = ChatManager::instance().privateChatForUser( u.id() );
    if( !c.isValid() )
      c = ChatManager::instance().defaultChat();
    dispatchSystemMessage( c.id(), u.id(), sys_msg, DispatchToChat, ChatMessage::Other );
  }
  else
    qWarning() << "Unable to send a buzz to offline user" << qPrintable( u.path() );
}

void Core::removeSavedChat( const QString& chat_name )
{
  if( chat_name.isEmpty() )
    return;

  qDebug() << "Delete saved chat:" << chat_name;
  ChatManager::instance().removeSavedTextFromChat( chat_name );
  Chat c = ChatManager::instance().findChatByName( chat_name );
  if( c.isValid() )
    emit chatChanged( c );
}

void Core::addChatHeader( Chat* p_chat )
{
  if( !p_chat )
    return;

  bool chat_has_history = ChatManager::instance().chatHasSavedText( p_chat->name() );
  if( !chat_has_history && p_chat->name().contains( "@" ) )
    chat_has_history = ChatManager::instance().chatHasSavedText( User::nameFromPath( p_chat->name() ) );

  if( chat_has_history )
    p_chat->addMessage( ChatMessage( ID_SYSTEM_MESSAGE, Protocol::instance().systemMessage( "" ), ChatMessage::History ) );

  QString header_msg = "";

  if( p_chat->isDefault() )
  {
    QString hv_msg = QByteArray::fromBase64( "QmVlQkVFUCAlMSAmY29weTsgJTIgTWFyY28gTWFzdHJvZGRp=" );
    header_msg = QString( "%1 <b>%2</b>." ).arg( IconManager::instance().toHtml( "beebeep.png", "***" ), hv_msg.arg( Settings::instance().version( false, false ) ).arg( QDate::currentDate().year() ) );
    p_chat->addMessage( ChatMessage( ID_SYSTEM_MESSAGE, Protocol::instance().systemMessage( header_msg ), ChatMessage::System ) );
    header_msg = QString( "%1 %2." ).arg( IconManager::instance().toHtml( "chat-small.png", "*C*" ), tr( "Chat with all users" ) );
  }
  else if( p_chat->isPrivate() )
  {
    User u = UserManager::instance().findUser( p_chat->privateUserId() );
    header_msg = tr( "%1 Chat with %2." ).arg( IconManager::instance().toHtml( "chat-small.png", "*C*" ), u.name().isEmpty() ? tr( "Unknown %1" ).arg( p_chat->privateUserId() ) : Bee::replaceHtmlSpecialCharacters( u.name() ) );
  }
  else
  {
    QStringList user_string_list;
    UserList ul = UserManager::instance().userList().fromUsersId( p_chat->usersId() );
    foreach( User u, ul.toList() )
    {
      if( !u.isLocal() )
        user_string_list.append( Bee::replaceHtmlSpecialCharacters( u.name() ) );
    }
    header_msg = tr( "%1 Chat with %2." ).arg( IconManager::instance().toHtml( "group.png", "*G*" ), Bee::stringListToTextString( user_string_list ) );
  }

  p_chat->addMessage( ChatMessage( ID_SYSTEM_MESSAGE, Protocol::instance().systemMessage( header_msg ), ChatMessage::Header ) );
}

void Core::linkSavedChat( const QString& from_saved_chat_name, const QString& to_saved_chat_name, bool prepend_to_existing_saved_chat )
{
  ChatManager::instance().updateChatSavedText( from_saved_chat_name, to_saved_chat_name, prepend_to_existing_saved_chat );
  Chat c = ChatManager::instance().findChatByName( from_saved_chat_name );
  if( c.isValid() )
    emit chatChanged( c );

  c = ChatManager::instance().findChatByName( to_saved_chat_name );
  if( c.isValid() )
    emit chatChanged( c );
}
