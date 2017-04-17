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

#include "BeeUtils.h"
#include "GuiShareLocal.h"
#include "FileDialog.h"
#include "FileShare.h"
#include "Settings.h"


GuiShareLocal::GuiShareLocal( QWidget *parent )
  : QWidget(parent)
{
  setupUi( this );
  setAcceptDrops( true );

  mp_lTitle->setText( QString( "<b>%1</b>" ).arg( tr( "Share your folders or files" ) ) );

  mp_twMyShares->setContextMenuPolicy( Qt::CustomContextMenu );
  mp_twMyShares->setRootIsDecorated( false );
  mp_twMyShares->setSortingEnabled( true );
  mp_twMyShares->setAlternatingRowColors( true );
  mp_twMyShares->setSortingEnabled( true );

  QStringList labels;
  labels << tr( "File" ) << tr( "Size" ) << tr( "Path" );
  mp_twMyShares->setHeaderLabels( labels );

  QHeaderView* header_view = mp_twMyShares->header();
#if QT_VERSION >= 0x050000
  header_view->setSectionResizeMode( 0, QHeaderView::ResizeToContents );
  header_view->setSectionResizeMode( 1, QHeaderView::ResizeToContents );
  header_view->setSectionResizeMode( 2, QHeaderView::Stretch );
#else
  header_view->setResizeMode( 0, QHeaderView::ResizeToContents );
  header_view->setResizeMode( 1, QHeaderView::ResizeToContents );
  header_view->setResizeMode( 2, QHeaderView::Stretch );
#endif
  header_view->setSortIndicator( 2, Qt::AscendingOrder );

  mp_menuContext = new QMenu( this );

  connect( mp_twMyShares, SIGNAL( customContextMenuRequested( const QPoint& ) ), this, SLOT( openMySharesMenu( const QPoint& ) ) );
}

void GuiShareLocal::setupToolBar( QToolBar* bar )
{
  mp_labelShareStats = new QLabel( bar );
  mp_labelShareStats->setObjectName( "GuiLabelLocalShareStats" );
  mp_labelShareStats->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
  bar->addWidget( mp_labelShareStats );
  bar->addSeparator();

  mp_actAddFile = bar->addAction( QIcon( ":/images/file-add.png" ), tr( "Share a file" ), this, SLOT( addFilePath() ) );
  mp_actAddFile->setStatusTip( tr( "Add a file to your local share" ) );

  mp_actAddFolder = bar->addAction( QIcon( ":/images/folder-add.png" ), tr( "Share a folder" ), this, SLOT( addFolderPath() ) );
  mp_actAddFolder->setStatusTip( tr( "Add a folder to your local share" ) );

  mp_actUpdate = bar->addAction( QIcon( ":/images/update.png" ), tr( "Update shares" ), this, SLOT( updateList() ) );
  mp_actUpdate->setStatusTip( tr( "Update shared folders and files" ) );

  mp_actRemove = bar->addAction( QIcon( ":/images/delete.png" ), tr( "Remove shared path" ), this, SLOT( removePath() ) );
  mp_actRemove->setStatusTip( tr( "Remove shared path from the list" ) );

  mp_actClear = bar->addAction( QIcon( ":/images/clear.png" ), tr( "Clear all shares" ), this, SLOT( clearAllPaths() ) );
  mp_actClear->setStatusTip( tr( "Clear all shared paths from the list" ) );

  showStats( 0, 0 );
  setActionsEnabled( true );
}

void GuiShareLocal::showStats( int file_count, FileSizeType total_file_size )
{
  if( Settings::instance().fileTransferIsEnabled() )
    mp_labelShareStats->setText( QString( "%1: <b>%2</b> (%3)  " ).arg( tr( "Shared files" ) ).arg( file_count ).arg( Bee::bytesToString( total_file_size ) ) );
  else
    mp_labelShareStats->setText( QString( "<b>%1</b>  " ).arg( tr( "File transfer is disabled" ) ) );
}

void GuiShareLocal::setActionsEnabled( bool enable )
{
  mp_actAddFile->setEnabled( enable );
  mp_actAddFolder->setEnabled( enable );
  mp_actRemove->setEnabled( enable && mp_twMyShares->topLevelItemCount() > 0 );
  mp_actUpdate->setEnabled( enable && mp_twMyShares->topLevelItemCount() > 0 );
  mp_actClear->setEnabled( enable && mp_twMyShares->topLevelItemCount() > 0 );
  if( enable )
    setCursor( Qt::ArrowCursor );
  else
    setCursor( Qt::WaitCursor );
}

void GuiShareLocal::addFilePath()
{
  QStringList file_path_list = FileDialog::getOpenFileNames( true, this, tr( "Select a file to share" ),
                                                     Settings::instance().lastDirectorySelected() );
  if( file_path_list.isEmpty() )
    return;

  Settings::instance().setLastDirectorySelectedFromFile( file_path_list.last() );

  foreach( QString file_path, file_path_list )
    addSharePath( file_path );
}

