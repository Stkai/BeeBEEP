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

#include "ConnectionSocket.h"
#include "FileInfo.h"
#include "Message.h"


class FileTransferPeer : public QObject
{
  Q_OBJECT

public:
  enum TransferType { Download, Upload };
  enum TransferState { Unknown, Request, Transferring, Completed, Error, Cancelled };

  explicit FileTransferPeer( QObject *parent = 0 );

  inline void setTransferType( TransferType );
  inline bool isDownload() const;
  inline void setId( VNumber );
  inline VNumber id() const;
  inline VNumber userId() const;
  void setConnectionDescriptor( int ); // if descriptor = 0 socket tries to connect to remote host (client side)
  void setFileInfo( const FileInfo& );
  inline const FileInfo& fileInfo() const;
  inline const Message& messageAuth() const;
  inline QHostAddress peerAddress() const;

  void setUserAuthorized( VNumber );
  void startUpload( const FileInfo& );
  void cancelTransfer();

signals:
  void message( VNumber peer_id, VNumber user_id, const FileInfo&, const QString& );
  void progress( VNumber peer_id, VNumber user_id, const FileInfo&, FileSizeType );
  void fileUploadRequest( VNumber, const QByteArray& );
  void authenticationRequested();

protected slots:
  void socketError( QAbstractSocket::SocketError );
  void checkTransferData( const QByteArray& );
  void checkAuthenticationRequested( const Message& );

protected:
  void showProgress();
  void setError( const QString& );
  void setTransferCompleted();
  void closeAll();
  void sendTransferData();

  /* FileTransferUpload */
  void checkUploadData( const QByteArray& );
  void sendUploadData();
  void checkUploadRequest( const QByteArray& );
  void checkUploading( const QByteArray& );
  /* FileTransferDownload */
  void checkDownloadData( const QByteArray& );
  void sendDownloadData();
  void sendDownloadRequest();
  void sendDownloadDataConfirmation();

protected:
  TransferType m_transferType;
  VNumber m_id;
  FileInfo m_fileInfo;
  QFile m_file;
  TransferState m_state;
  int m_bytesTransferred;
  FileSizeType m_totalBytesTransferred;
  ConnectionSocket* mp_socket;
  Message m_messageAuth; // This class for a ? reason does not emit an authentication signal with arguments
                         // so i have to store message so the parent class can access it

};


// Inline Functions
inline void FileTransferPeer::setTransferType( FileTransferPeer::TransferType new_value ) { m_transferType = new_value; }
inline bool FileTransferPeer::isDownload() const { return m_transferType == FileTransferPeer::Download; }
inline void FileTransferPeer::setId( VNumber new_value ) { m_id = new_value; }
inline VNumber FileTransferPeer::id() const { return m_id; }
inline const FileInfo& FileTransferPeer::fileInfo() const { return m_fileInfo; }
inline VNumber FileTransferPeer::userId() const { return mp_socket->userId(); }
inline const Message& FileTransferPeer::messageAuth() const { return m_messageAuth; }
inline QHostAddress FileTransferPeer::peerAddress() const { return mp_socket->peerAddress(); }

#endif // BEEBEEP_FILETRANSFERSERVERPEER_H
