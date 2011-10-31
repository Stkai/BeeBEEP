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
#include "PluginManager.h"
#include "UserManager.h"


GuiUserItem::GuiUserItem( QTreeWidget* parent )
  : QTreeWidgetItem( parent )
{
}

static QIcon GetUserIcon( int unread_messages, const QString& user_service, int user_status )
{
  return unread_messages > 0 ? QIcon( ":/images/chat.png" ) : Bee::userStatusIcon( user_service, user_status );
}

bool GuiUserItem::updateItem()
{
  User u = UserManager::instance().userList().find( userId() );
  if( !u.isValid() )
    return false;
  bool ok = false;
  int unread_messages = data( 0, UnreadMessages ).toInt( &ok );
  if( !ok )
    unread_messages = 0;
  int user_status = u.status();
  if( !user_status )
    unread_messages = 0;

  QString s = u.isOnLan() ? (Settings::instance().showOnlyUsername() && user_status != User::Offline ? u.name() : u.path()) : u.name();

  if( unread_messages > 0 )
    s.prepend( QString( "(%1) " ).arg( unread_messages ) );

  s += " ";
  setText( 0, s );

  setIcon( 0, GetUserIcon( 0, u.service(), user_status ) );

  if( !m_defaultForegroundColor.isValid() )
    m_defaultForegroundColor = foreground( 0 ).color();

  if( user_status == User::Offline )
    setForeground( 0, QBrush( QColor( "#808080" ) ) );
  else
    setForeground( 0, QBrush( m_defaultForegroundColor ) );

  QString status_tip;
  QString tool_tip;
  if( u.isLocal() )
  {
    status_tip = QObject::tr( "Open chat with all users" );
    tool_tip = status_tip;
  }
  else
  {
    status_tip = QObject::tr( "Open chat with %1" ).arg( u.name() );
    tool_tip = QObject::tr( "%1 is %2" ).arg( u.name(), Bee::userStatusToString( user_status ) );
  }

  setToolTip( 0, tool_tip );
  setStatusTip( 0, status_tip );
  return true;
}
