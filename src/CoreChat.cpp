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

#include "BeeUtils.h"
#include "ChatManager.h"
#include "Connection.h"
#include "Core.h"
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
  c.addUser( ID_LOCAL_USER );
  QString sHtmlMsg = tr( "%1 Chat with all local users." ).arg( Bee::iconToHtml( ":/images/chat.png", "*C*" ) );
  ChatMessage cm( ID_LOCAL_USER, Protocol::instance().systemMessage( sHtmlMsg ) );
  c.addMessage( cm );
  ChatManager::instance().setChat( c );
  emit updateChat( c.id() );
}

void Core::createPrivateChat( const User& u )
{
  qDebug() << "Creating private chat room for user" << u.path();
  QList<VNumber> user_list;
  user_list.append( u.id() );
  Chat c = Protocol::instance().createChat( user_list );
  QString sHtmlMsg = tr( "%1 Chat with %2 (%3)." ).arg( Bee::iconToHtml( ":/images/chat.png", "*C*" ), u.name(), u.path() );
  ChatMessage cm( u.id(), Protocol::instance().systemMessage( sHtmlMsg ) );
  c.addMessage( cm );
  ChatManager::instance().setChat( c );
  emit updateChat( c.id() );
}

void Core::checkGroupChatAfterUserReconnect( const User& u )
{
  QList<Chat> chat_list = ChatManager::instance().groupChatForUser( u.id() );
  if( chat_list.isEmpty() )
    return;

  foreach( Chat c, chat_list )
  {
    UserList ul = UserManager::instance().userList().fromUsersId( c.usersId() );
    sendGroupChatRequestMessage( c, ul );
  }
}

void Core::createGroupChat( const QString& chat_name, const QList<VNumber>& users_id, const QString& chat_private_id, bool broadcast_message )
{
  qDebug() << "Creating group chat named" << chat_name;
  UserList ul = UserManager::instance().userList().fromUsersId( users_id );
  QString sHtmlMsg;

  Chat c = Protocol::instance().createChat( users_id );
  c.setName( chat_name );

  if( chat_private_id.isEmpty() )
  {
    sHtmlMsg = tr( "%1 You have created the group %2." ).arg( Bee::iconToHtml( ":/images/chat-create.png", "*G*" ), chat_name );
    c.addMessage( ChatMessage( ID_LOCAL_USER, Protocol::instance().systemMessage( sHtmlMsg ) ) );
  }
  else
  {
    c.setPrivateId( chat_private_id );
    sHtmlMsg = tr( "%1 Welcome to the group %2." ).arg( Bee::iconToHtml( ":/images/chat-create.png", "*G*" ), chat_name );
    c.addMessage( ChatMessage( ID_LOCAL_USER, Protocol::instance().systemMessage( sHtmlMsg ) ) );
  }

  QStringList user_string_list;
  foreach( User u, ul.toList() )
  {
    if( !u.isLocal() )
      user_string_list.append( QString( "%1 (%2)" ).arg( u.name(), u.path() ) );
  }

  sHtmlMsg = tr( "%1 Chat with %2." ).arg( Bee::iconToHtml( ":/images/group-add.png", "*G*" ), user_string_list.join( ", " ) );
  c.addMessage( ChatMessage( ID_LOCAL_USER, Protocol::instance().systemMessage( sHtmlMsg ) ) );

  ChatManager::instance().setChat( c );

  if( broadcast_message )
    sendGroupChatRequestMessage( c, ul );

  emit updateChat( c.id() );
}

void Core::changeGroupChat( VNumber chat_id, const QString& chat_name, const QList<VNumber>& users_id, bool broadcast_message )
{
  qDebug() << "Changing group chat named" << chat_name;
  UserList ul = UserManager::instance().userList().fromUsersId( users_id );
  Chat c = ChatManager::instance().chat( chat_id );
  if( !c.isValid() )
  {
    qWarning() << "Unable to change group with id" << chat_id << ". Chat not found";
    return;
  }

  QStringList user_added_string_list;
  QStringList user_string_list;
  QString sHtmlMsg;
  bool chat_changed = false;

  if( c.name() != chat_name )
  {
    sHtmlMsg = tr( "%1 The group has a new name: %2." ).arg( Bee::iconToHtml( ":/images/chat.png", "*G*" ), chat_name );
    c.addMessage( ChatMessage( ID_LOCAL_USER, Protocol::instance().systemMessage( sHtmlMsg ) ) );
    c.setName( chat_name );
    chat_changed = true;
  }

  foreach( User u, ul.toList() )
  {
    if( !u.isLocal() )
    {
      if( !c.usersId().contains( u.id() ) )
        user_added_string_list << u.path();
      user_string_list.append( QString( "%1 (%2)" ).arg( u.name(), u.path() ) );
    }
  }

  if( !user_added_string_list.isEmpty() )
  {
    sHtmlMsg = tr( "%1 Adding in group: %2." ).arg( Bee::iconToHtml( ":/images/group-add.png", "*G*" ), user_added_string_list.join( ", " ) );
    c.addMessage( ChatMessage( ID_LOCAL_USER, Protocol::instance().systemMessage( sHtmlMsg ) ) );

    sHtmlMsg = tr( "%1 Chat with %2." ).arg( Bee::iconToHtml( ":/images/user-list.png", "*G*" ), user_string_list.join( ", " ) );
    c.addMessage( ChatMessage( ID_LOCAL_USER, Protocol::instance().systemMessage( sHtmlMsg ) ) );

    chat_changed = true;
  }

  if( chat_changed )
    ChatManager::instance().setChat( c );

  if( broadcast_message )
    sendGroupChatRequestMessage( c, ul );
  else
  {
    if( chat_changed )
      emit updateChat( c.id() );
  }
}

