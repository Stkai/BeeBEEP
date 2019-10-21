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

#ifndef BEEBEEP_GUIFILETRANSFER_H
#define BEEBEEP_GUIFILETRANSFER_H

#include "GuiFileTransferItem.h"
class User;


class GuiFileTransfer : public QTreeWidget
{
  Q_OBJECT

public:
  GuiFileTransfer( QWidget* parent = Q_NULLPTR );

public slots:
  void setProgress( VNumber, const User&, const FileInfo&, FileSizeType, int );
  void setMessage( VNumber, const User&, const FileInfo&, const QString&, FileTransferPeer::TransferState );
  void updateUser( const User& );

signals:
  void transferCanceled( VNumber );
  void openFileCompleted( const QUrl& );

protected:
  GuiFileTransferItem* findItem( VNumber );
  GuiFileTransferItem* createItem( VNumber, const User&, const FileInfo& );
  void showProgress( QTreeWidgetItem*, const FileInfo&, FileSizeType, int );
  void showIcon( QTreeWidgetItem* );
  void setCanceled( QTreeWidgetItem* );

private slots:
  void checkItemClicked( QTreeWidgetItem*, int );
  void checkItemDoubleClicked( QTreeWidgetItem*, int );
  void removeAllCompleted();
  void openMenu( const QPoint& );

private:
  QMenu* mp_menuContext;

};


#endif // BEEBEEP_GUIFILETRANSFER_H
