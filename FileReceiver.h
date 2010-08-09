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
// $Id: FileReceiver.h 29 2010-06-30 16:29:04Z mastroddi $
//
//////////////////////////////////////////////////////////////////////

#ifndef BEEBEEP_FILERECEIVER_H
#define BEEBEEP_FILERECEIVER_H

#include "Config.h"


class FileReceiver : public QObject
{
  Q_OBJECT

public:
  FileReceiver( const QString& file_name, const QString& host_ip, int host_port, QObject* );

signals:
  void finished();

protected slots:
  void startConnection();
  void sendRequest();

private:
  QTcpSocket* mp_socket;
  QString m_fileName;
  QString m_hostName;
  int m_hostPort;

};

#endif // BEEBEEP_FILERECEIVER_H
