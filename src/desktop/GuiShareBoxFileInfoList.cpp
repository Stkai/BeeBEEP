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

#include "BeeUtils.h"
#include "GuiShareBoxFileInfoList.h"
#include "FileShare.h"
#include "IconManager.h"
#include "User.h"


GuiShareBoxFileInfoList::GuiShareBoxFileInfoList( QWidget* parent )
 : QTreeWidget( parent )
{
}

void GuiShareBoxFileInfoList::initTree()
{
  setColumnCount( 3 );

  QStringList labels;
  labels << tr( "Shared folders and files" ) << tr( "Size" ) << tr( "Last modified" );
  setHeaderLabels( labels );

  sortItems( GuiShareBoxFileInfoItem::ColumnFile, Qt::AscendingOrder );

  setAlternatingRowColors( true );
  setRootIsDecorated( true );
  setContextMenuPolicy( Qt::CustomContextMenu );
  setSelectionMode( QAbstractItemView::ExtendedSelection );
  setDragDropMode( QAbstractItemView::DragDrop );
  setDragEnabled( true );
  setAcceptDrops( true );

  QHeaderView* hv = header();
#if QT_VERSION >= 0x050000
  hv->setSectionResizeMode( GuiShareBoxFileInfoItem::ColumnFile, QHeaderView::Stretch );
  hv->setSectionResizeMode( GuiShareBoxFileInfoItem::ColumnSize, QHeaderView::ResizeToContents );
  hv->setSectionResizeMode( GuiShareBoxFileInfoItem::ColumnLastModified, QHeaderView::ResizeToContents );
#else
  hv->setResizeMode( GuiShareBoxFileInfoItem::ColumnFile, QHeaderView::Stretch );
  hv->setResizeMode( GuiShareBoxFileInfoItem::ColumnSize, QHeaderView::ResizeToContents );
  hv->setResizeMode( GuiShareBoxFileInfoItem::ColumnLastModified, QHeaderView::ResizeToContents );
#endif

  setColumnHidden( 2, true );
}

void GuiShareBoxFileInfoList::clearTree()
{
  clearSelection();
  if( !isEmpty() )
    clear();
}

void GuiShareBoxFileInfoList::setFileInfoList( const QList<FileInfo>& file_info_list )
{
  setUpdatesEnabled( false );
  clearTree();
  GuiShareBoxFileInfoItem* item;
  foreach( FileInfo fi, file_info_list )
  {
    item = new GuiShareBoxFileInfoItem( this );
    item->setFileInfo( fi );
  }
  setUpdatesEnabled( true );
}

void GuiShareBoxFileInfoList::addDotDotFolder()
{
  GuiShareBoxFileInfoItem* item = new GuiShareBoxFileInfoItem( this );
  FileInfo fi( ID_DOTDOT_FOLDER, FileInfo::Download );
  fi.setName( ".." );
  fi.setIsFolder( true );
  item->setFileInfo( fi );
}

QList<FileInfo> GuiShareBoxFileInfoList::selectedFileInfoList() const
{
  QList<FileInfo> file_info_selected_list;
  QList<QTreeWidgetItem*> selected_items = selectedItems();
  if( !selected_items.isEmpty() )
  {
    GuiShareBoxFileInfoItem* share_box_item;
    foreach( QTreeWidgetItem* item, selected_items )
    {
      share_box_item = (GuiShareBoxFileInfoItem*)item;
      if( share_box_item->isFile() )
        file_info_selected_list.append( share_box_item->fileInfo() );
    }
  }
  return file_info_selected_list;
}

int GuiShareBoxFileInfoList::countFileItems() const
{
  int count_file_items = 0;
  GuiShareBoxFileInfoItem* item;
  QTreeWidgetItemIterator it( (QTreeWidget*)this );
  while( *it )
  {
    item = (GuiShareBoxFileInfoItem*)(*it);
    if( item->isFile() )
      count_file_items++;
    ++it;
  }
  return count_file_items;
}

void GuiShareBoxFileInfoList::mousePressEvent( QMouseEvent* event )
{
  if( event->button() == Qt::LeftButton )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "BeeBOX mouse press event: store start point";
#endif
    m_dragStartPoint = event->pos();
  }
  QTreeWidget::mousePressEvent( event );
}

void GuiShareBoxFileInfoList::mouseMoveEvent( QMouseEvent* event )
{
  if( event->buttons() & Qt::LeftButton )
  {
    int distance = (event->pos() - m_dragStartPoint).manhattanLength();
    if( distance >= QApplication::startDragDistance() )
    {
      if( !performDrag() )
      {
        event->accept();
        return;
      }
    }
  }
  QTreeWidget::mouseMoveEvent( event );
}

bool GuiShareBoxFileInfoList::performDrag()
{
  QList<FileInfo> selected_list = selectedFileInfoList();
  if( selected_list.isEmpty() )
    return false;

  QStringList sl_path;
  foreach( FileInfo fi, selected_list )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "BeeBOX performs DRAG of the file:" << fi.name();
#endif
    sl_path << fi.name();
  }

  QDrag* drag = new QDrag( this );
  QMimeData* mime_data = new QMimeData;
  mime_data->setText( sl_path.join( "\n" ) );
  drag->setMimeData( mime_data );

  QPixmap pix = IconManager::instance().icon( "sharebox.png" ).pixmap( 24 );
  QPainter painter( &pix );
  painter.setPen( Qt::red );
  QFont f = QApplication::font();
  f.setBold( true );
  painter.setFont( f );
  painter.drawText( QRect( 4, 4, 18, 18 ), Qt::AlignCenter, QString::number( selected_list.size() ) );
  drag->setPixmap( pix );

  if( drag->exec( Qt::CopyAction ) == Qt::CopyAction )
    drag->deleteLater();

  return true;
}

void GuiShareBoxFileInfoList::dragEnterEvent( QDragEnterEvent* event )
{
  GuiShareBoxFileInfoList* source = qobject_cast<GuiShareBoxFileInfoList*>(event->source());
  if( source && source != this )
  {
    event->setDropAction( Qt::CopyAction );
    event->accept();
  }
}

void GuiShareBoxFileInfoList::dragMoveEvent( QDragMoveEvent* event )
{
  GuiShareBoxFileInfoList* source = qobject_cast<GuiShareBoxFileInfoList*>(event->source());
  if( source && source != this )
  {
    event->setDropAction( Qt::CopyAction );
    event->accept();
  }
}

void GuiShareBoxFileInfoList::dropEvent( QDropEvent* event )
{
  GuiShareBoxFileInfoList* source = qobject_cast<GuiShareBoxFileInfoList*>(event->source());
  if( source && source != this )
  {
    if( event->mimeData()->hasText() )
    {

#ifdef BEEBEEP_DEBUG
      QString s = event->mimeData()->text().simplified();
      qDebug() << "BeeBOX performs DROP of the path:" << s;
#endif
      emit dropEventRequest( event->mimeData()->text() );
    }
    event->setDropAction( Qt::CopyAction );
    event->accept();
  }
}
