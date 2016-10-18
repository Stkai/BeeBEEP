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

static int UserStatusSortingOrder( int user_status )
{
  switch( user_status )
  {
  case User::Offline : return 4;
  case User::Busy    : return 3;
  case User::Away    : return 2;
  default:
    return 1;
  }
}

bool GuiUserItem::operator<( const QTreeWidgetItem& item ) const
{
  QString user_name = data( 0, GuiUserItem::UserName ).toString().toLower();
  QString other_name = item.data( 0, GuiUserItem::UserName ).toString().toLower();

  if( Settings::instance().userSortingMode() == 1 ) // by name
  {
    return user_name < other_name;
  }
  else if( Settings::instance().userSortingMode() == 2 ) // by status
  {
    int user_status = UserStatusSortingOrder( data( 0, GuiUserItem::Status ).toInt() );
    int other_status = UserStatusSortingOrder( item.data( 0, GuiUserItem::Status ).toInt() );

    if( user_status != other_status )
      return user_status < other_status;
    else
      return user_name < other_name;
  }
  else if( Settings::instance().userSortingMode() == 3 ) // by messages
  {
    int user_messages = unreadMessages();
    int other_messages = data( 0, UnreadMessages ).toInt();
    if( user_messages != other_messages )
      return user_messages < other_messages;
    else
      return user_name < other_name;
  }
  else
  {
    int user_item_priority = data( 0, GuiUserItem::Priority ).toInt();
    int other_priority = item.data( 0, GuiUserItem::Priority ).toInt();
    if( user_item_priority != other_priority )
      return user_item_priority < other_priority;
    else
      return user_name < other_name;
  }
}

QIcon GuiUserItem::selectUserIcon( int user_status, bool use_big_icon ) const
{
  return use_big_icon ? QIcon( Bee::menuUserStatusIconFileName( user_status ) ) : Bee::userStatusIcon( user_status );
}

bool GuiUserItem::updateUser()
{
  return updateUser( UserManager::instance().findUser( userId() ) );
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

  setData( 0, UserName, u.isLocal() ? QString( " all lan users " ) : u.name() );
  setData( 0, Status, u.status() );

  int unread_messages = unreadMessages();

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
      bool paint_status_box = false;
      bool default_avatar_used = false;

      if( u.vCard().photo().isNull() )
      {
        default_avatar_used = true;
        Avatar av;
        av.setName( u.name() );
        if( u.isStatusConnected() )
          av.setColor( u.color() );
        else
          av.setColor( QColor( Qt::gray ).name() );
        av.setSize( icon_size );
        if( av.create() )
        {
          user_avatar = av.pixmap();
          if( u.isStatusConnected() )
            paint_status_box = true;
        }
        else
          user_avatar = selectUserIcon( user_status, true ).pixmap( icon_size );
      }
      else
      {
        user_avatar = u.vCard().photo();
        if( !u.isStatusConnected() )
          user_avatar = Bee::convertToGrayScale( user_avatar );
        else
          paint_status_box = true;
      }

      if( paint_status_box && !Settings::instance().showUserStatusBackgroundColor() )
      {
        QPixmap pix = avatarWithStatusBox( user_avatar, u.status(), default_avatar_used );
        setIcon( 0, pix );
      }
      else
        setIcon( 0, user_avatar );

    }
    else
      setIcon( 0, selectUserIcon( user_status, false ) );

  }
  else
    setIcon( 0, m_defaultIcon );

  QString tool_tip;
  if( u.isLocal() )
  {
    tool_tip = QObject::tr( "Click to open chat with all local users" );
  }
  else
  {
    tool_tip = QObject::tr( "%1 is %2" ).arg( u.name(), Bee::userStatusToString( user_status ) );
    if( u.isStatusConnected() )
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
    user_priority = u.isFavorite() ? 100 : 10000;
    user_priority += u.isStatusConnected() ? (1000*user_status) : 10000000;
  }

  user_priority -= (unread_messages > 99 ? 99 : unread_messages);
  user_priority -= qMin( (int)messages(), 899 );
  user_priority = u.isLocal() ? 1 : qMax( 2, user_priority );
  setData( 0, GuiUserItem::Priority, user_priority );
  setToolTip( 0, tool_tip );

  QFont f = font( 0 );
  f.setBold( unread_messages > 0 );
  setFont( 0, f );

  showUserStatus();

  if( !u.isLocal() )
    m_defaultIcon = icon( 0 );
  onTickEvent( 2 );
  return true;
}

