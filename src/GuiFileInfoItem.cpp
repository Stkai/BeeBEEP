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


GuiFileInfoItem::GuiFileInfoItem( QTreeWidget* parent, int size_in_column, int size_role )
 : QTreeWidgetItem( parent ), m_sizeInColumn( size_in_column ), m_sizeRole( size_role )
{
}

bool GuiFileInfoItem::operator<( const QTreeWidgetItem& item ) const
{
  if( m_sizeInColumn == treeWidget()->sortColumn() )
    return data( m_sizeInColumn, m_sizeRole ).toLongLong() > item.data( m_sizeInColumn, m_sizeRole ).toLongLong();

  return QTreeWidgetItem::operator<( item );
}

