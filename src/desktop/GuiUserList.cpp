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

#include "ChatManager.h"
#include "GuiUserList.h"
#include "GuiConfig.h"
#include "IconManager.h"
#include "Protocol.h"
#include "Settings.h"
#include "UserManager.h"


GuiUserList::GuiUserList( QWidget* parent )
  : QWidget( parent )
{
  setObjectName( "GuiUserList" );
  setupUi( this );

  mp_menuSettings = Q_NULLPTR;
  mp_menuUsers = Q_NULLPTR;

  mp_twUsers->setContextMenuPolicy( Qt::CustomContextMenu );
  mp_twUsers->setRootIsDecorated( false );
  mp_twUsers->setSortingEnabled( true );
  mp_twUsers->setColumnCount( 1 );
  mp_twUsers->setObjectName( "GuiCustomList" );
  mp_twUsers->setMouseTracking( true );
  mp_twUsers->setHeaderHidden( true );

  m_filter = "";
  m_blockShowChatRequest = false;
#if QT_VERSION >= 0x040700
  mp_leFilter->setPlaceholderText( tr( "Search user" ) );
#endif
  resetList();

  mp_pbSettings->setIcon( IconManager::instance().icon( "settings.png" ) );
  mp_pbClearFilter->setIcon( IconManager::instance().icon( "clear.png" ) );

  connect( mp_twUsers, SIGNAL( customContextMenuRequested( const QPoint& ) ), this, SLOT( showUserMenu( const QPoint& ) ) );
  connect( mp_twUsers, SIGNAL( itemClicked( QTreeWidgetItem*, int ) ), this, SLOT( userItemClicked( QTreeWidgetItem*, int ) ), Qt::QueuedConnection );
  connect( mp_twUsers, SIGNAL( itemEntered( QTreeWidgetItem*, int ) ), this, SLOT( onItemEntered( QTreeWidgetItem*, int ) ) );
  connect( mp_leFilter, SIGNAL( textChanged( const QString& ) ), this, SLOT( filterText( const QString& ) ) );
  connect( mp_pbClearFilter, SIGNAL( clicked() ), this, SLOT( clearFilter() ) );
  connect( mp_pbSettings, SIGNAL( clicked() ), this, SLOT( showMenuSettings() ) );
}

void GuiUserList::clear()
{
  resetList();
}

void GuiUserList::sortUsers()
{
  mp_twUsers->sortItems( 0, Settings::instance().sortUsersAscending() ? Qt::AscendingOrder : Qt::DescendingOrder );
}

void GuiUserList::resetList()
{
  if( mp_twUsers->topLevelItemCount() > 0 )
    mp_twUsers->clear();
  mp_twUsers->setIconSize( Settings::instance().avatarIconSize() );
  if( Settings::instance().showUsersInWorkgroups() )
  {
    mp_twUsers->setRootIsDecorated( true );
    GuiUserItem* others_workgroup = new GuiUserItem( mp_twUsers );
    others_workgroup->setWorkgroup( Bee::capitalizeFirstLetter( GuiUserItem::othersWorkgroup(), false, false ) );
  }
  else
    mp_twUsers->setRootIsDecorated( false );
}

void GuiUserList::updateUsers()
{
  resetList();
  foreach( User u, UserManager::instance().userList().toList() )
  {
    if( m_filter.isEmpty() || u.vCard().nickName().contains( m_filter, Qt::CaseInsensitive ) ||
        u.vCard().fullName( Settings::instance().useUserFirstNameFirstInFullName() ).contains( m_filter, Qt::CaseInsensitive ) ||
        u.vCard().email().contains( m_filter, Qt::CaseInsensitive ) ||
        u.vCard().phoneNumber().contains( m_filter, Qt::CaseInsensitive ) )
      setUser( u, false );
  }
  sortUsers();
}

QList<GuiUserItem*> GuiUserList::itemsFromUserId( VNumber user_id )
{
  QList<GuiUserItem*> items;
  GuiUserItem* item;
  QTreeWidgetItemIterator it( mp_twUsers );
  while( *it )
  {
    item = dynamic_cast<GuiUserItem*>( *it );
    if( item && !item->isWorkgroup() && item->userId() == user_id )
      items.append( item );
    ++it;
  }
  return items;
}

QList<GuiUserItem*> GuiUserList::itemsFromChatId( VNumber chat_id )
{
  QList<GuiUserItem*> items;
  GuiUserItem* item;
  QTreeWidgetItemIterator it( mp_twUsers );
  while( *it )
  {
    item = dynamic_cast<GuiUserItem*>( *it );
    if( item && !item->isWorkgroup() && item->chatId() == chat_id )
      items.append( item );
    ++it;
  }
  return items;
}

