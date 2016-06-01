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
#include "GuiShareBox.h"
#include "GuiShareBoxFileInfoList.h"
#include "FileShare.h"
#include "Settings.h"
#include "UserManager.h"


GuiShareBox::GuiShareBox( QWidget *parent )
  : QWidget( parent ), m_fileInfoList()
{
  setupUi( this );

  setObjectName( "GuiShareBox" );
  mp_lTitle->setText( QString( "<b>%1</b>" ).arg( tr( "Shared box" ) ) );

  m_fileInfoList.initTree( mp_twBox, false );

  connect( mp_twBox, SIGNAL( itemDoubleClicked( QTreeWidgetItem*, int ) ), this, SLOT( checkItemDoubleClicked( QTreeWidgetItem*, int ) ) );
  connect( mp_twBox, SIGNAL( customContextMenuRequested( const QPoint& ) ), this, SLOT( openDownloadMenu( const QPoint& ) ) );
}

void GuiShareBox::setupToolBar( QToolBar* bar )
{
  QLabel *label;

  /* Update button */
  mp_actUpdate = bar->addAction( QIcon( ":/images/update.png" ), tr( "Update" ), this, SLOT( updateBox() ) );
  mp_actUpdate->setEnabled( false );

  /* Download button */
  mp_actDownload = bar->addAction( QIcon( ":/images/download-box.png" ), tr( "Download" ), this, SLOT( downloadSelected() ) );

  /* Upload button */
  mp_actUpload = bar->addAction( QIcon( ":/images/upload-box.png" ), tr( "Upload" ), this, SLOT( downloadSelected() ) );

  /* filter by keywords */
  label = new QLabel( bar );
  label->setObjectName( "GuiLabelFilterText" );
  label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
  label->setText( QString( "   " ) + tr( "Filter" ) + QString( " " ) );
  bar->addWidget( label );
  mp_leFilter = new QLineEdit( bar );
  mp_leFilter->setObjectName( "GuiLineEditFilter" );
  mp_leFilter->setMaximumWidth( 140 );
#if QT_VERSION >= 0x040700
  mp_leFilter->setPlaceholderText( tr( "Search" ) );
#endif
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
  //connect( mp_comboFileType, SIGNAL( currentIndexChanged( int ) ), this, SLOT( applyFilter() ), Qt::QueuedConnection );

  /* filter by user */
  label = new QLabel( bar );
  label->setObjectName( "GuiLabelFilterUser" );
  label->setAlignment( Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter );
  label->setText( QString( "   " ) + tr( "User" ) + QString( " " ) );
  label->setMinimumWidth( 40 );
  bar->addWidget( label );
  mp_comboUsers = new QComboBox( bar );
  mp_comboUsers->setObjectName( "GuiComboBoxFilterUser" );
  mp_comboUsers->setMinimumSize( QSize( 100, 0 ) );
  resetComboUsers();
  bar->addWidget( mp_comboUsers );
  //connect( mp_comboUsers, SIGNAL( currentIndexChanged( int ) ), this, SLOT( applyFilter() ), Qt::QueuedConnection );

}

void GuiShareBox::resetComboUsers()
{
  mp_comboUsers->blockSignals( true );
  if( mp_comboUsers->count() > 0 )
    mp_comboUsers->clear();

  mp_comboUsers->insertItem( 0, tr( "You" ), ID_LOCAL_USER );
  mp_comboUsers->setCurrentIndex( 0 );
  mp_comboUsers->setEnabled( false );
  mp_comboUsers->blockSignals( false );
}

void GuiShareBox::enableUpdateButton()
{
  if( !mp_actUpdate->isEnabled() )
    mp_actUpdate->setEnabled( true );
}

void GuiShareBox::updateBox()
{
  mp_actUpdate->setEnabled( false );
  emit shareBoxRequest( ID_LOCAL_USER, "test" );
  QTimer::singleShot( 10000, this, SLOT( enableUpdateButton() ) );
}

void GuiShareBox::updateBox( VNumber user_id, const QList<FileInfo>& file_info_list )
{

}

