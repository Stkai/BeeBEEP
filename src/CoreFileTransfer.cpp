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
#include "Connection.h"
#include "Core.h"
#include "FileShare.h"
#include "FileTransferPeer.h"
#include "Protocol.h"
#include "Settings.h"
#include "XmppManager.h"
#include "UserManager.h"


bool Core::startFileTransferServer()
{
  qDebug() << "Starting File Transfer Server";
  if( !mp_fileTransfer->startListener() )
  {
    QString icon_html = Bee::iconToHtml( ":/images/upload.png", "*F*" );
    dispatchSystemMessage( "", ID_DEFAULT_CHAT, ID_LOCAL_USER, tr( "%1 Unable to start file transfer server: bind address/port failed." ).arg( icon_html ), DispatchToAllChatsWithUser );
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
  User user_connected = UserManager::instance().userList().find( user_to_check.path() );
  if( user_connected.isValid() )
    qDebug() << "Found a connected user to validate file transfer:" << user_connected.path();
  else
    qDebug() << user_to_check.path() << "is not authorized to file transfer";
  mp_fileTransfer->validateUser( peer_id, user_connected.id() );
}

void Core::downloadFile( const User& u, const FileInfo& fi )
{
  QString icon_html = Bee::iconToHtml( ":/images/download.png", "*F*" );
  dispatchSystemMessage( "", ID_DEFAULT_CHAT, u.id(), tr( "%1 Downloading %2 from %3." )
                         .arg( icon_html, fi.name(), u.path() ),
                         DispatchToAllChatsWithUser );
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
  dispatchSystemMessage( "", ID_DEFAULT_CHAT, u.id(), QString( "%1 %2 %3 %4: %5." ).arg( icon_html, fi.name(),
                         fi.isDownload() ? tr( "from") : tr( "to" ), u.path(), msg ),
                         DispatchToAllChatsWithUser );

  FileTransferPeer *peer = mp_fileTransfer->peer( peer_id );
  if( peer )
  {
    if( peer->isTransferCompleted() && fi.isDownload() )
    {
      QString s_open = tr( "Open" );
      dispatchSystemMessage( "", ID_DEFAULT_CHAT, u.id(), tr( "%1 %2 <a href='%3'>%4</a>." )
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
  QString icon_html = Bee::iconToHtml( ":/images/upload.png", "*F*" );

  QFileInfo file( file_path );
  if( !file.exists() )
  {
     dispatchSystemMessage( "", ID_DEFAULT_CHAT, u.id(), tr( "%1 %2: file not found." ).arg( icon_html, file_path ), DispatchToAllChatsWithUser );
     return false;
  }

  Settings::instance().setLastDirectorySelected( file.absoluteDir().absolutePath() );
  FileInfo fi = mp_fileTransfer->addFile( file );

  if( u.isOnLan() )
  {
    Connection* c = connection( u.id() );
    if( !c )
    {
      dispatchSystemMessage( "", ID_DEFAULT_CHAT, u.id(), tr( "%1 Unable to send the file: user is not connected." ).arg( icon_html ), DispatchToAllChatsWithUser );
      return false;
    }

    if( !mp_fileTransfer->isWorking() )
    {
      if( !startFileTransferServer() )
        return false;
      fi.setHostAddress( mp_fileTransfer->serverAddress() );
      fi.setHostPort( mp_fileTransfer->serverPort() );
    }

    Message m = Protocol::instance().fileInfoToMessage( fi );
    c->sendMessage( m );
  }
  else
  {
    if( !u.isConnected() )
    {
      dispatchSystemMessage( "", ID_DEFAULT_CHAT, u.id(), tr( "%1 Unable to send the file: user is not connected." ).arg( icon_html ), DispatchToAllChatsWithUser );
      return false;
    }

    if( !mp_xmppManager->sendFile( u, fi ) )
    {
      dispatchSystemMessage( "", ID_DEFAULT_CHAT, u.id(), tr( "%1 Unable to send the file %2 to %3." )
                             .arg( icon_html, fi.name(), u.name() ), DispatchToAllChatsWithUser );
      return false;
    }
  }

  dispatchSystemMessage( "", ID_DEFAULT_CHAT, u.id(), tr( "%1 You send the file %2 to %3." )
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

  dispatchSystemMessage( "", ID_DEFAULT_CHAT, u.id(), tr( "%1 You have refused to download %2 from %3." ).arg( icon_html, fi.name(), u.path() ), DispatchToAllChatsWithUser );
}

void Core::fileTransferServerListening()
{
  createLocalShareMessage();
}

void Core::sendFileShareRequestToAll()
{
  QList<FileInfo> file_info_list;
  file_info_list.append( FileInfo() );

  const QByteArray& file_share_request_message = Protocol::instance().fileShareRequestMessage();

  foreach( Connection* c, m_connections )
  {
    if( !FileShare::instance().network().contains( c->userId() ) )
    {
      if( c->sendData( file_share_request_message ) )
        FileShare::instance().addToNetwork( c->userId(), file_info_list );
    }
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
  if( !Settings::instance().fileShare() )
    return;

  const QByteArray& share_list_message = Protocol::instance().fileShareListMessage();

  if( share_list_message.isEmpty() )
    return;

  foreach( Connection* c, m_connections )
    c->sendData( share_list_message );
}

int Core::addPathToShare( const QString& share_path )
{
  int num_files = FileShare::instance().addPath( share_path );

  QStringList local_share = Settings::instance().localShare();
  local_share << share_path;
  Settings::instance().setLocalShare( local_share );

  createLocalShareMessage();
  sendFileShareListToAll();

  return num_files;
}

int Core::removePathFromShare( const QString& share_path )
{
  QStringList local_share = Settings::instance().localShare();
  if( local_share.removeOne( share_path ) )
    Settings::instance().setLocalShare( local_share );

  int num_files = FileShare::instance().removePath( share_path );

  if( num_files <= 0 )
    return 0;

  createLocalShareMessage();
  sendFileShareListToAll();

  return num_files;
}

void Core::createLocalShareMessage()
{
  if( mp_fileTransfer->isWorking() )
    Protocol::instance().createFileShareListMessage( FileShare::instance().local(), mp_fileTransfer->serverPort() );
  else
    Protocol::instance().createFileShareListMessage( FileShare::instance().local(), -1 );
}

void Core::buildLocalShareList()
{
  foreach( QString share_path, Settings::instance().localShare() )
    FileShare::instance().addPath( share_path );
}