GuiUserItem* GuiUserList::itemFromWorkgroup( const QString& workgroup_name )
{
  GuiUserItem* item;
  QTreeWidgetItemIterator it( mp_twUsers );
  while( *it )
  {
    item = dynamic_cast<GuiUserItem*>( *it );
    if( item && item->isWorkgroup() && item->workgroup().toLower() == workgroup_name.toLower() )
      return item;
    ++it;
  }
  return Q_NULLPTR;
}

GuiUserItem* GuiUserList::itemFromUserIdAndWorkgroup( VNumber user_id, const QString& workgroup_name )
{
  GuiUserItem* item;
  QTreeWidgetItemIterator it( mp_twUsers );
  while( *it )
  {
    item = dynamic_cast<GuiUserItem*>( *it );
    if( item && !item->isWorkgroup() && item->userId() == user_id && item->workgroup().toLower() == workgroup_name.toLower() )
      return item;
    ++it;
  }
  return Q_NULLPTR;
}

void GuiUserList::setUnreadMessages( VNumber private_chat_id, int n, bool update_user )
{
  QList<GuiUserItem*> items = itemsFromChatId( private_chat_id );
  if( items.isEmpty() )
    return;

  foreach( GuiUserItem* item, items )
  {
    item->setUnreadMessages( n );
    if( update_user )
      item->updateUser();
  }

  if( update_user )
    sortUsers();
}

void GuiUserList::setMessages( VNumber private_chat_id, int n, bool update_user )
{
  QList<GuiUserItem*> items = itemsFromChatId( private_chat_id );
  if( items.isEmpty() )
    return;

  foreach( GuiUserItem* item, items )
  {
    item->setMessages( n );
    if( update_user )
      item->updateUser();
  }

  if( update_user )
    sortUsers();
}

GuiUserItem* GuiUserList::createUserItemInWorkgroup( const User& u, const QString& user_workgroup )
{
  GuiUserItem* item = Q_NULLPTR;
  if( !user_workgroup.isEmpty() )
  {
    GuiUserItem* parent_workgroup = itemFromWorkgroup( user_workgroup );
    if( !parent_workgroup )
    {
      parent_workgroup = new GuiUserItem( mp_twUsers );
      parent_workgroup->setWorkgroup( Bee::capitalizeFirstLetter( user_workgroup, false, false ) );
    }
    item = new GuiUserItem( parent_workgroup );
    item->setUserId( u.id() );
    item->setParentWorkgroup( user_workgroup );
    if( u.isStatusConnected() )
      parent_workgroup->setExpanded( true );
  }
  else
  {
    item = new GuiUserItem( mp_twUsers );
    item->setUserId( u.id() );
  }
  return item;
}

void GuiUserList::setUser( const User& u, bool sort_users )
{
  if( u.isLocal() )
    return;

  QList<GuiUserItem*> items = itemsFromUserId( u.id() );
  if( !u.isStatusConnected() && Settings::instance().showOnlyOnlineUsers() )
  {
    if( !items.isEmpty() )
    {
      foreach( GuiUserItem* item, items )
        removeUserItem( item );
    }
    return;
  }

  if( !items.isEmpty() )
  {
    if( Settings::instance().showUsersInWorkgroups() )
    {
      bool reload_items = false;
      foreach( GuiUserItem* item, items )
      {
        if( u.workgroups().isEmpty() )
        {
          if( item->workgroup().toLower() != GuiUserItem::othersWorkgroup().toLower() )
          {
            removeUserItem( item );
            reload_items = true;
          }
        }
        else
        {
          if( !u.workgroups().contains( item->workgroup(), Qt::CaseInsensitive ) )
          {
            removeUserItem( item );
            reload_items = true;
          }
        }
      }
      if( reload_items )
        items = itemsFromUserId( u.id() );
    }
  }

  if( Settings::instance().showUsersInWorkgroups() )
  {
    if( !u.workgroups().isEmpty() )
    {
      foreach( QString workgroup_name, u.workgroups() )
      {
        if( !itemFromUserIdAndWorkgroup( u.id(), workgroup_name ) )
          items.append( createUserItemInWorkgroup( u, workgroup_name ) );
      }
    }
    else
    {
      if( !itemFromUserIdAndWorkgroup( u.id(), GuiUserItem::othersWorkgroup() ) )
        items.append( createUserItemInWorkgroup( u, GuiUserItem::othersWorkgroup() ) );
    }
  }
  else
  {
    if( items.isEmpty() )
      items.append( createUserItemInWorkgroup( u, QString::null ) );
  }

  foreach( GuiUserItem* item, items )
  {
    Chat c = ChatManager::instance().privateChatForUser( u.id() );
    if( c.isValid() )
    {
      item->setChatId( c.id() );
      item->setUnreadMessages( c.unreadMessages() );
    }
    item->updateUser( u );
  }

  if( sort_users )
    sortUsers();
}

