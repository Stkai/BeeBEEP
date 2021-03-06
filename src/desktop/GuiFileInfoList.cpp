//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2021 Marco Mastroddi
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
#include "GuiFileInfoList.h"
#include "FileShare.h"
#include "Settings.h"
#include "User.h"


GuiFileInfoList::GuiFileInfoList()
 : QObject( 0 ), mp_tree( 0 ), m_selectedFileInfoList(), m_isLocal( true )
{
}

void GuiFileInfoList::initTree( QTreeWidget* tree_widget, bool is_local )
{
  m_isLocal = is_local;
  mp_tree = tree_widget;
  mp_tree->setColumnCount( 3 );

  QStringList labels;
  labels << tr( "Shared folders and files" ) << tr( "Size" ) << tr( "Status" );
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

void GuiFileInfoList::clearTree()
{
  if( mp_tree->topLevelItemCount() > 0 )
    mp_tree->clear();
  m_selectedFileInfoList.clear();
}

void GuiFileInfoList::clearTreeSelection()
{
  mp_tree->clearSelection();
  m_selectedFileInfoList.clear();
}

GuiFileInfoItem* GuiFileInfoList::userItem( VNumber user_id )
{
  GuiFileInfoItem* item;
  QTreeWidgetItemIterator it( mp_tree );
  while( *it )
  {
    item = (GuiFileInfoItem*)(*it);
    if( item->isObjectUser() && item->userId() == user_id )
    {
      return item;
    }
    ++it;
  }
  return 0;
}

GuiFileInfoItem* GuiFileInfoList::createUserItem( const User& u )
{
  GuiFileInfoItem* item = new GuiFileInfoItem( mp_tree );
  item->initUser( u.id(), Bee::userNameToShow( u, false ) );
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
    {
      return item;
    }
    ++it;
  }
  return 0;
}

GuiFileInfoItem* GuiFileInfoList::createSubFolderItem( GuiFileInfoItem* parent_item, VNumber user_id,
                                                       const QString& subfolder_name, const QString& subfolder_path )
{
  GuiFileInfoItem* item;
  if( parent_item )
    item = new GuiFileInfoItem( parent_item );
  else
    item = new GuiFileInfoItem( mp_tree );

  item->initFolder( user_id, subfolder_name, subfolder_path );

  return item;
}

GuiFileInfoItem* GuiFileInfoList::createFolderItem( const User& u, const QString& folder_name )
{
  GuiFileInfoItem* parent_item = 0;
  GuiFileInfoItem* item = 0;
  if( !u.isLocal() )
  {
    parent_item = userItem( u.id() );
    if( !parent_item )
      parent_item = createUserItem( u );
  }

  QStringList folder_list_path = folder_name.split( Bee::nativeFolderSeparator(), QString::SkipEmptyParts );

  if( !folder_list_path.isEmpty() )
  {
    QString subfolder_path = "";
    foreach( QString fn, folder_list_path )
    {
      if( subfolder_path.isEmpty() )
        subfolder_path = fn;
      else
        subfolder_path = subfolder_path + Bee::nativeFolderSeparator() + fn;
      item = folderItem( u.id(), subfolder_path );
      if( !item )
        item = createSubFolderItem( parent_item, u.id(), fn, subfolder_path );
      parent_item = item;
    }
  }
  else
    item = parent_item;

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
  GuiFileInfoItem* parent_item = 0;
  if( file_info.shareFolder().isEmpty() )
  {
    if( !u.isLocal() )
    {
      parent_item = userItem( u.id() );
      if( !parent_item )
        parent_item = createUserItem( u );
    }
  }
  else
  {
    parent_item = folderItem( u.id(), file_info.shareFolder() );
    if( !parent_item )
      parent_item = createFolderItem( u, file_info.shareFolder() );
  }

  GuiFileInfoItem* item;
  if( parent_item )
    item = new GuiFileInfoItem( parent_item );
  else
    item = new GuiFileInfoItem( mp_tree );

  item->initFile( u.id(), file_info );
  return item;
}

void GuiFileInfoList::addFileInfoToList( VNumber user_id, const FileInfo& fi )
{
  SharedFileInfo sfi( user_id, fi );
  if( !m_selectedFileInfoList.contains( sfi ) )
    m_selectedFileInfoList.append( sfi );
}

void GuiFileInfoList::addItemToFileInfoList( GuiFileInfoItem* fi_item )
{
  if( !fi_item->isObjectFile() )
    return;

  FileInfo fi = m_isLocal ? FileShare::instance().localFileInfo( fi_item->fileInfoId() ) : FileShare::instance().networkFileInfo( fi_item->userId(), fi_item->fileInfoId() );
  if( !fi.isValid() )
  {
#ifdef BEEBEEP_DEBUG
    qWarning() << "Unable to add an invalid file info in list:" << fi_item->text( GuiFileInfoItem::ColumnFile );
#endif
    return;
  }

  addFileInfoToList( fi_item->userId(), fi );
}

void GuiFileInfoList::parseItem( QTreeWidgetItem* tw_item )
{
  if( m_selectedFileInfoList.size() > Settings::instance().maxQueuedDownloads() )
    return;

  GuiFileInfoItem* item = (GuiFileInfoItem*)(tw_item);

  if( item->isObjectFile() )
  {
    addItemToFileInfoList( item );
  }
  else if( item->isObjectFolder()  )
  {
    if( item->childCount() > 0 )
    {
      for( int i=0; i < item->childCount(); i++ )
      {
        parseItem( item->child( i ) );
        QApplication::processEvents(); // we don't want to block socket operations
        if( m_selectedFileInfoList.size() > Settings::instance().maxQueuedDownloads() )
          return;
      }
    }
  }
  else if( item->isObjectUser() )
  {
    QList<FileInfo> user_file_info_list = m_isLocal ? FileShare::instance().fileSharedFromLocalUser() : FileShare::instance().fileSharedFromUser( item->userId() );
    foreach( FileInfo fi, user_file_info_list )
    {
      addFileInfoToList( item->userId(), fi );
      if( selectedFileInfoList().size() > Settings::instance().maxQueuedDownloads() )
        return;
    }
  }
  else
    qWarning() << "Invalid GuiFileInfoItem object found";
}

int GuiFileInfoList::parseSelectedItems()
{
  m_selectedFileInfoList.clear();
  QList<QTreeWidgetItem*> selected_items = mp_tree->selectedItems();
  if( selected_items.isEmpty() )
    return 0;

  mp_tree->setUpdatesEnabled( false );
  QApplication::setOverrideCursor( Qt::WaitCursor );
  QApplication::processEvents();
  foreach( QTreeWidgetItem* item, selected_items )
  {
    parseItem( item );
    QApplication::processEvents(); // we don't want to block socket operations;
  }

  QApplication::restoreOverrideCursor();
  mp_tree->setUpdatesEnabled( true );

  return m_selectedFileInfoList.size();
}

int GuiFileInfoList::countFileItems() const
{
  int count_file_items = 0;
  GuiFileInfoItem* item;
  QTreeWidgetItemIterator it( mp_tree );
  while( *it )
  {
    item = (GuiFileInfoItem*)(*it);
    if( item->isObjectFile() )
      count_file_items++;
    ++it;
  }
  return count_file_items;
}
