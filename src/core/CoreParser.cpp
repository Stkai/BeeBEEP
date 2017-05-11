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
  case Message::Buzz:
    parseBuzzMessage( u, m );
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
    qDebug() << "User" << qPrintable( u.path() ) << "is writing";
#endif
    Chat c = ChatManager::instance().findChatByPrivateId( m.data(), false, u.id() );
    if( c.hasUser( u.id() ) )
      emit userIsWriting( u, c.id() );
    return;
  }
  else if( m.hasFlag( Message::UserStatus ) )
  {
    User user_with_new_status = u;
    if( Protocol::instance().changeUserStatusFromMessage( &user_with_new_status, m ) )
    {
#ifdef BEEBEEP_DEBUG
      qDebug() << "User" << qPrintable( user_with_new_status.path() ) << "changes status to" << user_with_new_status.status() << user_with_new_status.statusDescription();
#endif
      UserManager::instance().setUser( user_with_new_status );
      emit userChanged( user_with_new_status );
    }
  }
  else if( m.hasFlag( Message::UserVCard ) )
  {
    User user_with_new_vcard = u;

    if( Protocol::instance().changeVCardFromMessage( &user_with_new_vcard, m ) )
    {
      bool color_changed = user_with_new_vcard.color() != u.color();
      bool vcard_changed = !(user_with_new_vcard.vCard() == u.vCard() );

      if( vcard_changed || color_changed )
      {
        UserManager::instance().setUser( user_with_new_vcard );
        if( user_with_new_vcard.path() != u.path() )
          ChatManager::instance().changePrivateChatNameAfterUserNameChanged( u.id(), user_with_new_vcard.path() );
        if( vcard_changed )
          showUserVCardChanged( user_with_new_vcard );

        emit userChanged( user_with_new_vcard );
      }
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
      emit userChanged( u );
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

  Chat chat_to_show_message = ChatManager::instance().findChatByPrivateId( fi.chatPrivateId(), false, u.id() );

  if( m.hasFlag( Message::Refused ) )
  {
    dispatchSystemMessage( chat_to_show_message.id(), u.id(), tr( "%1 %2 has refused to download %3." )
                           .arg( Bee::iconToHtml( ":/images/upload.png", "*F*" ), u.name(), fi.name() ),
                           chat_to_show_message.isValid() ? DispatchToChat : DispatchToAllChatsWithUser, ChatMessage::FileTransfer );
    return;
  }

  if( !Settings::instance().enableFileTransfer() )
  {
    refuseToDownloadFile( u.id(), fi );
    return;
  }

  if( fi.isInShareBox() )
  {
    if( !Settings::instance().useShareBox() || Settings::instance().disableFileSharing() )
    {
      refuseToDownloadFile( u.id(), fi );
      return;
    }
  }

  fi.setHostAddress( u.networkAddress().hostAddress() );

  QString sys_msg = tr( "%1 %2 is sending to you the file: %3." ).arg( Bee::iconToHtml( ":/images/download.png", "*F*" ), u.name(), fi.name() );
  dispatchSystemMessage( chat_to_show_message.id(), u.id(), sys_msg, chat_to_show_message.isValid() ? DispatchToChat : DispatchToAllChatsWithUser, ChatMessage::FileTransfer );

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
  if( m.hasFlag( Message::Private ) || m.flags() == 0 || m.hasFlag( Message::GroupChat ) )
    dispatchChatMessageReceived( u.id(), m );
  else
    qWarning() << "Invalid flag found in chat message from" << qPrintable( u.path() );
}

void Core::parseGroupMessage( const User& u, const Message& m )
{
  ChatMessageData cmd = Protocol::instance().dataFromChatMessage( m );

  if( m.hasFlag( Message::Request ) )
  {
    if( ChatManager::instance().isChatRefused( cmd.groupId() ) )
    {
      Message group_refuse_message = Protocol::instance().groupChatRefuseMessage( cmd );
      sendMessageToLocalNetwork( u, group_refuse_message );
      return;
    }

    UserList ul;
    ul.set( u ); // User from request
    ul.set( Settings::instance().localUser() );

    if( u.protocolVersion() < NEW_GROUP_PROTO_VERSION )
    {
      QStringList user_paths = Protocol::instance().userPathsFromGroupRequestMessage_obsolete( m );
      if( user_paths.isEmpty() )
      {
        qWarning() << "Invalid group message request from" << qPrintable( u.path() ) << "for chat" << qPrintable( cmd.groupName() );
        return;
      }

      foreach( QString user_path, user_paths )
      {
        if( user_path.isEmpty() )
        {
          qWarning() << "Invalid group message with empty user path from" << qPrintable( u.path() ) << "for chat" << qPrintable( cmd.groupName() );
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
          UserRecord ur( User::nameFromPath( user_path ), "", "" );
          ur.setNetworkAddress( NetworkAddress::fromString( User::hostAddressAndPortFromPath( user_path ) ) );
          if( !ur.networkAddressIsValid() )
            qWarning() << "Invalid network address found in user" << qPrintable( user_path ) << "from group request of" << qPrintable( u.name() );
          user_tmp = Protocol::instance().createTemporaryUser( ur );
          qDebug() << "Temporary user" << user_tmp.path() << "created by group request of" << qPrintable( u.name() );
          UserManager::instance().setUser( user_tmp );
          ul.set( user_tmp );
        }
      }
    }
    else
    {
      QList<UserRecord> user_records = Protocol::instance().userRecordsFromGroupRequestMessage( m );
      if( user_records.isEmpty() )
      {
        qWarning() << "Invalid group message request with empty user records from" << qPrintable( u.path() ) << "for chat" << qPrintable( cmd.groupName() );;
        return;
      }

      foreach( UserRecord ur, user_records )
      {
        User group_member = UserManager::instance().findUserByUserRecord( ur );
        if( group_member.isLocal() )
          continue;

        if( !group_member.isValid() )
        {
          group_member = Protocol::instance().createTemporaryUser( ur );
          UserManager::instance().setUser( group_member );
          qDebug() << "Temporary user" << group_member.path() << "created by group request of" << u.name();
          emit userChanged( group_member );
        }

        ul.set( group_member );
      }
    }

    Chat group_chat = ChatManager::instance().findChatByPrivateId( cmd.groupId(), true, ID_INVALID );

    if( group_chat.isValid() )
    {
      if( group_chat.usersId().contains( ID_LOCAL_USER ) )
      {
        changeGroupChat( u, group_chat.id(), cmd.groupName(), ul.toUsersId() );
      }
      else
      {
        sendGroupChatRefuseMessage( group_chat, ul );
        ChatManager::instance().addToRefusedChat( ChatRecord( group_chat.name(), group_chat.privateId() ) );
      }
    }
    else
    {
      if( ul.toList().size() < 2 )
      {
        qWarning() << "Unable to create group chat" << cmd.groupName() << "from user" << u.path();
        dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(),
                               tr( "%1 An error occurred when %2 tries to add you to the group chat: %3." )
                                 .arg( Bee::iconToHtml( ":/images/chat-create.png", "*G*" ), u.name(), cmd.groupName() ),
                               DispatchToChat, ChatMessage::System );
        return;
      }
      else
      {
        QString sys_msg = tr( "%1 %2 has added you to the group chat: %3." ).arg( Bee::iconToHtml( ":/images/chat-create.png", "*G*" ), u.name(), cmd.groupName() );
        dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), sys_msg, DispatchToChat, ChatMessage::System );
        group_chat = createGroupChat( cmd.groupName(), ul.toUsersId(), cmd.groupId(), false );
        dispatchSystemMessage( group_chat.id(), u.id(), sys_msg, DispatchToChat, ChatMessage::System );
      }
    }
  }
  else if( m.hasFlag( Message::Refused ) )
  {
    removeUserFromGroup( u, cmd.groupId() );
    removeUserFromChat( u, cmd.groupId() );
    dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), tr( "%1 %2 has left the group: %3." )
                                                      .arg( Bee::iconToHtml( ":/images/group-remove.png", "*G*" ), u.name(), cmd.groupName() ),
                           DispatchToChat, ChatMessage::System );
  }
  else
    qWarning() << "Invalid flag found in group message from" << qPrintable( u.path() );
}

