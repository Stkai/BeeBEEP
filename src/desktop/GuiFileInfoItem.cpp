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

#include "GuiFileInfoItem.h"
#include "BeeUtils.h"
#include "FileInfo.h"
#include "GuiIconProvider.h"


GuiFileInfoItem::GuiFileInfoItem( QTreeWidget *parent )
 : QTreeWidgetItem( parent ), m_type( ObjectInvalid ), m_userId( ID_INVALID ),
   m_fileInfoId( ID_INVALID ), m_fileSize( 0 ), m_folder( "" ), m_filePath( "" )
{
}

GuiFileInfoItem::GuiFileInfoItem( QTreeWidgetItem *parent )
 : QTreeWidgetItem( parent ), m_type( ObjectInvalid ), m_userId( ID_INVALID ),
   m_fileInfoId( ID_INVALID ), m_fileSize( 0 ), m_folder( "" ), m_filePath( "" )
{
}

bool GuiFileInfoItem::operator<( const QTreeWidgetItem& item ) const
{
  if( treeWidget()->sortColumn() == (int)ColumnFile)
  {
    const GuiFileInfoItem& fi_item = (GuiFileInfoItem&)item;
    if( isObjectFile() && fi_item.isObjectFolder() )
      return false;
    else if( isObjectFolder() && fi_item.isObjectFile() )
      return true;
    else
      return QTreeWidgetItem::operator<( item );
  }
  else if( treeWidget()->sortColumn() == (int)ColumnSize )
  {
    const GuiFileInfoItem& fi_item = (GuiFileInfoItem&)item;
    return m_fileSize < fi_item.fileSize();
  }
  else
    return QTreeWidgetItem::operator<( item );
}

void GuiFileInfoItem::initUser( VNumber user_id, const QString& user_name )
{
  m_type = ObjectUser;
  m_userId = user_id;
  setIcon( ColumnFile, QIcon( ":/images/user.png" ) );
  setText( ColumnFile, user_name );
  setText( ColumnSize, "" );
  setText( ColumnStatus, "" );
}

void GuiFileInfoItem::initFolder( VNumber user_id, const QString& folder_name, const QString& folder_path )
{
  m_type = ObjectFolder;
  m_userId = user_id;
  m_folder = folder_path;
  setIcon( ColumnFile, QIcon( ":/images/folder.png" ) );
  setText( ColumnFile, folder_name );
  setText( ColumnSize, "" );
  setText( ColumnStatus, "" );
}

void GuiFileInfoItem::initFile( VNumber user_id, const FileInfo& file_info )
{
  m_type = ObjectFile;
  m_userId = user_id;
  m_fileInfoId = file_info.id();
  m_fileSize = file_info.size();
  m_folder = file_info.shareFolder();
  setIcon( ColumnFile, GuiIconProvider::instance().findIcon( file_info ) );
  setText( ColumnFile, file_info.name() );
  setText( ColumnSize, Bee::bytesToString( file_info.size() ) );
  setText( ColumnStatus, "" );
}

int GuiFileInfoItem::removeChildren()
{
  int children_removed = 0;

  QList<QTreeWidgetItem*> children_list = takeChildren();

  if( children_list.isEmpty() )
    return children_removed;
  else
    children_removed = children_list.size();

  qDeleteAll( children_list );

  return children_removed;
}
