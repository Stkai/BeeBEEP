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

#include "BeeUtils.h"
#include "Broadcaster.h"
#include "ChatManager.h"
#include "Connection.h"
#include "Core.h"
#include "FileInfo.h"
#include "FileShare.h"
#include "Hive.h"
#include "Protocol.h"
#include "Settings.h"
#include "UserManager.h"


void Core::parseMessage( VNumber user_id, const Message& m )
{
#ifdef BEEBEEP_DEBUG
  qDebug() << "Parsing message received from user" << user_id;
#endif
  User u = UserManager::instance().findUser( user_id );
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
  case Message::Read:
    parseChatReadMessage( u, m );
    break;
  case Message::Group:
    parseGroupMessage( u, m );
    break;
  case Message::File:
    parseFileMessage( u, m );
    break;
  case Message::Folder:
    parseFolderMessage( u, m );
    break;
  case Message::Share:
    parseFileShareMessage( u, m );
    break;
  case Message::Hive:
    parseHiveMessage( u, m );
    break;
  case Message::ShareBox:
    parseShareBoxMessage( u, m );
    break;
  case Message::ShareDesktop:
#ifdef BEEBEEP_USE_SHAREDESKTOP
    parseDesktopShareMessage( u, m );
#endif
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
#ifdef BEEBEEP_DEBUG
    qDebug() << "User" << u.path() << "is writing";
#endif
    emit userIsWriting( u );
    return;
  }
  else if( m.hasFlag( Message::UserStatus ) )
  {
    User user_with_new_status = u;
    if( Protocol::instance().changeUserStatusFromMessage( &user_with_new_status, m ) )
    {
#ifdef BEEBEEP_DEBUG
      qDebug() << "User" << user_with_new_status.path() << "changes status to" << user_with_new_status.status() << user_with_new_status.statusDescription();
#endif
      UserManager::instance().setUser( user_with_new_status );
      showUserStatusChanged( user_with_new_status );
    }
  }
  else if( m.hasFlag( Message::UserVCard ) )
  {
    User user_with_new_vcard = u;
    if( Protocol::instance().changeVCardFromMessage( &user_with_new_vcard, m ) )
    {
#ifdef BEEBEEP_DEBUG
      qDebug() << "User" << user_with_new_vcard.path() << "has new vCard";
#endif
      UserManager::instance().setUser( user_with_new_vcard );
      if( user_with_new_vcard.path() != u.path() )
        ChatManager::instance().changePrivateChatNameAfterUserNameChanged( u.id(), user_with_new_vcard.path() );
      showUserVCardChanged( user_with_new_vcard );
    }
    else
      qWarning() << "Unable to read vCard from" << qPrintable( u.path() );
  }
  else if( m.hasFlag( Message::UserName ) )
  {
    User user_with_new_name = u;
    if( Protocol::instance().changeUserNameFromMessage( &user_with_new_name, m ) )
    {
#ifdef BEEBEEP_DEBUG
      qDebug() << "User" << u.path() << "changes his name to" << user_with_new_name.name();
#endif
      UserManager::instance().setUser( user_with_new_name );
      ChatManager::instance().changePrivateChatNameAfterUserNameChanged( u.id(), user_with_new_name.path() );
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

  // check message first
  if( m.hasFlag( Message::Refused ) )
  {
    dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), tr( "%1 %2 has refused to download %3." )
                           .arg( Bee::iconToHtml( ":/images/upload.png", "*F*" ), u.name(), fi.name() ),
                           DispatchToDefaultAndPrivateChat, ChatMessage::FileTransfer );
    return;
  }

  // check file is valid
  if( !fi.isValid() )
  {
    qWarning() << "Invalid FileInfo received from user" << u.id() << ": [" << m.data() << "]:" << m.text();
    return;
  }

  if( !Settings::instance().fileTransferIsEnabled() )
  {
    refuseToDownloadFile( u.id(), fi );
    return;
  }

  if( fi.isInShareBox() && !Settings::instance().useShareBox() )
  {
    refuseToDownloadFile( u.id(), fi );
    return;
  }

  Connection* c = connection( u.id() );
  if( !c )
  {
    qWarning() << "Connection not found for user" << u.id() << "while parsing file message";
    return;
  }
  fi.setHostAddress( c->peerAddress() );

  QString sys_msg = tr( "%1 %2 is sending to you the file: %3." ).arg( Bee::iconToHtml( ":/images/download.png", "*F*" ), u.name(), fi.name() );

  dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), sys_msg, DispatchToAllChatsWithUser, ChatMessage::FileTransfer );

  if( fi.isInShareBox() )
  {
    QString to_path;
    if( !fi.shareFolder().isEmpty() )
      to_path = Bee::convertToNativeFolderSeparator( QString( "%1/%2/%3" ).arg( Settings::instance().shareBoxPath() )
                                                                          .arg( fi.shareFolder() )
                                                                          .arg( fi.name() ) );
    else
      to_path = Bee::convertToNativeFolderSeparator( QString( "%1/%2" ).arg( Settings::instance().shareBoxPath() )
                                                                       .arg( fi.name() ) );
    qDebug() << "ShareBox downloads from user" << u.path() << "the file" << fi.name() << "in path" << to_path;
    fi.setPath( to_path );

    mp_fileTransfer->downloadFile( fi );
  }
  else
    emit fileDownloadRequest( u, fi );
}

