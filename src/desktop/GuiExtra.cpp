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
#include "GuiConfig.h"
#include "GuiHome.h"
#include "GuiLog.h"
#include "GuiScreenShot.h"
#include "GuiSearchUser.h"
#include "GuiShareBox.h"
#include "GuiShareLocal.h"
#include "GuiShareNetwork.h"
#include "GuiExtra.h"
#include "PluginManager.h"
#include "Settings.h"
#include "UserManager.h"
#ifdef Q_OS_WIN
  #include <windows.h>
#endif


GuiExtra::GuiExtra( Core* main_core, QWidget *parent )
 : QMainWindow( parent ), mp_core( main_core )
{
  setObjectName( "GuiExtra" );
  setWindowIcon( QIcon( ":/images/beebeep.png" ) );
  setWindowTitle( Settings::instance().programName() );

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
  connect( mp_shareNetwork, SIGNAL( updateStatus( const QString&, int ) ), this, SLOT( showMessage( const QString&, int ) ) );

  connect( mp_shareBox, SIGNAL( shareBoxRequest( VNumber, const QString& ) ), this, SLOT( onShareBoxRequest( VNumber, const QString& ) ) );
  connect( mp_shareBox, SIGNAL( openUrlRequest( const QUrl& ) ), this, SIGNAL( openUrlRequest( const QUrl& ) ) );
  connect( mp_shareBox, SIGNAL( shareBoxDownloadRequest( VNumber, const FileInfo&, const QString& ) ), this, SLOT( onShareBoxDownloadRequest( VNumber, const FileInfo&, const QString& ) ) );
  connect( mp_shareBox, SIGNAL( shareBoxUploadRequest( VNumber, const FileInfo&, const QString& ) ), this, SLOT( onShareBoxUploadRequest( VNumber, const FileInfo&, const QString& ) ) );

  connect( mp_screenShot, SIGNAL( hideRequest() ), this, SLOT( hide() ) );
  connect( mp_screenShot, SIGNAL( showRequest() ), this, SLOT( show() ) );
  connect( mp_screenShot, SIGNAL( screenShotToSend( const QString& ) ), this, SIGNAL( sendFileRequest( const QString& ) ) );

  connect( mp_home, SIGNAL( openUrlRequest( const QUrl& ) ), this, SIGNAL( openUrlRequest( const QUrl& ) ) );

  initGuiItems();

  setMinimumWidth( 620 );
  statusBar()->showMessage( tr( "Ready" ) );
}

void GuiExtra::keyPressEvent( QKeyEvent* e )
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

void GuiExtra::changeEvent( QEvent* e )
{
  QMainWindow::changeEvent( e );
}

void GuiExtra::closeEvent( QCloseEvent* e )
{
  // maybe timer is active
  mp_logView->stopCheckingLog();
  emit aboutToClose();
  QMainWindow::closeEvent( e );
}

void GuiExtra::initGuiItems()
{
  raiseHomeView();
  checkViewActions();
}

void GuiExtra::checkViewActions()
{
  bool is_connected = mp_core->isConnected();
  int connected_users = mp_core->connectedUsers();

  mp_actViewHome->setEnabled( mp_stackedWidget->currentWidget() != mp_home );
  mp_actViewShareLocal->setEnabled( Settings::instance().fileTransferIsEnabled() && mp_stackedWidget->currentWidget() != mp_shareLocal );
  mp_actViewShareNetwork->setEnabled( Settings::instance().fileTransferIsEnabled() && mp_stackedWidget->currentWidget() != mp_shareNetwork && is_connected && connected_users > 0 );
  mp_actViewLog->setEnabled( mp_stackedWidget->currentWidget() != mp_logView );
  mp_actViewScreenShot->setEnabled( mp_stackedWidget->currentWidget() != mp_screenShot );
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

  if( mp_stackedWidget->currentWidget() == mp_screenShot )
    mp_barScreenShot->show();
  else
    mp_barScreenShot->hide();
}
void GuiExtra::createActions()
{
  mp_actViewToolBar = mp_barView->toggleViewAction();
  mp_actViewToolBar->setStatusTip( tr( "Show the view tool bar" ) );
  mp_actViewToolBar->setData( 99 );
}

