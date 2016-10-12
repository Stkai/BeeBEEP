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

#include "BeeUtils.h"
#include "FileTransferPeer.h"
#include "Protocol.h"
#include "Settings.h"


FileTransferPeer::FileTransferPeer( QObject *parent )
  : QObject( parent ), m_transferType( FileInfo::Upload ), m_id( ID_INVALID ),
    m_fileInfo( 0, FileInfo::Upload ), m_file(), m_state( FileTransferPeer::Unknown ),
    m_bytesTransferred( 0 ), m_totalBytesTransferred( 0 ), m_socket( parent ), m_messageAuth(),
    m_time( QTime::currentTime() ), m_socketDescriptor( 0 )
{
  setObjectName( "FileTransferPeer" );
  qDebug() << "Peer created for transfer file";

  connect( &m_socket, SIGNAL( error( QAbstractSocket::SocketError ) ), this, SLOT( socketError( QAbstractSocket::SocketError ) ) );
  connect( &m_socket, SIGNAL( authenticationRequested( const Message& ) ), this, SLOT( checkAuthenticationRequested( const Message& ) ) );
  connect( &m_socket, SIGNAL( dataReceived( const QByteArray& ) ), this, SLOT( checkTransferData( const QByteArray& ) ) );
  connect( &m_socket, SIGNAL( tickEvent( int ) ), this, SLOT( onTickEvent( int ) ) );
}

void FileTransferPeer::cancelTransfer()
{
  qDebug() << qPrintable( name() ) << "cancels the transfer";
  if( m_socket.isOpen() )
    m_socket.abortConnection();
  m_state = FileTransferPeer::Cancelled;
  closeAll();
  emit message( id(), userId(), m_fileInfo, tr( "Transfer cancelled" ) );
  deleteLater();
}

void FileTransferPeer::closeAll()
{
  qDebug() << qPrintable( name() ) << "cleans up";
  if( m_socket.isOpen() )
  {
    qDebug() << qPrintable( name() ) << "close socket with descriptor" << m_socket.socketDescriptor();
    m_socket.closeConnection();
  }

  if( m_file.isOpen() )
  {
    qDebug() << qPrintable( name() ) << "close file" << qPrintable( Bee::convertToNativeFolderSeparator( m_file.fileName() ) );
    m_file.flush();
    m_file.close();
  }
}

void FileTransferPeer::setFileInfo( FileInfo::TransferType ftt, const FileInfo& fi )
{
  m_fileInfo = fi;
  m_fileInfo.setTransferType( ftt );
  m_file.setFileName( m_fileInfo.path() );
  qDebug() << qPrintable( name() ) << "init the file" << qPrintable( Bee::convertToNativeFolderSeparator( m_fileInfo.path() ) );
}

void FileTransferPeer::startConnection()
{
  if( m_state >= FileTransferPeer::Request )
  {
    qDebug() << qPrintable( name() ) << "is already started and it is in state" << m_state;
    return;
  }

  m_state = FileTransferPeer::Request;
  m_bytesTransferred = 0;
  m_totalBytesTransferred = 0;

  m_time.start();

  if( m_socketDescriptor )
  {
    qDebug() << qPrintable( name() ) << "set socket descriptor" << m_socketDescriptor;
    m_socket.initSocket( m_socketDescriptor );
  }
  else
  {
    qDebug() << qPrintable( name() ) << "is connecting to" << qPrintable( m_fileInfo.networkAddress().toString() );
    m_socket.connectToNetworkAddress( m_fileInfo.networkAddress() );
  }

  QTimer::singleShot( Settings::instance().fileTransferConfirmTimeout(), this, SLOT( connectionTimeout() ) );
}

void FileTransferPeer::setTransferCompleted()
{
  qDebug() << qPrintable( name() ) << "has completed the transfer of file" << qPrintable( Bee::convertToNativeFolderSeparator( m_fileInfo.name() ) );
  m_state = FileTransferPeer::Completed;
  closeAll();
  if( isDownload() && m_fileInfo.lastModified().isValid() )
    Bee::setLastModifiedToFile( m_fileInfo.path(), m_fileInfo.lastModified() );
  emit message( id(), userId(), m_fileInfo, tr( "Transfer completed in %1" ).arg( Bee::elapsedTimeToString( m_time.elapsed() ) ) );
  emit completed( id(), userId(), m_fileInfo );
  deleteLater();
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
  qWarning() << qPrintable( name() ) << "found an error when transfer file" << qPrintable( Bee::convertToNativeFolderSeparator( m_fileInfo.name() ) ) << ":" << str_err;
  closeAll();
  emit message( id(), userId(), m_fileInfo, str_err );
  deleteLater();
}

void FileTransferPeer::showProgress()
{
  if( m_totalBytesTransferred > 0 )
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
  if( isDownload() )
    sendDownloadRequest();
}

void FileTransferPeer::connectionTimeout()
{
  if( m_state <= FileTransferPeer::Request )
    setError( tr( "Connection timeout" ) );
}

void FileTransferPeer::onTickEvent( int )
{
  if( m_state == FileTransferPeer::Transferring )
  {
    if( m_socket.activityIdle() > Settings::instance().pongTimeout() )
      setError( tr( "Transfer timeout" ) );
  }
}
