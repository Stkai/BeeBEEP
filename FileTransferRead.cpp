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

   QFile file( m_fileInfo.path() );
   if( !file.open( QIODevice::ReadOnly) )
   {
     emit error( "Unable to open file" );
     return;
   }

   qDebug() << "Read file";
   QByteArray text;
   QByteArray block;
   QDataStream out(&block, QIODevice::WriteOnly);
   out.setVersion(QDataStream::Qt_4_0);

   text = file.readAll();

   out << (quint32)0;
   out << text;
   out.device()->seek(0);
   out << (quint32)(block.size() - sizeof(quint32));

   qDebug() << "send";
   tcpSocket.write(block);

   qDebug() << "sent";
   file.close();

   qDebug() << "disconnectiong";
   tcpSocket.disconnectFromHost();
   tcpSocket.waitForDisconnected();
   qDebug() << "disconnected";
   qDebug() << "finished";
 }
