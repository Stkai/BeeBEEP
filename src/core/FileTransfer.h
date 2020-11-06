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

#ifndef BEEBEEP_FILETRANSFER_H
#define BEEBEEP_FILETRANSFER_H

#include "Config.h"
#include "FileInfo.h"
#include "FileTransferPeer.h"
#include "User.h"
class Message;


class FileTransfer : public QTcpServer
{
  Q_OBJECT

public:
  explicit FileTransfer( QObject *parent = Q_NULLPTR );

  bool startListener();
  void stopListener();

  inline bool isActive() const;
  inline bool hasActivePeers() const;

  FileInfo addFile( const QFileInfo&, const QString& share_folder, bool to_share_box, const QString& chat_private_id, FileInfo::ContentType, qint64 message_duration );
  void addFileInfoList( const QList<FileInfo>& );
  void removeFile( const QString& file_path );

  void downloadFile( VNumber from_user_id, const FileInfo& );
  bool cancelTransfer( VNumber peer_id );
  bool pauseTransfer( VNumber peer_id );

  void removeFilesToUser( VNumber user_id );

  inline void clearFiles();

  void onTickEvent( int );

signals:
  void message( VNumber peer_id, VNumber user_id, const FileInfo&, const QString&, FileTransferPeer::TransferState );
  void progress( VNumber peer_id, VNumber user_id, const FileInfo&, FileSizeType, qint64 );
  void listening();

protected:
  void incomingConnection( qintptr );
  void resetServerFiles();
  int activeDownloads() const;

  FileTransferPeer* peer( VNumber ) const;

  FileInfo fileInfo( VNumber ) const;
  FileInfo fileInfo( const QString& file_absolute_path, const QString chat_private_id ) const;
  FileTransferPeer* nextDownloadInQueue() const;
  int downloadsInQueue() const;

protected slots:
  void startNewDownload();
  void checkUploadRequest( const FileInfo& );
  void deletePeer();
  void setupPeer( FileTransferPeer*, qintptr, quint16 server_port = 0 );

private:
  QList<FileInfo> m_files;
  QList<FileTransferPeer*> m_peers;

};


// Inline Functions
inline bool FileTransfer::isActive() const { return isListening() && serverPort() > 0; }
inline void FileTransfer::clearFiles() { m_files.clear(); }
inline bool FileTransfer::hasActivePeers() const { return !m_peers.isEmpty(); }

#endif // BEEBEEP_FILETRANSFERSERVER_H
