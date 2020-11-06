//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2020 Marco Mastroddi
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

#include "Core.h"
#include "BeeApplication.h"
#include "BeeUtils.h"
#include "FileInfo.h"
#include "FileShare.h"
#include "GuiConfig.h"
#include "GuiShareBox.h"
#include "GuiShareLocal.h"
#include "GuiShareNetwork.h"
#include "GuiFileSharing.h"
#include "IconManager.h"
#include "Settings.h"
#include "UserManager.h"


GuiFileSharing::GuiFileSharing( QWidget *parent )
 : QMainWindow( parent )
{
  setObjectName( "GuiFileSharing" );
  setWindowIcon( IconManager::instance().icon( "file-sharing.png" ) );
  setWindowTitle( QString( "%1 - %2" ).arg( tr( "File Sharing" ), Settings::instance().programName() ) );

  // Create a status bar before the actions and the menu
  (void) statusBar();

  mp_stackedWidget = new QStackedWidget( this );
  createStackedWidgets();
  setCentralWidget( mp_stackedWidget );

  mp_barView = addToolBar( tr( "Show the view tool bar" ) );
  mp_barView->setObjectName( "GuiViewToolBar" );
  mp_barView->setIconSize( Settings::instance().mainBarIconSize() );
  mp_barView->setFloatable( false );
  mp_barView->toggleViewAction()->setVisible( false );

  createActions();
  createToolbars();

  connect( beeCore, SIGNAL( localShareListAvailable() ), mp_shareLocal, SLOT( updateFileSharedList() ) );
  connect( beeCore, SIGNAL( shareBoxAvailable( const User&, const QString&, const QList<FileInfo>& ) ), this, SLOT( updateShareBox( const User&, const QString&, const QList<FileInfo>& ) ) );
  connect( beeCore, SIGNAL( shareBoxUnavailable( const User&, const QString& ) ), this, SLOT( onShareFolderUnavailable( const User&, const QString& ) ) );
  connect( beeCore, SIGNAL( fileTransferProgress( VNumber, const User&, const FileInfo&, FileSizeType, qint64 ) ), this, SLOT( onFileTransferProgress( VNumber, const User&, const FileInfo&, FileSizeType, qint64 ) ) );
  connect( beeCore, SIGNAL( fileTransferMessage( VNumber, const User&, const FileInfo&, const QString&, FileTransferPeer::TransferState ) ), this, SLOT( onFileTransferMessage( VNumber, const User&, const FileInfo&, const QString&, FileTransferPeer::TransferState ) ) );

  connect( mp_shareLocal, SIGNAL( sharePathAdded( const QString& ) ), this, SLOT( addToShare( const QString& ) ) );
  connect( mp_shareLocal, SIGNAL( sharePathRemoved( const QString& ) ), this, SLOT( removeFromShare( const QString& ) ) );
  connect( mp_shareLocal, SIGNAL( openUrlRequest( const QUrl& ) ), this, SIGNAL( openUrlRequest( const QUrl& ) ) );
  connect( mp_shareLocal, SIGNAL( updateListRequest() ), beeCore, SLOT( buildLocalShareList() ) );
  connect( mp_shareLocal, SIGNAL( removeAllPathsRequest() ), beeCore, SLOT( removeAllPathsFromShare() ) );

  connect( mp_shareNetwork, SIGNAL( fileShareListRequested() ), beeCore, SLOT( sendFileShareRequestToAll() ) );
  connect( mp_shareNetwork, SIGNAL( downloadSharedFile( VNumber, VNumber ) ), this, SIGNAL( downloadSharedFileRequest( VNumber, VNumber ) ) );
  connect( mp_shareNetwork, SIGNAL( downloadSharedFiles( const QList<SharedFileInfo>& ) ), this, SIGNAL( downloadSharedFilesRequest( const QList<SharedFileInfo>& ) ) );
  connect( mp_shareNetwork, SIGNAL( openFileCompleted( const QUrl& ) ), this, SIGNAL( openUrlRequest( const QUrl& ) ) );
  connect( mp_shareNetwork, SIGNAL( updateStatus( const QString&, int ) ), statusBar(), SLOT( showMessage( const QString&, int ) ) );

  connect( mp_shareBox, SIGNAL( shareBoxRequest( VNumber, const QString&, bool ) ), this, SLOT( onShareBoxRequest( VNumber, const QString&, bool ) ) );
  connect( mp_shareBox, SIGNAL( openUrlRequest( const QUrl& ) ), this, SIGNAL( openUrlRequest( const QUrl& ) ) );
  connect( mp_shareBox, SIGNAL( shareBoxDownloadRequest( VNumber, const FileInfo&, const QString& ) ), this, SLOT( onShareBoxDownloadRequest( VNumber, const FileInfo&, const QString& ) ) );
  connect( mp_shareBox, SIGNAL( shareBoxUploadRequest( VNumber, const FileInfo&, const QString& ) ), this, SLOT( onShareBoxUploadRequest( VNumber, const FileInfo&, const QString& ) ) );

  initGuiItems();
  if( !Settings::instance().fileSharingGeometry().isEmpty() )
    restoreGeometry( Settings::instance().fileSharingGeometry() );
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

void GuiFileSharing::closeEvent( QCloseEvent* e )
{
  if( isVisible() )
  {
    Settings::instance().setFileSharingGeometry( saveGeometry() );
    Settings::instance().save();
  }

  QMainWindow::closeEvent( e );
}

void GuiFileSharing::initGuiItems()
{
  raiseLocalShareView();
  checkViewActions();
  QTimer::singleShot( 0, mp_shareBox, SLOT( updateShareBoxes() ) );
}

void GuiFileSharing::checkViewActions()
{
  setEnabled( Settings::instance().enableFileTransfer() && Settings::instance().enableFileSharing() );

  if( !isEnabled() )
    return;

  bool is_connected = beeCore->isConnected();
  int connected_users = beeCore->connectedUsers();

  mp_actViewShareLocal->setEnabled( mp_stackedWidget->currentWidget() != mp_shareLocal );
  mp_actViewShareNetwork->setEnabled( mp_stackedWidget->currentWidget() != mp_shareNetwork && is_connected && connected_users > 0 );
  mp_actViewShareBox->setEnabled( mp_stackedWidget->currentWidget() != mp_shareBox );

  if( mp_stackedWidget->currentWidget() == mp_shareNetwork  )
    mp_barShareNetwork->show();
  else
    mp_barShareNetwork->hide();

  if( mp_stackedWidget->currentWidget() == mp_shareLocal )
    mp_barShareLocal->show();
  else
    mp_barShareLocal->hide();
}

void GuiFileSharing::createActions()
{
  mp_actViewToolBar = mp_barView->toggleViewAction();
  mp_actViewToolBar->setStatusTip( tr( "Show the view tool bar" ) );
  mp_actViewToolBar->setData( 99 );
}

void GuiFileSharing::createToolbars()
{
  mp_actViewShareLocal = mp_barView->addAction( IconManager::instance().icon( "upload.png" ), tr( "Show my shared files" ), this, SLOT( raiseLocalShareView() ) );
  mp_actViewShareNetwork = mp_barView->addAction( IconManager::instance().icon( "download.png" ), tr( "Show the network shared files" ), this, SLOT( raiseNetworkShareView() ) );
  mp_actViewShareBox = mp_barView->addAction( IconManager::instance().icon( "sharebox.png" ), tr( "Show the BeeBOX" ), this, SLOT( raiseShareBoxView() ) );
  if( !Settings::instance().allowedFileExtensionsInFileTransfer().isEmpty() )
  {
    mp_barView->addSeparator();
    mp_barView->addAction( IconManager::instance().icon( "warning.png" ), tr( "Only certain types of files are allowed for file transfer" ), this, SLOT( showAllowedFileExtensions() ) );
  }
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
  mp_barShareLocal->setFloatable( false );
  mp_shareLocal->setupToolBar( mp_barShareLocal );
  act = mp_barShareLocal->toggleViewAction();
  act->setEnabled( false );
  act->setVisible( false );

  mp_shareNetwork = new GuiShareNetwork( this );
  mp_stackedWidget->addWidget( mp_shareNetwork );
  mp_barShareNetwork = new QToolBar( tr( "Show the bar of network file sharing" ), this );
  addToolBar( Qt::BottomToolBarArea, mp_barShareNetwork );
  mp_barShareNetwork->setObjectName( "GuiShareNetworkToolBar" );
  mp_barShareNetwork->setIconSize( Settings::instance().mainBarIconSize() );
  mp_barShareNetwork->setAllowedAreas( Qt::BottomToolBarArea | Qt::TopToolBarArea );
  mp_barShareNetwork->setFloatable( false );
  mp_shareNetwork->setupToolBar( mp_barShareNetwork );
  act = mp_barShareNetwork->toggleViewAction();
  act->setEnabled( false );
  act->setVisible( false );

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
  beeCore->addPathToShare( share_path );
}

void GuiFileSharing::removeFromShare( const QString& share_path )
{
  beeCore->removePathFromShare( share_path );
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

void GuiFileSharing::raiseShareBoxView()
{
  raiseView( mp_shareBox );
}

void GuiFileSharing::showUp()
{
  Bee::showUp( this );
}

void GuiFileSharing::onTickEvent( int )
{

}

void GuiFileSharing::onShareBoxRequest( VNumber user_id, const QString& share_box_path, bool create_folder )
{
  beeCore->sendShareBoxRequest( user_id, share_box_path, create_folder );
}

void GuiFileSharing::onShareBoxDownloadRequest( VNumber user_id, const FileInfo& fi, const QString& to_path )
{
  beeCore->downloadFromShareBox( user_id, fi, to_path );
  statusBar()->showMessage( QString( "%1: %2..." ).arg( fi.name(), tr( "downloading" ) ), 30000 );
}

void GuiFileSharing::onShareBoxUploadRequest( VNumber user_id, const FileInfo& fi, const QString& to_path )
{
  beeCore->uploadToShareBox( user_id, fi, to_path );
  statusBar()->showMessage( QString( "%1: %2..." ).arg( fi.name(), tr( "uploading" ) ), 30000 );
}

void GuiFileSharing::updateShareBox( const User& u, const QString& folder_path, const QList<FileInfo>& file_list )
{
  mp_shareBox->updateBox( u, folder_path, file_list );
}

void GuiFileSharing::onShareFolderUnavailable( const User& u, const QString& folder_path )
{
  mp_shareBox->onShareFolderUnavailable( u, folder_path );
  statusBar()->showMessage( tr( "%1 is not available" ).arg( folder_path ), 5000 );
}

void GuiFileSharing::updateLocalFileList()
{
  mp_shareLocal->updateFileSharedList();
}

void GuiFileSharing::updateNetworkFileList()
{
  mp_shareNetwork->reloadList();
}

void GuiFileSharing::showUserFileList( const User& u )
{
  mp_shareNetwork->showSharesForUser( u );
  QString share_message = tr( "%1 has shared %2 files" ).arg( u.name() ).arg( FileShare::instance().fileSharedFromUser( u.id() ).size() );
  statusBar()->showMessage( share_message, 5000 );
}

void GuiFileSharing::onFileTransferProgress( VNumber peer_id, const User& u, const FileInfo& fi, FileSizeType bytes, qint64 elapsed_time )
{
  if( fi.size() == 0 )
  {
  #ifdef BEEBEEP_DEBUG
    qWarning() << "GuiFileSharing::onFileTransferProgress try to show progress divided by 0:" << qPrintable( fi.path() );
  #endif
    return;
  }

  mp_shareNetwork->onFileTransferProgress( peer_id, u, fi, bytes, elapsed_time );

  QString file_transfer_progress;
  int msg_timeout = 0;

  if( bytes < fi.size() )
  {

    file_transfer_progress = QString( "%1: %2 %3 of %4 (%5%)" )
                                       .arg( fi.name(), fi.isDownload() ? tr( "downloading" ) : tr( "uploading" ),
                                          Bee::bytesToString( bytes ), Bee::bytesToString( fi.size() ),
                                          QString::number( static_cast<FileSizeType>( (bytes * 100) / fi.size())) );
    msg_timeout = 1000;
  }
  else
  {
    file_transfer_progress = QString( "%1: %2" ).arg( fi.name(),
                               fi.isDownload() ? tr( "download completed" ) : tr( "upload completed" ) );
    file_transfer_progress += QString( " (%1)" ).arg( Bee::timeToString( elapsed_time ) );
    msg_timeout = 3000;
  }

  statusBar()->showMessage( file_transfer_progress, msg_timeout );
}

void GuiFileSharing::onFileTransferMessage( VNumber peer_id, const User& u, const FileInfo& file_info, const QString&, FileTransferPeer::TransferState ft_state )
{
  if( ft_state != FileTransferPeer::Completed )
    return;
  if( file_info.isInShareBox() )
  {
    if( file_info.isDownload() )
      mp_shareBox->onFileDownloadCompleted( u.id(), file_info );
    else
      mp_shareBox->onFileUploadCompleted( u.id(), file_info );
  }
  else
  {
    if( file_info.isDownload() )
      mp_shareNetwork->onFileTransferCompleted( peer_id, u, file_info );
  }
}

void GuiFileSharing::showAllowedFileExtensions()
{
  QString msg_box_title = QString( "%1 - %2" ).arg( tr( "Allowed file extensions" ), Settings::instance().programName() );
  if( Settings::instance().allowedFileExtensionsInFileTransfer().isEmpty() )
    QMessageBox::information( this, msg_box_title, QString( "%1<br>%2" ).arg( tr( "File transfer has no rules on file extensions." ), tr( "All types of files can be sent and received.") ), tr( "Ok" ) );
  else
    QMessageBox::warning( this, msg_box_title, QString( "%1<br>%2" ).arg( tr( "Only files with this type of extension can be transferred:" ), Settings::instance().allowedFileExtensionsInFileTransfer().join( ", " ).toUpper() ), tr( "Ok" ) );
}
