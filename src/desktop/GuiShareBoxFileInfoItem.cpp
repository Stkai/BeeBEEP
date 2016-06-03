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

#include "GuiShareBoxFileInfoItem.h"
#include "BeeUtils.h"
#include "GuiIconProvider.h"


GuiShareBoxFileInfoItem::GuiShareBoxFileInfoItem( QTreeWidget *parent )
 : QTreeWidgetItem( parent ), m_fileInfo()
{
}

bool GuiShareBoxFileInfoItem::operator<( const QTreeWidgetItem& item ) const
{
  const GuiShareBoxFileInfoItem& fi_item = (GuiShareBoxFileInfoItem&)item;
  if( isFile() && fi_item.isFolder() )
    return false;
  else if( isFolder() && fi_item.isFile() )
    return true;

  if( treeWidget()->sortColumn() == (int)ColumnFile)
  {
    return QTreeWidgetItem::operator<( item );
  }
  else if( treeWidget()->sortColumn() == (int)ColumnSize )
  {
    const GuiShareBoxFileInfoItem& fi_item = (GuiShareBoxFileInfoItem&)item;
    return m_fileInfo.size() < fi_item.fileInfo().size();
  }
  else
    return QTreeWidgetItem::operator<( item );
}

void GuiShareBoxFileInfoItem::setFileInfo( const FileInfo& fi )
{
  m_fileInfo = fi;
  if( fi.isFolder() )
  {
    setIcon( ColumnFile, QIcon( ":/images/folder.png" ) );
    setText( ColumnFile, m_fileInfo.name() );
    setText( ColumnSize, "" );
    setText( ColumnLastModified, "" );
  }
  else
  {
    setIcon( ColumnFile, GuiIconProvider::instance().findIcon( m_fileInfo ) );
    setText( ColumnFile, m_fileInfo.name() );
    setText( ColumnSize, Bee::bytesToString( m_fileInfo.size() ) );
    setText( ColumnLastModified, m_fileInfo.lastModified().toString( Qt::ISODate ) );
  }
}
