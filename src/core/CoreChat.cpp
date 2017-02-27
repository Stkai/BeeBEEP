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
#include "MessageManager.h"
#include "Protocol.h"
#include "PluginManager.h"
#include "Random.h"
#include "Settings.h"
#include "Tips.h"
#include "UserManager.h"


void Core::createDefaultChat()
{
  qDebug() << "Creating default chat";
  Chat c;
  c.setId( ID_DEFAULT_CHAT );
  c.setName( Settings::instance().defaultChatName() );
  c.addUser( ID_LOCAL_USER );
  c.setPrivateId( Settings::instance().defaultChatPrivateId() );
  qDebug() << "Default chat private id:" << c.privateId();
  QString sHtmlMsg = tr( "%1 Chat with all local users." ).arg( Bee::iconToHtml( ":/images/chat.png", "*C*" ) );
  c.addMessage( ChatMessage( ID_LOCAL_USER, Protocol::instance().systemMessage( sHtmlMsg ), ChatMessage::Header ) );

  if( !Settings::instance().isFacebookPageLinkClicked() )
  {
    sHtmlMsg = QString( "%1 %2 <a href=""%3"">%4</a><br />" ).arg( Bee::iconToHtml( ":/images/thumbup.png", "*!*" ),
                                                                 tr( "Help me to know how many people are really using BeeBEEP." ),
                                                                 Settings::instance().facebookPage(),
                                                                 tr( "Please add a like on Facebook." ) );
    c.addMessage( ChatMessage( ID_LOCAL_USER, Protocol::instance().systemMessage( sHtmlMsg ), ChatMessage::Other ) );
  }

  sHtmlMsg = QString( "%1 <b>%2</b><br />" ).arg( Bee::iconToHtml( ":/images/CopyMastro.png", "*!*" ),
                                               tr( "Try the new awesome application developed by Marco Mastroddi" )
                                               + QString( " : <a href=""%1""><b>CopyMastro</b></a> !!!" ).arg( Settings::instance().copyMastroWebSite() ) );
  c.addMessage( ChatMessage( ID_LOCAL_USER, Protocol::instance().systemMessage( sHtmlMsg ), ChatMessage::Other ) );

  if( QDate::currentDate().month() == 4 && QDate::currentDate().day() == 6 )
  {
    int my_age = QDate::currentDate().year() - 1975;
    sHtmlMsg = QString( "%1 <b>%2</b>" ).arg( Bee::iconToHtml( ":/images/birthday.png", "*!*" ),
                                                 tr( "Happy birthday to Marco Mastroddi: %1 years old today! Cheers!!!" ).arg( my_age ) );
    c.addMessage( ChatMessage( ID_LOCAL_USER, Protocol::instance().systemMessage( sHtmlMsg ), ChatMessage::Chat ) );
  }

  if( QDate::currentDate().month() == 1 && QDate::currentDate().day() >= 1 && QDate::currentDate().day() <= 8 )
  {
    QString new_year_icons;
    if( Settings::instance().useNativeEmoticons() )
      new_year_icons = QString( "%1&nbsp;&nbsp;%2&nbsp;&nbsp;%3" ).arg( QString::fromUtf8( "🎆" ), QString::fromUtf8( "🎆" ), QString::fromUtf8( "🎆" ) );
    else
      new_year_icons = QString( "%1&nbsp;&nbsp;%2&nbsp;&nbsp;%3" ).arg( Bee::iconToHtml( ":/emojis/objects/1f386.png", "*!*" ), Bee::iconToHtml( ":/emojis/objects/1f386.png", "*!*" ), Bee::iconToHtml( ":/emojis/objects/1f386.png", "*!*" ) );
    sHtmlMsg = QString( "%1&nbsp;&nbsp;&nbsp;<font color=red><b>%2</b></font>&nbsp;&nbsp;&nbsp;%3" ).arg( new_year_icons, tr( "Happy New Year!" ), new_year_icons );
    c.addMessage( ChatMessage( ID_LOCAL_USER, Protocol::instance().systemMessage( sHtmlMsg ), ChatMessage::Chat ) );
  }

  if( Settings::instance().localUser().isBirthDay() )
  {
    sHtmlMsg = QString( "%1 <b>%2</b>" ).arg( Bee::iconToHtml( ":/images/birthday.png", "*!*" ), tr( "Happy Birthday to you!" ) );
    c.addMessage( ChatMessage( ID_LOCAL_USER, Protocol::instance().systemMessage( sHtmlMsg ), ChatMessage::Chat ) );
  }

  ChatManager::instance().setChat( c );
  emit chatChanged( c );
}

