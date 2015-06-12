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
#include "GuiIconProvider.h"
#include "GuiFileInfoList.h"
#include "GuiShareNetwork.h"
#include "FileShare.h"
#include "Settings.h"
#include "UserManager.h"


GuiShareNetwork::GuiShareNetwork( QWidget *parent )
  : QWidget( parent ), m_fileInfoList()
{
  setupUi( this );

  setObjectName( "GuiShareNetwork" );
  mp_lTitle->setText( QString( "<b>%1</b>" ).arg( tr( "Folder and Files shared in your network" ) ) );

  m_fileInfoList.initTree( mp_twShares );

  connect( mp_twShares, SIGNAL( itemDoubleClicked( QTreeWidgetItem*, int ) ), this, SLOT( checkItemDoubleClicked( QTreeWidgetItem*, int ) ) );
  connect( mp_twShares, SIGNAL( customContextMenuRequested( const QPoint& ) ), this, SLOT( openDownloadMenu( const QPoint& ) ) );
}

void GuiShareNetwork::setupToolBar( QToolBar* bar )
{
  QLabel *label;

  /* scan button */
  mp_actScan = bar->addAction( QIcon( ":/images/network-scan.png" ), tr( "Scan network" ), this, SLOT( scanNetwork() ) );
  mp_actScan->setStatusTip( tr( "Search shared files in your network" ) );

  /* Reload button */
  mp_actReload = bar->addAction( QIcon( ":/images/update.png" ), tr( "Reload list" ), this, SLOT( reloadList() ) );
  mp_actReload->setStatusTip( tr( "Clear and reload list" ) );
  mp_actReload->setEnabled( false );

  /* filter by keywords */
  label = new QLabel( bar );
  label->setObjectName( "GuiLabelFilter" );
  label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
  label->setText( QString( "   " ) + tr( "Filter" ) + QString( " " ) );
  bar->addWidget( label );
  mp_leFilter = new QLineEdit( bar );
  mp_leFilter->setObjectName( "GuiLineEditFilter" );
  mp_leFilter->setMaximumWidth( 140 );
  bar->addWidget( mp_leFilter );
  connect( mp_leFilter, SIGNAL( textChanged( QString ) ), this, SLOT( enableFilterButton() ) );
  connect( mp_leFilter, SIGNAL( returnPressed() ), this, SLOT( applyFilter() ) );
  mp_actFilter = bar->addAction( QIcon( ":/images/filter.png" ), tr( "Apply Filter" ), this, SLOT( applyFilter() ) );
  mp_actFilter->setStatusTip( tr( "Filter the files in list using some keywords" ) );
  mp_actFilter->setEnabled( false );

  /* filter by file type */
  label = new QLabel( bar );
  label->setObjectName( "GuiLabelFilterFileType" );
  label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
  label->setText( QString( "   " ) + tr( "File Type" ) + QString( " " ) );
  bar->addWidget( label );
  mp_comboFileType = new QComboBox( bar );
  mp_comboFileType->setObjectName( "GuiComboBoxFilterFileType" );
  for( int i = Bee::FileAudio; i < Bee::NumFileType; i++ )
    mp_comboFileType->insertItem( i, GuiIconProvider::instance().iconFromFileType( i ), Bee::fileTypeToString( (Bee::FileType)i ), i );
  mp_comboFileType->insertItem( Bee::NumFileType, QIcon( ":/images/star.png" ), tr( "All Files" ), Bee::NumFileType );
  mp_comboFileType->setCurrentIndex( Bee::NumFileType );
  bar->addWidget( mp_comboFileType );
  connect( mp_comboFileType, SIGNAL( currentIndexChanged( int ) ), this, SLOT( applyFilter() ), Qt::QueuedConnection );

  /* filter by user */
  label = new QLabel( bar );
  label->setObjectName( "GuiLabelFilterUser" );
  label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
  label->setText( QString( "   " ) + tr( "User" ) + QString( " " ) );
  label->setMinimumWidth( 40 );
  bar->addWidget( label );
  mp_comboUsers = new QComboBox( bar );
  mp_comboUsers->setObjectName( "GuiComboBoxFilterUser" );
  mp_comboUsers->setMinimumSize( QSize( 100, 0 ) );
  resetComboUsers();
  bar->addWidget( mp_comboUsers );
  connect( mp_comboUsers, SIGNAL( currentIndexChanged( int ) ), this, SLOT( applyFilter() ), Qt::QueuedConnection );

  /* Download button */
  bar->addSeparator();
  mp_actDownload = bar->addAction( QIcon( ":/images/download-box.png" ), tr( "Download" ), this, SLOT( downloadSelected() ) );
  mp_actDownload->setStatusTip( tr( "Download single or multiple files simultaneously" ) );
}

