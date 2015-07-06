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

#include "GuiUserItem.h"
#include "Settings.h"
#include "PluginManager.h"
#include "UserManager.h"
#include "Avatar.h"


GuiUserItem::GuiUserItem( QTreeWidget* parent )
  : QTreeWidgetItem( parent )
{
}

GuiUserItem::GuiUserItem( QTreeWidgetItem* parent )
  : QTreeWidgetItem( parent )
{
}

bool GuiUserItem::operator<( const QTreeWidgetItem& item ) const
{
  int user_item_priority = data( 0, GuiUserItem::Priority ).toInt();
  int other_priority = item.data( 0, GuiUserItem::Priority ).toInt();
  if( user_item_priority != other_priority )
    return user_item_priority < other_priority;

  QString user_item_name = data( 0, GuiUserItem::UserName ).toString().toLower();
  QString other_name = item.data( 0, GuiUserItem::UserName ).toString().toLower();

  return user_item_name < other_name;
}

QIcon GuiUserItem::selectUserIcon( int user_status, bool use_big_icon ) const
{
  return use_big_icon ? QIcon( Bee::menuUserStatusIconFileName( user_status ) ) : Bee::userStatusIcon( user_status );
}

bool GuiUserItem::updateUser()
{
  return updateUser( UserManager::instance().userList().find( userId() ) );
}

bool GuiUserItem::updateUser( const User& u )
{
  if( u.id() != userId() )
    return false;

  if( !u.isValid() )
  {
    setData( 0, GuiUserItem::Priority, 100000000 );
    return false;
  }

  setData( 0, UserName, u.name() );

  bool ok = false;
  int unread_messages = unreadMessages();
  if( !ok )
    unread_messages = 0;
  int user_status = u.status();
  if( !user_status )
    unread_messages = 0;

  QString s = u.isLocal() ? QObject::tr( "All Lan Users" ) : (user_status != User::Offline ? u.name() : u.path());

  int user_priority = 1;

  if( unread_messages > 0 )
    s.prepend( QString( "(%1) " ).arg( unread_messages ) );

  s += " ";
  setText( 0, s );

  if( !u.isLocal() )
  {
    if( Settings::instance().showUserPhoto() )
    {
      QPixmap user_avatar;
      QSize icon_size = Settings::instance().avatarIconSize();

      if( u.vCard().photo().isNull() )
      {
        Avatar av;
        av.setName( u.name() );
        if( u.isConnected() )
          av.setColor( u.color() );
        else
          av.setColor( QColor( Qt::gray ).name() );
        av.setSize( icon_size );
        if( av.create() )
          user_avatar = av.pixmap();
        else
          user_avatar = selectUserIcon( user_status, true ).pixmap( icon_size );
      }
      else
      {
        user_avatar = u.vCard().photo();
        if( !u.isConnected() )
          user_avatar = Bee::convertToGrayScale( user_avatar );
      }

      QPixmap pix( icon_size );
      pix.fill( Bee::userStatusColor( u.status() ) );
      QPainter p( &pix );
      p.drawPixmap( 1, 1, icon_size.width() - 2, icon_size.height() - 2, user_avatar );
      setIcon( 0, pix );
    }
    else
      setIcon( 0, selectUserIcon( user_status, false ) );

  }

  QString tool_tip;

  if( u.isLocal() )
  {
    tool_tip = QObject::tr( "Click to open chat with all local users" );
  }
  else
  {
    tool_tip = QObject::tr( "%1 is %2" ).arg( u.name(), Bee::userStatusToString( user_status ) );
    if( u.isConnected() )
    {
      if( u.statusDescription().isEmpty() )
        tool_tip += QString( ".\n" );
      else
        tool_tip += QString( ": %1\n" ).arg( u.statusDescription() );

      if( !u.vCard().info().isEmpty() )
      {
        tool_tip += QString( "~~~\n" );
        tool_tip += u.vCard().info();
        tool_tip += QString( "\n~~~\n" );
      }

      tool_tip += QString( "(%1)" ).arg( QObject::tr( "Click to send a private message" ) );
    }
    user_priority = 1000;
    user_priority += u.isConnected() ? (1000*user_status) : 1000000;
  }

  user_priority -= (unread_messages > 99 ? 99 : unread_messages);
  user_priority -= qMin( (int)messages(), 899 );
  user_priority = qMax( 1, user_priority );
  setData( 0, GuiUserItem::Priority, user_priority );
  setToolTip( 0, tool_tip );

  return true;
}

void GuiUserItem::setChatOpened( bool chat_is_opened )
{
  QFont f = font( 0 );
  if( chat_is_opened )
  {
    setBackground( 0, Bee::defaultHighlightBrush() );
    setTextColor( 0, Bee::defaultHighlightedText().color() );
    f.setBold( true );
    setFont( 0, f );
  }
  else
  {
    setBackground( 0, Bee::defaultBackgroundBrush() );
    setTextColor( 0, Bee::defaultTextBrush().color() );
    f.setBold( false );
    setFont( 0, f );
  }
}
