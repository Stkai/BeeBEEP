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

#include "GuiChatList.h"
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

  mp_menu = new QMenu( this );

  QAction* act = mp_menu->addAction( QIcon( ":/images/chat.png" ), tr( "Show" ), this, SLOT( openChatSelected() ) );
  mp_menu->setDefaultAction( act );
  mp_menu->addSeparator();
  mp_actClear = mp_menu->addAction( QIcon( ":/images/clear.png" ), tr( "Clear" ), this, SLOT( clearChatSelected() ) );
  mp_actClear->setToolTip( tr( "Clear all chat messages" ) );
  mp_menu->addSeparator();
  act = mp_menu->addAction( QIcon( ":/images/disconnect.png" ), tr( "Delete" ), this, SLOT( removeChatSelected() ) );

  connect( this, SIGNAL( itemDoubleClicked( QTreeWidgetItem*, int ) ), this, SLOT( chatDoubleClicked( QTreeWidgetItem*, int ) ) );
  connect( this, SIGNAL( customContextMenuRequested( const QPoint& ) ), this, SLOT( showChatMenu( const QPoint& ) ) );
}

QSize GuiChatList::sizeHint() const
{
  return QSize( 140, 300 );
}

void GuiChatList::reloadChatList()
{
  clearSelection();
  clear();
  foreach( Chat c, ChatManager::instance().constChatList() )
  {
    updateChat( c.id() );
  }
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
  Chat c = ChatManager::instance().chat( chat_id, false );
  if( !c.isValid() )
    return;
  if( c.isEmpty() && !c.isGroup() && !c.isDefault() )
    return;

  GuiChatItem* item = itemFromChatId( chat_id );
  if( !item )
  {
    item = new GuiChatItem( this );
    item->setChatId( chat_id );
  }

  item->updateItem( c );
}

void GuiChatList::chatDoubleClicked( QTreeWidgetItem* item, int )
{
  if( !item )
    return;

  GuiChatItem* user_item = (GuiChatItem*)item;
  emit chatSelected( user_item->chatId() );
  clearSelection();
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

  mp_actClear->setDisabled( c.isEmpty() );

  mp_menu->exec( QCursor::pos() );

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

