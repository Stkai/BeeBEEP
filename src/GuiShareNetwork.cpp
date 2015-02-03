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
#include "GuiShareNetwork.h"
#include "FileShare.h"
#include "Settings.h"
#include "UserManager.h"


GuiShareNetwork::GuiShareNetwork( QWidget *parent )
  : QWidget(parent)
{
  setupUi( this );

  setObjectName( "GuiShareNetwork" );
  QStringList labels;
  labels << tr( "File" ) << tr( "Size" ) << tr( "User" ) << tr( "Status" );
  mp_twShares->setHeaderLabels( labels );

  mp_twShares->sortItems( ColumnFile, Qt::AscendingOrder );

  mp_twShares->setAlternatingRowColors( true );
  mp_twShares->setRootIsDecorated( false );

  QHeaderView* hv = mp_twShares->header();
#if QT_VERSION >= 0x050000
  hv->setSectionResizeMode( ColumnFile, QHeaderView::Stretch );
  hv->setSectionResizeMode( ColumnSize, QHeaderView::ResizeToContents );
  hv->setSectionResizeMode( ColumnUser, QHeaderView::ResizeToContents );
  hv->setSectionResizeMode( ColumnStatus, QHeaderView::ResizeToContents );
#else
  hv->setResizeMode( ColumnFile, QHeaderView::Stretch );
  hv->setResizeMode( ColumnSize, QHeaderView::ResizeToContents );
  hv->setResizeMode( ColumnUser, QHeaderView::ResizeToContents );
  hv->setResizeMode( ColumnStatus, QHeaderView::ResizeToContents );
#endif

  connect( mp_twShares, SIGNAL( itemDoubleClicked( QTreeWidgetItem*, int ) ), this, SLOT( checkItemDoubleClicked( QTreeWidgetItem*, int ) ) );
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
  mp_leFilter->setMaximumWidth( 200 );
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
  mp_comboUsers->insertItem( 0, tr( "All Users" ), 0 );
  mp_comboUsers->setCurrentIndex( 0 );
  mp_comboUsers->setEnabled( false );
  bar->addWidget( mp_comboUsers );
  connect( mp_comboUsers, SIGNAL( currentIndexChanged( int ) ), this, SLOT( applyFilter() ), Qt::QueuedConnection );

}

void GuiShareNetwork::initShares()
{
  if( FileShare::instance().network().isEmpty() )
    QTimer::singleShot( 200, this, SLOT( scanNetwork() ) );
  if( countVisibleItems() > 0 )
    mp_leFilter->setFocus();
}

void GuiShareNetwork::enableScanButton()
{
  mp_actScan->setEnabled( true );
}

void GuiShareNetwork::enableFilterButton()
{
  mp_actFilter->setEnabled( true );
}

void GuiShareNetwork::enableReloadButton()
{
  mp_actReload->setEnabled( true );
}

void GuiShareNetwork::scanNetwork()
{
  mp_actScan->setDisabled( true );
  mp_actReload->setEnabled( true );
  QTimer::singleShot( 10000, this, SLOT( enableScanButton() ) );
  showStatus( tr( "%1 is searching shared files in your network" ).arg( Settings::instance().programName() ) );
  emit fileShareListRequested();
}

void GuiShareNetwork::applyFilter()
{
  mp_actFilter->setDisabled( true );
  QTimer::singleShot( 200, this, SLOT( updateList() ) );
}

void GuiShareNetwork::reloadList()
{
  mp_actReload->setDisabled( true );
  mp_twShares->clear();
  QTimer::singleShot( 10000, this, SLOT( enableReloadButton() ) );
  QTimer::singleShot( 200, this, SLOT( updateList() ) );
}

