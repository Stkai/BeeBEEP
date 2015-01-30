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

#include "BeeUtils.h"
#include "GuiIconProvider.h"
#include "GuiFileInfoItem.h"
#include "GuiShareLocal.h"
#include "FileShare.h"
#include "Settings.h"


GuiShareLocal::GuiShareLocal( QWidget *parent )
  : QWidget(parent)
{
  setupUi( this );

  mp_twMyShares->setRootIsDecorated( false );
  mp_twMyShares->setSortingEnabled( true );

  mp_twLocalShares->setRootIsDecorated( false );
  mp_twLocalShares->setSelectionMode( QAbstractItemView::NoSelection );

  mp_twMyShares->setHeaderLabel( tr( "Share" ) );
  QStringList labels;
  labels << tr( "Filename" ) << tr( "Size" ) << tr( "Path" );
  mp_twLocalShares->setHeaderLabels( labels );
  mp_twLocalShares->setAlternatingRowColors( true );
  mp_twLocalShares->setSortingEnabled( true );

  QHeaderView* header_view = mp_twLocalShares->header();
#if QT_VERSION >= 0x050000
  header_view->setSectionResizeMode( 0, QHeaderView::Stretch );
  header_view->setSectionResizeMode( 1, QHeaderView::ResizeToContents );
  header_view->setSectionResizeMode( 2, QHeaderView::Stretch );
#else
  header_view->setResizeMode( 0, QHeaderView::Stretch );
  header_view->setResizeMode( 1, QHeaderView::ResizeToContents );
  header_view->setResizeMode( 2, QHeaderView::Stretch );
#endif
  header_view->setSortIndicator( 0, Qt::AscendingOrder );

  header_view = mp_twMyShares->header();
  header_view->setSortIndicator( 0, Qt::AscendingOrder );

  mp_pbAddFile->setIconSize( Settings::instance().mainBarIconSize() );
  mp_pbAddFolder->setIconSize( Settings::instance().mainBarIconSize() );
  mp_pbRemove->setIconSize( Settings::instance().mainBarIconSize() );
  mp_pbUpdate->setIconSize( Settings::instance().mainBarIconSize() );

  showStats( 0, 0 );
  connect( mp_pbAddFile, SIGNAL( clicked() ), this, SLOT( addFilePath() ) );
  connect( mp_pbAddFolder, SIGNAL( clicked() ), this, SLOT( addFolderPath() ) );
  connect( mp_pbRemove, SIGNAL( clicked() ), this, SLOT( removePath() ) );
  connect( mp_pbUpdate, SIGNAL( clicked() ), this, SLOT( updateList() ) );
  connect( mp_twLocalShares, SIGNAL( itemDoubleClicked( QTreeWidgetItem*, int ) ), this, SLOT( openItemDoubleClicked( QTreeWidgetItem*, int ) ) );
}

void GuiShareLocal::showStats( int file_count, FileSizeType total_file_size )
{
  if( Settings::instance().fileShare() )
    mp_labelShareStats->setText( QString( "%1: <b>%2</b> (%3)" ).arg( tr( "Shared files" ) ).arg( file_count ).arg( Bee::bytesToString( total_file_size ) ) );
  else
    mp_labelShareStats->setText( QString( "<b>%1</b>" ).arg( "File sharing is disabled" ) );
}

void GuiShareLocal::setActionsEnabled( bool enable )
{
  mp_pbAddFile->setEnabled( enable );
  mp_pbAddFolder->setEnabled( enable );
  mp_pbRemove->setEnabled( enable );
  mp_pbUpdate->setEnabled( enable );
  if( enable )
    setCursor( Qt::ArrowCursor );
  else
    setCursor( Qt::WaitCursor );
}

void GuiShareLocal::addFilePath()
{
  QString file_path = QFileDialog::getOpenFileName( this, tr( "Select a file to share" ),
                                                     Settings::instance().lastDirectorySelected(),
                                                     "", 0, QFileDialog::DontResolveSymlinks );
  if( file_path.isEmpty() )
    return;

  Settings::instance().setLastDirectorySelectedFromFile( file_path );

  addSharePath( file_path );
}

void GuiShareLocal::addFolderPath()
{
  QString folder_path = QFileDialog::getExistingDirectory( this, tr( "Select a folder to share" ),
                                                           Settings::instance().lastDirectorySelected(),
                                                           QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );
  if( folder_path.isEmpty() )
    return;

  Settings::instance().setLastDirectorySelected( folder_path );

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

  setActionsEnabled( false );

  QString share_selected = item_list.first()->text( 0 );

  QStringList local_share = Settings::instance().localShare();
  if( local_share.removeOne( share_selected ) )
    Settings::instance().setLocalShare( local_share );

  updatePaths();

  emit sharePathRemoved( share_selected );
}

void GuiShareLocal::updatePaths()
{
  mp_twMyShares->clear();
  QTreeWidgetItem *item;
  foreach( QString share_path, Settings::instance().localShare() )
  {
    item = new QTreeWidgetItem( mp_twMyShares );
    item->setText( 0, share_path );
  }
}

void GuiShareLocal::updateFileSharedList()
{
  setActionsEnabled( false );
  mp_twLocalShares->clear();
  QTimer::singleShot( 200, this, SLOT( loadFileInfoInList() ) );
}

void GuiShareLocal::loadFileInfoInList()
{
  GuiFileInfoItem *item;
  int file_count = 0;
  FileSizeType total_file_size = 0;

  foreach( FileInfo fi, FileShare::instance().local() )
  {
    file_count++;
    total_file_size += fi.size();
    item = new GuiFileInfoItem( mp_twLocalShares, 1, Qt::UserRole + 1 );
    item->setText( 0, fi.name() );
    item->setIcon( 0, GuiIconProvider::instance().findIcon( fi ) );
    item->setData( 0, Qt::UserRole + 1, fi.path() );
    item->setToolTip( 0, fi.name() );
    item->setText( 1, Bee::bytesToString( fi.size() ) );
    item->setData( 1, Qt::UserRole + 1, fi.size() );
    item->setText( 2, fi.path() );
    item->setToolTip( 2, fi.path() );
  }

  setActionsEnabled( true );
  showStats( file_count, total_file_size );
}

void GuiShareLocal::addSharePath( const QString& share_path )
{
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
  if( Settings::instance().fileShare() )
    return true;

  QMessageBox::information( this, Settings::instance().programName(), tr( "File sharing is disabled. Open the option menu to enable it." ) );
  return false;
}
