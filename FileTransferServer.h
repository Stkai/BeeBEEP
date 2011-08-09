
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

#ifndef BEEBEEP_FILETRANSFERSERVER_H
#define BEEBEEP_FILETRANSFERSERVER_H

#include "Config.h"
#include "FileInfo.h"
#include "User.h"


class FileTransferServer : public QTcpServer
{
  Q_OBJECT

public:
  explicit FileTransferServer( QObject *parent = 0 );

  void uploadFile( const User&, const FileInfo& );
  void downloadFile( const User&, const FileInfo& );

  inline bool isBusy() const;

signals:
  void transferMessage( const User&, const FileInfo&, const QString& );
  void transferProgress( const User&, const FileInfo&, int );

protected:
  void incomingConnection( int );

protected slots:
  void stopUpload();

private:
  User m_user;
  FileInfo m_fileInfo;
  bool m_isBusy;

};


// Inline Functions
inline bool FileTransferServer::isBusy() const { return m_isBusy; }


#endif // BEEBEEP_FILETRANSFERSERVER_H
