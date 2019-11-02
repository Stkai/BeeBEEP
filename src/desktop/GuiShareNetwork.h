//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2019 Marco Mastroddi
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

#ifndef BEEBEEP_GUISHARENETWORK_H
#define BEEBEEP_GUISHARENETWORK_H

#include "ui_GuiShareNetwork.h"
#include "GuiFileInfoList.h"
#include "User.h"

typedef QPair<User, FileInfo> UserFileInfo;

class GuiShareNetwork : public QWidget, private Ui::GuiShareNetworkWidget
{
  Q_OBJECT

public:
  explicit GuiShareNetwork( QWidget *parent = Q_NULLPTR );
  void setupToolBar( QToolBar* );
  void initShares();
  void showSharesForUser( const User& );
  void updateUser( const User& );

signals:
  void fileShareListRequested();
  void downloadSharedFile( VNumber, VNumber );
  void downloadSharedFiles( const QList<SharedFileInfo>& );
  void openFileCompleted( const QUrl& );
  void updateStatus( const QString&, int );

public slots:
  void showMessage( VNumber, VNumber, const QString& );
  void onFileTransferProgress( VNumber, const User&, const FileInfo&, FileSizeType, int );
  void onFileTransferCompleted( VNumber, const User&, const FileInfo& );
  void reloadList();

protected slots:
  void checkItemDoubleClicked( QTreeWidgetItem*, int );
  void filterByText( const QString& );
  void enableScanButton();
  void scanNetwork();
  void applyFilter();
  void updateList();
  void openDownloadMenu( const QPoint& );
  void downloadSelected();
  void processNextItemInQueue();

protected:
  bool filterPassThrough( VNumber, const FileInfo& );
  void showStatus( const QString& );
  void loadShares( const User& );
  void showFileTransferCompleted( GuiFileInfoItem*, const QString& );
  void resetComboUsers();
  void downloadSelectedItem( QTreeWidgetItem* );

private:
  QLineEdit* mp_leFilter;
  QComboBox* mp_comboUsers;
  QComboBox* mp_comboFileType;
  QAction* mp_actScan;
  QAction* mp_actReload;
  QAction* mp_actDownload;

  GuiFileInfoList m_fileInfoList;
  QQueue<UserFileInfo> m_queue;
  QMenu* mp_menuContext;

};

#endif // BEEBEEP_GUISHARENETWORK_H