void GuiUserItem::setChatOpened( bool chat_is_opened )
{
  if( chat_is_opened )
  {
    setBackground( 0, Bee::defaultHighlightBrush() );
    setTextColor( 0, Bee::defaultHighlightedTextBrush().color() );
  }
  else
    showUserStatus();
}

void GuiUserItem::showUserStatus()
{
  int user_status = data( 0, Status ).toInt();

  if( userId() == ID_LOCAL_USER )
    setBackground( 0, Bee::defaultBackgroundBrush() );
  else if( !Settings::instance().showUserPhoto() )
    setBackground( 0, Bee::defaultBackgroundBrush() );
  else if( Settings::instance().showUserStatusBackgroundColor() )
    setBackground( 0, Bee::userStatusBackgroundBrush( user_status ) );
  else
    setBackground( 0, Bee::defaultBackgroundBrush() );

  if( user_status == User::Offline )
    setTextColor( 0, Bee::userStatusColor( user_status ) );
  else
    setTextColor( 0, Bee::defaultTextBrush().color() );
}

static int GetBoxSize( int pix_size )
{
  int box_size = pix_size > 10 ? pix_size / 10 : 1;
  if( box_size % 2 > 7 )
    box_size++;
  box_size = qMax( 1, box_size );
  return box_size;
}

QPixmap GuiUserItem::avatarWithStatusBox( const QPixmap& user_avatar, int user_status, bool default_avatar_used ) const
{
  int pix_height = user_avatar.height();
  int pix_width = user_avatar.width();
  int box_height = GetBoxSize( pix_height );
  int box_width = GetBoxSize( pix_width );
  int box_start_height = qMax( 1, box_height / 2 );
  int box_start_width = qMax( 1, box_width / 2 );

//#ifdef BEEBEEP_DEBUG
//  qDebug() << "Avatar size:" << pix_width << "x" << pix_height << "-> Box size:" << box_width << "x" << box_height << ":" << box_start_width << box_start_height;
//#endif

  QPixmap pix( pix_width, pix_height );
  QPainter p( &pix );
  if( !default_avatar_used )
  {
    pix.fill( Bee::userStatusColor( user_status ) );
    p.drawPixmap( box_start_width, box_start_height, pix_width - box_width, pix_height - box_height, user_avatar.scaled( pix_width - box_width, pix_height - box_height ) );
  }
  else
  {
    p.drawPixmap( 0, 0, pix_width, pix_height, user_avatar );
    p.setPen( Bee::userStatusColor( user_status ) );
    for( int i = 0; i < box_height; i++ )
    {
      p.drawLine( 0, i, pix_width, i );
      p.drawLine( 0, pix_height-box_height+i, pix_width, pix_height-box_height+i );
    }

    for( int i = 0; i < box_width; i++ )
    {
      p.drawLine( i, 0, i, pix_height );
      p.drawLine( pix_width-box_width+i, 0, pix_width-box_width+i, pix_height );
    }
  }
  return pix;
}

void GuiUserItem::onTickEvent( int ticks )
{
  if( unreadMessages() > 0 )
  {
    if( ticks % 2 == 0 )
      setIcon( 0, QIcon( ":/images/beebeep-message.png" ) );
    else
      setIcon( 0, m_defaultIcon );
  }
}
