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
// Author: Marco Mastroddi (marco.mastroddi(AT)gmail.com)
//
// $Id$
//
//////////////////////////////////////////////////////////////////////

#ifndef BEEBEEP_GUISAVEDCHATITEM_H
#define BEEBEEP_GUISAVEDCHATITEM_H

#include "Config.h"


class GuiSavedChatItem : public QTreeWidgetItem
{
public:
  enum ChatDataType { ChatName = Qt::UserRole+2 };

  GuiSavedChatItem( QTreeWidget* );

  bool operator<( const QTreeWidgetItem& ) const;

  inline void setChatName( const QString& );
  inline QString chatName() const;

};


// Inline Functions
inline void GuiSavedChatItem::setChatName( const QString& chat_name ) { setData( 0, ChatName, chat_name ); }
inline QString GuiSavedChatItem::chatName() const { return data( 0, ChatName ).toString(); }

#endif // BEEBEEP_GUISAVEDCHATITEM_H
