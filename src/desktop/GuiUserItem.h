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

#ifndef BEEBEEP_GUIUSERITEM_H
#define BEEBEEP_GUIUSERITEM_H

#include "BeeUtils.h"
class User;


class GuiUserItem : public QTreeWidgetItem
{

public:
  enum UserDataType { UserId = Qt::UserRole+2, ChatId, Messages, UnreadMessages, UserName, Priority, Status, Workgroup };

  GuiUserItem( QTreeWidget* );
  GuiUserItem( QTreeWidgetItem* );

  bool operator<( const QTreeWidgetItem& ) const;

  inline void setUserId( VNumber );
  inline VNumber userId() const;
  inline void setChatId( VNumber );
  inline VNumber chatId() const;
  inline void setMessages( int );
  inline int messages() const;
  inline void setUnreadMessages( int );
  inline int unreadMessages() const;
  inline void setParentWorkgroup( const QString& );
  inline const QString& parentWorkgroup() const;
  static QString othersWorkgroup();
  inline QString workgroup() const;

  bool updateUser();
  bool updateUser( const User& );

  void setWorkgroup( const QString& );
  inline bool isWorkgroup() const;

  void onTickEvent( int );

  inline void setDefaultIcon( const QIcon& );

private:
  QIcon selectUserIcon( int, bool ) const;
  void showUserStatus();

  QIcon m_defaultIcon;
  QString m_parentWorkgroup;

};


// Inline Functions
inline void GuiUserItem::setUserId( VNumber user_id ) { setData( 0, UserId, user_id ); }
inline VNumber GuiUserItem::userId() const { return Bee::qVariantToVNumber( data( 0, UserId ) ); }
inline void GuiUserItem::setChatId( VNumber chat_id ) { setData( 0, ChatId, chat_id ); }
inline VNumber GuiUserItem::chatId() const { return Bee::qVariantToVNumber( data( 0, ChatId ) ); }
inline void GuiUserItem::setMessages( int num_messages ) { setData( 0, Messages, num_messages ); }
inline int GuiUserItem::messages() const { return data( 0, Messages ).toInt(); }
inline void GuiUserItem::setUnreadMessages( int unread_messages ) { setData( 0, UnreadMessages, unread_messages ); }
inline int GuiUserItem::unreadMessages() const { return data( 0, UnreadMessages ).toInt(); }
inline void GuiUserItem::setDefaultIcon( const QIcon& new_value ) { m_defaultIcon = new_value; }
inline bool GuiUserItem::isWorkgroup() const { return data( 0, Workgroup ).toBool(); }
inline void GuiUserItem::setParentWorkgroup( const QString& new_value ) { m_parentWorkgroup = new_value; }
inline const QString& GuiUserItem::parentWorkgroup() const { return m_parentWorkgroup; }
inline QString GuiUserItem::workgroup() const { return isWorkgroup() ? data( 0, UserName ).toString() : ""; }

#endif // BEEBEEP_GUIUSERITEM_H
