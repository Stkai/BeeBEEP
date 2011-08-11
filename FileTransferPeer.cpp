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

#include "FileTransferPeer.h"


FileTransferPeer::FileTransferPeer( VNumber peer_id, QObject *parent )
  : QObject( parent ), m_id( peer_id ), m_fileInfo( 0, FileInfo::Upload ), m_socket(), m_file(), m_state( FileTransferPeer::Unknown ),
    m_bytesTransferred( 0 ), m_totalBytesTransferred( 0 )
{
#if defined( BEEBEEP_DEBUG )
  qDebug() << "Peer" << m_id << "created for file" << m_fileInfo.name();
#endif
  connect( &m_socket, SIGNAL( error( QAbstractSocket::SocketError ) ), this, SLOT( socketError( QAbstractSocket::SocketError ) ) );
  connect( &m_socket, SIGNAL( dataReceived( const QByteArray& ) ), this, SLOT( checkData( const QByteArray& ) ) );
  connect( &m_socket, SIGNAL( userAuthenticated() ), this, SIGNAL( userAuthenticated() ) );
}

void FileTransferPeer::cancelTransfer()
{
#if defined( BEEBEEP_DEBUG )
  qDebug() << m_fileInfo.name() << "transfer cancelled";
#endif
  m_state = FileTransferPeer::Cancelled;
  emit message( m_socket.user(), m_fileInfo, tr( "transfer cancelled" ) );
  closeAll();
}

void FileTransferPeer::closeAll()
{
#if defined( BEEBEEP_DEBUG )
  qDebug() << "Making cleanup";
#endif
  if( m_socket.isOpen() )
  {
#if defined( BEEBEEP_DEBUG )
    qDebug() << "Closing socket";
#endif
    m_socket.flush();
    m_socket.close();
  }
  if( m_file.isOpen() )
  {
#if defined( BEEBEEP_DEBUG )
    qDebug() << "Closing file";
#endif
    m_file.flush();
    m_file.close();
  }
  emit transferFinished();
}

void FileTransferPeer::setFileInfo( const FileInfo& fi )
{
  m_fileInfo = fi;
#if defined( BEEBEEP_DEBUG )
  qDebug() << "Init the file" << m_file.fileName();
#endif
  m_file.setFileName( m_fileInfo.path() );
}

void FileTransferPeer::setConnectionDescriptor( int socket_descriptor )
{
#if defined( BEEBEEP_DEBUG )
  qDebug() << "Setup connection with socket" << socket_descriptor;
#endif
  m_state = FileTransferPeer::Request;
  m_bytesTransferred = 0;
  m_totalBytesTransferred = 0;

  if( socket_descriptor )
  {
    m_socket.setSocketDescriptor( socket_descriptor );
#if defined( BEEBEEP_DEBUG )
    qDebug() << "Using socket descriptor" << socket_descriptor;
#endif
  }
  else
  {
    m_socket.connectToHost( m_fileInfo.hostAddress(), m_fileInfo.hostPort() );
#if defined( BEEBEEP_DEBUG )
    qDebug() << "Connecting to" << m_fileInfo.hostAddress().toString() << ":" << m_fileInfo.hostPort();
#endif
  }
}

void FileTransferPeer::setTransferCompleted()
{
#if defined( BEEBEEP_DEBUG )
  qDebug() << m_fileInfo.name() << "transfer completed";
#endif
  m_state = FileTransferPeer::Completed;
  emit message( m_socket.user(), m_fileInfo, tr( "transfer completed" ) );
  closeAll();
}

void FileTransferPeer::socketError( QAbstractSocket::SocketError se )
{
#if defined( BEEBEEP_DEBUG )
  qDebug() << "Connection error id" << se << ":" << m_socket.errorString();
#endif
  setError( tr( "connection error (%1 - %2)" ).arg( QString::number( se ) ).arg( m_socket.errorString()) );
}

void FileTransferPeer::setError( const QString& str_err )
{
  m_state = FileTransferPeer::Error;
  QString s = tr( "transfer error (%1)" ).arg( str_err );
  qWarning() << s;
  emit message( m_socket.user(), m_fileInfo, str_err );
  closeAll();
}

void FileTransferPeer::showProgress()
{
  emit progress( m_socket.user(), m_fileInfo, m_totalBytesTransferred );
}
