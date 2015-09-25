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
#include "BuildFileShareList.h"
#include "Connection.h"
#include "Core.h"
#include "FileShare.h"
#include "FileTransferPeer.h"
#include "Protocol.h"
#include "Settings.h"
#include "UserManager.h"


bool Core::startFileTransferServer()
{
  if( !mp_fileTransfer->startListener() )
  {
    QString icon_html = Bee::iconToHtml( ":/images/upload.png", "*F*" );
    dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER,
                           tr( "%1 Unable to start file transfer server: bind address/port failed." ).arg( icon_html ),
                           DispatchToChat, ChatMessage::FileTransfer );
    qWarning() << "Unable to start file transfer server: bind address/port failed";
    return false;
  }

  return true;
}

void Core::stopFileTransferServer()
{
  mp_fileTransfer->stopListener();
  createLocalShareMessage();
}

void Core::validateUserForFileTransfer( VNumber peer_id, const QHostAddress& peer_address, const Message& m  )
{
  User user_to_check = Protocol::instance().createUser( m, peer_address );

  User user_connected = user_to_check.isValid() ? UserManager::instance().findUserByPath( user_to_check.path() ) : User();
  if( user_connected.isValid() )
    qDebug() << "Found a connected user" << user_connected.id() << user_connected.path() << "to continue file transfer" << peer_id;
  else
    qWarning() << user_to_check.path() << "is not authorized for file transfer" << peer_id;
  mp_fileTransfer->validateUser( peer_id, user_connected.id() );
}

bool Core::downloadFile( VNumber user_id, const FileInfo& fi, bool show_message )
{
  if( !mp_fileTransfer->isListening() )
  {
    if( !startFileTransferServer() )
      return false;
  }

  User u = UserManager::instance().userList().find( user_id );
  if( !u.isValid() )
  {
    qWarning() << "Unable to find user" << user_id << "to download file" << fi.name();
    return false;
  }

  QString icon_html;

  if( !u.isConnected() )
  {
    icon_html = Bee::iconToHtml( ":/images/red-ball.png", "*F*" );
    dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), tr( "%1 Unable to download %2 from %3: user is offline." ).arg( icon_html, fi.name(), u.name() ),
                             DispatchToAllChatsWithUser, ChatMessage::FileTransfer );
    qWarning() << "Unable to download" << fi.name() << "from" << u.path() << "because user is offline";
    return false;
  }

  QFileInfo file_info( fi.path() );
  QDir folder_path = file_info.absoluteDir();
  if( !folder_path.exists() )
  {
    if( !folder_path.mkpath( "." ) )
    {
      if( show_message )
      {
        icon_html = Bee::iconToHtml( ":/images/red-ball.png", "*F*" );
        dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), tr( "%1 Unable to download %2 from %3: folder %4 cannot be created." )
                             .arg( icon_html, fi.name(), u.name(), folder_path.absolutePath() ),
                             DispatchToAllChatsWithUser, ChatMessage::FileTransfer );
      }
      qWarning() << "Unable to download" << fi.name() << "because folder" << folder_path.absolutePath() << "can not be created";
      return false;
    }
  }

  if( show_message )
  {
    icon_html = Bee::iconToHtml( ":/images/download.png", "*F*" );
    dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), tr( "%1 Downloading %2 from %3." )
                         .arg( icon_html, fi.name(), u.name() ),
                         DispatchToAllChatsWithUser, ChatMessage::FileTransfer );
  }

  qDebug() << "Downloading file" << fi.path() << "from user" << u.path();
  mp_fileTransfer->downloadFile( fi );
  return true;
}

