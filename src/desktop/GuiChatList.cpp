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
#include "IconManager.h"
#include "Settings.h"
#include "UserManager.h"


GuiChatList::GuiChatList( QWidget* parent )
  : QWidget( parent )
{
  setObjectName( "GuiChatList" );
  setupUi( this );

  mp_twChatList->setColumnCount( 1 );
  mp_twChatList->setRootIsDecorated( false );
  mp_twChatList->setSortingEnabled( true );
  mp_twChatList->setIconSize( Settings::instance().avatarIconSize() );
  mp_twChatList->setContextMenuPolicy( Qt::CustomContextMenu );
  mp_twChatList->setMouseTracking( true );
  mp_twChatList->setHeaderHidden( true );
  mp_twChatList->setObjectName( "GuiCustomList" );

  m_chatSelected = ID_INVALID;
  m_blockShowChatRequest = false;
  m_filter = "";

#if QT_VERSION >= 0x040700
  mp_leFilter->setPlaceholderText( tr( "Search chat" ) );
#endif

  mp_menuContext = new QMenu( parent );
  mp_menuSettings = new QMenu( parent );
  mp_pbClearFilter->setIcon( IconManager::instance().icon( "clear.png" ) );

  connect( mp_twChatList, SIGNAL( customContextMenuRequested( const QPoint& ) ), this, SLOT( showChatMenu( const QPoint& ) ) );
  connect( mp_twChatList, SIGNAL( itemClicked( QTreeWidgetItem*, int ) ), this, SLOT( chatClicked( QTreeWidgetItem*, int ) ), Qt::QueuedConnection );
  connect( mp_leFilter, SIGNAL( textChanged( const QString& ) ), this, SLOT( filterText( const QString& ) ) );
  connect( mp_pbClearFilter, SIGNAL( clicked() ), this, SLOT( clearFilter() ) );
  connect( mp_pbSettings, SIGNAL( clicked() ), this, SLOT( showMenuSettings() ) );
}

void GuiChatList::updateChats()
{
  mp_twChatList->clearSelection();
  mp_twChatList->clear();
  mp_twChatList->setIconSize( Settings::instance().avatarIconSize() );
  foreach( Chat c, ChatManager::instance().constChatList() )
    updateChat( c );
}

GuiChatItem* GuiChatList::itemFromChatId( VNumber chat_id )
{
  GuiChatItem* item;
  QTreeWidgetItemIterator it( mp_twChatList );
  while( *it )
  {
    item = dynamic_cast<GuiChatItem*>( *it );
    if( item->chatId() == chat_id )
      return item;
    ++it;
  }
  return Q_NULLPTR;
}

void GuiChatList::updateChat( const Chat& c )
{
  if( !c.isValid() )
    return;

  GuiChatItem* item = itemFromChatId( c.id() );
  if( !item )
  {
    item = new GuiChatItem( mp_twChatList );
    item->setChatId( c.id() );
  }
  item->updateItem( c );

  bool hide_item = false;

  if( !c.isDefault() && Settings::instance().hideEmptyChatsInList() )
    hide_item = ChatManager::instance().isChatEmpty( c, true );

  if( !hide_item && !m_filter.isEmpty() )
  {
    if( c.isDefault() )
      hide_item = !GuiChatItem::defaultChatName().contains( m_filter, Qt::CaseInsensitive );
    else
      hide_item = !c.name().contains( m_filter, Qt::CaseInsensitive );
  }

  item->setHidden( hide_item );
}

void GuiChatList::updateUser( const User& u )
{
  QList<Chat> chat_list = ChatManager::instance().chatsWithUser( u.id() );
  foreach( Chat c, chat_list )
    updateChat( c );
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

  mp_twChatList->clearSelection();
  GuiChatItem* user_item = dynamic_cast<GuiChatItem*>( item );
  emit chatSelected( user_item->chatId() );
}

