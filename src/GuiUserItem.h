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

#ifndef BEEBEEP_GUIUSERITEM_H
#define BEEBEEP_GUIUSERITEM_H

#include "Config.h"
#include "BeeUtils.h"


class GuiUserItem : public QTreeWidgetItem
{

public:
  enum UserDataType { UserId = Qt::UserRole+2, ChatId, UnreadMessages };

  GuiUserItem( QTreeWidget* );

  inline void setUserId( VNumber );
  inline VNumber userId() const;
  inline void setChatId( VNumber );
  inline VNumber chatId() const;
  inline void setUnreadMessages( int );
  inline int unreadMessages() const;

  bool updateItem();

private:
  QColor m_defaultForegroundColor;

};


// Inline Functions
inline void GuiUserItem::setUserId( VNumber user_id ) { setData( 0, UserId, user_id ); }
inline VNumber GuiUserItem::userId() const { return Bee::qVariantToVNumber( data( 0, UserId ) ); }
inline void GuiUserItem::setChatId( VNumber chat_id ) { setData( 0, ChatId, chat_id ); }
inline VNumber GuiUserItem::chatId() const { return Bee::qVariantToVNumber( data( 0, ChatId ) ); }
inline void GuiUserItem::setUnreadMessages( int unread_messages ) { setData( 0, UnreadMessages, unread_messages ); }
inline int GuiUserItem::unreadMessages() const { return data( 0, UnreadMessages ).toInt(); }


#endif // BEEBEEP_GUIUSERITEM_H
