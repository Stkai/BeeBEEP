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

#include "Core.h"
#include "BeeApplication.h"
#include "BeeUtils.h"
#include "FileInfo.h"
#include "GuiConfig.h"
#include "GuiHome.h"
#include "GuiLog.h"
#include "GuiSearchUser.h"
#include "GuiShareBox.h"
#include "GuiShareLocal.h"
#include "GuiShareNetwork.h"
#include "GuiFileSharing.h"
#include "PluginManager.h"
#include "Settings.h"
#include "UserManager.h"
#ifdef Q_OS_WIN
  #include <windows.h>
#endif


GuiFileSharing::GuiFileSharing( Core* main_core, QWidget *parent )
 : QMainWindow( parent ), mp_core( main_core )
{
  setObjectName( "GuiFileSharing" );
  setWindowIcon( QIcon( ":/images/file-sharing.png" ) );
  setWindowTitle( QString( "%1 - %2" ).arg( tr( "File Sharing" ), Settings::instance().programName() ) );

  // Create a status bar before the actions and the menu
  (void) statusBar();

  mp_stackedWidget = new QStackedWidget( this );
  createStackedWidgets();
  setCentralWidget( mp_stackedWidget );

  mp_barView = addToolBar( tr( "Show the view tool bar" ) );
  mp_barView->setObjectName( "GuiViewToolBar" );
  mp_barView->setIconSize( Settings::instance().mainBarIconSize() );

  createActions();
  createToolbars();

  connect( mp_core, SIGNAL( localShareListAvailable() ), mp_shareLocal, SLOT( updateFileSharedList() ) );
  connect( mp_core, SIGNAL( shareBoxAvailable( const User&, const QString&, const QList<FileInfo>& ) ), mp_shareBox, SLOT( updateBox( const User&, const QString&, const QList<FileInfo>& ) ) );
  connect( mp_core, SIGNAL( shareBoxUnavailable( const User&, const QString& ) ), mp_shareBox, SLOT( onShareFolderUnavailable( const User&, const QString& ) ) );
  connect( mp_core, SIGNAL( shareBoxDownloadCompleted( VNumber, const FileInfo& ) ), mp_shareBox, SLOT( onFileDownloadCompleted( VNumber, const FileInfo& ) ) );
  connect( mp_core, SIGNAL( shareBoxUploadCompleted( VNumber, const FileInfo& ) ), mp_shareBox, SLOT( onFileUploadCompleted( VNumber, const FileInfo& ) ) );

  connect( mp_shareLocal, SIGNAL( sharePathAdded( const QString& ) ), this, SLOT( addToShare( const QString& ) ) );
  connect( mp_shareLocal, SIGNAL( sharePathRemoved( const QString& ) ), this, SLOT( removeFromShare( const QString& ) ) );
  connect( mp_shareLocal, SIGNAL( openUrlRequest( const QUrl& ) ), this, SIGNAL( openUrlRequest( const QUrl& ) ) );
  connect( mp_shareLocal, SIGNAL( updateListRequest() ), mp_core, SLOT( buildLocalShareList() ) );
  connect( mp_shareLocal, SIGNAL( removeAllPathsRequest() ), mp_core, SLOT( removeAllPathsFromShare() ) );

  connect( mp_shareNetwork, SIGNAL( fileShareListRequested() ), mp_core, SLOT( sendFileShareRequestToAll() ) );
  connect( mp_shareNetwork, SIGNAL( downloadSharedFile( VNumber, VNumber ) ), this, SLOT( downloadSharedFile( VNumber, VNumber ) ) );
  connect( mp_shareNetwork, SIGNAL( downloadSharedFiles( const QList<SharedFileInfo>& ) ), this, SLOT( downloadSharedFiles( const QList<SharedFileInfo>& ) ) );
  connect( mp_shareNetwork, SIGNAL( openFileCompleted( const QUrl& ) ), this, SLOT( openUrl( const QUrl& ) ) );
  connect( mp_shareNetwork, SIGNAL( updateStatus( const QString&, int ) ), statusBar(), SLOT( showMessage( const QString&, int ) ) );

  connect( mp_shareBox, SIGNAL( shareBoxRequest( VNumber, const QString& ) ), this, SLOT( onShareBoxRequest( VNumber, const QString& ) ) );
  connect( mp_shareBox, SIGNAL( openUrlRequest( const QUrl& ) ), this, SIGNAL( openUrlRequest( const QUrl& ) ) );
  connect( mp_shareBox, SIGNAL( shareBoxDownloadRequest( VNumber, const FileInfo&, const QString& ) ), this, SLOT( onShareBoxDownloadRequest( VNumber, const FileInfo&, const QString& ) ) );
  connect( mp_shareBox, SIGNAL( shareBoxUploadRequest( VNumber, const FileInfo&, const QString& ) ), this, SLOT( onShareBoxUploadRequest( VNumber, const FileInfo&, const QString& ) ) );



  initGuiItems();

  setMinimumWidth( 620 );
  statusBar()->showMessage( tr( "Ready" ), 10000 );
}

