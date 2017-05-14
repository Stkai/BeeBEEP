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

#include "BeeUtils.h"
#include "ChatManager.h"
#include "Core.h"
#include "GuiFloatingChat.h"
#include "GuiEmoticons.h"
#include "GuiPresetMessageList.h"
#include "GuiUserList.h"
#include "Settings.h"
#include "ShortcutManager.h"
#include "UserManager.h"
#ifdef Q_OS_WIN
  #include <windows.h>
#endif


GuiFloatingChat::GuiFloatingChat( Core* p_core, QWidget *parent )
 : QMainWindow( parent ), mp_core( p_core )
{
  setObjectName( "GuiFloatingChat" );
  m_mainWindowIcon = QIcon( ":/images/chat.png" );
  setMainIcon( false );
  mp_chat = new GuiChat( this );

  mp_actGroupMenu = new QAction( QIcon( ":/images/group-edit.png" ), tr( "Show group menu" ), this );
  connect( mp_actGroupMenu, SIGNAL( triggered() ), this, SLOT( showGroupMenu() ) );

  mp_barMembers = new QToolBar( tr( "Show the bar of members" ), this );
  addToolBar( Qt::RightToolBarArea, mp_barMembers );
  mp_barMembers->setObjectName( "GuiFloatingChatMemberToolBar" );
  mp_barMembers->setIconSize( Settings::instance().mainBarIconSize() );
  mp_barMembers->setAllowedAreas( Qt::AllToolBarAreas );
  mp_barMembers->setFloatable( false );
  mp_barMembers->toggleViewAction()->setVisible( false );

  mp_barChat = new QToolBar( tr( "Show the bar of chat" ), this );
  addToolBar( Qt::BottomToolBarArea, mp_barChat );
  mp_barChat->setObjectName( "GuiFloatingChatToolBar" );
  mp_barChat->setIconSize( Settings::instance().mainBarIconSize() );
  mp_barChat->setAllowedAreas( Qt::AllToolBarAreas );
  mp_chat->setupToolBar( mp_barChat );
  mp_barChat->setVisible( Settings::instance().showChatToolbar() );
  mp_barChat->insertSeparator( mp_barChat->actions().first() );
  mp_barChat->setFloatable( false );

  mp_dockPresetMessageList = new QDockWidget( tr( "Preset messages" ), this );
  mp_dockPresetMessageList->setObjectName( "GuiDockPresetMessageList" );
  mp_presetMessageListWidget = new GuiPresetMessageList( this );
  mp_presetMessageListWidget->loadFromSettings();
  connect( mp_presetMessageListWidget, SIGNAL( presetMessageSelected( const QString& ) ), mp_chat, SLOT( addText( const QString& ) ) );
  mp_dockPresetMessageList->setWidget( mp_presetMessageListWidget );
  mp_dockPresetMessageList->setAllowedAreas( Qt::AllDockWidgetAreas );
  addDockWidget( Qt::LeftDockWidgetArea, mp_dockPresetMessageList );
  QAction* actViewPresetMessageList = mp_dockPresetMessageList->toggleViewAction();
  actViewPresetMessageList->setIcon( QIcon( ":/images/preset-message.png" ) );
  actViewPresetMessageList->setToolTip( tr( "Show the preset messages panel" ) );
  mp_barChat->insertAction( mp_barChat->actions().first(), actViewPresetMessageList );
  mp_barChat->addSeparator();
  mp_actSaveWindowGeometry = mp_barChat->addAction( QIcon( ":/images/save-window.png" ), tr( "Save window's geometry" ), this, SLOT( saveGeometryAndState() ) );
  mp_dockPresetMessageList->hide();

  mp_dockEmoticons = new QDockWidget( tr( "Emoticons" ), this );
  mp_dockEmoticons->setObjectName( "GuiDockEmoticons" );
  mp_emoticonsWidget = new GuiEmoticons( this );
  updateEmoticon();
  connect( mp_emoticonsWidget, SIGNAL( emoticonSelected( const Emoticon& ) ), mp_chat, SLOT( addEmoticon( const Emoticon& ) ) );
  mp_dockEmoticons->setWidget( mp_emoticonsWidget );
  mp_dockEmoticons->setAllowedAreas( Qt::AllDockWidgetAreas );
  addDockWidget( Qt::LeftDockWidgetArea, mp_dockEmoticons );
  QAction* mp_actViewEmoticons = mp_dockEmoticons->toggleViewAction();
  mp_actViewEmoticons->setIcon( QIcon( ":/images/emoticon.png" ) );
  mp_actViewEmoticons->setText( tr( "Show the emoticon panel" ) );
  mp_actViewEmoticons->setVisible( !Settings::instance().useOnlyTextEmoticons() );
  mp_barChat->insertAction( mp_barChat->actions().first(), mp_actViewEmoticons );
  mp_dockEmoticons->hide();

  setCentralWidget( mp_chat );
  statusBar();
  m_chatIsVisible = true;
  m_prevActivatedState = false;

  connect( mp_chat, SIGNAL( toggleVisibilityEmoticonsPanelRequest() ), this, SLOT( toggleVisibilityEmoticonPanel() ) );
  connect( mp_chat, SIGNAL( toggleVisibilityPresetMessagesPanelRequest() ), this, SLOT( toggleVisibilityPresetMessagesPanel() ) );
  connect( qApp, SIGNAL( focusChanged( QWidget*, QWidget* ) ), this, SLOT( onApplicationFocusChanged( QWidget*, QWidget* ) ) );
}