int Core::sendChatMessage( VNumber chat_id, const QString& msg )
{
  if( !isConnected( true ) )
  {
    dispatchSystemMessage( "", chat_id, ID_LOCAL_USER, tr( "Unable to send the message: you are not connected." ), DispatchToChat );
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

  Message m;
  QString msg_to_send = msg;

  if( !Settings::instance().chatUseHtmlTags() )
  {
    msg_to_send.replace( QLatin1Char( '<' ), QLatin1String( "&lt;" ) );
    msg_to_send.replace( "&lt;3", "<3" ); // hearth emoticon
  }

  PluginManager::instance().parseText( &msg_to_send, true );
  m = Protocol::instance().chatMessage( msg_to_send );
  ChatMessageData cmd;
  cmd.setTextColor( Settings::instance().chatFontColor() );
  if( c.isGroup() )
  {
    m.addFlag( Message::GroupChat );
    cmd.setGroupId( c.privateId() );
  }

  m.setData( Protocol::instance().chatMessageDataToString( cmd ) );

  int messages_sent = 0;

  if( chat_id == ID_DEFAULT_CHAT )
  {
    foreach( Connection *c, m_connections )
    {
      if( !c->sendMessage( m ) )
        dispatchSystemMessage( "", ID_DEFAULT_CHAT, ID_LOCAL_USER, tr( "Unable to send the message to %1." )
                               .arg( UserManager::instance().userList().find( c->userId() ).path() ), DispatchToChat );
      else
        messages_sent += 1;
    }
  }
  else
  {
    if( !c.isGroup() )
      m.addFlag( Message::Private );

    UserList user_list = UserManager::instance().userList().fromUsersId( c.usersId() );
    foreach( User u, user_list.toList() )
    {
      if( u.isLocal() )
        continue;

      if( !u.isOnLan() )
      {
        sendXmppChatMessage( u, m );
        messages_sent += 1;
        continue;
      }

      if( sendMessageToLocalNetwork( u, m ) )
        messages_sent += 1;
      else
      {
        if( !c.isGroup() )
          dispatchSystemMessage( "", chat_id, ID_LOCAL_USER, tr( "Unable to send the message to %1." ).arg( u.path() ), DispatchToChat );
      }
    }
  }

  ChatMessage cm( ID_LOCAL_USER, m );
  dispatchToChat( cm, chat_id );

  if( messages_sent == 0 )
    dispatchSystemMessage( "", chat_id, ID_LOCAL_USER, tr( "Nobody has received the message." ), DispatchToChat );

  return messages_sent;
}

void Core::sendWritingMessage( VNumber chat_id )
{
  if( !isConnected( true ) )
    return;

  Chat from_chat = ChatManager::instance().chat( chat_id );
  UserList user_list = UserManager::instance().userList().fromUsersId( from_chat.usersId() );
  foreach( User u, user_list.toList() )
  {
    if( u.isLocal() )
      continue;

    if( !u.isOnLan() )
    {
      sendXmppUserComposing( u );
      continue;
    }

    Connection* c = connection( u.id() );
    if( !c )
      continue;

    c->sendData( Protocol::instance().writingMessage() );
  }
}

void Core::showTipOfTheDay()
{
  QString tip_of_the_day = QString( "%1 %2" ).arg( Bee::iconToHtml( ":/images/tip.png", "*T*" ),
                                                   qApp->translate( "Tips", BeeBeepTips[ Random::number( 0, (BeeBeepTipsSize-1) ) ] ) );
  dispatchSystemMessage( "", ID_DEFAULT_CHAT, ID_LOCAL_USER, tip_of_the_day, DispatchToChat );
}

bool Core::chatHasService( const Chat& c, const QString& service_name )
{
  if( !c.isValid() )
    return false;
  if( c.isDefault() )
    return true;
  UserList chat_users = UserManager::instance().userList().fromUsersId( c.usersId() );
  if( chat_users.serviceUserList( service_name ).toList().isEmpty() )
    return false;
  else
    return true;
}

void Core::sendGroupChatRequestMessage( const Chat& group_chat, const UserList& user_list )
{
  Message group_message = Protocol::instance().groupChatRequestMessage( group_chat );
  qDebug() << "Group message request:" << Protocol::instance().fromMessage( group_message );
  qDebug() << "Send group chat request to:" << group_message.text();

  foreach( User u, user_list.toList() )
  {
    if( u.isLocal() )
      continue;

    if( !u.isOnLan() )
    {
      // FIXME!!!
      continue;
    }

    if( !sendMessageToLocalNetwork( u, group_message ) )
      dispatchSystemMessage( "", group_chat.id(), ID_LOCAL_USER, tr( "%1 cannot be invited to the group." ).arg( u.path() ), DispatchToChat );
  }
}

bool Core::sendMessageToLocalNetwork( const User& to_user, const Message& m )
{
  Connection* c = connection( to_user.id() );
  if( !c )
  {
    qDebug() << "Unable to find connection socket for user" << to_user.id();
    return false;
  }

  return c->sendMessage( m );
}