void Core::parseFileShareMessage( const User& u, const Message& m )
{
  if( m.hasFlag( Message::List ) )
  {
    QList<FileInfo> file_info_list = Protocol::instance().messageToFileShare( m, u.networkAddress().hostAddress() );
    int prev_files = FileShare::instance().network().count( u.id() );
    int new_files = FileShare::instance().addToNetwork( u.id(), file_info_list );

    QString share_status;
    if( prev_files > 0 && new_files == 0 )
      share_status = tr( "%1 has removed shared files" ).arg( u.name() );
    else if( new_files > 0 )
      share_status = tr( "%1 has shared %2 files" ).arg( u.name() ).arg( new_files );
    else
      share_status = "";

    if( !share_status.isEmpty() )
      dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER, QString( "%1 %2." ).arg( Bee::iconToHtml( ":/images/download.png", "*F*" ), share_status ),
                             DispatchToChat, ChatMessage::FileTransfer );

    emit fileShareAvailable( u );
  }
  else if( m.hasFlag( Message::Request ) )
  {
    if( !Settings::instance().enableFileTransfer() )
      return;

    if( !Settings::instance().enableFileSharing() )
      return;

    sendFileShareListTo( u.id() );
  }
  else
    qWarning() << "Invalid flag found in file share message from" << qPrintable( u.path() );
}

