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

#ifndef BEEBEEP_GUICHATITEM_H
#define BEEBEEP_GUICHATITEM_H

#include "Config.h"
#include "BeeUtils.h"
class Chat;


class GuiChatItem : public QTreeWidgetItem
{

public:
  enum ChatDataType { ChatId = Qt::UserRole+2, ChatName, ChatIsGroup };

  GuiChatItem( QTreeWidget* );

  bool operator<( const QTreeWidgetItem& ) const;

  inline void setChatId( VNumber );
  inline VNumber chatId() const;
  inline void setIsGroup( bool );
  inline bool isGroup() const;

  bool updateItem( const Chat& );

};


// Inline Functions
inline void GuiChatItem::setChatId( VNumber chat_id ) { setData( 0, ChatId, chat_id ); }
inline VNumber GuiChatItem::chatId() const { return Bee::qVariantToVNumber( data( 0, ChatId ) ); }
inline void GuiChatItem::setIsGroup( bool new_value ) { setData( 0, ChatIsGroup, new_value ); }
inline bool GuiChatItem::isGroup() const { return data( 0, ChatIsGroup ).toBool(); }

#endif // BEEBEEP_GUICHATITEM_H
