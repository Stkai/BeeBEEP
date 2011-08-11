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


#include "FileTransferUpload.h"
#include "Protocol.h"


FileTransferUpload::FileTransferUpload( VNumber peer_id, QObject *parent )
  : FileTransferPeer( peer_id, parent )
{
#if defined( BEEBEEP_DEBUG )
  qDebug() << "FileTransferPeer created in Upload Mode";
#endif
}

void FileTransferUpload::checkData( const QByteArray& byte_array )
{
  switch( m_state )
  {
  case FileTransferPeer::Request:
    checkRequest( byte_array );
    break;
  case FileTransferPeer::Transferring:
    checkSending( byte_array );
    break;
  default:
    // discard data
    qWarning() << "Server Peer discard data:" << byte_array;
  }
}

void FileTransferUpload::checkRequest( const QByteArray& byte_array )
{
  Message m = Protocol::instance().toMessage( byte_array );
  if( !m.isValid() )
  {
#if defined( BEEBEEP_DEBUG )
    qDebug() << "Invalid file request received:" << byte_array;
#endif
    cancelTransfer();
    return;
  }

  FileInfo file_info = Protocol::instance().fileInfoFromMessage( m );
  if( !file_info.isValid() )
  {
#if defined( BEEBEEP_DEBUG )
    qDebug() << "Invalid file info in message:" << byte_array;
#endif
    cancelTransfer();
    return;
  }

#if defined( BEEBEEP_DEBUG )
  qDebug() << "File request received:" << file_info.id() << file_info.password();
#endif
  emit fileTransferRequest( file_info.id(), file_info.password() );
}

void FileTransferUpload::startTransfer( const FileInfo& fi )
{
  setFileInfo( fi );
  m_state = FileTransferPeer::Transferring;
#if defined( BEEBEEP_DEBUG )
  qDebug() << fi.path() << "file transfer started";
#endif
  sendData();
}

void FileTransferUpload::checkSending( const QByteArray& byte_array )
{
  if( byte_array.toInt() == m_bytesTransferred )
  {
#if defined( BEEBEEP_DEBUG )
    qDebug() << m_bytesTransferred << "bytes sent confirmed";
#endif
    m_totalBytesTransferred += m_bytesTransferred;
    showProgress();
    sendData();
  }
  else
    setError( tr( "%1 bytes sent not confirmed (%2 bytes confirmed)").arg( m_bytesTransferred ).arg( byte_array.toInt() ) );
}

void FileTransferUpload::sendData()
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
  {
    setTransferCompleted();
    return;
  }

  QByteArray byte_array = m_file.read( 32704 );

  if( m_socket.sendData( byte_array ) )
  {
    m_bytesTransferred = byte_array.size();
#if defined( BEEBEEP_DEBUG )
    qDebug() << m_fileInfo.name() << ":" << m_bytesTransferred << "bytes sent";
#endif
  }
  else
  {
    setError( tr( "Unable to sent %1" ).arg( byte_array.size() ) );
    return;
  }
}