void GuiUserList::removeUserItem( GuiUserItem* item )
{
  if( !item )
    return;
  QTreeWidgetItem* root_item = mp_twUsers->invisibleRootItem();
  if( root_item )
  {
    root_item->removeChild( dynamic_cast<QTreeWidgetItem*>( item ) );
    delete item;
  }
}

void GuiUserList::removeUser( const User& u )
{
  QList<GuiUserItem*> items = itemsFromUserId( u.id() );
  if( items.isEmpty() )
    return;
  foreach( GuiUserItem* item, items )
    removeUserItem( item );
}

void GuiUserList::showUserMenu( const QPoint& p )
{
  QTreeWidgetItem* item = mp_twUsers->itemAt( p );
  if( !item )
  {
    if( mp_menuUsers )
      mp_menuUsers->exec( QCursor::pos() );
    return;
  }

  GuiUserItem* user_item = dynamic_cast<GuiUserItem*>( item );
  if( user_item->isWorkgroup() )
    return;

  if( user_item->chatId() == ID_DEFAULT_CHAT )
  {
    emit chatSelected( ID_DEFAULT_CHAT );
  }
  else
  {
    if( !Settings::instance().showVCardOnRightClick() )
      return;

    m_blockShowChatRequest = true;
    emit showVCardRequest( user_item->userId() );
  }
  mp_twUsers->clearSelection();
}

void GuiUserList::userItemClicked( QTreeWidgetItem* item, int )
{
  if( !item )
    return;

  if( m_blockShowChatRequest )
  {
    m_blockShowChatRequest = false;
    return;
  }

  GuiUserItem* user_item = dynamic_cast<GuiUserItem*>( item );
  if( !user_item )
    return;
  if( user_item->isWorkgroup() )
    return;
  if( user_item->chatId() != ID_INVALID )
    emit chatSelected( user_item->chatId() );
  else
    emit userSelected( user_item->userId() );
  mp_twUsers->clearSelection();
}

void GuiUserList::filterText( const QString& txt )
{
  QString new_filter = txt.trimmed().toLower();
  if( m_filter == new_filter )
    return;

  m_filter = new_filter;
  updateUsers();
}

void GuiUserList::clearFilter()
{
  mp_leFilter->setText( "" );
  mp_leFilter->setFocus();
}

void GuiUserList::showMenuSettings()
{
  if( mp_menuSettings )
    mp_menuSettings->exec( QCursor::pos() );
}

void GuiUserList::onTickEvent( int ticks )
{
  GuiUserItem* item;
  QTreeWidgetItemIterator it( mp_twUsers );
  while( *it )
  {
    item = dynamic_cast<GuiUserItem*>( *it );
    if( item )
      item->onTickEvent( ticks );
    ++it;
  }
}

void GuiUserList::updateChat( const Chat& c )
{
  setUnreadMessages( c.id(), c.unreadMessages(), false );
  int chat_messages = c.chatMessages() + ChatManager::instance().savedChatSize( c.name() );
  setMessages( c.id(), chat_messages, true );
}

void GuiUserList::onItemEntered( QTreeWidgetItem* item, int )
{
  if( item )
  {
    if( !isActiveWindow() )
      QToolTip::showText( QCursor::pos(), item->toolTip( 0 ) );
  }
}

void GuiUserList::setContextMenuUsers( QMenu* new_value )
{
  mp_menuUsers = new_value;
  mp_menuUsers->addSeparator();
  mp_menuUsers->addAction( IconManager::instance().icon( "background-color.png" ), tr( "Change background color" ) + QString("..."), this, SLOT( selectBackgroundColor() ) );
}

void GuiUserList::setMenuSettings( QMenu* new_value )
{
  mp_menuSettings = new_value;
  mp_menuSettings->addSeparator();
  mp_menuSettings->addAction( IconManager::instance().icon( "background-color.png" ), tr( "Change background color" ) + QString("..."), this, SLOT( selectBackgroundColor() ) );
}

void GuiUserList::selectBackgroundColor()
{
  QColor c = Bee::selectColor( this, Settings::instance().userListBackgroundColor() );
  if( c.isValid() )
  {
    Settings::instance().setUserListBackgroundColor( c.name() );
    updateBackground();
  }
}

