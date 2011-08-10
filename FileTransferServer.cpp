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

#include "FileTransferServer.h"
#include "FileTransferDownload.h"
#include "FileTransferUpload.h"
#include "Settings.h"



FileTransferServer::FileTransferServer( QObject *parent )
  : QTcpServer( parent ), m_id( ID_START ), m_files()
{
}

bool FileTransferServer::startServer()
{
#if defined( BEEBEEP_DEBUG )
  qDebug() << "Starting FileTransferServer";
#endif
  if( isListening() )
  {
#if defined( BEEBEEP_DEBUG )
    qDebug() << "FileTransferServer is already listening";
#endif
    return true;
  }

  if( !listen( QHostAddress::Any ) )
  {
#if defined( BEEBEEP_DEBUG )
    qDebug() << "FileTransferServer cannot bind an address or a port";
#endif
    return false;
  }

#if defined( BEEBEEP_DEBUG )
  qDebug() << "FileTransferServer listen" << serverAddress() << serverPort();
#endif
  resetServerFiles();
  return true;
}

void FileTransferServer::stopServer()
{
#if defined( BEEBEEP_DEBUG )
  qDebug() << "FileTransferServer closed";
#endif
  close();
}

void FileTransferServer::resetServerFiles()
{
#if defined( BEEBEEP_DEBUG )
  qDebug() << "FileTransferServer reset files to" << serverAddress() << serverPort();
#endif
  QList<FileInfo>::iterator it = m_files.begin();
  while( it != m_files.end() )
  {
    (*it).setHostAddress( serverAddress() );
    (*it).setHostPort( serverPort() );
    ++it;
  }
}

FileInfo FileTransferServer::fileInfo( VNumber file_id ) const
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

FileInfo FileTransferServer::fileInfo( const QString& file_absolute_path ) const
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

FileInfo FileTransferServer::addFile( const QFileInfo& fi )
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

void FileTransferServer::incomingConnection( int socketDescriptor )
{
  FileTransferUpload *upload_peer = new FileTransferUpload( this );
  connect( upload_peer, SIGNAL( transferFinished() ), this, SLOT( stopUpload() ) );
  connect( upload_peer, SIGNAL( fileTransferRequest( VNumber, const QByteArray& ) ), this, SLOT( checkFileTransferRequest( VNumber, const QByteArray& ) ) );
  connect( upload_peer, SIGNAL( transferMessage( const User&, const FileInfo&, const QString& ) ), this, SIGNAL( transferMessage( const User&, const FileInfo&, const QString& ) ) );
  connect( upload_peer, SIGNAL( transferProgress( const User&, const FileInfo&, FileSizeType ) ), this, SIGNAL( transferProgress( const User&, const FileInfo&, FileSizeType ) ) );
  upload_peer->setConnectionDescriptor( socketDescriptor );
}

void FileTransferServer::checkFileTransferRequest( VNumber file_id, const QByteArray& file_password )
{
#if defined( BEEBEEP_DEBUG )
  qDebug() << "Checking file request:" << file_id << file_password;
#endif

  FileTransferUpload *upload_peer = qobject_cast<FileTransferUpload*>( sender() );
  if( !upload_peer )
  {
    qWarning() << "FileTransferServer received a signal from invalid FileTransferUpload instance";
    return;
  }

  FileInfo file_info = fileInfo( file_id );
  if( !file_info.isValid() )
  {
    qWarning() << "FileTransferServer received a request of a file not in list";
    upload_peer->cancelTransfer();
    return;
  }

  upload_peer->startTransfer( file_info );
}



void FileTransferServer::downloadFile( const User& u, const FileInfo& fi )
{
  FileTransferDownload *download_peer = new FileTransferDownload( u, fi, this );
  connect( download_peer, SIGNAL( transferFinished() ), this, SLOT( stopDownload() ) );
  connect( download_peer, SIGNAL( transferMessage( const User&, const FileInfo&, const QString& ) ), this, SIGNAL( transferMessage( const User&, const FileInfo&, const QString& ) ) );
  connect( download_peer, SIGNAL( transferProgress( const User&, const FileInfo&, FileSizeType ) ), this, SIGNAL( transferProgress( const User&, const FileInfo&, FileSizeType ) ) );
  download_peer->setConnectionDescriptor( 0 );
}

void FileTransferServer::stopUpload()
{
#if defined( BEEBEEP_DEBUG )
  qDebug() << "Upload finished";
#endif
  if( isListening() )
    close();
  FileTransferUpload *upload_peer = qobject_cast<FileTransferUpload*>( sender() );
  if( upload_peer )
  {
#if defined( BEEBEEP_DEBUG )
  qDebug() << "Deleting peer";
#endif
    upload_peer->deleteLater();
  }
}

void FileTransferServer::stopDownload()
{
#if defined( BEEBEEP_DEBUG )
  qDebug() << "Download finished";
#endif
  FileTransferDownload *download_peer = qobject_cast<FileTransferDownload*>( sender() );
  if( download_peer )
  {
#if defined( BEEBEEP_DEBUG )
  qDebug() << "Deleting peer";
#endif
    download_peer->deleteLater();
  }
}