void GuiFloatingChat::updateChatTitle( const Chat& c )
{
  QString window_title = "";

  if( c.isPrivate() )
  {
    VNumber user_id = c.privateUserId();
    User u = UserManager::instance().findUser( user_id );
    if( u.isValid() )
    {
      QString user_status = u.status() != User::Online ? Bee::userStatusToString( u.status() ) : "";
      QString user_status_description = u.status() != User::Offline ? u.statusDescription() : "";

      if( !user_status.isEmpty() && !user_status_description.isEmpty() )
        window_title = QString( "%1 (%2 - %3)" ).arg( u.name(), Bee::userStatusToString( u.status() ), user_status_description );
      else if( !user_status.isEmpty() )
        window_title = QString( "%1 (%2)" ).arg( u.name(), user_status );
      else if( !user_status_description.isEmpty() )
        window_title = QString( "%1 (%2)" ).arg( u.name(), user_status_description );
      else
        window_title = u.name();

      m_mainWindowIcon = Bee::avatarForUser( u, QSize( 256, 256 ), true );
      setWindowTitle( window_title );
    }
    else
    {
      qWarning() << "Invalid user" << user_id << "found for private chat" << c.name();
      m_mainWindowIcon = QIcon( ":/images/chat.png" );
      window_title = c.name();
    }
  }
  else if( c.isDefault() )
  {
    window_title = tr( "Chat with all connected users" );
    m_mainWindowIcon = QIcon( ":/images/default-chat-online.png" );
  }
  else if( c.isGroup() )
  {
    window_title = c.name();
    m_mainWindowIcon = QIcon( ":/images/group.png" );
  }
  else
  {
    window_title = c.name();
    m_mainWindowIcon = QIcon( ":/images/chat.png" );
  }

  setMainIcon( c.unreadMessages() > 0 );
  setWindowTitle( window_title );
}

void GuiFloatingChat::updateChatMember( const Chat& c, const User& u )
{
  if( !mp_barMembers->isEnabled() )
    return;

  if( u.isLocal() )
    return;

  QAction* act_user = 0;
  QList<QAction*> member_actions = mp_barMembers->actions();
  foreach( QAction* act, member_actions )
  {
    if( u.id() == Bee::qVariantToVNumber( act->data() ) )
    {
      act_user = act;
      break;
    }
  }

  if( !act_user )
  {
    act_user = mp_barMembers->addAction( u.name(), this, SLOT( onGroupMemberActionTriggered() ) );
    act_user->setData( u.id() );
  }

  act_user->setText( u.name() );
  int avatar_size = qMax( mp_barMembers->iconSize().width(), 32 );
  act_user->setIcon( Bee::avatarForUser( u, QSize( avatar_size, avatar_size ), Settings::instance().showUserPhoto() ) );
  QString user_tooltip = Bee::toolTipForUser( u, true );
  if( !u.isLocal() && u.protocolVersion() >= 63 && !c.userHasReadMessages( u.id() ) )
    user_tooltip += tr( "%1 has not read last messages" ).arg( u.name() );

  act_user->setToolTip( user_tooltip.trimmed() );
  act_user->setEnabled( u.isStatusConnected() );
}

