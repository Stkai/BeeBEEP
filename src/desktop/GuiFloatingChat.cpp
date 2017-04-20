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


GuiFloatingChat::GuiFloatingChat( QWidget *parent )
 : QMainWindow( parent )
{
  setObjectName( "GuiFloatingChat" );
  m_mainWindowIcon = QIcon( ":/images/chat-view.png" );
  setMainIcon( false );
  mp_chat = new GuiChat( this );
  connect( mp_chat, SIGNAL( saveStateAndGeometryRequest() ), this, SLOT( saveGeometryAndState() ) );

  mp_barChat = new QToolBar( tr( "Show the bar of chat" ), this );
  addToolBar( Qt::BottomToolBarArea, mp_barChat );
  mp_barChat->setObjectName( "GuiFloatingChatToolBar" );
  mp_barChat->setIconSize( Settings::instance().mainBarIconSize() );
  mp_barChat->setAllowedAreas( Qt::AllToolBarAreas );
  mp_chat->setupToolBar( mp_barChat );
  mp_barChat->setVisible( Settings::instance().showChatToolbar() );

  mp_barChat->insertSeparator( mp_barChat->actions().first() );
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
  m_prevActivatedState = true;

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
  else
  {
    window_title = c.name();
    m_mainWindowIcon = QIcon( ":/images/group.png" );
  }

  setMainIcon( c.unreadMessages() > 0 );
  setWindowTitle( window_title );
}

bool GuiFloatingChat::setChat( const Chat& c )
{
  updateChatTitle( c );
  return mp_chat->setChat( c );
}

void GuiFloatingChat::updateUser( const User& u )
{
  Chat c = ChatManager::instance().chat( mp_chat->chatId() );
  if( !c.hasUser( u.id() ) )
    return;

  if( c.isPrivateForUser( u.id() ) )
    updateChatTitle( c );

  mp_chat->updateUsers( c );
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

  QSplitter* chat_splitter = mp_chat->chatSplitter();
  if( Settings::instance().floatingChatSplitterState().isEmpty() )
  {
    int central_widget_height = centralWidget()->size().height();
    QList<int> splitter_size_list;
    splitter_size_list.append( central_widget_height - 80);
    splitter_size_list.append( 80 );
    chat_splitter->setSizes( splitter_size_list );
  }
  else
    chat_splitter->restoreState( Settings::instance().floatingChatSplitterState() );
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
}

void GuiFloatingChat::setFocusInChat()
{
  QWidget* w = QApplication::activeWindow();
  if( !w )
    return;
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