void GuiShareBox::loadShareBox( const User& u )
{
  setCursor( Qt::WaitCursor );
  QApplication::processEvents();

  m_fileInfoList.setUpdatesEnabled( false );
  int file_shared = 0;
  FileSizeType share_size = 0;
  QTime timer;
  timer.start();

  GuiShareBoxFileInfoItem *item = m_fileInfoList.userItem( u.id() );
  if( item )
    item->removeChildren();

  if( u.isStatusConnected() )
  {
    FileInfo file_info_downloaded;

    foreach( FileInfo fi, FileShare::instance().network().values( u.id() ) )
    {
      if( fi.isValid() )
      {
        if( filterPassThrough( u.id(), fi ) )
        {
          item = m_fileInfoList.createFileItem( u, fi );

          file_info_downloaded = FileShare::instance().downloadedFile( fi.fileHash() );
          if( file_info_downloaded.isValid() )
          {
            showFileTransferCompleted( item, file_info_downloaded.path() );
          }
          else
          {
            item->setFilePath( "" );
            item->setToolTip( GuiShareBoxFileInfoItem::ColumnFile, tr( "Double click to download %1" ).arg( fi.name() ) );
          }
        }

        file_shared++;
        share_size += fi.size();

        if( timer.elapsed() > 10000 )
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

void GuiShareBox::checkItemDoubleClicked( QTreeWidgetItem* item, int )
{
  if( !item )
    return;

  GuiShareBoxFileInfoItem* file_info_item = (GuiShareBoxFileInfoItem*)item;

  if( !file_info_item->isObjectFile() )
    return;

  if( !file_info_item->filePath().isEmpty() )
    emit openFileCompleted( QUrl::fromLocalFile( file_info_item->filePath() ) );
  else
    emit downloadSharedFile( file_info_item->userId(), file_info_item->fileInfoId() );
}

bool GuiShareBox::filterPassThrough( VNumber user_id, const FileInfo& fi )
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

void GuiShareBox::showFileTransferCompleted( GuiShareBoxFileInfoItem* item, const QString& file_path )
{
  item->setFilePath( file_path );
  item->setToolTip( GuiShareBoxFileInfoItem::ColumnFile, tr( "Double click to open %1" ).arg( file_path ) );
  if( item->text( GuiShareBoxFileInfoItem::ColumnStatus ).isEmpty() )
    item->setText( GuiShareBoxFileInfoItem::ColumnStatus, tr( "Transfer completed" ) );
  for( int i = 0; i < mp_twBox->columnCount(); i++ )
    item->setBackgroundColor( i, QColor( "#91D606" ) );
}

void GuiShareBox::showStatus( const QString& status_text )
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

void GuiShareBox::showBoxForUser( const User& u )
{
  bool user_is_not_in_list = mp_comboUsers->findData( u.id() ) == -1;
  int num_file_shared = FileShare::instance().network().count( u.id() );

  if( user_is_not_in_list || num_file_shared < 999 )
  {
    loadShareBox( u );
    if( m_fileInfoList.countFileItems() < 100 )
      mp_twBox->expandAll();
  }
  else
    mp_actUpdate->setEnabled( true );
}

void GuiShareBox::openDownloadMenu( const QPoint& p )
{
  QTreeWidgetItem* item = mp_twBox->itemAt( p );
  int selected_items;
  if( item )
  {
    if( !item->isSelected() )
      item->setSelected( true );
    selected_items = m_fileInfoList.parseSelectedItems();
  }
  else
    selected_items = 0;

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

  menu.addAction( QIcon( ":/images/add.png" ), tr( "Expand all items" ), mp_twBox, SLOT( expandAll() ) );
  menu.addAction( QIcon( ":/images/remove.png" ), tr( "Collapse all items" ), mp_twBox, SLOT( collapseAll() ) );
  menu.exec( QCursor::pos() );
}

void GuiShareBox::downloadSelected()
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

void GuiShareBox::filterByText( const QString& )
{
}
