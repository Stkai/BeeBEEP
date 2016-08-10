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

#ifndef BEEBEEP_GUISHARELOCAL_H
#define BEEBEEP_GUISHARELOCAL_H

#include "ui_GuiShareLocal.h"
#include "Config.h"
#include "GuiFileInfoList.h"


class GuiShareLocal : public QWidget, private Ui::GuiShareLocalWidget
{
  Q_OBJECT

public:
  explicit GuiShareLocal( QWidget *parent = 0 );

  void updatePaths();
  void setupToolBar( QToolBar* );

signals:
  void sharePathAdded( const QString& );
  void sharePathRemoved( const QString& );
  void openUrlRequest( const QUrl& );
  void updateListRequest();
  void removeAllPathsRequest();

public slots:
  void updateFileSharedList();

protected slots:
  void addFilePath();
  void addFolderPath();
  void removePath();
  void loadFileInfoInList();
  void updateList();
  void openItemDoubleClicked( QTreeWidgetItem*, int );
  void openMySharesMenu( const QPoint& );
  void openLocalSharesMenu( const QPoint& );
  void clearAllPaths();
  void processNextItemInQueue();

protected:
  void addSharePath( const QString& );
  void setActionsEnabled( bool );
  void showStats( int, FileSizeType );
  bool isFileSharingEnabled();
  void dragEnterEvent( QDragEnterEvent* );
  void dropEvent( QDropEvent* );

private:
  QLabel* mp_labelShareStats;
  QAction* mp_actAddFile;
  QAction* mp_actAddFolder;
  QAction* mp_actRemove;
  QAction* mp_actUpdate;
  QAction* mp_actClear;

  GuiFileInfoList m_fileInfoList;
  QQueue<FileInfo> m_queue;

};


#endif // BEEBEEP_GUISHARELOCAL_H
