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

#include "FileTransfer.h"
#include "FileTransferDownload.h"
#include "FileTransferUpload.h"
#include "Settings.h"


FileTransfer::FileTransfer( QObject *parent )
  : QTcpServer( parent ), m_id( ID_START ), m_files(), m_peers()
{
}

bool FileTransfer::startListener()
{
  qDebug() << "Starting FileTransfer listener";
  if( isListening() )
  {
    qDebug() << "FileTransfer is already listening";
    return true;
  }

  if( !listen( QHostAddress::Any ) )
  {
    qDebug() << "FileTransfer cannot bind an address or a port";
    return false;
  }

  qDebug() << "FileTransfer listen" << serverAddress() << serverPort();
  resetServerFiles();
  return true;
}

void FileTransfer::stopListener()
{
  if( isListening() )
  {
    qDebug() << "FileTransfer listener closed";
    close();
  }
}

void FileTransfer::resetServerFiles()
{
  qDebug() << "FileTransfer reset files to" << serverAddress() << serverPort();
  QList<FileInfo>::iterator it = m_files.begin();
  while( it != m_files.end() )
  {
    (*it).setHostAddress( serverAddress() );
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

FileInfo FileTransfer::addFile( const QFileInfo& fi )
{
  FileInfo file_info = fileInfo( fi.absoluteFilePath() );
  if( file_info.isValid() )
    return file_info;
  file_info = FileInfo( newFileId(), FileInfo::Upload );
  file_info.setName( fi.fileName() );
  file_info.setPath( fi.absoluteFilePath() );
  file_info.setSize( fi.size() );
  file_info.setHostAddress( serverAddress() );
  file_info.setHostPort( serverPort() );
  QString password_key = QString( "%1%2%3" ).arg( file_info.id() ).arg( file_info.path() ).arg( QDateTime::currentDateTime().toString() );
  file_info.setPassword( Settings::instance().hash( password_key ) );
  m_files.append( file_info );
  return file_info;
}

void FileTransfer::incomingConnection( int socketDescriptor )
{
  FileTransferUpload *upload_peer = new FileTransferUpload( newFileId(), this );

  connect( upload_peer, SIGNAL( userConnected( const User& ) ), this, SIGNAL( userConnected( const User& ) ) );
  connect( upload_peer, SIGNAL( transferFinished() ), this, SLOT( stopUpload() ) );
  connect( upload_peer, SIGNAL( fileTransferRequest( VNumber, const QByteArray& ) ), this, SLOT( checkFileTransferRequest( VNumber, const QByteArray& ) ) );
  connect( upload_peer, SIGNAL( message( VNumber, const FileInfo&, const QString& ) ), this, SIGNAL( message( VNumber, const FileInfo&, const QString& ) ) );
  connect( upload_peer, SIGNAL( progress( VNumber, const FileInfo&, FileSizeType ) ), this, SIGNAL( progress( VNumber, const FileInfo&, FileSizeType ) ) );
  connect( upload_peer, SIGNAL( destroyed() ), this, SLOT( peerDestroyed() ) );
  m_peers.append( upload_peer );
  upload_peer->setConnectionDescriptor( socketDescriptor );
}

void FileTransfer::validateUser( const User& user_to_check, const User& user_connected )
{
  qDebug() << "FileTransfer validate user" << user_to_check.path() << "with" << user_connected.path();
  QList<FileTransferPeer*>::iterator it = m_peers.begin();
  while( it != m_peers.end() )
  {
    if( (*it)->userId() == user_to_check.id() )
      (*it)->setUserAuthenticated( user_connected );
    ++it;
  }
}

void FileTransfer::checkFileTransferRequest( VNumber file_id, const QByteArray& file_password )
{
  qDebug() << "Checking file request:" << file_id << file_password;
  FileTransferUpload *upload_peer = qobject_cast<FileTransferUpload*>( sender() );
  if( !upload_peer )
  {
    qWarning() << "FileTransfer received a signal from invalid FileTransferUpload instance";
    return;
  }

  FileInfo file_info = fileInfo( file_id );
  if( !file_info.isValid() )
  {
    qWarning() << "FileTransfer received a request of a file not in list";
    upload_peer->cancelTransfer();
    return;
  }

  if( file_info.password() != file_password )
  {
    qWarning() << "FileTransfer received a request for the file" << file_info.name() << "but with the wrong password";
    upload_peer->cancelTransfer();
    return;
  }

  upload_peer->startTransfer( file_info );
}

void FileTransfer::downloadFile( const FileInfo& fi )
{
  FileTransferDownload *download_peer = new FileTransferDownload( newFileId(), fi, this );
  connect( download_peer, SIGNAL( transferFinished() ), this, SLOT( stopDownload() ) );
  connect( download_peer, SIGNAL( message( VNumber, const FileInfo&, const QString& ) ), this, SIGNAL( message( VNumber, const FileInfo&, const QString& ) ) );
  connect( download_peer, SIGNAL( progress( VNumber, const FileInfo&, FileSizeType ) ), this, SIGNAL( progress( VNumber, const FileInfo&, FileSizeType ) ) );
  connect( download_peer, SIGNAL( destroyed() ), this, SLOT( dowloadPeerDestroyed() ) );

  m_peers.append( download_peer );
  download_peer->setConnectionDescriptor( 0 );
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
  return 0;
}

void FileTransfer::peerDestroyed()
{
  if( !sender() )
  {
    qWarning() << "Unable to cast object FileTransferPeer from the sender of the signal. List become invalid";
    return;
  }

  if( m_peers.removeOne( (FileTransferPeer*)sender() ) )
    qDebug() << "Removing peer from list." << m_peers.size() << "peers remained";
}

bool FileTransfer::cancelTransfer( VNumber peer_id )
{
  FileTransferPeer* transfer_peer = peer( peer_id );
  if( transfer_peer )
  {
    qDebug() << "Cancel transfer in progress of peer" << transfer_peer->id();
    transfer_peer->cancelTransfer();
    return true;
  }
  qDebug() << "Unable to cancel transfer in progress. Peer not found";
  return false;
}

void FileTransfer::stopUpload()
{
  qDebug() << "Upload finished";
  FileTransferUpload *upload_peer = qobject_cast<FileTransferUpload*>( sender() );
  if( upload_peer )
  {
    qDebug() << "Deleting peer" << upload_peer->id();
    upload_peer->deleteLater();
  }
}

void FileTransfer::stopDownload()
{
  qDebug() << "Download finished";
  FileTransferDownload *download_peer = qobject_cast<FileTransferDownload*>( sender() );
  if( download_peer )
  {
    qDebug() << "Deleting peer" << download_peer->id();
    download_peer->deleteLater();
  }
}
