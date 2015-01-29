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

signals:
  void fileShareListRequested();
  void downloadSharedFile( VNumber, VNumber );
  void openFileCompleted( const QUrl& );

public slots:
  void loadShares( const User& );
  void showMessage( VNumber, VNumber, const QString& );
  void setFileTransferCompleted( VNumber, VNumber, const QString& );

protected slots:
  void checkItemDoubleClicked( QTreeWidgetItem*, int );
  void enableScanButton();
  void enableUpdateButton();
  void enableReloadButton();
  void scanNetwork();
  void applyFilter();
  void reloadList();
  void updateList();

protected:
  bool filterPassThrough( const User&, const FileInfo& );
  GuiFileInfoItem* findItem( VNumber, VNumber );


};

#endif // BEEBEEP_GUISHARENETWORK_H
