//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2021 Marco Mastroddi
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
// along with BeeBEEP. If not, see <http://www.gnu.org/licenses/>.
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
  case FileTransferPeer::Pausing:
    checkUploading( byte_array );
    break;
  default:
    // discard data
    qWarning() << qPrintable( name() ) << "discards data:" << byte_array;
  }
}

void FileTransferPeer::checkUploadRequest( const QByteArray& byte_array )
{
  Message m = Protocol::instance().toMessage( byte_array, mp_socket->protocolVersion() );
  if( !m.isValid() )
  {
    qWarning() << qPrintable( name() ) << "receives an invalid file request:" << byte_array;
    cancelTransfer();
    return;
  }

  FileInfo file_info = Protocol::instance().fileInfoFromMessage( m, mp_socket->protocolVersion() );
  if( !file_info.isValid() )
  {
    qWarning() << qPrintable( name() ) << "receives an invalid file info:" << byte_array;
    cancelTransfer();
    return;
  }
#ifdef BEEBEEP_DEBUG
  qDebug() << qPrintable( name() ) << "receives a file request:" << file_info.name() << file_info.id() << file_info.password();
#else
  qDebug() << qPrintable( name() ) << "receives a file request:" << file_info.name();
#endif
  if( !file_info.name().endsWith( file_info.suffix(), Qt::CaseInsensitive ) )
  {
    qWarning() << qPrintable( name() ) << "receives an invalid file info with name" << qPrintable( file_info.name() ) << "and extension" << file_info.suffix();
    cancelTransfer();
    return;
  }

  emit fileUploadRequest( file_info );
}

void FileTransferPeer::startUpload( const FileInfo& fi )
{
  setTransferType( FileInfo::Upload );
  setFileInfo( FileInfo::Upload, fi );
  qDebug() << qPrintable( name() ) << "starts uploading" << qPrintable( fi.path() ) << "from" << fi.startingPosition() << "to" << fi.size() << "bytes";
  if( mp_socket->protocolVersion() < FILE_TRANSFER_2_PROTO_VERSION )
  {
    qWarning() << qPrintable( name() ) << "using an old file upload protocol version" << mp_socket->protocolVersion();
    setTransferringState();
    sendUploadData();
  }
  else
    sendFileHeader();
}

void FileTransferPeer::sendFileHeader()
{
  if( m_state == FileTransferPeer::FileHeader )
    return;
  m_state = FileTransferPeer::FileHeader;
#ifdef BEEBEEP_DEBUG
  qDebug() << qPrintable( name() ) << "sends File Header for" << m_fileInfo.path();
#endif
  m_bytesTransferred = 0;
  m_totalBytesTransferred = 0;

  QFileInfo file_info_now_in_system( m_fileInfo.path() );
  if( file_info_now_in_system.exists() )
  {
    m_fileInfo.setSize( file_info_now_in_system.size() );
    m_fileInfo.setLastModified( file_info_now_in_system.lastModified() );
    if( m_fileInfo.startingPosition() > 0 )
    {
      if( m_fileInfo.startingPosition() > m_fileInfo.size() )
        m_fileInfo.setStartingPosition( 0 );
      m_bytesTransferred = m_fileInfo.startingPosition();
      m_isSkipped = m_bytesTransferred == m_fileInfo.size();
    }
  }
  else
  {
    setError( tr( "file no longer exists" ) );
    return;
  }

  Message file_header_message = Protocol::instance().fileInfoToMessage( m_fileInfo, mp_socket->protocolVersion() );
  QByteArray file_header = Protocol::instance().fromMessage( file_header_message, mp_socket->protocolVersion() );

  if( !mp_socket->sendData( file_header ) )
    setError( tr( "unable to send file header" ) );
  else
    setTransferringState();
}

void FileTransferPeer::checkUploading( const QByteArray& byte_array )
{
  FileSizeType bytes_arrived = 0;
  FileSizeType total_bytes = 0;
  bool pause_transfer = false;
  if( !Protocol::instance().parseFileTransferBytesArrivedConfirmation( mp_socket->protocolVersion(), byte_array, &bytes_arrived, &total_bytes, &pause_transfer ) )
  {
    setError( tr( "remote host sent invalid data" ) );
    return;
  }

  if( bytes_arrived <= 0 && pause_transfer )
  {
    setTransferPaused();
  }
  else if( bytes_arrived == m_bytesTransferred )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << qPrintable( name() ) << "receives corfirmation for" << m_bytesTransferred << "bytes";
#endif
    m_totalBytesTransferred += m_bytesTransferred;

    showProgress();

    if( total_bytes > 0 && m_totalBytesTransferred != total_bytes )
      setError( tr( "%1 bytes uploaded but the remote file size is %2 bytes" ).arg( m_totalBytesTransferred ).arg( total_bytes ) );
    else if( m_totalBytesTransferred > m_fileInfo.size() )
      setError( tr( "%1 bytes uploaded but the file size is only %2 bytes" ).arg( m_totalBytesTransferred ).arg( m_fileInfo.size() ) );
    else if( m_totalBytesTransferred == m_fileInfo.size() )
      setTransferCompleted();
    else if( pause_transfer )
      setTransferPaused();
    else
      sendTransferData();
  }
  else
    setError( tr( "%1 bytes sent not confirmed (%2 bytes confirmed)").arg( m_bytesTransferred ).arg( bytes_arrived ) );
}

void FileTransferPeer::sendUploadData()
{
  if( m_state == FileTransferPeer::Paused || m_state == FileTransferPeer::Pausing )
    return;

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

    if( m_totalBytesTransferred >= m_file.size() || !m_file.seek( m_totalBytesTransferred ) )
    {
      setError( tr( "Unable to seek %1 bytes in file %2" ).arg( m_totalBytesTransferred ).arg( m_file.fileName() ) );
      return;
    }
  }

  if( m_file.atEnd() )
    return;

  QByteArray byte_array = m_file.read( mp_socket->fileTransferBufferSize() );

  if( mp_socket->sendData( byte_array ) )
  {
    m_bytesTransferred = byte_array.size();
  }
  else
  {
    setError( tr( "Unable to upload data" ) );
    return;
  }
}
