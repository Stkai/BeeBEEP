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
#include "GuiShareBoxFileInfoList.h"
#include "FileShare.h"
#include "User.h"


GuiShareBoxFileInfoList::GuiShareBoxFileInfoList()
 : QObject( 0 ), mp_tree( 0 )
{
}

void GuiShareBoxFileInfoList::initTree( QTreeWidget* tree_widget )
{
  mp_tree = tree_widget;
  mp_tree->setColumnCount( 3 );

  QStringList labels;
  labels << tr( "Shared folders and files" ) << tr( "Size" ) << tr( "Last modified" );
  mp_tree->setHeaderLabels( labels );

  mp_tree->sortItems( GuiShareBoxFileInfoItem::ColumnFile, Qt::AscendingOrder );

  mp_tree->setAlternatingRowColors( true );
  mp_tree->setRootIsDecorated( true );
  mp_tree->setContextMenuPolicy( Qt::CustomContextMenu );
  mp_tree->setSelectionMode( QAbstractItemView::ExtendedSelection );

  QHeaderView* hv = mp_tree->header();
#if QT_VERSION >= 0x050000
  hv->setSectionResizeMode( GuiShareBoxFileInfoItem::ColumnFile, QHeaderView::Stretch );
  hv->setSectionResizeMode( GuiShareBoxFileInfoItem::ColumnSize, QHeaderView::ResizeToContents );
  hv->setSectionResizeMode( GuiShareBoxFileInfoItem::ColumnLastModified, QHeaderView::ResizeToContents );
#else
  hv->setResizeMode( GuiShareBoxFileInfoItem::ColumnFile, QHeaderView::Stretch );
  hv->setResizeMode( GuiShareBoxFileInfoItem::ColumnSize, QHeaderView::ResizeToContents );
  hv->setResizeMode( GuiShareBoxFileInfoItem::ColumnLastModified, QHeaderView::ResizeToContents );
#endif

  mp_tree->setColumnHidden( 2, true );
}

void GuiShareBoxFileInfoList::clearTree()
{
  clearTreeSelection();
  if( mp_tree->topLevelItemCount() > 0 )
    mp_tree->clear();
}

void GuiShareBoxFileInfoList::clearTreeSelection()
{
  mp_tree->clearSelection();
}

void GuiShareBoxFileInfoList::setFileInfoList( const QList<FileInfo>& file_info_list )
{
  mp_tree->setUpdatesEnabled( false );
  clearTree();
  GuiShareBoxFileInfoItem* item;
  foreach( FileInfo fi, file_info_list )
  {
    item = new GuiShareBoxFileInfoItem( mp_tree );
    item->setFileInfo( fi );
  }
  mp_tree->setUpdatesEnabled( true );
}

void GuiShareBoxFileInfoList::addDotDotFolder()
{
  GuiShareBoxFileInfoItem* item = new GuiShareBoxFileInfoItem( mp_tree );
  FileInfo fi( ID_DOTDOT_FOLDER, FileInfo::Download );
  fi.setName( ".." );
  fi.setIsFolder( true );
  item->setFileInfo( fi );
}

QList<FileInfo> GuiShareBoxFileInfoList::selectedFileInfoList() const
{
  QList<FileInfo> file_info_selected_list;
  QList<QTreeWidgetItem*> selected_items = mp_tree->selectedItems();
  if( !selected_items.isEmpty() )
  {
    GuiShareBoxFileInfoItem* share_box_item;
    foreach( QTreeWidgetItem* item, selected_items )
    {
      share_box_item = (GuiShareBoxFileInfoItem*)item;
      file_info_selected_list.append( share_box_item->fileInfo() );
    }
  }
  return file_info_selected_list;
}

int GuiShareBoxFileInfoList::countFileItems() const
{
  int count_file_items = 0;
  GuiShareBoxFileInfoItem* item;
  QTreeWidgetItemIterator it( mp_tree );
  while( *it )
  {
    item = (GuiShareBoxFileInfoItem*)(*it);
    if( item->isFile() )
      count_file_items++;
    ++it;
  }
  return count_file_items;
}
