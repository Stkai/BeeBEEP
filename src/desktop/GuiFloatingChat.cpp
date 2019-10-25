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
#ifdef BEEBEEP_USE_VOICE_CHAT
  #include "GuiRecordVoiceMessage.h"
#endif
#include "GuiUserList.h"
#include "IconManager.h"
#include "Settings.h"
#include "ShortcutManager.h"
#include "UserManager.h"
#ifdef Q_OS_WIN
  #include <Windows.h>
#endif


GuiFloatingChat::GuiFloatingChat( QWidget *parent )
 : QMainWindow( parent )
{
  setObjectName( "GuiFloatingChat" );
  m_mainWindowIcon = IconManager::instance().icon( "chat.png" );
  setMainIcon( false );
  mp_chat = new GuiChat( this );

  mp_actGroupMenu = new QAction( IconManager::instance().icon( "group-edit.png" ), tr( "Edit group chat" ), this );
  connect( mp_actGroupMenu, SIGNAL( triggered() ), this, SLOT( showGroupMenu() ) );

  mp_barMembers = new QToolBar( tr( "Show the bar of members" ), this );
  addToolBar( Qt::RightToolBarArea, mp_barMembers );
  mp_barMembers->setObjectName( "GuiFloatingChatMemberToolBar" );
  mp_barMembers->setIconSize( Settings::instance().avatarIconSize() );
  mp_barMembers->setAllowedAreas( Qt::AllToolBarAreas );
  mp_barMembers->setFloatable( false );
  mp_barMembers->toggleViewAction()->setVisible( false );

  mp_barChat = new QToolBar( tr( "Show chat toolbar" ), this );
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
  actViewPresetMessageList->setIcon( IconManager::instance().icon( "preset-message.png" ) );
  actViewPresetMessageList->setToolTip( tr( "Show the preset messages panel" ) );
  mp_barChat->insertAction( mp_barChat->actions().first(), actViewPresetMessageList );
  mp_actSaveWindowGeometry = mp_barChat->addAction( IconManager::instance().icon( "save-window.png" ), tr( "Save window's geometry" ), this, SLOT( saveGeometryAndState() ) );

  mp_dockEmoticons = new QDockWidget( tr( "Emoticons" ), this );
  mp_dockEmoticons->setObjectName( "GuiDockEmoticons" );
  mp_emoticonsWidget = new GuiEmoticons( this );
  updateEmoticons();
  connect( mp_emoticonsWidget, SIGNAL( emoticonSelected( const Emoticon& ) ), mp_chat, SLOT( addEmoticon( const Emoticon& ) ) );
  mp_dockEmoticons->setWidget( mp_emoticonsWidget );
  mp_dockEmoticons->setAllowedAreas( Qt::AllDockWidgetAreas );
  addDockWidget( Qt::LeftDockWidgetArea, mp_dockEmoticons );
  QAction* mp_actViewEmoticons = mp_dockEmoticons->toggleViewAction();
  mp_actViewEmoticons->setIcon( IconManager::instance().icon( "emoticon.png" ) );
  mp_actViewEmoticons->setText( tr( "Show the emoticon panel" ) );
  mp_actViewEmoticons->setVisible( !Settings::instance().useOnlyTextEmoticons() );
  mp_barChat->insertAction( mp_barChat->actions().first(), mp_actViewEmoticons );

  setCentralWidget( mp_chat );
  statusBar();
  m_chatIsVisible = false;
  m_prevActivatedState = false;

  connect( mp_chat, SIGNAL( toggleVisibilityEmoticonsPanelRequest() ), this, SLOT( toggleVisibilityEmoticonPanel() ) );
  connect( mp_chat, SIGNAL( toggleVisibilityPresetMessagesPanelRequest() ), this, SLOT( toggleVisibilityPresetMessagesPanel() ) );
  connect( mp_chat, SIGNAL( hideRequest() ), this, SLOT( showMinimized() ) );
  connect( mp_chat, SIGNAL( closeRequest() ), this, SLOT( close() ) );
  connect( mp_chat, SIGNAL( updateChatColorsRequest() ), this, SIGNAL( updateChatColorsRequest() ) );
  connect( mp_chat, SIGNAL( showStatusMessageRequest( const QString&, int ) ), this, SLOT( showStatusMessage(const QString&, int ) ) );
#ifdef BEEBEEP_USE_VOICE_CHAT
  connect( mp_chat, SIGNAL( showVoiceMessageDialogRequest() ), this, SLOT( showRecordMessageDialog() ) );
#endif
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
      QString user_name = Bee::userNameToShow( u );

      if( !user_status.isEmpty() && !user_status_description.isEmpty() )
        window_title = QString( "%1 [%2 - %3]" ).arg( user_name, Bee::userStatusToString( u.status() ), user_status_description );
      else if( !user_status.isEmpty() )
        window_title = QString( "%1 (%2)" ).arg( user_name, user_status );
      else if( !user_status_description.isEmpty() )
        window_title = QString( "%1 [%2]" ).arg( user_name, user_status_description );
      else
        window_title = user_name;

      m_mainWindowIcon = Bee::avatarForUser( u, QSize( 256, 256 ), true );
      setWindowTitle( window_title );
    }
    else
    {
      qWarning() << "Invalid user" << user_id << "found for private chat" << c.name();
      m_mainWindowIcon = IconManager::instance().icon( "chat.png" );
      window_title = c.name();
    }
  }
  else if( c.isDefault() )
  {
    window_title = tr( "Chat with all users" );
    m_mainWindowIcon = IconManager::instance().icon( "default-chat-online.png" );
  }
  else if( c.isGroup() )
  {
    window_title = c.name();
    m_mainWindowIcon = IconManager::instance().icon( "group.png" );
  }
  else
  {
    window_title = c.name();
    m_mainWindowIcon = IconManager::instance().icon( "chat.png" );
  }

  setMainIcon( c.unreadMessages() > 0 );
  setWindowTitle( c.isDefault() ? window_title : QString( "%1 - %2").arg( window_title, "Chat" ) );
}