void Core::parseFolderMessage( const User& u, const Message& m )
{
  Chat chat_to_show_message;
  if( m.hasFlag( Message::Refused ) )
  {
    chat_to_show_message = ChatManager::instance().findChatByPrivateId( m.data(), false, u.id() );
    dispatchSystemMessage( chat_to_show_message.id(), u.id(), tr( "%1 %2 has refused to download folder %3." )
                             .arg( Bee::iconToHtml( ":/images/upload.png", "*F*" ), u.name(), m.text() ),
                             chat_to_show_message.isValid() ? DispatchToChat : DispatchToDefaultAndPrivateChat, ChatMessage::FileTransfer );
    return;
  }
  else if( m.hasFlag( Message::Request ) )
  {
    if( !Settings::instance().enableFileTransfer() )
      return;

    QString folder_name = tr( "unknown folder" );
    QList<FileInfo> file_info_list = Protocol::instance().messageFolderToInfoList( m, u.networkAddress().hostAddress(), &folder_name );
    if( file_info_list.isEmpty() )
    {
      qWarning() << "Invalid file info list found in folder message from" << qPrintable( u.path() );
      return;
    }

    chat_to_show_message = ChatManager::instance().findChatByPrivateId( file_info_list.first().chatPrivateId(), false, u.id() );
    QString sys_msg = tr( "%1 %2 is sending to you the folder: %3." ).arg( Bee::iconToHtml( ":/images/download.png", "*F*" ), u.name(), folder_name );

    dispatchSystemMessage( chat_to_show_message.id(), u.id(), sys_msg, chat_to_show_message.isValid() ? DispatchToChat : DispatchToAllChatsWithUser, ChatMessage::FileTransfer );

    emit folderDownloadRequest( u, folder_name, file_info_list );
  }
  else
    qWarning() << "Invalid flag found in folder message from user" << qPrintable( u.path() );
}

void Core::parseChatReadMessage( const User& u, const Message& m )
{
  Chat c = findChatFromMessageData( u.id(), m );
  if( !c.isValid() )
  {
    qWarning() << "Invalid chat message read received from" << qPrintable( u.path() );
    return;
  }

#ifdef BEEBEEP_DEBUG
  qDebug() << "User" << qPrintable( u.path() ) << "has read messages in chat" << c.name();
#endif

  c.setReadMessagesByUser( u.id() );
  ChatManager::instance().setChat( c );
  emit chatReadByUser( c, u );
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
    QList<UserRecord> user_record_list = Protocol::instance().hiveMessageToUserRecordList( m );
    if( user_record_list.isEmpty() )
      return;
#ifdef BEEBEEP_DEBUG
    qDebug() << "Hive message arrived with" << user_record_list.size() << "users from" << qPrintable( u.path() );
#endif
    foreach( UserRecord ur, user_record_list )
    {
      if( Hive::instance().addNetworkAddress( ur.networkAddress() ) && !hasConnection( ur.networkAddress().hostAddress(), ur.networkAddress().hostPort() ) )
      {
#ifdef BEEBEEP_DEBUG
        qDebug() << "Hive message contains this path" << qPrintable( ur.networkAddress().toString() ) << "and it is added to contact list";
#endif
        mp_broadcaster->setNewBroadcastRequested( true );
      }
    }
  }
  else
    qWarning() << "Invalid flag found in hive message from user" << qPrintable( u.path() );
}

void Core::parseShareBoxMessage( const User& u, const Message& m )
{
  if( !Settings::instance().enableFileTransfer() )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Skips share box message arrived from" << qPrintable( u.path() ) << "(file transfer disabled)";
#endif
    return;
  }

  if( !Settings::instance().enableFileSharing() )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Skips share box message arrived from" << qPrintable( u.path() ) << "(file sharing disabled)";
#endif
    return;
  }

  QString folder_name = Protocol::instance().folderNameFromShareBoxMessage( m );

  if( m.hasFlag( Message::List ) )
  {
    QList<FileInfo> file_info_list = Protocol::instance().messageToShareBoxFileList( m, u.networkAddress().hostAddress() );
    emit shareBoxAvailable( u, folder_name, file_info_list );
  }
  else if( m.hasFlag( Message::Request ) )
  {
    if( !m.hasFlag( Message::Refused ) )
    {
      if( Settings::instance().useShareBox() )
        buildShareBoxFileList( u, folder_name );
      else
        sendMessageToLocalNetwork( u, Protocol::instance().refuseToShareBoxPath( folder_name ) );
    }
    else
      emit shareBoxUnavailable( u, folder_name );
  }
  else
    qWarning() << "Invalid flag found in share box message from user" << qPrintable( u.path() );
}

void Core::parseBuzzMessage( const User& u, const Message& )
{
  QString sys_msg = tr( "%1 %2 is buzzing you." ).arg( Bee::iconToHtml( ":/images/bell.png", "*Z*" ), u.name() );
  Chat c = ChatManager::instance().privateChatForUser( u.id() );
  if( !c.isValid() )
    c = ChatManager::instance().defaultChat();
  c.addUnreadMessage();
  ChatManager::instance().setChat( c );
  dispatchSystemMessage( c.id(), u.id(), sys_msg, DispatchToChat, ChatMessage::Other );
  emit localUserIsBuzzedBy( u );
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