void GuiShareNetwork::resetComboUsers()
{
  if( mp_comboUsers->count() > 0 )
    mp_comboUsers->clear();

  mp_comboUsers->insertItem( 0, tr( "All Users" ), 0 );
  mp_comboUsers->setCurrentIndex( 0 );
  mp_comboUsers->setEnabled( false );
}

void GuiShareNetwork::initShares()
{
  if( FileShare::instance().network().isEmpty() )
    QTimer::singleShot( 200, this, SLOT( scanNetwork() ) );
  mp_leFilter->setFocus();
}

void GuiShareNetwork::enableFilterButton()
{
  mp_actFilter->setEnabled( true );
}

void GuiShareNetwork::enableScanButton()
{
  mp_actScan->setEnabled( true );
}

void GuiShareNetwork::scanNetwork()
{
  resetComboUsers();
  m_fileInfoList.clearTree();
  mp_actScan->setDisabled( true );
  mp_actReload->setEnabled( true );
  showStatus( tr( "%1 is searching shared files in your network" ).arg( Settings::instance().programName() ) );
  emit fileShareListRequested();
  QTimer::singleShot( 30000, this, SLOT( enableScanButton() ) );
}

void GuiShareNetwork::applyFilter()
{
  mp_actFilter->setDisabled( true );
  if( mp_actReload->isEnabled() )
    reloadList();
  else
    QTimer::singleShot( 200, this, SLOT( updateList() ) );
}

void GuiShareNetwork::reloadList()
{
  mp_actReload->setDisabled( true );
  QTimer::singleShot( 200, this, SLOT( updateList() ) );
}

void GuiShareNetwork::loadShares( const User& u )
{
  int file_shared = 0;
  int file_shared_visible = 0;
  FileSizeType share_size = 0;

  if( u.isConnected() )
  {
    GuiFileInfoItem *item;
    FileInfo file_info_downloaded;

    foreach( FileInfo fi, FileShare::instance().network().values( u.id() ) )
    {
      if( fi.isValid() )
      {
        if( filterPassThrough( u.id(), fi ) )
        {
          item = m_fileInfoList.fileItem( u.id(), fi.id() );
          if( !item )
            item = m_fileInfoList.createFileItem( u, fi );

          file_info_downloaded = FileShare::instance().downloadedFile( fi.fileHash() );
          if( file_info_downloaded.isValid() )
          {
            showFileTransferCompleted( item, file_info_downloaded.path() );
          }
          else
          {
            item->setFilePath( "" );
            item->setToolTip( GuiFileInfoItem::ColumnFile, tr( "Double click to download %1" ).arg( fi.name() ) );
          }
          file_shared_visible++;
        }

        file_shared++;
        share_size += fi.size();
      }
    }
  }

  if( file_shared > 0 )
  {
    if( mp_comboUsers->findData( u.id() ) == -1 )
      mp_comboUsers->addItem( u.name(), u.id() );
  }
  else
  {
    int user_id_index_to_remove = mp_comboUsers->findData( u.id() );
    if( user_id_index_to_remove > 0 )
    {
      if( user_id_index_to_remove == mp_comboUsers->currentIndex() )
        mp_comboUsers->setCurrentIndex( 0 );
      mp_comboUsers->removeItem( user_id_index_to_remove );
    }
  }

  mp_comboUsers->setEnabled( mp_comboUsers->count() > 1 );
  showStatus( tr( "%1 has shared %2 files (%3)" ).arg( u.name() ).arg( file_shared ).arg( Bee::bytesToString( share_size ) ) );
}

void GuiShareNetwork::checkItemDoubleClicked( QTreeWidgetItem* item, int )
{
  if( !item )
    return;

  GuiFileInfoItem* file_info_item = (GuiFileInfoItem*)item;

  if( !file_info_item->isObjectFile() )
    return;

  if( !file_info_item->filePath().isEmpty() )
    emit openFileCompleted( QUrl::fromLocalFile( file_info_item->filePath() ) );
  else
    emit downloadSharedFile( file_info_item->userId(), file_info_item->fileInfoId() );
}

void GuiShareNetwork::updateList()
{
  setCursor( Qt::WaitCursor );
  m_fileInfoList.clearTree();

  foreach( User u, UserManager::instance().userList().toList() )
    loadShares( u );

  mp_twShares->expandAll();
  setCursor( Qt::ArrowCursor );
  showStatus( "" );
}

