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
  : QThread( parent ), m_fileInfo( fi )
{
  mp_socket = new QTcpSocket( this );
  connect( mp_socket, SIGNAL( readyRead() ), this, SLOT( readData() ) );
  connect( mp_socket, SIGNAL( disconnected() ), this, SLOT( quit() ) );
  connect( mp_socket, SIGNAL( connected() ), this, SLOT( startTransfer() ) );
}

void FileTransferClient::run()
{
  mp_socket->connectToHost( m_fileInfo.hostAddress(), m_fileInfo.hostPort() );


  // quando c'e' un tcp socket e serve un loop si usa exec()
  exec();
}

void FileTransferClient::socketError( QAbstractSocket::SocketError )
{

  quit();
}

void FileTransferClient::startTransfer()
{


}
