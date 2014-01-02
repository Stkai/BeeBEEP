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
#include "FileInfo.h"
#include "FileShare.h"
#include "Protocol.h"
#include "Settings.h"
#include "UserManager.h"


void Core::parseMessage( VNumber user_id, const Message& m )
{
  qDebug() << "Parsing message received from user" << user_id;
  User u = UserManager::instance().userList().find( user_id );
  if( !u.isValid() )
  {
    qWarning() << "Invalid user" << user_id << "found while parsing message";
    return;
  }
  parseMessage( u, m );
}

void Core::parseMessage( const User& u, const Message& m )
{
  switch( m.type() )
  {
  case Message::User:
    parseUserMessage( u, m );
    break;
  case Message::Chat:
    parseChatMessage( u, m );
    break;
 case Message::File:
    parseFileMessage( u, m );
    break;
 case Message::Share:
    parseFileShareMessage( u, m );
    break;
  default:
    qWarning() << "Core cannot parse the message with type" << m.type();
    break;
  }
}

void Core::parseUserMessage( const User& u, const Message& m )
{
  if( m.hasFlag( Message::UserWriting ) )
  {
    qDebug() << "User" << u.path() << "is writing";
    emit userIsWriting( u );
    return;
  }
  else if( m.hasFlag( Message::UserStatus ) )
  {
    User user_with_new_status = u;
    if( Protocol::instance().changeUserStatusFromMessage( &user_with_new_status, m ) )
    {
      qDebug() << "User" << user_with_new_status.path() << "changes status to" << user_with_new_status.status() << user_with_new_status.statusDescription();
      UserManager::instance().setUser( user_with_new_status );
      showUserStatusChanged( user_with_new_status );
    }
  }
  else if( m.hasFlag( Message::UserVCard ) )
  {
    User user_with_new_vcard = u;
    if( Protocol::instance().changeVCardFromMessage( &user_with_new_vcard, m ) )
    {
      qDebug() << "User" << user_with_new_vcard.path() << "has new vCard";
      UserManager::instance().setUser( user_with_new_vcard );
      showUserVCardChanged( user_with_new_vcard );
    }
    else
      qWarning() << "Unable to read vCard from the user" << u.path();
  }
  else if( m.hasFlag( Message::UserName ) )
  {
    User user_with_new_name = u;
    if( Protocol::instance().changeUserNameFromMessage( &user_with_new_name, m ) )
    {
      qDebug() << "User" << u.path() << "changes his name to" << user_with_new_name.name();
      UserManager::instance().setUser( user_with_new_name );
      showUserNameChanged( user_with_new_name, u.name() );
    }
    else
      qWarning() << "Unable to change the username of the user" << u.path() << "because message is invalid";
  }
  else
    qWarning() << "Invalid flag found in user message (CoreParser)";
}

void Core::parseFileMessage( const User& u, const Message& m )
{
  FileInfo fi = Protocol::instance().fileInfoFromMessage( m );
  if( !fi.isValid() )
  {
    qWarning() << "Invalid FileInfo received from user" << u.id() << ": [" << m.data() << "]:" << m.text();
    return;
  }

  if( m.hasFlag( Message::Refused ) )
  {
    dispatchSystemMessage( "", ID_DEFAULT_CHAT, u.id(), tr( "%1 %2 has refused to download %3." )
                           .arg( Bee::iconToHtml( ":/images/upload.png", "*F*" ), u.path(), fi.name() ), DispatchToAllChatsWithUser );
    return;
  }

  Connection* c = connection( u.id() );
  if( !c )
  {
    qWarning() << "Connection not found for user" << u.id() << "while parsing file message";
    return;
  }
  fi.setHostAddress( c->peerAddress() );

  dispatchSystemMessage( "", ID_DEFAULT_CHAT, u.id(), tr( "%1 %2 is sending to you the file: %3." )
                         .arg( Bee::iconToHtml( ":/images/download.png", "*F*" ), u.path(), fi.name() ), DispatchToAllChatsWithUser );
  emit fileDownloadRequest( u, fi );
}

