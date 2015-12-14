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
#include "Settings.h"


void FileTransferPeer::checkUploadData( const QByteArray& byte_array )
{
  switch( m_state )
  {
  case FileTransferPeer::Request:
    checkUploadRequest( byte_array );
    break;
  case FileTransferPeer::Transferring:
    checkUploading( byte_array );
    break;
  default:
    // discard data
    qWarning() << name() << "discards data:" << byte_array;
  }
}

void FileTransferPeer::checkUploadRequest( const QByteArray& byte_array )
{
  Message m = Protocol::instance().toMessage( byte_array );
  if( !m.isValid() )
  {
    qWarning() << name() << "receives an invalid file request:" << byte_array;
    cancelTransfer();
    return;
  }

  FileInfo file_info = Protocol::instance().fileInfoFromMessage( m );
  if( !file_info.isValid() )
  {
    qWarning() << name() << "receives an invalid file info:" << byte_array;
    cancelTransfer();
    return;
  }

  qDebug() << name() << "receives a file request:" << file_info.id() << file_info.password();
  emit fileUploadRequest( file_info.id(), file_info.password() );
}

void FileTransferPeer::startUpload( const FileInfo& fi )
{
  setFileInfo( fi );
  qDebug() << name() << "starts uploading" << fi.path();
  if( m_socket.protoVersion() < 63 )
  {
    m_state = FileTransferPeer::Transferring;
    sendUploadData();
  }
  else
  {
    m_state = FileTransferPeer::FileSizeHeader;
    sendFileSizeHeader();
  }
}

void FileTransferPeer::sendFileSizeHeader()
{
#ifdef BEEBEEP_DEBUG
  qDebug() << name() << "sends File Size Header for" << m_fileInfo.path();
#endif
  m_bytesTransferred = 0;
  m_state = FileTransferPeer::Transferring;

  QFileInfo file_info_now_in_system( m_fileInfo.path() );
  if( file_info_now_in_system.exists() )
    m_fileInfo.setSize( file_info_now_in_system.size() );

  QByteArray file_size_header = QString::number( m_fileInfo.size() ).toLatin1();
  if( !m_socket.sendData( file_size_header ) )
    setError( tr( "unable to send file header" ) );
}

void FileTransferPeer::checkUploading( const QByteArray& byte_array )
{
  if( byte_array.simplified().toInt() == m_bytesTransferred )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << name() << "receives corfirmation for" << m_bytesTransferred << "bytes";
#endif
    m_totalBytesTransferred += m_bytesTransferred;

    showProgress();

    if( m_totalBytesTransferred > m_fileInfo.size() )
      setError( tr( "%1 bytes uploaded but the file size is only %2 bytes" ).arg( m_totalBytesTransferred ).arg( m_fileInfo.size() ) );
    else if( m_totalBytesTransferred == m_fileInfo.size() )
      setTransferCompleted();
    else
      sendTransferData();
  }
  else
    setError( tr( "%1 bytes sent not confirmed (%2 bytes confirmed)").arg( m_bytesTransferred ).arg( byte_array.toInt() ) );
}

void FileTransferPeer::sendUploadData()
{
  if( m_state != FileTransferPeer::Transferring )
  {
    qWarning() << name() << "tries to send data, but it was in state" << m_state << "... skipped";
    return;
  }

  if( !m_file.isOpen() )
  {
    if( !m_file.open( QIODevice::ReadOnly ) )
    {
      setError( "Unable to open file" );
      return;
    }
  }

  if( m_file.atEnd() )
    return;

  QByteArray byte_array = m_file.read( m_socket.fileTransferBufferSize() );

  if( m_socket.sendData( byte_array ) )
  {
    m_bytesTransferred = byte_array.size();
  }
  else
  {
    setError( tr( "Unable to upload data" ) );
    return;
  }
}
