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


#include "FileTransferServerPeer.h"


FileTransferServerPeer::FileTransferServerPeer( const FileInfo& fi, QObject *parent )
  : FileTransferPeer( fi, parent )
{
#if defined( BEEBEEP_DEBUG )
  qDebug() << "Server Peer created for file" << m_fileInfo.name();
#endif
}

void FileTransferServerPeer::checkData( const QByteArray& byte_array )
{
  switch( m_state )
  {
  case FileTransferPeer::Auth:
    checkAuth( byte_array );
    break;
  case FileTransferPeer::Transferring:
    checkSending( byte_array );
    break;
  default:
    // discard data
    qWarning() << "Server Peer discard data:" << byte_array;
  }
}

void FileTransferServerPeer::checkAuth( const QByteArray& byte_array )
{
  if( byte_array == m_fileInfo.password() )
  {
#if defined( BEEBEEP_DEBUG )
    qDebug() << "Server Peer auth received for file" << m_fileInfo.name();
#endif
    m_state = FileTransferPeer::Transferring;
    sendData();
  }
  else
  {
    setError( tr( "Not authorized" ) );
  }
}

void FileTransferServerPeer::checkSending( const QByteArray& byte_array )
{
  if( byte_array.toInt() == m_bytesTransferred )
  {
#if defined( BEEBEEP_DEBUG )
    qDebug() << m_fileInfo.name() << ":" << m_bytesTransferred << "bytes sent confirmed";
#endif
    m_totalBytesTransferred += m_bytesTransferred;
    sendData();
  }
  else
    setError( tr( "%1 bytes sent not confirmed (%2 bytes confirmed)").arg( m_bytesTransferred ).arg( byte_array.toInt() ) );
}

void FileTransferServerPeer::sendData()
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