void GuiFloatingChat::updateChatMembers( const Chat& c )
{
  mp_barMembers->setVisible( !c.isDefault() );
  mp_barMembers->setEnabled( !c.isDefault() );

  if( mp_barMembers->isEnabled() )
  {
    mp_barMembers->clear();
    mp_barMembers->setVisible( true );

    UserList ul = UserManager::instance().userList().fromUsersId( c.usersId() );
    foreach( User u, ul.toList() )
      updateChatMember( c, u );

    if( c.isGroup() )
    {
      mp_barMembers->addSeparator();
      mp_barMembers->addAction( mp_actGroupMenu );
    }
    else
      mp_actGroupMenu->setDisabled( true );
  }
}

bool GuiFloatingChat::setChat( const Chat& c )
{
  if( mp_chat->setChat( c ) )
  {
    updateChatTitle( c );
    updateChatMembers( c );
    mp_chat->updateShortcuts();
    mp_chat->updateActions( c, mp_core->isConnected(), mp_core->connectedUsers() );
    return true;
  }
  else
    return false;
}

void GuiFloatingChat::updateActions( bool is_connected, int connected_users )
{
  Chat c = ChatManager::instance().chat( mp_chat->chatId() );
  if( !c.isValid() )
    return;
  mp_chat->updateActions( c, is_connected, connected_users );
}

void GuiFloatingChat::updateUser( const User& u )
{
  Chat c = ChatManager::instance().chat( mp_chat->chatId() );
  if( !c.hasUser( u.id() ) )
    return;

  if( c.isPrivateForUser( u.id() ) )
    updateChatTitle( c );

  updateChatMember( c, u );
}

void GuiFloatingChat::closeEvent( QCloseEvent* e )
{
  QMainWindow::closeEvent( e );
  emit chatIsAboutToClose( mp_chat->chatId() );
  e->accept();
}

void GuiFloatingChat::checkWindowFlagsAndShow()
{
  Bee::setWindowStaysOnTop( this, Settings::instance().stayOnTop() );
  setAttribute( Qt::WA_ShowWithoutActivating );

  if( !isVisible() )
    show();

  if( !Settings::instance().floatingChatState().isEmpty() )
    restoreState( Settings::instance().floatingChatState() );

  if( !Settings::instance().floatingChatGeometry().isEmpty() )
    restoreGeometry( Settings::instance().floatingChatGeometry() );
  else
    resize( 620, 400 );

  QSplitter* chat_splitter = mp_chat->chatSplitter();
  if( Settings::instance().floatingChatSplitterState().isEmpty() )
  {
    int central_widget_height = centralWidget()->size().height();
    QList<int> splitter_size_list;
    splitter_size_list.append( central_widget_height - 80 );
    splitter_size_list.append( 80 );
    chat_splitter->setSizes( splitter_size_list );
  }
  else
    chat_splitter->restoreState( Settings::instance().floatingChatSplitterState() );

  mp_chat->ensureLastMessageVisible();
}

void GuiFloatingChat::showUp()
{
  bool on_top_flag_added = false;

  if( !(windowFlags() & Qt::WindowStaysOnTopHint) )
  {
    Bee::setWindowStaysOnTop( this, true );
    on_top_flag_added = true;
  }

  if( isMinimized() )
    showNormal();

  if( !isVisible() )
    show();

  raise();

  if( on_top_flag_added )
    Bee::setWindowStaysOnTop( this, false );

  mp_chat->ensureLastMessageVisible();
}

void GuiFloatingChat::setFocusInChat()
{
  QWidget* w = QApplication::activeWindow();
  if( !w )
  {
#ifdef BEEBEEP_DEBUG
    qWarning() << "Unable to set focus in chat: application has not the focus";
#endif
    return;
  }

  QApplication::setActiveWindow( this );
  mp_chat->ensureFocusInChat();
}

