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
// $Id: GuiFileInfoItem.cpp 346 2015-04-05 16:12:37Z mastroddi $
//
//////////////////////////////////////////////////////////////////////

#include "GuiFileInfoList.h"
#include "FileInfo.h"
#include "User.h"


GuiFileInfoList::GuiFileInfoList()
{
}

void GuiFileInfoList::initTree( QTreeWidget* tree_widget )
{
  mp_tree = tree_widget;
  mp_tree->setColumnCount( 3 );

  QStringList labels;
  labels << QObject::tr( "File" ) << QObject::tr( "Size" ) << QObject::tr( "Status" );
  mp_tree->setHeaderLabels( labels );

  mp_tree->sortItems( GuiFileInfoItem::ColumnFile, Qt::AscendingOrder );

  mp_tree->setAlternatingRowColors( true );
  mp_tree->setRootIsDecorated( true );
  mp_tree->setContextMenuPolicy( Qt::CustomContextMenu );
  mp_tree->setSelectionMode( QAbstractItemView::ExtendedSelection );

  QHeaderView* hv = mp_tree->header();
#if QT_VERSION >= 0x050000
  hv->setSectionResizeMode( GuiFileInfoItem::ColumnFile, QHeaderView::Stretch );
  hv->setSectionResizeMode( GuiFileInfoItem::ColumnSize, QHeaderView::ResizeToContents );
  hv->setSectionResizeMode( GuiFileInfoItem::ColumnStatus, QHeaderView::ResizeToContents );
#else
  hv->setResizeMode( GuiFileInfoItem::ColumnFile, QHeaderView::Stretch );
  hv->setResizeMode( GuiFileInfoItem::ColumnSize, QHeaderView::ResizeToContents );
  hv->setResizeMode( GuiFileInfoItem::ColumnStatus, QHeaderView::ResizeToContents );
#endif

}

GuiFileInfoItem* GuiFileInfoList::userItem( VNumber user_id )
{
  GuiFileInfoItem* item;
  QTreeWidgetItemIterator it( mp_tree );
  while( *it )
  {
    item = (GuiFileInfoItem*)(*it);
    if( item->isObjectUser() && item->userId() == user_id )
      return item;
    ++it;
  }
  return 0;
}

GuiFileInfoItem* GuiFileInfoList::createUserItem( const User& u )
{
  GuiFileInfoItem* item = new GuiFileInfoItem( mp_tree );
  item->initUser( u.id(), u.name() );
  return item;
}

GuiFileInfoItem* GuiFileInfoList::folderItem( VNumber user_id, const QString& folder_name )
{
  GuiFileInfoItem* item;
  QTreeWidgetItemIterator it( mp_tree );
  while( *it )
  {
    item = (GuiFileInfoItem*)(*it);
    if( item->isObjectFolder() && item->userId() == user_id && item->folder() == folder_name )
      return item;
    ++it;
  }
  return 0;
}

GuiFileInfoItem* GuiFileInfoList::createFolderItem( const User& u, const QString& folder_name )
{
  GuiFileInfoItem* item;
  if( u.isLocal() )
  {
    item = new GuiFileInfoItem( mp_tree );
  }
  else
  {
    GuiFileInfoItem* parent_item = userItem( u.id() );
    if( !parent_item )
      parent_item = createUserItem( u );
    item = new GuiFileInfoItem( parent_item );
  }
  item->initFolder( u.id(), folder_name );
  return item;
}

GuiFileInfoItem* GuiFileInfoList::fileItem( VNumber user_id, VNumber file_info_id )
{
  GuiFileInfoItem* item;
  QTreeWidgetItemIterator it( mp_tree );
  while( *it )
  {
    item = (GuiFileInfoItem*)(*it);
    if( item->isObjectFile() && item->userId() == user_id && item->fileInfoId() == file_info_id )
      return item;
    ++it;
  }
  return 0;
}

GuiFileInfoItem* GuiFileInfoList::createFileItem( const User& u, const FileInfo& file_info )
{
  GuiFileInfoItem* parent_item;
  if( file_info.shareFolder().isEmpty() )
  {
    parent_item = userItem( u.id() );
    if( !parent_item )
      parent_item = createUserItem( u );
  }
  else
  {
    parent_item = folderItem( u.id(), file_info.shareFolder() );
    if( !parent_item )
      parent_item = createFolderItem( u, file_info.shareFolder() );
  }

  GuiFileInfoItem* item = new GuiFileInfoItem( parent_item );
  item->initFile( u.id(), file_info );
  return item;
}
