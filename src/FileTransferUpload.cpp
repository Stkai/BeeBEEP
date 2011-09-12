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
    qWarning() << "Upload Peer discard data:" << byte_array;
  }
}

void FileTransferPeer::checkUploadRequest( const QByteArray& byte_array )
{
  Message m = Protocol::instance().toMessage( byte_array );
  if( !m.isValid() )
  {
    qDebug() << "Invalid file request received:" << byte_array;
    cancelTransfer();
    return;
  }

  FileInfo file_info = Protocol::instance().fileInfoFromMessage( m );
  if( !file_info.isValid() )
  {
    qDebug() << "Invalid file info in message:" << byte_array;

    cancelTransfer();
    return;
  }

  qDebug() << "File request received:" << file_info.id() << file_info.password();
  emit fileUploadRequest( file_info.id(), file_info.password() );
}

void FileTransferPeer::startUpload( const FileInfo& fi )
{
  setFileInfo( fi );
  qDebug() << "Uploading" << fi.path();
  m_state = FileTransferPeer::Transferring;
  sendUploadData();
}

void FileTransferPeer::checkUploading( const QByteArray& byte_array )
{
  if( byte_array.toInt() == m_bytesTransferred )
  {
    qDebug() << m_fileInfo.name() << ":" << m_bytesTransferred << "bytes sent confirmed";
    m_totalBytesTransferred += m_bytesTransferred;
    showProgress();
    if( m_totalBytesTransferred == m_fileInfo.size() )
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
    qWarning() << m_fileInfo.name() << ": try to send data, but it id in state" << m_state;
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

  QByteArray byte_array = m_file.read( Settings::instance().fileTransferBufferSize() );

  if( m_socket.sendData( byte_array ) )
  {
    m_bytesTransferred = byte_array.size();
    qDebug() << m_fileInfo.name() << ":" << m_bytesTransferred << "bytes sent";
  }
  else
  {
    setError( tr( "Unable to upload data" ) );
    return;
  }
}