void GuiFileSharing::keyPressEvent( QKeyEvent* e )
{
  if( e->key() == Qt::Key_Escape )
  {
    if( Settings::instance().keyEscapeMinimizeInTray() )
      QTimer::singleShot( 0, this, SLOT( hideToTrayIcon() ) );
    else
      QTimer::singleShot( 0, this, SLOT( showMinimized() ) );
    e->accept();
    return;
  }

  QMainWindow::keyPressEvent( e );
}

void GuiFileSharing::changeEvent( QEvent* e )
{
  QMainWindow::changeEvent( e );
}

void GuiFileSharing::closeEvent( QCloseEvent* e )
{
  // maybe timer is active
  mp_logView->stopCheckingLog();
  emit aboutToClose();
  QMainWindow::closeEvent( e );
}

void GuiFileSharing::initGuiItems()
{
  raiseLocalShareView();
  checkViewActions();
}

void GuiFileSharing::checkViewActions()
{
  bool is_connected = mp_core->isConnected();
  int connected_users = mp_core->connectedUsers();

  mp_actViewShareLocal->setEnabled( Settings::instance().fileTransferIsEnabled() && mp_stackedWidget->currentWidget() != mp_shareLocal );
  mp_actViewShareNetwork->setEnabled( Settings::instance().fileTransferIsEnabled() && mp_stackedWidget->currentWidget() != mp_shareNetwork && is_connected && connected_users > 0 );
  mp_actViewLog->setEnabled( mp_stackedWidget->currentWidget() != mp_logView );
  mp_actViewShareBox->setEnabled( Settings::instance().fileTransferIsEnabled() && mp_stackedWidget->currentWidget() != mp_shareBox );

  if( mp_stackedWidget->currentWidget() == mp_shareNetwork  )
    mp_barShareNetwork->show();
  else
    mp_barShareNetwork->hide();

  if( mp_stackedWidget->currentWidget() == mp_shareLocal )
    mp_barShareLocal->show();
  else
    mp_barShareLocal->hide();

  if( mp_stackedWidget->currentWidget() == mp_logView )
  {
    mp_barLog->show();
    mp_logView->startCheckingLog();
  }
  else
  {
    mp_barLog->hide();
    mp_logView->stopCheckingLog();
  }
}

void GuiFileSharing::createActions()
{
  mp_actViewToolBar = mp_barView->toggleViewAction();
  mp_actViewToolBar->setStatusTip( tr( "Show the view tool bar" ) );
  mp_actViewToolBar->setData( 99 );
}

void GuiFileSharing::createToolbars()
{
  mp_actViewShareLocal = mp_barView->addAction( QIcon( ":/images/upload.png" ), tr( "Show my shared files" ), this, SLOT( raiseLocalShareView() ) );
  mp_actViewShareNetwork = mp_barView->addAction( QIcon( ":/images/download.png" ), tr( "Show the network shared files" ), this, SLOT( raiseNetworkShareView() ) );
  mp_actViewShareBox = mp_barView->addAction( QIcon( ":/images/sharebox.png" ), tr( "Show the shared boxes" ), this, SLOT( raiseShareBoxView() ) );
  mp_actViewLog = mp_barView->addAction( QIcon( ":/images/log.png" ), tr( "Show the %1 log" ).arg( Settings::instance().programName() ), this, SLOT( raiseLogView() ) );
  addToolBarBreak( Qt::RightToolBarArea );
}

