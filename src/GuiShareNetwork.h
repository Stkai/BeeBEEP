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

#ifndef BEEBEEP_GUISHARENETWORK_H
#define BEEBEEP_GUISHARENETWORK_H

#include "ui_GuiShareNetwork.h"
#include "Config.h"
class FileInfo;
class GuiFileInfoItem;
class User;


class GuiShareNetwork : public QWidget, private Ui::GuiShareNetworkWidget
{
  Q_OBJECT

public:
  enum ColumnType { ColumnFile, ColumnSize, ColumnUser, ColumnStatus };
  enum DataType { UserId = Qt::UserRole + 1, FileId, FilePath, FileSize };

  explicit GuiShareNetwork( QWidget *parent = 0 );
  void setupToolBar( QToolBar* );
  void initShares();
  void showSharesForUser( const User& );

signals:
  void fileShareListRequested();
  void downloadSharedFile( VNumber, VNumber );
  void openFileCompleted( const QUrl& );
  void updateStatus( const QString&, int );

public slots:
  void showMessage( VNumber, VNumber, const QString& );
  void setFileTransferCompleted( VNumber, VNumber, const QString& );
  void reloadList();

protected slots:
  void checkItemDoubleClicked( QTreeWidgetItem*, int );
  void enableFilterButton();
  void enableScanButton();
  void scanNetwork();
  void applyFilter();
  void updateList();

protected:
  bool filterPassThrough( VNumber, const FileInfo& );
  GuiFileInfoItem* findItem( VNumber, VNumber );
  void showStatus( const QString& );
  void loadShares( const User& );
  void showFileTransferCompleted( GuiFileInfoItem*, const QString& );
  void resetComboUsers();

private:
  QLineEdit* mp_leFilter;
  QComboBox* mp_comboUsers;
  QComboBox* mp_comboFileType;
  QAction* mp_actScan;
  QAction* mp_actReload;
  QAction* mp_actFilter;

};

#endif // BEEBEEP_GUISHARENETWORK_H
