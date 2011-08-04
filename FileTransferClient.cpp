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
  : QObject( parent ), m_fileInfo( fi ), m_dataSize( 0 ), m_socket(), m_file(), m_byteReceived( 0 )
{
  connect( &m_socket, SIGNAL( connected() ), this, SLOT( sendAuth() ) );
  connect( &m_socket, SIGNAL( readyRead() ), this, SLOT( readData() ) );
  connect( &m_socket, SIGNAL( disconnected() ), this, SLOT( closeAll() ) );

}

void FileTransferClient::startTransfer()
{
  m_file.setFileName( m_fileInfo.name() );
  m_socket.connectToHost( m_fileInfo.hostAddress(), m_fileInfo.hostPort() );
  m_byteReceived = 0;
  m_dataSize = 0;
}

void FileTransferClient::sendAuth()
{
  qDebug() << "Send FILE AUTH";
  m_socket.write( m_fileInfo.password().toUtf8() );
}

void FileTransferClient::catchError( QAbstractSocket::SocketError se )
{
  qWarning() << "FileTransferClient error" << se;
  emit error( "File Transfer Error" );
}

void FileTransferClient::closeAll()
{
  qDebug() << "FileTransferClient close all";
  if( m_socket.isOpen() )
    m_socket.close();
  if( m_file.isOpen() )
    m_file.close();
  emit finished();
}

void FileTransferClient::readData()
{
  QTcpSocket* mp_socket = qobject_cast<QTcpSocket*>( sender() );
  QDataStream data_stream( mp_socket );
  data_stream.setVersion( QDataStream::Qt_4_0 );
  if( m_dataSize == 0 )
  {
    if( mp_socket->bytesAvailable() < (int)sizeof(quint32))
      return;
    data_stream >> m_dataSize;
    qDebug() << "FileTransferClient" << m_dataSize << "bytes to read";
  }

  if( mp_socket->bytesAvailable() < m_dataSize )
   return;

  QByteArray raw_data;
  data_stream >> raw_data;
  qDebug() << "FileTransferClient read" << raw_data.size() << "bytes";
  m_dataSize = 0;
  m_byteReceived += raw_data.size();
  writeToFile( raw_data );
  emit byteReceived( m_fileInfo, m_byteReceived );

  if( m_byteReceived == m_fileInfo.size() )
  {
    emit transferCompleted( m_fileInfo );
    qDebug() << "COMPLETED";
  }
}

bool FileTransferClient::writeToFile( const QByteArray& data )
{
  if( !m_file.isOpen() && !m_file.open( QIODevice::WriteOnly ) )
  {
    qWarning() << "Unable to open file" << m_file.fileName();
    closeAll();
    return false;
  }

  return m_file.write( data ) == data.size();
}
