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
#include "GuiFileTransfer.h"
#include "GuiFileTransferItem.h"
#include "FileInfo.h"
#include "IconManager.h"
#include "Settings.h"
#include "User.h"


GuiFileTransfer::GuiFileTransfer( QWidget *parent )
 : QTreeWidget( parent )
{
  setObjectName( "GuiFileTransfer" );
  QStringList labels;
  labels << "" << "%" << tr( "Time left" ) << tr( "File" ) << tr( "User" ) << tr( "Status" ) << "";
  setHeaderLabels( labels );
  setRootIsDecorated( false );
  setSortingEnabled( false );
  setColumnHidden( GuiFileTransferItem::ColumnSort, true );
  sortItems( GuiFileTransferItem::ColumnSort, Qt::DescendingOrder );
  setContextMenuPolicy( Qt::CustomContextMenu );

  QHeaderView* hv = header();
#if QT_VERSION >= 0x050000
  hv->setSectionResizeMode( GuiFileTransferItem::ColumnCancel, QHeaderView::Fixed );
  hv->setSectionResizeMode( GuiFileTransferItem::ColumnReport, QHeaderView::ResizeToContents );
  hv->setSectionResizeMode( GuiFileTransferItem::ColumnFile, QHeaderView::ResizeToContents );
  hv->setSectionResizeMode( GuiFileTransferItem::ColumnUser, QHeaderView::ResizeToContents );
  hv->setSectionResizeMode( GuiFileTransferItem::ColumnTimeLeft, QHeaderView::ResizeToContents );
  hv->setSectionResizeMode( GuiFileTransferItem::ColumnProgress, QHeaderView::Stretch );
#else
  hv->setResizeMode( GuiFileTransferItem::ColumnCancel, QHeaderView::Fixed );
  hv->setResizeMode( GuiFileTransferItem::ColumnReport, QHeaderView::ResizeToContents );
  hv->setResizeMode( GuiFileTransferItem::ColumnFile, QHeaderView::ResizeToContents );
  hv->setResizeMode( GuiFileTransferItem::ColumnUser, QHeaderView::ResizeToContents );
  hv->setResizeMode( GuiFileTransferItem::ColumnTimeLeft, QHeaderView::ResizeToContents );
  hv->setResizeMode( GuiFileTransferItem::ColumnProgress, QHeaderView::Stretch );
#endif

  setColumnWidth( GuiFileTransferItem::ColumnCancel, 32 );
  setColumnWidth( GuiFileTransferItem::ColumnReport, 48 );
  setColumnWidth( GuiFileTransferItem::ColumnTimeLeft, 40 );
  hv->hide();

  mp_menuContext = new QMenu( this );
  mp_menuContext->addAction( IconManager::instance().icon( "remove.png" ), tr( "Remove all completed transfers" ), this, SLOT( removeAllCompleted() ) );

  connect( this, SIGNAL( itemClicked( QTreeWidgetItem*, int ) ), this, SLOT( checkItemClicked( QTreeWidgetItem*, int ) ) );
  connect( this, SIGNAL( itemDoubleClicked( QTreeWidgetItem*, int ) ), this, SLOT( checkItemDoubleClicked( QTreeWidgetItem*, int ) ) );
  connect( this, SIGNAL( customContextMenuRequested( const QPoint& ) ), this, SLOT( openMenu( const QPoint& ) ) );
}

void GuiFileTransfer::updateUser( const User& u )
{
  QTreeWidgetItemIterator it( this );
  while( *it )
  {
    GuiFileTransferItem* item = reinterpret_cast<GuiFileTransferItem*>( *it );
    if( item && item->userId() == u.id() )
      item->updateUser( u );
    ++it;
  }
}

GuiFileTransferItem* GuiFileTransfer::findItem( VNumber peer_id )
{
  QTreeWidgetItemIterator it( this );
  while( *it )
  {
    GuiFileTransferItem* item = reinterpret_cast<GuiFileTransferItem*>( *it );
    if( item && item->peerId() == peer_id )
      return item;
    ++it;
  }
  return Q_NULLPTR;
}

