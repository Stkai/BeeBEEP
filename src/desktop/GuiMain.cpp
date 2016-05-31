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

#include "AudioManager.h"
#include "Core.h"
#include "BeeApplication.h"
#include "BeeUtils.h"
#include "ChatManager.h"
#include "FileDialog.h"
#include "FileShare.h"
#include "GuiAddUser.h"
#include "GuiAskPassword.h"
#include "GuiChat.h"
#include "GuiChatList.h"
#include "GuiCreateGroup.h"
#include "GuiEditVCard.h"
#include "GuiEmoticons.h"
#include "GuiFloatingChat.h"
#include "GuiGroupList.h"
#include "GuiHome.h"
#include "GuiLanguage.h"
#include "GuiLog.h"
#include "GuiPluginManager.h"
#include "GuiSavedChat.h"
#include "GuiSavedChatList.h"
#include "GuiScreenShot.h"
#include "GuiSearchUser.h"
#include "GuiShareBox.h"
#include "GuiShareLocal.h"
#include "GuiShareNetwork.h"
#include "GuiShortcut.h"
#include "GuiSystemTray.h"
#include "GuiTransferFile.h"
#include "GuiUserList.h"
#include "GuiMain.h"
#include "GuiVCard.h"
#include "GuiWizard.h"
#include "PluginManager.h"
#include "Protocol.h"
#include "SaveChatList.h"
#include "Settings.h"
#include "ShortcutManager.h"
#include "SpellChecker.h"
#include "UserManager.h"
#ifdef Q_OS_WIN
  #include <windows.h>
#endif


GuiMain::GuiMain( QWidget *parent )
 : QMainWindow( parent ), m_floatingChats()
{
  setObjectName( "GuiMainWindow" );

  mp_core = new Core( this );

  updateMainIcon();
  // Create a status bar before the actions and the menu
  (void) statusBar();

  mp_defaultGameMenu = new QMenu( this );

  mp_stackedWidget = new QStackedWidget( this );
  createStackedWidgets();
  setCentralWidget( mp_stackedWidget );

  mp_barMain = addToolBar( tr( "Show the main tool bar" ) );
  mp_barMain->setObjectName( "GuiMainToolBar" );
  mp_barMain->setIconSize( Settings::instance().mainBarIconSize() );
  mp_barGames = 0;
  mp_trayIcon = new GuiSystemTray( this );

  m_lastUserStatus = User::Online;
  m_forceShutdown = false;
  m_autoConnectOnInterfaceUp = false;
  m_prevActivatedState = true;

  createActions();
  createDockWindows();
  createMenus();
  createToolAndMenuBars();
  createPluginWindows();
  updadePluginMenu();

  connect( mp_core, SIGNAL( chatMessage( VNumber, const ChatMessage& ) ), this, SLOT( showChatMessage( VNumber, const ChatMessage& ) ) );
  connect( mp_core, SIGNAL( fileDownloadRequest( const User&, const FileInfo& ) ), this, SLOT( downloadFile( const User&, const FileInfo& ) ) );
  connect( mp_core, SIGNAL( folderDownloadRequest( const User&, const QString&, const QList<FileInfo>& ) ), this, SLOT( downloadFolder( const User&, const QString&, const QList<FileInfo>& ) ) );
  connect( mp_core, SIGNAL( userChanged( const User& ) ), this, SLOT( updateUser( const User& ) ) );
  connect( mp_core, SIGNAL( userIsWriting( const User& ) ), this, SLOT( showWritingUser( const User& ) ) );
  connect( mp_core, SIGNAL( fileTransferProgress( VNumber, const User&, const FileInfo&, FileSizeType ) ), mp_fileTransfer, SLOT( setProgress( VNumber, const User&, const FileInfo&, FileSizeType ) ) );
  connect( mp_core, SIGNAL( fileTransferMessage( VNumber, const User&, const FileInfo&, const QString& ) ), mp_fileTransfer, SLOT( setMessage( VNumber, const User&, const FileInfo&, const QString& ) ) );
  connect( mp_core, SIGNAL( fileShareAvailable( const User& ) ), this, SLOT( showSharesForUser( const User& ) ) );
  connect( mp_core, SIGNAL( updateChat( VNumber ) ), this, SLOT( checkChat( VNumber ) ) );
  connect( mp_core, SIGNAL( localShareListAvailable() ), mp_shareLocal, SLOT( updateFileSharedList() ) );
  connect( mp_core, SIGNAL( savedChatListAvailable() ), this, SLOT( loadSavedChatsCompleted() ) );
  connect( mp_core, SIGNAL( updateStatus( const QString&, int ) ), this, SLOT( showMessage( const QString&, int ) ) );
  connect( mp_core, SIGNAL( updateGroup( VNumber ) ), this, SLOT( checkGroup( VNumber ) ) );
  connect( mp_core, SIGNAL( userConnectionStatusChanged( const User& ) ), this, SLOT( showConnectionStatusChanged( const User& ) ) );
  connect( mp_core, SIGNAL( networkInterfaceIsDown() ), this, SLOT( onNetworkInterfaceDown() ) );
  connect( mp_core, SIGNAL( networkInterfaceIsUp() ), this, SLOT( onNetworkInterfaceUp() ) );
  connect( mp_core, SIGNAL( chatReadByUser( VNumber, VNumber ) ), this, SLOT( onChatReadByUser( VNumber, VNumber ) ) );
  connect( mp_fileTransfer, SIGNAL( transferCancelled( VNumber ) ), mp_core, SLOT( cancelFileTransfer( VNumber ) ) );
  connect( mp_fileTransfer, SIGNAL( stringToShow( const QString&, int ) ), this, SLOT( showMessage( const QString&, int ) ) );
  connect( mp_fileTransfer, SIGNAL( fileTransferProgress( VNumber, VNumber, const QString& ) ), mp_shareNetwork, SLOT( showMessage( VNumber, VNumber, const QString& ) ) );
  connect( mp_fileTransfer, SIGNAL( fileTransferCompleted( VNumber, VNumber, const QString& ) ), mp_shareNetwork, SLOT( setFileTransferCompleted( VNumber, VNumber, const QString& ) ) );
  connect( mp_fileTransfer, SIGNAL( openFileCompleted( const QUrl& ) ), this, SLOT( openUrl( const QUrl& ) ) );

  setupChatConnections( mp_chat );
  connect( mp_chat, SIGNAL( toggleVisibilityEmoticonsPanelRequest() ), this, SLOT( toggleVisibilityEmoticonPanel() ) );

  connect( mp_savedChat, SIGNAL( openUrl( const QUrl& ) ), this, SLOT( openUrl( const QUrl& ) ) );

  connect( mp_shareLocal, SIGNAL( sharePathAdded( const QString& ) ), this, SLOT( addToShare( const QString& ) ) );
  connect( mp_shareLocal, SIGNAL( sharePathRemoved( const QString& ) ), this, SLOT( removeFromShare( const QString& ) ) );
  connect( mp_shareLocal, SIGNAL( openUrlRequest( const QUrl& ) ), this, SLOT( openUrl( const QUrl& ) ) );
  connect( mp_shareLocal, SIGNAL( updateListRequest() ), mp_core, SLOT( buildLocalShareList() ) );
  connect( mp_shareLocal, SIGNAL( removeAllPathsRequest() ), mp_core, SLOT( removeAllPathsFromShare() ) );

  connect( mp_shareNetwork, SIGNAL( fileShareListRequested() ), mp_core, SLOT( sendFileShareRequestToAll() ) );
  connect( mp_shareNetwork, SIGNAL( downloadSharedFile( VNumber, VNumber ) ), this, SLOT( downloadSharedFile( VNumber, VNumber ) ) );
  connect( mp_shareNetwork, SIGNAL( downloadSharedFiles( const QList<SharedFileInfo>& ) ), this, SLOT( downloadSharedFiles( const QList<SharedFileInfo>& ) ) );
  connect( mp_shareNetwork, SIGNAL( openFileCompleted( const QUrl& ) ), this, SLOT( openUrl( const QUrl& ) ) );
  connect( mp_shareNetwork, SIGNAL( updateStatus( const QString&, int ) ), this, SLOT( showMessage( const QString&, int ) ) );

  connect( mp_userList, SIGNAL( chatSelected( VNumber ) ), this, SLOT( showChat( VNumber ) ) );
  connect( mp_userList, SIGNAL( userSelected( VNumber ) ), this, SLOT( checkUserSelected( VNumber ) ) );
  connect( mp_userList, SIGNAL( showVCardRequest( VNumber, bool ) ), this, SLOT( showVCard( VNumber, bool ) ) );

  connect( mp_groupList, SIGNAL( openChatForGroupRequest( VNumber ) ), this, SLOT( showChatForGroup( VNumber ) ) );
  connect( mp_groupList, SIGNAL( createGroupRequest() ), this, SLOT( createGroup() ) );
  connect( mp_groupList, SIGNAL( editGroupRequest( VNumber ) ), this, SLOT( editGroup( VNumber ) ) );
  connect( mp_groupList, SIGNAL( showVCardRequest( VNumber, bool ) ), this, SLOT( showVCard( VNumber, bool ) ) );
  connect( mp_groupList, SIGNAL( removeGroupRequest( VNumber ) ), this, SLOT( removeGroup( VNumber ) ) );

  connect( mp_chatList, SIGNAL( chatSelected( VNumber ) ), this, SLOT( showChat( VNumber ) ) );
  connect( mp_chatList, SIGNAL( chatToClear( VNumber ) ), this, SLOT( clearChat( VNumber ) ) );
  connect( mp_chatList, SIGNAL( chatToRemove( VNumber ) ), this, SLOT( removeChat( VNumber ) ) );

  connect( mp_trayIcon, SIGNAL( activated( QSystemTrayIcon::ActivationReason ) ), this, SLOT( trayIconClicked( QSystemTrayIcon::ActivationReason ) ) );
  connect( mp_trayIcon, SIGNAL( messageClicked() ), this, SLOT( trayMessageClicked() ) );

  connect( mp_savedChatList, SIGNAL( savedChatSelected( const QString& ) ), this, SLOT( showSavedChatSelected( const QString& ) ) );
  connect( mp_savedChatList, SIGNAL( savedChatRemoved( const QString& ) ), this, SLOT( removeSavedChat( const QString& ) ) );
  connect( mp_savedChatList, SIGNAL( savedChatLinkRequest( const QString& ) ), this, SLOT( linkSavedChat( const QString& ) ) );

  connect( mp_screenShot, SIGNAL( hideRequest() ), this, SLOT( hide() ) );
  connect( mp_screenShot, SIGNAL( showRequest() ), this, SLOT( show() ) );
  connect( mp_screenShot, SIGNAL( screenShotToSend( const QString& ) ), this, SLOT( sendFile( const QString& ) ) );

  connect( mp_home, SIGNAL( openDefaultChatRequest() ), this, SLOT( showDefaultChat() ) );
  connect( mp_home, SIGNAL( openUrlRequest( const QUrl& ) ), this, SLOT( openUrl( const QUrl& ) ) );

  mp_chat->setChatId( ID_DEFAULT_CHAT, false );
  mp_home->loadDefaultChat();

  initShortcuts();
  initGuiItems();
  updateShortcuts();

  connect( qApp, SIGNAL( focusChanged( QWidget*, QWidget* ) ), this, SLOT( onApplicationFocusChanged( QWidget*, QWidget* ) ) );

  statusBar()->showMessage( tr( "Ready" ) );
}

void GuiMain::initShortcuts()
{
  mp_scMinimizeAllChats = new QShortcut( this );
  mp_scMinimizeAllChats->setContext( Qt::ApplicationShortcut );
  connect( mp_scMinimizeAllChats, SIGNAL( activated() ), this, SLOT( minimizeAllChats() ) );

#ifdef BEEBEEP_USE_QXT
  mp_scShowAllChats = new QxtGlobalShortcut( this );
  connect( mp_scShowAllChats, SIGNAL( activated() ), this, SLOT( showAllChats() ) );
#endif

  mp_scShowNextUnreadMessage = new QShortcut( this );
  mp_scShowNextUnreadMessage->setContext( Qt::ApplicationShortcut );
  connect( mp_scShowNextUnreadMessage, SIGNAL( activated() ), this, SLOT( showNextChat() ) );
}

void GuiMain::setupChatConnections( GuiChat* gui_chat )
{
  connect( gui_chat, SIGNAL( newMessage( VNumber, const QString& ) ), this, SLOT( sendMessage( VNumber, const QString& ) ) );
  connect( gui_chat, SIGNAL( writing( VNumber ) ), mp_core, SLOT( sendWritingMessage( VNumber ) ) );
  connect( gui_chat, SIGNAL( nextChat() ), this, SLOT( showNextChat() ) );
  connect( gui_chat, SIGNAL( openUrl( const QUrl& ) ), this, SLOT( openUrl( const QUrl& ) ) );
  connect( gui_chat, SIGNAL( sendFileFromChatRequest( VNumber, const QString& ) ), this, SLOT( sendFileFromChat( VNumber, const QString& ) ) );
  connect( gui_chat, SIGNAL( editGroupRequestFromChat( VNumber ) ), this, SLOT( editGroupFromChat( VNumber ) ) );
  connect( gui_chat, SIGNAL( chatToClear( VNumber ) ), this, SLOT( clearChat( VNumber ) ) );
  connect( gui_chat, SIGNAL( leaveThisChat( VNumber ) ), this, SLOT( leaveGroupChat( VNumber ) ) );
  connect( gui_chat, SIGNAL( showChatMenuRequest() ), this, SLOT( showChatSettingsMenu() ) );
  connect( gui_chat, SIGNAL( showVCardRequest( VNumber, bool ) ), this, SLOT( showVCard( VNumber, bool ) ) );
  connect( gui_chat, SIGNAL( createGroupFromChatRequest( VNumber ) ), this, SLOT( createGroupFromChat( VNumber ) ) );
  connect( gui_chat, SIGNAL( detachChatRequest( VNumber ) ), this, SLOT( detachChat( VNumber ) ) );

}

void GuiMain::toggleVisibilityEmoticonPanel()
{
  if( mp_dockEmoticons->isVisible() )
    mp_dockEmoticons->hide();
  else
    mp_dockEmoticons->show();
}

void GuiMain::applyFlagStaysOnTop()
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

  if( windowFlags() & Qt::WindowStaysOnTopHint )
    qDebug() << "ON TOP 2";
}

void GuiMain::checkWindowFlagsAndShow()
{
  checkViewActions();

  applyFlagStaysOnTop();

  if( Settings::instance().showMinimizedAtStartup() )
    QTimer::singleShot( 100, this, SLOT( showMinimized() ) );

  QSplitter* chat_splitter = mp_chat->chatSplitter();
  if( Settings::instance().chatSplitterState().isEmpty() )
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
#ifdef BEEBEEP_DEBUG
    qDebug() << "Restoring chat splitter state";
#endif
    chat_splitter->restoreState( Settings::instance().chatSplitterState() );
  }
}

void GuiMain::refreshTitle( const User& )
{
  QString window_title;

  window_title = QString( "%1 - %2 (%3)" ).arg( Settings::instance().programName(),
                     Settings::instance().localUser().name(),
                     mp_core->isConnected() ?
                     Bee::userStatusToString( Settings::instance().localUser().status() ) : tr( "offline" ) );

  setWindowTitle( window_title );
}

void GuiMain::keyPressEvent( QKeyEvent* e )
{
  if( e->key() == Qt::Key_Escape )
  {
    if( Settings::instance().keyEscapeMinimizeInTray() )
      QTimer::singleShot( 0, this, SLOT( hideToTrayIcon() ) );
    else
      QTimer::singleShot( 0, this, SLOT( showMinimized() ) );
    e->accept();
    return;
  }

  QMainWindow::keyPressEvent( e );
}

void GuiMain::changeEvent( QEvent* e )
{
  QMainWindow::changeEvent( e );

  if( e->type() == QEvent::WindowStateChange )
    setGameInPauseMode();
}

void GuiMain::closeEvent( QCloseEvent* e )
{
  if( mp_core->isConnected() )
  {
    if( !m_forceShutdown )
    {
      if( Settings::instance().minimizeInTray() && QSystemTrayIcon::isSystemTrayAvailable() )
      {
        QTimer::singleShot( 0, this, SLOT( hideToTrayIcon() ) );
        e->ignore();
        return;
      }

      if( Settings::instance().promptOnCloseEvent() )
      {
        if( QMessageBox::question( this, Settings::instance().programName(), tr( "Do you really want to quit %1?" ).arg( Settings::instance().programName() ),
                               tr( "Yes" ), tr( "No" ), QString::null, 1, 1 ) == 1 )
        {
          e->ignore();
          return;
        }
      }
    }

    mp_core->stop();
  }

  saveGeometryAndState();
  raiseHomeView();

  QSettings* sets = Settings::instance().objectSettings();
  sets->deleteLater();

  if( !m_forceShutdown )
  {
    if( !sets->isWritable() )
    {
      if( QMessageBox::warning( this, Settings::instance().programName(),
                              QString( "%1<br />%2<br />%3<br />%4<br />%5" ).arg( tr( "<b>Settings can not be saved</b>. Path:" ) )
                                                                     .arg( sets->fileName() )
                                                                     .arg( tr( "<b>is not writable</b> by user:" ) )
                                                                     .arg( Settings::instance().localUser().accountName() )
                                                                     .arg( tr( "Do you want to close anyway?" ) ),
                              tr( "Yes" ), tr( "No" ), QString::null, 1, 1 ) == 1 )
      {
        e->ignore();
        return;
      }
    }

    if( Settings::instance().chatAutoSave() )
    {
      if( !SaveChatList::canBeSaved() )
      {
        if( QMessageBox::warning( this, Settings::instance().programName(),
                              QString( "%1<br />%2<br />%3<br />%4<br />%5" ).arg( tr( "<b>Chat messages can not be saved</b>. Path:" ) )
                                                                     .arg( Settings::instance().savedChatsFilePath() )
                                                                     .arg( tr( "<b>is not writable</b> by user:" ) )
                                                                     .arg( Settings::instance().localUser().accountName() )
                                                                     .arg( tr( "Do you want to close anyway?" ) ),
                              tr( "Yes" ), tr( "No" ), QString::null, 1, 1 ) == 1 )
        {
          e->ignore();
          return;
        }
      }
    }
  }

  foreach( GuiFloatingChat* fl_chat, m_floatingChats )
    fl_chat->close();

  mp_trayIcon->hide();

  if( mp_dockGroupList->isFloating() && mp_dockGroupList->isVisible() )
    mp_dockGroupList->hide();
  if( mp_dockSavedChatList->isFloating() && mp_dockSavedChatList->isVisible() )
    mp_dockSavedChatList->hide();
  if( mp_dockChatList->isFloating() && mp_dockChatList->isVisible() )
    mp_dockChatList->hide();
  if( mp_dockFileTransfers->isFloating() && mp_dockFileTransfers->isVisible() )
    mp_dockFileTransfers->hide();
  if( mp_dockEmoticons->isFloating() && mp_dockEmoticons->isVisible() )
    mp_dockEmoticons->hide();

  // maybe timer is active
  mp_logView->stopCheckingLog();

  // quit now on last window closed
  qApp->setQuitOnLastWindowClosed( true );

  e->accept();
}

