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

#ifndef BEEBEEP_GUIGROUPITEM_H
#define BEEBEEP_GUIGROUPITEM_H

#include "BeeUtils.h"
class Group;
class User;


class GuiGroupItem : public QTreeWidgetItem
{

public:
  enum ItemDataType { ItemId = Qt::UserRole+1, ChatId, ObjectId };
  enum ObjectTypeId { ObjectInvalid = 0, ObjectUser, ObjectGroup };

  GuiGroupItem( QTreeWidget* );
  GuiGroupItem( QTreeWidgetItem* );

  bool operator<( const GuiGroupItem& ) const;

  void init( VNumber item_id, VNumber chat_id, bool is_group );

  inline void setItemId( VNumber );
  inline VNumber itemId() const;
  inline void setChatId( VNumber );
  inline VNumber chatId() const;
  inline bool isGroup() const;

  bool updateGroup( const Group& );
  bool updateUser( const User& );

private:
  inline void setObjectType( ObjectTypeId );
  inline int objectType() const;

};


// Inline Functions
inline void GuiGroupItem::setItemId( VNumber new_value  ) { setData( 0, ItemId, new_value ); }
inline VNumber GuiGroupItem::itemId() const { return Bee::qVariantToVNumber( data( 0, ItemId ) ); }
inline void GuiGroupItem::setChatId( VNumber new_value  ) { setData( 0, ChatId, new_value ); }
inline VNumber GuiGroupItem::chatId() const { return Bee::qVariantToVNumber( data( 0, ChatId ) ); }
inline void GuiGroupItem::setObjectType( ObjectTypeId new_value  ) { setData( 0, ObjectId, (int)new_value ); }
inline int GuiGroupItem::objectType() const { return data( 0, ObjectId ).toInt(); }
inline bool GuiGroupItem::isGroup() const { return objectType() == ObjectGroup; }


#endif // BEEBEEP_GUIGROUPITEM_H