GuiFileTransferItem* GuiFileTransfer::createItem( VNumber peer_id, const User& u, const FileInfo& fi )
{
  QHeaderView* hv = header();
  if( hv->isHidden() )
    hv->show();

  /* clean similar stopped items */
  QList<QTreeWidgetItem*> items_to_clear;
  QTreeWidgetItemIterator it( this );
  while( *it )
  {
    GuiFileTransferItem* item = reinterpret_cast<GuiFileTransferItem*>( *it );
    if( item && item->fileInfo().fileHash() == fi.fileHash() && item->userId() == u.id() && item->isStopped() )
      items_to_clear.append( *it );
    ++it;
  }

  foreach( QTreeWidgetItem* item_to_clear, items_to_clear )
  {
    int index_to_clear = indexOfTopLevelItem( item_to_clear );
    if( index_to_clear >= 0 )
      takeTopLevelItem( index_to_clear );
    delete item_to_clear;
  }

  GuiFileTransferItem* new_item = new GuiFileTransferItem( this );
  new_item->setFirstColumnSpanned( false );
  new_item->init( peer_id, u, fi );
  return new_item;
}

void GuiFileTransfer::setProgress( VNumber peer_id, const User& u, const FileInfo& fi, FileSizeType bytes, int elapsed_time )
{
  GuiFileTransferItem* item = findItem( peer_id );
  if( !item && bytes > 0 )
  {
    item = createItem( peer_id, u, fi );
    item->setTransferState( FileTransferPeer::Transferring );
  }
  if( item )
    item->updateFileInfo( fi, bytes, elapsed_time );
}

void GuiFileTransfer::setMessage( VNumber peer_id, const User& u, const FileInfo& fi, const QString& msg, FileTransferPeer::TransferState ft_state )
{
#ifdef BEEBEEP_DEBUG
  qDebug() << "Showing file transfer message" << msg << "with state" << ft_state;
#endif
  GuiFileTransferItem* item = findItem( peer_id );
  if( !item )
    item = createItem( peer_id, u, fi );
  item->setMessage( msg, ft_state );
}

void GuiFileTransfer::checkItemClicked( QTreeWidgetItem* tw_item, int col )
{
  if( !tw_item )
  {
    qWarning() << "Invalid QTreeWidgetItem* found in GuiFileTransfer::checkItemClicked(...)";
    return;
  }

  if( col == GuiFileTransferItem::ColumnCancel )
  {
    GuiFileTransferItem* item = reinterpret_cast<GuiFileTransferItem*>( tw_item );
    if( item->transferState() == FileTransferPeer::Transferring )
    {
      QString q_txt = tr( "Do you want to cancel the transfer of %1?" ).arg( item->fileInfo().name() );
      QString b_remove_partial_download = item->fileInfo().isDownload() && Settings::instance().resumeFileTransfer() ? tr( "Yes and delete the partially downloaded file" ) : QString();
      int i_ret = QMessageBox::question( this, Settings::instance().programName(), q_txt, tr( "Yes" ), tr( "No" ), b_remove_partial_download, 1, 1 );
      if( i_ret == 1 )
        return;
      else if( i_ret == 0 && Settings::instance().resumeFileTransfer() )
        emit transferPaused( item->peerId() );
      else
        emit transferCanceled( item->peerId() );
    }
    else if( item->transferState() == FileTransferPeer::Paused )
    {
      QString q_txt = tr( "Do you want to try resuming the transfer of %1?" ).arg( item->fileInfo().name() );
      if( QMessageBox::question( this, Settings::instance().programName(), q_txt, tr( "Yes" ), tr( "No" ), QString(), 0, 1 ) == 0 )
        emit resumeTransfer( item->userId(), item->fileInfo() );
    }
    else
    {
      // do nothing for now
    }
  }
}

void GuiFileTransfer::checkItemDoubleClicked( QTreeWidgetItem* tw_item, int )
{
  if( !tw_item )
  {
    qWarning() << "Invalid QTreeWidgetItem* found in GuiFileTransfer::checkItemDoubleClicked(...)";
    return;
  }

  GuiFileTransferItem* item = reinterpret_cast<GuiFileTransferItem*>( tw_item );
  if( !item )
    return;

  if( item->fileInfo().isDownload() && item->transferState() != FileTransferPeer::Completed )
  {
    QMessageBox::information( this, Settings::instance().programName(), tr( "You cannot open this file if the transfer has not been completed." ), tr( "Ok" ) );
    return;
  }

  QUrl url = QUrl::fromLocalFile( item->fileInfo().path() );
  url.setScheme( FileInfo::urlSchemeShowFileInFolder() );
  emit openFileCompleted( url );
}

void GuiFileTransfer::openMenu( const QPoint& )
{
  if( topLevelItemCount() > 0 )
    mp_menuContext->exec( QCursor::pos() );
}

void GuiFileTransfer::removeAllCompleted()
{
  clear();
  if( header()->isVisible() )
    header()->hide();
}
