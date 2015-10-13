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
#include "GuiFloatingChat.h"
#include "GuiEmoticons.h"
#include "Settings.h"


GuiFloatingChat::GuiFloatingChat( QWidget *parent )
 : QMainWindow( parent )
{
  setObjectName( "GuiFloatingChat" );
  mp_chat = new GuiChat( this );

  mp_barChat = new QToolBar( tr( "Show the bar of chat" ), this );
  addToolBar( Qt::BottomToolBarArea, mp_barChat );
  mp_barChat->setObjectName( "GuiFloatingChatToolBar" );
  mp_barChat->setIconSize( Settings::instance().mainBarIconSize() );
  mp_barChat->setAllowedAreas( Qt::AllToolBarAreas );
  mp_chat->setupToolBar( mp_barChat );

  mp_dockEmoticons = new QDockWidget( tr( "Emoticons" ), this );
  mp_dockEmoticons->setObjectName( "GuiDockEmoticons" );

  mp_emoticonsWidget = new GuiEmoticons( this );
  mp_emoticonsWidget->initEmoticons();
  connect( mp_emoticonsWidget, SIGNAL( emoticonSelected( const Emoticon& ) ), mp_chat, SLOT( addEmoticon( const Emoticon& ) ) );
  mp_dockEmoticons->setWidget( mp_emoticonsWidget );

  mp_dockEmoticons->setAllowedAreas( Qt::AllDockWidgetAreas );
  addDockWidget( Qt::LeftDockWidgetArea, mp_dockEmoticons );
  QAction* mp_actViewEmoticons = mp_dockEmoticons->toggleViewAction();
  mp_actViewEmoticons->setIcon( QIcon( ":/images/emoticon.png" ) );
  mp_actViewEmoticons->setText( tr( "Show the emoticon panel" ) );
  mp_actViewEmoticons->setStatusTip( tr( "Add your preferred emoticon to the message" ) );
  mp_actViewEmoticons->setData( 28 );
  mp_barChat->insertAction( mp_barChat->actions().first(), mp_actViewEmoticons );
  mp_dockEmoticons->hide();

  setCentralWidget( mp_chat );
  statusBar();
}

bool GuiFloatingChat::setChatId( VNumber chat_id )
{
  Chat c = ChatManager::instance().chat( chat_id );
  if( c.isPrivate() )
  {
    QStringList sl = c.name().split( "@" );
    if( !sl.isEmpty() )
    {
      sl.removeLast();
      setWindowTitle( sl.join( "@" ) );
    }
    else
      setWindowTitle( c.name() );
  }
  else
    setWindowTitle( c.name() );

  return mp_chat->setChatId( chat_id );
}

void GuiFloatingChat::closeEvent( QCloseEvent* e )
{
  QMainWindow::closeEvent( e );
  emit attachChatRequest( chatId() );
  e->accept();
}

void GuiFloatingChat::applyFlagStaysOnTop()
{
  if( Settings::instance().stayOnTop() )
  {
    setWindowFlags( windowFlags() | Qt::WindowStaysOnTopHint );
  }
  else
  {
    setWindowFlags( windowFlags() & ~Qt::WindowStaysOnTopHint );
  }
}

void GuiFloatingChat::checkWindowFlagsAndShow()
{
  applyFlagStaysOnTop();

  show();

  QSplitter* chat_splitter = mp_chat->chatSplitter();
  if( Settings::instance().resetGeometryAtStartup() || Settings::instance().chatSplitterState().isEmpty() )
  {
    int central_widget_height = centralWidget()->size().height();
#ifdef BEEBEEP_DEBUG
    qDebug() << "Central widget height is" << central_widget_height << "then chat view height will be" << (int)(central_widget_height-80);
#endif
    QList<int> splitter_size_list;
    splitter_size_list.append( central_widget_height - 80);
    splitter_size_list.append( 80 );
    chat_splitter->setSizes( splitter_size_list );
  }
  else
  {
    chat_splitter->restoreState( Settings::instance().chatSplitterState() );
  }
}

void GuiFloatingChat::raiseOnTop()
{
  if( isMinimized() || !isVisible() )
    showNormal();
  else
    show();

#ifdef Q_OS_WIN
  SetWindowPos( (HWND)winId(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
  SetWindowPos( (HWND)winId(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
  SetActiveWindow( (HWND)winId() );
  SetFocus( (HWND)winId() );
#else
  raise();
  qApp->setActiveWindow( this );
#endif

  mp_chat->ensureFocusInChat();
}

void GuiFloatingChat::showUserWriting( VNumber user_id, const QString& msg )
{
  if( mp_chat->hasUser( user_id ) )
    statusBar()->showMessage( msg, Settings::instance().writingTimeout() );
}
