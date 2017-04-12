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

#include "GuiChatList.h"
#include "GuiConfig.h"
#include "ChatManager.h"
#include "Settings.h"


GuiChatList::GuiChatList( QWidget* parent )
  : QTreeWidget( parent )
{
  setObjectName( "GuiChatList" );

  setColumnCount( 1 );
  header()->hide();
  setRootIsDecorated( false );
  setSortingEnabled( true );

  setContextMenuPolicy( Qt::CustomContextMenu );
  setMouseTracking( true );

  m_chatSelected = ID_INVALID;
  m_blockShowChatRequest = false;

  mp_menuContext = new QMenu( this );

  QAction* act = mp_menuContext->addAction( QIcon( ":/images/chat.png" ), tr( "Show" ), this, SLOT( openChatSelected() ) );
  mp_menuContext->setDefaultAction( act );
  mp_menuContext->addSeparator();
  mp_actClear = mp_menuContext->addAction( QIcon( ":/images/clear.png" ), tr( "Clear" ), this, SLOT( clearChatSelected() ) );
  mp_actClear->setToolTip( tr( "Clear all chat messages" ) );
  mp_menuContext->addSeparator();
  mp_actDelete = mp_menuContext->addAction( QIcon( ":/images/delete.png" ), tr( "Delete" ), this, SLOT( removeChatSelected() ) );

  connect( this, SIGNAL( customContextMenuRequested( const QPoint& ) ), this, SLOT( showChatMenu( const QPoint& ) ) );
  connect( this, SIGNAL( itemClicked( QTreeWidgetItem*, int ) ), this, SLOT( chatClicked( QTreeWidgetItem*, int ) ), Qt::QueuedConnection );
}

void GuiChatList::reloadChatList()
{
  clearSelection();
  clear();
  foreach( Chat c, ChatManager::instance().constChatList() )
    updateChat( c );
}

GuiChatItem* GuiChatList::itemFromChatId( VNumber chat_id )
{
  GuiChatItem* item;
  QTreeWidgetItemIterator it( this );
  while( *it )
  {
    item = (GuiChatItem*)(*it);
    if( item->chatId() == chat_id )
      return item;
    ++it;
  }
  return 0;
}

void GuiChatList::updateChat( const Chat& c )
{
  if( !c.isValid() )
    return;
  if( c.isEmpty() && !c.isGroup() && !c.isDefault() )
    return;

  GuiChatItem* item = itemFromChatId( c.id() );
  if( !item )
  {
    item = new GuiChatItem( this );
    item->setChatId( c.id() );
  }

  item->updateItem( c );
}

void GuiChatList::chatClicked( QTreeWidgetItem* item, int )
{
  if( !item )
    return;

  if( m_blockShowChatRequest )
  {
    m_blockShowChatRequest = false;
    return;
  }

  clearSelection();
  GuiChatItem* user_item = (GuiChatItem*)item;
  emit chatSelected( user_item->chatId() );
}

void GuiChatList::showChatMenu( const QPoint& p )
{
  QTreeWidgetItem* item = itemAt( p );
  if( !item )
  {
    QMenu menu_create_chat;
    menu_create_chat.addAction( QIcon( ":/images/chat-create.png" ), tr( "Create chat" ), this, SIGNAL( createNewChatRequest() ) );
    menu_create_chat.exec( QCursor::pos() );
    return;
  }

  GuiChatItem* chat_item = (GuiChatItem*)item;
  m_chatSelected = chat_item->chatId();

  Chat c = ChatManager::instance().chat( m_chatSelected );
  if( !c.isValid() )
    return;

  m_blockShowChatRequest = true;

  mp_actClear->setDisabled( c.isEmpty() );
  mp_actDelete->setDisabled( c.isDefault() );

  mp_menuContext->exec( QCursor::pos() );

  clearSelection();
}

void GuiChatList::openChatSelected()
{
  emit chatSelected( m_chatSelected );
}

void GuiChatList::clearChatSelected()
{
  emit chatToClear( m_chatSelected );
}

void GuiChatList::removeChatSelected()
{
  emit chatToRemove( m_chatSelected );
}

void GuiChatList::onTickEvent( int ticks )
{
  GuiChatItem* item;
  QTreeWidgetItemIterator it( this );
  while( *it )
  {
    item = (GuiChatItem*)(*it);
    item->onTickEvent( ticks );
    ++it;
  }
}
