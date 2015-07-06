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

#include "GuiSavedChatList.h"
#include "GuiConfig.h"
#include "ChatManager.h"
#include "Settings.h"


GuiSavedChatList::GuiSavedChatList( QWidget* parent )
  : QTreeWidget( parent ), m_savedChatSelected( "" )
{
  setObjectName( "GuiSavedChatList" );

  setColumnCount( 1 );
  header()->hide();
  setRootIsDecorated( false );
  setSortingEnabled( true );

  setContextMenuPolicy( Qt::CustomContextMenu );
  setMouseTracking( true );

  mp_menu = new QMenu( this );

  QAction* act = mp_menu->addAction( QIcon( ":/images/saved-chat.png" ), tr( "Show" ), this, SLOT( showSavedChatSelected() ) );
  mp_menu->setDefaultAction( act );
  mp_menu->addSeparator();
  mp_actLink = mp_menu->addAction( QIcon( ":/images/update.png" ), tr( "Link to chat" ), this, SLOT( linkSavedChatSelected() ) );
  mp_menu->addSeparator();
  mp_menu->addAction( QIcon( ":/images/remove-saved-chat.png" ), tr( "Delete" ), this, SLOT( removeSavedChatSelected() ) );

  connect( this, SIGNAL( itemClicked( QTreeWidgetItem*, int ) ), this, SLOT( savedChatClicked( QTreeWidgetItem*, int ) ) );
  connect( this, SIGNAL( customContextMenuRequested( const QPoint& ) ), this, SLOT( showSavedChatMenu( const QPoint& ) ) );
}

QSize GuiSavedChatList::sizeHint() const
{
  return QSize( BEE_DOCK_WIDGET_SIZE_HINT_WIDTH, BEE_DOCK_WIDGET_SIZE_HINT_HEIGHT );
}

void GuiSavedChatList::savedChatClicked( QTreeWidgetItem* item, int )
{
  if( !item )
    return;

  GuiSavedChatItem* saved_chat_item = (GuiSavedChatItem*)item;
  m_savedChatSelected = saved_chat_item->chatName();
  showSavedChatSelected();
  clearSelection();
}

void GuiSavedChatList::showSavedChatMenu( const QPoint& p )
{
  QTreeWidgetItem* item = itemAt( p );
  if( !item )
    return;

  GuiSavedChatItem* saved_chat_item = (GuiSavedChatItem*)item;
  m_savedChatSelected = saved_chat_item->chatName();

  mp_actLink->setEnabled( !ChatManager::instance().hasName( m_savedChatSelected ) );

  mp_menu->exec( QCursor::pos() );

  clearSelection();
}

void GuiSavedChatList::showSavedChatSelected()
{
  emit savedChatSelected( m_savedChatSelected );
}

void GuiSavedChatList::removeSavedChatSelected()
{
  if( QMessageBox::warning( this, Settings::instance().programName(), tr( "Do you really want to delete this saved chat?" ), tr( "Yes" ), tr( "No" ), QString(), 1, 1 ) == 1 )
    return;
  emit savedChatRemoved( m_savedChatSelected );
}

void GuiSavedChatList::linkSavedChatSelected()
{
  emit savedChatLinkRequest( m_savedChatSelected );
}

void GuiSavedChatList::updateSavedChats()
{
  if( topLevelItemCount() > 0 )
    clear();

  if( ChatManager::instance().constHistoryMap().isEmpty() )
    return;

  GuiSavedChatItem *item;
  QMap<QString, QString>::const_iterator it = ChatManager::instance().constHistoryMap().constBegin();
  while( it !=  ChatManager::instance().constHistoryMap().constEnd() )
  {
    item = new GuiSavedChatItem( this );
    item->setChatName( it.key() );
    item->setIcon( 0, QIcon( ":/images/saved-chat.png" ) );
    item->setText( 0, it.key() );
    item->setToolTip( 0, QObject::tr( "Click to view chat history: %1" ).arg( it.key() ) );
    ++it;
  }
}

void GuiSavedChatList::setSavedChatOpened( const QString& saved_chat_opened )
{
  GuiSavedChatItem* item;
  QTreeWidgetItemIterator it( this );
  while( *it )
  {
    item = (GuiSavedChatItem*)(*it);
    if( saved_chat_opened.isEmpty() )
      item->setSavedChatOpened( false );
    else if( item->chatName() == saved_chat_opened )
      item->setSavedChatOpened( true );
    else
      item->setSavedChatOpened( false );
    ++it;
  }
}
