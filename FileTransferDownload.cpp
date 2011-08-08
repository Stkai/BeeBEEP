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

#include "FileTransferClient.h"


FileTransferClient::FileTransferClient( const FileInfo& fi, QObject *parent )
  : FileTransferPeer( fi, parent )
{
#if defined( BEEBEEP_DEBUG )
  qDebug() << "File transfer client created for file" << m_fileInfo.name();
#endif
}

void FileTransferClient::sendData()
{
  switch( m_state )
  {
  case FileTransferPeer::Auth:
    sendAuth();
    break;
  case FileTransferPeer::Transferring:
    sendDataConfirmation();
    break;
  default:
    // do_nothing
    break;
  }
}

void FileTransferClient::sendAuth()
{
  m_socket.sendData( m_fileInfo.password() );
  m_state = FileTransferPeer::Transferring;
}


void FileTransferClient::sendDataConfirmation()
{
#if defined( BEEBEEP_DEBUG )
  qDebug() << "File transfer send a corfirmation for" << m_bytesTransferred << "bytes";
#endif
   m_socket.sendData( QByteArray::number( m_bytesTransferred ) );
}

void FileTransferClient::checkData( const QByteArray& byte_array )
{
  m_bytesTransferred = byte_array.size();
  m_totalBytesTransferred += m_bytesTransferred;

  if( !m_file.isOpen() )
  {
    if( !m_file.open( QIODevice::WriteOnly ) )
    {
      setError( tr( "Unable to open file") );
      return;
    }
  }

  if( m_file.write( byte_array ) == m_bytesTransferred )
  {
    sendData();
  }
  else
  {
    setError( tr( "Unable to write in the file" ) );
    return;
  }

  if( m_totalBytesTransferred == m_fileInfo.size() )
    setTransferCompleted();
}

