//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2021 Marco Mastroddi
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

#include "ChatManager.h"
#include "GuiChatItem.h"
#include "GuiConfig.h"
#include "GuiSavedChatList.h"
#include "IconManager.h"
#include "Settings.h"


GuiSavedChatList::GuiSavedChatList( QWidget* parent )
  : QWidget( parent )
{
  setObjectName( "GuiSavedChatList" );
  setupUi( this );
  mp_menuContext = new QMenu( parent );

  mp_twSavedChatList->setColumnCount( 1 );
  mp_twSavedChatList->setRootIsDecorated( false );
  mp_twSavedChatList->setSortingEnabled( true );
  mp_twSavedChatList->setObjectName( "GuiCustomList" );
  mp_twSavedChatList->setContextMenuPolicy( Qt::CustomContextMenu );
  mp_twSavedChatList->setMouseTracking( true );
  mp_twSavedChatList->setIconSize( Settings::instance().avatarIconSize() );
  mp_twSavedChatList->setHeaderHidden( true );

  m_savedChatSelected = "";
  m_blockShowChatRequest = false;
  m_filter = "";

#if QT_VERSION >= 0x040700
  mp_leFilter->setPlaceholderText( tr( "Search saved chat" ) );
#endif
  mp_pbClearFilter->setIcon( IconManager::instance().icon( "clear.png" ) );

  setContextMenuPolicy( Qt::CustomContextMenu );
  connect( this, SIGNAL( customContextMenuRequested( const QPoint& ) ), this, SLOT( showSavedChatMenu( const QPoint& ) ) );
  connect( mp_twSavedChatList, SIGNAL( customContextMenuRequested( const QPoint& ) ), this, SLOT( showSavedChatMenu( const QPoint& ) ) );
  connect( mp_twSavedChatList, SIGNAL( itemClicked( QTreeWidgetItem*, int ) ), this, SLOT( savedChatClicked( QTreeWidgetItem*, int ) ), Qt::QueuedConnection );
  connect( mp_leFilter, SIGNAL( textChanged( const QString& ) ), this, SLOT( filterText( const QString& ) ) );
  connect( mp_pbClearFilter, SIGNAL( clicked() ), this, SLOT( clearFilter() ) );
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

  GuiSavedChatItem* saved_chat_item = dynamic_cast<GuiSavedChatItem*>( item );
  m_savedChatSelected = saved_chat_item->chatName();
  showSavedChatSelected();
}

void GuiSavedChatList::showSavedChatMenu( const QPoint& p )
{
  QTreeWidgetItem* item = mp_twSavedChatList->itemAt( p );
  mp_menuContext->clear();

  if( !item )
  {
    mp_menuContext->addAction( IconManager::instance().icon( "info.png" ), tr( "Please select an item" ) );
  }
  else
  {
    GuiSavedChatItem* saved_chat_item = dynamic_cast<GuiSavedChatItem*>( item );
    m_savedChatSelected = saved_chat_item->chatName();
    QAction* act = mp_menuContext->addAction( IconManager::instance().icon( "saved-chat.png" ), tr( "Show" ), this, SLOT( showSavedChatSelected() ) );
    mp_menuContext->setDefaultAction( act );
    mp_menuContext->addSeparator();
    act = mp_menuContext->addAction( IconManager::instance().icon( "update.png" ), tr( "Link to chat" ), this, SLOT( linkSavedChatSelected() ) );
    act->setEnabled( ChatManager::instance().constChatList().size() > 1 );
    mp_menuContext->addSeparator();
    mp_menuContext->addAction( IconManager::instance().icon( "remove-saved-chat.png" ), tr( "Delete" ), this, SLOT( removeSavedChatSelected() ) );
    m_blockShowChatRequest = true;
    mp_twSavedChatList->clearSelection();
  }

  mp_menuContext->addSeparator();
  mp_menuContext->addAction( IconManager::instance().icon( "background-color.png" ), tr( "Change background color" ) + QString("..."), this, SLOT( selectBackgroundColor() ) );
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
  mp_twSavedChatList->setIconSize( Settings::instance().avatarIconSize() );

  if( mp_twSavedChatList->topLevelItemCount() > 0 )
    mp_twSavedChatList->clear();

  if( ChatManager::instance().constHistoryMap().isEmpty() )
    return;

  GuiSavedChatItem *item;
  QString saved_chat_name;
  bool saved_chat_is_default = false;
  QMap<QString, QString>::const_iterator it = ChatManager::instance().constHistoryMap().constBegin();
  while( it !=  ChatManager::instance().constHistoryMap().constEnd() )
  {
    saved_chat_is_default = it.key() == Settings::instance().defaultChatName();

    saved_chat_name = saved_chat_is_default ? QString( " %1" ).arg( GuiChatItem::defaultChatName() ) : it.key();

    if( !m_filter.isEmpty() )
    {
      if( !saved_chat_name.contains( m_filter, Qt::CaseInsensitive ) )
      {
        ++it;
        continue;
      }
    }

    item = new GuiSavedChatItem( mp_twSavedChatList );
    item->setChatName( it.key() );
    item->setIcon( 0, IconManager::instance().icon( "saved-chat.png" ) );
    QFont f = this->font();
    f.setItalic( true );
    f.setBold( saved_chat_is_default );
    item->setText( 0, saved_chat_is_default ? saved_chat_name.toUpper() : saved_chat_name );
    item->setFont( 0, f );
    item->setToolTip( 0, tr( "Click to view saved chat with %1" ).arg( saved_chat_name ) );
    ++it;
  }

  mp_twSavedChatList->sortItems( 0, Qt::AscendingOrder );
}

void GuiSavedChatList::filterText( const QString& txt )
{
  QString new_filter = txt.trimmed().toLower();
  if( m_filter == new_filter )
    return;

  m_filter = new_filter;
  updateSavedChats();
}

void GuiSavedChatList::clearFilter()
{
  mp_leFilter->setText( "" );
  mp_leFilter->setFocus();
}

void GuiSavedChatList::selectBackgroundColor()
{
  QColor c = Bee::selectColor( this, Settings::instance().savedChatListBackgroundColor() );
  if( c.isValid() )
  {
    Settings::instance().setSavedChatListBackgroundColor( c.name() );
    updateBackground();
  }
}

void GuiSavedChatList::updateBackground()
{
  if( Settings::instance().useDarkStyle() && Settings::instance().savedChatListBackgroundColor() == Settings::instance().defaultListBackgroundColor() )
  {
    if( !mp_twSavedChatList->styleSheet().isEmpty() )
      mp_twSavedChatList->setStyleSheet( QString() );
    return;
  }
  QString w_stylesheet = Settings::instance().guiCustomListStyleSheet( Settings::instance().savedChatListBackgroundColor(),
                                                                    IconManager::instance().iconPath( "saved-chat-list.png" ) );
  mp_twSavedChatList->setStyleSheet( w_stylesheet );
}
