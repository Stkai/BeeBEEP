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

#include "GuiSavedChatItem.h"


GuiSavedChatItem::GuiSavedChatItem( QTreeWidget* parent )
 : QTreeWidgetItem( parent )
{
}

bool GuiSavedChatItem::operator<( const QTreeWidgetItem& item ) const
{
  QString user_item_name = data( 0, GuiSavedChatItem::ChatName ).toString().toLower();
  QString other_name = item.data( 0, GuiSavedChatItem::ChatName ).toString().toLower();

  return user_item_name > other_name; // correct order
}

