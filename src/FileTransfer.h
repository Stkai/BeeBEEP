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
// Author: Marco Mastroddi <marco.mastroddi(AT)gmail.com>
//
// $Id$
//
//////////////////////////////////////////////////////////////////////

#ifndef BEEBEEP_FILETRANSFER_H
#define BEEBEEP_FILETRANSFER_H

#include "Config.h"
#include "FileInfo.h"
#include "User.h"
class FileTransferPeer;
class Message;


class FileTransfer : public QTcpServer
{
  Q_OBJECT

public:
  explicit FileTransfer( QObject *parent = 0 );

  bool startListener();
  void stopListener();

  FileInfo addFile( const QFileInfo&, const QString& );
  void addFileInfoList( const QList<FileInfo>& );
  void removeFile( const QFileInfo& );

  void downloadFile( const FileInfo& );
  bool cancelTransfer( VNumber peer_id );

  inline void clearFiles();

  void validateUser( VNumber peer_id, VNumber user_id );

  FileTransferPeer* peer( VNumber ) const;

signals:
  void message( VNumber peer_id, VNumber user_id, const FileInfo&, const QString& );
  void progress( VNumber peer_id, VNumber user_id, const FileInfo&, FileSizeType );
  void userConnected( VNumber peer_id, const QHostAddress& peer_address, const Message& hello_message );
  void listening();

protected:
  void incomingConnection( qintptr );
  void resetServerFiles();
  int activeDownloads() const;

  FileInfo fileInfo( VNumber ) const;
  FileInfo fileInfoFromPath( const QString& ) const;
  FileTransferPeer* nextDownloadInQueue() const;
  int downloadsInQueue() const;

protected slots:
  void startNewDownload();
  void checkAuthentication();
  void checkUploadRequest( VNumber, const QByteArray& );
  void peerDestroyed();
  void setupPeer( FileTransferPeer*, int );

private:
  QList<FileInfo> m_files;
  QList<FileTransferPeer*> m_peers;

};


// Inline Functions
inline void FileTransfer::clearFiles() { m_files.clear(); }


#endif // BEEBEEP_FILETRANSFERSERVER_H
