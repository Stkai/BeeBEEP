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

#ifndef BEEBEEP_FILETRANSFERPEER_H
#define BEEBEEP_FILETRANSFERPEER_H

#include "Config.h"
#include "ConnectionSocket.h"
#include "FileInfo.h"



class FileTransferPeer : public QObject
{
  Q_OBJECT

public:
  enum TransferState { Unknown, Auth, Transferring, Completed, Error };

  FileTransferPeer( const FileInfo&, QObject *parent = 0 );

  void startTransfer( int socket_descriptor ); // if descriptor = 0 socket tries to connect to remote host (client side)

protected slots:
  void socketError( QAbstractSocket::SocketError );

  virtual void checkData( const QByteArray& ) = 0;
  virtual void sendData() = 0;

signals:
  void transferMessage( const FileInfo&, const QString& );
  void transferFinished();

protected:
  void setError( const QString& );
  void setTransferCompleted();
  void closeAll();

protected:
  FileInfo m_fileInfo;
  ConnectionSocket m_socket;
  QFile m_file;
  TransferState m_state;
  int m_bytesTransferred;
  int m_totalBytesTransferred;

};

#endif // BEEBEEP_FILETRANSFERSERVERPEER_H
