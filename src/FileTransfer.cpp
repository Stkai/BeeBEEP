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
#include "FileTransferPeer.h"
#include "Random.h"
#include "Settings.h"
#include "Protocol.h"


FileTransfer::FileTransfer( QObject *parent )
  : QTcpServer( parent ), m_files(), m_peers()
{
}

bool FileTransfer::startListener()
{
  qDebug() << "Starting File Transfer listener";
  if( isListening() )
  {
    qDebug() << "File Transfer server is already listening";
    return true;
  }

  if( !listen( QHostAddress::Any, Settings::instance().defaultFileTransferPort() ) )
  {
    qWarning() << "Unable to bind default file transfer port" << Settings::instance().defaultFileTransferPort();
    if( !listen( QHostAddress::Any ) )
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
    qDebug() << "File Transfer listener closed";
    close();
  }
}

void FileTransfer::resetServerFiles()
{
  qDebug() << "File Transfer reset files to" << Settings::instance().localUser().hostAddress().toString() << serverPort();
  QList<FileInfo>::iterator it = m_files.begin();
  while( it != m_files.end() )
  {
    (*it).setHostAddress( Settings::instance().localUser().hostAddress() );
    (*it).setHostPort( serverPort() );
    ++it;
  }
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

FileInfo FileTransfer::fileInfo( const QString& file_absolute_path ) const
{
  QList<FileInfo>::const_iterator it = m_files.begin();
  while( it != m_files.end() )
  {
    if( (*it).path() == file_absolute_path )
      return *it;
    ++it;
  }
  return FileInfo();
}

void FileTransfer::removeFile( const QFileInfo& fi )
{
  QString file_path = fi.absoluteFilePath();
  QList<FileInfo>::iterator it = m_files.begin();
  while( it != m_files.end() )
  {
    if( (*it).path() == file_path )
      it = m_files.erase( it );
    else
      ++it;
  }
}

FileInfo FileTransfer::addFile( const QFileInfo& fi, const QString& share_folder )
{
  FileInfo file_info = fileInfo( fi.absoluteFilePath() );
  if( file_info.isValid() )
    return file_info;
  file_info = Protocol::instance().fileInfo( fi, share_folder );
  file_info.setHostAddress( Settings::instance().localUser().hostAddress() );
  file_info.setHostPort( serverPort() );
  m_files.append( file_info );
  return file_info;
}

void FileTransfer::incomingConnection( qintptr socket_descriptor )
{
  FileTransferPeer *upload_peer = new FileTransferPeer( this );
  upload_peer->setTransferType( FileTransferPeer::Upload );
  upload_peer->setId( Protocol::instance().newId() );
  m_peers.append( upload_peer );
  setupPeer( upload_peer, socket_descriptor );
}

void FileTransfer::setupPeer( FileTransferPeer* transfer_peer, int socket_descriptor )
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
    connect( transfer_peer, SIGNAL( fileUploadRequest( VNumber, const QByteArray& ) ), this, SLOT( checkUploadRequest( VNumber, const QByteArray& ) ) );
  }

  connect( transfer_peer, SIGNAL( authenticationRequested() ), this, SLOT( checkAuthentication() ) );
  connect( transfer_peer, SIGNAL( progress( VNumber, VNumber, const FileInfo&, FileSizeType ) ), this, SIGNAL( progress( VNumber, VNumber, const FileInfo&, FileSizeType ) ) );
  connect( transfer_peer, SIGNAL( message( VNumber, VNumber, const FileInfo&, const QString& ) ), this, SIGNAL( message( VNumber, VNumber, const FileInfo&, const QString& ) ) );
  connect( transfer_peer, SIGNAL( destroyed() ), this, SLOT( peerDestroyed() ) );

  transfer_peer->setConnectionDescriptor( socket_descriptor );
  int delay = Random::number( 1, 9 ) * 100;