void GuiUserList::updateBackground()
{
  if( Settings::instance().useDarkStyle() && Settings::instance().userListBackgroundColor() == Settings::instance().defaultListBackgroundColor() )
  {
    if( !mp_twUsers->styleSheet().isEmpty() )
      mp_twUsers->setStyleSheet( QString() );
    return;
  }
  QString w_stylesheet = Settings::instance().guiCustomListStyleSheet( Settings::instance().userListBackgroundColor(),
                                                                  IconManager::instance().iconPath( "user-list.png" ) );
  mp_twUsers->setStyleSheet( w_stylesheet );
}

void GuiUserList::dragEnterEvent( QDragEnterEvent* event )
{
  if( event->mimeData()->hasUrls() )
  {
    QTreeWidgetItem* item = mp_twUsers->itemAt( event->pos() );
    if( item )
    {
      GuiUserItem* user_item = dynamic_cast<GuiUserItem*>( item );
      if( user_item && !user_item->isWorkgroup() )
      {
        item->setSelected( true );
        event->acceptProposedAction();
      }
    }
  }
}

void GuiUserList::dragMoveEvent( QDragMoveEvent* event )
{
  if( event->mimeData()->hasUrls() )
  {
    QTreeWidgetItem* item = mp_twUsers->itemAt( event->pos() );
    if( item  )
    {
      GuiUserItem* user_item = dynamic_cast<GuiUserItem*>( item );
      if( user_item && !user_item->isWorkgroup() )
      {
        int user_status = user_item->data( 0, GuiUserItem::Status ).toInt();
        if( user_status != User::Offline )
        {
          if( !user_item->isSelected() )
          {
            mp_twUsers->clearSelection();
            user_item->setSelected( true );
          }
        }
        else
          mp_twUsers->clearSelection();
      }
    }
  }
}

void GuiUserList::dropEvent( QDropEvent *event )
{
  if( event->mimeData()->hasUrls() )
  {
    QTreeWidgetItem* item = mp_twUsers->itemAt( event->pos() );
    GuiUserItem* user_item = dynamic_cast<GuiUserItem*>( item );
    if( user_item && !user_item->isWorkgroup() )
    {
      checkAndSendUrls( item, event->mimeData() );
      mp_twUsers->clearSelection();
    }
  }
}

void GuiUserList::checkAndSendUrls( QTreeWidgetItem* item, const QMimeData* source )
{
  if( !source->hasUrls() )
    return;

  if( !item )
    return;

  GuiUserItem* user_item = dynamic_cast<GuiUserItem*>(item);
  if( !user_item )
  {
    qWarning() << "Invalid item found in GuiUserList::checkAndSendUrls";
    return;
  }

  QString user_name = user_item->data( 0, GuiUserItem::UserName ).toString();

  if( user_item->isWorkgroup() )
  {
    QMessageBox::information( this, Settings::instance().programName(), tr( "You cannot send files to the workgroup '%1'." ).arg( user_name ) );
    return;
  }

  VNumber chat_id = user_item->chatId();
  int user_status = user_item->data( 0, GuiUserItem::Status ).toInt();
  if( user_status == User::Offline )
  {
    QMessageBox::information( this, Settings::instance().programName(), tr( "You cannot send files to %1 because the user is offline." ).arg( user_name ) );
    return;
  }

  QStringList file_path_list;
  QString file_path;
  int num_files = 0;

  foreach( QUrl url, source->urls() )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Checking pasted url:" << qPrintable( url.toString() );
#endif

#if QT_VERSION >= 0x040800
    if( url.isLocalFile() )
#else
    if( url.scheme() == QLatin1String( "file" ) )
#endif
    {
      file_path = url.toLocalFile();
      num_files += Protocol::instance().countFilesCanBeSharedInPath( file_path );
      if( num_files > Settings::instance().maxQueuedDownloads() )
        break;
      file_path_list.append( Bee::convertToNativeFolderSeparator( file_path ) );
    }
  }

  if( num_files <= 0 )
    return;

  if( file_path_list.isEmpty() )
  {
    if( num_files > 0 )
    {
      QMessageBox::warning( this, Settings::instance().programName(), tr( "You are trying to send %1 files simultaneously but the maximum allowed is %2." )
                                                                      .arg( num_files ).arg( Settings::instance().maxQueuedDownloads() ), tr( "Ok" ) );
    }
    return;
  }

  num_files = qMin( num_files, Settings::instance().maxQueuedDownloads() );

  if( QMessageBox::question( this, Settings::instance().programName(),
                             tr( "Do you want to send %1 %2 to %3?" ).arg( num_files )
                             .arg( num_files == 1 ? tr( "file" ) : tr( "files" ) ).arg( user_name ),
                             tr( "Yes" ), tr( "No" ), QString(), 0, 1 ) == 1 )
  {
    return;
  }

  emit sendFilesToChatRequest( chat_id, file_path_list );
}