void GuiFloatingChat::onApplicationFocusChanged( QWidget* old, QWidget* now )
{
  if( old == 0 && isAncestorOf( now )  )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Floating chat" << mp_chat->chatId() << "has grab focus";
#endif
    m_chatIsVisible = true;
    m_prevActivatedState = true;
    mp_chat->updateActionsOnFocusChanged();
    emit readAllMessages( mp_chat->chatId() );
    mp_chat->ensureFocusInChat();
    return;
  }

  if( isAncestorOf( old ) && now == 0 )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Floating chat" << mp_chat->chatId() << "has lost focus";
#endif
    m_chatIsVisible = false;
    m_prevActivatedState = false;
    return;
  }

  bool current_state = isActiveWindow();
  if( current_state != m_prevActivatedState )
  {
    m_prevActivatedState = current_state;
    if( current_state )
    {
#ifdef BEEBEEP_DEBUG
      qDebug() << "Floating chat" << mp_chat->chatId() << "has grab focus (active)";
#endif
      m_chatIsVisible = true;
      mp_chat->updateActionsOnFocusChanged();
      emit readAllMessages( mp_chat->chatId() );
      mp_chat->ensureFocusInChat();
    }
    else
    {
#ifdef BEEBEEP_DEBUG
      qDebug() << "Floating chat" << mp_chat->chatId() << "has lost focus (inactive)";
#endif
      m_chatIsVisible = false;
    }
  }

  qDebug() << "Focus changed";
}

void GuiFloatingChat::saveGeometryAndState()
{
  if( isVisible() )
  {
    Settings::instance().setFloatingChatGeometry( saveGeometry() );
    Settings::instance().setFloatingChatState( saveState() );
    QSplitter* chat_splitter = mp_chat->chatSplitter();
    Settings::instance().setFloatingChatSplitterState( chat_splitter->saveState() );
    Settings::instance().save();
    statusBar()->showMessage( tr( "The window geometry and state are saved" ), 5000 );
  }
  else
    qWarning() << "Unable to save floating chat geometry and state (window is not visible)";
}

void GuiFloatingChat::keyPressEvent( QKeyEvent* e )
{
  if( e->key() == Qt::Key_Escape )
  {
    QMetaObject::invokeMethod( this, "showMinimized", Qt::QueuedConnection );
    e->accept();
    return;
  }

  QMainWindow::keyPressEvent( e );
}

void GuiFloatingChat::setMainIcon( bool with_message )
{
  if( with_message )
    setWindowIcon( QIcon( ":/images/beebeep-message.png" ) );
  else
    setWindowIcon( m_mainWindowIcon );
}

void GuiFloatingChat::updateEmoticon()
{
  QMetaObject::invokeMethod( mp_emoticonsWidget, "updateEmoticons", Qt::QueuedConnection );
}

void GuiFloatingChat::toggleVisibilityEmoticonPanel()
{
  if( mp_dockEmoticons->isVisible() )
    mp_dockEmoticons->hide();
  else
    mp_dockEmoticons->show();
}

void GuiFloatingChat::toggleVisibilityPresetMessagesPanel()
{
  if( mp_dockPresetMessageList->isVisible() )
    mp_dockPresetMessageList->hide();
  else
    mp_dockPresetMessageList->show();
}

void GuiFloatingChat::onGroupMemberActionTriggered()
{
  QAction *act = qobject_cast<QAction*>( sender() );
  if( act )
  {
    VNumber user_id = Bee::qVariantToVNumber( act->data() );
    emit showVCardRequest( user_id );
  }
}

void GuiFloatingChat::setChatReadByUser( const Chat& c, const User& u )
{
  if( c.id() == mp_chat->chatId() )
    updateChatMember( c, u );
}

void GuiFloatingChat::showChatMessage( const Chat& c, const ChatMessage& cm )
{
  User u = UserManager::instance().findUser( cm.userId() );
  if( mp_chat->appendChatMessage( c, u, cm ) )
  {
    if( cm.isFromLocalUser() && !cm.isFromSystem() )
      updateChatMembers( c );
  }
}

void GuiFloatingChat::showGroupMenu()
{
  mp_chat->editChatMembers();
}
