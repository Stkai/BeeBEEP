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
  header_view->setSectionResizeMode( QHeaderView::ResizeToContents );
#else
  header_view->setResizeMode( QHeaderView::ResizeToContents );
#endif
  header_view->setSortIndicator( 0, Qt::AscendingOrder );

  header_view = mp_twMyShares->header();
  header_view->setSortIndicator( 0, Qt::AscendingOrder );

  mp_pbAddFile->setIconSize( Settings::instance().mainBarIconSize() );
  mp_pbAddFolder->setIconSize( Settings::instance().mainBarIconSize() );
  mp_pbRemove->setIconSize( Settings::instance().mainBarIconSize() );

  connect( mp_pbAddFile, SIGNAL( clicked() ), this, SLOT( addFilePath() ) );
  connect( mp_pbAddFolder, SIGNAL( clicked() ), this, SLOT( addFolderPath() ) );
  connect( mp_pbRemove, SIGNAL( clicked() ), this, SLOT( removePath() ) );
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
  mp_twLocalShares->clear();
  QTreeWidgetItem *item;
  foreach( FileInfo fi, FileShare::instance().local() )
  {
    item = new QTreeWidgetItem( mp_twLocalShares );
    item->setText( 0, fi.name() );
    item->setIcon( 0, QIcon( Bee::fileTypeIconFileName( Bee::fileTypeFromSuffix( fi.suffix() ) ) ) );
    item->setText( 1, Bee::bytesToString( fi.size() ) );
    item->setText( 2, fi.path() );
  }
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

  emit sharePathAdded( share_path );
}
