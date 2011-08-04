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


#include "FileTransferRead.h"


FileTransferRead::FileTransferRead( const FileInfo& fi, int socket_descriptor, QObject *parent )
  : QThread( parent ), m_fileInfo( fi), m_socketDescriptor( socket_descriptor )
{
  qDebug() << "Init file transfer:" << m_fileInfo.path();
}

 void FileTransferRead::run()
 {
   qDebug() << "Start";

   QTcpSocket tcpSocket;
   if( !tcpSocket.setSocketDescriptor( m_socketDescriptor ) )
   {
     emit error( "Unable to initialize socket" );
     return;
   }

   bool auth = false;
   QByteArray file_password = m_fileInfo.password().toUtf8();

   while( tcpSocket.bytesAvailable() < file_password.size() )
     wait( 500 );

   QByteArray password_to_check = tcpSocket.readAll();
   if( password_to_check != file_password )
   {
     emit error( "Password Error" );
     tcpSocket.close();
     qDebug() << "PASSWORD ERROR";
     return;
   }

   QFile file( m_fileInfo.path() );
   if( !file.open( QIODevice::ReadOnly) )
   {
     emit error( "Unable to open file" );
     qDebug() << "FILE ERROR";
     tcpSocket.close();
     return;
   }

   qDebug() << "Read file";
   QByteArray raw_data;
   while( !file.atEnd() )
   {
     raw_data = file.read( 32000 );
     qDebug() << "FileTransfer read" << raw_data.size() << "bytes";
     QByteArray data_block;
     QDataStream data_stream( &data_block, QIODevice::WriteOnly );
     data_stream.setVersion( QDataStream::Qt_4_0 );
     data_stream << (quint32)0;
     data_stream << raw_data;
     data_stream.device()->seek( 0 );
     data_stream << (quint32)(data_block.size() - sizeof(quint32));
     tcpSocket.write( data_block );
     if( tcpSocket.waitForBytesWritten() )
       qDebug() << "FileTransfer" << (data_block.size() - sizeof(quint32)) << "bytes sent";
   }
   file.close();
   qDebug() << "disconnecting";
   tcpSocket.disconnectFromHost();
   if( tcpSocket.state() == QAbstractSocket::ClosingState )
     tcpSocket.waitForDisconnected();
   qDebug() << "disconnected";
   qDebug() << "finished";
 }