void Core::createPrivateChat( const User& u )
{
  qDebug() << "Creating private chat room for user" << u.id() << u.path();
  QList<VNumber> user_list;
  user_list.append( u.id() );
  Chat c = Protocol::instance().createChat( user_list );
  c.setName( u.name() );
  QString sHtmlMsg = tr( "%1 Chat with %2." ).arg( Bee::iconToHtml( ":/images/chat.png", "*C*" ), u.name() );
  ChatMessage cm( u.id(), Protocol::instance().systemMessage( sHtmlMsg ), ChatMessage::Header );
  c.addMessage( cm );
  ChatManager::instance().setChat( c );

  bool chat_has_history = ChatManager::instance().chatHasSavedText( c.name() );
  if( !chat_has_history && c.name().contains( "@" ) )
    chat_has_history = ChatManager::instance().chatHasSavedText( User::nameFromPath( c.name() ) );

  if( chat_has_history )
  {
    c = ChatManager::instance().chat( c.id() );
    cm = ChatMessage( u.id(), Protocol::instance().systemMessage( "" ), ChatMessage::History );
    c.addMessage( cm );
    ChatManager::instance().setChat( c );
  }

  emit chatChanged( c );
}

void Core::checkGroupChatAfterUserReconnect( const User& u )
{
  QList<Chat> chat_list = ChatManager::instance().groupChatsForUser( u.id() );
  if( chat_list.isEmpty() )
    return;

  foreach( Chat c, chat_list )
  {
    UserList ul = UserManager::instance().userList().fromUsersId( c.usersId() );
    sendGroupChatRequestMessage( c, ul );
  }
}

void Core::createGroupChat( const Group& g, bool broadcast_message )
{
  createGroupChat( g.name(), g.usersId(), g.privateId(), broadcast_message );
}

void Core::createGroupChat( const QString& chat_name, const QList<VNumber>& users_id, const QString& chat_private_id, bool broadcast_message )
{
  qDebug() << "Creating group chat named" << chat_name << "with private id:" << chat_private_id;
  UserList ul = UserManager::instance().userList().fromUsersId( users_id );
  QString sHtmlMsg;

  Chat c = Protocol::instance().createChat( users_id );
  c.setName( chat_name );

  if( chat_private_id.isEmpty() )
  {
    sHtmlMsg = tr( "%1 You have created the group %2." ).arg( Bee::iconToHtml( ":/images/chat-create.png", "*G*" ), QString( "<b>%1</b>" ).arg( chat_name ) );
    c.addMessage( ChatMessage( ID_LOCAL_USER, Protocol::instance().systemMessage( sHtmlMsg ), ChatMessage::Header ) );
  }
  else
    c.setPrivateId( chat_private_id );

  QStringList user_string_list;
  foreach( User u, ul.toList() )
  {
    if( !u.isLocal() )
      user_string_list.append( QString( "%1 (%2)" ).arg( u.name(), u.accountPath() ) );
  }

  sHtmlMsg = tr( "%1 Chat with %2." ).arg( Bee::iconToHtml( ":/images/group.png", "*G*" ), user_string_list.join( ", " ) );
  c.addMessage( ChatMessage( ID_LOCAL_USER, Protocol::instance().systemMessage( sHtmlMsg ), ChatMessage::Header ) );

  if( ChatManager::instance().chatHasSavedText( c.name() ) )
    c.addMessage( ChatMessage( ID_LOCAL_USER, Protocol::instance().systemMessage( "" ), ChatMessage::History ) );

  ChatManager::instance().setChat( c );

  if( broadcast_message && isConnected() )
    sendGroupChatRequestMessage( c, ul );

  emit chatChanged( c );
}

