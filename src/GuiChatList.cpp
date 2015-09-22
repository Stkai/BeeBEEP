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

  m_chatOpened = ID_INVALID;
  m_chatSelected = ID_INVALID;
  m_blockShowChatRequest = false;

  mp_menu = new QMenu( this );

  QAction* act = mp_menu->addAction( QIcon( ":/images/chat.png" ), tr( "Show" ), this, SLOT( openChatSelected() ) );
  mp_menu->setDefaultAction( act );
  mp_menu->addSeparator();
  mp_actClear = mp_menu->addAction( QIcon( ":/images/clear.png" ), tr( "Clear" ), this, SLOT( clearChatSelected() ) );
  mp_actClear->setToolTip( tr( "Clear all chat messages" ) );
  mp_menu->addSeparator();
  mp_actDelete = mp_menu->addAction( QIcon( ":/images/delete.png" ), tr( "Delete" ), this, SLOT( removeChatSelected() ) );

  connect( this, SIGNAL( customContextMenuRequested( const QPoint& ) ), this, SLOT( showChatMenu( const QPoint& ) ) );
  connect( this, SIGNAL( itemClicked( QTreeWidgetItem*, int ) ), this, SLOT( chatClicked( QTreeWidgetItem*, int ) ), Qt::QueuedConnection );
}

QSize GuiChatList::sizeHint() const
{
  return QSize( BEE_DOCK_WIDGET_SIZE_HINT_WIDTH, BEE_DOCK_WIDGET_SIZE_HINT_HEIGHT );
}

void GuiChatList::reloadChatList()
{
  clearSelection();
  clear();
  foreach( Chat c, ChatManager::instance().constChatList() )
    updateChat( c.id() );
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

void GuiChatList::updateChat( VNumber chat_id )
{
  Chat c = ChatManager::instance().chat( chat_id );
  if( !c.isValid() )
    return;
  if( c.isEmpty() && !c.isGroup() && !c.isDefault() )
    return;

  GuiChatItem* item = itemFromChatId( chat_id );
  if( !item )
  {
    item = new GuiChatItem( this );
    item->setChatId( chat_id );
    item->setChatOpened( chat_id == m_chatOpened );
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

  GuiChatItem* user_item = (GuiChatItem*)item;
  emit chatSelected( user_item->chatId() );
}

void GuiChatList::showChatMenu( const QPoint& p )
{
  QTreeWidgetItem* item = itemAt( p );
  if( !item )
    return;

  GuiChatItem* chat_item = (GuiChatItem*)item;
  m_chatSelected = chat_item->chatId();

  Chat c = ChatManager::instance().chat( m_chatSelected );
  if( !c.isValid() )
    return;

  m_blockShowChatRequest = true;

  mp_actClear->setDisabled( c.isEmpty() );
  mp_actDelete->setDisabled( c.isDefault() );

  mp_menu->exec( QCursor::pos() );

  clearSelection();
  setChatOpened( m_chatOpened );
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

void GuiChatList::setChatOpened( VNumber chat_id )
{
  m_chatOpened = chat_id;

  GuiChatItem* item;
  QTreeWidgetItemIterator it( this );
  while( *it )
  {
    item = (GuiChatItem*)(*it);
    item->setChatOpened( item->chatId() == chat_id );
    ++it;
  }
}