void GuiFloatingChat::updateChatMember( const Chat& c, const User& u )
{
  if( !mp_barMembers->isEnabled() )
    return;

  if( u.isLocal() )
    return;

  QAction* act_user = Q_NULLPTR;
  QList<QAction*> member_actions = mp_barMembers->actions();
  foreach( QAction* act, member_actions )
  {
    if( u.id() == Bee::qVariantToVNumber( act->data() ) )
    {
      act_user = act;
      break;
    }
  }

  QString user_name = Bee::userNameToShow( u );
  if( !act_user )
  {
    act_user = mp_barMembers->addAction( user_name, this, SLOT( onGroupMemberActionTriggered() ) );
    act_user->setData( u.id() );
  }
  else
    act_user->setText( user_name );

  int avatar_size = mp_barMembers->iconSize().width();
  QString user_tooltip = Bee::toolTipForUser( u, true );
  if( !u.isLocal() && u.protocolVersion() >= 63 && !c.userHasReadMessages( u.id() ) )
  {
    user_tooltip += QString( "\n%1" ).arg( tr( "%1 has not read last messages" ).arg( u.name() ) );
    act_user->setIcon( Bee::avatarForUser( u, QSize( avatar_size, avatar_size ), Settings::instance().showUserPhoto(), User::Away ) );
  }
  else
    act_user->setIcon( Bee::avatarForUser( u, QSize( avatar_size, avatar_size ), Settings::instance().showUserPhoto() ) );
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
    mp_barMembers->setIconSize( Settings::instance().avatarIconSize() );
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

void GuiFloatingChat::updateChat( const Chat& c )
{
  if( mp_chat->updateChat( c ) )
  {
    setMainIcon( c.unreadMessages() > 0 );
    updateChatTitle( c );
    updateChatMembers( c );
    mp_chat->updateShortcuts();
    mp_chat->updateActions( c, beeCore->isConnected(), beeCore->connectedUsers(), beeCore->isFileTransferActive() );
  }
}

bool GuiFloatingChat::setChat( const Chat& c )
{
  if( mp_chat->setChat( c ) )
  {
    setMainIcon( c.unreadMessages() > 0 );
    updateChatTitle( c );
    updateChatMembers( c );
    mp_chat->updateShortcuts();
    mp_chat->updateActions( c, beeCore->isConnected(), beeCore->connectedUsers(), beeCore->isFileTransferActive() );
    return true;
  }
  else
    return false;
}

void GuiFloatingChat::updateActions( bool is_connected, int connected_users, bool file_transfer_is_active )
{
  Chat c = ChatManager::instance().chat( mp_chat->chatId() );
  if( !c.isValid() )
    return;
  mp_chat->updateActions( c, is_connected, connected_users, file_transfer_is_active );
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
  if( Settings::instance().floatingChatState().isEmpty() )
    Settings::instance().setShowEmoticonMenu( mp_dockEmoticons->isVisible() );
  QMainWindow::closeEvent( e );
  emit chatIsAboutToClose( mp_chat->chatId() );
  e->accept();
}

void GuiFloatingChat::setWindowFlagsAndGeometry()
{
  setAttribute( Qt::WA_ShowWithoutActivating );
  Bee::setWindowStaysOnTop( this, Settings::instance().stayOnTop() );

  if( Settings::instance().floatingChatGeometry().isEmpty() )
  {
    resize( 600, 400 );
    if( !QApplication::activeWindow() )
      move( QApplication::desktop()->availableGeometry().width() - frameGeometry().width() - 30, 40 );
  }
  else
    restoreGeometry( Settings::instance().floatingChatGeometry() );

  if( Settings::instance().floatingChatState().isEmpty() )
  {
    mp_dockEmoticons->setVisible( Settings::instance().showEmoticonMenu() );
    mp_dockPresetMessageList->hide();
  }
  else
    restoreState( Settings::instance().floatingChatState() );

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
}

void GuiFloatingChat::showUp()
{
  Bee::showUp( this );
  mp_chat->ensureLastMessageVisible();
}

void GuiFloatingChat::raiseOnTop()
{
  Bee::raiseOnTop( this );
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
  if( old == Q_NULLPTR && isAncestorOf( now )  )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Floating chat" << mp_chat->chatId() << "has grab focus";
#endif
    m_chatIsVisible = true;
    m_prevActivatedState = true;
    setWindowOpacity( Settings::instance().chatActiveWindowOpacityLevel() / 100.0 );
    mp_chat->updateActionsOnFocusChanged();
    emit readAllMessages( mp_chat->chatId() );
    mp_chat->ensureFocusInChat();
    return;
  }

  if( isAncestorOf( old ) && now == Q_NULLPTR )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Floating chat" << mp_chat->chatId() << "has lost focus";
#endif
    m_chatIsVisible = false;
    m_prevActivatedState = false;
    setWindowOpacity( Settings::instance().chatInactiveWindowOpacityLevel() / 100.0 );
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
      setWindowOpacity( Settings::instance().chatActiveWindowOpacityLevel() / 100.0 );
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
      setWindowOpacity( Settings::instance().chatInactiveWindowOpacityLevel() / 100.0 );
    }
  }
}

