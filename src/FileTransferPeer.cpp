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
#include "FileTransferPeer.h"
#include "Protocol.h"
#include "Settings.h"


FileTransferPeer::FileTransferPeer( QObject *parent )
  : QObject( parent ), m_transferType( FileTransferPeer::Upload ), m_id( ID_INVALID ), m_fileInfo( 0, FileInfo::Upload ), m_file(), m_state( FileTransferPeer::Unknown ),
    m_bytesTransferred( 0 ), m_totalBytesTransferred( 0 ), m_socket( parent ), m_messageAuth()
{
  setObjectName( "FileTransferPeer ");
  qDebug() << "Peer created for transfer file";

  m_time = QTime::currentTime();

  connect( &m_socket, SIGNAL( error( QAbstractSocket::SocketError ) ), this, SLOT( socketError( QAbstractSocket::SocketError ) ) );
  connect( &m_socket, SIGNAL( authenticationRequested( const Message& ) ), this, SLOT( checkAuthenticationRequested( const Message& ) ) );
  connect( &m_socket, SIGNAL( dataReceived( const QByteArray& ) ), this, SLOT( checkTransferData( const QByteArray& ) ) );
}

void FileTransferPeer::cancelTransfer()
{
  m_socket.abort();
  qDebug() << name() << "cancels the transfer";
  m_state = FileTransferPeer::Cancelled;
  emit message( id(), userId(), m_fileInfo, tr( "Transfer cancelled" ) );
  closeAll();
}

void FileTransferPeer::closeAll()
{
  qDebug() << name() << "cleans up";
  if( m_socket.isOpen() )
  {
    qDebug() << name() << "close socket with descriptor" << m_socket.socketDescriptor();
    m_socket.flush();
    m_socket.close();
  }
  if( m_file.isOpen() )
  {
    qDebug() << name() << "close file" << m_file.fileName();
    m_file.flush();
    m_file.close();
  }
  deleteLater();
}

void FileTransferPeer::setFileInfo( const FileInfo& fi )
{
  m_fileInfo = fi;
  m_file.setFileName( m_fileInfo.path() );
  qDebug() << name() << "init the file" << m_fileInfo.path();
}

void FileTransferPeer::setConnectionDescriptor( int socket_descriptor )
{
  m_state = FileTransferPeer::Request;
  m_bytesTransferred = 0;
  m_totalBytesTransferred = 0;

  m_time.start();

  if( socket_descriptor )
  {
    qDebug() << name() << "set socket descriptor" << socket_descriptor;
    m_socket.setSocketDescriptor( socket_descriptor );
  }
  else
  {
    qDebug() << name() << "is connecting to" << m_fileInfo.hostAddress().toString() << ":" << m_fileInfo.hostPort();
    m_socket.connectToHost( m_fileInfo.hostAddress(), m_fileInfo.hostPort() );
  }

  QTimer::singleShot( Settings::instance().fileTransferConfirmTimeout(), this, SLOT( connectionTimeout() ) );
}

void FileTransferPeer::setTransferCompleted()
{
  qDebug() << name() << "has completed the transfer of file" << m_fileInfo.name();
  m_state = FileTransferPeer::Completed;
  emit message( id(), userId(), m_fileInfo, tr( "Transfer completed in %1" ).arg( Bee::elapsedTimeToString( m_time.elapsed() ) ) );
  closeAll();
}

void FileTransferPeer::socketError( QAbstractSocket::SocketError )
{
  // Make a check to remove the error after a transfer completed
  if( m_state <= FileTransferPeer::Transferring )
    setError( m_socket.errorString() );
}

void FileTransferPeer::setError( const QString& str_err )
{
  m_state = FileTransferPeer::Error;
  qWarning() << name() << "found an error when transfer file" << m_fileInfo.name() << ":" << str_err;
  emit message( id(), userId(), m_fileInfo, str_err );
  closeAll();
}

void FileTransferPeer::showProgress()
{
  emit progress( id(), userId(), m_fileInfo, m_totalBytesTransferred );
}

void FileTransferPeer::checkTransferData( const QByteArray& byte_array )
{
  if( isDownload() )
    checkDownloadData( byte_array );
  else
    checkUploadData( byte_array );
}

void FileTransferPeer::sendTransferData()
{
  if( isDownload() )
    sendDownloadData();
  else
    sendUploadData();
}

void FileTransferPeer::checkAuthenticationRequested( const Message& m )
{
  m_messageAuth = m;
  emit authenticationRequested();
}

void FileTransferPeer::setUserAuthorized( VNumber user_id )
{
  m_socket.setUserId( user_id );
}

void FileTransferPeer::connectionTimeout()
{
  if( m_state <= FileTransferPeer::Request )
    setError( tr( "Connection timeout" ) );
}