void Core::changeGroupChat( VNumber chat_id, const QString& chat_name, const QList<VNumber>& users_id, bool broadcast_message )
{
#ifdef BEEBEEP_DEBUG
  qDebug() << "Changing group chat named" << chat_name;
#endif
  Chat c = ChatManager::instance().chat( chat_id );
  if( !c.isValid() )
  {
    qWarning() << "Unable to change group with id" << chat_id << ". Chat not found";
    return;
  }

  UserList group_new_members = UserManager::instance().userList().fromUsersId( users_id );
  UserList group_old_members = UserManager::instance().userList().fromUsersId( c.usersId() );

  QStringList user_added_string_list;
  QStringList user_removed_string_list;
  QStringList user_string_list;
  QString sHtmlMsg;
  bool chat_changed = false;

  if( c.name() != chat_name )
  {
    sHtmlMsg = tr( "%1 The group has a new name: %2." ).arg( Bee::iconToHtml( ":/images/chat.png", "*G*" ), chat_name );
    c.addMessage( ChatMessage( ID_LOCAL_USER, Protocol::instance().systemMessage( sHtmlMsg ), ChatMessage::System ) );
    chat_changed = true;
    if( ChatManager::instance().chatHasSavedText( c.name() ) )
    {
#ifdef BEEBEEP_DEBUG
      qDebug() << "Changing saved chat from" << c.name() << "to" << chat_name;
#endif
      ChatManager::instance().updateChatSavedText( c.name(), chat_name, false );
    }
    c.setName( chat_name );
  }

  foreach( User u, group_new_members.toList() )
  {
    if( !u.isLocal() )
    {
      if( c.addUser( u.id() ) )
        user_added_string_list << QString( "%1 (%2)" ).arg( u.name(), u.accountPath() );
      user_string_list.append( u.name() );
    }
  }

  foreach( User u, group_old_members.toList() )
  {
    if( !u.isLocal() )
    {
      if( !group_new_members.has( u.id() ) )
      {
        user_removed_string_list << QString( "%1 (%2)" ).arg( u.name(), u.accountPath() );
        c.removeUser( u.id() );
      }
    }
  }

  if( user_removed_string_list.size() > 0 )
  {
    sHtmlMsg = tr( "%1 Members removed from the group: %2." ).arg( Bee::iconToHtml( ":/images/group-remove.png", "*G*" ), user_removed_string_list.join( ", " ) );
    c.addMessage( ChatMessage( ID_LOCAL_USER, Protocol::instance().systemMessage( sHtmlMsg ), ChatMessage::System ) );
    chat_changed = true;
  }

  if( user_added_string_list.size() > 0 )
  {
    sHtmlMsg = tr( "%1 Members added to the group: %2." ).arg( Bee::iconToHtml( ":/images/group-add.png", "*G*" ), user_added_string_list.join( ", " ) );
    c.addMessage( ChatMessage( ID_LOCAL_USER, Protocol::instance().systemMessage( sHtmlMsg ), ChatMessage::System ) );
    chat_changed = true;
  }

  if( user_removed_string_list.size() > 0 || user_added_string_list.size() > 0 )
  {
    sHtmlMsg = tr( "%1 Chat with %2." ).arg( Bee::iconToHtml( ":/images/group.png", "*G*" ), user_string_list.join( ", " ) );
    c.addMessage( ChatMessage( ID_LOCAL_USER, Protocol::instance().systemMessage( sHtmlMsg ), ChatMessage::System ) );
  }

  if( chat_changed )
  {
    ChatManager::instance().setChat( c );
    emit chatChanged( c );
  }

  if( broadcast_message && isConnected() )
    sendGroupChatRequestMessage( c, group_new_members );
}

int Core::sendChatMessage( VNumber chat_id, const QString& msg )
{
  if( !isConnected() )
  {
    dispatchSystemMessage( chat_id, ID_LOCAL_USER, tr( "Unable to send the message: you are not connected." ), DispatchToChat, ChatMessage::System );
    return 0;
  }

  if( chat_id == ID_DEFAULT_CHAT && !Settings::instance().chatWithAllUsersIsEnabled() )
  {
    dispatchSystemMessage( chat_id, ID_LOCAL_USER, tr( "Unable to send the message: this chat is disabled." ), DispatchToChat, ChatMessage::System );
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

  int messages_sent = 0;

  ChatMessage cm( ID_LOCAL_USER, m, ChatMessage::Chat );
  dispatchToChat( cm, chat_id );

  if( chat_id == ID_DEFAULT_CHAT )
  {
    foreach( Connection *user_connection, m_connections )
    {
      if( !user_connection->sendMessage( m ) )
        dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER, tr( "Unable to send the message to %1." )
                               .arg( UserManager::instance().findUser( user_connection->userId() ).path() ),
                               DispatchToChat, ChatMessage::System );
      else
        messages_sent += 1;
    }
  }
  else
  {
    UserList user_list = UserManager::instance().userList().fromUsersId( c.usersId() );
    QStringList offline_users;
    foreach( User u, user_list.toList() )
    {
      if( u.isLocal() )
        continue;

      if( sendMessageToLocalNetwork( u, m ) )
      {
        messages_sent += 1;
      }
      else
      {
        MessageManager::instance().addMessageToSend( u.id(), chat_id, m );
        offline_users.append( u.name() );
      }
    }

    if( !offline_users.isEmpty() )
      dispatchSystemMessage( chat_id, ID_LOCAL_USER, tr( "The message will be delivered to %1." ).arg( offline_users.join( ", " ) ),
                             DispatchToChat, ChatMessage::System );
  }

  if( chat_id == ID_DEFAULT_CHAT && messages_sent == 0 )
    dispatchSystemMessage( chat_id, ID_LOCAL_USER, tr( "Nobody has received the message." ), DispatchToChat, ChatMessage::System );

  c = ChatManager::instance().chat( chat_id ); // reload chat
  emit chatChanged( c );
  return messages_sent;
}

