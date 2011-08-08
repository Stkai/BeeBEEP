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

#include "FileTransferServer.h"
#include "FileTransferDownload.h"
#include "FileTransferUpload.h"



FileTransferServer::FileTransferServer( QObject *parent )
  : QTcpServer( parent ), m_user(), m_fileInfo( FileInfo::Upload )
{
}

void FileTransferServer::uploadFile( const User& u, const FileInfo& fi )
{
  m_user = u;
  m_fileInfo = fi;
}

void FileTransferServer::incomingConnection( int socketDescriptor )
{
  FileTransferUpload *upload_peer = new FileTransferUpload( m_user, m_fileInfo, this );
  connect( upload_peer, SIGNAL( transferFinished() ), upload_peer, SLOT( deleteLater() ) );
  connect( upload_peer, SIGNAL( transferMessage( const User&, const FileInfo&, const QString& ) ), this, SIGNAL( transferMessage( const User&, const FileInfo&, const QString& ) ) );
  upload_peer->startTransfer( socketDescriptor );
}

void FileTransferServer::downloadFile( const User& u, const FileInfo& fi )
{
  FileTransferDownload *download_peer = new FileTransferDownload( u, fi, this );
  connect( download_peer, SIGNAL( transferFinished() ), download_peer, SLOT( deleteLater() ) );
  connect( download_peer, SIGNAL( transferMessage( const User&, const FileInfo&, const QString& ) ), this, SIGNAL( transferMessage( const User&, const FileInfo&, const QString& ) ) );
  download_peer->startTransfer( 0 );
}
