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

#ifndef BEEBEEP_GUIFILESHARING_H
#define BEEBEEP_GUIFILESHARING_H

#include "Config.h"
#include "GuiShareNetwork.h"
class FileInfo;
class GuiLog;
class GuiShareBox;
class GuiShareLocal;
class User;


class GuiFileSharing : public QMainWindow
{
  Q_OBJECT

public:
  GuiFileSharing( QWidget* parent = 0 );

  void checkViewActions();
  void updateLocalFileList();
  void updateNetworkFileList();
  void showUserFileList( const User& );

signals:
  void openUrlRequest( const QUrl& );
  void sendFileRequest( const QString& );
  void downloadSharedFileRequest( VNumber, VNumber );
  void downloadSharedFilesRequest( const QList<SharedFileInfo>& );

public slots:
  void showUp();
  void onTickEvent( int );
  void onUserChanged( const User& );

private slots:
  void raiseLocalShareView();
  void raiseNetworkShareView();
  void raiseShareBoxView();
  void addToShare( const QString& );
  void removeFromShare( const QString& );
  void onShareBoxRequest( VNumber, const QString&, bool );
  void onShareBoxDownloadRequest( VNumber, const FileInfo&, const QString& );
  void onShareBoxUploadRequest( VNumber, const FileInfo&, const QString& );
  void updateShareBox( const User&, const QString&, const QList<FileInfo>& );
  void onShareFolderUnavailable( const User&, const QString& );
  void onFileTransferProgress( VNumber, const User&, const FileInfo&, FileSizeType );
  void onFileTransferCompleted( VNumber, const User&, const FileInfo& );

protected:
  void keyPressEvent( QKeyEvent* );
  void raiseView( QWidget* );

private:
  void createActions();
  void createToolbars();
  void createStackedWidgets();
  void refreshUserList();
  void initGuiItems();

private:
  QStackedWidget* mp_stackedWidget;
  GuiShareLocal* mp_shareLocal;
  GuiShareNetwork* mp_shareNetwork;
  GuiShareBox* mp_shareBox;

  QToolBar* mp_barView;
  QToolBar* mp_barShareNetwork;
  QToolBar* mp_barShareLocal;
  QToolBar* mp_barScreenShot;

  QAction* mp_actViewToolBar;
  QAction* mp_actViewShareLocal;
  QAction* mp_actViewShareNetwork;
  QAction* mp_actViewScreenShot;
  QAction* mp_actViewLog;
  QAction* mp_actViewShareBox;

};

#endif // BEEBEEP_GUIFILESHARING_H
