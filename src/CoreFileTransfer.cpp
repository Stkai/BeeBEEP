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
    dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER, tr( "%1 Unable to start file transfer server: bind address/port failed." ).arg( icon_html ), DispatchToAllChatsWithUser );
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

void Core::downloadFile( const User& u, const FileInfo& fi )
{
  if( !mp_fileTransfer->isListening() )
  {
    if( !startFileTransferServer() )
      return;
  }

  QString icon_html = Bee::iconToHtml( ":/images/download.png", "*F*" );
  dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), tr( "%1 Downloading %2 from %3." )
                         .arg( icon_html, fi.name(), u.name() ),
                         DispatchToAllChatsWithUser );

  qDebug() << "Downloading file" << fi.path() << "from user" << u.path();
  mp_fileTransfer->downloadFile( fi );
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
  dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), QString( "%1 %2 %3 %4: %5." ).arg( icon_html, fi.name(),
                         fi.isDownload() ? tr( "from") : tr( "to" ), u.name(), msg ),
                         DispatchToAllChatsWithUser );

  FileTransferPeer *peer = mp_fileTransfer->peer( peer_id );
  if( peer )
  {
    if( peer->isTransferCompleted() && fi.isDownload() )
    {
      FileShare::instance().addDownloadedFile( fi );
      QString s_open = tr( "Open" );
      dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), QString( "%1 %2 <a href='%3'>%4</a>." )
                             .arg( icon_html, s_open, QUrl::fromLocalFile( fi.path() ).toString(), fi.name() ),
                             DispatchToAllChatsWithUser );
    }
  }
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

bool Core::sendFile( const User& u, const QString& file_path )
{
  if( u.isLocal() )
    return false;

  QString icon_html = Bee::iconToHtml( ":/images/upload.png", "*F*" );

  if( !Settings::instance().fileTransferIsEnabled() )
  {
    dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), tr( "%1 Unable to send %2. File transfer is disabled." ).arg( icon_html, file_path ), DispatchToAllChatsWithUser );
    return false;
  }

  QFileInfo file( file_path );
  if( !file.exists() )
  {
    dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), tr( "%1 %2: file not found." ).arg( icon_html, file_path ), DispatchToAllChatsWithUser );
    return false;
  }

  if( file.isDir() )
  {
    dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), tr( "%1 %2 is a folder. You can share it." ).arg( icon_html, file.fileName() ), DispatchToAllChatsWithUser );
    return false;
  }

  FileInfo fi = mp_fileTransfer->addFile( file );

  Connection* c = connection( u.id() );
  if( !c )
  {
    dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), tr( "%1 Unable to send %2: %3 is not connected." )
                           .arg( icon_html ).arg( fi.name() ).arg( u.name() ), DispatchToAllChatsWithUser );
    return false;
  }

  if( !mp_fileTransfer->isListening() )
  {
    if( !startFileTransferServer() )
      return false;
    fi.setHostAddress( mp_fileTransfer->serverAddress() );
    fi.setHostPort( mp_fileTransfer->serverPort() );
  }

  qDebug() << "File Transfer: sending" << fi.path() << "to" << u.path();
  Message m = Protocol::instance().fileInfoToMessage( fi );
  c->sendMessage( m );

  dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), tr( "%1 You send %2 to %3." )
                       .arg( icon_html, fi.name(), u.name() ), DispatchToAllChatsWithUser );
  return true;
}

void Core::cancelFileTransfer( VNumber peer_id )
{
  qDebug() << "Core received a request for cancelling file transfer" << peer_id;
  mp_fileTransfer->cancelTransfer( peer_id );
}

void Core::refuseToDownloadFile( const User& u, const FileInfo& fi )
{
  qDebug() << "Download refused:" << fi.name() << "from" << u.path();

  QString icon_html = Bee::iconToHtml( ":/images/download.png", "*F*" );

  Message m = Protocol::instance().fileInfoRefusedToMessage( fi );

  Connection* c = connection( u.id() );
  if( !c )
  {
    qDebug() << u.path() << "is not connected. Unable to send download refused message";
    return;
  }

  if( c->protoVersion() > 1 )
    c->sendMessage( m );

  dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), tr( "%1 You have refused to download %2 from %3." ).arg( icon_html, fi.name(), u.name() ), DispatchToAllChatsWithUser );
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
  bfsl->setPath( share_path );
  bfsl->setBroadcastList( broadcast_list );
  connect( bfsl, SIGNAL( listCompleted() ), this, SLOT( addListToLocalShare() ) );
  BeeApplication* bee_app = (BeeApplication*)qApp;
  bfsl->moveToThread( bee_app->jobThread() );
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
    share_status = tr( "%1 is added to file sharing (%2)" ).arg( bfsl->path(), Bee::bytesToString( bfsl->shareSize() ) );
  else
    share_status = tr( "%1 is added to file sharing with %2 files, %3 (elapsed time: %4)" )
                           .arg( bfsl->path() )
                           .arg( bfsl->shareList().size() )
                           .arg( Bee::bytesToString( bfsl->shareSize() ) )
                           .arg( Bee::elapsedTimeToString( bfsl->elapsedTime() ) );
  emit updateStatus( share_status, 0 );
  dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER, QString( "%1 %2." ).arg( Bee::iconToHtml( ":/images/upload.png", "*F*" ), share_status ), DispatchToChat );

  FileShare::instance().addToLocal( bfsl->path(), bfsl->shareList(), bfsl->shareSize() );

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
  dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER, QString( "%1 %2." ).arg( Bee::iconToHtml( ":/images/upload.png", "*F*" ), share_status ), DispatchToChat );

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
  m_shareListToBuild = Settings::instance().localShare().size();
  if( !FileShare::instance().local().isEmpty() )
    FileShare::instance().clearLocal();
  foreach( QString share_path, Settings::instance().localShare() )
    addPathToShare( share_path, false );
}