void Core::checkFileTransferMessage( VNumber peer_id, VNumber user_id, const FileInfo& fi, const QString& msg )
{
  User u = UserManager::instance().userList().find( user_id );
  if( !u.isValid() )
  {
    qWarning() << "Unable to find user" << user_id << "for the file transfer" << fi.name();
    return;
  }

  QString icon_html = Bee::iconToHtml( fi.isDownload() ? ":/images/download.png" : ":/images/upload.png", "*F*" );
  QString sys_msg = QString( "%1 %2 %3 %4: %5." ).arg( icon_html, fi.name(), fi.isDownload() ? tr( "from") : tr( "to" ), u.name(), msg );

  FileTransferPeer *peer = mp_fileTransfer->peer( peer_id );
  if( peer )
  {
    if( peer->isTransferCompleted() && fi.isDownload() )
    {
      FileShare::instance().addDownloadedFile( fi );
      bool show_image_preview = false;
      if( Settings::instance().showImagePreview() && Bee::isFileTypeImage( fi.suffix() ) )
      {
        QImage img;
        QImageReader img_reader( fi.path() );
        img_reader.setAutoDetectImageFormat( true );
        int img_preview_height = Settings::instance().imagePreviewHeight();
        QString img_preview_path = fi.path();
        if( img_reader.read( &img ) )
        {
          if( img.height() > img_preview_height )
          {
            QString img_file_name = QString( "beeimgtmp-%1-%2.png" ).arg( Bee::dateTimeStringSuffix( QDateTime::currentDateTime() ) ).arg( fi.id() );
            QString img_file_path = Bee::convertToNativeFolderSeparator( QString( "%1/%2" ).arg( Settings::instance().downloadDirectory() ).arg( img_file_name ) );
            QImage img_scaled = img.scaledToHeight( Settings::instance().imagePreviewHeight(), Qt::SmoothTransformation );
            if( img_scaled.save( img_file_path, "png" ) )
            {
              Settings::instance().addTemporaryFilePath( img_file_path );
              img_preview_path = img_file_path;
            }
          }
          else
            img_preview_height = img.height();

          sys_msg += QString( "<br /><br />&nbsp;&nbsp;&nbsp;&nbsp;<img src=\"%1\" height=\"%2\" /><br />&nbsp;&nbsp;&nbsp;&nbsp;" )
                  .arg( img_preview_path ).arg( img_preview_height );
          show_image_preview = true;
        }
        else
          qWarning() << "Unable to show image preview of the file" << img_preview_path;
      }

      QString s_open = tr( "Open" );
      sys_msg += QString( " %1 <a href=\"%2\">%3</a>." ).arg( s_open, QUrl::fromLocalFile( fi.path() ).toString(), fi.name() );
      QFileInfo file_info( fi.path() );
      sys_msg += QString( " %1 <a href=\"%2\">%3</a>." ).arg( s_open, QUrl::fromLocalFile( file_info.absoluteDir().absolutePath() ).toString(), tr( "folder" ) );
      if( show_image_preview )
        sys_msg += QString( "<br />" );
    }
  }

  dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), sys_msg, DispatchToAllChatsWithUser, ChatMessage::FileTransfer );

  emit fileTransferMessage( peer_id, u, fi, msg );
}

void Core::checkFileTransferProgress( VNumber peer_id, VNumber user_id, const FileInfo& fi, FileSizeType bytes )
{
  User u = UserManager::instance().userList().find( user_id );
  if( !u.isValid() )
  {
    qWarning() << "Unable to find user" << user_id << "for the file transfer" << fi.name();
    return;
  }
  emit fileTransferProgress( peer_id, u, fi, bytes );
}

