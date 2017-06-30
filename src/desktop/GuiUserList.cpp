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

#include "ChatManager.h"
#include "GuiUserList.h"
#include "GuiConfig.h"
#include "IconManager.h"
#include "Settings.h"
#include "UserManager.h"


GuiUserList::GuiUserList( QWidget* parent )
  : QWidget( parent )
{
  setObjectName( "GuiUserList" );
  setupUi( this );

  mp_menuSettings = 0;
  mp_menuUsers = 0;

  mp_twUsers->setContextMenuPolicy( Qt::CustomContextMenu );
  mp_twUsers->setRootIsDecorated( false );
  mp_twUsers->setSortingEnabled( true );
  mp_twUsers->setColumnCount( 1 );
  mp_twUsers->setObjectName( "GuiCustomList" );
  QString w_stylesheet = QString( "#GuiCustomList { background: white url(%1);"
                        "background-repeat: no-repeat;"
                        "background-position: bottom center;"
                        "color: black; }" ).arg( IconManager::instance().iconPath( "user-list.png" ) );
  mp_twUsers->setStyleSheet( w_stylesheet );
  mp_twUsers->setMouseTracking( true );
  mp_twUsers->setHeaderHidden( true );

  m_filter = "";
  m_blockShowChatRequest = false;
#if QT_VERSION >= 0x040700
  mp_leFilter->setPlaceholderText( tr( "Search user" ) );
#endif
  resetList();

  mp_pbSettings->setIcon( IconManager::instance().icon( "settings.png" ) );
  mp_pbClearFilter->setIcon( IconManager::instance().icon( "clear.png" ) );

  connect( mp_twUsers, SIGNAL( customContextMenuRequested( const QPoint& ) ), this, SLOT( showUserMenu( const QPoint& ) ) );
  connect( mp_twUsers, SIGNAL( itemClicked( QTreeWidgetItem*, int ) ), this, SLOT( userItemClicked( QTreeWidgetItem*, int ) ), Qt::QueuedConnection );
  connect( mp_twUsers, SIGNAL( itemEntered( QTreeWidgetItem*, int ) ), this, SLOT( onItemEntered( QTreeWidgetItem*, int ) ) );
  connect( mp_leFilter, SIGNAL( textChanged( const QString& ) ), this, SLOT( filterText( const QString& ) ) );
  connect( mp_pbClearFilter, SIGNAL( clicked() ), this, SLOT( clearFilter() ) );
  connect( mp_pbSettings, SIGNAL( clicked() ), this, SLOT( showMenuSettings() ) );
}

void GuiUserList::clear()
{
  resetList();
}

void GuiUserList::sortUsers()
{
  mp_twUsers->sortItems( 0, Settings::instance().sortUsersAscending() ? Qt::AscendingOrder : Qt::DescendingOrder );
}

void GuiUserList::resetList()
{
  if( mp_twUsers->topLevelItemCount() > 0 )
    mp_twUsers->clear();
  mp_twUsers->setIconSize( Settings::instance().avatarIconSize() );
}

void GuiUserList::updateUsers()
{
  resetList();
  foreach( User u, UserManager::instance().userList().toList() )
  {
    if( m_filter.isEmpty() || u.vCard().nickName().contains( m_filter, Qt::CaseInsensitive ) ||
        u.vCard().fullName().contains( m_filter, Qt::CaseInsensitive ) ||
        u.vCard().email().contains( m_filter, Qt::CaseInsensitive ) ||
        u.vCard().phoneNumber().contains( m_filter, Qt::CaseInsensitive ) )
      setUser( u, false );
  }
  sortUsers();
}

GuiUserItem* GuiUserList::itemFromUserId( VNumber user_id )
{
  GuiUserItem* item;
  QTreeWidgetItemIterator it( mp_twUsers );
  while( *it )
  {
    item = (GuiUserItem*)(*it);
    if( item->userId() == user_id )
      return item;
    ++it;
  }
  return 0;
}

GuiUserItem* GuiUserList::itemFromChatId( VNumber chat_id )
{
  GuiUserItem* item;
  QTreeWidgetItemIterator it( mp_twUsers );
  while( *it )
  {
    item = (GuiUserItem*)(*it);
    if( item->chatId() == chat_id )
      return item;
    ++it;
  }
  return 0;
}