void GuiMain::showNextChat()
{
#ifdef BEEBEEP_DEBUG
  qDebug() << "Show next chat in list with unread messages";
#endif
  Chat c = ChatManager::instance().firstChatWithUnreadMessages();
  if( c.isValid() )
    showChat( c.id() );
  else
    showMessage( tr( "No new message available" ), 0 );
}

void GuiMain::startStopCore()
{
  if( mp_core->isConnected() )
    stopCore();
  else
    startCore();
}

void GuiMain::forceShutdown()
{
  qDebug() << "Shutdown...";
  m_forceShutdown = true;
  if( mp_core->isConnected() )
    mp_core->stop();
  close();
}

void GuiMain::startCore()
{
  m_autoConnectOnInterfaceUp = false;

  if( Settings::instance().firstTime() )
  {
    Settings::instance().setFirstTime( false );
    if( Settings::instance().askNicknameAtStartup() )
    {
      showWizard();
      Settings::instance().setAskNicknameAtStartup( false );
    }

    /* Save geometry for the first time */
    /* If the user closes the application when it is not visible
     * there are problems in saving state and geometry */
    saveGeometryAndState();
  }
  else
  {
    if( Settings::instance().askNicknameAtStartup() )
      showWizard();
  }

  if( Settings::instance().askPassword() )
  {
    if( !promptConnectionPassword() )
      return;
  }

  mp_core->start();
  initGuiItems();
}

bool GuiMain::promptConnectionPassword()
{
  GuiAskPassword gap( this );
  gap.setModal( true );
  gap.loadData();
  gap.show();
  gap.setFixedSize( gap.size() );
  if( gap.exec() == QDialog::Rejected )
    return false;
  mp_actPromptPassword->setChecked( Settings::instance().askPasswordAtStartup() );
  return true;
}

void GuiMain::stopCore()
{
  mp_core->stop();
  initGuiItems();
}

void GuiMain::initGuiItems()
{
  bool enable = mp_core->isConnected();

  if( Settings::instance().showHomeAsDefaultPage() && !mp_barMain->isHidden() )
    raiseHomeView();

  if( enable )
  {
    mp_actStartStopCore->setIcon( QIcon( ":/images/disconnect.png") );
    mp_actStartStopCore->setText( tr( "Disconnect" ) );
    mp_actStartStopCore->setStatusTip( tr( "Disconnect from %1 network").arg( Settings::instance().programName() ) );
  }
  else
  {
    mp_actStartStopCore->setIcon( QIcon( ":/images/connect.png") );
    mp_actStartStopCore->setText( tr( "Connect" ) );
    mp_actStartStopCore->setStatusTip( tr( "Connect to %1 network").arg( Settings::instance().programName() ) );
  }

  mp_actBroadcast->setEnabled( enable );
  refreshUserList();

  updateStatusIcon();
  updateNewMessageAction();
  checkViewActions();

  refreshTitle( Settings::instance().localUser() );

  if( enable && mp_stackedWidget->currentWidget() == mp_chat && isActiveWindow() )
    mp_chat->ensureFocusInChat();
}

void GuiMain::checkViewActions()
{
  bool is_connected = mp_core->isConnected();
  int connected_users = mp_core->connectedUsers();

  mp_actViewHome->setEnabled( mp_stackedWidget->currentWidget() != mp_home );
  mp_actViewDefaultChat->setEnabled( mp_stackedWidget->currentWidget() != mp_chat );
  mp_actViewShareLocal->setEnabled( Settings::instance().fileTransferIsEnabled() && mp_stackedWidget->currentWidget() != mp_shareLocal );
  mp_actViewShareNetwork->setEnabled( Settings::instance().fileTransferIsEnabled() && mp_stackedWidget->currentWidget() != mp_shareNetwork && is_connected && connected_users > 0 );
  mp_actViewLog->setEnabled( mp_stackedWidget->currentWidget() != mp_logView );
  mp_actViewScreenShot->setEnabled( mp_stackedWidget->currentWidget() != mp_screenShot );
  mp_actViewShareBox->setEnabled( mp_stackedWidget->currentWidget() != mp_shareBox );

  mp_actCreateGroup->setEnabled( is_connected && UserManager::instance().userList().toList().size() >= 2 );
  mp_actCreateGroupChat->setEnabled( is_connected && connected_users > 1 );

  if( mp_stackedWidget->currentWidget() == mp_chat )
  {
    mp_chat->updateActions( is_connected, connected_users );
    checkChatToolbar();
    mp_dockEmoticons->setVisible( Settings::instance().showEmoticonMenu() );
  }
  else
  {
    mp_barChat->hide();
    mp_dockEmoticons->hide();
  }

  if( mp_stackedWidget->currentWidget() == mp_shareNetwork )
    mp_barShareNetwork->show();
  else
    mp_barShareNetwork->hide();

  if( mp_stackedWidget->currentWidget() == mp_shareLocal )
    mp_barShareLocal->show();
  else
    mp_barShareLocal->hide();

  if( mp_stackedWidget->currentWidget() == mp_logView )
  {
    mp_barLog->show();
    mp_logView->startCheckingLog();
  }
  else
  {
    mp_barLog->hide();
    mp_logView->stopCheckingLog();
  }

  if( mp_stackedWidget->currentWidget() == mp_screenShot )
    mp_barScreenShot->show();
  else
    mp_barScreenShot->hide();

  if( mp_stackedWidget->currentWidget() == mp_shareBox )
    mp_barShareBox->show();
  else
    mp_barShareBox->hide();

  showDefaultServerPortInMenu();

  if( !m_floatingChats.isEmpty() )
  {
    foreach( GuiFloatingChat* fl_chat, m_floatingChats )
      fl_chat->guiChat()->updateActions( is_connected, connected_users );
  }
}

void GuiMain::showAbout()
{
  QMessageBox::about( this, Settings::instance().programName(),
                      QString( "<b>%1</b> - %2<br /><br />%3 %4 %5 %6<br />%7 %8<br />%9<br />" )
                      .arg( Settings::instance().programName() )
                      .arg( tr( "Secure Lan Messenger" ) )
                      .arg( tr( "Version" ) )
                      .arg( Settings::instance().version( true, true ) )
                      .arg( tr( "for" ) )
                      .arg( Settings::instance().operatingSystem( true ) )
                      .arg( tr( "developed by" ) )
                      .arg( QString( "<a href='http://it.linkedin.com/pub/marco-mastroddi/20/5a7/191'>Marco Mastroddi</a>" ) )
                      .arg( QString( "e-mail: marco.mastroddi@gmail.com<br />web: <a href='http://www.beebeep.net'>www.beebeep.net</a>" ) )
                      );

}

void GuiMain::showLicense()
{
  QString license_txt = tr( "BeeBEEP is free software: you can redistribute it and/or modify<br />"
  "it under the terms of the GNU General Public License as published<br />"
  "by the Free Software Foundation, either version 3 of the License<br />"
  "or (at your option) any later version.<br /><br />"
  "BeeBEEP is distributed in the hope that it will be useful,<br />"
  "but WITHOUT ANY WARRANTY; without even the implied warranty<br />"
  "of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.<br />"
  "See the GNU General Public License for more details." );
  QMessageBox::about( this, Settings::instance().programName(), license_txt );
}

void GuiMain::createActions()
{
  mp_actStartStopCore = new QAction( this );
  connect( mp_actStartStopCore, SIGNAL( triggered() ), this, SLOT( startStopCore() ) );

  mp_actConfigureNetwork = new QAction( QIcon( ":/images/search-users.png"), tr( "Search for users..."), this );
  mp_actConfigureNetwork->setStatusTip( tr( "Configure %1 network to search a user who is not in your local subnet" ).arg( Settings::instance().programName() ) );
  connect( mp_actConfigureNetwork, SIGNAL( triggered() ), this, SLOT( searchUsers() ) );

  mp_actQuit = new QAction( QIcon( ":/images/quit.png" ), tr( "Quit" ), this );
  mp_actQuit->setShortcuts( QKeySequence::Quit );
  mp_actQuit->setStatusTip( tr( "Close the chat and quit %1" ).arg( Settings::instance().programName() ) );
  connect( mp_actQuit, SIGNAL( triggered() ), this, SLOT( forceShutdown() ) );

  mp_actVCard = new QAction( QIcon( ":/images/profile-edit.png"), tr( "Edit your profile..." ), this );
  mp_actVCard->setStatusTip( tr( "Change your profile information like your picture or your email or phone number" ) );
  connect( mp_actVCard, SIGNAL( triggered() ), this, SLOT( changeVCard() ) );

  mp_actToolBar = mp_barMain->toggleViewAction();
  mp_actToolBar->setStatusTip( tr( "Show the main tool bar with settings" ) );
  mp_actToolBar->setData( 99 );

  mp_actAbout = new QAction( QIcon( ":/images/beebeep.png" ), tr( "About %1..." ).arg( Settings::instance().programName() ), this );
  mp_actAbout->setStatusTip( tr( "Show the informations about %1" ).arg( Settings::instance().programName() ) );
  connect( mp_actAbout, SIGNAL( triggered() ), this, SLOT( showAbout() ) );

  mp_actCreateGroupChat = new QAction( QIcon( ":/images/chat-create.png" ), tr( "Create chat" ), this );
  mp_actCreateGroupChat->setStatusTip( tr( "Create a chat with two or more users" ) );
  connect( mp_actCreateGroupChat, SIGNAL( triggered() ), this, SLOT( createChat() ) );

  mp_actCreateGroup = new QAction(  QIcon( ":/images/group-add.png" ), tr( "Create group" ), this );
  mp_actCreateGroup->setStatusTip( tr( "Create a group with two or more users" ) );
  connect( mp_actCreateGroup, SIGNAL( triggered() ), this, SLOT( createGroup() ) );

}

