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

#ifndef BEEBEEP_FILETRANSFERREAD_H
#define BEEBEEP_FILETRANSFERREAD_H

#include "Config.h"


class FileTransferRead : public QThread
{
  Q_OBJECT

public:
  FileTransferRead( const QFileInfo&, int socket_descriptor, const QString& file_password, QObject *parent = 0);

protected:
  virtual void run();

signals:
  void error( const QString& );

private:
  QFileInfo m_fileInfo;
  int m_socketDescriptor;
  QString m_filePassword;


};

#endif // BEEBEEP_FILETRANSFERREAD_H
