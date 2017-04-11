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

#ifndef BEEBEEP_GUIEXTRA_H
#define BEEBEEP_GUIEXTRA_H


#include "Config.h"
#include "FileInfo.h"

class Core;
class ChatMessage;
class GameInterface;
class GuiLog;
class GuiScreenShot;
class GuiShareBox;
class GuiShareLocal;
class GuiShareNetwork;
class User;


class GuiExtra : public QMainWindow
{
  Q_OBJECT

public:
  GuiExtra( Core*, QWidget* parent = 0 );

  void updateLocalFileList();
  void updateNetworkFileList();
  void showUserFileList( const User& );

signals:
  void aboutToClose();
  void openUrlRequest( const QUrl& );
  void sendFileRequest( const QString& );

public slots:
  void showUp();
  void onTickEvent( int );
  void onUserChanged( const User& );

private slots:
  void raiseLocalShareView();
  void raiseNetworkShareView();
  void raiseLogView();
  void raiseScreenShotView();
  void raiseShareBoxView();
  void addToShare( const QString& );
  void removeFromShare( const QString& );
  void onShareBoxRequest( VNumber, const QString& );
  void onShareBoxDownloadRequest( VNumber, const FileInfo&, const QString& );
  void onShareBoxUploadRequest( VNumber, const FileInfo&, const QString& );

protected:
  void keyPressEvent( QKeyEvent* );
  void closeEvent( QCloseEvent* );
  void changeEvent( QEvent* );
  bool promptConnectionPassword();
  void raiseOnTop();
  void raiseView( QWidget* );

private:
  void createActions();
  void createToolbars();
  void createStackedWidgets();
  void refreshUserList();
  void initGuiItems();
  void checkViewActions();

private:
  Core* mp_core;
  QStackedWidget* mp_stackedWidget;
  GuiShareLocal* mp_shareLocal;
  GuiShareNetwork* mp_shareNetwork;
  GuiLog* mp_logView;
  GuiScreenShot* mp_screenShot;
  GuiShareBox* mp_shareBox;

  QToolBar* mp_barView;
  QToolBar* mp_barShareNetwork;
  QToolBar* mp_barShareLocal;
  QToolBar* mp_barScreenShot;
  QToolBar* mp_barLog;

  QAction* mp_actViewToolBar;
  QAction* mp_actViewShareLocal;
  QAction* mp_actViewShareNetwork;
  QAction* mp_actViewScreenShot;
  QAction* mp_actViewLog;
  QAction* mp_actViewShareBox;

};

#endif // BEEBEEP_GUIEXTRA_H