bool GuiShareNetwork::filterPassThrough( VNumber user_id, const FileInfo& fi )
{
  QString filter_name = mp_leFilter->text().simplified();
  if( filter_name == QString( "*" ) || filter_name == QString( "*.*" ) )
    filter_name = "";

  if( !filter_name.isEmpty() )
  {
    QStringList filter_name_list = filter_name.split( QString( " " ), QString::SkipEmptyParts );
    foreach( QString filter_name_item, filter_name_list )
    {
      if( !fi.name().contains( filter_name_item, Qt::CaseInsensitive ) )
        return false;
    }
  }

  VNumber filter_user_id = mp_comboUsers->currentIndex() <= 0 ? 0 : Bee::qVariantToVNumber( mp_comboUsers->itemData( mp_comboUsers->currentIndex() ) );
  if( filter_user_id > 0 && user_id != filter_user_id )
    return false;

  if( mp_comboFileType->currentIndex() == (int)Bee::NumFileType )
    return true;
  else
    return mp_comboFileType->currentIndex() == (int)Bee::fileTypeFromSuffix( fi.suffix() );
}

void GuiShareNetwork::showMessage( VNumber user_id, VNumber file_info_id, const QString& msg )
{
  GuiFileInfoItem* item = m_fileInfoList.fileItem( user_id, file_info_id );
  if( !item )
    return;

  item->setText( GuiFileInfoItem::ColumnStatus, msg );
}

void GuiShareNetwork::setFileTransferCompleted( VNumber user_id, VNumber file_info_id, const QString& file_path )
{
  GuiFileInfoItem* item = m_fileInfoList.fileItem( user_id, file_info_id );
  if( !item )
    return;

  showFileTransferCompleted( item, file_path );
}

void GuiShareNetwork::showFileTransferCompleted( GuiFileInfoItem* item, const QString& file_path )
{
  item->setFilePath( file_path );
  item->setToolTip( GuiFileInfoItem::ColumnFile, tr( "Double click to open %1" ).arg( file_path ) );
  if( item->text( GuiFileInfoItem::ColumnStatus ).isEmpty() )
    item->setText( GuiFileInfoItem::ColumnStatus, tr( "Transfer completed" ) );
  for( int i = 0; i < mp_twShares->columnCount(); i++ )
    item->setBackgroundColor( i, QColor( "#91D606" ) );
}

void GuiShareNetwork::showStatus( const QString& status_text )
{
  if( status_text.isEmpty() )
  {
    int share_size = FileShare::instance().network().size();
    int num_items_visible = mp_twShares->topLevelItemCount();

    QString status_msg;
    if( share_size != num_items_visible )
      status_msg = tr( "%1 files are shown in list (%2 are available in your network)" ).arg( num_items_visible ).arg( share_size );
    else
      status_msg = tr( "%1 files shared in your network" ).arg( share_size );

    emit updateStatus( status_msg, 0 );
  }
  else
  {
    emit updateStatus( status_text, 0 );
  }
}

void GuiShareNetwork::showSharesForUser( const User& u )
{
  if( FileShare::instance().network().count( u.id() ) > 0 && mp_twShares->topLevelItemCount() == 0 )
  {
    QTimer::singleShot( 200, this, SLOT( updateList() ) );
  }
  else
  {
    if( mp_comboUsers->findData( u.id() ) == -1 )
      loadShares( u );
    else
      mp_actReload->setEnabled( true );
  }

  showStatus( "" );
}

void GuiShareNetwork::openDownloadMenu( const QPoint& )
{
  int selected_items = m_fileInfoList.parseSelectedItems();
  QMenu menu;

  if( selected_items )
  {
    QString action_text = selected_items == 1 ? tr( "Download single file" ) : tr( "Download %1 selected files" ).arg( selected_items );
    menu.addAction( QIcon( ":/images/download.png" ), action_text, this, SLOT( downloadSelected() ) );
    menu.addSeparator();
    menu.addAction( QIcon( ":/images/clear.png" ), tr( "Clear selection" ), &m_fileInfoList, SLOT( clearTreeSelection() ) );
    menu.addSeparator();
  }

  menu.addAction( QIcon( ":/images/add.png" ), tr( "Expand all items" ), mp_twShares, SLOT( expandAll() ) );
  menu.addAction( QIcon( ":/images/remove.png" ), tr( "Collapse all items" ), mp_twShares, SLOT( collapseAll() ) );
  menu.exec( QCursor::pos() );
}

void GuiShareNetwork::downloadSelected()
{
  if( m_fileInfoList.selectedFileInfoList().isEmpty() )
  {
    int selected_items = m_fileInfoList.parseSelectedItems();
    if( selected_items <= 0 )
    {
      QMessageBox::information( this, Settings::instance().programName(), tr( "Please select one or more files to download." ) );
      return;
    }
  }

  QList<SharedFileInfo> selected_items = m_fileInfoList.selectedFileInfoList();
  if( selected_items.size() == 1 )
    emit downloadSharedFile( selected_items.first().first, selected_items.first().second.id() );
  else
    emit downloadSharedFiles( selected_items );
}