void Core::parseChatMessage( const User& u, const Message& m )
{
  qDebug() << "Chat message received from user" << u.path();
  if( m.hasFlag( Message::Group ) )
    parseGroupChatMessage( u, m );
  if( m.hasFlag( Message::Private ) || m.flags() == 0 )
    dispatchChatMessageReceived( u.id(), m );
  else
    qWarning() << "Invalid flag found in chat message (CoreParser)";
}

void Core::parseGroupChatMessage( const User& u, const Message& m )
{
  qDebug() <<"Parsing group chat message";

  if( !m.hasFlag( Message::Request ) )
  {
    dispatchChatMessageReceived( u.id(), m );
    return;
  }

  ChatMessageData cmd = Protocol::instance().dataFromChatMessage( m );
  QStringList user_paths = Protocol::instance().userPathsFromGroupRequestMessage( m );
  UserList ul;
  QList<VNumber> users_id;
  foreach( QString user_path, user_paths )
  {
    User u = UserManager::instance().userList().find( user_path );
    if( u.isValid() )
    {
      ul.set( u );
      users_id.append( u.id() );
    }
    else
    {
      qWarning() << "User" << user_path << "not found in list";
      u = Protocol::instance().createTemporaryUser( user_path );
      if( u.isValid() )
      {
        qDebug() << "Connecting to user" << user_path;
        UserManager::instance().setUser( u );
        ul.set( u );
        newPeerFound( u.hostAddress(), u.hostPort() );
        users_id.append( u.id() );
      }
    }
  }

  if( ul.toList().size() < 2 )
  {
    qWarning() << "Unable to create group chat" << cmd.groupName() << "from user" << u.path();
    dispatchSystemMessage( "", ID_DEFAULT_CHAT, u.id(), tr( "%1 An error occurred when %2 tries to add you to the group: %3." )
                         .arg( Bee::iconToHtml( ":/images/chat-create.png", "*G*" ), u.path(), cmd.groupName() ), DispatchToAllChatsWithUser );
    return;
  }

  Chat group_chat = ChatManager::instance().groupChat( cmd.groupId() );
  if( !group_chat.isValid() )
  {
    dispatchSystemMessage( "", ID_DEFAULT_CHAT, u.id(), tr( "%1 %2 adds you to the group: %3." )
                         .arg( Bee::iconToHtml( ":/images/chat-create.png", "*G*" ), u.path(), cmd.groupName() ), DispatchToAllChatsWithUser );
    createGroupChat( cmd.groupName(), users_id, cmd.groupId(), false );
  }
  else
    changeGroupChat( group_chat.id(), cmd.groupName(), users_id, false );
}

void Core::parseFileShareMessage( const User& u, const Message& m )
{
  if( m.hasFlag( Message::List ) )
  {
     QList<FileInfo> file_info_list = Protocol::instance().messageToFileShare( m, u.hostAddress() );
     if( !file_info_list.isEmpty() )
     {
       qDebug() << "Received list of file shared from" << u.path();

       QString icon_html = Bee::iconToHtml( ":/images/download.png", "*F*" );
       QString msg;

       if( FileShare::instance().userHasFileShareList( u.id() ) )
         msg = tr( "%1 %2 has updated the list of the file shared." );
       else
         msg = tr( "%1 %2 has shared some files." );

       dispatchSystemMessage( "", ID_DEFAULT_CHAT, u.id(), msg.arg( icon_html, u.name() ), DispatchToAllChatsWithUser );

       FileShare::instance().addToNetwork( u.id(), file_info_list );

       emit fileShareAvailable( u );
     }
  }
  else if( m.hasFlag( Message::Request ) )
  {
    if( Settings::instance().fileShare() && !Protocol::instance().fileShareListMessage().isEmpty() )
      sendFileShareListTo( u.id() );
  }
  else
    qWarning() << "Invalid flag found in file share message (CoreParser)";
}
