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

#include "FileShare.h"
#include "FileTransfer.h"
#include "Random.h"
#include "Settings.h"
#include "Protocol.h"
#include "UserManager.h"


FileTransfer::FileTransfer( QObject *parent )
  : QTcpServer( parent ), m_files(), m_peers()
{
}

bool FileTransfer::startListener()
{
  qDebug() << "Starting File Transfer server";

  if( !listen( Settings::instance().hostAddressToListen(), static_cast<quint16>(Settings::instance().defaultFileTransferPort() ) ) )
  {
    qWarning() << "Unable to bind default file transfer port" << Settings::instance().defaultFileTransferPort();
    if( !listen( Settings::instance().hostAddressToListen() ) )
    {
      qWarning() << "Unable to bind a valid file transfer port";
      return false;
    }
  }

  qDebug() << "File Transfer server listen" << serverAddress().toString() << serverPort();
  resetServerFiles();
  emit listening();

  if( downloadsInQueue() > 0  )
    qDebug() << "File Transfer has" << downloadsInQueue() << "files in queue";

  return true;
}

void FileTransfer::stopListener()
{
  if( isListening() )
  {
    close();
    qDebug() << "File Transfer server closed";
    foreach( FileTransferPeer* transfer_peer, m_peers )
    {
      if( Settings::instance().resumeFileTransfer() )
        transfer_peer->pauseTransfer( true );
      else
        transfer_peer->cancelTransfer();
    }
  }
  else
    qDebug() << "File Transfer server is not active";
}

void FileTransfer::resetServerFiles()
{
#ifdef BEEBEEP_DEBUG
  qDebug() << "File Transfer reset files to"
           << qPrintable( Settings::instance().localUser().networkAddress().hostAddress().toString() )
           << serverPort();
#endif
  QList<FileInfo>::iterator it = m_files.begin();
  while( it != m_files.end() )
  {
    (*it).setHostAddress( Settings::instance().localUser().networkAddress().hostAddress() );
    (*it).setHostPort( serverPort() );
    ++it;
  }
}

void FileTransfer::removeFilesToUser( VNumber user_id )
{
  int peer_counter = 0;
  foreach( FileTransferPeer* transfer_peer, m_peers )
  {
    if( transfer_peer->isDownload() && transfer_peer->remoteUserId() == user_id )
    {
      transfer_peer->cancelTransfer();
      peer_counter++;
    }
  }

  if( peer_counter > 0 )
    qDebug() << peer_counter << "file transfer peers removed for user" << user_id;
}

FileInfo FileTransfer::fileInfo( VNumber file_id ) const
{
  QList<FileInfo>::const_iterator it = m_files.begin();
  while( it != m_files.end() )
  {
    if( (*it).id() == file_id )
      return *it;
    ++it;
  }
  return FileInfo();
}

FileInfo FileTransfer::fileInfo( const QString& file_absolute_path, const QString chat_private_id ) const
{
  QList<FileInfo>::const_iterator it = m_files.begin();
  while( it != m_files.end() )
  {
    if( (*it).path() == file_absolute_path && (*it).chatPrivateId() == chat_private_id )
      return *it;
    ++it;
  }
  return FileInfo();
}

void FileTransfer::removeFile( const QString& file_path )
{
  QList<FileInfo>::iterator it = m_files.begin();
  while( it != m_files.end() )
  {
    if( (*it).path() == file_path )
      it = m_files.erase( it );
    else
      ++it;
  }
}

FileInfo FileTransfer::addFile( const QFileInfo& fi, const QString& share_folder, bool to_share_box, const QString& chat_private_id, FileInfo::ContentType content_type )
{
  FileInfo file_info = fileInfo( fi.absoluteFilePath(), chat_private_id );
  if( file_info.isValid() )
  {
    QString file_hash = Protocol::instance().fileInfoHash( fi );
    if( file_info.fileHash() == file_hash )
      return file_info;
    else
      removeFile( fi.absoluteFilePath() );
  }

  file_info = Protocol::instance().fileInfo( fi, share_folder, to_share_box, chat_private_id, content_type );
  file_info.setHostAddress( Settings::instance().localUser().networkAddress().hostAddress() );
  file_info.setHostPort( serverPort() );
  m_files.append( file_info );
  return file_info;
}

void FileTransfer::addFileInfoList( const QList<FileInfo>& file_info_list )
{
  foreach( FileInfo fi, file_info_list )
  {
    if( !fileInfo( fi.id() ).isValid() )
      m_files.append( fi );
  }
}

void FileTransfer::incomingConnection( qintptr socket_descriptor )
{
  FileTransferPeer *upload_peer = new FileTransferPeer( this );
  upload_peer->setTransferType( FileInfo::Upload );
  upload_peer->setId( Protocol::instance().newId() );
  m_peers.append( upload_peer );
  setupPeer( upload_peer, socket_descriptor, serverPort() );
}