void Core::parseChatMessage( const User& u, const Message& m )
{
#ifdef BEEBEEP_DEBUG
  qDebug() << "Chat message received from user" << u.path();
#endif
  if( m.hasFlag( Message::Private ) || m.flags() == 0 || m.hasFlag( Message::GroupChat ) )
    dispatchChatMessageReceived( u.id(), m );
  else
    qWarning() << "Invalid flag found in chat message from" << qPrintable( u.path() );
}

void Core::parseGroupMessage( const User& u, const Message& m )
{
  ChatMessageData cmd = Protocol::instance().dataFromChatMessage( m );

#ifdef BEEBEEP_DEBUG
  qDebug() << "Message for group" << cmd.groupId() << cmd.groupName();
#endif

  if( m.hasFlag( Message::Request ) )
  {
    QStringList user_paths = Protocol::instance().userPathsFromGroupRequestMessage( m );
    UserList ul;
    ul.set( u ); // User from request
    ul.set( Settings::instance().localUser() );

    foreach( QString user_path, user_paths )
    {
      if( user_path.isEmpty() )
      {
#ifdef BEEBEEP_DEBUG
        qDebug() << "Invalid group message with empty user paths from" << qPrintable( u.path() );
#endif
        continue;
      }

      User user_tmp = UserManager::instance().findUserByPath( user_path );
      if( user_tmp.isValid() )
      {
        if( !user_tmp.isLocal() )
          ul.set( user_tmp );
      }
      else
      {
        qWarning() << "Creating temporary user" << user_path << "for group chat";
        user_tmp = Protocol::instance().createTemporaryUser( user_path, "" );
        if( user_tmp.isValid() )
        {
          qDebug() << "Connecting to the temporary user" << user_path;
          UserManager::instance().setUser( user_tmp );
          ul.set( user_tmp );
          newPeerFound( user_tmp.hostAddress(), user_tmp.hostPort() );
        }
      }
    }

    Chat group_chat = ChatManager::instance().findGroupChatByPrivateId( cmd.groupId() );

    if( !group_chat.isValid() )
    {
      if( ul.toList().size() < 2 )
      {
        qWarning() << "Unable to create group chat" << cmd.groupName() << "from user" << u.path();
        dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), tr( "%1 An error occurred when %2 tries to add you to the group chat: %3." )
                           .arg( Bee::iconToHtml( ":/images/chat-create.png", "*G*" ), u.name(), cmd.groupName() ),
                               DispatchToChat, ChatMessage::Other );
        return;
      }

      dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), tr( "%1 %2 adds you to the group chat: %3." )
                         .arg( Bee::iconToHtml( ":/images/chat-create.png", "*G*" ), u.name(), cmd.groupName() ),
                             DispatchToChat, ChatMessage::Other );
      createGroupChat( cmd.groupName(), ul.toUsersId(), cmd.groupId(), false );
    }
    else
    {
      if( group_chat.usersId().contains( ID_LOCAL_USER ) )
        changeGroupChat( group_chat.id(), cmd.groupName(), ul.toUsersId(), false );
      else
        sendGroupChatRefuseMessage( group_chat, ul );
    }
  }
  else if( m.hasFlag( Message::Refused ) )
  {
    Chat group_chat = ChatManager::instance().findGroupChatByPrivateId( cmd.groupId() );
    if( group_chat.isValid() )
      removeUserFromChat( u, group_chat.id() );
  }
  else
    qWarning() << "Invalid flag found in group message from" << qPrintable( u.path() );
}

void Core::parseFileShareMessage( const User& u, const Message& m )
{
  if( m.hasFlag( Message::List ) )
  {
    QString icon_html = Bee::iconToHtml( ":/images/download.png", "*F*" );
    QString msg;

    QList<FileInfo> file_info_list = Protocol::instance().messageToFileShare( m, u.hostAddress() );

    if( file_info_list.isEmpty() )
    {
      msg = tr( "%1 %2 has not shared files." ).arg( icon_html ).arg( u.name() );
      qDebug() << qPrintable( u.path() ) << "has not shared files";
    }
    else
    {
      msg = tr( "%1 %2 has shared %3 files." ).arg( icon_html ).arg( u.name() ).arg( file_info_list.size() );
      qDebug() << qPrintable( u.path() ) << "has shared" << file_info_list.size() << "files";
    }

    dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), msg, DispatchToDefaultAndPrivateChat, ChatMessage::FileTransfer );

    FileShare::instance().addToNetwork( u.id(), file_info_list );

    emit fileShareAvailable( u );
  }
  else if( m.hasFlag( Message::Request ) )
  {
    if( !Settings::instance().fileTransferIsEnabled() )
    {
      qDebug() << "File transfer is disabled. Ignoring request from user" << qPrintable( u.path() );
      return;
    }

    if( !Protocol::instance().fileShareListMessage().isEmpty() )
      sendFileShareListTo( u.id() );
  }
  else
    qWarning() << "Invalid flag found in file share message from" << qPrintable( u.path() );
}