bool Core::sendFile( VNumber user_id, const QString& file_path )
{
  QString icon_html = Bee::iconToHtml( ":/images/red-ball.png", "*F*" );

  if( !Settings::instance().fileTransferIsEnabled() )
  {
    dispatchSystemMessage( ID_DEFAULT_CHAT, user_id, tr( "%1 Unable to send %2. File transfer is disabled." ).arg( icon_html, file_path ),
                           DispatchToAllChatsWithUser, ChatMessage::FileTransfer );
    return false;
  }

  if( !mp_fileTransfer->isListening() )
  {
    if( !startFileTransferServer() )
      return false;
  }

  User u = UserManager::instance().userList().find( user_id );
  if( !u.isValid() )
  {
    qWarning() << "Unable to find user" << user_id << "to send file" << file_path;
    return false;
  }

  if( u.isLocal() )
  {
    qWarning() << "Unable to send file to local user";
    return false;
  }

  if( !u.isConnected() )
  {
    dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), tr( "%1 Unable to send %2 to %3: user is offline." ).arg( icon_html, file_path, u.name() ),
                             DispatchToAllChatsWithUser, ChatMessage::FileTransfer );
    qWarning() << "Unable to send" << file_path << "to" << u.path() << "because user is offline";
    return false;
  }

  QFileInfo file( file_path );
  if( !file.exists() )
  {
    dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), tr( "%1 %2: file not found." ).arg( icon_html, file_path ),
                           DispatchToAllChatsWithUser, ChatMessage::FileTransfer );
    return false;
  }

  if( file.isDir() )
  {
    if( sendFolder( u, file ) )
      return true;

    dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), tr( "%1 %2 is a folder. You can share it." ).arg( icon_html, file.fileName() ),
                           DispatchToAllChatsWithUser, ChatMessage::FileTransfer );
    return false;
  }

  FileInfo fi = mp_fileTransfer->addFile( file, "" );

  Connection* c = connection( u.id() );
  if( !c )
  {
    dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), tr( "%1 Unable to send %2: %3 is not connected." )
                           .arg( icon_html ).arg( fi.name() ).arg( u.name() ),
                           DispatchToAllChatsWithUser, ChatMessage::FileTransfer );
    return false;
  }

  qDebug() << "File Transfer: sending" << fi.path() << "to" << u.path();
  icon_html = Bee::iconToHtml( ":/images/upload.png", "*F*" );
  Message m = Protocol::instance().fileInfoToMessage( fi );
  c->sendMessage( m );

  dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), tr( "%1 You send %2 to %3." )
                       .arg( icon_html, fi.name(), u.name() ), DispatchToAllChatsWithUser, ChatMessage::FileTransfer );
  return true;
}

void Core::cancelFileTransfer( VNumber peer_id )
{
  qDebug() << "Core received a request for cancelling file transfer" << peer_id;
  mp_fileTransfer->cancelTransfer( peer_id );
}

void Core::refuseToDownloadFile( VNumber user_id, const FileInfo& fi )
{
  User u = UserManager::instance().userList().find( user_id );

  if( !u.isValid() )
  {
    qWarning() << "Unable to find user" << user_id << "to refuse file" << fi.name();
    return;
  }

  qDebug() << "Download refused:" << fi.name() << "from" << u.path();

  dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), tr( "%1 You have refused to download %2 from %3." )
                         .arg( Bee::iconToHtml( ":/images/download.png", "*F*" ), fi.name(), u.name() ),
                         DispatchToAllChatsWithUser, ChatMessage::FileTransfer );

  Message m = Protocol::instance().fileInfoRefusedToMessage( fi );

  Connection* c = connection( u.id() );
  if( !c )
  {
    qDebug() << u.path() << "is not connected. Unable to send file download refused message";
    return;
  }

  if( c->protoVersion() > 1 )
    c->sendMessage( m );

}

void Core::refuseToDownloadFolder( VNumber user_id, const QString& folder_name )
{
  User u = UserManager::instance().userList().find( user_id );

  if( !u.isValid() )
  {
    qWarning() << "Unable to find user" << user_id << "to refuse folder" << folder_name;
    return;
  }

  dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), tr( "%1 You have refused to download folder %2 from %3." )
                         .arg( Bee::iconToHtml( ":/images/download.png", "*F*" ), folder_name, u.name() ),
                         DispatchToAllChatsWithUser, ChatMessage::FileTransfer );

  Message m = Protocol::instance().folderRefusedToMessage( folder_name );

  Connection* c = connection( u.id() );
  if( !c )
  {
    qDebug() << u.path() << "is not connected. Unable to send folder download refused message";
    return;
  }

  if( c->protoVersion() > 1 )
    c->sendMessage( m );
}

void Core::fileTransferServerListening()
{
  createLocalShareMessage();
}

void Core::sendFileShareRequestToAll()
{
  const QByteArray& file_share_request_message = Protocol::instance().fileShareRequestMessage();
  foreach( Connection* c, m_connections )
  {
    if( !FileShare::instance().userHasFileShareList( c->userId() ) )
      c->sendData( file_share_request_message );
  }
}

