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

  /* Download button */
  mp_actDownload = bar->addAction( QIcon( ":/images/download-box.png" ), tr( "Download" ), this, SLOT( downloadSelected() ) );
  mp_actDownload->setStatusTip( tr( "Download single or multiple files simultaneously" ) );

  /* filter by keywords */
  label = new QLabel( bar );
  label->setObjectName( "GuiLabelFilterText" );
  label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
  label->setText( QString( "   " ) + tr( "Filter" ) + QString( " " ) );
  bar->addWidget( label );
  mp_leFilter = new QLineEdit( bar );
  mp_leFilter->setObjectName( "GuiLineEditFilter" );
  mp_leFilter->setMaximumWidth( 140 );
  mp_leFilter->setPlaceholderText( tr( "Search" ) );
  bar->addWidget( mp_leFilter );
  connect( mp_leFilter, SIGNAL( textChanged( const QString& ) ), this, SLOT( filterByText( const QString& ) ) );

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

}

void GuiShareNetwork::resetComboUsers()
{
  mp_comboUsers->blockSignals( true );
  if( mp_comboUsers->count() > 0 )
    mp_comboUsers->clear();

  mp_comboUsers->insertItem( 0, tr( "All Users" ), 0 );
  mp_comboUsers->setCurrentIndex( 0 );
  mp_comboUsers->setEnabled( false );
  mp_comboUsers->blockSignals( false );
}

void GuiShareNetwork::initShares()
{
  if( FileShare::instance().network().isEmpty() )
    QTimer::singleShot( 200, this, SLOT( scanNetwork() ) );
  mp_leFilter->setFocus();
}

void GuiShareNetwork::enableScanButton()
{
  if( !mp_actScan->isEnabled() )
    mp_actScan->setEnabled( true );
}

void GuiShareNetwork::scanNetwork()
{
  resetComboUsers();
  m_fileInfoList.clearTree();
  mp_actScan->setEnabled( false );
  mp_actReload->setEnabled( true );
  showStatus( tr( "%1 is searching shared files in your network" ).arg( Settings::instance().programName() ) );
  emit fileShareListRequested();
  QTimer::singleShot( 30000, this, SLOT( enableScanButton() ) );
}

void GuiShareNetwork::applyFilter()
{
  if( mp_actReload->isEnabled() )
    reloadList();
  else
    updateList();
}

void GuiShareNetwork::reloadList()
{
  mp_actReload->setEnabled( false );
  updateList();
}

void GuiShareNetwork::loadShares( const User& u )
{
  setCursor( Qt::WaitCursor );
  QApplication::processEvents();

  m_fileInfoList.setUpdatesEnabled( false );
  int file_shared = 0;
  FileSizeType share_size = 0;
  QTime timer;
  timer.start();

  GuiFileInfoItem *item = m_fileInfoList.userItem( u.id() );
  if( item )
    item->removeChildren();

  if( u.isConnected() )
  {
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
        }

        file_shared++;
        share_size += fi.size();

        if( timer.elapsed() > 20000 )
        {
          qWarning() << "File share operation is too long, time out!";
          break;
        }
      }
    }
  }

  m_fileInfoList.setUpdatesEnabled( true );

  mp_comboUsers->blockSignals( true );
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
  mp_comboUsers->blockSignals( false );
  mp_actDownload->setEnabled( !m_fileInfoList.isEmpty() );

  QString status_msg = tr( "%1 has shared %2 files (%3)" ).arg( u.name() ).arg( file_shared ).arg( Bee::bytesToString( share_size ) );
#ifdef BEEBEEP_DEBUG
  qDebug() << qPrintable( status_msg );
#endif
  showStatus( status_msg );
  setCursor( Qt::ArrowCursor );
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
  m_fileInfoList.clearTree();
  foreach( User u, UserManager::instance().userList().toList() )
    loadShares( u );

  if( m_fileInfoList.countFileItems() < 100 )
    mp_twShares->expandAll();
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
    int file_items = m_fileInfoList.countFileItems();

    QString status_msg;
    if( share_size != file_items )
      status_msg = tr( "%1 files are shown in list (%2 are available in your network)" ).arg( file_items ).arg( share_size );
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
  if( mp_comboUsers->findData( u.id() ) == -1 )
  {
    loadShares( u );
    if( m_fileInfoList.countFileItems() < 100 )
      mp_twShares->expandAll();
  }
  else
    mp_actReload->setEnabled( true );
}

void GuiShareNetwork::openDownloadMenu( const QPoint& )
{
  int selected_items = m_fileInfoList.parseSelectedItems();
  QMenu menu;

  if( selected_items )
  {
    QString action_text = selected_items == 1 ? tr( "Download single file" ) : tr( "Download %1 selected files" ).arg( selected_items );
    if( selected_items >= Settings::instance().maxQueuedDownloads() )
      action_text += QString( " (%1 %2)" ).arg( Settings::instance().maxQueuedDownloads() ).arg( tr( "MAX" ) );
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

void GuiShareNetwork::filterByText( const QString& )
{
  updateList();
}
