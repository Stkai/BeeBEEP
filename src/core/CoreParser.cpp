//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2021 Marco Mastroddi
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
// along with BeeBEEP. If not, see <http://www.gnu.org/licenses/>.
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
#include "IconManager.h"
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
    parseShareDesktopMessage( u, m );
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
      VCard vc_tmp = user_with_new_vcard.vCard();
      vc_tmp.setNickName( u.vCard().nickName() );
      bool only_user_name_changed = vc_tmp == u.vCard();

      if( vcard_changed || color_changed )
      {
        UserManager::instance().setUser( user_with_new_vcard );
        if( user_with_new_vcard.name() != u.name() )
        {
          ChatManager::instance().changePrivateChatNameAfterUserNameChanged( u.id(), user_with_new_vcard.name() );
          showUserNameChanged( user_with_new_vcard, u.name() );
        }
        if( vcard_changed && !only_user_name_changed )
          showUserVCardChanged( user_with_new_vcard, u.vCard() );
        emit userChanged( user_with_new_vcard );
      }
    }
    else
      qWarning() << "Unable to read vCard from" << qPrintable( u.path() );
  }
  else
    qWarning() << "Invalid flag" << m.flags() << "found in user message from" << qPrintable( u.name() );
}

void Core::parseFileMessage( const User& u, const Message& m )
{
  FileInfo fi = Protocol::instance().fileInfoFromMessage( m, u.protocolVersion() );
  if( !fi.isValid() )
  {
    qWarning() << "Invalid FileInfo received from user" << u.id() << ": [" << qPrintable( m.data() ) << "]:" << qPrintable( m.text() );
    return;
  }

  Chat chat_to_show_message = ChatManager::instance().findChatByPrivateId( fi.chatPrivateId(), false, u.id() );

  if( m.hasFlag( Message::Refused ) )
  {
    dispatchSystemMessage( chat_to_show_message.id(), u.id(), tr( "%1 %2 has refused to download the file: %3" )
                           .arg( IconManager::instance().toHtml( "red-ball.png", "*F*" ), Bee::userNameToShow( u, true ), fi.name() ),
                           chat_to_show_message.isValid() ? DispatchToChat : DispatchToAllChatsWithUser, ChatMessage::FileTransfer, false );
    return;
  }

  if( !Settings::instance().enableFileTransfer() )
  {
    refuseToDownloadFile( u.id(), fi );
    return;
  }

  if( !fi.name().endsWith( fi.suffix(), Qt::CaseInsensitive ) )
  {
    qWarning() << "Invalid file transfer request received from user" << qPrintable( u.path() ) << ": file extension is" << fi.suffix() << "but the file name is" << fi.name();
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

  QString sys_msg;
  if( m.hasFlag( Message::VoiceMessage ) )
    sys_msg = tr( "%1 %2 is sending to you the voice message: %3" ).arg( IconManager::instance().toHtml( "download.png", "*F*" ), Bee::userNameToShow( u, true ), fi.name() );
  else
    sys_msg = tr( "%1 %2 is sending to you the file: %3" ).arg( IconManager::instance().toHtml( "download.png", "*F*" ), Bee::userNameToShow( u, true ), fi.name() );
  dispatchSystemMessage( chat_to_show_message.id(), u.id(), sys_msg, chat_to_show_message.isValid() ? DispatchToChat : DispatchToAllChatsWithUser, ChatMessage::FileTransfer, false );

  if( !Settings::instance().isFileExtensionAllowedInFileTransfer( fi.suffix() ) )
  {
    qWarning() << "User" << qPrintable( u.path() ) << "is sending to you the file" << fi.name() << "but you are not allowed to download this file extension";
    refuseToDownloadFile( u.id(), fi );
    return;
  }

  fi.setHostAddress( u.networkAddress().hostAddress() );
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

    QFileInfo file_info( to_path );
    if( file_info.exists() && file_info.isDir() && Settings::instance().onExistingFileAction() == Settings::GenerateNewFileName )
      to_path = Bee::uniqueFilePath( to_path, false );

    qDebug() << "BeeBOX downloads from user" << qPrintable( u.path() ) << "the file" << qPrintable( fi.name() ) << "in path" << qPrintable( to_path );
    fi.setPath( to_path );

    mp_fileTransfer->downloadFile( u.id(), fi );
  }
  else if( fi.isVoiceMessage() )
  {
    QString file_path = Bee::uniqueFilePath( QString( "%1/%2" ).arg( Settings::instance().cacheFolder(), fi.name() ), false );
    fi.setPath( file_path );
    mp_fileTransfer->downloadFile( u.id(), fi );
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
 #ifdef BEEBEEP_DEBUG
    qDebug() << "Group chat request from" << qPrintable( u.path() );
 #endif
    if( ChatManager::instance().isChatRefused( cmd.groupId() ) )
    {
      qWarning() << "Group chat request from" << qPrintable( u.name() ) << "is refused because the chat"
                 << qPrintable( cmd.groupName() ) << "is blocked on id" << qPrintable( cmd.groupId() );
      sendMessageToLocalNetwork( u, Protocol::instance().groupChatRefuseMessage( cmd ) );
      return;
    }

    Chat group_chat = ChatManager::instance().findChatByPrivateId( cmd.groupId(), true, ID_INVALID );
    if( group_chat.isValid() )
    {
      if( cmd.groupLastModified().isValid() && group_chat.lastModified().isValid() )
      {
        if( cmd.groupLastModified() <= group_chat.lastModified() )
        {
#ifdef BEEBEEP_DEBUG
          qDebug() << "Group chat request for" << qPrintable( cmd.groupName() ) << "from" << qPrintable( u.name() ) << "is skipped because it has old timestamp:" << qPrintable( cmd.groupLastModified().toString( Qt::ISODate ) );
#endif
          return;
        }
#ifdef BEEBEEP_DEBUG
        else
          qDebug() << "Group chat request from" << qPrintable( u.path() ) << "is newer" << qPrintable( cmd.groupLastModified().toString( Qt::ISODate ) )
                   << "than your" << qPrintable( group_chat.lastModified().toString( Qt::ISODate ) ) ;
#endif
      }
      else if( !cmd.groupLastModified().isValid() )
      {
        if( group_chat.lastModified().isValid() )
        {
#ifdef BEEBEEP_DEBUG
          qDebug() << "Group chat request for" << qPrintable( cmd.groupName() ) << "from" << qPrintable( u.name() ) << "is skipped because it has not last modified date";
#endif
          return;
        }
#ifdef BEEBEEP_DEBUG
        else
          qDebug() << "Group chat request from" << qPrintable( u.path() ) << "has not last modified date as your chat";
#endif
      }
#ifdef BEEBEEP_DEBUG
      else
        qDebug() << "Group chat request from" << qPrintable( u.path() ) << "has last modified date" << qPrintable( cmd.groupLastModified().toString( Qt::ISODate ) );
#endif

      if( !group_chat.usersId().contains( ID_LOCAL_USER ) )
      {
        ChatManager::instance().addToRefusedChat( ChatRecord( group_chat.name(), group_chat.privateId() ) );
        qWarning() << "Group chat request from" << qPrintable( u.name() ) << "is refused because you have left the chat" << qPrintable( group_chat.name() );
        sendMessageToLocalNetwork( u, Protocol::instance().groupChatRefuseMessage( cmd ) );
        return;
      }
    }

    UserList ul;
    ul.set( u ); // User from request
    ul.set( Settings::instance().localUser() );

    if( u.protocolVersion() < NEW_GROUP_PROTO_VERSION )
    {
      QStringList user_paths = Protocol::instance().userPathsFromGroupRequestMessage_obsolete( m );
      if( user_paths.isEmpty() )
      {
        qWarning() << "(obsolete) Group chat request for" << qPrintable( cmd.groupName() ) << "from" << qPrintable( u.name() ) << "has not members";
        return;
      }
      else
        qDebug() << "(obsolete) Group chat request for" << qPrintable( cmd.groupName() ) << "from" << qPrintable( u.name() ) << "has" << user_paths.size() << "members";

      foreach( QString user_path, user_paths )
      {
        if( user_path.isEmpty() )
        {
          qWarning() << "(obsolete) Group chat request for" << qPrintable( cmd.groupName() ) << "from" << qPrintable( u.name() ) << "has a NULL member";
          continue;
        }

        User user_tmp = UserManager::instance().findUserByNickname( User::nameFromPath( user_path ) );
        if( user_tmp.isValid() )
        {
          if( !user_tmp.isLocal() )
          {
#ifdef BEEBEEP_DEBUG
            qDebug() << "(obsolete) Group chat request for" << qPrintable( cmd.groupName() ) << "from" << qPrintable( u.name() ) << "has found member" << qPrintable( user_tmp.name() );
#endif
            ul.set( user_tmp );
          }
        }
        else
        {
          UserRecord ur( User::nameFromPath( user_path ), "", "", "" );
          ur.setNetworkAddress( NetworkAddress::fromString( User::hostAddressAndPortFromPath( user_path ) ) );
          if( !ur.networkAddressIsValid() )
            qWarning() << "(obsolete) Invalid network address found in user" << qPrintable( user_path ) << "from group request of" << qPrintable( u.name() );
          user_tmp = Protocol::instance().createTemporaryUser( ur );
          qDebug() << "(obsolete) Group chat request for" << qPrintable( cmd.groupName() ) << "from" << qPrintable( u.name() ) << "has created temporary user" << user_tmp.id() << qPrintable( user_tmp.name() );
          UserManager::instance().setUser( user_tmp );
          createPrivateChat( user_tmp );
          emit userChanged( user_tmp );
          ul.set( user_tmp );
        }
      }
    }
    else
    {
      QList<UserRecord> user_records = Protocol::instance().userRecordsFromGroupRequestMessage( m );
      if( !user_records.isEmpty() )
      {
        qDebug() << "Group chat request for" << qPrintable( cmd.groupName() ) << "from" << qPrintable( u.name() ) << "has" << user_records.size() << "members";
        foreach( UserRecord ur, user_records )
        {
          User group_member = Protocol::instance().recognizeUser( ur, Settings::instance().userRecognitionMethod() );
          if( group_member.isLocal() )
            continue;

          if( !group_member.isValid() )
          {
            group_member = Protocol::instance().createTemporaryUser( ur );
            UserManager::instance().setUser( group_member );
            qDebug() << "Temporary user" << group_member.id() << qPrintable( group_member.name() ) << "is created after request of" << qPrintable( u.name() ) << "for group" << qPrintable( cmd.groupName() );
            createPrivateChat( group_member );
            emit userChanged( group_member );
          }

          ul.set( group_member );
        }
      }
      else
        qDebug() << "Group chat request for" << qPrintable( cmd.groupName() ) << "from" << qPrintable( u.name() ) << "makes this group private";
    }

    if( group_chat.isValid() )
    {
      qDebug() << "Group chat request for" << qPrintable( cmd.groupName() ) << "from" << qPrintable( u.name() ) << "is accepted with timestamp" << qPrintable( cmd.groupLastModified().toString( Qt::ISODate ) );
      Group g = group_chat.group();
      g.setName( cmd.groupName() );
      g.setUsers( ul.toUsersId() );
      if( cmd.groupLastModified().isValid() )
        g.setLastModified( cmd.groupLastModified() );
      changeGroupChat( u, g );
    }
    else
    {
      if( ul.toList().size() > 1 )
      {
        Group g;
        g.setName( cmd.groupName() );
        g.setPrivateId( cmd.groupId() );
        g.setUsers( ul.toUsersId() );
        g.setChatType( Group::GroupChat );
        if( cmd.groupLastModified().isValid() )
          g.setLastModified( cmd.groupLastModified() );

        createGroupChat( u, g, false );
      }
      else
        qWarning() << "Unable to create group chat" << qPrintable( cmd.groupName() ) << "from user" << qPrintable( u.name() ) << "because members are minus than 2";
    }
  }
  else if( m.hasFlag( Message::Refused ) )
  {
    removeUserFromGroupChat( u, cmd.groupId() );
  }
  else if( m.hasFlag( Message::Delete ) )
  {
    removeLocalUserFromGroupChatByOther( u, cmd.groupId() );
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
      share_status = tr( "%1 has removed shared files" ).arg( Bee::userNameToShow( u, true ) );
    else if( new_files > 0 )
      share_status = tr( "%1 has shared %2 files" ).arg( Bee::userNameToShow( u, true ) ).arg( new_files );
    else
      share_status = "";

    if( !share_status.isEmpty() )
      dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER, QString( "%1 %2." ).arg( IconManager::instance().toHtml( "download.png", "*F*" ), share_status ),
                             DispatchToChat, ChatMessage::System, false );

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
                             .arg( IconManager::instance().toHtml( "red-ball.png", "*F*" ), Bee::userNameToShow( u, true ), m.text() ),
                             chat_to_show_message.isValid() ? DispatchToChat : DispatchToDefaultAndPrivateChat, ChatMessage::FileTransfer, false );
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

    QString chat_private_id = file_info_list.first().chatPrivateId();
    chat_to_show_message = ChatManager::instance().findChatByPrivateId( chat_private_id, false, u.id() );
    QString sys_msg = tr( "%1 %2 is sending to you the folder: %3." ).arg( IconManager::instance().toHtml( "download.png", "*F*" ), Bee::userNameToShow( u, true ), folder_name );
    dispatchSystemMessage( chat_to_show_message.id(), u.id(), sys_msg, chat_to_show_message.isValid() ? DispatchToChat : DispatchToAllChatsWithUser, ChatMessage::FileTransfer, false );

    QList<FileInfo> file_info_list_allowed;
    foreach( FileInfo fi, file_info_list )
    {
      if( !Settings::instance().isFileExtensionAllowedInFileTransfer( fi.suffix() ) )
      {
        qWarning() << "User" << qPrintable( u.path() ) << "is sending to you the file" << fi.name() << "in folder" << folder_name << "but you are not allowed to download this file extension";
        refuseToDownloadFile( u.id(), fi );
      }
      else
        file_info_list_allowed.append( fi );
    }

    if( file_info_list_allowed.isEmpty() )
    {
      qWarning() << "User" << qPrintable( u.path() ) << "sent you the folder" << folder_name << "but all the files contained are not allowed";
      refuseToDownloadFolder( u.id(), folder_name, chat_private_id );
    }
    else
      emit folderDownloadRequest( u, folder_name, file_info_list_allowed );
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
    qDebug() << "Hive message arrived with" << user_record_list.size() << "user(s) from" << qPrintable( u.path() );