void GuiFileSharing::createStackedWidgets()
{
  QAction* act;
  mp_shareLocal = new GuiShareLocal( this );
  mp_stackedWidget->addWidget( mp_shareLocal );
  mp_barShareLocal= new QToolBar( tr( "Show the bar of local file sharing" ), this );
  addToolBar( Qt::BottomToolBarArea, mp_barShareLocal );
  mp_barShareLocal->setObjectName( "GuiShareLocalToolBar" );
  mp_barShareLocal->setIconSize( Settings::instance().mainBarIconSize() );
  mp_barShareLocal->setAllowedAreas( Qt::BottomToolBarArea | Qt::TopToolBarArea );
  mp_shareLocal->setupToolBar( mp_barShareLocal );
  act = mp_barShareLocal->toggleViewAction();
  act->setEnabled( false );

  mp_shareNetwork = new GuiShareNetwork( this );
  mp_stackedWidget->addWidget( mp_shareNetwork );
  mp_barShareNetwork = new QToolBar( tr( "Show the bar of network file sharing" ), this );
  addToolBar( Qt::BottomToolBarArea, mp_barShareNetwork );
  mp_barShareNetwork->setObjectName( "GuiShareNetworkToolBar" );
  mp_barShareNetwork->setIconSize( Settings::instance().mainBarIconSize() );
  mp_barShareNetwork->setAllowedAreas( Qt::BottomToolBarArea | Qt::TopToolBarArea );
  mp_shareNetwork->setupToolBar( mp_barShareNetwork );
  act = mp_barShareNetwork->toggleViewAction();
  act->setEnabled( false );

  mp_logView = new GuiLog( this );
  mp_stackedWidget->addWidget( mp_logView );
  mp_barLog = new QToolBar( tr( "Show the bar of log" ), this );
  addToolBar( Qt::BottomToolBarArea, mp_barLog );
  mp_barLog->setObjectName( "GuiLogToolBar" );
  mp_barLog->setIconSize( Settings::instance().mainBarIconSize() );
  mp_barLog->setAllowedAreas( Qt::BottomToolBarArea | Qt::TopToolBarArea );
  mp_logView->setupToolBar( mp_barLog );
  act = mp_barLog->toggleViewAction();
  act->setEnabled( false );

  mp_shareBox = new GuiShareBox( this );
  mp_stackedWidget->addWidget( mp_shareBox );

  mp_stackedWidget->setCurrentWidget( mp_shareLocal );
}

void GuiFileSharing::onUserChanged( const User& u )
{
  mp_shareBox->updateUser( u );
  mp_shareNetwork->updateUser( u );
}

void GuiFileSharing::addToShare( const QString& share_path )
{
  mp_core->addPathToShare( share_path, true );
}

void GuiFileSharing::removeFromShare( const QString& share_path )
{
  mp_core->removePathFromShare( share_path );
}

void GuiFileSharing::raiseView( QWidget* w )
{
  mp_stackedWidget->setCurrentWidget( w );
  checkViewActions();
  raise();
}

void GuiFileSharing::raiseLocalShareView()
{
  raiseView( mp_shareLocal );
}

void GuiFileSharing::raiseNetworkShareView()
{
  mp_shareNetwork->initShares();
  raiseView( mp_shareNetwork );
}

void GuiFileSharing::raiseLogView()
{
  raiseView( mp_logView );
}

void GuiFileSharing::raiseShareBoxView()
{
  raiseView( mp_shareBox );
  mp_shareBox->updateShareBoxes();
}

void GuiFileSharing::raiseOnTop()
{
  if( isMinimized() )
    showNormal();

  if( !isVisible() )
    show();

#ifdef Q_OS_WIN
  SetWindowPos( (HWND)winId(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE );
  SetWindowPos( (HWND)winId(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE );
#else
  raise();
#endif
}

void GuiFileSharing::showUp()
{
  raiseOnTop();
}

void GuiFileSharing::onTickEvent( int )
{

}

void GuiFileSharing::onShareBoxRequest( VNumber user_id, const QString& share_box_path )
{
  mp_core->sendShareBoxRequest( user_id, share_box_path );
}

void GuiFileSharing::onShareBoxDownloadRequest( VNumber user_id, const FileInfo& fi, const QString& to_path )
{
  mp_core->downloadFromShareBox( user_id, fi, to_path );
}

void GuiFileSharing::onShareBoxUploadRequest( VNumber user_id, const FileInfo& fi, const QString& to_path )
{
  mp_core->uploadToShareBox( user_id, fi, to_path );
}

void GuiFileSharing::updateLocalFileList()
{
  mp_shareLocal->updatePaths();
}

void GuiFileSharing::updateNetworkFileList()
{
  mp_shareNetwork->reloadList();
}

void GuiFileSharing::showUserFileList( const User& u )
{
  mp_shareNetwork->showSharesForUser( u );
}
