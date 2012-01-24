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
#include "FileTransferPeer.h"
#include "Settings.h"
#include "Protocol.h"


FileTransfer::FileTransfer( QObject *parent )
  : QTcpServer( parent ), m_id( ID_START ), m_files(), m_peers()
{
  connect( this, SIGNAL( newPeerConnected( FileTransferPeer* , int ) ), this, SLOT( setupPeer( FileTransferPeer*, int ) ) );
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
  file_info.setSuffix( fi.suffix() );
  file_info.setSize( fi.size() );
  file_info.setHostAddress( serverAddress() );
  file_info.setHostPort( serverPort() );
  QString password_key = QString( "%1%2%3" ).arg( file_info.id() ).arg( file_info.path() ).arg( QDateTime::currentDateTime().toString() );
  file_info.setPassword( Settings::instance().hash( password_key ) );
  m_files.append( file_info );
  return file_info;
}

void FileTransfer::incomingConnection( int socket_descriptor )
{
  FileTransferPeer *upload_peer = new FileTransferPeer( this );
  upload_peer->setTransferType( FileTransferPeer::Upload );
  upload_peer->setId( newFileId() );
  m_peers.append( upload_peer );
  emit newPeerConnected( upload_peer, socket_descriptor );
}

void FileTransfer::setupPeer( FileTransferPeer* transfer_peer, int socket_descriptor )
{
  qDebug() << "Setup peer" << transfer_peer->id() << "with socket descriptor" << socket_descriptor;
  if( !transfer_peer->isDownload() )
  {
    connect( transfer_peer, SIGNAL( fileUploadRequest( VNumber, const QByteArray& ) ), this, SLOT( checkUploadRequest( VNumber, const QByteArray& ) ) );
  }

  connect( transfer_peer, SIGNAL( authenticationRequested() ), this, SLOT( checkAuthentication() ) );
  connect( transfer_peer, SIGNAL( progress( VNumber, VNumber, const FileInfo&, FileSizeType ) ), this, SIGNAL( progress( VNumber, VNumber, const FileInfo&, FileSizeType ) ) );
  connect( transfer_peer, SIGNAL( message( VNumber, VNumber, const FileInfo&, const QString& ) ), this, SIGNAL( message( VNumber, VNumber, const FileInfo&, const QString& ) ) );
  connect( transfer_peer, SIGNAL( destroyed() ), this, SLOT( peerDestroyed() ) );
  transfer_peer->setConnectionDescriptor( socket_descriptor );
}

void FileTransfer::checkAuthentication()
{
  qDebug() << "Checking authentication message";
  FileTransferPeer* mp_peer = (FileTransferPeer*)sender();
  emit userConnected( mp_peer->id(), mp_peer->peerAddress(), mp_peer->messageAuth() );
}

void FileTransfer::validateUser( VNumber FileTransferPeer_id, VNumber user_id )
{
  qDebug() << "FileTransfer validate user for FileTransferPeer" << FileTransferPeer_id;
  QList<FileTransferPeer*>::iterator it = m_peers.begin();
  while( it != m_peers.end() )
  {
    if( (*it)->id() == FileTransferPeer_id )
    {
      if( user_id == ID_INVALID )
      {
        qDebug() << "FileTransferPeer not authorized";
        (*it)->cancelTransfer();
      }
      else
      {
        qDebug() << "FileTransferPeer authorized with user id" << user_id;
        (*it)->setUserAuthorized( user_id );
      }
      return;
    }
    ++it;
  }
}

void FileTransfer::checkUploadRequest( VNumber file_id, const QByteArray& file_password )
{
  qDebug() << "Checking upload request:" << file_id << file_password;
  FileTransferPeer *upload_peer = qobject_cast<FileTransferPeer*>( sender() );
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

  upload_peer->startUpload( file_info );
}

void FileTransfer::downloadFile( const FileInfo& fi )
{
  qDebug() << "Downloading file" << fi.name();
  FileTransferPeer *download_peer = new FileTransferPeer( this );
  download_peer->setTransferType( FileTransferPeer::Download );
  download_peer->setId( newFileId() );
  download_peer->setFileInfo( fi );
  m_peers.append( download_peer );
  emit newPeerConnected( download_peer, 0 );
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
  qWarning() << "FileTransferPeer" << peer_id << "not found";
  return 0;
}

void FileTransfer::peerDestroyed()
{
  if( !sender() )
  {
    qWarning() << "Unable to find peer sender of signal destroyed(). List become invalid";
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
    qDebug() << "Cancel transfer in progress of peeer" << transfer_peer->id();
    transfer_peer->cancelTransfer();
    return true;
  }
  qDebug() << "Unable to cancel transfer in progress. Peer not found";
  return false;
}