void FileTransfer::setupPeer( FileTransferPeer* transfer_peer, qintptr socket_descriptor, quint16 server_port )
{
  if( transfer_peer->isInQueue() )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << transfer_peer->name() << "is removed from queue";
#endif
    transfer_peer->removeFromQueue();
  }

#ifdef BEEBEEP_DEBUG
  qDebug() << transfer_peer->name() << "starts its connection. Active downloads:" << activeDownloads();
#endif
  if( !transfer_peer->isDownload() )
  {
    connect( transfer_peer, SIGNAL( fileUploadRequest( const FileInfo& ) ), this, SLOT( checkUploadRequest( const FileInfo& ) ) );
  }

  connect( transfer_peer, SIGNAL( progress( VNumber, VNumber, const FileInfo&, FileSizeType, int ) ), this, SIGNAL( progress( VNumber, VNumber, const FileInfo&, FileSizeType, int ) ) );
  connect( transfer_peer, SIGNAL( message( VNumber, VNumber, const FileInfo&, const QString&, FileTransferPeer::TransferState ) ), this, SIGNAL( message( VNumber, VNumber, const FileInfo&, const QString&, FileTransferPeer::TransferState ) ) );
  connect( transfer_peer, SIGNAL( operationCompleted() ), this, SLOT( deletePeer() ) );

  transfer_peer->setConnectionDescriptor( socket_descriptor, server_port );
  int delay = Random::number( 1, 9 ) * 100;
#ifdef BEEBEEP_DEBUG
  qDebug() << transfer_peer->name() << "starts in" << delay << "ms";
#endif
  QTimer::singleShot( delay, transfer_peer, SLOT( startConnection() ) );
}


void FileTransfer::checkUploadRequest( const FileInfo& file_info_to_check )
{
#ifdef BEEBEEP_DEBUG
  qDebug() << "Checking upload request:" << file_info_to_check.id() << file_info_to_check.password();
  qDebug() << "FileInfo name:" << file_info_to_check.name();
  qDebug() << "FileInfo path:" << file_info_to_check.path();
  qDebug() << "FileInfo share folder:" << file_info_to_check.shareFolder();
  qDebug() << "FileInfo is share box:" << file_info_to_check.isInShareBox();
  qDebug() << "FileInfo is folder:" << file_info_to_check.isFolder();
#endif

  FileTransferPeer *upload_peer = qobject_cast<FileTransferPeer*>( sender() );
  if( !upload_peer )
  {
    qWarning() << "File Transfer server received a signal from invalid upload peer";
    return;
  }

  if( !Settings::instance().enableFileTransfer() )
  {
    qWarning() << "File Transfer is disabled";
    upload_peer->cancelTransfer();
    return;
  }

  FileInfo file_info;

  if( file_info_to_check.isInShareBox() )
  {
    if( !Settings::instance().enableFileSharing() )
    {
      qWarning() << "BeeBOX file upload request refused (file sharing disabled):" << file_info_to_check.name();
      upload_peer->cancelTransfer();
      return;
    }

    if( !Settings::instance().useShareBox() )
    {
      qWarning() << "BeeBOX file upload request refused (sharebox disabled):" << file_info_to_check.name();
      upload_peer->cancelTransfer();
      return;
    }

    file_info = fileInfo( file_info_to_check.id() );
    if( !file_info.isValid() )
    {
      QString file_path;
      if( file_info_to_check.shareFolder().isEmpty() )
        file_path = QString( "%1/%2" ).arg( Settings::instance().shareBoxPath() )
                                      .arg( file_info_to_check.name() );
      else
        file_path = QString( "%1/%2/%3" ).arg( Settings::instance().shareBoxPath() )
                                         .arg( file_info_to_check.shareFolder() )
                                         .arg( file_info_to_check.name() );

      QFileInfo share_box_file_info( file_path );
      if( !share_box_file_info.exists() )
      {
        qWarning() << "BeeBOX file upload request refused (not exists):" << file_path;
        upload_peer->cancelTransfer();
      }

      if( !share_box_file_info.isReadable() )
      {
        qWarning() << "BeeBOX file upload request refused (not readable):" << file_path;
        upload_peer->cancelTransfer();
      }

      file_info = Protocol::instance().fileInfo( share_box_file_info, "", true, "", FileInfo::File );
    }
  }
  else
  {
    file_info = fileInfo( file_info_to_check.id() );

    if( !file_info.isValid() && !Settings::instance().disableFileSharing() )
    {
      // Now check file sharing
      file_info = FileShare::instance().localFileInfo( file_info_to_check.id() );
    }

    if( !file_info.isValid() )
    {
      qWarning() << "File Transfer server received a request of a file not in list (or file sharing is disabled)";
      upload_peer->cancelTransfer();
      return;
    }

    if( file_info.password() != file_info_to_check.password() )
    {
      qWarning() << "File Transfer server received a request for the file" << file_info.name() << "but with the wrong password";
      upload_peer->cancelTransfer();
      return;
    }
  }

  if( Settings::instance().resumeFileTransfer() )
    file_info.setStartingPosition( file_info_to_check.startingPosition() );
  else
    file_info.setStartingPosition( 0 );
  upload_peer->startUpload( file_info );
}