void GuiExtra::createToolbars()
{
  mp_actViewHome = mp_barView->addAction( QIcon( ":/images/home.png" ), tr( "Show %1 home" ).arg( Settings::instance().programName() ), this, SLOT( raiseHomeView() ) );
  mp_actViewHome->setStatusTip( tr( "Show the homepage with %1 activity" ).arg( Settings::instance().programName() ) );
  mp_actViewShareLocal = mp_barView->addAction( QIcon( ":/images/upload.png" ), tr( "Show my shared files" ), this, SLOT( raiseLocalShareView() ) );
  mp_actViewShareLocal->setStatusTip( tr( "Show the list of the files which I have shared" ) );
  mp_actViewShareNetwork = mp_barView->addAction( QIcon( ":/images/download.png" ), tr( "Show the network shared files" ), this, SLOT( raiseNetworkShareView() ) );
  mp_actViewShareNetwork->setStatusTip( tr( "Show the list of the network shared files" ) );
  mp_actViewShareBox = mp_barView->addAction( QIcon( ":/images/sharebox.png" ), tr( "Show the shared boxes" ), this, SLOT( raiseShareBoxView() ) );
  mp_actViewScreenShot = mp_barView->addAction( QIcon( ":/images/screenshot.png" ), tr( "Make a screenshot" ), this, SLOT( raiseScreenShotView() ) );
  mp_actViewScreenShot->setStatusTip( tr( "Show the utility to capture a screenshot" ) );
  mp_actViewLog = mp_barView->addAction( QIcon( ":/images/log.png" ), tr( "Show the %1 log" ).arg( Settings::instance().programName() ), this, SLOT( raiseLogView() ) );
  mp_actViewLog->setStatusTip( tr( "Show the application log to see if an error occurred" ) );
  addToolBarBreak( Qt::RightToolBarArea );
}

void GuiExtra::createStackedWidgets()
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

  mp_screenShot = new GuiScreenShot( this );
  mp_stackedWidget->addWidget( mp_screenShot );
  mp_barScreenShot = new QToolBar( tr( "Show the bar of screenshot plugin" ), this );
  addToolBar( Qt::BottomToolBarArea, mp_barScreenShot );
  mp_barScreenShot->setObjectName( "GuiScreenShotToolBar" );
  mp_barScreenShot->setIconSize( Settings::instance().mainBarIconSize() );
  mp_barScreenShot->setAllowedAreas( Qt::BottomToolBarArea | Qt::TopToolBarArea );
  mp_screenShot->setupToolBar( mp_barScreenShot );
  act = mp_barScreenShot->toggleViewAction();
  act->setEnabled( false );

  mp_home = new GuiHome( this );
  mp_stackedWidget->addWidget( mp_home );

  mp_shareBox = new GuiShareBox( this );
  mp_stackedWidget->addWidget( mp_shareBox );

  mp_stackedWidget->setCurrentWidget( mp_home );
}

void GuiExtra::onUserChanged( const User& u )
{
  mp_shareBox->updateUser( u );
  mp_shareNetwork->updateUser( u );
}



void GuiExtra::showTipOfTheDay()
{
  raiseHomeView();
  mp_core->showTipOfTheDay();
}

void GuiExtra::showFactOfTheDay()
{
  raiseHomeView();
  mp_core->showFactOfTheDay();
}

void GuiExtra::addToShare( const QString& share_path )
{
  mp_core->addPathToShare( share_path, true );
}

void GuiExtra::removeFromShare( const QString& share_path )
{
  mp_core->removePathFromShare( share_path );
}

void GuiExtra::raiseView( QWidget* w )
{
  mp_stackedWidget->setCurrentWidget( w );
  checkViewActions();
  raise();
}

void GuiExtra::raiseHomeView()
{
  raiseView( mp_home );
}

void GuiExtra::raiseLocalShareView()
{
  raiseView( mp_shareLocal );
}

void GuiExtra::raiseNetworkShareView()
{
  mp_shareNetwork->initShares();
  raiseView( mp_shareNetwork );
}

void GuiExtra::raiseLogView()
{
  raiseView( mp_logView );
}

void GuiExtra::raiseScreenShotView()
{
  raiseView( mp_screenShot );
}

void GuiExtra::raiseShareBoxView()
{
  raiseView( mp_shareBox );
  mp_shareBox->updateShareBoxes();
}

void GuiExtra::raiseOnTop()
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

void GuiExtra::showUp()
{
  raiseOnTop();
}

void GuiExtra::onTickEvent( int )
{

}

void GuiExtra::onShareBoxRequest( VNumber user_id, const QString& share_box_path )
{
  mp_core->sendShareBoxRequest( user_id, share_box_path );
}

void GuiExtra::onShareBoxDownloadRequest( VNumber user_id, const FileInfo& fi, const QString& to_path )
{
  mp_core->downloadFromShareBox( user_id, fi, to_path );
}

void GuiExtra::onShareBoxUploadRequest( VNumber user_id, const FileInfo& fi, const QString& to_path )
{
  mp_core->uploadToShareBox( user_id, fi, to_path );
}

void GuiExtra::updateSystemMessages()
{
  mp_home->loadDefaultChat();
}

void GuiExtra::addSystemMessage( const ChatMessage& chat_msg )
{
  mp_home->addSystemMessage( chat_msg );
}

void GuiExtra::updateLocalFileList()
{
  mp_shareLocal->updatePaths();
}

void GuiExtra::updateNetworkFileList()
{
  mp_shareNetwork->reloadList();
}

void GuiExtra::showUserFileList( const User& u )
{
  mp_shareNetwork->showSharesForUser( u );
}
