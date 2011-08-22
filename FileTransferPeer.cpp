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
#include "Protocol.h"


FileTransferPeer::FileTransferPeer( VNumber peer_id, QObject *parent )
  : QObject( parent ), m_id( peer_id ), m_fileInfo( 0, FileInfo::Upload ), m_socket(), m_file(), m_state( FileTransferPeer::Unknown ),
    m_bytesTransferred( 0 ), m_totalBytesTransferred( 0 )
{
  qDebug() << "Peer" << m_id << "created for transfer file";
  connect( &m_socket, SIGNAL( error( QAbstractSocket::SocketError ) ), this, SLOT( socketError( QAbstractSocket::SocketError ) ) );
  connect( &m_socket, SIGNAL( dataReceived( const QByteArray& ) ), this, SLOT( checkData( const QByteArray& ) ) );
  connect( &m_socket, SIGNAL( authenticationRequested( const Message& ) ), this, SLOT( checkAuthentication( const Message& ) ) );
}

void FileTransferPeer::cancelTransfer()
{
  qDebug() << m_fileInfo.name() << "transfer cancelled";
  m_state = FileTransferPeer::Cancelled;
  emit message( userId(), m_fileInfo, tr( "transfer cancelled" ) );
  closeAll();
}

void FileTransferPeer::closeAll()
{
  qDebug() << "Making cleanup of peer" << m_id;
  if( m_socket.isOpen() )
  {
    qDebug() << "Closing socket";
    m_socket.flush();
    m_socket.close();
  }
  if( m_file.isOpen() )
  {
    qDebug() << "Closing file";
    m_file.flush();
    m_file.close();
  }
  emit transferFinished();
}

void FileTransferPeer::setFileInfo( const FileInfo& fi )
{
  m_fileInfo = fi;
  m_file.setFileName( m_fileInfo.path() );
  qDebug() << "Init the file" << m_file.fileName();
}

void FileTransferPeer::setConnectionDescriptor( int socket_descriptor )
{
  qDebug() << "Setup connection with socket" << socket_descriptor;
  m_state = FileTransferPeer::Request;
  m_bytesTransferred = 0;
  m_totalBytesTransferred = 0;

  if( socket_descriptor )
  {
    m_socket.setSocketDescriptor( socket_descriptor );
    qDebug() << "Using socket descriptor" << socket_descriptor;
  }
  else
  {
    m_socket.connectToHost( m_fileInfo.hostAddress(), m_fileInfo.hostPort() );
    qDebug() << "Connecting to" << m_fileInfo.hostAddress().toString() << ":" << m_fileInfo.hostPort();
  }
}

void FileTransferPeer::setTransferCompleted()
{
  qDebug() << m_fileInfo.name() << "transfer completed";
  m_state = FileTransferPeer::Completed;
  emit message( userId(), m_fileInfo, tr( "transfer completed" ) );
  closeAll();
}

void FileTransferPeer::socketError( QAbstractSocket::SocketError se )
{
  qDebug() << "Connection error id" << se << ":" << m_socket.errorString();
  setError( tr( "connection error (%1 - %2)" ).arg( QString::number( se ) ).arg( m_socket.errorString()) );
}

void FileTransferPeer::setError( const QString& str_err )
{
  m_state = FileTransferPeer::Error;
  QString s = tr( "transfer error (%1)" ).arg( str_err );
  qWarning() << s;
  emit message( userId(), m_fileInfo, str_err );
  closeAll();
}

void FileTransferPeer::showProgress()
{
  emit progress( userId(), m_fileInfo, m_totalBytesTransferred );
}

void FileTransferPeer::checkAuthentication( const Message& m )
{
  qDebug() << "File transfer is checking the authentication";
  User u = Protocol::instance().createUser( m, m_socket.peerAddress() );
  if( !u.isValid() )
  {
    qWarning() << "Unable to create a new user from the message arrived from:" << m_socket.peerAddress().toString() << m_socket.peerPort();
    m_socket.abort();
    closeAll();
    return;
  }

  m_socket.setUserId( u.id() );
  qDebug() << "FileTransfer has received connection (must be validated) from" << u.path();
  emit userConnected( u );
}

void FileTransferPeer::setUserAuthenticated( const User& u )
{
  if( !u.isValid() )
  {
    qWarning() << "User" << m_socket.userId() << "is not authenticated. File transfer aborted";
    m_socket.abort();
    closeAll();
    return;
  }

  m_socket.setUserId( u.id() );
  qDebug() << "File transfer has confirmed the user" << u.path();
  m_socket.setUserAuthenticated( true );
  qDebug() << "Authentication completed";
  emit userAuthenticated();
}
