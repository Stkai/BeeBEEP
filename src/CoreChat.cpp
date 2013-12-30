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
}

void Core::createPrivateChat( const User& u )
{
  qDebug() << "Creating private chat room for user" << u.path();
  QList<VNumber> user_list;
  user_list.append( u.id() );
  Chat c = Protocol::instance().createChat( user_list );
  QString sHtmlMsg = tr( "%1 Chat with %2." ).arg( Bee::iconToHtml( ":/images/chat.png", "*C*" ), u.path() );
  ChatMessage cm( u.id(), Protocol::instance().systemMessage( sHtmlMsg ) );
  c.addMessage( cm );
  ChatManager::instance().setChat( c );
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
    m.addFlag( Message::Group );
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

      Connection* c = connection( u.id() );
      if( !c )
        continue;

      if( c->sendMessage( m ) )
        messages_sent += 1;
      else
        dispatchSystemMessage( "", chat_id, ID_LOCAL_USER, tr( "Unable to send the message to %1." ).arg( u.path() ), DispatchToChat );
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
    if( c )
    {
      qDebug() << "Sending Writing Message to" << c->peerAddress().toString() << c->peerPort();
      c->sendData( Protocol::instance().writingMessage() );
    }
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

VNumber Core::createOrEditGroupChat( const Chat& c )
{
  Message group_message;
  UserList user_list;
  VNumber chat_id;

  if( c.isGroup() )
  {
    user_list = UserManager::instance().userList().fromUsersId( c.usersId() );
    ChatManager::instance().setChat( c );
    group_message = Protocol::instance().groupChatRequestMessage( c );
    chat_id = c.id();
  }
  else
  {
    Chat group_chat = Protocol::instance().createChat( c.usersId() );
    group_chat.setName( c.name() );
    user_list = UserManager::instance().userList().fromUsersId( group_chat.usersId() );
    ChatManager::instance().setChat( group_chat );
    group_message = Protocol::instance().groupChatRequestMessage( group_chat );
    chat_id = group_chat.id();
  }

  foreach( User u, user_list.toList() )
  {
    if( u.isLocal() )
      continue;

    if( !u.isOnLan() )
    {
      // FIXME!!!
      continue;
    }

    Connection* c = connection( u.id() );
    if( !c )
      continue;

    if( !c->sendMessage( group_message ) )
      dispatchSystemMessage( "", chat_id, ID_LOCAL_USER, tr( "Unable to send the group chat request message to %1." ).arg( u.path() ), DispatchToChat );
  }

  return chat_id;
}