void GuiMain::createMenus()
{
  QAction* act;

  /* Main Menu */
  mp_menuMain = new QMenu( tr( "Main" ), this );
  mp_menuMain->addAction( mp_actStartStopCore );
  mp_menuMain->addSeparator();
  mp_menuMain->addAction( mp_actVCard );
  mp_menuMain->addSeparator();

  mp_actBroadcast = mp_menuMain->addAction( QIcon( ":/images/broadcast.png" ), tr( "Broadcast to network" ), this, SLOT( sendBroadcastMessage() ) );
  mp_actBroadcast->setStatusTip( tr( "Broadcast a message in your network to find available users" ) );
  mp_menuMain->addAction( mp_actConfigureNetwork );

  act = mp_menuMain->addAction( QIcon( ":/images/user-add.png" ), tr( "Add users manually..."), this, SLOT( showAddUser() ) );
  act->setStatusTip( tr( "Add the IP address and the port of the users you want to connect" ) );
  mp_menuMain->addSeparator();

  act = mp_menuMain->addAction( QIcon( ":/images/language.png" ), tr( "Select language..."), this, SLOT( selectLanguage() ) );
  act->setStatusTip( tr( "Select your preferred language" ) );
  act = mp_menuMain->addAction( QIcon( ":/images/download-folder.png" ), tr( "Download folder..."), this, SLOT( selectDownloadDirectory() ) );
  act->setStatusTip( tr( "Select the download folder" ) );
  act = mp_menuMain->addAction( QIcon( ":/images/shortcut.png" ), tr( "Shortcuts..." ), this, SLOT( editShortcuts() ) );
  act->setStatusTip( tr( "Enable and edit your custom shortcuts" ) );
  act = mp_menuMain->addAction( QIcon( ":/images/dictionary.png" ), tr( "Dictionary..." ), this, SLOT( selectDictionatyPath() ) );
  act->setStatusTip( tr( "Select your preferred dictionary for spell checking" ) );
  mp_menuMain->addSeparator();

  act = mp_menuMain->addAction( QIcon( ":/images/file-beep.png" ), tr( "Select beep file..." ), this, SLOT( selectBeepFile() ) );
  act->setStatusTip( tr( "Select the file to play on new message arrived" ) );
  act = mp_menuMain->addAction( QIcon( ":/images/play.png" ), tr( "Play beep" ), this, SLOT( testBeepFile() ) );
  act->setStatusTip( tr( "Test the file to play on new message arrived" ) );
  mp_menuMain->addSeparator();
  if( Settings::instance().resourceFolder() != Settings::instance().dataFolder() )
  {
    act = mp_menuMain->addAction( QIcon( ":/images/resource-folder.png" ), tr( "Open your resource folder" ), this, SLOT( openResourceFolder() ) );
    act->setStatusTip( tr( "Click to open your resource folder" ) );
  }
  act = mp_menuMain->addAction( QIcon( ":/images/data-folder.png" ), tr( "Open your data folder" ), this, SLOT( openDataFolder() ) );
  act->setStatusTip( tr( "Click to open your data folder" ) );
  mp_menuMain->addSeparator();
  mp_menuMain->addAction( mp_actQuit );

  /* Chat Menu */
  mp_menuChat = new QMenu( tr( "Chat" ), this );

  act = mp_menuChat->addAction( tr( "Save messages" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled the messages are saved when the program is closed" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().chatAutoSave() );
  act->setData( 18 );

  mp_menuChat->addSeparator();

  act = mp_menuChat->addAction( tr( "Show colored nickname" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled the user's nickname in chat and in list is colored" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().showUserColor() );
  act->setData( 5 );

  act = mp_menuChat->addAction( tr( "Enable the compact mode in chat window" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled the sender's nickname and his message are in the same line" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().chatCompact() );
  act->setData( 1 );

  act = mp_menuChat->addAction( tr( "Add a blank line between the messages" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled the messages in the chat window are separated by a blank line" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().chatAddNewLineToMessage() );
  act->setData( 2 );

  act = mp_menuChat->addAction( tr( "Show the timestamp" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled the message shows its timestamp in the chat window" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().chatShowMessageTimestamp() );
  act->setData( 3 );

  act = mp_menuChat->addAction( tr( "Show the datestamp" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled the message shows its datestamp in the chat window" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().chatShowMessageDatestamp() );
  act->setData( 42 );

  act = mp_menuChat->addAction( tr( "Show preview of the images" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled the preview of the downloaded images will be showed in the chat window" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().showImagePreview() );
  act->setData( 33 );

  act = mp_menuChat->addAction( tr( "Parse Unicode and ASCII emoticons" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled the ASCII emoticons will be recognized and shown as images" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().showEmoticons() );
  act->setData( 10 );

  act = mp_menuChat->addAction( tr( "Use native emoticons" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled the emoticons will be parsed by your system font" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().useNativeEmoticons() );
  act->setData( 31 );

  act = mp_menuChat->addAction( tr( "Show messages grouped by user" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled the messages will be shown grouped by user" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().showMessagesGroupByUser() );
  act->setData( 13 );

  act = mp_menuChat->addAction( "", this, SLOT( settingsChanged() ) );
  setChatMessagesToShowInAction( act );
  act->setCheckable( true );
  act->setChecked( Settings::instance().chatMaxMessagesToShow() );
  act->setData( 27 );

  act = mp_menuChat->addAction( tr( "Use your name instead of 'You'" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().chatUseYourNameInsteadOfYou() );
  act->setData( 41 );

  mp_menuChat->addSeparator();

  act = mp_menuChat->addAction( tr( "Use HTML tags" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled HTML tags are not removed from the message" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().chatUseHtmlTags() );
  act->setData( 8 );

  act = mp_menuChat->addAction( tr( "Use clickable links" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled the links in the message are recognized and made clickable" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().chatUseClickableLinks() );
  act->setData( 9 );

  /* System Menu */
  mp_menuSettings = new QMenu( tr( "Settings" ), this );

  act = mp_menuSettings->addAction( tr( "Prompts for nickname on startup" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().askNicknameAtStartup() );
  act->setData( 45 );

  mp_actPromptPassword = mp_menuSettings->addAction( tr( "Prompts for network password on startup" ), this, SLOT( settingsChanged() ) );
  mp_actPromptPassword->setStatusTip( tr( "If enabled the password dialog will be shown on connection startup" ) );
  mp_actPromptPassword->setCheckable( true );
  mp_actPromptPassword->setChecked( Settings::instance().askPasswordAtStartup() );
  mp_actPromptPassword->setData( 17 );

  act = mp_menuSettings->addAction( tr( "Show activities home page at startup" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled the activities home page instead of chat page will be showed at startup" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().showHomeAsDefaultPage() );
  act->setData( 34 );

  act = mp_menuSettings->addAction( tr( "Show minimized at startup" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled %1 is showed minimized at startup" ).arg( Settings::instance().programName() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().showMinimizedAtStartup() );
  act->setData( 35 );

  act = mp_menuSettings->addAction( tr( "Reset window geometry at startup" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled the window geometry will be reset to default value at the next startup" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().resetGeometryAtStartup() );
  act->setData( 26 );

  act = mp_menuSettings->addAction( tr( "Check for new version at startup" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().checkNewVersionAtStartup() );
  act->setData( 43 );

#ifdef Q_OS_WIN
  act = mp_menuSettings->addAction( tr( "Load %1 on Windows startup" ).arg( Settings::instance().programName() ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled you can automatically load %1 at system startup" ).arg( Settings::instance().programName() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().hasStartOnSystemBoot() );
  act->setData( 16 );
#endif

  mp_menuSettings->addSeparator();

  act = mp_menuSettings->addAction( tr( "Enable file transfer" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled you can transfer files with the other users" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().fileTransferIsEnabled() );
  act->setData( 12 );
  act->setEnabled( !Settings::instance().disableFileTransfer() );

  QMenu* existing_file_menu = mp_menuSettings->addMenu( tr( "If a file already exists" ) + QString( "..." ) );
  mp_actGroupExistingFile = new QActionGroup( this );
  mp_actGroupExistingFile->setExclusive( true );
  mp_actOverwriteExistingFile = existing_file_menu->addAction( tr( "Overwrite" ), this, SLOT( settingsChanged() ) );
  mp_actOverwriteExistingFile->setStatusTip( tr( "If the file to be downloaded already exists it is automatically overwritten" ) );
  mp_actOverwriteExistingFile->setCheckable( true );
  mp_actOverwriteExistingFile->setChecked( Settings::instance().overwriteExistingFiles() );
  mp_actOverwriteExistingFile->setData( 99 );
  mp_actGenerateAutomaticFilename = existing_file_menu->addAction( tr( "Generate automatic filename" ), this, SLOT( settingsChanged() ) );
  mp_actGenerateAutomaticFilename->setStatusTip( tr( "If the file to be downloaded already exists a new filename is automatically generated" ) );
  mp_actGenerateAutomaticFilename->setCheckable( true );
  mp_actGenerateAutomaticFilename->setChecked( Settings::instance().automaticFileName() );
  mp_actGenerateAutomaticFilename->setData( 99 );
  mp_actAskToDoOnExistingFile = existing_file_menu->addAction( tr( "Ask me" ), this, SLOT( settingsChanged() ) );
  mp_actAskToDoOnExistingFile->setCheckable( true );
  mp_actAskToDoOnExistingFile->setChecked( !Settings::instance().automaticFileName() && !Settings::instance().overwriteExistingFiles() );
  mp_actAskToDoOnExistingFile->setData( 99 );
  mp_actGroupExistingFile->addAction( mp_actOverwriteExistingFile );
  mp_actGroupExistingFile->addAction( mp_actGenerateAutomaticFilename );
  mp_actGroupExistingFile->addAction( mp_actAskToDoOnExistingFile );
  connect( mp_actGroupExistingFile, SIGNAL( triggered( QAction* ) ), this, SLOT( onChangeSettingOnExistingFile( QAction* ) ) );

  mp_actConfirmDownload = mp_menuSettings->addAction( tr( "Prompt before downloading file" ), this, SLOT( settingsChanged() ) );
  mp_actConfirmDownload->setStatusTip( tr( "If enabled you have to confirm the action before downloading a file" ) );
  mp_actConfirmDownload->setCheckable( true );
  mp_actConfirmDownload->setChecked( Settings::instance().confirmOnDownloadFile() );
  mp_actConfirmDownload->setData( 30 );

  mp_menuSettings->addSeparator();

  act = mp_menuSettings->addAction( tr( "Set status to away automatically" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled %1 change your status to away after an idle of %2 minutes" ).arg( Settings::instance().programName() ).arg( Settings::instance().userAwayTimeout() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().autoUserAway() );
  act->setData( 20 );

  mp_menuSettings->addSeparator();

  act = mp_menuSettings->addAction( tr( "Always open a new floating chat window" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled when you always open chat in a new floating window" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().alwaysOpenNewFloatingChat() );
  act->setData( 37 );

  QMenu* beep_file_menu = mp_menuSettings->addMenu( tr( "Enable BEEP alert on new message" ) + QString( "..." ) );
  beep_file_menu->setStatusTip( tr( "If enabled when a new message is arrived a sound is emitted" ) );
  mp_actGroupBeepOnNewMessage = new QActionGroup( this );
  mp_actGroupBeepOnNewMessage->setExclusive( true );
  mp_actBeepOnNewMessage = beep_file_menu->addAction( tr( "When the chat is not visible" ), this, SLOT( settingsChanged() ) );
  mp_actBeepOnNewMessage->setCheckable( true );
  mp_actBeepOnNewMessage->setChecked( Settings::instance().beepOnNewMessageArrived() );
  mp_actBeepOnNewMessage->setData( 99 );
  mp_actAlwaysBeepOnNewMessage = beep_file_menu->addAction( tr( "Always" ), this, SLOT( settingsChanged() ) );
  mp_actAlwaysBeepOnNewMessage->setCheckable( true );
  mp_actAlwaysBeepOnNewMessage->setChecked( Settings::instance().beepAlwaysOnNewMessageArrived() );
  mp_actAlwaysBeepOnNewMessage->setData( 99 );
  mp_actNeverBeepOnNewMessage = beep_file_menu->addAction( tr( "Never" ), this, SLOT( settingsChanged() ) );
  mp_actNeverBeepOnNewMessage->setCheckable( true );
  mp_actNeverBeepOnNewMessage->setChecked( !Settings::instance().beepAlwaysOnNewMessageArrived() && !Settings::instance().beepOnNewMessageArrived() );
  mp_actNeverBeepOnNewMessage->setData( 99 );
  mp_actGroupBeepOnNewMessage->addAction( mp_actBeepOnNewMessage );
  mp_actGroupBeepOnNewMessage->addAction( mp_actAlwaysBeepOnNewMessage );
  mp_actGroupBeepOnNewMessage->addAction( mp_actNeverBeepOnNewMessage );
  connect( mp_actGroupBeepOnNewMessage, SIGNAL( triggered( QAction* ) ), this, SLOT( onChangeSettingBeepOnNewMessage( QAction* ) ) );

  act = mp_menuSettings->addAction( tr( "Raise on top on new message" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled when a new message is arrived %1 is shown on top of all other windows" ).arg( Settings::instance().programName() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().raiseOnNewMessageArrived() );
  act->setData( 15 );

  act = mp_menuSettings->addAction( tr( "Always stay on top" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled %1 stays on top of the other windows" ).arg( Settings::instance().programName() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().stayOnTop() );
  act->setData( 14 );

  mp_menuSettings->addSeparator();

  act = mp_menuSettings->addAction( tr( "Prompt on quit (only when connected)" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled you will be asked if you really want to close %1" ).arg( Settings::instance().programName() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().promptOnCloseEvent() );
  act->setData( 36 );


  /* User List Menu */
  mp_menuUserList = new QMenu( tr( "Options" ), this );

  act = mp_menuUserList->addAction( tr( "Save the users on exit" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled the user list will be save on exit" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().saveUserList() );
  act->setData( 32 );

  mp_menuUserList->addSeparator();

  act = mp_menuUserList->addAction( tr( "Show only the online users" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled only the online users are shown in the list" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().showOnlyOnlineUsers() );
  act->setData( 6 );

  act = mp_menuUserList->addAction( tr( "Show the user's picture" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled you can see a picture of the users in the list (if they have)" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().showUserPhoto() );
  act->setData( 21 );

  act = mp_menuUserList->addAction( tr( "Show the user's vCard on right click" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled you can see the user's vCard when right click on it" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().showVCardOnRightClick() );
  act->setData( 25 );

  act = mp_menuUserList->addAction( tr( "Show status color in background" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled the user in list has colored backrgound as status" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().showUserStatusBackgroundColor() );
  act->setData( 38 );

  mp_menuUserList->addSeparator();

  act = mp_menuUserList->addAction( tr( "Change size of the user's picture" ), this, SLOT( changeAvatarSizeInList() ) );
  act->setStatusTip( tr( "Click to change the picture size of the users in the list" ) );

  mp_userList->setMenuSettings( mp_menuUserList );

  /* Status Menu */
  mp_menuStatus = new QMenu( tr( "Status" ), this );
  mp_menuStatus->setStatusTip( tr( "Select your status" ) );
  mp_menuStatus->setIcon( QIcon( ":/images/user-status.png" ) );
  for( int i = User::Online; i < User::NumStatus; i++ )
  {
    act = mp_menuStatus->addAction( QIcon( Bee::menuUserStatusIconFileName( i ) ), Bee::userStatusToString( i ), this, SLOT( statusSelected() ) );
    act->setData( i );
    act->setStatusTip( tr( "Your status will be %1" ).arg( Bee::userStatusToString( i ) ) );
    act->setIconVisibleInMenu( true );
  }

  mp_menuStatus->addSeparator();

  mp_menuUserStatusList = new QMenu( tr( "Recently used" ), this );
  act = mp_menuStatus->addMenu( mp_menuUserStatusList );
  act->setIcon( QIcon( ":/images/recent.png" ) );
  loadUserStatusRecentlyUsed();

  act = mp_menuStatus->addAction( QIcon( ":/images/user-status.png" ), tr( "Change your status description..." ), this, SLOT( changeStatusDescription() ) );
  act = mp_menuStatus->addAction( QIcon( ":/images/clear.png" ), tr( "Clear all status descriptions" ), this, SLOT( clearRecentlyUsedUserStatus() ) );
  mp_menuStatus->addSeparator();
  act = mp_menuStatus->addAction( QIcon( Bee::menuUserStatusIconFileName( User::Offline ) ), Bee::userStatusToString( User::Offline ), this, SLOT( statusSelected() ) );
  act->setData( User::Offline );
  act->setStatusTip( tr( "Your status will be %1" ).arg( Bee::userStatusToString( User::Offline ) ) );
  act->setIconVisibleInMenu( true );

  act = mp_menuStatus->menuAction();
  connect( act, SIGNAL( triggered() ), this, SLOT( showLocalUserVCard() ) );

  /* View Menu */
  mp_menuView = new QMenu( tr( "View" ), this );
  mp_menuView->addAction( QIcon( ":/images/save-window.png" ), tr( "Save main window geometry" ), this, SLOT( saveGeometryAndState() ) );
  mp_menuView->addSeparator();
  mp_menuView->addAction( mp_actToolBar );
  mp_menuView->addSeparator();
  mp_menuView->addAction( mp_actViewUsers );
  mp_menuView->addAction( mp_actViewGroups );
  mp_menuView->addAction( mp_actViewChats );
  mp_menuView->addAction( mp_actViewSavedChats );
  mp_menuView->addAction( mp_actViewFileTransfer );
  mp_menuView->addSeparator();
  mp_actViewHome = mp_menuView->addAction( QIcon( ":/images/home.png" ), tr( "Show %1 home" ).arg( Settings::instance().programName() ), this, SLOT( raiseHomeView() ) );
  mp_actViewHome->setStatusTip( tr( "Show the homepage with %1 activity" ).arg( Settings::instance().programName() ) );
  mp_actViewDefaultChat = mp_menuView->addAction( QIcon( ":/images/chat-view.png" ), tr( "Show the chat" ), this, SLOT( showCurrentChat() ) );
  mp_actViewDefaultChat->setStatusTip( tr( "Show the chat view" ) );
  mp_actViewShareLocal = mp_menuView->addAction( QIcon( ":/images/upload.png" ), tr( "Show my shared files" ), this, SLOT( raiseLocalShareView() ) );
  mp_actViewShareLocal->setStatusTip( tr( "Show the list of the files which I have shared" ) );
  mp_actViewShareNetwork = mp_menuView->addAction( QIcon( ":/images/download.png" ), tr( "Show the network shared files" ), this, SLOT( raiseNetworkShareView() ) );
  mp_actViewShareNetwork->setStatusTip( tr( "Show the list of the network shared files" ) );
  mp_actViewShareBox = mp_menuView->addAction( QIcon( ":/images/sharebox.png" ), tr( "Show the shared boxes" ), this, SLOT( raiseShareBoxView() ) );
  mp_actViewLog = mp_menuView->addAction( QIcon( ":/images/log.png" ), tr( "Show the %1 log" ).arg( Settings::instance().programName() ), this, SLOT( raiseLogView() ) );
  mp_actViewLog->setStatusTip( tr( "Show the application log to see if an error occurred" ) );
  mp_actViewScreenShot = mp_menuView->addAction( QIcon( ":/images/screenshot.png" ), tr( "Make a screenshot" ), this, SLOT( raiseScreenShotView() ) );
  mp_actViewScreenShot->setStatusTip( tr( "Show the utility to capture a screenshot" ) );
  mp_actViewNewMessage = mp_menuView->addAction( QIcon( ":/images/beebeep-message.png" ), tr( "Show new message" ), this, SLOT( showNextChat() ) );

  /* Plugins Menu */
  mp_menuPlugins = new QMenu( tr( "Plugins" ), this );

  /* Help Menu */
  mp_menuInfo = new QMenu( tr("?" ), this );
  act = mp_menuInfo->addAction( QIcon( ":/images/tip.png" ), tr( "Tip of the day" ), this, SLOT( showTipOfTheDay() ) );
  act->setStatusTip( tr( "Show me the tip of the day" ) );
  act = mp_menuInfo->addAction( QIcon( ":/images/fact.png" ), tr( "Fact of the day" ), this, SLOT( showFactOfTheDay() ) );
  act->setStatusTip( tr( "Show me the fact of the day" ) );
  mp_menuInfo->addSeparator();
  mp_menuInfo->addAction( mp_actAbout );
  act = mp_menuInfo->addAction( QIcon( ":/images/license.png" ), tr( "Show %1's license..." ).arg( Settings::instance().programName() ), this, SLOT( showLicense() ) );
  act->setStatusTip( tr( "Show the informations about %1's license" ).arg( Settings::instance().programName() ) );
  act = mp_menuInfo->addAction( QIcon( ":/images/qt.png" ), tr( "Qt Library..." ), qApp, SLOT( aboutQt() ) );
  act->setStatusTip( tr( "Show the informations about Qt library" ) );
  mp_menuInfo->addSeparator();
  act = mp_menuInfo->addAction( QIcon( ":/images/beebeep.png" ), tr( "Open %1 official website..." ).arg( Settings::instance().programName() ), this, SLOT( openWebSite() ) );
  act->setStatusTip( tr( "Explore %1 official website" ).arg( Settings::instance().programName() ) );
  act = mp_menuInfo->addAction( QIcon( ":/images/update.png" ), tr( "Check for new version..." ), this, SLOT( checkNewVersion() ) );
  act->setStatusTip( tr( "Open %1 website and check if a new version exists" ).arg( Settings::instance().programName() ) );
  act = mp_menuInfo->addAction( QIcon( ":/images/plugin.png" ), tr( "Download plugins..." ), this, SLOT( openDownloadPluginPage() ) );
  act->setStatusTip( tr( "Open %1 website and download your preferred plugin" ).arg( Settings::instance().programName() ) );
  act = mp_menuInfo->addAction( QIcon( ":/images/info.png" ), tr( "Help online..." ), this, SLOT( openHelpPage() ) );
  act->setStatusTip( tr( "Open %1 website to have online support" ).arg( Settings::instance().programName() ) );
  mp_menuInfo->addSeparator();
  act = mp_menuInfo->addAction( QIcon( ":/images/thumbup.png" ), tr( "Like %1 on Facebook" ).arg( Settings::instance().programName() ), this, SLOT( openFacebookPage() ) );
#ifdef BEEBEEP_DEBUG
  act = mp_menuInfo->addAction( tr( "Add +1 user to anonymous usage statistics" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "Help me to know how many users have BeeBEEP" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().postUsageStatistics() );
  act->setData( 44 );
#endif
  act = mp_menuInfo->addAction( QIcon( ":/images/donate.png" ), tr( "Donate for %1" ).arg( Settings::instance().programName() ), this, SLOT( openDonationPage() ) );
  act->setStatusTip( tr( "I'm so grateful and pleased about that" ) );

  /* Tray icon menu */
  mp_menuTrayIcon = new QMenu( this );
  act = mp_menuTrayIcon->addAction( QIcon( ":/images/beebeep.png" ), tr( "Show" ), this, SLOT( showUp() ) );
  mp_menuTrayIcon->setDefaultAction( act );

  mp_menuTrayIcon->addSeparator();
  mp_menuTrayIcon->addAction( mp_menuStatus->menuAction() );
  mp_menuTrayIcon->addSeparator();

  mp_menuNetworkStatus = mp_menuTrayIcon->addMenu( QIcon( ":/images/connect.png" ), tr( "Network" ) );
  mp_actHostAddress = mp_menuNetworkStatus->addAction( QString( "ip" ) );
  mp_actPortBroadcast = mp_menuNetworkStatus->addAction( QString( "udp1" ) );
  mp_actPortListener = mp_menuNetworkStatus->addAction( QString( "tcp1" ) );
  mp_actPortFileTransfer = mp_menuNetworkStatus->addAction( QString( "tcp2" ) );
#ifdef BEEBEEP_USE_MULTICAST_DNS
  mp_actMulticastDns = mp_menuNetworkStatus->addAction( QString( "mdns" ) );
#endif
  mp_menuTrayIcon->addSeparator();

  act = mp_menuTrayIcon->addAction( tr( "Load on system tray at startup" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled %1 will be start hided in system tray" ).arg( Settings::instance().programName() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().loadOnTrayAtStartup() );
  act->setData( 24 );

  act = mp_menuTrayIcon->addAction( tr( "Close button minimize to tray icon" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled when the close button is clicked the window will be minimized to the system tray icon" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().minimizeInTray() );
  act->setData( 11 );

  act = mp_menuTrayIcon->addAction( tr( "Escape key minimize to tray icon" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled when the escape button is clicked the window will be minimized to the system tray icon" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().keyEscapeMinimizeInTray() );
  act->setData( 29 );

  act = mp_menuTrayIcon->addAction( tr( "Enable tray icon notification" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled tray icon shows some notification about status and message" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().showNotificationOnTray()  );
  act->setData( 19 );

  act = mp_menuTrayIcon->addAction( tr( "Show only message notifications" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled tray icon shows only message notifications" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().showOnlyMessageNotificationOnTray()  );
  act->setData( 40 );

  act = mp_menuTrayIcon->addAction( tr( "Show chat message preview" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().showChatMessageOnTray() );
  act->setData( 46 );

  mp_menuTrayIcon->addSeparator();
  mp_menuTrayIcon->addAction( mp_actQuit );
  mp_trayIcon->setContextMenu( mp_menuTrayIcon );

}

void GuiMain::createToolAndMenuBars()
{
  menuBar()->addMenu( mp_menuMain );
  menuBar()->addMenu( mp_menuSettings );
  menuBar()->addMenu( mp_menuView );
  menuBar()->addMenu( mp_menuPlugins );
  menuBar()->addMenu( mp_menuInfo );

  QLabel *label_version = new QLabel( this );
  label_version->setTextFormat( Qt::RichText );
  label_version->setAlignment( Qt::AlignCenter );
  QString label_version_text = QString( "&nbsp;<b>v %1</b> %2&nbsp;" )
#ifdef BEEBEEP_DEBUG
                            .arg( Settings::instance().version( true, true ) )
#else
                            .arg( Settings::instance().version( true, false ) )
#endif
                            .arg( Bee::iconToHtml( Settings::instance().operatingSystemIconPath(), "*", 12, 12 ) );
  label_version->setText( label_version_text );
  menuBar()->setCornerWidget( label_version );

  mp_barMain->addAction( mp_actBroadcast );
  mp_barMain->addAction( mp_menuStatus->menuAction() );
  mp_barMain->addSeparator();
  mp_barMain->addAction( mp_actViewNewMessage );
  mp_barMain->addAction( mp_actCreateGroupChat );
  mp_barMain->addAction( mp_actCreateGroup );
  mp_barMain->addSeparator();
  mp_barMain->addAction( mp_actViewUsers );
  mp_barMain->addAction( mp_actViewGroups );
  mp_barMain->addAction( mp_actViewChats );
  mp_barMain->addAction( mp_actViewSavedChats );
  mp_barMain->addAction( mp_actViewFileTransfer );
  mp_barMain->addSeparator();
  mp_barMain->addAction( mp_actViewHome );
  mp_barMain->addAction( mp_actViewDefaultChat );
  mp_barMain->addAction( mp_actViewShareLocal );
  mp_barMain->addAction( mp_actViewShareNetwork );
  mp_barMain->addAction( mp_actViewShareBox );
  mp_barMain->addAction( mp_actViewLog );
  mp_barMain->addAction( mp_actViewScreenShot );

  mp_barMain->setVisible( !Settings::instance().hideMainToolbar() );
}

void GuiMain::createDockWindows()
{
  mp_dockUserList = new QDockWidget( tr( "Users" ), this );
  mp_dockUserList->setObjectName( "GuiUserListDock" );
  mp_userList = new GuiUserList( mp_dockUserList );
  mp_dockUserList->setWidget( mp_userList );
  mp_dockUserList->setAllowedAreas( Qt::AllDockWidgetAreas );
  addDockWidget( Qt::RightDockWidgetArea, mp_dockUserList );
  mp_actViewUsers = mp_dockUserList->toggleViewAction();
  mp_actViewUsers->setIcon( QIcon( ":/images/user-list.png" ) );
  mp_actViewUsers->setText( tr( "Show the user panel" ) );
  mp_actViewUsers->setStatusTip( tr( "Show the list of the connected users" ) );
  mp_actViewUsers->setData( 99 );

  mp_dockGroupList = new QDockWidget( tr( "Groups" ), this );
  mp_dockGroupList->setObjectName( "GuiGroupListDock" );
  mp_groupList = new GuiGroupList( mp_dockGroupList );
  mp_dockGroupList->setWidget( mp_groupList );
  mp_dockGroupList->setAllowedAreas( Qt::AllDockWidgetAreas );
  addDockWidget( Qt::RightDockWidgetArea, mp_dockGroupList );
  mp_actViewGroups = mp_dockGroupList->toggleViewAction();
  mp_actViewGroups->setIcon( QIcon( ":/images/group.png" ) );
  mp_actViewGroups->setText( tr( "Show the group panel" ) );
  mp_actViewGroups->setStatusTip( tr( "Show the list of your groups" ) );
  mp_actViewGroups->setData( 99 );

  mp_dockChatList = new QDockWidget( tr( "Chats" ), this );
  mp_dockChatList->setObjectName( "GuiChatListDock" );
  mp_chatList = new GuiChatList( this );
  mp_dockChatList->setWidget( mp_chatList );
  mp_dockChatList->setAllowedAreas( Qt::AllDockWidgetAreas );
  addDockWidget( Qt::RightDockWidgetArea, mp_dockChatList );
  mp_actViewChats = mp_dockChatList->toggleViewAction();
  mp_actViewChats->setIcon( QIcon( ":/images/chat-list.png" ) );
  mp_actViewChats->setText( tr( "Show the chat panel" ) );
  mp_actViewChats->setStatusTip( tr( "Show the list of the chats" ) );
  mp_actViewChats->setData( 99 );

  mp_dockSavedChatList = new QDockWidget( tr( "History" ), this );
  mp_dockSavedChatList->setObjectName( "GuiSavedChatListDock" );
  mp_savedChatList = new GuiSavedChatList( this );
  mp_dockSavedChatList->setWidget( mp_savedChatList );
  mp_dockSavedChatList->setAllowedAreas( Qt::AllDockWidgetAreas );
  addDockWidget( Qt::RightDockWidgetArea, mp_dockSavedChatList );
  mp_actViewSavedChats = mp_dockSavedChatList->toggleViewAction();
  mp_actViewSavedChats->setIcon( QIcon( ":/images/saved-chat-list.png" ) );
  mp_actViewSavedChats->setText( tr( "Show the history panel" ) );
  mp_actViewSavedChats->setStatusTip( tr( "Show the list of the saved chats" ) );
  mp_actViewSavedChats->setData( 99 );

  mp_dockFileTransfers = new QDockWidget( tr( "File Transfers" ), this );
  mp_dockFileTransfers->setObjectName( "GuiFileTransferDock" );
  mp_fileTransfer = new GuiTransferFile( this );
  mp_dockFileTransfers->setWidget( mp_fileTransfer );
  mp_dockFileTransfers->setAllowedAreas( Qt::AllDockWidgetAreas );
  addDockWidget( Qt::BottomDockWidgetArea, mp_dockFileTransfers );
  mp_actViewFileTransfer = mp_dockFileTransfers->toggleViewAction();
  mp_actViewFileTransfer->setIcon( QIcon( ":/images/file-transfer.png" ) );
  mp_actViewFileTransfer->setText( tr( "Show the file transfer panel" ) );
  mp_actViewFileTransfer->setStatusTip( tr( "Show the list of the file transfers" ) );
  mp_actViewFileTransfer->setData( 99 );

  mp_dockEmoticons = new QDockWidget( tr( "Emoticons" ), this );
  mp_dockEmoticons->setObjectName( "GuiDockEmoticons" );

  mp_emoticonsWidget = new GuiEmoticons( this );
  updateEmoticons();
  connect( mp_emoticonsWidget, SIGNAL( emoticonSelected( const Emoticon& ) ), mp_chat, SLOT( addEmoticon( const Emoticon& ) ) );
  mp_dockEmoticons->setWidget( mp_emoticonsWidget );

  mp_dockEmoticons->setAllowedAreas( Qt::AllDockWidgetAreas );
  addDockWidget( Qt::LeftDockWidgetArea, mp_dockEmoticons );
  QAction* mp_actViewEmoticons = mp_dockEmoticons->toggleViewAction();
  mp_actViewEmoticons->setIcon( QIcon( ":/images/emoticon.png" ) );
  mp_actViewEmoticons->setText( tr( "Show the emoticon panel" ) );
  mp_actViewEmoticons->setStatusTip( tr( "Add your preferred emoticon to the message" ) );
  mp_actViewEmoticons->setData( 28 );
  connect( mp_dockEmoticons, SIGNAL( visibilityChanged( bool ) ), this, SLOT( emoticonMenuVisibilityChanged( bool ) ) );
  connect( mp_actViewEmoticons, SIGNAL( triggered() ), this, SLOT( settingsChanged() ) );
  mp_barChat->insertAction( mp_barChat->actions().first(), mp_actViewEmoticons );

  if( Settings::instance().firstTime() || Settings::instance().resetGeometryAtStartup() || Settings::instance().hideOtherPanels() )
  {
    mp_dockGroupList->hide();
    mp_dockSavedChatList->hide();
    mp_dockFileTransfers->hide();
    mp_dockEmoticons->hide();
    if( Settings::instance().hideOtherPanels() )
      mp_dockChatList->hide();
  }

  mp_dockUserList->setVisible( !Settings::instance().hideUsersPanel() );
}

void GuiMain::emoticonMenuVisibilityChanged( bool is_visible )
{
  if( mp_stackedWidget->currentWidget() == mp_chat )
  {
    if( !is_visible )
      Settings::instance().setShowEmoticonMenu( false );
  }
}

void GuiMain::createStackedWidgets()
{
  mp_chat = new GuiChat( this );
  mp_stackedWidget->addWidget( mp_chat );
  mp_barChat = new QToolBar( tr( "Show the bar of chat" ), this );
  addToolBar( Qt::BottomToolBarArea, mp_barChat );
  mp_barChat->setObjectName( "GuiChatToolBar" );
  mp_barChat->setIconSize( Settings::instance().mainBarIconSize() );
  mp_barChat->setAllowedAreas( Qt::AllToolBarAreas );
  mp_chat->setupToolBar( mp_barChat );
  QAction* act = mp_barChat->toggleViewAction();
  act->setData( 23 );
  mp_chat->setChatId( ID_DEFAULT_CHAT, false );
  connect( act, SIGNAL( triggered() ), this, SLOT( settingsChanged() ) );

  mp_shareLocal = new GuiShareLocal( this );
  mp_stackedWidget->addWidget( mp_shareLocal );
  mp_barShareLocal= new QToolBar( tr( "Show the bar of local file sharing" ), this );
  addToolBar( Qt::BottomToolBarArea, mp_barShareLocal );
  mp_barShareLocal->setObjectName( "GuiShareLocalToolBar" );
  mp_barShareLocal->setIconSize( Settings::instance().mainBarIconSize() );
  mp_barShareLocal->setAllowedAreas( Qt::BottomToolBarArea | Qt::TopToolBarArea );
  mp_shareLocal->setupToolBar( mp_barShareLocal );
  act = mp_barShareLocal->toggleViewAction();
  act->setEnabled( false );

  mp_shareNetwork = new GuiShareNetwork( this );
  mp_stackedWidget->addWidget( mp_shareNetwork );
  mp_barShareNetwork = new QToolBar( tr( "Show the bar of network file sharing" ), this );
  addToolBar( Qt::BottomToolBarArea, mp_barShareNetwork );
  mp_barShareNetwork->setObjectName( "GuiShareNetworkToolBar" );
  mp_barShareNetwork->setIconSize( Settings::instance().mainBarIconSize() );
  mp_barShareNetwork->setAllowedAreas( Qt::BottomToolBarArea | Qt::TopToolBarArea );
  mp_shareNetwork->setupToolBar( mp_barShareNetwork );
  act = mp_barShareNetwork->toggleViewAction();
  act->setEnabled( false );

  mp_logView = new GuiLog( this );
  mp_stackedWidget->addWidget( mp_logView );
  mp_barLog = new QToolBar( tr( "Show the bar of log" ), this );
  addToolBar( Qt::BottomToolBarArea, mp_barLog );
  mp_barLog->setObjectName( "GuiLogToolBar" );
  mp_barLog->setIconSize( Settings::instance().mainBarIconSize() );
  mp_barLog->setAllowedAreas( Qt::BottomToolBarArea | Qt::TopToolBarArea );
  mp_logView->setupToolBar( mp_barLog );
  act = mp_barLog->toggleViewAction();
  act->setEnabled( false );

  mp_savedChat = new GuiSavedChat( this );
  mp_stackedWidget->addWidget( mp_savedChat );

  mp_screenShot = new GuiScreenShot( this );
  mp_stackedWidget->addWidget( mp_screenShot );
  mp_barScreenShot = new QToolBar( tr( "Show the bar of screenshot plugin" ), this );
  addToolBar( Qt::BottomToolBarArea, mp_barScreenShot );
  mp_barScreenShot->setObjectName( "GuiScreenShotToolBar" );
  mp_barScreenShot->setIconSize( Settings::instance().mainBarIconSize() );
  mp_barScreenShot->setAllowedAreas( Qt::BottomToolBarArea | Qt::TopToolBarArea );
  mp_screenShot->setupToolBar( mp_barScreenShot );
  act = mp_barScreenShot->toggleViewAction();
  act->setEnabled( false );

  mp_home = new GuiHome( this );
  mp_stackedWidget->addWidget( mp_home );

  mp_shareBox = new GuiShareBox( this );
  mp_stackedWidget->addWidget( mp_shareBox );
  mp_barShareBox = new QToolBar( tr( "Show the bar of share box" ), this );
  addToolBar( Qt::BottomToolBarArea, mp_barShareBox );
  mp_barShareBox->setObjectName( "GuiShareBoxToolBar" );
  mp_barShareBox->setIconSize( Settings::instance().mainBarIconSize() );
  mp_barShareBox->setAllowedAreas( Qt::BottomToolBarArea | Qt::TopToolBarArea );
  mp_shareBox->setupToolBar( mp_barShareBox );
  act = mp_barShareBox->toggleViewAction();
  act->setEnabled( false );

  mp_barChat->setVisible( Settings::instance().showChatToolbar() );

  mp_stackedWidget->setCurrentWidget( mp_chat );
}

QMenu* GuiMain::gameMenu( GameInterface* gi )
{
  if( m_mapGameMenu.contains( gi->name() ) )
    return m_mapGameMenu.value( gi->name(), mp_defaultGameMenu );

  QMenu *menu_game = new QMenu( gi->name(), this );
  menu_game->setIcon( gi->icon() );

  QAction* act = menu_game->addAction( QIcon( ":/images/play.png" ), tr( "Play %1" ).arg( gi->name() ), this, SLOT( raisePluginView() ) );
  act->setData( mp_stackedWidget->addWidget( gi->mainWindow() ) );
  menu_game->setDefaultAction( act );

  QString help_data_ts = tr( "is a game developed by" );
  QString help_data_format = QString( "<p>%1 <b>%2</b> %3 <b>%4</b>.<br /><i>%5</i></p><br />" );

  act = menu_game->addAction( QIcon( ":/images/info.png" ), tr( "About %1" ).arg( gi->name() ), this, SLOT( showPluginHelp() ) );
  act->setData( help_data_format
                .arg( Bee::iconToHtml( (gi->icon().isNull() ? ":/images/plugin.png" : gi->iconFileName()), "*P*" ),
                      gi->name(), help_data_ts, gi->author(), gi->help() ) );

  m_mapGameMenu.insert( gi->name(), menu_game );

  return menu_game;
}

void GuiMain::createPluginWindows()
{
  if( PluginManager::instance().games().size() <= 0 )
    return;

  foreach( GameInterface* gi, PluginManager::instance().games() )
  {
    gameMenu( gi );
  }
}

void GuiMain::updateUser( const User& u )
{
  if( !u.isValid() )
  {
    qWarning() << "Invalid user found in GuiMain::checkUser( const User& u )";
    return;
  }

#ifdef BEEBEEP_DEBUG
  qDebug() << "User" << u.path() << "has updated his info";
#endif

  mp_userList->setUser( u, true );
  mp_groupList->updateUser( u );
  mp_chat->updateUser( u );
  foreach( GuiFloatingChat* fl_chat, m_floatingChats )
    fl_chat->updateUser( u );
  checkViewActions();
}

void GuiMain::refreshUserList()
{
  mp_userList->updateUsers( mp_core->isConnected() );
}

void GuiMain::settingsChanged()
{
  QAction* act = qobject_cast<QAction*>( sender() );
  if( !act )
    return;

  bool refresh_users = false;
  bool refresh_chat = false;
  int settings_data_id = act->data().toInt();
  bool ok = false;

#ifdef BEEBEEP_DEBUG
  qDebug() << "Settings changed for action id" << settings_data_id << "to" << (bool)(act->isChecked());
#endif

  switch( settings_data_id )
  {
  case 1:
    Settings::instance().setChatCompact( act->isChecked() );
    refresh_chat = true;
    break;
  case 2:
    Settings::instance().setChatAddNewLineToMessage( act->isChecked() );
    refresh_chat = true;
    break;
  case 3:
    Settings::instance().setChatShowMessageTimestamp( act->isChecked() );
    refresh_chat = true;
    break;
  case 4:
    // free
    break;
  case 5:
    Settings::instance().setShowUserColor( act->isChecked() );
    refresh_users = true;
    refresh_chat = true;
    break;
  case 6:
    Settings::instance().setShowOnlyOnlineUsers( act->isChecked() );
    refresh_users = true;
    refresh_chat = true;
  case 7:
    // free
    break;
  case 8:
    Settings::instance().setChatUseHtmlTags( act->isChecked() );
    refresh_chat = true;
    break;
  case 9:
    Settings::instance().setChatUseClickableLinks( act->isChecked() );
    refresh_chat = true;
    break;
  case 10:
    Settings::instance().setShowEmoticons( act->isChecked() );
    refresh_chat = true;
    break;
  case 11:
    Settings::instance().setMinimizeInTray( act->isChecked() );
    break;
  case 12:
    {
      Settings::instance().setFileTransferIsEnabled( act->isChecked() );
      if( act->isChecked() )
        mp_core->startFileTransferServer();
      else
        mp_core->stopFileTransferServer();
      checkViewActions();
    }
    break;
  case 13:
    Settings::instance().setShowMessagesGroupByUser( act->isChecked() );
    refresh_chat = true;
    break;
  case 14:
    Settings::instance().setStayOnTop( act->isChecked() );
    applyFlagStaysOnTop();
    break;
  case 15:
    Settings::instance().setRaiseOnNewMessageArrived( act->isChecked() );
    break;
  case 16:
    checkAutoStartOnBoot( act->isChecked() );
    break;
  case 17:
    {
      if( !act->isChecked() )
      {
        Settings::instance().setAskPasswordAtStartup( false );
        if( Settings::instance().askPassword() )
        {
          QMessageBox::information( this, Settings::instance().programName(), tr( "Please save the network password in the next dialog." ) );
          promptConnectionPassword();
          return;
        }
      }
      else
        Settings::instance().setAskPasswordAtStartup( true );
    }
    break;
  case 18:
    Settings::instance().setChatAutoSave( act->isChecked() );
    break;
  case 19:
    Settings::instance().setShowNotificationOnTray( act->isChecked() );
    break;
  case 20:
    {
      Settings::instance().setAutoUserAway( act->isChecked() );
      if( act->isChecked() )
      {
        BeeApplication* bee_app = (BeeApplication*)qApp;
        int away_timeout = QInputDialog::getInt( this, Settings::instance().programName(),
                              tr( "How many minutes of idle %1 can wait before changing status to away?" ).arg( Settings::instance().programName() ),
                              Settings::instance().userAwayTimeout(), 1, 30, 1, &ok );
        if( ok && away_timeout > 0 )
          Settings::instance().setUserAwayTimeout( away_timeout );

        bee_app->setIdleTimeout( Settings::instance().userAwayTimeout() );
      }
    }
    break;
  case 21:
    Settings::instance().setShowUserPhoto( act->isChecked() );
    refresh_users = true;
    break;
  case 23:
    Settings::instance().setShowChatToolbar( act->isChecked() );
    checkChatToolbar();
    break;
  case 24:
    Settings::instance().setLoadOnTrayAtStartup( act->isChecked() );
    break;
  case 25:
    Settings::instance().setShowVCardOnRightClick( act->isChecked() );
    break;
  case 26:
    Settings::instance().setResetGeometryAtStartup( act->isChecked() );
    break;
  case 27:
    {
      Settings::instance().setChatMaxMessagesToShow( act->isChecked() );
      if( act->isChecked() )
      {
#if QT_VERSION >= 0x050000
        int num_messages = QInputDialog::getInt( this, Settings::instance().programName(),
#else
        int num_messages = QInputDialog::getInteger( this, Settings::instance().programName(),
#endif
                                                     tr( "Please select the maximum number of messages to be showed" ),
                                                     Settings::instance().chatMessagesToShow(),
                                                     10, 1000, 5, &ok );
        if( ok )
        {
          Settings::instance().setChatMessagesToShow( num_messages );
          setChatMessagesToShowInAction( act );
        }
      }
      refresh_chat = true;
    }
    break;
  case 28:
    Settings::instance().setShowEmoticonMenu( act->isChecked() );
    break;
  case 29:
    Settings::instance().setKeyEscapeMinimizeInTray( act->isChecked() );
    break;
  case 30:
    Settings::instance().setConfirmOnDownloadFile( act->isChecked() );
    break;
  case 31:
    Settings::instance().setUseNativeEmoticons( act->isChecked() );
    updateEmoticons();
    refresh_chat = true;
  case 32:
    Settings::instance().setSaveUserList( act->isChecked() );
    break;
  case 33:
    Settings::instance().setShowImagePreview( act->isChecked() );
    break;
  case 34:
    Settings::instance().setShowHomeAsDefaultPage( act->isChecked() );
    break;
  case 35:
    Settings::instance().setShowMinimizedAtStartup( act->isChecked() );
    break;
  case 36:
    Settings::instance().setPromptOnCloseEvent( act->isChecked() );
    break;
  case 37:
    Settings::instance().setAlwaysOpenNewFloatingChat( act->isChecked() );
    break;
  case 38:
    Settings::instance().setShowUserStatusBackgroundColor( act->isChecked() );
    refresh_users = true;
    break;
  case 39:
    Settings::instance().setUseShortcuts( act->isChecked() );
    break;
  case 40:
    Settings::instance().setShowOnlyMessageNotificationOnTray( act->isChecked() );
    break;
  case 41:
    Settings::instance().setChatUseYourNameInsteadOfYou( act->isChecked() );
    refresh_chat = true;
    break;
  case 42:
    Settings::instance().setChatShowMessageDatestamp( act->isChecked() );
    refresh_chat = true;
    break;
  case 43:
    Settings::instance().setCheckNewVersionAtStartup( act->isChecked() );
    break;
  case 44:
    Settings::instance().setPostUsageStatistics( act->isChecked() );
    break;
  case 45:
    Settings::instance().setAskNicknameAtStartup( act->isChecked() );
    break;
  case 46:
    Settings::instance().setShowChatMessageOnTray( act->isChecked() );
    break;
  case 99:
    break;
  default:
    qWarning() << "GuiMain::settingsChanged(): error in setting id" << act->data().toInt();
  }

  if( refresh_users )
    refreshUserList();
  if( refresh_chat )
  {
    QApplication::setOverrideCursor( Qt::WaitCursor );
    QApplication::processEvents();
    mp_chat->reloadChat();
    foreach( GuiFloatingChat* fl_chat, m_floatingChats )
      fl_chat->guiChat()->reloadChat();
    QApplication::restoreOverrideCursor();
  }
  if( settings_data_id > 0 && settings_data_id < 99 )
    Settings::instance().save();
}

void GuiMain::setChatMessagesToShowInAction( QAction* act )
{
  act->setText( tr( "Show only last %1 messages" ).arg( Settings::instance().chatMessagesToShow() ) );
  act->setStatusTip( tr( "If enabled only the last %1 messages will be shown in chat" ).arg( Settings::instance().chatMessagesToShow() ) );
}

void GuiMain::sendMessage( VNumber chat_id, const QString& msg )
{
#ifdef BEEBEEP_DEBUG
  int num_messages = mp_core->sendChatMessage( chat_id, msg );
  qDebug() << num_messages << "messages sent";
#else
  mp_core->sendChatMessage( chat_id, msg );
#endif
}

void GuiMain::showAlertForMessage( VNumber chat_id, const ChatMessage& cm )
{
  Chat c = ChatManager::instance().chat( chat_id );
  if( c.isValid() && c.isGroup() && Settings::instance().isNotificationDisabledForGroup( c.privateId() ) )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Notification disabled for group:" << c.privateId() << c.name();
#endif
    return;
  }

  if( Settings::instance().beepOnNewMessageArrived() || Settings::instance().beepAlwaysOnNewMessageArrived() )
    playBeep();

  bool show_message_in_tray = true;

  GuiFloatingChat* fl_chat = floatingChat( chat_id );

  if( !fl_chat && Settings::instance().alwaysOpenNewFloatingChat() && Settings::instance().raiseOnNewMessageArrived() )
  {
    detachChat( chat_id );
    fl_chat = floatingChat( chat_id );
  }

  if( fl_chat )
  {
    if( Settings::instance().raiseOnNewMessageArrived() )
    {
      fl_chat->raiseOnTop();
      show_message_in_tray = false;
    }
    else
    {
      fl_chat->setMainIcon( true );
      QApplication::alert( fl_chat );
    }
  }
  else
  {
    if( Settings::instance().raiseOnNewMessageArrived() )
    {
      showChat( chat_id );
      raiseOnTop();
      show_message_in_tray = false;
    }
    else
      QApplication::alert( this );
  }

  if( show_message_in_tray )
  {
    User u = UserManager::instance().findUser( cm.userId() );
    QString msg;
    bool long_time_show = false;

    if( u.isValid() )
    {
      if( Settings::instance().showChatMessageOnTray() )
      {
        QString txt = Bee::removeHtmlTags( cm.message() );
        if( txt.size() > Settings::instance().textSizeInChatMessagePreviewOnTray() )
        {
          txt.truncate( Settings::instance().textSizeInChatMessagePreviewOnTray() );
          txt.append( "..." );
        }
        msg = QString( "%1: %2" ).arg( u.name() ).arg( txt );
        long_time_show = true;
      }
      else
        msg = tr( "New message from %1" ).arg( u.name() );
    }
    else
      msg = tr( "New message arrived" );

    mp_trayIcon->showNewMessageArrived( chat_id, msg, long_time_show );
  }
  else
    mp_trayIcon->setUnreadMessages( chat_id, 0 );

  updateMainIcon();
}

bool GuiMain::chatIsVisible( VNumber chat_id )
{
  GuiFloatingChat* fl_chat = floatingChat( chat_id );
  if( fl_chat )
    return fl_chat->chatIsVisible();

  if( !isVisible() )
    return false;

  if( isMinimized() )
    return false;

  if( !isActiveWindow() )
    return false;

  return mp_stackedWidget->currentWidget() == mp_chat && mp_chat->chatId() == chat_id;
}

void GuiMain::showChatMessage( VNumber chat_id, const ChatMessage& cm )
{
  if( chat_id == ID_DEFAULT_CHAT && cm.isFromSystem() )
    mp_home->addSystemMessage( cm );

  bool chat_is_visible = chatIsVisible( chat_id );

  if( !cm.isFromSystem() && !cm.isFromLocalUser() )
  {
    if( !chat_is_visible )
      showAlertForMessage( chat_id, cm );
    else if( Settings::instance().beepAlwaysOnNewMessageArrived() )
      playBeep();
  }

  chat_is_visible = chatIsVisible( chat_id );

  GuiChat* gui_chat = guiChat( chat_id );

  if( gui_chat )
  {
    if( chat_is_visible )
      readAllMessagesInChat( chat_id );
    gui_chat->appendChatMessage( chat_id, cm );
  }
  else
  {
    chat_is_visible = false;
  }

  if( !chat_is_visible )
  {
    Chat chat_hidden = ChatManager::instance().chat( chat_id );
    mp_userList->setUnreadMessages( chat_id, chat_hidden.unreadMessages() );
    int chat_messages = chat_hidden.chatMessages() + ChatManager::instance().savedChatSize( chat_hidden.name() );
    mp_userList->setMessages( chat_id, chat_messages );
    mp_chatList->updateChat( chat_id );
    mp_groupList->updateChat( chat_id );
  }

  updateNewMessageAction();
}

void GuiMain::searchUsers()
{
  GuiSearchUser gsu( this );
  gsu.setModal( true );
  gsu.loadSettings();
  gsu.setSizeGripEnabled( true );
  gsu.show();

  if( gsu.exec() != QDialog::Accepted )
    return;

  if( !mp_core->isConnected() )
    return;

  raiseHomeView();

#ifdef BEEBEEP_USE_MULTICAST_DNS
  if( Settings::instance().useMulticastDns() )
    mp_core->startDnsMulticasting();
  else
    mp_core->stopDnsMulticasting();
#endif

  if( Settings::instance().acceptConnectionsOnlyFromWorkgroups() && !Settings::instance().workgroups().isEmpty() )
    qDebug() << "Protocol now accepts connections only from these workgroups:" << qPrintable( Settings::instance().workgroups().join( ", " ) );

  mp_core->checkBroadcastInterval();

  QTimer::singleShot( 0, mp_core, SLOT( sendBroadcastMessage() ) );
}

void GuiMain::showWritingUser( const User& u )
{
  QString msg = tr( "%1 is writing..." ).arg( u.name() );
  if( mp_stackedWidget->currentWidget() == mp_chat && mp_chat->hasUser( u.id() ) )
    showMessage( msg, Settings::instance().writingTimeout() );

  foreach( GuiFloatingChat* fl_chat, m_floatingChats )
    fl_chat->showUserWriting( u.id(), msg );
}

void GuiMain::setUserStatusSelected( int user_status )
{
  if( user_status == User::Offline && mp_core->isConnected() )
  {
    if( QMessageBox::question( this, Settings::instance().programName(),
                               tr( "Do you want to disconnect from %1 network?" ).arg( Settings::instance().programName() ),
                               QMessageBox::Yes, QMessageBox::No ) == QMessageBox::Yes )
      stopCore();
    return;
  }

  mp_core->setLocalUserStatus( user_status );

  if( !mp_core->isConnected() )
    startCore();
  else
    updateStatusIcon();
}

void GuiMain::statusSelected()
{
  QAction* act = qobject_cast<QAction*>( sender() );
  if( !act )
    return;

  int user_status = act->data().toInt();
  setUserStatusSelected( user_status );
}

void GuiMain::updateStatusIcon()
{
  int status_type;
  if( !mp_core->isConnected() )
    status_type = User::Offline;
  else
    status_type = Settings::instance().localUser().status();

  mp_menuStatus->setIcon( QIcon( Bee::menuUserStatusIconFileName( status_type ) ) );
  QString tip = tr( "You are %1%2" ).arg( Bee::userStatusToString( status_type ) )
      .arg( (Settings::instance().localUser().statusDescription().isEmpty() ? QString( "" ) : QString( ": %1" ).arg( Settings::instance().localUser().statusDescription() ) ) );
  QAction* act = mp_menuStatus->menuAction();
  act->setToolTip( tip );
  act->setText( Bee::capitalizeFirstLetter( Bee::userStatusToString( status_type ), true ) );
}

void GuiMain::changeStatusDescription()
{
  bool ok = false;
  QString status_description = QInputDialog::getText( this, Settings::instance().programName(),
                           tr( "Please insert the new status description" ), QLineEdit::Normal, Settings::instance().localUser().statusDescription(), &ok );
  if( !ok || status_description.isNull() )
    return;
  mp_core->setLocalUserStatusDescription( Settings::instance().localUser().status(), status_description, true );
  loadUserStatusRecentlyUsed();
  updateStatusIcon();
}

void GuiMain::sendFileFromChat( VNumber chat_id, const QString& file_path )
{
  Chat c = ChatManager::instance().chat( chat_id );
  if( !c.isValid() )
    return;

  if( c.isDefault() && !Settings::instance().chatWithAllUsersIsEnabled() )
    return;

  QStringList files_path_selected = checkFilePath( file_path );
  if( files_path_selected.isEmpty() )
    return;

  UserList chat_members = UserManager::instance().userList().fromUsersId( c.usersId() );
  foreach( User u, chat_members.toList() )
    sendFiles( u, files_path_selected );
}

void GuiMain::sendFile( VNumber user_id )
{
  User u = UserManager::instance().findUser( user_id );
  QStringList files_path_selected = checkFilePath( "" );
  if( files_path_selected.isEmpty() )
    return;
  sendFiles( u, files_path_selected );
}

void GuiMain::sendFiles( const User& u, const QStringList& file_list )
{
  foreach( QString file_path, file_list )
  {
    if( !sendFile( u, file_path ) )
      return;
  }
}

QStringList GuiMain::checkFilePath( const QString& file_path )
{
  QStringList files_path_selected;
  if( file_path.isEmpty() || !QFile::exists( file_path ) )
  {
    files_path_selected = FileDialog::getOpenFileNames( true, activeChatWindow(), tr( "%1 - Select a file" ).arg( Settings::instance().programName() ) + QString( " %1" ).arg( tr( "or more" ) ),
                                                       Settings::instance().lastDirectorySelected() );
    if( files_path_selected.isEmpty() )
      return files_path_selected;

    Settings::instance().setLastDirectorySelectedFromFile( files_path_selected.last() );
  }
  else
  {
    files_path_selected.append( file_path );
  }

  return files_path_selected;
}

bool GuiMain::sendFile( const User& u, const QString& file_path )
{
  if( !Settings::instance().fileTransferIsEnabled() )
  {
    QMessageBox::information( this, Settings::instance().programName(), tr( "File transfer is not enabled." ) );
    return false;
  }

  if( !mp_core->isConnected() )
  {
    QMessageBox::information( this, Settings::instance().programName(), tr( "You are not connected." ) );
    return false;
  }

  User user_selected;

  if( !u.isValid() )
  {
    QStringList user_string_list = UserManager::instance().userList().toStringList( false, true );
    if( user_string_list.isEmpty() )
    {
      QMessageBox::information( activeChatWindow(), Settings::instance().programName(), tr( "There is no user connected." ) );
      return false;
    }

    bool ok = false;
    QString user_path = QInputDialog::getItem( activeChatWindow(), Settings::instance().programName(),
                                        tr( "Please select the user to whom you would like to send a file."),
                                        user_string_list, 0, false, &ok );
    if( !ok )
      return false;

    user_selected = UserManager::instance().findUserByPath( user_path );

    if( !user_selected.isValid() )
    {
      QMessageBox::warning( activeChatWindow(), Settings::instance().programName(), tr( "User not found." ) );
      return false;
    }

    Chat c = ChatManager::instance().privateChatForUser( user_selected.id() );
    if( c.isValid() )
      showChat( c.id() );
  }
  else
    user_selected = u;

  return mp_core->sendFile( user_selected.id(), file_path );
}

void GuiMain::sendFile( const QString& file_path )
{
  sendFile( User(), file_path );
}

bool GuiMain::askToDownloadFile( const User& u, const FileInfo& fi, const QString& download_path, bool make_questions )
{
  if( !Settings::instance().fileTransferIsEnabled() )
  {
    QMessageBox::warning( this, Settings::instance().programName(), tr( "File transfer is disabled. You cannot download %1." ).arg( fi.name() ) );
    return false;
  }

  int msg_result = make_questions ? 0 : 1;
  bool auto_file_name = Settings::instance().automaticFileName();
  if( !make_questions )
    auto_file_name = true;

  if( msg_result == 0 )
  {
    if( Settings::instance().confirmOnDownloadFile() )
    {
      if( isMinimized() || !isActiveWindow() )
      {
        if( Settings::instance().raiseOnNewMessageArrived() )
          raiseOnTop();
        else
          QApplication::alert( this );
        raiseHomeView();
      }
      QString msg = tr( "Do you want to download %1 (%2) from %3?" ).arg( fi.name(), Bee::bytesToString( fi.size() ), u.name() );
      msg_result = QMessageBox::question( this, Settings::instance().programName(), msg, tr( "No" ), tr( "Yes" ), tr( "Yes, and don't ask anymore" ), 0, 0 );
    }
    else
      msg_result = 1;
  }

  if( msg_result == 2 )
  {
    qDebug() << "Prompt on download file disabled by user request";
    Settings::instance().setConfirmOnDownloadFile( false );
    mp_actConfirmDownload->setChecked( false );
  }

  if( msg_result > 0 )
  {
    // Accepted
    qDebug() << "You accept to download" << fi.name() << "from" << u.path();

    QFileInfo qfile_info( download_path, fi.name() );
    if( qfile_info.exists() )
    {
      if( !Settings::instance().overwriteExistingFiles() )
      {
        QString file_name;
        if( auto_file_name )
        {
          file_name = Bee::uniqueFilePath( qfile_info.absoluteFilePath() );
          qDebug() << "File" << qfile_info.absoluteFilePath() << "exists. Save with" << file_name;
        }
        else
        {
          file_name = FileDialog::getSaveFileName( this,
                            tr( "%1 already exists. Please select a new filename." ).arg( qfile_info.fileName() ),
                            qfile_info.absoluteFilePath() );
          if( file_name.isNull() || file_name.isEmpty() )
            return false;
        }
        qfile_info = QFileInfo( file_name );
      }
    }
    FileInfo file_info = fi;
    file_info.setName( qfile_info.fileName() );
    file_info.setPath( qfile_info.absoluteFilePath() );
    file_info.setSuffix( qfile_info.suffix() );
    return mp_core->downloadFile( u.id(), file_info, make_questions );
  }
  else
  {
    qDebug() << "You refuse to download" << fi.name() << "from" << u.path();
    return false;
  }
}

void GuiMain::downloadFile( const User& u, const FileInfo& fi )
{
  if( !askToDownloadFile( u, fi, Settings::instance().downloadDirectory(), true ) )
    mp_core->refuseToDownloadFile( u.id(), fi );
}

void GuiMain::downloadSharedFiles( const QList<SharedFileInfo>& share_file_info_list )
{
  if( share_file_info_list.isEmpty() )
    return;

  QString download_folder;
  User u;
  int files_to_download = 0;

  if( share_file_info_list.size() > Settings::instance().maxQueuedDownloads() )
  {
    if( QMessageBox::question( this, Settings::instance().programName(),
                               tr( "You cannot download all these files at once. Do you want to download the first %1 files of the list?" )
                               .arg( Settings::instance().maxQueuedDownloads() ),
                               tr( "Yes" ), tr( "No" ), QString::null, 1, 1 ) != 0 )
      return;
  }
  else if( share_file_info_list.size() > 100 )
  {
    if( QMessageBox::question( this, Settings::instance().programName(),
                           tr( "Downloading %1 files is a hard duty. Maybe you have to wait a lot of minutes. Do yo want to continue?" ).arg( share_file_info_list.size() ),
                           tr( "Yes" ), tr( "No" ), QString::null, 1, 1 ) != 0 )
      return;
  }

  foreach( SharedFileInfo sfi, share_file_info_list )
  {
    download_folder = Bee::convertToNativeFolderSeparator( QString( "%1/%2" ).arg( Settings::instance().downloadDirectory(), sfi.second.shareFolder() ) );
    u = UserManager::instance().findUser( sfi.first );
    if( !askToDownloadFile( u, sfi.second, download_folder, false ) )
      return;

    files_to_download++;

    if( files_to_download > Settings::instance().maxQueuedDownloads() )
      break;
  }

  showMessage( tr( "%1 files are scheduled for download" ).arg( files_to_download ), 5 );
}

void GuiMain::downloadSharedFile( VNumber user_id, VNumber file_id )
{
  User u = UserManager::instance().findUser( user_id );
  FileInfo file_info = FileShare::instance().networkFileInfo( user_id, file_id );

  if( u.isStatusConnected() && file_info.isValid() )
  {
    askToDownloadFile( u, file_info, Settings::instance().downloadDirectory(), true );
    return;
  }

  qWarning() << "Unable to download shared file" << file_id << "from user" << user_id;
  QString info_msg = tr( "File is not available for download." );
  if( u.isValid() && !u.isStatusConnected() )
    info_msg += QLatin1String( "\n" ) + tr( "%1 is not connected." ).arg( u.name() );
  info_msg += QLatin1String( "\n" ) + tr( "Please reload the list of shared files." );

  if( QMessageBox::information( this, Settings::instance().programName(), info_msg,
                              tr( "Reload file list" ), tr( "Cancel" ), QString::null, 1, 1 ) == 0 )
  {
    mp_shareNetwork->reloadList();
  }
}

void GuiMain::downloadFolder( const User& u, const QString& folder_name, const QList<FileInfo>& file_info_list )
{
  if( !Settings::instance().fileTransferIsEnabled() )
  {
    QMessageBox::warning( this, Settings::instance().programName(), tr( "File transfer is disabled. You cannot download %1." ).arg( folder_name ) );
    return;
  }

  int msg_result = Settings::instance().confirmOnDownloadFile() ? 0 : 1;

  if( msg_result == 0 )
  {
    QString msg = tr( "Do you want to download folder %1 (%2 files) from %3?" ).arg( folder_name ).arg( file_info_list.size() ).arg( u.name() );
    msg_result = QMessageBox::question( this, Settings::instance().programName(), msg, tr( "No" ), tr( "Yes" ), tr( "Yes, and don't ask anymore" ), 0, 0 );
  }

  if( msg_result == 2 )
  {
    Settings::instance().setConfirmOnDownloadFile( false );
    mp_actConfirmDownload->setChecked( false );
  }

  if( msg_result > 0 )
  {
    // Accepted
    qDebug() << "You accept to download folder" << folder_name << "from" << u.path();
    QString download_folder;
    int files_to_download = 0;
    foreach( FileInfo fi, file_info_list )
    {
      download_folder = Bee::convertToNativeFolderSeparator( QString( "%1/%2" ).arg( Settings::instance().downloadDirectory(), fi.shareFolder() ) );
      if( !askToDownloadFile( u, fi, download_folder, false ) )
        return;

      files_to_download++;

      if( files_to_download > Settings::instance().maxQueuedDownloads() )
      {
        qWarning() << "Unable to download all the" << file_info_list.size() << "files because max queued reached" << Settings::instance().maxQueuedDownloads();
        break;
      }
    }
  }
  else
  {
    qDebug() << "You refuse to download folder" << folder_name << "from" << u.path();
    mp_core->refuseToDownloadFolder( u.id(), folder_name );
  }
}

void GuiMain::selectDownloadDirectory()
{
  QString download_directory_path = FileDialog::getExistingDirectory( this,
                                                                       tr( "%1 - Select the download folder" )
                                                                       .arg( Settings::instance().programName() ),
                                                                       Settings::instance().downloadDirectory() );
  if( download_directory_path.isEmpty() )
    return;

  Settings::instance().setDownloadDirectory( download_directory_path );
}

void GuiMain::showTipOfTheDay()
{
  raiseHomeView();
  mp_core->showTipOfTheDay();
}

void GuiMain::showFactOfTheDay()
{
  raiseHomeView();
  mp_core->showFactOfTheDay();
}

void GuiMain::showCurrentChat()
{
  if( mp_chat->chatId() != ID_INVALID )
    showChat( mp_chat->chatId() );
  else
    showChat( ID_DEFAULT_CHAT );
}

void GuiMain::showDefaultChat()
{
  showChat( ID_DEFAULT_CHAT );
}

void GuiMain::showChat( VNumber chat_id )
{
  if( chat_id == ID_INVALID )
  {
#ifdef BEEBEEP_DEBUG
    qWarning() << "Unable to show invalid chat";
#endif
    return;
  }

  GuiFloatingChat* fl_chat = floatingChat( chat_id );
  if( fl_chat )
  {
    if( !fl_chat->isVisible() )
      checkWindowFlagsAndShow();
    else
      fl_chat->raiseOnTop();

    readAllMessagesInChat( chat_id );
    fl_chat->guiChat()->updateActions( mp_core->isConnected(), mp_core->connectedUsers() );
    fl_chat->guiChat()->reloadChatUsers();
    fl_chat->guiChat()->ensureFocusInChat();
    return;
  }

  if( mp_stackedWidget->currentWidget() == mp_chat && mp_chat->chatId() == chat_id )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Chat" << chat_id << "is already shown... skip";
#endif
    mp_chat->reloadChatUsers();
    readAllMessagesInChat( chat_id );
    mp_chat->updateActions( mp_core->isConnected(), mp_core->connectedUsers() );
    if( !isActiveWindow() )
      raiseOnTop();
    mp_chat->ensureFocusInChat();
    return;
  }

  if( chat_id != ID_DEFAULT_CHAT && Settings::instance().alwaysOpenNewFloatingChat() )
  {
    qApp->setActiveWindow( this ); // to get focus back on floating chat
    detachChat( chat_id );
    return;
  }

  if( mp_chat->setChatId( chat_id, false ) )
  {
    raiseChatView();
    mp_userList->setChatOpened( chat_id );
    readAllMessagesInChat( chat_id );
  }
  else
    raiseHomeView();

  if( !isActiveWindow() )
    raiseOnTop();
}

void GuiMain::changeVCard()
{
  GuiEditVCard gvc( this );
  gvc.setModal( true );
  gvc.setUser( Settings::instance().localUser() );
  gvc.setSizeGripEnabled( true );
  gvc.show();

  if( gvc.exec() == QDialog::Accepted )
  {
    if( mp_core->setLocalUserVCard( gvc.userColor(), gvc.vCard() ) )
      refreshTitle( Settings::instance().localUser() );
  }
}

void GuiMain::showLocalUserVCard()
{
  showVCard( Settings::instance().localUser(), false );
}

void GuiMain::showVCard( VNumber user_id, bool ensure_visible )
{
  User u = UserManager::instance().findUser( user_id );
  if( !u.isValid() )
    return;

  showVCard( u, ensure_visible );
}

void GuiMain::showVCard( const User& u, bool ensure_visible )
{
  GuiVCard* gvc = new GuiVCard( this );
  connect( gvc, SIGNAL( showChat( VNumber ) ), this, SLOT( showChat( VNumber ) ) );
  connect( gvc, SIGNAL( sendFile( VNumber ) ), this, SLOT( sendFile( VNumber ) ) );
  connect( gvc, SIGNAL( changeUserColor( VNumber ) ), this, SLOT( changeUserColor( VNumber) ) );
  connect( gvc, SIGNAL( toggleFavorite( VNumber ) ), this, SLOT( toggleUserFavorite( VNumber ) ) );
  connect( gvc, SIGNAL( removeUser( VNumber ) ), this, SLOT( removeUserFromList( VNumber ) ) );
  gvc->setVCard( u, ChatManager::instance().privateChatForUser( u.id() ).id(), mp_core->isConnected() );

  if( ensure_visible && dockWidgetArea( mp_dockUserList ) == Qt::RightDockWidgetArea )
  {
    // Ensure vCard visible
    QPoint pos = QCursor::pos();
    pos.setX( pos.x() - gvc->size().width() );
    gvc->move( pos );
  }
  else
  {
    gvc->move( QCursor::pos() );
  }

  gvc->show();
  gvc->setFixedSize( gvc->size() );
}

void GuiMain::updadePluginMenu()
{
  mp_menuPlugins->clear();
  QAction* act;

  act = mp_menuPlugins->addAction( QIcon( ":/images/plugin.png" ), tr( "Plugin Manager..." ), this, SLOT( showPluginManager() ) );
  act->setStatusTip( tr( "Open the plugin manager dialog and manage the installed plugins" ) );

  QString help_data_ts = tr( "is a plugin developed by" );
  QString help_data_format = QString( "<p>%1 <b>%2</b> %3 <b>%4</b>.<br /><i>%5</i></p><br />" );

  if( PluginManager::instance().textMarkers().size() > 0 )
  {
    mp_menuPlugins->addSeparator();

    foreach( TextMarkerInterface* text_marker, PluginManager::instance().textMarkers() )
    {
      act = mp_menuPlugins->addAction( text_marker->name(), this, SLOT( showPluginHelp() ) );

      act->setData( help_data_format
                  .arg( Bee::iconToHtml( (text_marker->icon().isNull() ? ":/images/plugin.png" : text_marker->iconFileName()), "*P*" ),
                        text_marker->name(), help_data_ts, text_marker->author(), text_marker->help() ) );
      act->setIcon( text_marker->icon() );
      act->setEnabled( text_marker->isEnabled() );
    }
  }

  if( PluginManager::instance().games().size() > 0 )
  {
    mp_menuPlugins->addSeparator();

    if( !mp_barGames )
    {
      mp_barGames = addToolBar( tr( "Show the bar of games" ) );
      mp_barGames->setObjectName( "GuiGamesToolBar" );
      mp_barGames->setIconSize( Settings::instance().mainBarIconSize() );
    }
    else
      mp_barGames->clear();

    QMenu* game_menu;
    int game_widget_id;

    foreach( GameInterface* gi, PluginManager::instance().games() )
    {
      game_menu = gameMenu( gi );
      game_menu->setEnabled( gi->isEnabled() );
      gi->mainWindow()->setEnabled( gi->isEnabled() );
      mp_menuPlugins->addMenu( game_menu );

      act = mp_barGames->addAction( gi->icon(), tr( "Play %1" ).arg( gi->name() ), this, SLOT( raisePluginView() ) );
      game_widget_id = mp_stackedWidget->indexOf( gi->mainWindow() ); // ensured by gameMenu function
      act->setData( game_widget_id );
    }
  }
}

void GuiMain::showPluginHelp()
{
  QAction* act = qobject_cast<QAction*>(sender());
  if( !act )
    return;

  QMessageBox::information( this, act->text(), act->data().toString() );
}

void GuiMain::showPluginManager()
{
  GuiPluginManager gpm( this );
  gpm.setModal( true );
  gpm.setSizeGripEnabled( true );
  gpm.updatePlugins();
  gpm.show();
  gpm.exec();
  if( gpm.isChanged() )
    updadePluginMenu();
}

void GuiMain::showWizard()
{
  GuiWizard gw( this );
  gw.setModal( true );
  gw.loadSettings();
  gw.show();
  gw.setFixedSize( gw.size() );
  if( gw.exec() == QDialog::Accepted )
    refreshTitle( Settings::instance().localUser() );
}

void GuiMain::hideToTrayIcon()
{
  Chat c = ChatManager::instance().firstChatWithUnreadMessages();
  if( c.isValid() )
    mp_trayIcon->setUnreadMessages( c.id(), c.unreadMessages() );
  else
    mp_trayIcon->setUnreadMessages( mp_chat->chatId(), 0 );
  hide();
}

void GuiMain::trayIconClicked( QSystemTrayIcon::ActivationReason ar )
{
#ifdef Q_OS_MAC

  // In Mac that is the expected behavior, there is no distinction
  // between left and right buttons for the systray icons.
  // They will always show the context menu, that's the Mac behavior.
  Q_UNUSED( ar );

#else

  // Other OS

  if( ar == QSystemTrayIcon::Context )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "TrayIcon is activated with context click and menu is showed";
#endif
    return;
  }

  if( ar == QSystemTrayIcon::Trigger )
  {
    if( mp_menuTrayIcon->isVisible() )
    {
#ifdef BEEBEEP_DEBUG
      qDebug() << "TrayIcon is activated with trigger click and menu will be hided";
#endif
      mp_menuTrayIcon->hide();
      return;
    }

    if( !isActiveWindow() || isMinimized() )
    {
#ifdef BEEBEEP_DEBUG
      qDebug() << "TrayIcon is activated with trigger click and main window will be showed";
#endif
      trayMessageClicked();
    }
    else
    {
#ifdef BEEBEEP_DEBUG
      qDebug() << "TrayIcon is activated with trigger click and menu will be showed";
#endif
      mp_menuTrayIcon->popup( QCursor::pos() );
    }
  }
  else
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "TrayIcon is activated with unknown click";
#endif
  }
#endif
}

void GuiMain::trayMessageClicked()
{
#ifdef BEEBEEP_DEBUG
  qDebug() << "TrayIcon message clicked";
#endif
  GuiFloatingChat* fl_chat = floatingChat( mp_trayIcon->chatId() );
  if( fl_chat )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Found a floating chat and show it";
#endif
    QTimer::singleShot( 0, fl_chat, SLOT( showUp() ) );
  }
  else
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Creating chat and show it";
#endif
    showChat( mp_trayIcon->chatId() );
  }
}

void GuiMain::addToShare( const QString& share_path )
{
  mp_core->addPathToShare( share_path, true );
}

void GuiMain::removeFromShare( const QString& share_path )
{
  mp_core->removePathFromShare( share_path );
}

void GuiMain::raiseView( QWidget* w, VNumber chat_id, const QString& chat_name )
{
  setGameInPauseMode();
  mp_stackedWidget->setCurrentWidget( w );
  checkViewActions();
  mp_userList->setChatOpened( chat_id );
  mp_chatList->setChatOpened( chat_id );
  mp_groupList->setChatOpened( chat_id );
  mp_savedChatList->setSavedChatOpened( chat_name );
  raise();
}

void GuiMain::raiseHomeView()
{
  raiseView( mp_home, ID_INVALID, "" );
}

void GuiMain::raiseChatView()
{
  raiseView( mp_chat, mp_chat->chatId(), mp_chat->chatName() );
  mp_chat->updateActionsOnFocusChanged();
  mp_chat->ensureLastMessageVisible();
  mp_chat->ensureFocusInChat();
}

void GuiMain::raiseLocalShareView()
{
  raiseView( mp_shareLocal, ID_INVALID, "" );
}

void GuiMain::raiseNetworkShareView()
{
  mp_shareNetwork->initShares();
  raiseView( mp_shareNetwork, ID_INVALID, "" );
}

void GuiMain::raisePluginView()
{
  QAction* act = qobject_cast<QAction*>( sender() );
  if( !act )
    return;

  int widget_index = act->data().toInt();
  if( widget_index == mp_stackedWidget->currentIndex() )
    return;

  QWidget* plugin_widget = mp_stackedWidget->widget( widget_index );
  if( !plugin_widget )
    return;

  raiseView( plugin_widget, ID_INVALID, "" );
}

void GuiMain::raiseLogView()
{
  raiseView( mp_logView, ID_INVALID, "" );
}

void GuiMain::raiseScreenShotView()
{
  raiseView( mp_screenShot, ID_INVALID, "" );
}

void GuiMain::raiseShareBoxView()
{
  raiseView( mp_shareBox, ID_INVALID, "" );
  mp_shareBox->updateBox();
}

void GuiMain::setGameInPauseMode()
{
  PluginManager::instance().setGamePauseOn();
}

void GuiMain::openUrl( const QUrl& file_url )
{
#ifdef BEEBEEP_DEBUG
  qDebug() << "Opening url:" << file_url.toString();
#endif
#if QT_VERSION >= 0x040800
  if( file_url.isLocalFile() )
#else
  if( file_url.scheme() == QLatin1String( "file" ) )
#endif
  {
    QString file_path = file_url.toLocalFile();
    if( file_path.isEmpty() )
    {
      qWarning() << "Unable to open an empty file path";
      return;
    }

    QFileInfo fi( file_path );
#ifdef Q_OS_MAC
    bool is_exe_file = fi.isBundle();
#else
    bool is_exe_file = fi.isExecutable() && !fi.isDir();
#endif
    if( is_exe_file && QMessageBox::question( this, Settings::instance().programName(),
                             tr( "Do you really want to open the file %1?" ).arg( file_path ),
                             tr( "Yes" ), tr( "No" ), QString(), 1, 1 ) != 0 )
      return;

    qDebug() << "Open file:" << file_url.toString();
    if( !QDesktopServices::openUrl( file_url ) )
      QMessageBox::information( this, Settings::instance().programName(),
                              tr( "Unable to open %1" ).arg( file_path.isEmpty() ? file_url.toString() : file_path ), tr( "Ok" ) );
  }
  else
  {
    QString url_txt = file_url.toString();
    qDebug() << "Open url:" << url_txt;
    if( QDesktopServices::openUrl( file_url ) )
    {
      if( url_txt == Settings::instance().facebookPage() )
        Settings::instance().setIsFacebookPageLinkClicked( true );
    }
    else
      qWarning() << "Unable to open link url:" << url_txt;
  }
}

void GuiMain::selectBeepFile()
{
  QString file_path = FileDialog::getOpenFileName( false, this, Settings::instance().programName(), Settings::instance().beepFilePath(), tr( "Sound files (*.wav)" ) );
  if( file_path.isNull() || file_path.isEmpty() )
    return;

  Settings::instance().setBeepFilePath( file_path );
  qDebug() << "New sound file selected:" << file_path;

  AudioManager::instance().clearBeep();

  if( !Settings::instance().beepOnNewMessageArrived() && !Settings::instance().beepAlwaysOnNewMessageArrived() )
  {
    if( QMessageBox::question( this, Settings::instance().programName(), tr( "Sound is not enabled on a new message. Do you want to enable it?" ), tr( "Yes" ), tr( "No" ) ) == 0 )
    {
      Settings::instance().setBeepOnNewMessageArrived( true );
      mp_actBeepOnNewMessage->setChecked( true );
    }
  }
}

void GuiMain::testBeepFile()
{
  if( !AudioManager::instance().isAudioDeviceAvailable() )
  {
    qWarning() << "Sound device is not available";
    QMessageBox::warning( this, Settings::instance().programName(), tr( "Sound module is not working. The default BEEP will be used." ) );
  }
  else if( !QFile::exists( Settings::instance().beepFilePath() ) )
  {
    QString warn_text = QString( "%1\n%2. %3." ).arg( Settings::instance().beepFilePath() )
                                                  .arg( tr( "Sound file not found" ) )
                                                  .arg( tr( "The default BEEP will be used" ) );
    QMessageBox::warning( this, Settings::instance().programName(), warn_text );
  }

  playBeep();
}

void GuiMain::playBeep()
{
  AudioManager::instance().playBeep();
}

void GuiMain::createGroup()
{
  GuiCreateGroup gcg( this );
  gcg.loadData( true );
  gcg.setModal( true );
  gcg.show();
  gcg.setFixedSize( gcg.size() );
  if( gcg.exec() == QDialog::Accepted )
  {
    mp_core->createGroup( gcg.selectedName(), gcg.selectedUsersId() );
  }
}

void GuiMain::editGroup( VNumber group_id )
{
  Group g = UserManager::instance().group( group_id );
  if( !g.isValid() )
    return;

  GuiCreateGroup gcg( activeChatWindow() );
  gcg.init( g.name(), g.usersId() );
  gcg.loadData( true );
  gcg.setModal( true );
  gcg.show();
  gcg.setFixedSize( gcg.size() );
  if( gcg.exec() == QDialog::Accepted )
  {
    mp_core->changeGroup( group_id, gcg.selectedName(), gcg.selectedUsersId() );
  }
}

void GuiMain::createChat()
{
  switch( QMessageBox::question( this, Settings::instance().programName(),
                 tr( "Group chat will be deleted when all members goes offline." ) + QString( " " ) +
                 tr( "If you want a persistent chat please consider to make a Group instead." ) + QString( " " ) +
                 tr( "Do you wish to continue or create group?" ),
                 tr( "Continue" ), tr( "Create Group" ), tr( "Cancel" ), 0, 2 ) )
  {
  case 0:
    // do nothing
    break;
  case 1:
    createGroup();
    return;
  default:
    return;
  }

  GuiCreateGroup gcg( this );
  gcg.loadData( false );
  gcg.setModal( true );
  gcg.show();
  gcg.setFixedSize( gcg.size() );
  if( gcg.exec() == QDialog::Accepted )
    mp_core->createGroupChat( gcg.selectedName(), gcg.selectedUsersId(), "", true );
}

void GuiMain::editGroupFromChat( VNumber chat_id )
{
  Chat group_chat_tmp = ChatManager::instance().chat( chat_id );
  if( !group_chat_tmp.isGroup() )
  {
    QMessageBox::information( activeChatWindow(), Settings::instance().programName(), tr( "Unable to add users in this chat. Please select a group one." ) );
    return;
  }

  GuiCreateGroup gcg( activeChatWindow() );
  gcg.init( group_chat_tmp.name(), group_chat_tmp.usersId() );
  gcg.loadData( false );
  gcg.setModal( true );
  gcg.show();
  gcg.setFixedSize( gcg.size() );
  if( gcg.exec() == QDialog::Accepted )
  {
    Group g = UserManager::instance().findGroupByPrivateId( group_chat_tmp.privateId() );
    if( g.isValid() )
      mp_core->changeGroup( g.id(), gcg.selectedName(), gcg.selectedUsersId() );
    else
      mp_core->changeGroupChat( group_chat_tmp.id(), gcg.selectedName(), gcg.selectedUsersId(), true );
  }
}

void GuiMain::raiseOnTop()
{
  if( isMinimized() || !isVisible() )
    showNormal();
  else
    show();

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

  if( mp_stackedWidget->currentWidget() == mp_chat )
    mp_chat->ensureFocusInChat();
}

void GuiMain::checkAutoStartOnBoot( bool add_service )
{
  if( add_service )
  {
    if( Settings::instance().addStartOnSystemBoot() )
      QMessageBox::information( this, Settings::instance().programName(), tr( "Now %1 will start on windows boot." ).arg( Settings::instance().programName() ) );
    else
      QMessageBox::warning( this, Settings::instance().programName(), tr( "Unable to add this key in the registry: permission denied." ) );
  }
  else
  {
    if( Settings::instance().removeStartOnSystemBoot() )
      QMessageBox::information( this, Settings::instance().programName(), tr( "%1 will not start on windows boot." ).arg( Settings::instance().programName() ) );
    else
      QMessageBox::warning( this, Settings::instance().programName(), tr( "Unable to add this key in the registry: permission denied." ) );
  }
}

void GuiMain::loadSession()
{
  mp_core->loadUsersAndGroups();
  QTimer::singleShot( 200, mp_core, SLOT( buildSavedChatList() ) );
  mp_shareLocal->updatePaths();
  QTimer::singleShot( 2000, mp_core, SLOT( buildLocalShareList() ) );

  if( !mp_trayIcon->isVisible() )
  {
    mp_trayIcon->show();
#ifdef Q_OS_LINUX
    qApp->processEvents();
    mp_trayIcon->hide();
    qApp->processEvents();
    mp_trayIcon->show();
#endif
  }
}

void GuiMain::showSavedChatSelected( const QString& chat_name )
{
  if( chat_name.isEmpty() )
    return;

  mp_savedChat->showSavedChat( chat_name );

  raiseView( mp_savedChat, ID_INVALID, chat_name );
}

void GuiMain::removeSavedChat( const QString& chat_name )
{
  if( chat_name.isEmpty() )
    return;

  qDebug() << "Delete saved chat:" << chat_name;
  ChatManager::instance().removeSavedTextFromChat( chat_name );
  mp_savedChatList->updateSavedChats();

  if( mp_stackedWidget->currentWidget() == mp_chat && mp_chat->chatName() == chat_name )
    mp_chat->reloadChat();
  else if( mp_stackedWidget->currentWidget() == mp_savedChat )
    mp_savedChat->showSavedChat( chat_name );
}

void GuiMain::linkSavedChat( const QString& chat_name )
{
  bool ok = false;
  QStringList chat_names_string_list = ChatManager::instance().chatNamesToStringList( true );
  chat_names_string_list.removeOne( chat_name );

  QString chat_name_selected = QInputDialog::getItem( this, Settings::instance().programName(),
                                        tr( "Please select a chat you would like to link the saved text."),
                                        chat_names_string_list, 0, false, &ok );
  if( !ok )
    return;

  bool add_to_existing_saved_text = false;
  if( ChatManager::instance().chatHasSavedText( chat_name_selected ) )
  {
     switch( QMessageBox::question( this, Settings::instance().programName(),
               tr( "The chat '%1' selected has already a saved text.<br />"
                   "What do you want to do with the selected saved text?" ).arg( chat_name_selected ),
                   tr( "Overwrite" ), tr( "Add in the head" ), tr( "Cancel" ), 2, 2 ) )
     {
     case 0:
       break;
     case 1:
       add_to_existing_saved_text = true;
       break;
     default:
       return;
     }

  }

  ChatManager::instance().updateChatSavedText( chat_name, chat_name_selected, add_to_existing_saved_text );
  mp_savedChatList->updateSavedChats();
}

bool GuiMain::openWebUrl( const QString& web_url )
{
  QUrl url( web_url );

  if( !QDesktopServices::openUrl( url ) )
  {
    QMessageBox::information( this, Settings::instance().programName(), tr( "Unable to open %1" ).arg( web_url ), tr( "Ok" ) );
    return false;
  }
  else
    return true;
}

void GuiMain::checkNewVersion()
{
  QString url_and_arguments = Settings::instance().checkVersionWebSite();
  foreach( GameInterface* gi, PluginManager::instance().games() )
    url_and_arguments.append( QString( "&%1=%2").arg( gi->name().toLower() ).arg( gi->version() ) );
  openWebUrl( url_and_arguments );
}

void GuiMain::openWebSite()
{
  openWebUrl( Settings::instance().officialWebSite() );
}

void GuiMain::openDownloadPluginPage()
{
  openWebUrl( Settings::instance().pluginWebSite() );
}

void GuiMain::openDonationPage()
{
  openWebUrl( Settings::instance().donationWebSite() );
}

void GuiMain::openHelpPage()
{
  openWebUrl( Settings::instance().helpWebSite() );
}

void GuiMain::openFacebookPage()
{
  if( openWebUrl( Settings::instance().facebookPage() ) )
    Settings::instance().setIsFacebookPageLinkClicked( true );
}

void GuiMain::setInIdle()
{
  if( !mp_core->isConnected() )
    return;

  if( !Settings::instance().autoUserAway() )
    return;

  if( Settings::instance().localUser().status() == User::Away )
    return;

  m_lastUserStatus = Settings::instance().localUser().status();
  mp_core->setLocalUserStatus( User::Away );
  updateStatusIcon();
}

void GuiMain::exitFromIdle()
{
  if( !mp_core->isConnected() )
    return;

  if( !Settings::instance().autoUserAway() )
    return;

  if( Settings::instance().localUser().status() != User::Away )
    return;

  mp_core->setLocalUserStatus( m_lastUserStatus );
  updateStatusIcon();
}

void GuiMain::showMessage( const QString& status_msg, int time_out )
{
  statusBar()->showMessage( status_msg, time_out );
}

void GuiMain::changeUserColor( VNumber user_id )
{
  User u = UserManager::instance().findUser( user_id );
  if( !u.isValid() )
  {
    QMessageBox::warning( this, Settings::instance().programName(), tr( "User not found." ) );
    return;
  }

  QColor c = QColorDialog::getColor( QColor( u.color() ), this );
  if( c.isValid() )
  {
    u.setColor( c.name() );
    UserManager::instance().setUser( u );
    mp_chat->reloadChat();
    mp_userList->setUser( u, false );
  }
}

void GuiMain::checkGroup( VNumber group_id )
{
  if( UserManager::instance().group( group_id ).isValid() )
    mp_groupList->updateGroup( group_id );
  else
    mp_groupList->loadGroups();
}

void GuiMain::checkChat( VNumber chat_id )
{
  mp_chatList->updateChat( chat_id );
  mp_savedChatList->updateSavedChats();

  if( mp_stackedWidget->currentWidget() == mp_chat && mp_chat->chatId() == chat_id )
  {
    mp_chat->reloadChat();
    return;
  }

  GuiFloatingChat* fl_chat = floatingChat( chat_id );
  if( fl_chat )
    fl_chat->guiChat()->reloadChat();

}

bool GuiMain::checkAllChatMembersAreConnected( const QList<VNumber>& users_id )
{
  if( !mp_core->areUsersConnected( users_id ) )
  {
    if( QMessageBox::question( activeChatWindow(), Settings::instance().programName(),
                               tr( "All the members of this chat are not online. The changes may not be permanent. Do you wish to continue?" ),
                               tr( "Yes" ), tr( "No" ), QString::null, 1, 1 ) != 0 )
       return false;
  }
  return true;
}

void GuiMain::leaveGroupChat( VNumber chat_id )
{
  Chat c = ChatManager::instance().chat( chat_id );
  if( !c.isValid() )
    return;
  if( !c.hasUser( ID_LOCAL_USER ) )
    return;

  if( !checkAllChatMembersAreConnected( c.usersId() ) )
    return;

  Group g = UserManager::instance().findGroupByPrivateId( c.privateId() );
  if( g.isValid() )
  {
    if( QMessageBox::warning( activeChatWindow(), Settings::instance().programName(),
                              tr( "%1 is a your group. You can not leave the chat." ).arg( g.name() ),
                              tr( "Delete this group" ), tr( "Cancel" ), QString(), 1, 1 ) == 1 )
        return;

    if( mp_core->removeGroup( g.id() ) )
    {
      closeFloatingChat( chat_id );
      raiseHomeView();
      mp_chat->setChatId( ID_DEFAULT_CHAT, false );
      mp_groupList->loadGroups();
      mp_chatList->reloadChatList();
      mp_savedChatList->updateSavedChats();
    }
    return;
  }

  if( mp_core->removeUserFromChat( Settings::instance().localUser(), chat_id ) )
  {
    GuiChat* gui_chat = guiChat( chat_id );
    if( gui_chat )
      gui_chat->reloadChatUsers();
    mp_chatList->reloadChatList();
    checkViewActions();
  }
  else
    QMessageBox::warning( this, Settings::instance().programName(), tr( "You cannot leave this chat." ) );
}

void GuiMain::removeGroup( VNumber group_id )
{
  Group g = UserManager::instance().group( group_id );
  if( g.isValid() )
  {
    if( !checkAllChatMembersAreConnected( g.usersId() ) )
      return;

    if( QMessageBox::question( this, Settings::instance().programName(),
                               tr( "Do you really want to delete group '%1'?" ).arg( g.name() ),
                               tr( "Yes" ), tr( "No" ), QString::null, 1, 1 ) == 0 )
    {
      Chat c = ChatManager::instance().findGroupChatByPrivateId( g.privateId() );
      if( mp_core->removeGroup( group_id ) )
      {
        if( c.isValid() )
          closeFloatingChat( c.id() );
        raiseHomeView();
        mp_chat->setChatId( ID_DEFAULT_CHAT, false );
        mp_groupList->loadGroups();
        mp_chatList->reloadChatList();
        mp_savedChatList->updateSavedChats();
      }
    }
  }
}

void GuiMain::clearChat( VNumber chat_id )
{
  Chat c = ChatManager::instance().chat( chat_id );
  if( !c.isValid() )
    return;
  QString chat_name = c.isDefault() ? QObject::tr( "All Lan Users" ).toLower() : c.name();
  if( c.isEmpty() && !ChatManager::instance().chatHasSavedText( c.name() ) )
  {
    QMessageBox::information( activeChatWindow(), Settings::instance().programName(), tr( "Chat with %1 is empty." ).arg( chat_name ) );
    return;
  }

  QString question_txt = tr( "Do you really want to clear messages with %1?" ).arg( chat_name );
  QString button_2_text;
  if( ChatManager::instance().chatHasSavedText( c.name() ) )
    button_2_text = QString( "  " ) + tr( "Yes and delete history" ) + QString( "  " );

  switch( QMessageBox::information( activeChatWindow(), Settings::instance().programName(), question_txt, tr( "Yes" ), tr( "No" ), button_2_text, 1, 1 ) )
  {
  case 0:
    mp_core->clearMessagesInChat( chat_id );
    break;
  case 2:
    mp_core->clearMessagesInChat( chat_id );
    ChatManager::instance().removeSavedTextFromChat( c.name() );
    break;
  default:
    return;
  }

  if( c.isPrivate() )
    mp_userList->setUnreadMessages( chat_id, 0 );
  mp_chatList->reloadChatList();
  mp_savedChatList->updateSavedChats();
  reloadChat( chat_id );
  GuiChat* gui_chat = guiChat( chat_id );
  if( gui_chat )
    gui_chat->ensureFocusInChat();
}

bool GuiMain::reloadChat( VNumber chat_id )
{
  if( mp_stackedWidget->currentWidget() == mp_chat && mp_chat->chatId() == chat_id )
    return mp_chat->reloadChat();

  GuiFloatingChat* fl_chat = floatingChat( chat_id );
  if( fl_chat )
    return fl_chat->guiChat()->reloadChat();

  return false;
}

void GuiMain::removeChat( VNumber chat_id )
{
  Chat c = ChatManager::instance().chat( chat_id );
  if( !c.isValid() )
  {
    qWarning() << "Invalid chat" << chat_id << "found in removeChat function";
    return;
  }

  QString question_txt = tr( "Do you really want to delete chat with %1?" ).arg( c.name() );
  if( QMessageBox::information( this, Settings::instance().programName(), question_txt, tr( "Yes" ), tr( "No" ), QString::null, 1, 1 ) != 0 )
    return;

  if( mp_core->removeChat( chat_id ) )
  {
    mp_chatList->reloadChatList();
    if( mp_chat->chatId() == chat_id )
    {
      if( mp_stackedWidget->currentWidget() == mp_chat )
        showDefaultChat();
      else
        mp_chat->setChatId( ID_DEFAULT_CHAT, false );
    }
    else
    {
      GuiFloatingChat* fl_chat = floatingChat( chat_id );
      if( fl_chat )
        fl_chat->close();
    }
  }
  else
    QMessageBox::warning( this, Settings::instance().programName(), tr( "Unable to delete this chat." ) );
}

void GuiMain::showChatForGroup( VNumber group_id )
{
  Group g = UserManager::instance().group( group_id );
  if( !g.isValid() )
    return;

  Chat c = ChatManager::instance().findGroupChatByPrivateId( g.privateId() );
  if( !c.isValid() )
  {
    mp_core->createGroupChat( g, true );
    c = ChatManager::instance().findGroupChatByPrivateId( g.privateId() );
  }

  showChat( c.id() );
}

void GuiMain::showSharesForUser( const User& u )
{
  mp_shareNetwork->showSharesForUser( u );
  QString share_message = tr( "%1 has shared %2 files" ).arg( u.name() ).arg( FileShare::instance().fileSharedFromUser( u.id() ).size() );
  showMessage( share_message, 0 );
}

void GuiMain::selectLanguage()
{
  GuiLanguage gl( this );
  gl.setModal( true );
  gl.loadLanguages();
  gl.setSizeGripEnabled( true );
  gl.show();

  if( gl.exec() == QDialog::Rejected )
    return;

  QString old_language_path = Settings::instance().languageFilePath( Settings::instance().languagePath(), Settings::instance().language() );
  QString new_language_path = Settings::instance().languageFilePath( gl.folderSelected(), gl.languageSelected() );

  if( old_language_path != new_language_path )
  {
    QString language_message;
    if( gl.languageSelected().isEmpty() )
      language_message = tr( "Default language is restored." );
    else
      language_message = tr( "New language '%1' is selected." ).arg( gl.languageSelected() );

    QMessageBox::information( this, Settings::instance().programName(),
                              QString( "%1<br />%2" ).arg( language_message ).arg( tr( "You must restart %1 to apply these changes." )
                                                                                    .arg( Settings::instance().programName() ) ) );

    Settings::instance().setLanguage( gl.languageSelected() );
    Settings::instance().setLanguagePath( gl.folderSelected() );
  }
}

void GuiMain::showUp()
{
  raiseOnTop();
}

void GuiMain::checkChatToolbar()
{
  if( mp_stackedWidget->currentWidget() != mp_chat )
  {
    if( mp_barChat->isVisible() )
      mp_barChat->hide();
  }
  else
  {
    mp_barChat->setVisible( Settings::instance().showChatToolbar() );
  }
}

void GuiMain::showAddUser()
{
  GuiAddUser gad( this );
  gad.loadUsers();
  gad.setModal( true );
  gad.setSizeGripEnabled( true );
  gad.show();

  if( gad.exec() == QDialog::Accepted )
    mp_core->sendHelloToHostsInSettings();
}

void GuiMain::showChatSettingsMenu()
{
  mp_menuChat->exec( QCursor::pos() );
}

void GuiMain::showDefaultServerPortInMenu()
{
  QString host_address = tr( "offline" );
  QString broadcast_port = tr( "offline" );
  QString listener_port = tr( "offline" );
  QString file_transfer_port = tr( "offline" );
#ifdef BEEBEEP_USE_MULTICAST_DNS
  QString multicast_dns = tr( "inactive" );
#endif

  if( mp_core->isConnected() )
  {
    mp_menuNetworkStatus->setIcon( QIcon( ":/images/network-connected.png" ) );
    mp_actHostAddress->setIcon( QIcon( ":/images/connect.png" ) );
    mp_actPortBroadcast->setIcon( QIcon( ":/images/broadcast.png" ) );
    mp_actPortListener->setIcon( QIcon( ":/images/chat.png" ) );

    host_address = Settings::instance().localUser().hostAddress().toString();
    broadcast_port = QString::number( Settings::instance().defaultBroadcastPort() );
    listener_port = QString::number( Settings::instance().localUser().hostPort() );
    if( Settings::instance().fileTransferIsEnabled() )
    {
      file_transfer_port = QString::number( mp_core->fileTransferPort() );
      mp_actPortFileTransfer->setIcon( QIcon( ":/images/network-scan.png" ) );
    }
    else
    {
      file_transfer_port = tr( "disabled" );
      mp_actPortFileTransfer->setIcon( QIcon() );
    }

#ifdef BEEBEEP_USE_MULTICAST_DNS
    if( mp_core->dnsMulticastingIsActive() )
    {
      multicast_dns = tr( "active" );
      mp_actMulticastDns->setIcon( QIcon( ":/images/mdns.png" ) );
    }
#endif
  }
  else
  {
    mp_menuNetworkStatus->setIcon( QIcon( ":/images/network-disconnected.png" ) );
    mp_actHostAddress->setIcon( QIcon() );
    mp_actPortBroadcast->setIcon( QIcon() );
    mp_actPortListener->setIcon( QIcon() );
    mp_actPortFileTransfer->setIcon( QIcon() );
#ifdef BEEBEEP_USE_MULTICAST_DNS
    mp_actMulticastDns->setIcon( QIcon() );
#endif
  }

  mp_actHostAddress->setText( QString( "ip: %1" ).arg( host_address ) );
  mp_actPortBroadcast->setText( QString( "udp: %1" ).arg( broadcast_port ) );
  mp_actPortListener->setText( QString( "tcp1: %1" ).arg( listener_port ) );
  mp_actPortFileTransfer->setText( QString( "tcp2: %1" ).arg( file_transfer_port ) );
#ifdef BEEBEEP_USE_MULTICAST_DNS
  mp_actMulticastDns->setText( QString( "mdns: %1" ).arg( multicast_dns ) );
#endif
}

void GuiMain::sendBroadcastMessage()
{
  mp_actBroadcast->setDisabled( true );
  mp_core->sendBroadcastMessage();
  mp_core->sendMulticastingMessage();
  QTimer::singleShot( 10000, this, SLOT( enableBroadcastAction() ) );
}

void GuiMain::enableBroadcastAction()
{
  mp_actBroadcast->setEnabled( true );
}

void GuiMain::checkUserSelected( VNumber user_id )
{
  User u = UserManager::instance().findUser( user_id );
  if( !u.isValid() )
  {
    qWarning() << "Invalid user id" << user_id << "found in check user selected";
    return;
  }

  Chat c = ChatManager::instance().privateChatForUser( user_id );
  if( !c.isValid() )
  {
    mp_core->createPrivateChat( u );
    c = ChatManager::instance().privateChatForUser( user_id );
    if( !c.isValid() )
    {
      qWarning() << "Unable to create private chat for user" << user_id;
      return;
    }
  }

  showChat( c.id() );
}

void GuiMain::showConnectionStatusChanged( const User& u )
{
  if( !mp_core->isConnected() )
    return;

  if( Settings::instance().showOnlyMessageNotificationOnTray() )
    return;

  QString msg;
  if( u.isStatusConnected() )
    msg = tr( "%1 is online" ).arg( u.name() );
  else
    msg = tr( "%1 is offline" ).arg( u.name() );

  Chat c = ChatManager::instance().privateChatForUser( u.id() );
  if( c.isValid() && u.isStatusConnected() )
    mp_trayIcon->showUserStatusChanged( c.id(), msg );
  else
    mp_trayIcon->showUserStatusChanged( ID_DEFAULT_CHAT, msg );
}

void GuiMain::changeAvatarSizeInList()
{
  bool ok = false;
  int avatar_size = QInputDialog::getInt( this, Settings::instance().programName(), tr( "Please select the new size of the user picture" ),
                                          Settings::instance().avatarIconSize().height(), 16, 96, 8, &ok );
  if( !ok )
    return;

  Settings::instance().setAvatarIconSize( QSize( avatar_size, avatar_size ) );
  refreshUserList();
}

void GuiMain::toggleUserFavorite( VNumber user_id )
{
  mp_core->toggleUserFavorite( user_id );
  refreshUserList();
}

void GuiMain::createGroupFromChat( VNumber chat_id )
{
  mp_core->createGroupFromChat( chat_id );
}

void GuiMain::removeUserFromList( VNumber user_id )
{
  if( mp_core->removeOfflineUser( user_id ) )
  {
    Chat c = ChatManager::instance().privateChatForUser( user_id );
    if( c.isValid() )
    {
      closeFloatingChat( c.id() );
      if( mp_chat->chatId() == c.id() )
      {
        if( mp_stackedWidget->currentWidget() == mp_chat )
          showDefaultChat();
        else
          mp_chat->setChatId( ID_DEFAULT_CHAT, false );
      }
    }
    refreshUserList();
    mp_chatList->reloadChatList();
  }
}

void GuiMain::openResourceFolder()
{
  QUrl data_folder_url = QUrl::fromLocalFile( Settings::instance().resourceFolder() );
  openUrl( data_folder_url );
}

void GuiMain::openDataFolder()
{
  QUrl data_folder_url = QUrl::fromLocalFile( Settings::instance().dataFolder() );
  openUrl( data_folder_url );
}

GuiFloatingChat* GuiMain::floatingChat( VNumber chat_id ) const
{
  foreach( GuiFloatingChat* fl_chat, m_floatingChats )
  {
    if( fl_chat->chatId() == chat_id )
      return fl_chat;
  }
  return 0;
}

bool GuiMain::floatingChatExists( VNumber chat_id ) const
{
  return floatingChat( chat_id ) != 0;
}

void GuiMain::closeFloatingChat( VNumber chat_id )
{
  GuiFloatingChat* fl_chat = floatingChat( chat_id );
  if( fl_chat )
    fl_chat->close();
}

void GuiMain::attachChat( VNumber chat_id )
{
  GuiFloatingChat* fl_chat = floatingChat( chat_id );
  if( !fl_chat )
  {
#ifdef BEEBEEP_DEBUG
    qWarning() << "Floating chat" << chat_id << "not found in GuiMain::attachChat";
#endif
    return;
  }

  m_floatingChats.removeOne( fl_chat );
  fl_chat->deleteLater();

#ifdef BEEBEEP_DEBUG
  qDebug() << "Floating chat" << chat_id << "closed and deleted";
#endif
}

void GuiMain::detachChat( VNumber chat_id )
{
  if( floatingChatExists( chat_id ) )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Floating chat" << chat_id << "already exists";
#endif
    return;
  }

  if( chat_id == ID_DEFAULT_CHAT )
  {
    showDefaultChat();
    return;
  }

  GuiFloatingChat* fl_chat = new GuiFloatingChat;
  if( !fl_chat->setChatId( chat_id ) )
  {
    qWarning() << "Unable to create floating chat" << chat_id;
    fl_chat->deleteLater();
    return;
  }

  if( mp_chat->chatId() == chat_id )
  {
    showDefaultChat();
    raiseHomeView();
  }

  fl_chat->guiChat()->updateActions( mp_core->isConnected(), mp_core->connectedUsers() );
  setupChatConnections( fl_chat->guiChat() );
  connect( fl_chat, SIGNAL( attachChatRequest( VNumber ) ),this, SLOT( attachChat( VNumber ) ) );
  connect( fl_chat, SIGNAL( readAllMessages( VNumber ) ), this, SLOT( readAllMessagesInChat( VNumber ) ) );
  m_floatingChats.append( fl_chat );

  fl_chat->checkWindowFlagsAndShow();
  fl_chat->guiChat()->updateShortcuts();
  fl_chat->guiChat()->updateActions( mp_core->isConnected(), mp_core->connectedUsers() );
  fl_chat->guiChat()->ensureLastMessageVisible();
  fl_chat->raiseOnTop();
  fl_chat->guiChat()->ensureFocusInChat();

}

QWidget* GuiMain::activeChatWindow()
{
  foreach( GuiFloatingChat* fl_chat, m_floatingChats )
  {
    if( fl_chat->isActiveWindow() )
      return (QWidget*)fl_chat;
  }

  return (QWidget*)this;
}

GuiChat* GuiMain::guiChat( VNumber chat_id )
{
  if( mp_chat->chatId() == chat_id )
    return mp_chat;

  GuiFloatingChat* fl_chat = floatingChat( chat_id );
  if( fl_chat )
    return fl_chat->guiChat();
  else
    return 0;
}

void GuiMain::loadUserStatusRecentlyUsed()
{
  if( !mp_menuUserStatusList->isEmpty() )
    mp_menuUserStatusList->clear();

  QList<UserStatusRecord> user_status_list;
  foreach( QString s, Settings::instance().userStatusList() )
  {
    UserStatusRecord usr = Protocol::instance().loadUserStatusRecord( s );
    if( usr.isValid() )
      user_status_list.append( usr );
  }

  if( user_status_list.size() < Settings::instance().maxUserStatusDescriptionInList() )
  {
    UserStatusRecord usr1;
    usr1.setStatus( User::Away );
    usr1.setStatusDescription( tr( "at lunch" ) );
    if( !user_status_list.contains( usr1 ) )
      user_status_list.append( usr1 );
  }

  if( user_status_list.size() < Settings::instance().maxUserStatusDescriptionInList() )
  {
    UserStatusRecord usr2;
    usr2.setStatus( User::Busy );
    usr2.setStatusDescription( tr( "in a meeting" ) );
    if( !user_status_list.contains( usr2 ) )
      user_status_list.append( usr2 );
  }

  qSort( user_status_list );

  QAction* act;
  foreach( UserStatusRecord usr, user_status_list )
  {
    if( usr.isValid() )
    {
      act = mp_menuUserStatusList->addAction( Bee::userStatusIcon( usr.status() ), usr.statusDescription(), this, SLOT( recentlyUsedUserStatusSelected() ) );
      act->setData( Protocol::instance().saveUserStatusRecord( usr ) );
    }
  }
}

void GuiMain::recentlyUsedUserStatusSelected()
{
  QAction* act = qobject_cast<QAction*>( sender() );
  if( !act )
    return;

  UserStatusRecord usr = Protocol::instance().loadUserStatusRecord( act->data().toString() );
  if( usr.isValid() )
  {
    mp_core->setLocalUserStatusDescription( usr.status(), usr.statusDescription(), false );
    setUserStatusSelected( usr.status() );
    loadUserStatusRecentlyUsed();
  }
}

void GuiMain::clearRecentlyUsedUserStatus()
{
  if( QMessageBox::question( this, Settings::instance().programName(),
                             tr( "Do you really want to clear all saved status descriptions?" ),
                             tr( "Yes" ), tr( "No" ), QString::null, 1, 1 ) != 0 )
    return;

  qDebug() << "User status description list is cleared";
  QStringList sl = Settings::instance().userStatusList();
  sl.clear();
  Settings::instance().setUserStatusList( sl );
  loadUserStatusRecentlyUsed();
}

void GuiMain::loadSavedChatsCompleted()
{
#ifdef BEEBEEP_DEBUG
  qDebug() << "Loading saved chats completed";
#endif
  mp_savedChatList->updateSavedChats();
  mp_chat->reloadChat();
}

void GuiMain::editShortcuts()
{
  GuiShortcut gs( this );
  gs.setModal( true );
  gs.loadShortcuts();
  gs.setSizeGripEnabled( true );
  gs.show();

  if( gs.exec() == QDialog::Rejected )
    return;

  Settings::instance().setShortcuts( ShortcutManager::instance().saveToStringList() );
  Settings::instance().save();
  updateShortcuts();
}

void GuiMain::updateShortcuts()
{
  mp_chat->updateShortcuts();
  foreach( GuiFloatingChat* fl_chat, m_floatingChats )
     fl_chat->guiChat()->updateShortcuts();

  mp_savedChat->updateShortcuts();

  QKeySequence ks = ShortcutManager::instance().shortcut( ShortcutManager::ShowFileTransfers );
  if( !ks.isEmpty() && Settings::instance().useShortcuts() )
    mp_actViewFileTransfer->setShortcut( ks );
  else
    mp_actViewFileTransfer->setShortcut( QKeySequence() );

  ks = ShortcutManager::instance().shortcut( ShortcutManager::MinimizeAllChats );
  if( !ks.isEmpty() )
  {
    mp_scMinimizeAllChats->setKey( ks );
    mp_scMinimizeAllChats->setEnabled( Settings::instance().useShortcuts() );
  }
  else
    mp_scMinimizeAllChats->setEnabled( false );

#ifdef BEEBEEP_USE_QXT
  ks = ShortcutManager::instance().shortcut( ShortcutManager::ShowAllChats );
  if( !ks.isEmpty() )
  {
    mp_scShowAllChats->setShortcut( ks );
    mp_scShowAllChats->setEnabled( Settings::instance().useShortcuts() );
  }
  else
    mp_scShowAllChats->setEnabled( false );
#endif
  ks = ShortcutManager::instance().shortcut( ShortcutManager::ShowNextUnreadMessage );
  if( !ks.isEmpty() )
  {
    mp_scShowNextUnreadMessage->setKey( ks );
    mp_scShowNextUnreadMessage->setEnabled( Settings::instance().useShortcuts() );
  }
  else
    mp_scShowNextUnreadMessage->setEnabled( false );

  ks = ShortcutManager::instance().shortcut( ShortcutManager::Broadcast );
  if( !ks.isEmpty() && Settings::instance().useShortcuts() )
    mp_actBroadcast->setShortcut( ks );
  else
    mp_actBroadcast->setShortcut( QKeySequence() );

}

void GuiMain::onApplicationFocusChanged( QWidget* old, QWidget* now )
{
  if( old == 0 && isAncestorOf( now )  )
  {
    if( mp_stackedWidget->currentWidget() == mp_chat )
    {
#ifdef BEEBEEP_DEBUG
      qDebug() << "Chat in the main window has grab the focus. All messages can be set to read";
#endif
      readAllMessagesInChat( mp_chat->chatId() );
      mp_chat->updateActionsOnFocusChanged();
      return;
    }
  }
  else
  {
    bool current_state = isActiveWindow();
    if( current_state != m_prevActivatedState )
    {
      m_prevActivatedState = current_state;
      if( current_state && mp_stackedWidget->currentWidget() == mp_chat )
      {
#ifdef BEEBEEP_DEBUG
        qDebug() << "Chat in the main window has grab focus (active)";
#endif
        readAllMessagesInChat( mp_chat->chatId() );
        mp_chat->updateActionsOnFocusChanged();
      }
    }
  }
}

void GuiMain::minimizeAllChats()
{
  QWidget* w = qApp->activeWindow();

  bool last_active_window_exists = w == this ? true : false;

  if( !m_floatingChats.isEmpty() )
  {
    foreach( GuiFloatingChat* fl_chat, m_floatingChats )
    {
      if( !fl_chat->isMinimized() )
        fl_chat->showMinimized();
      if( fl_chat == mp_lastActiveWindow )
        last_active_window_exists = true;
    }
  }

  if( !isMinimized() )
    showMinimized();

  if( last_active_window_exists )
    mp_lastActiveWindow = w;
}

void GuiMain::showAllChats()
{
  bool last_active_window_exists = false;

  if( !m_floatingChats.isEmpty() )
  {
    foreach( GuiFloatingChat* fl_chat, m_floatingChats )
    {
      if( fl_chat->isMinimized() )
        fl_chat->showNormal();
      else
        show();

      if( fl_chat == mp_lastActiveWindow )
        last_active_window_exists = true;
    }
  }

  if( isMinimized() )
    showNormal();
  else
    show();

  if( this == mp_lastActiveWindow )
    last_active_window_exists = true;

  if( last_active_window_exists )
  {
    mp_lastActiveWindow->raise();
    qApp->setActiveWindow( mp_lastActiveWindow );
  }
}

void GuiMain::selectDictionatyPath()
{
  QString dictionary_path = FileDialog::getOpenFileName( false, this, tr( "Select your dictionary path" ), Settings::instance().dictionaryPath(), QString( "*.dic" ) );

  if( dictionary_path.isEmpty() )
    return;

  Settings::instance().setDictionaryPath( dictionary_path );
#ifdef BEEBEEP_USE_HUNSPELL
  if( SpellChecker::instance().setDictionary( dictionary_path ) )
    showMessage( tr( "Dictionary selected: %1" ).arg( dictionary_path ), 5000 );
  else
    showMessage( tr( "Unable to set dictionary: %1" ).arg( dictionary_path ), 5000 );
#endif

  // update spellchecker and wordcompleter actions
  QList<GuiChat*> gui_chat_list = guiChatList();
  foreach( GuiChat* gc, gui_chat_list )
    gc->updateActionsOnFocusChanged();

}

void GuiMain::onNetworkInterfaceDown()
{
  if( mp_core->isConnected() )
  {
    raiseHomeView();
    m_autoConnectOnInterfaceUp = true;
    QTimer::singleShot( 1000, this, SLOT( stopCore() ) );
  }
}

void GuiMain::onNetworkInterfaceUp()
{
  if( m_autoConnectOnInterfaceUp && !mp_core->isConnected() )
    QTimer::singleShot( 1000, this, SLOT( startCore() ) );
}

static bool IsTimeToCheck( int ticks, int tick_for_check ) { return ticks % tick_for_check == 0; }

void GuiMain::onTickEvent( int ticks )
{
  if( IsTimeToCheck( ticks, Settings::instance().tickIntervalCheckNetwork() ) )
    QTimer::singleShot( 0, mp_core, SLOT( checkNetworkInterface() ) );

  if( IsTimeToCheck( ticks, Settings::instance().tickIntervalCheckIdle() ) )
  {
    BeeApplication* bee_app = (BeeApplication*)qApp;
    if( bee_app->idleTimeout() > 0 )
      QTimer::singleShot( 0, bee_app, SLOT( checkIdle() ) );
  }

  mp_trayIcon->onTickEvent( ticks );
  if( mp_trayIcon->iconStatusIsMessage() )
    setWindowIcon( mp_trayIcon->icon() );

  mp_chatList->onTickEvent( ticks );
  mp_userList->onTickEvent( ticks );
  mp_core->onTickEvent( ticks );

  if( mp_core->hasFileTransferInProgress() )
    mp_actViewFileTransfer->setIcon( ticks % 2 == 0 ? QIcon( ":/images/file-transfer-progress.png" ) : QIcon( ":/images/file-transfer.png" ) );

}

void GuiMain::onChatReadByUser( VNumber chat_id, VNumber user_id )
{
  GuiChat* gc = guiChat( chat_id );
  if( gc )
    gc->setChatReadByUser( user_id );
}

void GuiMain::readAllMessagesInChat( VNumber chat_id )
{
  if( mp_core->readAllMessagesInChat( chat_id ) )
  {
    mp_userList->setUnreadMessages( chat_id, 0 );
    mp_chatList->updateChat( chat_id );
    mp_groupList->updateChat( chat_id );
    statusBar()->clearMessage();
  }

  GuiFloatingChat *fl_chat = floatingChat( chat_id );
  if( fl_chat )
    fl_chat->setMainIcon( false );

  Chat c = ChatManager::instance().firstChatWithUnreadMessages();
  if( c.isValid() )
  {
    mp_trayIcon->setUnreadMessages( c.id(), c.unreadMessages() );
  }
  else
  {
    mp_trayIcon->resetChatId();
    mp_trayIcon->setDefaultIcon();
  }

  updateMainIcon();
  updateNewMessageAction();
}

void GuiMain::updateMainIcon()
{
  if( ChatManager::instance().hasUnreadMessages() )
    setWindowIcon( QIcon( ":/images/beebeep-message.png" ) );
  else
    setWindowIcon( QIcon( ":/images/beebeep.png" ) );
}

void GuiMain::saveSession( QSessionManager& )
{
#ifdef BEEBEEP_DEBUG
  qDebug() << "Session manager ask to save session";
#endif
  forceShutdown();
}

void GuiMain::updateEmoticons()
{
  QTimer::singleShot( 0, mp_emoticonsWidget, SLOT( updateEmoticons() ) );

  if( m_floatingChats.isEmpty() )
    return;

  foreach( GuiFloatingChat* fl_chat, m_floatingChats )
    fl_chat->updateEmoticon();
}

void GuiMain::saveChatMessagesOnExit()
{
  mp_core->saveChatMessages();
}

void GuiMain::updateNewMessageAction()
{
  mp_actViewNewMessage->setEnabled( ChatManager::instance().hasUnreadMessages() );
}

QList<GuiChat*> GuiMain::guiChatList() const
{
  QList<GuiChat*> gui_chat_list;

  gui_chat_list.append( mp_chat );
  if( !m_floatingChats.isEmpty() )
  {
    foreach( GuiFloatingChat* fl_chat, m_floatingChats )
      gui_chat_list.append( fl_chat->guiChat() );
  }

  return gui_chat_list;
}

void GuiMain::saveGeometryAndState()
{
  bool save_settings = false;
  if( isVisible() )
  {
    qDebug() << "Main window geometry and state saved";
    Settings::instance().setGuiGeometry( saveGeometry() );
    Settings::instance().setGuiState( saveState() );
    save_settings = true;
  }

  if( mp_chat->isVisible() )
  {
    qDebug() << "Main chat window splitter state saved";
    Settings::instance().setChatSplitterState( mp_chat->chatSplitter()->saveState() );
    save_settings = true;
  }

  if( save_settings )
  {
    Settings::instance().save();
    showMessage( tr( "Window geometry and state saved" ), 3000 );
  }
}

void GuiMain::onChangeSettingBeepOnNewMessage( QAction* act )
{
  if( !act )
    return;

  Settings::instance().setBeepOnNewMessageArrived( mp_actBeepOnNewMessage->isChecked() );
  Settings::instance().setBeepAlwaysOnNewMessageArrived( mp_actAlwaysBeepOnNewMessage->isChecked() );
  Settings::instance().save();
}

void GuiMain::onChangeSettingOnExistingFile( QAction* act )
{
  if( !act )
    return;

  Settings::instance().setOverwriteExistingFiles( mp_actOverwriteExistingFile->isChecked() );
  Settings::instance().setAutomaticFileName( mp_actGenerateAutomaticFilename->isChecked() );
  Settings::instance().save();
}