void Core::sendWritingMessage( VNumber chat_id )
{
  if( !isConnected() )
    return;

  if( chat_id == ID_DEFAULT_CHAT && !Settings::instance().chatWithAllUsersIsEnabled() )
    return;

  Chat from_chat = ChatManager::instance().chat( chat_id );
  foreach( VNumber user_id,  from_chat.usersId() )
  {
    if( user_id == ID_LOCAL_USER )
      continue;

    Connection* c = connection( user_id );
    if( !c )
      continue;

    c->sendData( Protocol::instance().writingMessage( from_chat.privateId() ) );
  }
}

void Core::showFactOfTheDay()
{
  QString fact_of_the_day = QString( "%1 %2" ).arg( Bee::iconToHtml( ":/images/fact.png", "*T*" ),
                                                   qApp->translate( "Tips", BeeBeepFacts[ Random::number( 0, (BeeBeepFactsSize-1) ) ] ) );
  dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER, fact_of_the_day, DispatchToChat, ChatMessage::Other );
}

void Core::showTipOfTheDay()
{
  QString tip_of_the_day = QString( "%1 %2" ).arg( Bee::iconToHtml( ":/images/tip.png", "*T*" ),
                                                   qApp->translate( "Tips", BeeBeepTips[ Random::number( 0, (BeeBeepTipsSize-1) ) ] ) );
  dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER, tip_of_the_day, DispatchToChat, ChatMessage::Other );
}

void Core::sendGroupChatRequestMessage( const Chat& group_chat, const UserList& user_list )
{
  Message group_message;

  foreach( User u, user_list.toList() )
  {
    if( u.isLocal() )
      continue;

    group_message = Protocol::instance().groupChatRequestMessage( group_chat, u );

#ifdef BEEBEEP_DEBUG
    QString group_members = group_message.text();
    group_members.replace( QChar::LineSeparator, ", " );
    qDebug() << "Send group chat request to:" << qPrintable( group_members );
#endif

    sendMessageToLocalNetwork( u, group_message );
  }
}

void Core::sendGroupChatRefuseMessage( const Chat& group_chat, const UserList& user_list )
{
  Message group_refuse_message = Protocol::instance().groupChatRefuseMessage( group_chat );

  foreach( User u, user_list.toList() )
  {
    if( u.isLocal() )
      continue;

    if( !u.isStatusConnected() || !sendMessageToLocalNetwork( u, group_refuse_message ) )
      dispatchSystemMessage( group_chat.id(), ID_LOCAL_USER, tr( "%1 %2 cannot be informed that you have left the group." )
                             .arg( Bee::iconToHtml( ":/images/group-remove.png", "*G*" ) ).arg( u.name() ),
                             DispatchToChat, ChatMessage::Other );
  }
}

