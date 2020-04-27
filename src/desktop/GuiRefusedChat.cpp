//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2020 Marco Mastroddi
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
// along with BeeBEEP. If not, see <http://www.gnu.org/licenses/>.
//
// Author: Marco Mastroddi <marco.mastroddi(AT)gmail.com>
//
// $Id$
//
//////////////////////////////////////////////////////////////////////

#include "BeeUtils.h"
#include "ChatManager.h"
#include "GuiRefusedChat.h"
#include "IconManager.h"
#include "Settings.h"


GuiRefusedChat::GuiRefusedChat( QWidget *parent )
  : QDialog( parent ), m_refusedChats()
{
  setupUi( this );
  setWindowTitle( tr( "Blocked chats" ) + QString( " - %1" ).arg( Settings::instance().programName() ) );
  setWindowIcon( IconManager::instance().icon( "refused-chat.png" ) );
  Bee::removeContextHelpButton( this );

  mp_twRefusedChats->setColumnCount( 2 );
  QStringList labels;
  labels << tr( "Name" ) << tr( "Blocked chat ID" );
  mp_twRefusedChats->setHeaderLabels( labels );
  mp_twRefusedChats->setAlternatingRowColors( true );
  mp_twRefusedChats->setSortingEnabled( true );
  mp_twRefusedChats->setRootIsDecorated( false );
  mp_twRefusedChats->setContextMenuPolicy( Qt::CustomContextMenu );
  mp_twRefusedChats->setSelectionMode( QAbstractItemView::MultiSelection );

  QHeaderView* hv = mp_twRefusedChats->header();
#if QT_VERSION >= 0x050000
  hv->setSectionResizeMode( 0, QHeaderView::ResizeToContents );
  hv->setSectionResizeMode( 1, QHeaderView::Stretch );
#else
  hv->setResizeMode( 0, QHeaderView::ResizeToContents );
  hv->setResizeMode( 1, QHeaderView::Stretch );
#endif

  mp_menuContext = new QMenu( this );
  mp_menuContext->addAction( IconManager::instance().icon( "delete.png" ), tr( "Remove blocked chat" ), this, SLOT( removeRefusedChat() ) );
  mp_menuContext->addSeparator();
  mp_menuContext->addAction( IconManager::instance().icon( "clear.png" ), tr( "Clear all" ), this, SLOT( removeAllRefusedChats() ) );

  connect( mp_pbOk, SIGNAL( clicked() ), this, SLOT( saveAndClose() ) );
  connect( mp_pbCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
  connect( mp_twRefusedChats, SIGNAL( customContextMenuRequested( const QPoint& ) ), this, SLOT( openCustomMenu( const QPoint& ) ) );
}

void GuiRefusedChat::loadRefusedChatsInList()
{
  if( mp_twRefusedChats->topLevelItemCount() > 0 )
    mp_twRefusedChats->clear();
  foreach( ChatRecord cr, m_refusedChats )
  {
    QTreeWidgetItem* item = new QTreeWidgetItem( mp_twRefusedChats );
    item->setText( 0, cr.name() );
    item->setIcon( 0, IconManager::instance().icon( "refused-chat.png" ));
    item->setText( 1, cr.privateId() );
  }
}

int GuiRefusedChat::loadRefusedChats()
{
  if( !m_refusedChats.isEmpty() )
    m_refusedChats.clear();

  m_refusedChats = ChatManager::instance().refusedChats();
  loadRefusedChatsInList();
  return m_refusedChats.size();
}

void GuiRefusedChat::saveAndClose()
{
  ChatManager::instance().clearRefusedChats();
  foreach( ChatRecord cr, m_refusedChats )
    ChatManager::instance().addToRefusedChat( cr );
  accept();
}

void GuiRefusedChat::openCustomMenu( const QPoint& p )
{
  QTreeWidgetItem* item = mp_twRefusedChats->itemAt( p );
  if( !item )
    return;

  if( !item->isSelected() )
    item->setSelected( true );

  mp_menuContext->exec( QCursor::pos() );
}

void GuiRefusedChat::removeAllRefusedChats()
{
  m_refusedChats.clear();
  loadRefusedChatsInList();
}

void GuiRefusedChat::removeRefusedChat()
{
  QList<QTreeWidgetItem*> items = mp_twRefusedChats->selectedItems();
  if( items.isEmpty() )
  {
    QMessageBox::information( this, Settings::instance().programName(), tr( "Please select an item in the list." ) );
    return;
  }

  foreach( QTreeWidgetItem* item, items )
  {
    ChatRecord cr( item->text( 0 ), item->text( 1 ) );
    m_refusedChats.removeOne( cr );
  }

  loadRefusedChatsInList();
}