void FileTransfer::downloadFile( VNumber from_user_id, const FileInfo& fi )
{
  FileTransferPeer *download_peer = new FileTransferPeer( this );
  download_peer->setTransferType( FileInfo::Download );
  download_peer->setId( Protocol::instance().newId() );
  download_peer->setFileInfo( FileInfo::Download, fi );
  download_peer->setRemoteUserId( from_user_id );
  download_peer->setInQueue();
  m_peers.append( download_peer );
#ifdef BEEBEEP_DEBUG
  qDebug() << download_peer->name() << "is scheduled for download";
#endif

  if( activeDownloads() < Settings::instance().maxSimultaneousDownloads() )
    setupPeer( download_peer, 0 );
}

FileTransferPeer* FileTransfer::peer( VNumber peer_id ) const
{
  QList<FileTransferPeer*>::const_iterator it = m_peers.begin();
  while( it != m_peers.end() )
  {
    if( (*it)->id() == peer_id )
      return *it;
    ++it;
  }
  return Q_NULLPTR;
}

void FileTransfer::deletePeer()
{
  if( !sender() )
  {
    qWarning() << "File Transfer is unable to find peer sender of signal destroyed(). List become invalid";
    return;
  }

  FileTransferPeer* sender_peer = dynamic_cast<FileTransferPeer*>( sender() );
  if( m_peers.removeOne( sender_peer ) )
  {
    qDebug() << "Removed peer from list." << m_peers.size() << "peers remained";
    sender_peer->deleteLater();
  }

  if( isListening() && downloadsInQueue() > 0 )
    QTimer::singleShot( 0, this, SLOT( startNewDownload() ) ); // to next main loop avoiding a crash
}

bool FileTransfer::cancelTransfer( VNumber peer_id )
{
  FileTransferPeer* transfer_peer = peer( peer_id );
  if( transfer_peer )
  {
    transfer_peer->cancelTransfer();
    if( transfer_peer->isDownload() )
      transfer_peer->removePartiallyDownloadedFile();
    return true;
  }
  qWarning() << "File Transfer server cannot cancel the file transfer because it has not found the peer" << peer_id;
  return false;
}

bool FileTransfer::pauseTransfer( VNumber peer_id )
{
  FileTransferPeer* transfer_peer = peer( peer_id );
  if( transfer_peer )
  {
    if( transfer_peer->isDownload() )
    {
      transfer_peer->pauseTransfer( false );
      return true;
    }
    else
      return false;
  }
  qWarning() << "File Transfer server cannot pause the file transfer because it has not found the peer" << peer_id;
  return false;
}

int FileTransfer::activeDownloads() const
{
  int active_downloads = 0;
  foreach( FileTransferPeer* transfer_peer, m_peers )
  {
    if( transfer_peer->isDownload() && transfer_peer->isActive() )
      active_downloads++;
  }
  return active_downloads;
}

int FileTransfer::downloadsInQueue() const
{
  int downloads_in_queue = 0;
  foreach( FileTransferPeer* transfer_peer, m_peers )
  {
    if( transfer_peer->isDownload() && transfer_peer->isInQueue() )
      downloads_in_queue++;
  }
  return downloads_in_queue;
}

FileTransferPeer* FileTransfer::nextDownloadInQueue() const
{
  foreach( FileTransferPeer* transfer_peer, m_peers )
  {
    if( transfer_peer->isDownload() && transfer_peer->isInQueue() )
    {
      if( transfer_peer->remoteUserId() != ID_INVALID )
      {
        User remote_user = UserManager::instance().findUser( transfer_peer->remoteUserId() );
        if( remote_user.isValid() && remote_user.isStatusConnected() )
          return transfer_peer;
      }
    }
  }
  return Q_NULLPTR;
}

void FileTransfer::startNewDownload()
{
  if( activeDownloads() >= Settings::instance().maxSimultaneousDownloads() )
    return;

  FileTransferPeer* download_peer = nextDownloadInQueue();
  if( !download_peer )
    return;

#ifdef BEEBEEP_DEBUG
  qDebug() << download_peer->name() << "is removed from queue and started";
#endif

  setupPeer( download_peer, 0 );
}

void FileTransfer::onTickEvent( int ticks )
{
  if( !isListening() )
    return;

  foreach( FileTransferPeer* transfer_peer, m_peers )
    transfer_peer->onTickEvent( ticks );
}