void GuiFloatingChat::saveGeometryAndState()
{
  if( isVisible() )
  {
    QByteArray ba_state = saveState();
#if QT_VERSION == 0x050906
    int default_button = mp_dockEmoticons->isVisible() || mp_dockPresetMessageList->isVisible() ? 0 : 1;
    int ret_code = QMessageBox::warning( this, Settings::instance().programName(),
                                         tr( "Qt libraries have a bug on saving the window's state." ) + QString( " " ) +
                                         tr( "If you have layout problem please save only geometry." ),
                                         tr( "Save all" ), tr( "Save only geometry" ), tr( "Cancel" ), default_button, 2 );
    switch( ret_code )
    {
    case 0:
      break;
    case 1:
      ba_state = QByteArray();
      break;
    default:
      return;
    }
#endif
    Settings::instance().setFloatingChatGeometry( saveGeometry() );
    Settings::instance().setFloatingChatState( ba_state );
    QSplitter* chat_splitter = mp_chat->chatSplitter();
    Settings::instance().setFloatingChatSplitterState( chat_splitter->saveState() );
    Settings::instance().setShowEmoticonMenu( mp_dockEmoticons->isVisible() );
    Settings::instance().save();
    if( ba_state.isEmpty() )
      statusBar()->showMessage( tr( "Window geometry saved" ), 5000 );
    else
      statusBar()->showMessage( tr( "Window geometry and state saved" ), 5000 );
  }
  else
    qWarning() << "Unable to save floating chat geometry and state (window is not visible)";
}

void GuiFloatingChat::keyPressEvent( QKeyEvent* e )
{
  if( e->key() == Qt::Key_Escape )
  {
    if( Settings::instance().keyEscapeMinimizeInTray() )
      QMetaObject::invokeMethod( this, "close", Qt::QueuedConnection );
    else
      QMetaObject::invokeMethod( this, "showMinimized", Qt::QueuedConnection );
    e->accept();
    return;
  }

  QMainWindow::keyPressEvent( e );
}

void GuiFloatingChat::setMainIcon( bool with_message )
{
  if( with_message )
    setWindowIcon( IconManager::instance().icon( "beebeep-message.png" ) );
  else
    setWindowIcon( m_mainWindowIcon );
}

void GuiFloatingChat::updateEmoticons()
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
  if( mp_chat->appendChatMessage( c, cm ) )
  {
    if( cm.isFromLocalUser() )
    {
      updateChatMembers( c );
    }
    else
    {
      if( !cm.isFromSystem()  )
        statusBar()->showMessage( "" ); // reset writing message
    }
  }
}

void GuiFloatingChat::showGroupMenu()
{
  mp_chat->editChatMembers();
}

void GuiFloatingChat::onTickEvent( int ticks )
{
  mp_chat->onTickEvent( ticks );
}

void GuiFloatingChat::showStatusMessage( const QString& msg, int timeout )
{
  statusBar()->showMessage( msg, timeout );
  QApplication::processEvents();
}

void GuiFloatingChat::loadSavedMessages()
{
  QTimer::singleShot( 0, mp_chat, SLOT( loadSavedMessages() ) );
}

#ifdef BEEBEEP_USE_VOICE_CHAT
void GuiFloatingChat::showRecordMessageDialog()
{
  if( Settings::instance().disableVoiceMessages() )
    return;
  GuiRecordVoiceMessage* grvm = new GuiRecordVoiceMessage( this );
  grvm->setModal( true );
  grvm->setRecipient( ChatManager::instance().chatName( mp_chat->chatId() ) );
  grvm->show();
  if( grvm->exec() == QDialog::Accepted )
    emit sendVoiceMessageRequest( mp_chat->chatId(), grvm->filePath() );
  // deleted in Close Event to bypass crash if you close a modal dialog with QUIT
}
#endif
