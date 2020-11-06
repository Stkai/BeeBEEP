//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2020 Marco Mastroddi
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
// along with BeeBEEP. If not, see <http://www.gnu.org/licenses/>.
//
// Author: Marco Mastroddi <marco.mastroddi(AT)gmail.com>
//
// $Id$
//
//////////////////////////////////////////////////////////////////////

#ifndef BEEBEEP_FILETRANSFERPEER_H
#define BEEBEEP_FILETRANSFERPEER_H

#include "ConnectionSocket.h"
#include "FileInfo.h"


class FileTransferPeer : public QObject
{
  Q_OBJECT

public:
  enum TransferState { Unknown, Queue, Starting, Request, FileHeader, Transferring, Completed, Error, Canceled, Pausing, Paused };

  explicit FileTransferPeer( QObject *parent = Q_NULLPTR );

  inline QString name() const;

  void setInQueue();
  inline bool isInQueue() const;
  inline void removeFromQueue();

  inline void setTransferType( FileInfo::TransferType );
  inline void setRemoteUserId( VNumber );
  inline VNumber remoteUserId() const;
  inline bool isDownload() const;
  inline void setId( VNumber );
  inline VNumber id() const;
  inline void setConnectionDescriptor( qintptr socket_descriptor, quint16 server_port ); // if descriptor = 0 socket tries to connect to remote host (client side)
  void setFileInfo( FileInfo::TransferType, const FileInfo& );
  inline const FileInfo& fileInfo() const;
  inline bool isSkipped() const;

  inline const Message& messageAuth() const; // Read below...
  inline QHostAddress peerAddress() const;

  inline bool isActive() const;
  inline bool isTransferCompleted() const;
  void startUpload( const FileInfo& );
  inline qint64 elapsedTime() const;

  bool canPauseTransfer() const;
  void pauseTransfer( bool close_connection );
  bool removePartiallyDownloadedFile();

  static bool stateIsStopped( FileTransferPeer::TransferState );

  void onTickEvent( int );

signals:
  void message( VNumber peer_id, VNumber user_id, const FileInfo&, const QString&, FileTransferPeer::TransferState );
  void progress( VNumber peer_id, VNumber user_id, const FileInfo&, FileSizeType, qint64 );
  void fileUploadRequest( const FileInfo& );
  void userValidationRequested( VNumber peer_id, VNumber user_id );
  void operationCompleted();

public slots:
  void startConnection();
  void cancelTransfer();

protected slots:
  void socketError( QAbstractSocket::SocketError );
  void checkTransferData( const QByteArray& );
  void connectionTimeout();
  void checkUserAuthentication( const QByteArray& );

protected:
  void setUserAuthorized( VNumber );
  void showProgress();
  void setError( const QString& );
  void setTransferCompleted();
  void closeAll();
  void sendTransferData();
  void computeElapsedTime();
  void setTransferPaused();
  void setTransferringState();

  /* FileTransferUpload */
  void sendUploadData();
  void checkUploadData( const QByteArray& );
  void checkUploadRequest( const QByteArray& );
  void checkUploading( const QByteArray& );
  void sendFileHeader();

  /* FileTransferDownload */
  void sendDownloadData();
  void checkDownloadData( const QByteArray& );
  void sendDownloadRequest();
  void sendDownloadDataConfirmation();
  QString temporaryFilePath() const;

protected:
  FileInfo::TransferType m_transferType;
  VNumber m_id;
  FileInfo m_fileInfo;
  QFile m_file;
  TransferState m_state;
  FileSizeType m_bytesTransferred;
  FileSizeType m_totalBytesTransferred;
  ConnectionSocket* mp_socket;
  qintptr m_socketDescriptor;
  VNumber m_remoteUserId;
  quint16 m_serverPort;
  QDateTime m_startTimestamp;
  qint64 m_elapsedTime;
  bool m_isSkipped;

};


// Inline Functions
inline QString FileTransferPeer::name() const { return QString( "%1 Peer #%2" ).arg( isDownload() ? "Download" : "Upload" ).arg( m_id ); }
inline void FileTransferPeer::setConnectionDescriptor( qintptr socket_descriptor, quint16 server_port ) { m_socketDescriptor = socket_descriptor; m_serverPort = server_port; }
inline bool FileTransferPeer::isInQueue() const { return m_state == FileTransferPeer::Queue; }
inline void FileTransferPeer::removeFromQueue() { m_state = FileTransferPeer::Starting; }
inline void FileTransferPeer::setTransferType( FileInfo::TransferType new_value ) { m_transferType = new_value; }
inline bool FileTransferPeer::isDownload() const { return m_transferType == FileInfo::Download; }
inline void FileTransferPeer::setId( VNumber new_value ) { m_id = new_value; }
inline VNumber FileTransferPeer::id() const { return m_id; }
inline const FileInfo& FileTransferPeer::fileInfo() const { return m_fileInfo; }
inline QHostAddress FileTransferPeer::peerAddress() const { return mp_socket->peerAddress(); }
inline bool FileTransferPeer::isActive() const { return m_state >= FileTransferPeer::Starting && m_state <= FileTransferPeer::Completed; }
inline bool FileTransferPeer::isTransferCompleted() const { return m_state == FileTransferPeer::Completed; }
inline void FileTransferPeer::setRemoteUserId( VNumber new_value ) { m_remoteUserId = new_value; }
inline VNumber FileTransferPeer::remoteUserId() const { return mp_socket->userId() != ID_INVALID ? mp_socket->userId() : m_remoteUserId; }
inline qint64 FileTransferPeer::elapsedTime() const { return m_elapsedTime; }
inline bool FileTransferPeer::isSkipped() const { return m_isSkipped; }

#endif // BEEBEEP_FILETRANSFERSERVERPEER_H
