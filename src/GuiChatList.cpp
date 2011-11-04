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


GuiChatList::GuiChatList( QWidget* parent )
  : QTreeWidget( parent )
{
  setObjectName( "GuiChatList" );

  setColumnCount( 1 );
  header()->hide();
  setRootIsDecorated( false );
  setSortingEnabled( true );

  connect( this, SIGNAL( itemDoubleClicked( QTreeWidgetItem*, int ) ), this, SLOT( chatDoubleClicked( QTreeWidgetItem*, int ) ) );
}

QSize GuiChatList::sizeHint() const
{
  return QSize( 140, 300 );
}

void GuiChatList::updateChats()
{
  GuiChatItem* item;
  QTreeWidgetItemIterator it( this );
  while( *it )
  {
    item = (GuiChatItem*)(*it);
    item->updateItem();
    ++it;
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
  if( !c.isDefault() && c.isEmpty() )
    return;
  GuiChatItem* item = itemFromChatId( chat_id );
  if( !item )
  {
    qDebug() << "Create new chat item in GuiChatList";
    item = new GuiChatItem( this );
    item->setChatId( chat_id );
  }

  item->updateItem();
}

void GuiChatList::chatDoubleClicked( QTreeWidgetItem* item, int )
{
  if( !item )
    return;

  GuiChatItem* user_item = (GuiChatItem*)item;
  emit chatSelected( user_item->chatId() );
}

