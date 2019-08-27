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
#include "UserManager.h"


FileTransferPeer::FileTransferPeer( QObject *parent )
  : QObject( parent ), m_transferType( FileInfo::Upload ), m_id( ID_INVALID ),
    m_fileInfo( ID_INVALID, FileInfo::Upload ), m_file(), m_state( FileTransferPeer::Unknown ),
    m_bytesTransferred( 0 ), m_totalBytesTransferred( 0 ), mp_socket( Q_NULLPTR ),
    m_socketDescriptor( 0 ), m_remoteUserId( ID_INVALID ), m_serverPort( 0 ), m_elapsedTime( 0 )
{
  setObjectName( "FileTransferPeer" );
#ifdef BEEBEEP_DEBUG
  qDebug() << "Peer created for transfer file";
#endif
  mp_socket = new ConnectionSocket( this );
  connect( mp_socket, SIGNAL( error( QAbstractSocket::SocketError ) ), this, SLOT( socketError( QAbstractSocket::SocketError ) ) );
  connect( mp_socket, SIGNAL( authenticationRequested( const QByteArray& ) ), this, SLOT( checkUserAuthentication( const QByteArray& ) ) );
  connect( mp_socket, SIGNAL( dataReceived( const QByteArray& ) ), this, SLOT( checkTransferData( const QByteArray& ) ) );
  connect( mp_socket, SIGNAL( abortRequest() ), this, SLOT( cancelTransfer() ) );
}

void FileTransferPeer::cancelTransfer()
{
  if( m_state == FileTransferPeer::Cancelled )
    return;
  if( m_state != FileTransferPeer::Completed )
    m_state = FileTransferPeer::Cancelled;
  qDebug() << qPrintable( name() ) << "cancels the transfer";
  if( mp_socket->isOpen() )
    mp_socket->abortConnection();
  closeAll();
  if( m_fileInfo.isValid() && remoteUserId() != ID_INVALID )
    emit message( id(), remoteUserId(), m_fileInfo, tr( "Transfer cancelled" ) );
  emit operationCompleted();
}

void FileTransferPeer::closeAll()
{
#ifdef BEEBEEP_DEBUG
  qDebug() << qPrintable( name() ) << "cleans up";
#endif
  if( mp_socket->isOpen() )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << qPrintable( name() ) << "close socket with descriptor" << mp_socket->socketDescriptor();
#endif
    mp_socket->closeConnection();
  }

  if( m_file.isOpen() )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << qPrintable( name() ) << "close file" << qPrintable( Bee::convertToNativeFolderSeparator( m_file.fileName() ) );
#endif
    m_file.flush();
    m_file.close();
  }

  if( !isTransferCompleted() && isDownload() && m_file.exists() )
    m_file.remove();

  computeElapsedTime();
}

void FileTransferPeer::setFileInfo( FileInfo::TransferType ftt, const FileInfo& fi )
{
  m_fileInfo = fi;
  m_fileInfo.setTransferType( ftt );
  m_file.setFileName( m_fileInfo.path() );
#ifdef BEEBEEP_DEBUG
  qDebug() << qPrintable( name() ) << "init the file" << qPrintable( Bee::convertToNativeFolderSeparator( m_fileInfo.path() ) );
#endif
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
  m_elapsedTime = 0;
  m_startTimestamp = QDateTime::currentDateTime();

  if( m_socketDescriptor > 0 )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << qPrintable( name() ) << "set socket descriptor" << m_socketDescriptor;
#endif
    mp_socket->initSocket( m_socketDescriptor, m_serverPort );
  }
  else
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << qPrintable( name() ) << "is connecting to" << qPrintable( m_fileInfo.networkAddress().toString() );
#endif
    mp_socket->connectToNetworkAddress( m_fileInfo.networkAddress() );
  }

  QTimer::singleShot( Settings::instance().fileTransferConfirmTimeout(), this, SLOT( connectionTimeout() ) );
}