void Core::sendFileShareListTo( VNumber user_id )
{
  Connection* c = connection( user_id );
  if( c )
    c->sendData( Protocol::instance().fileShareListMessage() );
  else
    qWarning() << user_id << "is not a valid user id. Unable to send share list message";
}

void Core::sendFileShareListToAll()
{
  if( !Settings::instance().fileTransferIsEnabled() )
    return;

  if( !isConnected() )
    return;

  if( !mp_fileTransfer->isListening() )
    return;

  const QByteArray& share_list_message = Protocol::instance().fileShareListMessage();

  if( share_list_message.isEmpty() )
    return;

  foreach( Connection* c, m_connections )
    c->sendData( share_list_message );
}

void Core::addPathToShare( const QString& share_path, bool broadcast_list )
{
  qDebug() << "Adding to file sharing" << share_path;

  QString share_status = tr( "Adding to file sharing" ) + QString( " %1" ).arg( share_path );
  emit updateStatus( share_status + QString( " ..." ), 1000 );

  BuildFileShareList *bfsl = new BuildFileShareList;
  bfsl->setFolderPath( share_path );
  bfsl->setBroadcastList( broadcast_list );
  connect( bfsl, SIGNAL( listCompleted() ), this, SLOT( addListToLocalShare() ) );
  BeeApplication* bee_app = (BeeApplication*)qApp;
  bee_app->addJob( bfsl );
  QMetaObject::invokeMethod( bfsl, "buildList", Qt::QueuedConnection );
}

void Core::addListToLocalShare()
{
  BuildFileShareList *bfsl = qobject_cast<BuildFileShareList*>( sender() );
  if( !bfsl )
  {
    qWarning() << "Core received a signal from invalid BuildFileShareList instance";
    emit localShareListAvailable();
    return;
  }

  if( m_shareListToBuild > 0 )
    m_shareListToBuild--;

  QString share_status;

  if( bfsl->shareList().size() < 2 )
    share_status = tr( "%1 is added to file sharing (%2)" ).arg( bfsl->folderPath(), Bee::bytesToString( bfsl->shareSize() ) );
  else
    share_status = tr( "%1 is added to file sharing with %2 files, %3 (elapsed time: %4)" )
                           .arg( bfsl->folderPath() )
                           .arg( bfsl->shareList().size() )
                           .arg( Bee::bytesToString( bfsl->shareSize() ) )
                           .arg( Bee::elapsedTimeToString( bfsl->elapsedTime() ) );
  emit updateStatus( share_status, 0 );
  dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER, QString( "%1 %2." ).arg( Bee::iconToHtml( ":/images/upload.png", "*F*" ), share_status ),
                         DispatchToChat, ChatMessage::FileTransfer );

  FileShare::instance().addToLocal( bfsl->folderPath(), bfsl->shareList(), bfsl->shareSize() );

  BeeApplication* bee_app = (BeeApplication*)qApp;
  bee_app->removeJob( bfsl );

  if( m_shareListToBuild == 0 )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Building local share list completed";
#endif
    createLocalShareMessage();

    if( bfsl->broadcastList() )
      sendFileShareListToAll();

    emit localShareListAvailable();
  }

  bfsl->deleteLater();
}

void Core::removePathFromShare( const QString& share_path )
{
  int num_files = FileShare::instance().removePath( share_path );

  QString share_status;

  if( num_files < 2 )
    share_status = tr( "%1 is removed from file sharing" ).arg( share_path );
  else
    share_status = tr( "%1 is removed from file sharing with %2 files" ).arg( share_path ).arg( num_files );

  emit updateStatus( share_status, 0 );
  dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER, QString( "%1 %2." ).arg( Bee::iconToHtml( ":/images/upload.png", "*F*" ), share_status ),
                         DispatchToChat, ChatMessage::FileTransfer );

  if( num_files > 0 )
  {
    createLocalShareMessage();
    sendFileShareListToAll();
  }

  emit localShareListAvailable();
}

