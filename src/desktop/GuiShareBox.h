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

#ifndef BEEBEEP_GUISHAREBOX_H
#define BEEBEEP_GUISHAREBOX_H

#include "ui_GuiShareBox.h"
#include "GuiFileInfoList.h"
class User;


class GuiShareBox : public QWidget, private Ui::GuiShareBoxWidget
{
  Q_OBJECT

public:
  explicit GuiShareBox( QWidget *parent = 0 );
  void setupToolBar( QToolBar* );
  void showBoxForUser( const User& );

signals:
  void downloadSharedFile( VNumber, VNumber );
  void downloadSharedFiles( const QList<SharedFileInfo>& );
  void openFileCompleted( const QUrl& );
  void updateStatus( const QString&, int );

public slots:
  void updateBox();

protected slots:
  void checkItemDoubleClicked( QTreeWidgetItem*, int );
  void filterByText( const QString& );
  void enableUpdateButton();
  void openDownloadMenu( const QPoint& );
  void downloadSelected();

protected:
  bool filterPassThrough( VNumber, const FileInfo& );
  void showStatus( const QString& );
  void loadShareBox( const User& );
  void showFileTransferCompleted( GuiFileInfoItem*, const QString& );
  void resetComboUsers();
  void downloadSelectedItem( QTreeWidgetItem* );

private:
  QLineEdit* mp_leFilter;
  QComboBox* mp_comboUsers;
  QComboBox* mp_comboFileType;
  QAction* mp_actUpdate;
  QAction* mp_actDownload;
  QAction* mp_actUpload;

  GuiFileInfoList m_fileInfoList;

};

#endif // BEEBEEP_GUISHAREBOX_H
