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

#include "GuiUserList.h"
#include "Settings.h"


GuiUserList::GuiUserList( QWidget* parent )
  : QListWidget( parent )
{
  setObjectName( "GuiUserList" );
  connect( this, SIGNAL( itemDoubleClicked( QListWidgetItem* ) ), this, SLOT( userDoubleClicked( QListWidgetItem* ) ) );
  connect( this, SIGNAL( itemClicked( QListWidgetItem* ) ), this, SLOT( showUserInfo( QListWidgetItem* ) ) );
}

QSize GuiUserList::sizeHint() const
{
  return QSize( 140, 300 );
}

void GuiUserList::updateItem( QListWidgetItem* item )
{
  bool ok = false;
  int unread_messages = item->data( UnreadMessages ).toInt( &ok );
  if( !ok )
    unread_messages = 0;
  int user_status = item->data( UserStatus ).toInt( &ok );
  if( !user_status )
    unread_messages = 0;

  VNumber user_id = Bee::qVariantToVNumber( item->data( UserId ) );
  bool is_local_user =  user_id == Settings::instance().localUser().id();

  QString s = is_local_user ? tr( "* All *" ) :
              ( Settings::instance().showOnlyUsername() && user_status != User::Offline ? item->data( Username ).toString() : item->data( UserPath ).toString() );

   if( unread_messages > 0 )
    s.prepend( QString( "(%1) " ).arg( unread_messages ) );
  if( !is_local_user && user_status > User::Online )
    s.append( QString( " [%1] " ).arg( Bee::userStatusToString( user_status ) ) );
  s += " ";
  item->setText( s );
  if( is_local_user )
    item->setIcon( userIcon( 1, user_status ) );
  else
    item->setIcon( userIcon( unread_messages, user_status ) );
}

void GuiUserList::updateUsers()
{
  QList<QListWidgetItem*> item_list = findItems( " ", Qt::MatchContains );
  foreach( QListWidgetItem* item, item_list )
    updateItem( item );
}

QListWidgetItem* GuiUserList::userItem( VNumber user_id )
{
  QList<QListWidgetItem*> item_list = findItems( " ", Qt::MatchContains );
  foreach( QListWidgetItem* item, item_list )
  {
    if( Bee::qVariantToVNumber( item->data( UserId ) ) == user_id )
      return item;
  }
  return 0;
}

QListWidgetItem* GuiUserList::chatItem( VNumber chat_id )
{
  QList<QListWidgetItem*> item_list = findItems( " ", Qt::MatchContains );
  foreach( QListWidgetItem* item, item_list )
  {
    if( Bee::qVariantToVNumber( item->data( PrivateChatId ) ) == chat_id )
      return item;
  }
  return 0;
}

void GuiUserList::setUnreadMessages( VNumber chat_id, int n )
{
  QListWidgetItem* item = chatItem( chat_id );
  if( item )
  {
    item->setData( UnreadMessages, n );
    updateItem( item );
  }
  else
    qWarning() << "Unable to set unread messages in chat" << chat_id;
}

void GuiUserList::setUser( const User& u, VNumber private_chat_id, int unread_messages )
{
  QListWidgetItem* item = userItem( u.id() );
  if( !item )
  {
    qDebug() << "Create new user item in GuiUserList";
    item = new QListWidgetItem( this );
    item->setData( UserId, u.id() );
    item->setData( PrivateChatId, private_chat_id );
  }
  item->setData( Username, u.name() );
  item->setData( UserPath, u.path() );
  item->setData( UserStatus, u.status() );
  item->setData( UserStatusDescription, u.statusDescription() );
  if( unread_messages >= 0 )
    item->setData( UnreadMessages, unread_messages );
  updateItem( item );
  sortItems();
}

void GuiUserList::removeUser( const User& u )
{
  QListWidgetItem* item = userItem( u.id() );
  if( item )
  {
    item->setData( UserStatus, User::Offline );
    updateItem( item );
  }
  else
    qWarning() << "Unable to set user" << u.id() << "offline in GuiUserList";
}

void GuiUserList::showUserInfo( QListWidgetItem* item )
{
  if( !item )
  {
    qWarning() << "Item for show user info not found in GuiUserList";
    return;
  }
  QString sInfo;
  if( Bee::qVariantToVNumber( item->data( UserId ) ) == Settings::instance().localUser().id() )
    sInfo = tr( "Chat with all connected users" );
  else
    sInfo = tr( "Chat with %1" ).arg( item->data( UserPath ).toString() );
  emit stringToShow( sInfo, 6000 );
}

void GuiUserList::userDoubleClicked( QListWidgetItem* item )
{
  if( !item )
  {
    qWarning() << "Item double clicked not found in GuiUserList";
    return;
  }
  item->setData( UnreadMessages, 0 ); // read all messages
  updateItem( item );
  VNumber chat_id = Bee::qVariantToVNumber( item->data( PrivateChatId ) );
  qDebug() << "Item double clicked: chat" << chat_id << "of the user" <<  Bee::qVariantToVNumber( item->data( UserId ) ) <<"selected";
  if( chat_id > 0 )
    emit chatSelected( chat_id );
  else
    qWarning() << "GuiUserList has invalid chat id stored in item";
}

bool GuiUserList::nextUserWithUnreadMessages()
{
  QList<QListWidgetItem*> item_list = findItems( " ", Qt::MatchContains );
  foreach( QListWidgetItem* item, item_list )
  {
    if( item->data( UnreadMessages ).toInt() > 0 )
    {
      qDebug() << "Chat" << Bee::qVariantToVNumber( item->data( PrivateChatId ) ) << "with unread messages found";
      setCurrentItem( item );
      userDoubleClicked( item );
      return true;
    }
  }
  return false;
}
