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

#include "FileTransferPeer.h"
#include "Protocol.h"


void FileTransferPeer::sendDownloadData()
{
  if( m_state == FileTransferPeer::Transferring )
    sendDownloadDataConfirmation();
  else
    qWarning() << qPrintable( name() ) << "tries to send download data but it is in invalid state";
}

void FileTransferPeer::sendDownloadRequest()
{
  qDebug() << qPrintable( name() ) << "sending file request:" << m_fileInfo.id() << m_fileInfo.password();
  if( mp_socket->sendData( Protocol::instance().fromMessage( Protocol::instance().fileInfoToMessage( m_fileInfo ), mp_socket->protoVersion() ) ) )
  {
    if( mp_socket->protoVersion() < FILE_TRANSFER_2_PROTO_VERSION )
    {
      m_state = FileTransferPeer::Transferring;
      qWarning() << qPrintable( name() ) << "using an old file download protocol version" << mp_socket->protoVersion();
    }
    else
      m_state = FileTransferPeer::FileHeader;
  }
  else
    cancelTransfer();
}

void FileTransferPeer::sendDownloadDataConfirmation()
{
#ifdef BEEBEEP_DEBUG
  qDebug() << qPrintable( name() ) << "sending corfirmation for" << m_bytesTransferred << "bytes";
#endif
  if( !mp_socket->sendData( Protocol::instance().bytesArrivedConfirmation( m_bytesTransferred ) ) )
    cancelTransfer();
}

void FileTransferPeer::checkDownloadData( const QByteArray& byte_array )
{
  if( m_state == FileTransferPeer::Request )
  {
    // after the authentication we receive "HELLO" from the other peer. Otherwise the connection is aborted.
    // Skip HELLO, but we will send file request;
    sendTransferData();
    return;
  }

  if( m_state == FileTransferPeer::FileHeader )
  {
    Message file_header_message = Protocol::instance().toMessage( byte_array, mp_socket->protoVersion() );
    if( !file_header_message.isValid() )
    {
      setError( tr( "invalid file header" ) );
      return;
    }

    FileInfo file_header = Protocol::instance().fileInfoFromMessage( file_header_message );

    m_fileInfo.setSize( file_header.size() );
    if( file_header.lastModified().isValid() )
      m_fileInfo.setLastModified( file_header.lastModified() );
    m_state = FileTransferPeer::Transferring;
    m_bytesTransferred = 0;
    sendTransferData();
    return;
  }

  if( m_state != FileTransferPeer::Transferring )
  {
    qWarning() << qPrintable( name() ) << "tries to check data with invalid state" << m_state;
    return;
  }

  m_bytesTransferred = byte_array.size();
  m_totalBytesTransferred += static_cast<FileSizeType>(m_bytesTransferred);

  sendTransferData(); // send to upload client that data is arrived

  if( !m_file.isOpen() )
  {
    if( !m_file.open( QIODevice::WriteOnly ) )
    {
      setError( tr( "Unable to open file %1" ).arg( m_file.fileName() ) );
      return;
    }
  }

  if( m_file.write( byte_array ) != m_bytesTransferred )
  {
    setError( tr( "Unable to write in the file %1" ).arg( m_file.fileName() ) );
    return;
  }

  showProgress();

  if( m_totalBytesTransferred > m_fileInfo.size() )
    setError( tr( "%1 bytes downloaded but the file size is only %2 bytes" ).arg( m_totalBytesTransferred ).arg( m_fileInfo.size() ) );

  if( m_totalBytesTransferred == m_fileInfo.size() )
    setTransferCompleted();
}

