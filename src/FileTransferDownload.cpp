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
    qWarning() << name() << "tries to send download data but it is in invalid state";
}

void FileTransferPeer::sendDownloadRequest()
{
  qDebug() << name() << "sending file request:" << m_fileInfo.id() << m_fileInfo.password();
  if( m_socket.sendData( Protocol::instance().fromMessage( Protocol::instance().fileInfoToMessage( m_fileInfo ) ) ) )
    m_state = FileTransferPeer::Transferring;
  else
    cancelTransfer();
}

void FileTransferPeer::sendDownloadDataConfirmation()
{
#ifdef BEEBEEP_DEBUG
  qDebug() << name() << "sending corfirmation for" << m_bytesTransferred << "bytes";
#endif
  if( !m_socket.sendData( Protocol::instance().bytesArrivedConfirmation( m_bytesTransferred ) ) )
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

  if( m_state != FileTransferPeer::Transferring )
  {
    qWarning() << name() << "tries to check data with invalid state" << m_state;
    return;
  }

  m_bytesTransferred = byte_array.size();
  m_totalBytesTransferred += m_bytesTransferred;

  if( !m_file.isOpen() )
  {
    if( !m_file.open( QIODevice::WriteOnly ) )
    {
      setError( tr( "Unable to open file" ) );
      return;
    }
  }

  showProgress();

  if( m_file.write( byte_array ) == m_bytesTransferred )
  {
    sendTransferData();
  }
  else
  {
    setError( tr( "Unable to write in the file" ) );
    return;
  }

  if( m_totalBytesTransferred > m_fileInfo.size() )
    setError( tr( "%1 bytes downloaded but the file size is only %2 bytes" ).arg( m_totalBytesTransferred ).arg( m_fileInfo.size() ) );

  if( m_totalBytesTransferred == m_fileInfo.size() )
    setTransferCompleted();
}

