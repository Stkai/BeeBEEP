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

#ifndef BEEBEEP_GUIFILETRANSFERITEM_H
#define BEEBEEP_GUIFILETRANSFERITEM_H

#include "FileTransferPeer.h"
class User;


class GuiFileTransferItem : public QTreeWidgetItem
{

public:
  enum ColumnType { ColumnCancel, ColumnReport, ColumnTimeLeft, ColumnFile, ColumnUser, ColumnProgress, ColumnSort };

  GuiFileTransferItem( QTreeWidget* );
  GuiFileTransferItem( QTreeWidgetItem* );
  bool operator<( const GuiFileTransferItem& ) const;

  inline VNumber peerId() const;
  inline VNumber userId() const;
  inline const FileInfo& fileInfo() const;
  inline FileTransferPeer::TransferState transferState() const;
  void setTransferState( FileTransferPeer::TransferState );
  inline bool isStopped() const;
  QIcon defaultIcon() const;

  void init( VNumber peer_id, const User&, const FileInfo& );
  bool updateFileInfo( const FileInfo&, FileSizeType, int );
  bool updateUser( const User& );

  void setMessage( const QString&, FileTransferPeer::TransferState );

  void onTickEvent( int );

protected:
  void update();

private:
  VNumber m_peerId;
  VNumber m_userId;
  FileInfo m_fileInfo;
  FileTransferPeer::TransferState m_transferState;

};


// Inline Functions
inline VNumber GuiFileTransferItem::peerId() const { return m_peerId; }
inline VNumber GuiFileTransferItem::userId() const { return m_userId; }
inline const FileInfo& GuiFileTransferItem::fileInfo() const { return m_fileInfo; }
inline FileTransferPeer::TransferState GuiFileTransferItem::transferState() const { return m_transferState; }
inline bool GuiFileTransferItem::isStopped() const { return m_transferState < FileTransferPeer::Starting || m_transferState >= FileTransferPeer::Completed; }
#endif // BEEBEEP_GUIFILETRANSFERITEM_H