void FileTransferPeer::setTransferCompleted()
{
  qDebug() << qPrintable( name() ) << "has completed the transfer of file" << qPrintable( Bee::convertToNativeFolderSeparator( m_fileInfo.name() ) ) << "with user id" << remoteUserId();
  m_state = FileTransferPeer::Completed;
  closeAll();
  if( isDownload() && m_fileInfo.lastModified().isValid() )
    Bee::setLastModifiedToFile( m_fileInfo.path(), m_fileInfo.lastModified() );
  emit message( id(), remoteUserId(), m_fileInfo, tr( "Transfer completed in %1" ).arg( Bee::timeToString( m_elapsedTime ) ) );
  emit completed( id(), remoteUserId(), m_fileInfo );
  emit operationCompleted();
}

void FileTransferPeer::socketError( QAbstractSocket::SocketError )
{
  // Make a check to remove the error after a transfer completed
  if( m_state <= FileTransferPeer::Transferring )
    setError( mp_socket->errorString() );
}

void FileTransferPeer::setError( const QString& str_err )
{
  m_state = FileTransferPeer::Error;
  qWarning() << qPrintable( name() ) << "found an error when transfer file" << qPrintable( Bee::convertToNativeFolderSeparator( m_fileInfo.name() ) ) << ":" << str_err;
  closeAll();
  if( remoteUserId() != ID_INVALID && m_fileInfo.isValid() )
    emit message( id(), remoteUserId(), m_fileInfo, str_err );
  operationCompleted();
}

void FileTransferPeer::showProgress()
{
  if( m_totalBytesTransferred > 0 )
  {
    computeElapsedTime();
#ifdef BEEBEEP_DEBUG
    qWarning() << qPrintable( name() ) << "has elapsed ms:" << m_elapsedTime;
#endif
    emit progress( id(), remoteUserId(), m_fileInfo, m_totalBytesTransferred, m_elapsedTime );
  }
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

void FileTransferPeer::checkUserAuthentication( const QByteArray& auth_byte_array )
{
#ifdef BEEBEEP_DEBUG
  qDebug() << qPrintable( name() ) << "checks authentication message";
#endif
  Message m = Protocol::instance().toMessage( auth_byte_array, mp_socket->protoVersion() );
  if( !m.isValid() )
  {
    qWarning() << qPrintable( name() ) << "has found an invalid auth message";
    cancelTransfer();
    return;
  }

  User user_to_check = Protocol::instance().createUser( m, mp_socket->peerAddress() );
  User user_connected;
  if( user_to_check.isValid() )
    user_connected = Protocol::instance().recognizeUser( user_to_check, Settings::instance().userRecognitionMethod() );

  if( !user_connected.isValid() )
  {
    qWarning() << qPrintable( user_to_check.path() ) << "is not authorized for file transfer" << id();
    cancelTransfer();
    return;
  }

  setUserAuthorized( user_connected.id() );
}

void FileTransferPeer::setUserAuthorized( VNumber user_id )
{
  mp_socket->setUserId( user_id );
  if( m_remoteUserId == ID_INVALID )
    m_remoteUserId = user_id;
  if( user_id != m_remoteUserId )
    qWarning() << qPrintable( name() ) << "was for user id" << m_remoteUserId << "but it is authorized also user id" << user_id;
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
    if( mp_socket->activityIdle() > Settings::instance().pongTimeout() )
      setError( tr( "Transfer timeout" ) );
  }
}

void FileTransferPeer::computeElapsedTime()
{
  if( m_startTimestamp.isValid() )
  {
    qint64 elapsed_time_ms = qAbs( QDateTime::currentDateTime().msecsTo( m_startTimestamp ) );
    if( elapsed_time_ms >= 86399999 ) // no more than 1 day - 1 ms
      m_elapsedTime = 86399999;
    else
      m_elapsedTime = static_cast<int>( elapsed_time_ms );
  }
  else
    m_elapsedTime = 0;
}
