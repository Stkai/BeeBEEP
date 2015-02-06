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

#include "GuiGroupList.h"


GuiGroupList::GuiGroupList( QWidget* parent )
  : QTreeWidget( parent )
{
  setObjectName( "GuiGroupList" );

  setColumnCount( 1 );
  header()->hide();
  setRootIsDecorated( false );
  setSortingEnabled( true );

  setContextMenuPolicy( Qt::CustomContextMenu );
  setMouseTracking( true );

  /*
  mp_menu = new QMenu( this );

  QAction* act = mp_menu->addAction( QIcon( ":/images/chat.png" ), tr( "Show" ), this, SLOT( openChatSelected() ) );
  mp_menu->setDefaultAction( act );
  mp_menu->addSeparator();
  mp_actClear = mp_menu->addAction( QIcon( ":/images/clear.png" ), tr( "Clear" ), this, SLOT( clearChatSelected() ) );
  mp_actClear->setToolTip( tr( "Clear all chat messages" ) );
  */

  connect( this, SIGNAL( itemDoubleClicked( QTreeWidgetItem*, int ) ), this, SLOT( checkItemDoubleClicked( QTreeWidgetItem*, int ) ) );
  connect( this, SIGNAL( customContextMenuRequested( const QPoint& ) ), this, SLOT( showGroupMenu( const QPoint& ) ) );
}

QSize GuiGroupList::sizeHint() const
{
  return QSize( 140, 300 );
}


GuiGroupItem* GuiGroupList::itemFromId( VNumber item_id )
{
  GuiGroupItem* item;
  QTreeWidgetItemIterator it( this );
  while( *it )
  {
    item = (GuiGroupItem*)(*it);
    if( item->itemId() == item_id )
      return item;
    ++it;
  }
  return 0;
}

void GuiGroupList::checkItemDoubleClicked( QTreeWidgetItem* item, int )
{
  if( !item )
    return;

  GuiGroupItem* group_item = (GuiGroupItem*)item;
  emit chatSelected( group_item->chatId() );
}

void GuiGroupList::showGroupMenu( const QPoint& p )
{
  QTreeWidgetItem* item = itemAt( p );
  if( !item )
    return;

  GuiGroupItem* chat_item = (GuiGroupItem*)item;


}