void Core::createLocalShareMessage()
{
  if( mp_fileTransfer->isListening() )
    Protocol::instance().createFileShareListMessage( FileShare::instance().local(), mp_fileTransfer->serverPort() );
  else
    Protocol::instance().createFileShareListMessage( FileShare::instance().local(), -1 );
}

void Core::buildLocalShareList()
{
  bool is_connected = isConnected();
  m_shareListToBuild = Settings::instance().localShare().size();
  if( !FileShare::instance().local().isEmpty() )
    FileShare::instance().clearLocal();
  if( Settings::instance().localShare().isEmpty() )
    return;
  foreach( QString share_path, Settings::instance().localShare() )
    addPathToShare( share_path, is_connected );
}

bool Core::sendFolder( const User& u, const QFileInfo& file_info )
{
  QString icon_html = Bee::iconToHtml( ":/images/upload.png", "*F*" );
  dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), tr( "%1 You are about to send %2 to %3. Checking folder..." )
                         .arg( icon_html, file_info.fileName(), u.name() ), DispatchToAllChatsWithUser, ChatMessage::FileTransfer );
  BuildFileShareList *bfsl = new BuildFileShareList;
  bfsl->setFolderPath( file_info.absoluteFilePath() );
  bfsl->setBroadcastList( false );
  bfsl->setUserId( u.id() );
  connect( bfsl, SIGNAL( listCompleted() ), this, SLOT( addFolderToFileTransfer() ) );
  BeeApplication* bee_app = (BeeApplication*)qApp;
  bee_app->addJob( bfsl );
  QMetaObject::invokeMethod( bfsl, "buildList", Qt::QueuedConnection );
  return true;
}

void Core::addFolderToFileTransfer()
{
  BuildFileShareList *bfsl = qobject_cast<BuildFileShareList*>( sender() );
  if( !bfsl )
  {
    qWarning() << "Core received a signal from invalid BuildFileShareList instance";
    return;
  }

  BeeApplication* bee_app = (BeeApplication*)qApp;
  bee_app->removeJob( bfsl );
  QString folder_name = bfsl->folderName();
  QList<FileInfo> file_info_list = bfsl->shareList();
  bfsl->deleteLater();

  if( folder_name.isEmpty() )
  {
    qWarning() << "Unable to send folder. Internal error. Folder is empty";
    return;
  }

  User u = UserManager::instance().userList().find( bfsl->userId() );
  QString sys_header = tr( "%1 Unable to send folder %2" ).arg( Bee::iconToHtml( ":/images/red-ball.png", "*F*" ) )
                                                          .arg( folder_name ) + QString( ": " );

  if( file_info_list.isEmpty() )
  {
    dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), sys_header + tr( "the folder is empty." ),
                           DispatchToAllChatsWithUser, ChatMessage::FileTransfer );
    return;
  }

  if( !mp_fileTransfer->isListening() || mp_fileTransfer->serverPort() <= 0 )
  {
    dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), sys_header + tr( "file transfer is not working." ),
                           DispatchToAllChatsWithUser, ChatMessage::FileTransfer );
    return;
  }

  Connection* c = connection( u.id() );
  if( !c )
  {
    dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), sys_header + tr( "%1 is not connected." ).arg( u.name() ),
                           DispatchToAllChatsWithUser, ChatMessage::FileTransfer );
    return;
  }

  qDebug() << "File Transfer: sending folder" << folder_name << "to" << u.path();

  mp_fileTransfer->addFileInfoList( file_info_list );

  QString icon_html = Bee::iconToHtml( ":/images/upload.png", "*F*" );
  Message m = Protocol::instance().createFolderMessage( folder_name, file_info_list, mp_fileTransfer->serverPort() );

  if( !m.isValid() )
  {
    dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), sys_header + tr( "internal error." ),
                             DispatchToAllChatsWithUser, ChatMessage::FileTransfer );
    return;
  }

  dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), tr( "%1 You send folder %2 to %3." )
                         .arg( icon_html, folder_name, u.name() ), DispatchToAllChatsWithUser, ChatMessage::FileTransfer );

  c->sendMessage( m );

}
