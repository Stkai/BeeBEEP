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

#ifndef BEEBEEP_GUIFILEINFOITEM_H
#define BEEBEEP_GUIFILEINFOITEM_H

#include "Config.h"


class GuiFileInfoItem : public QTreeWidgetItem
{
public:
  GuiFileInfoItem( QTreeWidget*, int size_in_column, int size_role );

  bool operator<( const QTreeWidgetItem& ) const;

private:
  int m_sizeInColumn;
  int m_sizeRole;

};


#endif // BEEBEEP_GUIFILEINFOITEM_H
