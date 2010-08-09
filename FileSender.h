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
// $Id: FileSender.h 36 2010-07-19 15:50:50Z mastroddi $
//
//////////////////////////////////////////////////////////////////////

#ifndef BEEBEEP_FILESENDER_H
#define BEEBEEP_FILESENDER_H

#include "Config.h"


class FileSender : public QObject
{
  Q_OBJECT

public:
  FileSender( const QFileInfo&, const QString&, QObject* );

  inline const QFileInfo& fileInfo() const;

signals:
  void fileReady();
  void finished();
  void bytesSent( int, int );
  void error( const QString& );

protected slots:
  virtual void startServer();
  void checkConnection();
  void readConnectionData();
  void socketDisconnected();
  void socketError( QAbstractSocket::SocketError );
  void sendData();

protected:
  void closeSocket( QTcpSocket* );
  void sendFile( QTcpSocket* );

private:
  QFileInfo m_fileInfo;
  QString m_key;
  int m_bytesSent;

  QTcpServer* mp_server;
  QTcpSocket* mp_socket;
  QFile m_file;

};


// Inline Functions

inline const QFileInfo& FileSender::fileInfo() const { return m_fileInfo; }


#endif // BEEBEEP_FILESENDER_H