void GuiUserList::setUnreadMessages( VNumber private_chat_id, int n )
{
  GuiUserItem* item = itemFromChatId( private_chat_id );
  if( !item )
    return;

  item->setUnreadMessages( n );
  item->updateUser();
  sortUsers();
}

void GuiUserList::setMessages( VNumber private_chat_id, int n )
{
  GuiUserItem* item = itemFromChatId( private_chat_id );
  if( !item )
    return;

  item->setMessages( n );
  item->updateUser();
  sortUsers();
}

void GuiUserList::setUser( const User& u, bool sort_users )
{
  if( u.isLocal() )
    return;

  GuiUserItem* item = itemFromUserId( u.id() );
  if( !item )
  {
    if( !u.isStatusConnected() && Settings::instance().showOnlyOnlineUsers() )
      return;

    item = new GuiUserItem( mp_twUsers );
    item->setUserId( u.id() );
  }
  else
  {
    if( !u.isStatusConnected() && Settings::instance().showOnlyOnlineUsers() )
    {
      removeUser( u );
      return;
    }
  }

  Chat c = ChatManager::instance().privateChatForUser( u.id() );
  if( c.isValid() )
  {
    item->setChatId( c.id() );
    item->setUnreadMessages( c.unreadMessages() );
  }

  item->updateUser( u );

  if( sort_users )
    sortUsers();
}

void GuiUserList::removeUser( const User& u )
{
  GuiUserItem* item = itemFromUserId( u.id() );
  if( item )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Delete user item from GuiUserList";
#endif
    QTreeWidgetItem* root_item = mp_twUsers->invisibleRootItem();
    if( root_item )
    {
      root_item->removeChild( (QTreeWidgetItem*)item );
      delete item;
    }
  }
}

void GuiUserList::showUserMenu( const QPoint& p )
{
  QTreeWidgetItem* item = mp_twUsers->itemAt( p );
  if( !item )
  {
    if( mp_menuUsers )
      mp_menuUsers->exec( QCursor::pos() );
    return;
  }

  GuiUserItem* user_item = (GuiUserItem*)item;
  if( user_item->chatId() == ID_DEFAULT_CHAT )
  {
    emit chatSelected( ID_DEFAULT_CHAT );
  }
  else
  {
    if( !Settings::instance().showVCardOnRightClick() )
      return;

    m_blockShowChatRequest = true;
    emit showVCardRequest( user_item->userId() );
  }
}

void GuiUserList::userItemClicked( QTreeWidgetItem* item, int )
{
  if( !item )
    return;

  if( m_blockShowChatRequest )
  {
    m_blockShowChatRequest = false;
    return;
  }

  GuiUserItem* user_item = (GuiUserItem*)item;
  if( user_item->chatId() != ID_INVALID )
    emit chatSelected( user_item->chatId() );
  else
    emit userSelected( user_item->userId() );
  mp_twUsers->clearSelection();
}

void GuiUserList::filterText( const QString& txt )
{
  QString new_filter = txt.trimmed().toLower();
  if( m_filter == new_filter )
    return;

  m_filter = new_filter;
  updateUsers();
}

void GuiUserList::clearFilter()
{
  mp_leFilter->setText( "" );
  mp_leFilter->setFocus();
}

void GuiUserList::showMenuSettings()
{
  if( mp_menuSettings )
    mp_menuSettings->exec( QCursor::pos() );
}

void GuiUserList::onTickEvent( int ticks )
{
  GuiUserItem* item;
  QTreeWidgetItemIterator it( mp_twUsers );
  while( *it )
  {
    item = (GuiUserItem*)(*it);
    item->onTickEvent( ticks );
    ++it;
  }
}

void GuiUserList::updateChat( const Chat& c )
{
  setUnreadMessages( c.id(), c.unreadMessages() );
  int chat_messages = c.chatMessages() + ChatManager::instance().savedChatSize( c.name() );
  setMessages( c.id(), chat_messages );
}

void GuiUserList::onItemEntered( QTreeWidgetItem* item, int )
{
  if( item )
  {
    if( !isActiveWindow() )
      QToolTip::showText( QCursor::pos(), item->toolTip( 0 ) );
  }
}