#ifdef BEEBEEP_DEBUG
  qDebug() << transfer_peer->name() << "starts in" << delay << "ms";
#endif
  QTimer::singleShot( delay, transfer_peer, SLOT( startConnection() ) );
}

void FileTransfer::checkAuthentication()
{
  FileTransferPeer* mp_peer = (FileTransferPeer*)sender();
  if( !mp_peer )
  {
    qWarning() << "Sender Peer not found in check authentication";
    return;
  }

  qDebug() << mp_peer->name() << "checks authentication message";
  emit userConnected( mp_peer->id(), mp_peer->peerAddress(), mp_peer->messageAuth() );
}

void FileTransfer::validateUser( VNumber FileTransferPeer_id, VNumber user_id )
{
  qDebug() << "File Transfer server validate user" << user_id << "for peer" << FileTransferPeer_id;
  QList<FileTransferPeer*>::iterator it = m_peers.begin();
  while( it != m_peers.end() )
  {
    if( (*it)->id() == FileTransferPeer_id )
    {
      if( user_id == ID_INVALID )
      {
        qWarning() << (*it)->name() << "has not authorized the user";
        (*it)->cancelTransfer();
      }
      else
      {
        qDebug() << (*it)->name() << "has authorized user" << user_id;
        (*it)->setUserAuthorized( user_id );
      }
      return;
    }
    ++it;
  }
}

void FileTransfer::checkUploadRequest( VNumber file_id, const QByteArray& file_password )
{
#ifdef BEEBEEP_DEBUG
  qDebug() << "Checking upload request:" << file_id << file_password;
#endif
  FileTransferPeer *upload_peer = qobject_cast<FileTransferPeer*>( sender() );
  if( !upload_peer )
  {
    qWarning() << "File Transfer server received a signal from invalid upload peer";
    return;
  }

  if( !Settings::instance().fileTransferIsEnabled() )
  {
    qWarning() << "File Transfer is disabled";
    upload_peer->cancelTransfer();
    return;
  }

  FileInfo file_info = fileInfo( file_id );
  if( !file_info.isValid() )
  {
    // Now check file sharing
    file_info = FileShare::instance().localFileInfo( file_id );

    if( !file_info.isValid() )
    {
      qWarning() << "File Transfer server received a request of a file not in list";
      upload_peer->cancelTransfer();
      return;
    }
  }

  if( file_info.password() != file_password )
  {
    qWarning() << "File Transfer server received a request for the file" << file_info.name() << "but with the wrong password";
    upload_peer->cancelTransfer();
    return;
  }

  upload_peer->startUpload( file_info );
}

void FileTransfer::downloadFile( const FileInfo& fi )
{
  FileTransferPeer *download_peer = new FileTransferPeer( this );
  download_peer->setTransferType( FileTransferPeer::Download );
  download_peer->setId( Protocol::instance().newId() );
  download_peer->setFileInfo( fi );
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
  qWarning() << "File Transfer server has not found the peer" << peer_id;
  return 0;
}

void FileTransfer::peerDestroyed()
{
  if( !sender() )
  {
    qWarning() << "File Transfer is unable to find peer sender of signal destroyed(). List become invalid";
    return;
  }

  if( m_peers.removeOne( (FileTransferPeer*)sender() ) )
    qDebug() << "Removing peer from list." << m_peers.size() << "peers remained";

  if( isListening() && downloadsInQueue() > 0 )
    startNewDownload();
}

bool FileTransfer::cancelTransfer( VNumber peer_id )
{
  FileTransferPeer* transfer_peer = peer( peer_id );
  if( transfer_peer )
  {
    qDebug() << "File Transfer server requests to cancel transfer in progress of" << transfer_peer->name();
    transfer_peer->cancelTransfer();
    return true;
  }
  qWarning() << "File Transfer server cannot cancel the file transfer because it has not found the peer" << peer_id;
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
      return transfer_peer;
  }
  return 0;
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
