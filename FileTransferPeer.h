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
#include "User.h"



class FileTransferPeer : public QObject
{
  Q_OBJECT

public:
  enum TransferState { Unknown, Request, Transferring, Completed, Error, Cancelled };

  explicit FileTransferPeer( VNumber, QObject *parent = 0 );

  void setConnectionDescriptor( int ); // if descriptor = 0 socket tries to connect to remote host (client side)

  void setFileInfo( const FileInfo& );
  inline const FileInfo& fileInfo() const;

  inline VNumber id() const;
  void cancelTransfer();

protected slots:
  void socketError( QAbstractSocket::SocketError );
  void checkAuthentication( const Message& );

  virtual void checkData( const QByteArray& ) = 0;
  virtual void sendData() = 0;

signals:
  void message( const User&, const FileInfo&, const QString& );
  void progress( const User&, const FileInfo&, FileSizeType );
  void transferFinished();
  void userAuthenticated();

protected:
  void showProgress();
  void setError( const QString& );
  void setTransferCompleted();
  void closeAll();

protected:
  VNumber m_id;
  User m_user;
  FileInfo m_fileInfo;
  ConnectionSocket m_socket;
  QFile m_file;
  TransferState m_state;
  int m_bytesTransferred;
  FileSizeType m_totalBytesTransferred;

};


// Inline Functions
inline VNumber FileTransferPeer::id() const { return m_id; }
inline const FileInfo& FileTransferPeer::fileInfo() const { return m_fileInfo; }


#endif // BEEBEEP_FILETRANSFERSERVERPEER_H