#endif
    int hive_users_added = 0;
    foreach( UserRecord ur, user_record_list )
    {
      if( !isUserConnected( ur.networkAddress() ) )
      {
        if( Hive::instance().addNetworkAddress( ur.networkAddress() ) )
        {
          hive_users_added++;
          qDebug() << "New user added from Hive with address" << qPrintable( ur.networkAddress().toString() );
        }
      }
    }
    if( hive_users_added > 0 )
      mp_broadcaster->updateUsersFromHive();
  }
  else
    qWarning() << "Invalid flag found in hive message from user" << qPrintable( u.path() );
}

void Core::parseShareBoxMessage( const User& u, const Message& m )
{
  QString folder_name = Protocol::instance().folderNameFromShareBoxMessage( m );

#ifdef BEEBEEP_DEBUG
  if( !Settings::instance().enableFileTransfer() )
    qDebug() << "Skips share box message arrived from" << qPrintable( u.path() ) << "(file transfer disabled)";
  else if( !Settings::instance().enableFileSharing() )
    qDebug() << "Skips share box message arrived from" << qPrintable( u.path() ) << "(file sharing disabled)";
  else if( !Settings::instance().useShareBox() )
    qDebug() << "Skips share box message arrived from" << qPrintable( u.path() ) << "(share box disabled)";
  else
    qDebug() << "Parsing share box message from" << qPrintable( u.path() ) << "for folder" << folder_name;
#endif

  bool sharebox_is_active = Settings::instance().enableFileTransfer() && Settings::instance().enableFileSharing() && Settings::instance().useShareBox();

  if( !sharebox_is_active )
  {
    if( !m.hasFlag( Message::Refused ) )
      sendMessageToLocalNetwork( u, Protocol::instance().refuseToShareBoxPath( folder_name, m.hasFlag( Message::Create ) ) );
  }
  else if( m.hasFlag( Message::List ) )
  {
    QList<FileInfo> file_info_list = Protocol::instance().messageToShareBoxFileList( m, u.networkAddress().hostAddress() );
    emit shareBoxAvailable( u, folder_name, file_info_list );
  }
  else if( m.hasFlag( Message::Request ) )
  {
    if( m.hasFlag( Message::Refused ) )
      emit shareBoxUnavailable( u, folder_name );
    else
      buildShareBoxFileList( u, folder_name, false );
  }
  else if( m.hasFlag( Message::Create ) )
  {
    if( m.hasFlag( Message::Refused ) )
      emit shareBoxUnavailable( u, folder_name );
    else
      buildShareBoxFileList( u, folder_name, true );
  }
  else
    qWarning() << "Invalid flag found in share box message from user" << qPrintable( u.path() );
}

void Core::parseBuzzMessage( const User& u, const Message& )
{
  QString sys_msg = tr( "%1 %2 is buzzing you." ).arg( IconManager::instance().toHtml( "bell.png", "*Z*" ), Bee::userNameToShow( u, true ) );
  Chat c = ChatManager::instance().privateChatForUser( u.id() );
  if( !c.isValid() )
    c = ChatManager::instance().defaultChat();
  c.addUnreadMessage();
  ChatManager::instance().setChat( c );
  dispatchSystemMessage( c.id(), u.id(), sys_msg, DispatchToChat, ChatMessage::Other, false );
  emit localUserIsBuzzedBy( u );
}