void GuiShareLocal::addFolderPath()
{
  QString folder_path = FileDialog::getExistingDirectory( this, tr( "Select a folder to share" ),
                                                           Settings::instance().lastDirectorySelected() );
  if( folder_path.isEmpty() )
    return;

  Settings::instance().setLastDirectorySelected( Bee::convertToNativeFolderSeparator( folder_path ) );

  addSharePath( folder_path );
}

void GuiShareLocal::removePath()
{
  if( Settings::instance().localShare().isEmpty() )
    return;

  QList<QTreeWidgetItem*> item_list = mp_twMyShares->selectedItems();
  if( item_list.isEmpty() )
  {
    QMessageBox::information( this, Settings::instance().programName(), tr( "Please select a shared path." ) );
    mp_twMyShares->setFocus();
    return;
  }

  QString share_selected = item_list.first()->text( 2 );

  if( QMessageBox::question( this, Settings::instance().programName(), tr( "Do you really want to remove this path:" )
                             + QString( "\n%1").arg( share_selected ),
                             tr( "Yes" ), tr( "No" ), QString::null, 1, 1 ) != 0 )
    return;

  setActionsEnabled( false );

  emit sharePathRemoved( share_selected );
}

void GuiShareLocal::clearAllPaths()
{
  if( Settings::instance().localShare().isEmpty() )
    return;

  if( QMessageBox::question( this, Settings::instance().programName(), tr( "Do you really want to remove all shared paths?" ),
                             tr( "Yes" ), tr( "No" ), QString::null, 1, 1 ) != 0 )
    return;

  emit removeAllPathsRequest();
}

void GuiShareLocal::updatePaths()
{
  mp_twMyShares->clear();
  if( Settings::instance().localShare().isEmpty() )
    return;
  QTreeWidgetItem *item;
  mp_twMyShares->setUpdatesEnabled( false );
  foreach( QString share_path, Settings::instance().localShare() )
  {
    item = new QTreeWidgetItem( mp_twMyShares );
    item->setText( 0, QString::number( FileShare::instance().local().count( share_path ) ) );
    item->setText( 1, Bee::bytesToString( FileShare::instance().localSize( share_path ) ) );
    item->setText( 2, share_path );
  }
  mp_twMyShares->setUpdatesEnabled( true );
}

void GuiShareLocal::updateFileSharedList()
{
  setActionsEnabled( false );
  updatePaths();
  int file_count = 0;
  FileSizeType total_file_size = 0;

  if( !FileShare::instance().local().isEmpty() )
  {
    foreach( FileInfo fi, FileShare::instance().local() )
    {
      file_count++;
      total_file_size += fi.size();
    }
  }

  showStats( file_count, total_file_size );
  setActionsEnabled( true );
}

void GuiShareLocal::addSharePath( const QString& sp )
{
  QString share_path = Bee::convertToNativeFolderSeparator( sp );
  if( Settings::instance().hasLocalSharePath( share_path ) )
  {
    QMessageBox::information( this, Settings::instance().programName(),
      tr( "%1 is already shared." ).arg( share_path ) );
    return;
  }

  QStringList local_share = Settings::instance().localShare();
  local_share << share_path;
  Settings::instance().setLocalShare( local_share );

  updatePaths();

  if( !isFileSharingEnabled() )
    return;

  setActionsEnabled( false );

  emit sharePathAdded( share_path );
}

void GuiShareLocal::openItemDoubleClicked( QTreeWidgetItem* item, int )
{
  if( !item )
    return;

  QString file_path = item->data( 0, Qt::UserRole + 1 ).toString();
  if( !file_path.isEmpty() )
    emit openUrlRequest( QUrl::fromLocalFile( file_path ) );
}

void GuiShareLocal::updateList()
{
  if( !isFileSharingEnabled() )
    return;
  setActionsEnabled( false );
  emit updateListRequest();
}

bool GuiShareLocal::isFileSharingEnabled()
{
  if( Settings::instance().fileTransferIsEnabled() )
    return true;

  QMessageBox::information( this, Settings::instance().programName(), tr( "File transfer is disabled. Open the option menu to enable it." ) );
  return false;
}

void GuiShareLocal::dragEnterEvent( QDragEnterEvent *event )
{
  if( event->mimeData()->hasUrls() )
    event->acceptProposedAction();
}

void GuiShareLocal::dropEvent( QDropEvent *event )
{
  if( event->mimeData()->hasUrls() )
  {
    foreach( QUrl url, event->mimeData()->urls() )
    {
#if QT_VERSION >= 0x040800
      if( url.isLocalFile() )
#else
      if( url.scheme() == QLatin1String( "file" ) )
#endif
        addSharePath( url.toLocalFile() );
    }
  }
}

void GuiShareLocal::openMySharesMenu( const QPoint& p )
{
  QTreeWidgetItem* item = mp_twMyShares->itemAt( p );

  mp_menuContext->clear();

  if( item )
  {
    if( !item->isSelected() )
      item->setSelected( true );
    mp_menuContext->addAction( mp_actRemove );
  }
  else
  {
    mp_menuContext->addAction( mp_actAddFile );
    mp_menuContext->addAction( mp_actAddFolder );
  }

  mp_menuContext->exec( QCursor::pos() );
}
