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

#ifndef BEEBEEP_FILETRANSFERCLIENT_H
#define BEEBEEP_FILETRANSFERCLIENT_H

#include "Config.h"
#include "FileInfo.h"


class FileTransferClient : public QThread
{
  Q_OBJECT

public:
  FileTransferClient( const FileInfo&, QObject *parent = 0 );

signals:
  void error( const QString& );

protected slots:
  void socketError( QAbstractSocket::SocketError );

protected:
  void virtual run();

private:
  FileInfo m_fileInfo;
  QTcpSocket* mp_socket;
  QFile* mp_file;

};


#endif // BEEBEEP_FILETRANSFERCLIENT_H
