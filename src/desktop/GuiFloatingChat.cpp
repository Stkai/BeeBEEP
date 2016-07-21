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
  mp_actViewEmoticons->setStatusTip( tr( "Add your preferred emoticon to the message" ) );
  mp_barChat->insertAction( mp_barChat->actions().first(), mp_actViewEmoticons );
  mp_dockEmoticons->hide();

  setCentralWidget( mp_chat );
  statusBar();
  m_chatIsVisible = true;
  m_prevActivatedState = true;

  connect( mp_chat, SIGNAL( toggleVisibilityEmoticonsPanelRequest() ), this, SLOT( toggleVisibilityEmoticonPanel() ) );
  connect( qApp, SIGNAL( focusChanged( QWidget*, QWidget* ) ), this, SLOT( onApplicationFocusChanged( QWidget*, QWidget* ) ) );
}

bool GuiFloatingChat::setChatId( VNumber chat_id )
{
  Chat c = ChatManager::instance().chat( chat_id );
  if( c.isPrivate() )
  {
    VNumber user_id = c.privateUserId();
    if( user_id != ID_INVALID )
    {
      User u = UserManager::instance().findUser( user_id );
      if( u.isValid() )
        setWindowTitle( u.name() );
      else
        setWindowTitle( c.name() );
    }
    else
      setWindowTitle( c.name() );
  }
  else
    setWindowTitle( c.name() );

  return mp_chat->setChatId( chat_id, true );
}

void GuiFloatingChat::updateUser( const User& u, bool is_connected )
{
  Chat c = ChatManager::instance().chat( mp_chat->chatId() );
  if( !c.hasUser( u.id() ) )
    return;

  if( c.isPrivateForUser( u.id() ) )
  {
    QString window_title = QString( "%1 (%2)" ).arg( u.name(), is_connected ? Bee::userStatusToString( u.status() ) : tr( "offline" ) );
    setWindowTitle( window_title );
  }

  mp_chat->updateUser( u );
}

void GuiFloatingChat::closeEvent( QCloseEvent* e )
{
  QMainWindow::closeEvent( e );
  emit attachChatRequest( chatId() );
  e->accept();
}

void GuiFloatingChat::applyFlagStaysOnTop()
{
#ifdef Q_OS_WIN
  if( Settings::instance().stayOnTop() )
    SetWindowPos( (HWND)winId(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE );
  else
    SetWindowPos( (HWND)winId(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE );
#else
  Qt::WindowFlags w_flags = this->windowFlags();
  if( Settings::instance().stayOnTop() )
    w_flags |= Qt::WindowStaysOnTopHint;
  else
    w_flags &= ~Qt::WindowStaysOnTopHint;
  setWindowFlags( w_flags );
#endif
  show();
}

void GuiFloatingChat::checkWindowFlagsAndShow()
{
  applyFlagStaysOnTop();

  if( !Settings::instance().floatingChatGeometry().isEmpty() )
    restoreGeometry( Settings::instance().floatingChatGeometry() );

  if( !Settings::instance().floatingChatState().isEmpty() )
    restoreState( Settings::instance().floatingChatState() );

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

void GuiFloatingChat::raiseOnTop()
{
  if( isMinimized() || !isVisible() )
    showNormal();

#ifdef Q_OS_WIN
  SetWindowPos( (HWND)winId(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW );
  SetWindowPos( (HWND)winId(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW );
  applyFlagStaysOnTop();
  SetActiveWindow( (HWND)winId() );
  SetFocus( (HWND)winId() );
#else
  raise();
  qApp->setActiveWindow( this );
#endif

  emit readAllMessages( chatId() );
  mp_chat->ensureFocusInChat();
}

void GuiFloatingChat::showUserWriting( VNumber user_id, const QString& msg )
{
  if( mp_chat->hasUser( user_id ) )
    statusBar()->showMessage( msg, Settings::instance().writingTimeout() );
}

void GuiFloatingChat::onApplicationFocusChanged( QWidget* old, QWidget* now )
{
  if( old == 0 && isAncestorOf( now )  )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Floating chat" << chatId() << "has grab focus";
#endif
    m_chatIsVisible = true;
    m_prevActivatedState = true;
    mp_chat->updateActionsOnFocusChanged();
    emit readAllMessages( chatId() );
    mp_chat->ensureFocusInChat();
    return;
  }

  if( isAncestorOf( old ) && now == 0 )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Floating chat" << chatId() << "has lost focus";
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
      qDebug() << "Floating chat" << chatId() << "has grab focus (active)";
#endif
      m_chatIsVisible = true;
      mp_chat->updateActionsOnFocusChanged();
      emit readAllMessages( chatId() );
      mp_chat->ensureFocusInChat();
    }
    else
    {
#ifdef BEEBEEP_DEBUG
      qDebug() << "Floating chat" << chatId() << "has lost focus (inactive)";
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
    statusBar()->showMessage( tr( "Window's geometry and state saved" ), 3000 );
  }
  else
    qWarning() << "Unable to save floating chat geometry and state (window is not visible)";
}

void GuiFloatingChat::keyPressEvent( QKeyEvent* e )
{
  if( e->key() == Qt::Key_Escape )
  {
    QTimer::singleShot( 0, this, SLOT( showMinimized() ) );
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
    setWindowIcon( QIcon( ":/images/beebeep.png" ) );
}

void GuiFloatingChat::showUp()
{
  raiseOnTop();
}

void GuiFloatingChat::updateEmoticon()
{
  QTimer::singleShot( 0, mp_emoticonsWidget, SLOT( updateEmoticons() ) );
}

void GuiFloatingChat::toggleVisibilityEmoticonPanel()
{
  if( mp_dockEmoticons->isVisible() )
    mp_dockEmoticons->hide();
  else
    mp_dockEmoticons->show();
}