void Core::parseFolderMessage( const User& u, const Message& m )
{
  if( m.hasFlag( Message::Refused ) )
  {
    dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), tr( "%1 %2 has refused to download folder %3." )
                             .arg( Bee::iconToHtml( ":/images/upload.png", "*F*" ), u.name(), m.text() ),
                             DispatchToDefaultAndPrivateChat, ChatMessage::FileTransfer );
    return;
  }
  else if( m.hasFlag( Message::Request ) )
  {
    QString folder_name = tr( "unknown folder" );
    QList<FileInfo> file_info_list = Protocol::instance().messageFolderToInfoList( m, u.hostAddress(), &folder_name );
    if( file_info_list.isEmpty() )
    {
      qWarning() << "Invalid file info list found in folder message from" << qPrintable( u.path() );
      return;
    }

    QString sys_msg = tr( "%1 %2 is sending to you the folder: %3." ).arg( Bee::iconToHtml( ":/images/download.png", "*F*" ), u.name(), folder_name );

    dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), sys_msg, DispatchToAllChatsWithUser, ChatMessage::FileTransfer );

    emit folderDownloadRequest( u, folder_name, file_info_list );
  }
  else
    qWarning() << "Invalid flag found in folder message from user" << qPrintable( u.path() );
}

void Core::parseChatReadMessage( const User& u, const Message& m )
{
#ifdef BEEBEEP_DEBUG
  qDebug() << "Message" << m.id() << "read from user" << qPrintable( u.path() );
#endif
  dispatchChatMessageReadReceived( u.id(), m );
}

void Core::parseHiveMessage( const User& u, const Message& m )
{
  if( !Settings::instance().useHive() )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Skips hive message arrived from" << qPrintable( u.path() ) << "(option disabled)";
#endif
    return;
  }

  if( m.hasFlag( Message::List ) )
  {
    QList<UserRecord> user_record_list = Protocol::instance().messageToUserRecordList( m );
    if( user_record_list.isEmpty() )
      return;
#ifdef BEEBEEP_DEBUG
    qDebug() << "Hive message arrived with" << user_record_list.size() << "users from" << qPrintable( u.path() );
#endif
    foreach( UserRecord ur, user_record_list )
    {
       if( Hive::instance().addNetworkAddress( NetworkAddress( ur.hostAddress(), ur.hostPort() ) ) )
         mp_broadcaster->setNewBroadcastRequested( true );
    }
  }
  else
    qWarning() << "Invalid flag found in hive message from user" << qPrintable( u.path() );
}

void Core::parseShareBoxMessage( const User& u, const Message& m )
{
  if( !Settings::instance().fileTransferIsEnabled() )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Skips share box message arrived from" << qPrintable( u.path() ) << "(option disabled)";
#endif
    return;
  }

  QString folder_name = Protocol::instance().folderNameFromShareBoxMessage( m );

  if( m.hasFlag( Message::List ) )
  {
    QList<FileInfo> file_info_list = Protocol::instance().messageToShareBoxFileList( m, u.hostAddress() );
    emit shareBoxAvailable( u, folder_name, file_info_list );
  }
  else if( m.hasFlag( Message::Request ) )
  {
    if( m.hasFlag( Message::Refused ) )
    {
      qDebug() << "User" << qPrintable( u.path() ) << "has refused to share info of folder:" << qPrintable( folder_name );
      emit shareBoxUnavailable( u, folder_name );
    }
    else
    {
      if( Settings::instance().useShareBox() )
      {
        qDebug() << "User" << qPrintable( u.path() ) << "has requested the list of folder:" << qPrintable( folder_name );
        buildShareBoxFileList( u, folder_name );
      }
      else
        sendMessageToLocalNetwork( u, Protocol::instance().refuseToShareBoxPath( folder_name ) );
    }
  }
  else
    qWarning() << "Invalid flag found in share box message from user" << qPrintable( u.path() );
}

#ifdef BEEBEEP_USE_SHAREDESKTOP
void Core::parseDesktopShareMessage( const User& u, const Message& m )
{
  if( m.hasFlag( Message::Request ) && m.hasFlag( Message::Refused ) )
  {
    qDebug() << "User" << qPrintable( u.path() ) << "has refused to view your shared desktop";
    return;
  }
  else if( m.hasFlag( Message::Private ) )
  {
    QPixmap pix = Protocol::instance().pixmapFromShareDesktopMessage( m );
    if( !pix.isNull() )
      emit shareDesktopImageAvailable( u, pix );
  }
  else
  {
    qWarning() << "Invalid flag found in desktop share message from user" << qPrintable( u.path() );
  }
}
#endif
