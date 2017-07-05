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
    qWarning() << qPrintable( name() ) << "discards data:" << byte_array;
  }
}

void FileTransferPeer::checkUploadRequest( const QByteArray& byte_array )
{
  Message m = Protocol::instance().toMessage( byte_array, m_socket.protoVersion() );
  if( !m.isValid() )
  {
    qWarning() << qPrintable( name() ) << "receives an invalid file request:" << byte_array;
    cancelTransfer();
    return;
  }

  FileInfo file_info = Protocol::instance().fileInfoFromMessage( m );
  if( !file_info.isValid() )
  {
    qWarning() << qPrintable( name() ) << "receives an invalid file info:" << byte_array;
    cancelTransfer();
    return;
  }

  qDebug() << qPrintable( name() ) << "receives a file request:" << file_info.id() << file_info.password();
  emit fileUploadRequest( file_info );
}

void FileTransferPeer::startUpload( const FileInfo& fi )
{
  setTransferType( FileInfo::Upload );
  setFileInfo( FileInfo::Upload, fi );
  qDebug() << qPrintable( name() ) << "starts uploading" << qPrintable( Bee::convertToNativeFolderSeparator( fi.path() ) );
  if( m_socket.protoVersion() < FILE_TRANSFER_2_PROTO_VERSION )
  {
    qWarning() << qPrintable( name() ) << "using an old file upload protocol version" << m_socket.protoVersion();
    m_state = FileTransferPeer::Transferring;
    sendUploadData();
  }
  else
  {
    m_state = FileTransferPeer::FileHeader;
    sendFileHeader();
  }
}

void FileTransferPeer::sendFileHeader()
{
#ifdef BEEBEEP_DEBUG
  qDebug() << qPrintable( name() ) << "sends File Size Header for" << m_fileInfo.path();
#endif
  m_bytesTransferred = 0;
  m_state = FileTransferPeer::Transferring;

  QFileInfo file_info_now_in_system( m_fileInfo.path() );
  if( file_info_now_in_system.exists() )
  {
    m_fileInfo.setSize( file_info_now_in_system.size() );
    m_fileInfo.setLastModified( file_info_now_in_system.lastModified() );
  }

  Message file_header_message = Protocol::instance().fileInfoToMessage( m_fileInfo );
  QByteArray file_header = Protocol::instance().fromMessage( file_header_message, m_socket.protoVersion() );

  if( !m_socket.sendData( file_header ) )
    setError( tr( "unable to send file header" ) );
}

void FileTransferPeer::checkUploading( const QByteArray& byte_array )
{
  if( byte_array.simplified().toInt() == m_bytesTransferred )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << qPrintable( name() ) << "receives corfirmation for" << m_bytesTransferred << "bytes";
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
    qWarning() << qPrintable( name() ) << "tries to send data, but it was in state" << m_state << "... skipped";
    return;
  }

  if( !m_file.isOpen() )
  {
    if( !m_file.open( QIODevice::ReadOnly ) )
    {
      setError( tr( "Unable to open file %1" ).arg( m_file.fileName() ) );
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
