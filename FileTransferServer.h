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


class FileTransferServer : public QTcpServer
{
  Q_OBJECT

public:
  explicit FileTransferServer( QObject *parent = 0 );

  void setupTransfer( const QFileInfo&, const QString& file_password );

signals:
  void bytesTransferred( int, int );

protected:
  void incomingConnection( int );
  void startTransfer();

private:
  QFileInfo m_fileInfo;
  QString m_password;

};


#endif // BEEBEEP_FILETRANSFERSERVER_H
