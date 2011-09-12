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

#include "GuiUserItem.h"
#include "Settings.h"


GuiUserItem::GuiUserItem( QTreeWidget* parent )
  : QTreeWidgetItem( parent )
{
}

void GuiUserItem::setUser( const User& u )
{
  setData( 0, UserId, u.id() );
  setData( 0, Username, u.name() );
  setData( 0, UserPath, u.path() );
  setData( 0, UserColor, u.color() );
  setData( 0, UserStatus, u.status() );
  setData( 0, UserStatusDescription, u.statusDescription() );
}

void GuiUserItem::setUserOffline()
{
  setData( 0, UserStatus, User::Offline );
  updateItem();
}

static QIcon GetUserIcon( int unread_messages, int user_status )
{
  return unread_messages > 0 ? QIcon( ":/images/chat.png" ) : Bee::userStatusIcon( user_status );
}

void GuiUserItem::updateItem()
{
  bool ok = false;
  int unread_messages = data( 0, UnreadMessages ).toInt( &ok );
  if( !ok )
    unread_messages = 0;
  int user_status = data( 0, UserStatus ).toInt( &ok );
  if( !user_status )
    unread_messages = 0;

  VNumber user_id = Bee::qVariantToVNumber( data( 0, UserId ) );
  bool is_local_user =  user_id == Settings::instance().localUser().id();

  QString s = is_local_user ? QObject::tr( "* All *" ) :
              ( Settings::instance().showOnlyUsername() && user_status != User::Offline ? data( 0, Username ).toString() : data( 0, UserPath ).toString() );

  if( unread_messages > 0 )
    s.prepend( QString( "(%1) " ).arg( unread_messages ) );

  s += " ";
  setText( 0, s );
  if( is_local_user )
    setIcon( 0, GetUserIcon( 1, user_status ) );
  else
    setIcon( 0, GetUserIcon( unread_messages, user_status ) );

  if( !m_defaultForegroundColor.isValid() )
    m_defaultForegroundColor = foreground( 0 ).color();

  if( user_status != User::Offline )
  {
    if( !is_local_user && Settings::instance().showUserColor() )
      setForeground( 0, QBrush( QColor( data( 0, UserColor ).toString() ) ) );
    else
      setForeground( 0, QBrush( m_defaultForegroundColor ) );
  }
  else
    setForeground( 0, QBrush( QColor( "#808080" ) ) );

  QString status_tip;
  QString tool_tip;
  if( is_local_user )
  {
    status_tip = QObject::tr( "Open chat with all users" );
    tool_tip = status_tip;
  }
  else
  {
    status_tip = QObject::tr( "Open chat with %1" ).arg( data( 0, UserPath ).toString() );
    tool_tip = QObject::tr( "%1 is %2" ).arg( data( 0, Username ).toString(), Bee::userStatusToString( user_status ) );
  }

  setToolTip( 0, tool_tip );
  setStatusTip( 0, status_tip );
}
