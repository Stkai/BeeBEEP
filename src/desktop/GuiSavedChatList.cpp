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
#include "GuiChatItem.h"
#include "GuiConfig.h"
#include "GuiSavedChatList.h"
#include "Settings.h"


GuiSavedChatList::GuiSavedChatList( QWidget* parent )
  : QTreeWidget( parent ), m_savedChatSelected( "" )
{
  setObjectName( "GuiSavedChatList" );

  setColumnCount( 1 );
  header()->hide();
  setRootIsDecorated( false );
  setSortingEnabled( true );
  QString w_stylesheet = "background: white url(:/images/saved-chat-list.png);"
                        "background-repeat: no-repeat;"
                        "background-position: bottom center;";
  setStyleSheet( w_stylesheet );

  setContextMenuPolicy( Qt::CustomContextMenu );
  setMouseTracking( true );

  m_blockShowChatRequest = false;

  mp_menuContext = new QMenu( parent );

  connect( this, SIGNAL( customContextMenuRequested( const QPoint& ) ), this, SLOT( showSavedChatMenu( const QPoint& ) ) );
  connect( this, SIGNAL( itemClicked( QTreeWidgetItem*, int ) ), this, SLOT( savedChatClicked( QTreeWidgetItem*, int ) ), Qt::QueuedConnection );
}

void GuiSavedChatList::savedChatClicked( QTreeWidgetItem* item, int )
{
  if( !item )
    return;

  if( m_blockShowChatRequest )
  {
    m_blockShowChatRequest = false;
    return;
  }

  GuiSavedChatItem* saved_chat_item = (GuiSavedChatItem*)item;
  m_savedChatSelected = saved_chat_item->chatName();
  showSavedChatSelected();
}

void GuiSavedChatList::showSavedChatMenu( const QPoint& p )
{
  QTreeWidgetItem* item = itemAt( p );
  mp_menuContext->clear();

  if( !item )
  {
    mp_menuContext->addAction( QIcon( ":/images/info.png" ), tr( "Please select an item" ) );
  }
  else
  {
    GuiSavedChatItem* saved_chat_item = (GuiSavedChatItem*)item;
    m_savedChatSelected = saved_chat_item->chatName();
    QAction* act = mp_menuContext->addAction( QIcon( ":/images/saved-chat.png" ), tr( "Show" ), this, SLOT( showSavedChatSelected() ) );
    mp_menuContext->setDefaultAction( act );
    mp_menuContext->addSeparator();
    act = mp_menuContext->addAction( QIcon( ":/images/update.png" ), tr( "Link to chat" ), this, SLOT( linkSavedChatSelected() ) );
    act->setEnabled( ChatManager::instance().constChatList().size() > 1 );
    mp_menuContext->addSeparator();
    mp_menuContext->addAction( QIcon( ":/images/remove-saved-chat.png" ), tr( "Delete" ), this, SLOT( removeSavedChatSelected() ) );
    m_blockShowChatRequest = true;
    clearSelection();
  }

  mp_menuContext->exec( QCursor::pos() );
}

void GuiSavedChatList::showSavedChatSelected()
{
  emit savedChatSelected( m_savedChatSelected );
}

void GuiSavedChatList::removeSavedChatSelected()
{
  emit savedChatRemoved( m_savedChatSelected );
}

void GuiSavedChatList::linkSavedChatSelected()
{
  emit savedChatLinkRequest( m_savedChatSelected );
}

void GuiSavedChatList::updateSavedChats()
{
  setIconSize( Settings::instance().avatarIconSize() );

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
    QFont f = this->font();
    f.setItalic( true );
    if( it.key() == Settings::instance().defaultChatName() )
    {
      item->setText( 0, GuiChatItem::defaultChatName() );
      f.setBold( true );
    }
    else
      item->setText( 0, it.key() );
    item->setFont( 0, f );
    item->setToolTip( 0, QObject::tr( "Click to view chat history: %1" ).arg( item->text( 0 ) ) );
    ++it;
  }
}