void GuiChatList::showChatMenu( const QPoint& p )
{
  mp_menuContext->clear();
  QAction* act;
  act = mp_menuContext->addAction( tr( "Hide empty chats" ), this, SIGNAL( hideEmptyChatsRequest() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().hideEmptyChatsInList() );
  mp_menuContext->addSeparator();

  QTreeWidgetItem* item = mp_twChatList->itemAt( p );
  if( !item )
  {
    bool create_chat_is_enabled = true;
    if( UserManager::instance().userList().toList().size() < 2 )
    {
      mp_menuContext->addAction( IconManager::instance().icon( "info.png" ), tr( "Please wait for two or more users" ) );
      create_chat_is_enabled = false;
      mp_menuContext->addSeparator();
    }

    act = mp_menuContext->addAction( IconManager::instance().icon( "message-create.png" ), tr( "Write a message" ), this, SIGNAL( createNewMessageRequest() ) );
    act = mp_menuContext->addAction( IconManager::instance().icon( "group-create.png" ), tr( "Create new group chat" ), this, SIGNAL( createNewChatRequest() ) );
    act->setEnabled( create_chat_is_enabled );
  }
  else
  {
    GuiChatItem* chat_item = dynamic_cast<GuiChatItem*>( item );
    m_chatSelected = chat_item->chatId();
    Chat c = ChatManager::instance().chat( m_chatSelected );
    if( !c.isValid() )
    {
      mp_twChatList->clearSelection();
      qWarning() << "ChatListView is unable to find chat id" << m_chatSelected << " and item is hidden";
      item->setHidden( true );
      return;
    }
    act = mp_menuContext->addAction( IconManager::instance().icon( "chat.png" ), tr( "Show" ), this, SLOT( openChatSelected() ) );
    mp_menuContext->setDefaultAction( act );
    mp_menuContext->addSeparator();
    act = mp_menuContext->addAction( IconManager::instance().icon( "clear.png" ), tr( "Clear" ), this, SLOT( clearChatSelected() ) );
    act->setToolTip( tr( "Clear all chat messages" ) );
    act->setDisabled( ChatManager::instance().isChatEmpty( c, true ) );
    if( c.isGroup() )
    {
      mp_menuContext->addSeparator();
      mp_menuContext->addAction( IconManager::instance().icon( "group-edit.png" ), tr( "Edit" ), this, SLOT( editChatSelected() ) );
    }
    m_blockShowChatRequest = true;
  }

  mp_menuContext->addSeparator();
  mp_menuContext->addAction( IconManager::instance().icon( "background-color.png" ), tr( "Change background color" ) + QString("..."), this, SLOT( selectBackgroundColor() ) );

  mp_menuContext->exec( QCursor::pos() );
  mp_twChatList->clearSelection();
}

void GuiChatList::openChatSelected()
{
  emit chatSelected( m_chatSelected );
}

void GuiChatList::clearChatSelected()
{
  emit chatToClear( m_chatSelected );
}

void GuiChatList::editChatSelected()
{
  emit chatToEdit( m_chatSelected );
}

void GuiChatList::onTickEvent( int ticks )
{
  GuiChatItem* item;
  QTreeWidgetItemIterator it( mp_twChatList );
  while( *it )
  {
    item = dynamic_cast<GuiChatItem*>( *it );
    item->onTickEvent( ticks );
    ++it;
  }
}

void GuiChatList::filterText( const QString& txt )
{
  QString new_filter = txt.trimmed().toLower();
  if( m_filter == new_filter )
    return;

  m_filter = new_filter;
  updateChats();
}

void GuiChatList::clearFilter()
{
  mp_leFilter->setText( "" );
  mp_leFilter->setFocus();
}

void GuiChatList::selectBackgroundColor()
{
  QColor c = Bee::selectColor( this, Settings::instance().chatListBackgroundColor() );
  if( c.isValid() )
  {
    Settings::instance().setChatListBackgroundColor( c.name() );
    updateBackground();
  }
}

void GuiChatList::updateBackground()
{
  if( Settings::instance().useDarkStyle() && Settings::instance().chatListBackgroundColor() == Settings::instance().defaultListBackgroundColor() )
  {
    if( !mp_twChatList->styleSheet().isEmpty() )
      mp_twChatList->setStyleSheet( QString() );
    return;
  }
  QString w_stylesheet = Settings::instance().guiCustomListStyleSheet( Settings::instance().chatListBackgroundColor(),
                                                                    IconManager::instance().iconPath( "chat-list.png" ) );
  mp_twChatList->setStyleSheet( w_stylesheet );
}

void GuiChatList::showMenuSettings()
{
  bool create_chat_is_enabled = UserManager::instance().userList().toList().size() >= 2;
  QAction* act;
  mp_menuSettings->clear();
  act = mp_menuSettings->addAction( tr( "Hide empty chats" ), this, SIGNAL( hideEmptyChatsRequest() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().hideEmptyChatsInList() );
  mp_menuSettings->addSeparator();
  act = mp_menuSettings->addAction( IconManager::instance().icon( "message-create.png" ), tr( "Write a message" ), this, SIGNAL( createNewMessageRequest() ) );
  act = mp_menuSettings->addAction( IconManager::instance().icon( "group-create.png" ), tr( "Create new group chat" ), this, SIGNAL( createNewChatRequest() ) );
  act->setEnabled( create_chat_is_enabled );
  mp_menuSettings->addSeparator();
  mp_menuSettings->addAction( IconManager::instance().icon( "background-color.png" ), tr( "Change background color" ) + QString("..."), this, SLOT( selectBackgroundColor() ) );
  mp_menuSettings->exec( QCursor::pos() );
}
