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
  connect( this, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(userDoubleClicked(QListWidgetItem*)));
  connect( this, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(showUserInfo(QListWidgetItem*)));
}

QSize GuiUserList::sizeHint() const
{
  return QSize( Settings::instance().userListWidth(), 300 );
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

  bool is_local_user = item->data( UserId ).toInt() == Settings::instance().localUser().id();

  QString s = is_local_user ? item->data( UserChatName ).toString() :
              ( Settings::instance().showUserNickname() ?  item->data( UserNickname ).toString() : item->data( Username ).toString() );

  if( !is_local_user && ( user_status == 0 || Settings::instance().showUserIp() ) )
    s += QString( "<%1@%2>" ).arg( item->data( Username ).toString() ).arg( item->data( UserHostAddress ).toString() );

  if( unread_messages > 0 )
    s.prepend( QString( "(%1) " ).arg( unread_messages ) );
  if( !is_local_user && user_status >= 2 )
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

QListWidgetItem* GuiUserList::widgetItem( UserDataType udt, const QString& text_to_match )
{
  QList<QListWidgetItem*> item_list = findItems( " ", Qt::MatchContains );
  foreach( QListWidgetItem* item, item_list )
  {
    if( item->data( udt ).toString() == text_to_match )
      return item;
  }
  return NULL;
}

void GuiUserList::setUnreadMessages( const QString& chat_name, int n )
{
  QListWidgetItem* item = widgetItem( UserChatName, chat_name );
  if( item )
  {
    item->setData( UnreadMessages, n );
    updateItem( item );
  }
}

void GuiUserList::setUser( const User& u, int unread_messages )
{
  QListWidgetItem* item = widgetItem( UserChatName, Settings::instance().chatName( u ) );
  if( !item )
  {
    item = new QListWidgetItem( this );
    item->setData( Username, u.name() );
    item->setData( UserNickname, u.nickname() );
    item->setData( UserHostAddress, u.hostAddress().toString() );
    item->setData( UserChatName, Settings::instance().chatName( u ) );
  }
  item->setData( UserId, u.id() );
  item->setData( UserStatus, u.status() );
  item->setData( UserStatusDescription, u.statusDescription() );
  if( unread_messages >= 0 )
    item->setData( UnreadMessages, unread_messages );
  updateItem( item );
  sortItems();
}

void GuiUserList::removeUser( const User& u )
{
  QListWidgetItem* item = widgetItem( UserChatName, Settings::instance().chatName( u ) );
  if( item )
  {
    item->setData( UserStatus, User::Offline );
    updateItem( item );
  }
}

void GuiUserList::showUserInfo( QListWidgetItem* item )
{
  if( !item )
    return;
  QString sInfo;
  if( item->data( UserId ).toInt() == Settings::instance().localUser().id() )
    sInfo = tr( "Chat with all users connected" );
  else
    sInfo = tr( "Chat with %1@%2" )
              .arg( item->data( UserNickname ).toString() )
              .arg( item->data( UserHostAddress ).toString() );
  emit stringToShow( sInfo, 6000 );
}

void GuiUserList::userDoubleClicked( QListWidgetItem* item )
{
  if( !item )
    return;
  item->setData( UnreadMessages, 0 ); // read all messages
  updateItem( item );
  emit chatSelected( item->data( UserId ).toInt(), item->data( UserChatName ).toString() );
}

bool GuiUserList::nextUserWithUnreadMessages()
{
  QList<QListWidgetItem*> item_list = findItems( " ", Qt::MatchContains );
  foreach( QListWidgetItem* item, item_list )
  {
    if( item->data( UnreadMessages ).toInt() > 0 )
    {
      setCurrentItem( item );
      userDoubleClicked( item );
      return true;
    }
  }
  return false;
}