bool Core::sendMessageToLocalNetwork( const User& to_user, const Message& m )
{
  Connection* c = connection( to_user.id() );
  if( !c )
  {
    if( to_user.isStatusConnected() )
      qWarning() << "Unable to find connection socket for user" << to_user.id();
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
  BeeApplication* bee_app = (BeeApplication*)qApp;
  bee_app->addJob( bscl );
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

  QString loading_status = QString( "%1 saved chats are added to history (elapsed time: %2)" )
                             .arg( bscl->savedChats().size() )
                             .arg( Bee::elapsedTimeToString( bscl->elapsedTime() ) );
  qDebug() << qPrintable( loading_status );
  loading_status = tr( "%1 saved chats are added to history" ).arg( bscl->savedChats().size() );

  if( bscl->savedChats().size() > 1 )
  {
    emit updateStatus( loading_status, 3000 );
    dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER, QString( "%1 %2." ).arg( Bee::iconToHtml( ":/images/saved-chat.png", "*H*" ), loading_status ),
                           DispatchToChat, ChatMessage::Other );
  }

  ChatManager::instance().addSavedChats( bscl->savedChats() );

  BeeApplication* bee_app = (BeeApplication*)qApp;
  bee_app->removeJob( bscl );

  bscl->deleteLater();
  emit savedChatListAvailable();
}

void Core::clearMessagesInChat( VNumber chat_id )
{
  Chat c = ChatManager::instance().chat( chat_id );
  if( c.isValid() )
  {
    c.clearMessages();
    ChatManager::instance().setChat( c );
  }
}

bool Core::removeUserFromChat( const User& u, VNumber chat_id )
{
  if( chat_id == ID_DEFAULT_CHAT )
    return false;

  Chat c = ChatManager::instance().chat( chat_id );
  if( !c.isValid() )
    return false;
  if( !c.isGroup() )
    return false;
  Group g = UserManager::instance().findGroupByPrivateId( c.privateId() );
  if( g.isValid() )
    return false;

  QString sHtmlMsg;

  if( u.isLocal() )
    sHtmlMsg = tr( "%1 You have left the group." ).arg( Bee::iconToHtml( ":/images/group-remove.png", "*G*" ) );
  else
    sHtmlMsg = tr( "%1 %2 has left the group." ).arg( Bee::iconToHtml( ":/images/group-remove.png", "*G*" ), u.name() );

  c.removeUser( u.id() );
  c.addMessage( ChatMessage( ID_LOCAL_USER, Protocol::instance().systemMessage( sHtmlMsg ), ChatMessage::Other ) );
  ChatManager::instance().setChat( c );
  emit chatChanged( c );

  if( u.isLocal() )
  {
    UserList group_members = UserManager::instance().userList().fromUsersId( c.usersId() );
    sendGroupChatRefuseMessage( c, group_members );
  }

  return true;
}

bool Core::removeChat( VNumber chat_id )
{
  Chat c = ChatManager::instance().chat( chat_id );
  if( !c.isValid() )
    return false;
  if( c.isDefault() )
    return false;

  if( ChatManager::instance().chatHasSavedText( c.name() ) )
    ChatManager::instance().removeSavedTextFromChat( c.name() );

  if( c.isPrivate() )
  {
    c.clearMessages();
    qDebug() << "Private chat with" << c.name() << "cleared";
    ChatManager::instance().setChat( c );
    return true;
  }


  if( ChatManager::instance().removeChat( c ) )
  {
    qDebug() << "Group chat deleted:" << c.name();
    UserList group_members = UserManager::instance().userList().fromUsersId( c.usersId() );
    sendGroupChatRefuseMessage( c, group_members );
    return true;
  }

  qWarning() << "Unable to delete chat with id" << c.id() << "and name" << c.name();
  return false;
}

void Core::checkOfflineMessagesForUser( const User& u )
{
  QList<MessageRecord> message_list = MessageManager::instance().takeMessagesToSend( u.id() );
  if( message_list.isEmpty() )
    return;

  Connection* c = connection( u.id() );
  if( !c )
    return;

  QList<VNumber> chat_list;
  qDebug() << message_list.size() << "offline messages sent to" << u.path();
  foreach( MessageRecord mr, message_list )
  {
    if( !chat_list.contains( mr.chatId() ) )
      chat_list.append( mr.chatId() );
    c->sendMessage( mr.message() );
  }

  foreach( VNumber ci, chat_list )
  {
    dispatchSystemMessage( ci, u.id(), tr( "Offline messages sent to %2." ).arg( u.name() ), DispatchToChat, ChatMessage::System );
  }
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
    }
    ChatManager::instance().setChat( c );

    // no updateChat signal emit (for now)
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
    QString sys_msg = tr( "%1 You have buzzed %2." ).arg( Bee::iconToHtml( ":/images/bell.png", "*Z*" ), u.name() );
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