void GuiShareNetwork::loadShares( const User& u )
{
  int file_shared = 0;
  FileSizeType share_size = 0;

  if( u.isConnected() )
  {
    GuiFileInfoItem *item;

    foreach( FileInfo fi, FileShare::instance().network().values( u.id() ) )
    {
      if( !fi.isValid() )
        continue;

      item = findItem( u.id(), fi.id() );

      if( !item )
      {
        item = new GuiFileInfoItem( mp_twShares, ColumnSize, FileSize );
        item->setIcon( ColumnFile, GuiIconProvider::instance().findIcon( fi ) );
        item->setText( ColumnFile, fi.name() );
        item->setData( ColumnFile, UserId, u.id() );
        item->setData( ColumnFile, FileId, fi.id() );
        item->setData( ColumnFile, FilePath, QString( "" ) );
        item->setText( ColumnSize, Bee::bytesToString( fi.size() ) );
        item->setData( ColumnSize, FileSize, fi.size() );
        item->setText( ColumnUser, u.name() );
        item->setToolTip( ColumnFile, tr( "Double click to download %1" ).arg( fi.name() ) );
      }

      if( filterPassThrough( u, fi ) )
        item->setHidden( false );
      else
        item->setHidden( true );

      file_shared++;
      share_size += fi.size();
    }
  }
  else
  {
    QTreeWidgetItemIterator it( mp_twShares );
    while( *it )
    {
      if( Bee::qVariantToVNumber( (*it)->data( ColumnFile, UserId ) ) == u.id() )
        (*it)->setHidden( true );
      ++it;
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
    if( user_id_index_to_remove > 0 && user_id_index_to_remove != mp_comboUsers->currentIndex() )
      mp_comboUsers->removeItem( user_id_index_to_remove );
  }

  mp_comboUsers->setEnabled( mp_comboUsers->count() > 1 );
  showStatus( tr( "%1 has shared %2 files (%3)" ).arg( u.name() ).arg( file_shared ).arg( Bee::bytesToString( share_size ) ) );
}

void GuiShareNetwork::checkItemDoubleClicked( QTreeWidgetItem* item, int )
{
  if( !item )
    return;

  QString file_path = item->data( ColumnFile, FilePath ).toString();
  if( !file_path.isEmpty() )
  {
    emit openFileCompleted( QUrl::fromLocalFile( file_path ) );
    return;
  }

  VNumber user_id = Bee::qVariantToVNumber( item->data( ColumnFile, UserId ) );
  VNumber file_id = Bee::qVariantToVNumber( item->data( ColumnFile, FileId ) );

  emit downloadSharedFile( user_id, file_id );
}

void GuiShareNetwork::updateList()
{
  setCursor( Qt::WaitCursor );

  foreach( User u, UserManager::instance().userList().toList() )
   loadShares( u );

  setCursor( Qt::ArrowCursor );
  showStatus( "" );
}

bool GuiShareNetwork::filterPassThrough( const User& u, const FileInfo& fi )
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
  if( filter_user_id > 0 && u.id() != filter_user_id )
    return false;

  if( mp_comboFileType->currentIndex() == (int)Bee::NumFileType )
    return true;
  else
    return mp_comboFileType->currentIndex() == (int)Bee::fileTypeFromSuffix( fi.suffix() );
}

GuiFileInfoItem* GuiShareNetwork::findItem( VNumber user_id, VNumber file_info_id )
{
  QTreeWidgetItemIterator it( mp_twShares );
  while( *it )
  {
    if( Bee::qVariantToVNumber( (*it)->data( ColumnFile, UserId ) ) == user_id
        && Bee::qVariantToVNumber( (*it)->data( ColumnFile, FileId ) ) == file_info_id )
      return (GuiFileInfoItem*)(*it);
    ++it;
  }
  return 0;
}

void GuiShareNetwork::showMessage( VNumber user_id, VNumber file_info_id, const QString& msg )
{
  GuiFileInfoItem* item = findItem( user_id, file_info_id );
  if( !item )
    return;

  item->setText( ColumnStatus, msg );
}

void GuiShareNetwork::setFileTransferCompleted( VNumber user_id, VNumber file_info_id, const QString& file_path )
{
  GuiFileInfoItem* item = findItem( user_id, file_info_id );
  if( !item )
    return;

  item->setData( ColumnFile, FilePath, file_path );
  item->setToolTip( ColumnFile, tr( "Double click to open %1" ).arg( file_path ) );
  for( int i = 0; i < mp_twShares->columnCount(); i++ )
    item->setBackgroundColor( i, QColor( "#91D606" ) );
}

int GuiShareNetwork::countVisibleItems() const
{
  int counter = 0;
  QTreeWidgetItemIterator it( mp_twShares );
  while( *it )
  {
    if( !(*it)->isHidden() )
      counter++;
    ++it;
  }
  return counter;
}

void GuiShareNetwork::showStatus( const QString& status_text )
{
  if( status_text.isEmpty() )
  {
    int share_size = FileShare::instance().network().size();
    int num_items_visible = countVisibleItems();

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
