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

#include "AudioManager.h"
#include "Core.h"
#include "BeeApplication.h"
#include "BeeUtils.h"
#include "ChatManager.h"
#include "EmoticonManager.h"
#include "FileDialog.h"
#include "FileShare.h"
#include "GuiAddUser.h"
#include "GuiAskPassword.h"
#include "GuiChat.h"
#include "GuiChatList.h"
#include "GuiCreateGroup.h"
#include "GuiCreateMessage.h"
#include "GuiConfig.h"
#include "GuiEditVCard.h"
#include "GuiFileSharing.h"
#include "GuiFloatingChat.h"
#include "GuiGroupList.h"
#include "GuiHome.h"
#include "GuiLanguage.h"
#include "GuiLog.h"
#include "GuiNetworkTest.h"
#include "GuiPluginManager.h"
#include "GuiRefusedChat.h"
#include "GuiSavedChat.h"
#include "GuiSavedChatList.h"
#include "GuiScreenShot.h"
#include "GuiNetwork.h"
#include "GuiShareBox.h"
#ifdef BEEBEEP_USE_SHAREDESKTOP
  #include "GuiShareDesktop.h"
  #include "ImageOptimizer.h"
#endif
#include "GuiShareLocal.h"
#include "GuiShareNetwork.h"
#include "GuiShortcut.h"
#include "GuiSystemTray.h"
#include "GuiFileTransfer.h"
#include "GuiUserList.h"
#include "GuiMain.h"
#include "GuiVCard.h"
#ifdef BEEBEEP_USE_WEBENGINE
  #include "GuiWebView.h"
#endif
#include "GuiWizard.h"
#include "GuiWorkgroups.h"
#include "IconManager.h"
#include "NetworkManager.h"
#include "PluginManager.h"
#include "Protocol.h"
#include "MessageManager.h"
#include "Settings.h"
#include "ShortcutManager.h"
#include "SpellChecker.h"
#include "UserManager.h"
#ifdef BEEBEEP_USE_VOICE_CHAT
  #include "VoicePlayer.h"
  #include "GuiRecordVoiceMessageSettings.h"
#endif
#ifdef Q_OS_WIN
  #include <Windows.h>
#endif


GuiMain::GuiMain( QWidget *parent )
 : QMainWindow( parent ), m_floatingChats()
{
  setObjectName( "GuiMainWindow" );
  setWindowIcon( IconManager::instance().icon( "beebeep.png" ) );

  // Create a status bar before the actions and the menu
  (void) statusBar();

  mp_tabMain = new QTabWidget( this );
  mp_tabMain->setObjectName( "GuiTabMain" );
  mp_tabMain->setTabPosition( QTabWidget::South );
  setCentralWidget( mp_tabMain );

  mp_fileSharing = Q_NULLPTR;
  mp_screenShot = Q_NULLPTR;
  mp_log = Q_NULLPTR;
  mp_networkTest = Q_NULLPTR;
  m_unreadActivities = 0;

  mp_barMain = addToolBar( tr( "Show the main tool bar" ) );
  mp_barMain->setObjectName( "GuiMainToolBar" );
  mp_barMain->setIconSize( Settings::instance().mainBarIconSize() );
  mp_barMain->toggleViewAction()->setVisible( false );

  mp_trayIcon = new GuiSystemTray( this );

  m_lastUserStatus = User::Online;
  m_forceShutdown = false;
  m_userSelectOffline = false;
  m_prevActivatedState = true;
  m_coreIsConnecting = false;
  m_changeTabToUserListOnFirstConnected = false;

  m_useFusionStyle = false;

  createActions();
  createMainWidgets();
  createMenus();
  createToolAndMenuBars();
  updadePluginMenu();

  connect( mp_tabMain, SIGNAL( currentChanged( int ) ), this, SLOT( onMainTabChanged( int ) ) );

  connect( beeCore, SIGNAL( connected() ), this, SLOT( onCoreConnected() ) );
  connect( beeCore, SIGNAL( disconnected() ), this, SLOT( onCoreDisconnected() ) );
  connect( beeCore, SIGNAL( newChatMessage( const Chat&, const ChatMessage& ) ), this, SLOT( onNewChatMessage( const Chat&, const ChatMessage& ) ) );
  connect( beeCore, SIGNAL( fileDownloadRequest( const User&, const FileInfo& ) ), this, SLOT( downloadFile( const User&, const FileInfo& ) ) );
  connect( beeCore, SIGNAL( folderDownloadRequest( const User&, const QString&, const QList<FileInfo>& ) ), this, SLOT( downloadFolder( const User&, const QString&, const QList<FileInfo>& ) ) );
  connect( beeCore, SIGNAL( userChanged( const User& ) ), this, SLOT( onUserChanged( const User& ) ) );
  connect( beeCore, SIGNAL( userRemoved( const User& ) ), this, SLOT( onUserRemoved( const User& ) ) );
  connect( beeCore, SIGNAL( userIsWriting( const User&, VNumber ) ), this, SLOT( showWritingUser( const User&, VNumber ) ) );
  connect( beeCore, SIGNAL( fileTransferProgress( VNumber, const User&, const FileInfo&, FileSizeType, qint64 ) ), this, SLOT( onFileTransferProgress( VNumber, const User&, const FileInfo&, FileSizeType, qint64 ) ) );
  connect( beeCore, SIGNAL( fileTransferMessage( VNumber, const User&, const FileInfo&, const QString&, FileTransferPeer::TransferState ) ), this, SLOT( onFileTransferMessage( VNumber, const User&, const FileInfo&, const QString&, FileTransferPeer::TransferState ) ) );
  connect( beeCore, SIGNAL( fileShareAvailable( const User& ) ), this, SLOT( showSharesForUser( const User& ) ) );
  connect( beeCore, SIGNAL( chatChanged( const Chat& ) ), this, SLOT( onChatChanged( const Chat& ) ) );
  connect( beeCore, SIGNAL( chatRemoved( const Chat& ) ), this, SLOT( onChatRemoved( const Chat& ) ) );
  connect( beeCore, SIGNAL( savedChatListAvailable() ), this, SLOT( loadSavedChatsCompleted() ) );
  connect( beeCore, SIGNAL( userConnectionStatusChanged( const User& ) ), this, SLOT( showConnectionStatusChanged( const User& ) ) );
  connect( beeCore, SIGNAL( networkInterfaceIsDown() ), this, SLOT( onNetworkInterfaceDown() ) );
  connect( beeCore, SIGNAL( networkInterfaceIsUp() ), this, SLOT( onNetworkInterfaceUp() ) );
  connect( beeCore, SIGNAL( chatReadByUser( const Chat&, const User& ) ), this, SLOT( onChatReadByUser( const Chat&, const User& ) ) );
  connect( beeCore, SIGNAL( localUserIsBuzzedBy( const User&, VNumber ) ), this, SLOT( showBuzzFromUser( const User&, VNumber ) ) );
  connect( beeCore, SIGNAL( helpRequestFrom( const User&, VNumber ) ), this, SLOT( showHelpRequestFromUser( const User&, VNumber ) ) );
  connect( beeCore, SIGNAL( helpAnswerFrom( const User&, VNumber ) ), this, SLOT( showHelpAnswerFromUser( const User&, VNumber ) ) );
  connect( beeCore, SIGNAL( newSystemStatusMessage( const QString&, int ) ), this, SLOT( showMessage( const QString&, int ) ) );
  connect( beeCore, SIGNAL( newsAvailable( const QString& ) ), this, SLOT( onNewsAvailable( const QString& ) ) );
  connect( beeCore, SIGNAL( offlineMessageSentToUser( const User& ) ), this, SLOT( updateUser( const User& ) ) );
#ifdef BEEBEEP_USE_SHAREDESKTOP
  connect( beeCore, SIGNAL( shareDesktopImageAvailable( const User&, const QImage&, const QString&, QRgb ) ), this, SLOT( onShareDesktopImageAvailable( const User&, const QImage&, const QString&, QRgb ) ) );
  connect( beeCore, SIGNAL( shareDesktopUpdate( const User& ) ), this, SLOT( onShareDesktopUpdate( const User& ) ) );
#endif
#ifdef BEEBEEP_USE_MULTICAST_DNS
  connect( beeCore, SIGNAL( multicastDnsChanged() ), this, SLOT( showDefaultServerPortInMenu() ) );
#endif
#ifdef BEEBEEP_USE_VOICE_CHAT
  connect( beeCore->voicePlayer(), SIGNAL( openWithExternalPlayer( const QUrl&, VNumber ) ), this, SLOT( openUrlFromChat( const QUrl&, VNumber ) ) );
#endif

  connect( mp_fileTransfer, SIGNAL( transferCanceled( VNumber ) ), beeCore, SLOT( cancelFileTransfer( VNumber ) ) );
  connect( mp_fileTransfer, SIGNAL( transferPaused( VNumber ) ), beeCore, SLOT( pauseFileTransfer( VNumber ) ) );
  connect( mp_fileTransfer, SIGNAL( openFileCompleted( const QUrl& ) ), this, SLOT( openUrl( const QUrl& ) ) );
  connect( mp_fileTransfer, SIGNAL( resumeTransfer( VNumber, const FileInfo& ) ), this, SLOT( resumeFileTransfer( VNumber, const FileInfo& ) ) );

  connect( mp_userList, SIGNAL( chatSelected( VNumber ) ), this, SLOT( showChat( VNumber ) ) );
  connect( mp_userList, SIGNAL( userSelected( VNumber ) ), this, SLOT( checkUserSelected( VNumber ) ) );
  connect( mp_userList, SIGNAL( showVCardRequest( VNumber ) ), this, SLOT( showVCard( VNumber ) ) );
  connect( mp_userList, SIGNAL( sendFilesToChatRequest( VNumber, const QStringList& ) ), this, SLOT( sendFilesFromChat( VNumber, const QStringList& ) ) );

  connect( mp_groupList, SIGNAL( openChatForGroupRequest( VNumber ) ), this, SLOT( showChat( VNumber ) ) );
  connect( mp_groupList, SIGNAL( createGroupRequest() ), this, SLOT( createGroupChat() ) );
  connect( mp_groupList, SIGNAL( editGroupRequest( VNumber ) ), this, SLOT( editGroupChat( VNumber ) ) );
  connect( mp_groupList, SIGNAL( showVCardRequest( VNumber ) ), this, SLOT( showVCard( VNumber ) ) );

  connect( mp_chatList, SIGNAL( chatSelected( VNumber ) ), this, SLOT( showChat( VNumber ) ) );
  connect( mp_chatList, SIGNAL( chatToClear( VNumber ) ), this, SLOT( clearChat( VNumber ) ) );
  connect( mp_chatList, SIGNAL( chatToEdit( VNumber ) ), this, SLOT( editGroupChat( VNumber ) ) );
  connect( mp_chatList, SIGNAL( createNewChatRequest() ), this, SLOT( createGroupChat() ) );
  connect( mp_chatList, SIGNAL( createNewMessageRequest() ), this, SLOT( createMessage() ) );
  connect( mp_chatList, SIGNAL( hideEmptyChatsRequest() ), this, SLOT( onHideEmptyChatsRequest() ) );

  connect( mp_trayIcon, SIGNAL( activated( QSystemTrayIcon::ActivationReason ) ), this, SLOT( trayIconClicked( QSystemTrayIcon::ActivationReason ) ) );
  connect( mp_trayIcon, SIGNAL( messageClicked() ), this, SLOT( trayMessageClicked() ) );

  connect( mp_savedChatList, SIGNAL( savedChatSelected( const QString& ) ), this, SLOT( showSavedChatSelected( const QString& ) ) );
  connect( mp_savedChatList, SIGNAL( savedChatRemoved( const QString& ) ), this, SLOT( removeSavedChat( const QString& ) ) );
  connect( mp_savedChatList, SIGNAL( savedChatLinkRequest( const QString& ) ), this, SLOT( linkSavedChat( const QString& ) ) );

  connect( statusBar(), SIGNAL( messageChanged( const QString& ) ), this, SLOT( onStatusBarMessageChanged( const QString& ) ) );

  initShortcuts();
  initGuiItems();
  updateShortcuts();
}

void GuiMain::initShortcuts()
{
  mp_scMinimizeAllChats = new QShortcut( this );
  mp_scMinimizeAllChats->setContext( Qt::ApplicationShortcut );
  connect( mp_scMinimizeAllChats, SIGNAL( activated() ), this, SLOT( minimizeAllChats() ) );
#ifdef BEEBEEP_USE_QXT
  mp_scShowAllChats = new QxtGlobalShortcut( this );
  connect( mp_scShowAllChats, SIGNAL( activated() ), this, SLOT( showAllChats() ) );
  mp_scSendHelpMessage = new QxtGlobalShortcut( this );
  connect( mp_scSendHelpMessage, SIGNAL( activated() ), this, SLOT( sendHelpMessage() ) );
  mp_scSelectFirstChat = new QxtGlobalShortcut( this );
  connect( mp_scSelectFirstChat, SIGNAL( activated() ), this, SLOT( selectFirstChat() ) );
#endif
  mp_scShowNextUnreadMessage = new QShortcut( this );
  mp_scShowNextUnreadMessage->setContext( Qt::ApplicationShortcut );
  connect( mp_scShowNextUnreadMessage, SIGNAL( activated() ), this, SLOT( showNextChat() ) );
}

void GuiMain::setupChatConnections( GuiChat* gui_chat )
{
  connect( gui_chat, SIGNAL( newMessage( VNumber, const QString&, bool ) ), this, SLOT( sendMessage( VNumber, const QString&, bool ) ) );
  connect( gui_chat, SIGNAL( writing( VNumber ) ), beeCore, SLOT( sendWritingMessage( VNumber ) ) );
  connect( gui_chat, SIGNAL( nextChat() ), this, SLOT( showNextChat() ) );
  connect( gui_chat, SIGNAL( openUrl( const QUrl&, VNumber ) ), this, SLOT( openUrlFromChat( const QUrl&, VNumber ) ) );
  connect( gui_chat, SIGNAL( sendFileFromChatRequest( VNumber, const QString& ) ), this, SLOT( sendFileFromChat( VNumber, const QString& ) ) );
  connect( gui_chat, SIGNAL( sendFilesFromChatRequest( VNumber, const QStringList& ) ), this, SLOT( sendFilesFromChat( VNumber, const QStringList& ) ) );
  connect( gui_chat, SIGNAL( editGroupRequest( VNumber ) ), this, SLOT( editGroupChat( VNumber ) ) );
  connect( gui_chat, SIGNAL( chatToClear( VNumber ) ), this, SLOT( clearChat( VNumber ) ) );
  connect( gui_chat, SIGNAL( clearSystemMessagesRequestFromChat( VNumber ) ), this, SLOT( clearSystemMessagesInChat( VNumber ) ) );
  connect( gui_chat, SIGNAL( showChatMenuRequest() ), this, SLOT( showChatSettingsMenu() ) );
#ifdef BEEBEEP_USE_SHAREDESKTOP
  connect( gui_chat, SIGNAL( shareDesktopToChatRequest( VNumber, bool ) ), this, SLOT( onShareDesktopRequestFromChat( VNumber, bool ) ) );
  connect( gui_chat, SIGNAL( screenshotToChatRequest( VNumber ) ), this, SLOT( sendScreenshotToChat( VNumber ) ) );
#endif
}

void GuiMain::checkWindowFlagsAndShow()
{
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

  if( mp_tabMain->currentWidget() != mp_home )
    mp_tabMain->setCurrentWidget( mp_home );

  if( !Settings::instance().enableMaximizeButton() )
    setWindowFlags( windowFlags() & ~Qt::WindowMaximizeButtonHint );

  Bee::setWindowStaysOnTop( this, Settings::instance().stayOnTop() );

  if( !isVisible() )
    show();

  if( Settings::instance().resetGeometryAtStartup() || Settings::instance().guiGeometry().isEmpty() )
    resetGeometryAndState();
  else
    restoreGeometryAndState();

  checkViewActions();

#ifdef Q_OS_WIN
  beeApp->setSessionNotificationForWindow( reinterpret_cast<HWND>( winId() ) );
#endif

  if( Settings::instance().loadOnTrayAtStartup() && QSystemTrayIcon::isSystemTrayAvailable() )
  {
    QMetaObject::invokeMethod( this, "hideToTrayIcon", Qt::QueuedConnection );
    return;
  }

  if( Settings::instance().showMinimizedAtStartup() )
    QMetaObject::invokeMethod( this, "showMinimized", Qt::QueuedConnection );
}

void GuiMain::showUp()
{
  Bee::showUp( this );
}

void GuiMain::raiseOnTop()
{
  Bee::raiseOnTop( this );
}

void GuiMain::updateWindowTitle()
{
  setWindowTitle( QString( "%1 - %2" ).arg( Bee::userNameToShow( Settings::instance().localUser(), false ), Settings::instance().programName() ) );
}

static QString RemoveMenuStringFromTooltip( const QString& s )
{
  return s.isEmpty() ? QString( "" ) : s.split( QString( "\n" ), QString::KeepEmptyParts ).first();
}

void GuiMain::updateTabTitles()
{
  int tab_index = mp_tabMain->indexOf( mp_home );
  int current_value = m_unreadActivities;
  mp_tabMain->setTabText( tab_index, current_value > 0 ? QString::number( current_value ) : "" );
  if( current_value > 0 )
    mp_tabMain->setTabToolTip( tab_index, QString( "%1: %2 %3" ).arg( RemoveMenuStringFromTooltip( mp_home->mainToolTip() ) ).arg( current_value ).arg( tr( "news" ) ) );
  else
    mp_tabMain->setTabToolTip( tab_index, RemoveMenuStringFromTooltip( mp_home->mainToolTip() ) );

  tab_index = mp_tabMain->indexOf( mp_userList );
  current_value = beeCore->connectedUsers();
  int other_value = UserManager::instance().userList().size();
  mp_tabMain->setTabText( tab_index, current_value > 0 ? QString::number( current_value ) : (other_value > 0 ? QString::number( other_value ) : "" ) );
  if( current_value > 0 )
    mp_tabMain->setTabToolTip( tab_index, QString( "%1: %2 %3" ).arg( RemoveMenuStringFromTooltip( mp_userList->mainToolTip() ) ).arg( current_value ).arg( tr( "connected" ) ) );
  else
    mp_tabMain->setTabToolTip( tab_index, RemoveMenuStringFromTooltip( mp_userList->mainToolTip() ) );

  tab_index = mp_tabMain->indexOf( mp_chatList );
  current_value = ChatManager::instance().countNotEmptyChats( false );
  mp_tabMain->setTabText( tab_index, current_value > 0 ? QString::number( current_value ) : "" );

  tab_index = mp_tabMain->indexOf( mp_groupList );
  current_value = ChatManager::instance().countGroupChats();
  mp_tabMain->setTabText( tab_index, current_value > 0 ? QString::number( current_value ) : "" );

  tab_index = mp_tabMain->indexOf( mp_savedChatList );
  current_value = ChatManager::instance().constHistoryMap().size();
  mp_tabMain->setTabText( tab_index, current_value > 0 ? QString::number( current_value ) : "" );
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
}

void GuiMain::closeEvent( QCloseEvent* e )
{
  if( !m_forceShutdown )
  {
    if( Settings::instance().closeMinimizeInTray() && QSystemTrayIcon::isSystemTrayAvailable() )
    {
      QTimer::singleShot( 0, this, SLOT( hideToTrayIcon() ) );
      e->ignore();
      return;
    }

    if( Settings::instance().promptOnCloseEvent() )
    {
      if( QMessageBox::question( this, Settings::instance().programName(), tr( "Do you want to quit %1?" ).arg( Settings::instance().programName() ),
                                 tr( "Yes" ), tr( "No" ), QString(), 1, 1 ) == 1 )
      {
        e->ignore();
        return;
      }
    }
  }

  if( beeCore->isConnected() )
    beeCore->stop();

  if( Settings::instance().saveGeometryOnExit() )
#if QT_VERSION == 0x050906
  {
    if( isVisible() )
    {
      Settings::instance().setGuiGeometry( saveGeometry() );
      Settings::instance().save();
    }
  }
#else
    saveGeometryAndState();
#endif

  QSettings* sets = Settings::instance().objectSettings();
  sets->deleteLater();

  if( !m_forceShutdown )
  {
    if( !sets->isWritable() )
    {
      if( QMessageBox::warning( this, Settings::instance().programName(),
                              QString( "%1<br>%2<br>%3<br>%4<br>%5" ).arg( tr( "<b>Settings can not be saved</b>. Path:" ) )
                                                                     .arg( sets->fileName() )
                                                                     .arg( tr( "<b>is not writable</b> by user:" ) )
                                                                     .arg( Settings::instance().localUser().accountName() )
                                                                     .arg( tr( "Do you want to close anyway?" ) ),
                              tr( "Yes" ), tr( "No" ), QString(), 1, 1 ) == 1 )
      {
        e->ignore();
        return;
      }
    }

    if( Settings::instance().chatAutoSave() )
    {
      if( !MessageManager::instance().chatMessageCanBeSaved() )
      {
        if( QMessageBox::warning( this, Settings::instance().programName(),
                              QString( "%1<br>%2<br>%3<br>%4<br>%5" ).arg( tr( "<b>Chat messages can not be saved</b>. Path:" ) )
                                                                     .arg( Settings::instance().savedChatsFilePath() )
                                                                     .arg( tr( "<b>is not writable</b> by user:" ) )
                                                                     .arg( Settings::instance().localUser().accountName() )
                                                                     .arg( tr( "Do you want to close anyway?" ) ),
                              tr( "Yes" ), tr( "No" ), QString(), 1, 1 ) == 1 )
        {
          e->ignore();
          return;
        }
      }
    }
  }

  if( mp_fileSharing )
    mp_fileSharing->close();

  if( mp_screenShot )
    mp_screenShot->close();

  if( mp_networkTest )
    mp_networkTest->close();

#ifdef BEEBEEP_USE_WEBENGINE
  if( mp_webView )
    mp_webView->close();
#endif

#ifdef BEEBEEP_USE_VOICE_CHAT
  if( !beeCore->voicePlayer()->isStopped() )
    beeCore->voicePlayer()->stop();
#endif

  if( mp_log )
    mp_log->close();

  foreach( QWidget* w, qApp->allWidgets() )
  {
    GuiSavedChat* gsv = qobject_cast<GuiSavedChat*>( w );
    if( gsv )
      gsv->close();
  }

  foreach( GuiFloatingChat* fl_chat, m_floatingChats )
    fl_chat->close();

  if( mp_menuMain && mp_menuMain->isVisible() )
    mp_menuMain->close();
  if( mp_menuSettings && mp_menuSettings->isVisible() )
    mp_menuSettings->close();
  if( mp_menuInfo && mp_menuInfo->isVisible() )
    mp_menuInfo->close();
  if( mp_menuStatus && mp_menuStatus->isVisible() )
    mp_menuStatus->close();
  if( mp_menuUsersSettings && mp_menuUsersSettings->isVisible() )
    mp_menuUsersSettings->close();

  mp_trayIcon->hide();

  if( mp_dockFileTransfers->isFloating() && mp_dockFileTransfers->isVisible() )
    mp_dockFileTransfers->hide();

#ifdef Q_OS_WIN
  beeApp->resetSessionNotificationForWindow();
#endif
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
  {
    showChat( c.id() );
    GuiFloatingChat* fl_chat = floatingChat( c.id() );
    if( fl_chat && !fl_chat->isActiveWindow() )
      fl_chat->setFocusInChat();
  }
  else
    showMessage( tr( "No new message available" ), 5000 );
}

void GuiMain::forceShutdown()
{
  qDebug() << "Shutdown...";
  m_forceShutdown = true;
  if( beeCore->isConnected() )
    beeCore->stop();
  close();
}

void GuiMain::onWakeUpRequest()
{
#ifdef BEEBEEP_DEBUG
  qDebug() << "Main window wakes up from sleep";
#endif
  initGuiItems();
}

void GuiMain::onSleepRequest()
{
#ifdef BEEBEEP_DEBUG
  qDebug() << "Main window goes to sleep";
#endif
  if( beeCore->isConnected() )
    stopCore();
}

void GuiMain::onCoreConnected()
{
  m_coreIsConnecting = false;
  initGuiItems();
#ifdef BEEBEEP_USE_WEBENGINE
  QTimer::singleShot( 0, mp_webView, SLOT( loadNews() ) );
#endif
}

void GuiMain::onCoreDisconnected()
{
  if( m_coreIsConnecting )
    m_coreIsConnecting = false;
  initGuiItems();
}

void GuiMain::startCore()
{
  if( beeCore->isConnected() || m_coreIsConnecting )
    return;

  mp_home->resetNews();
  m_coreIsConnecting = true;
  m_userSelectOffline = false;

  if( Settings::instance().askChangeUserAtStartup() )
  {
    if( !showWizard() )
    {
      m_coreIsConnecting = false;
      return;
    }
  }

  if( Settings::instance().firstTime() )
  {
    Settings::instance().setFirstTime( false );
    Settings::instance().setAskChangeUserAtStartup( false );
  }

  if( Settings::instance().askPassword() )
  {
    if( !promptConnectionPassword() )
    {
      m_coreIsConnecting = false;
      return;
    }
  }

  if( beeCore->start() )
  {
    if( Settings::instance().showChatsOnConnection() )
    {
      mp_tabMain->setCurrentWidget( mp_chatList );
      m_changeTabToUserListOnFirstConnected = false;
    }
    else if( Settings::instance().showUsersOnConnection() )
    {
      mp_tabMain->setCurrentWidget( mp_userList );
      m_changeTabToUserListOnFirstConnected = false;
    }
    else
      m_changeTabToUserListOnFirstConnected = true;
  }
  else
  {
    m_coreIsConnecting = false;
    QMetaObject::invokeMethod( beeCore, "checkNetworkInterface", Qt::QueuedConnection );
  }
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
  if( beeCore->isConnected() )
    showRestartConnectionAlertMessage();
  return true;
}

void GuiMain::disconnectFromNetwork()
{
  if( isFileTransferInProgress() )
    return;

  m_userSelectOffline = true;

  if( mp_tabMain->currentWidget() != mp_home )
    mp_tabMain->setCurrentWidget( mp_home );

#ifdef BEEBEEP_USE_SHAREDESKTOP
  foreach( GuiShareDesktop* gsd, m_desktops )
    gsd->close();
#endif

  beeCore->stop();
}

void GuiMain::stopCore()
{
  if( isFileTransferInProgress() )
    return;

  if( mp_tabMain->currentWidget() != mp_home )
    mp_tabMain->setCurrentWidget( mp_home );

#ifdef BEEBEEP_USE_SHAREDESKTOP
  foreach( GuiShareDesktop* gsd, m_desktops )
    gsd->close();
#endif

  beeCore->stop();
}

void GuiMain::restartCore()
{
  if( !beeCore->isConnected() && !m_coreIsConnecting )
  {
    startCore();
    return;
  }

  if( isFileTransferInProgress() )
    return;

#ifdef BEEBEEP_USE_SHAREDESKTOP
  foreach( GuiShareDesktop* gsd, m_desktops )
    gsd->close();
#endif

  mp_home->resetNews();
  m_coreIsConnecting = true;
  m_userSelectOffline = false;
  beeCore->restart();
}

bool GuiMain::isFileTransferInProgress()
{
  if( m_forceShutdown )
    return false;

  if( !NetworkManager::instance().isMainInterfaceUp() )
    return false;

  if( beeApp->isInSleepMode() )
    return false;

  if( beeCore->hasFileTransferInProgress() )
  {
    if( QMessageBox::warning( this, Settings::instance().programName(),
        tr( "There are still files that have not been transferred and will be interrupted. Do you want to disconnect anyway?" ),
        tr( "Yes" ), tr( "No" ), QString(), 1, 1 ) == 1 )
      return true;
  }
  return false;
}

void GuiMain::initGuiItems()
{
  mp_userList->updateUsers();
  mp_chatList->updateChats();
  mp_groupList->updateGroups();
  mp_savedChatList->updateSavedChats();
  updateStatusIcon();
  updateNewMessageAction();
  checkViewActions();
}

void GuiMain::checkViewActions()
{
  bool is_connected = beeCore->isConnected();
  int connected_users = beeCore->connectedUsers();
  bool file_transfer_is_active = beeCore->isFileTransferActive();
  bool helper_found = beeCore->hasHelper();

  mp_actConnect->setEnabled( !m_coreIsConnecting && !is_connected );
  mp_actDisconnect->setEnabled( is_connected );
  mp_actBroadcast->setEnabled( is_connected );
  mp_actCreateMessage->setEnabled( is_connected && !Settings::instance().disableCreateMessage() );
  mp_actCreateGroupChat->setEnabled( UserManager::instance().userList().size() > 1 );
  mp_actViewFileTransfer->setEnabled( Settings::instance().enableFileTransfer() );
  mp_actViewFileSharing->setEnabled( file_transfer_is_active && Settings::instance().enableFileSharing() && !Settings::instance().disableFileSharing() );
  if( mp_menuFileTransferSettings->isEnabled() )
  {
    foreach( QAction* act, mp_menuFileTransferSettings->actions() )
    {
      if( act->data().toInt() == 12 )
        continue;
      act->setEnabled( Settings::instance().enableFileTransfer() );
    }
  }
  mp_actEnableFileSharing->setEnabled( Settings::instance().enableFileTransfer() && !Settings::instance().disableFileSharing() );

#ifdef BEEBEEP_USE_VOICE_CHAT
  mp_menuVoiceMessage->setEnabled( Settings::instance().enableFileTransfer() );
#endif

  showDefaultServerPortInMenu();

  if( !m_floatingChats.isEmpty() )
  {
    foreach( GuiFloatingChat* fl_chat, m_floatingChats )
      fl_chat->updateActions( is_connected, connected_users, file_transfer_is_active );
  }

  if( mp_fileSharing )
  {
    if( mp_actViewFileSharing->isEnabled() )
      mp_fileSharing->checkViewActions();
    else
      mp_fileSharing->close();
  }

  mp_actHelpRequest->setEnabled( is_connected && connected_users > 0 && helper_found );

  updateWindowTitle();
  updateTabTitles();
}

void GuiMain::showAbout()
{
#ifdef Q_OS_MAC
  QMessageBox::about( Q_NULLPTR, Settings::instance().programName(),
#else
  QMessageBox::about( this, Settings::instance().programName(),
#endif
                      QString( "<b>%1</b> - Free Office Messenger<br><br>%2<br>%3 %4 %5<br><br>%6 %7<br>%8<br>" )
                        .arg( Settings::instance().programName() )
                        .arg( Settings::instance().isDevelopmentVersion() ? tr( "Development version") : tr( "Version" ) )
                        .arg( Settings::instance().version( true, true, true ) )
                        .arg( tr( "for" ) )
                        .arg( Settings::instance().operatingSystem( true ) )
                        .arg( tr( "developed by" ) )
                        .arg( QString( "<a href='%1'>Marco Mastroddi</a>" ).arg( Settings::instance().developerWebSite() ) )
                        .arg( QString( "e-mail: <a href='mailto://marco.mastroddi@gmail.com'>marco.mastroddi@gmail.com</a><br>web: <a href='%1'>www.beebeep.net</a>" ).arg( Settings::instance().officialWebSite() ) )
                      );
}

void GuiMain::showLicense()
{
  QString license_txt = QLatin1String(
  "BeeBEEP Copyright (C) 2010-2021 Marco Mastroddi <br><br>"
  "BeeBEEP is free software: you can redistribute it and/or modify "
  "it under the terms of the GNU General Public License as published "
  "by the Free Software Foundation, either version 3 of the License "
  "or (at your option) any later version. <br><br>"
  "BeeBEEP is distributed in the hope that it will be useful, "
  "but WITHOUT ANY WARRANTY; without even the implied warranty "
  "of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. <br><br>"
  "See the GNU General Public License for more details. <br><br>" );
#ifdef Q_OS_MAC
  QMessageBox::about( Q_NULLPTR, Settings::instance().programName(), license_txt );
#else
  QMessageBox::about( this, Settings::instance().programName(), license_txt );
#endif
}

void GuiMain::showAboutQt()
{
#ifdef Q_OS_MAC
  QMessageBox::aboutQt( Q_NULLPTR, Settings::instance().programName() );
#else
  QMessageBox::aboutQt( this, Settings::instance().programName() );
#endif
}

void GuiMain::createActions()
{
  mp_actConnect = new QAction( IconManager::instance().icon( "connection.png" ), tr( "Connect" ), this );
  connect( mp_actConnect, SIGNAL( triggered() ), this, SLOT( startCore() ) );

  mp_actDisconnect = new QAction( IconManager::instance().icon( "disconnection.png" ), tr( "Disconnect" ), this );
  connect( mp_actDisconnect, SIGNAL( triggered() ), this, SLOT( disconnectFromNetwork() ) );

  mp_actBroadcast = new QAction( IconManager::instance().icon( "broadcast.png" ), tr( "Search users" ), this );
  connect( mp_actBroadcast, SIGNAL( triggered() ), this, SLOT( sendBroadcastMessage() ) );

  mp_actConfigureNetwork = new QAction( IconManager::instance().icon( "network.png"), tr( "Configure network..."), this );
  connect( mp_actConfigureNetwork, SIGNAL( triggered() ), this, SLOT( searchUsers() ) );

  mp_actQuit = new QAction( IconManager::instance().icon( "quit.png" ), tr( "Quit" ), this );
  mp_actQuit->setShortcuts( QKeySequence::Quit );
  mp_actQuit->setMenuRole( QAction::QuitRole );
  connect( mp_actQuit, SIGNAL( triggered() ), this, SLOT( forceShutdown() ) );

  mp_actVCard = new QAction( IconManager::instance().icon( "profile-edit.png"), tr( "Edit your profile..." ), this );
  connect( mp_actVCard, SIGNAL( triggered() ), this, SLOT( changeVCard() ) );

  mp_actAbout = new QAction( IconManager::instance().icon( "beebeep.png" ), tr( "About %1..." ).arg( Settings::instance().programName() ), this );
  mp_actAbout->setMenuRole( QAction::AboutRole );
  connect( mp_actAbout, SIGNAL( triggered() ), this, SLOT( showAbout() ) );

  mp_actCreateGroupChat = new QAction( IconManager::instance().icon( "group-create.png" ), tr( "Create new group chat" ), this );
  connect( mp_actCreateGroupChat, SIGNAL( triggered() ), this, SLOT( createGroupChat() ) );

  mp_actViewNewMessage = new QAction( IconManager::instance().icon( "beebeep-message.png" ), tr( "Show new message" ), this );
  connect( mp_actViewNewMessage, SIGNAL( triggered() ), this, SLOT( showNextChat() ) );

  mp_actViewFileSharing = new QAction( IconManager::instance().icon( "file-sharing.png" ), tr( "Show file sharing window" ), this );
  connect( mp_actViewFileSharing, SIGNAL( triggered() ), this, SLOT( showFileSharingWindow() ) );

  mp_actViewLog = new QAction( IconManager::instance().icon( "log.png" ), tr( "Show the %1 log" ).arg( Settings::instance().programName() )+QString("..."), this );
  connect( mp_actViewLog, SIGNAL( triggered() ), this, SLOT( showLogWindow() ) );

  mp_actViewScreenShot = new QAction( IconManager::instance().icon( "screenshot.png" ), tr( "Make a screenshot" ), this );
  connect( mp_actViewScreenShot, SIGNAL( triggered() ), this, SLOT( showScreenShotWindow() ) );

  mp_actCreateMessage = new QAction( IconManager::instance().icon( "message-create.png" ), tr( "Write a message" ), this );
  connect( mp_actCreateMessage, SIGNAL( triggered() ), this, SLOT( createMessage() ) );
  if( Settings::instance().disableCreateMessage() )
    mp_actCreateMessage->setToolTip( tr( "The option has been disabled by your system administrator.") );

  mp_actHelpRequest = new QAction( IconManager::instance().icon( "help.png" ), tr( "Ask for help" ), this );
  connect( mp_actHelpRequest, SIGNAL( triggered() ), this, SLOT( sendHelpMessage() ) );

#ifdef BEEBEEP_USE_WEBENGINE
  mp_actWebView = new QAction( IconManager::instance().icon( "network.png" ), tr( "News" ), this );
  connect( mp_actWebView, SIGNAL( triggered() ), this, SLOT( showWebView() ) );
  mp_actWebView->setDisabled( true );
#endif
}

void GuiMain::createMenus()
{
  /* Plugins Menu */
  mp_menuPlugins = new QMenu( tr( "Plugins" ) + QString( "..." ), this );
  mp_menuPlugins->setIcon( IconManager::instance().icon( "plugin.png" ) );

  /* Main Menu */
  mp_menuMain = new QMenu( tr( "Main" ), this );
  mp_menuMain->addAction( mp_actConnect );
  mp_menuMain->addAction( mp_actDisconnect );
  mp_menuMain->addSeparator();
  mp_menuMain->addAction( mp_actBroadcast );
  mp_menuMain->addSeparator();
  mp_menuMain->addAction( mp_actHelpRequest );
  mp_menuMain->addSeparator();
  mp_menuMain->addAction( mp_actVCard );
  mp_menuMain->addSeparator();
  if( Settings::instance().resourceFolder() != Settings::instance().dataFolder() )
    mp_menuMain->addAction( IconManager::instance().icon( "resource-folder.png" ), tr( "Open your resource folder" )+QString("..."), this, SLOT( openResourceFolder() ) );
  mp_menuMain->addAction( IconManager::instance().icon( "data-folder.png" ), tr( "Open your data folder" )+QString("..."), this, SLOT( openDataFolder() ) );
  mp_menuMain->addAction( IconManager::instance().icon( "download-folder.png" ), tr( "Open your download folder" )+QString("..."), this, SLOT( openDownloadFolder() ) );
  mp_menuMain->addSeparator();
  mp_menuMain->addAction( IconManager::instance().icon( "network-test.png" ), tr( "Test your network" )+QString("..."), this, SLOT( showNetworkTest() ) );
  mp_menuMain->addSeparator();
  mp_menuMain->addAction( mp_actViewLog );
#ifdef BEEBEEP_USE_WEBENGINE
  mp_menuMain->addSeparator();
  mp_menuMain->addAction( mp_actWebView );
#endif
  mp_menuMain->addSeparator();
  mp_menuMain->addMenu( mp_menuPlugins );
  mp_menuMain->addSeparator();
  mp_menuMain->addAction( mp_actQuit );

  QAction* act;
  /* Chat Menu */
  mp_menuChat = new QMenu( tr( "Chat" ), this );

  /* System Menu */
  mp_menuSettings = new QMenu( tr( "Settings" ), this );
  mp_menuInterfaceSettings = new QMenu( tr( "Interface" ), this );
  mp_menuInterfaceSettings->setIcon( IconManager::instance().icon( "interface.png" ) );
  mp_menuSettings->addMenu( mp_menuInterfaceSettings );
  act = mp_menuInterfaceSettings->addAction( tr( "Use the dark theme" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().useDarkStyle() );
  act->setData( 77 );
#if QT_VERSION < 0x050000
  act->setEnabled( false );
#endif
  act = mp_menuInterfaceSettings->addAction( tr( "Reset minimum width for applied style" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().resetMinimumWidthForStyle() );
  act->setData( 103 );
  mp_menuInterfaceSettings->addSeparator();
  act = mp_menuInterfaceSettings->addAction( tr( "Escape key minimize to tray icon" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().keyEscapeMinimizeInTray() );
  act->setData( 29 );
  act = mp_menuInterfaceSettings->addAction( tr( "Close button minimize to tray icon" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().closeMinimizeInTray() );
  act->setData( 11 );
#ifdef Q_OS_MAC
  // Close button on MacOSX must quit the app
  act->setDisabled( true );
#endif
  act = mp_menuInterfaceSettings->addAction( tr( "Enable maximize button" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().enableMaximizeButton() );
  act->setData( 28 );
  mp_menuInterfaceSettings->addSeparator();
  mp_menuInterfaceSettings->addAction( IconManager::instance().icon( "shortcut.png" ), tr( "Shortcuts" ) + QString( "..." ), this, SLOT( editShortcuts() ) );
  mp_menuInterfaceSettings->addAction( IconManager::instance().icon( "language.png" ), tr( "Select language" ) + QString( "..." ), this, SLOT( selectLanguage() ) );
  mp_menuInterfaceSettings->addAction( IconManager::instance().icon( "theme.png" ), tr( "Select icon theme" ) + QString( "..." ), this, SLOT( selectIconSourcePath() ) );
  mp_menuInterfaceSettings->addSeparator();
  mp_menuInterfaceSettings->addAction( IconManager::instance().icon( "update.png" ), tr( "Restore the colors to the default ones" ), this, SLOT( resetAllColors() ) );
  mp_menuInterfaceSettings->addAction( IconManager::instance().icon( "reset-window.png" ), tr( "Reset geometry of all windows" ), this, SLOT( askResetGeometryAndState() ) );

  mp_menuStartupSettings = new QMenu( tr( "On start" ), this );
  mp_menuStartupSettings->setIcon( IconManager::instance().icon( "settings-start.png" ) );
  mp_menuSettings->addMenu( mp_menuStartupSettings );
  act = mp_menuStartupSettings->addAction( tr( "Show minimized" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().showMinimizedAtStartup() );
  act->setData( 35 );
  act = mp_menuStartupSettings->addAction( tr( "Show only on system tray" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().loadOnTrayAtStartup() );
  act->setData( 24 );
  act = mp_menuStartupSettings->addAction( tr( "Reset window geometry" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().resetGeometryAtStartup() );
  act->setData( 26 );
  act = mp_menuStartupSettings->addAction( tr( "Check for new version" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().checkNewVersionAtStartup() );
  act->setData( 43 );
  mp_menuStartupSettings->addSeparator();
  act = mp_menuStartupSettings->addAction( IconManager::instance().icon( "timer.png" ), tr( "Delay first connection" ) + QString( "..." ), this, SLOT( settingsChanged() ) );
  act->setData( 65 );

  mp_menuCloseSettings = new QMenu( tr( "On close" ), this );
  mp_menuCloseSettings->setIcon( IconManager::instance().icon( "settings-close.png" ) );
  mp_menuSettings->addMenu( mp_menuCloseSettings );
  act = mp_menuCloseSettings->addAction( tr( "Prompt on quit when connected" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().promptOnCloseEvent() );
  act->setData( 36 );
  act = mp_menuCloseSettings->addAction( tr( "Save window's geometry" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().saveGeometryOnExit() );
  act->setData( 68 );

  mp_menuConnectionSettings = new QMenu( tr( "On connection" ), this );
  mp_menuConnectionSettings->setIcon( IconManager::instance().icon( "connection.png" ) );
  mp_menuSettings->addMenu( mp_menuConnectionSettings );
  act = mp_menuConnectionSettings->addAction( tr( "Show the user list" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().showUsersOnConnection() );
  act->setData( 69 );
  act = mp_menuConnectionSettings->addAction( tr( "Show the chat list" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().showChatsOnConnection() );
  act->setData( 79 );
  mp_menuConnectionSettings->addSeparator();
  act = mp_menuConnectionSettings->addAction( tr( "Prompts to change user" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().askChangeUserAtStartup() );
  act->setData( 45 );
  mp_actPromptPassword = mp_menuConnectionSettings->addAction( tr( "Prompts to ask network password" ), this, SLOT( settingsChanged() ) );
  mp_actPromptPassword->setCheckable( true );
  mp_actPromptPassword->setChecked( Settings::instance().askPasswordAtStartup() );
  mp_actPromptPassword->setData( 17 );

  mp_menuNetworkStatus = new QMenu( tr( "Network" ), this );
  mp_menuNetworkStatus->setIcon( IconManager::instance().icon( "network.png" ) );
  mp_menuSettings->addMenu( mp_menuNetworkStatus );
  mp_menuNetworkStatus->addAction( mp_actConfigureNetwork );
  mp_menuNetworkStatus->addSeparator();
  mp_actEditWorkgroups = mp_menuNetworkStatus->addAction( IconManager::instance().icon( "workgroup.png" ), tr( "Your workgroups" ) + QString( "..." ), this, SLOT( showWorkgroups() ) );
  mp_menuNetworkStatus->addSeparator();
  mp_actHostAddress = mp_menuNetworkStatus->addAction( IconManager::instance().icon( "network.png" ), QString( "ip" ) );
  mp_actPortBroadcast = mp_menuNetworkStatus->addAction( IconManager::instance().icon( "broadcast.png" ), QString( "udp1" ) );
  mp_actMulticastGroupAddress = mp_menuNetworkStatus->addAction( IconManager::instance().icon( "multicast-group.png" ), QString( "multicast" ) );
  mp_actPortListener = mp_menuNetworkStatus->addAction( IconManager::instance().icon( "default-chat-online.png" ), QString( "tcp1" ) );
  mp_actPortFileTransfer = mp_menuNetworkStatus->addAction( IconManager::instance().icon( "network-scan.png" ), QString( "tcp2" ) );
#ifdef BEEBEEP_USE_MULTICAST_DNS
  mp_actMulticastDns = mp_menuNetworkStatus->addAction( IconManager::instance().icon( "mdns.png" ), QString( "mdns" ) );
#endif
  mp_menuNetworkStatus->addSeparator();
  mp_actEncryptedConnectionByDefault = mp_menuNetworkStatus->addAction( Settings::instance().disableConnectionSocketEncryption() ? IconManager::instance().icon( "encryption-disabled.png" ) : IconManager::instance().icon( "encryption-enabled.png" ), QString( "End-to-end encryption" ) );
  mp_actCompressedDataByDefault = mp_menuNetworkStatus->addAction( Settings::instance().disableConnectionSocketDataCompression() ? IconManager::instance().icon( "folder.png" ) : IconManager::instance().icon( "file-compressed.png" ), QString( "Data compression" ) );

  mp_menuUsersSettings = new QMenu( tr( "Users" ), this );
  mp_menuUsersSettings->setIcon( IconManager::instance().icon( "user-list.png" ) );
  mp_menuSettings->addMenu( mp_menuUsersSettings );

  QMenu* menu_recognize_users = mp_menuUsersSettings->addMenu( tr( "Recognize users" ) + QString( "..." ) );
  menu_recognize_users->setIcon( IconManager::instance().icon( "user-list.png" ) );
  mp_actGroupRecognizeUsers = new QActionGroup( this );
  mp_actGroupRecognizeUsers->setExclusive( true );
  act = menu_recognize_users->addAction( tr( "By nickname" ) + QString( " (%1)" ).arg( tr( "default" ) ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().userRecognitionMethod() == Settings::RecognizeByNickname );
  act->setData( 59 );
  mp_actGroupRecognizeUsers->addAction( act );
  act = menu_recognize_users->addAction( tr( "By account name and domain name" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().userRecognitionMethod() == Settings::RecognizeByAccountAndDomain );
  act->setData( 57 );
  mp_actGroupRecognizeUsers->addAction( act );
  act = menu_recognize_users->addAction( tr( "By account name" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().userRecognitionMethod() == Settings::RecognizeByAccount );
  act->setData( 58 );
  mp_actGroupRecognizeUsers->addAction( act );
  menu_recognize_users->addSeparator();
  connect( mp_actGroupRecognizeUsers, SIGNAL( triggered( QAction* ) ), this, SLOT( settingsChanged( QAction* ) ) );
  mp_menuUsersSettings->addSeparator();
  mp_actAddUsers = mp_menuUsersSettings->addAction( IconManager::instance().icon( "user-add.png" ), tr( "Add users" ) + QString( "..." ), this, SLOT( showAddUser() ) );
  mp_menuUsersSettings->addSeparator();
  mp_actSaveUserList = mp_menuUsersSettings->addAction( tr( "Save users" ), this, SLOT( settingsChanged() ) );
  mp_actSaveUserList->setCheckable( true );
  mp_actSaveUserList->setChecked( Settings::instance().saveUserList() );
  mp_actSaveUserList->setData( 32 );
  mp_actSaveGroupList = mp_menuUsersSettings->addAction( tr( "Save groups" ), this, SLOT( settingsChanged() ) );
  mp_actSaveGroupList->setCheckable( true );
  mp_actSaveGroupList->setChecked( Settings::instance().saveGroupList() );
  mp_actSaveGroupList->setData( 2 );
  mp_menuUsersSettings->addSeparator();
  mp_actShowUserFullName = mp_menuUsersSettings->addAction( tr( "Show the name and surname of the users" ) , this, SLOT( settingsChanged() ) );
  mp_actShowUserFullName->setCheckable( true );
  mp_actShowUserFullName->setChecked( Settings::instance().useUserFullName() );
  mp_actShowUserFullName->setDisabled( true );
  mp_actShowUserFirstNameFirstInFullName = mp_menuUsersSettings->addAction( tr( "Show surname before firstname of the users" ) , this, SLOT( settingsChanged() ) );
  mp_actShowUserFirstNameFirstInFullName->setCheckable( true );
  mp_actShowUserFirstNameFirstInFullName->setChecked( !Settings::instance().useUserFirstNameFirstInFullName() );
  mp_actShowUserFirstNameFirstInFullName->setData( 102 );
  mp_menuUsersSettings->addSeparator();
  mp_actRemoveInactiveUsers = mp_menuUsersSettings->addAction( "", this, SLOT( settingsChanged() ) );
  mp_actRemoveInactiveUsers->setCheckable( true );
  mp_actRemoveInactiveUsers->setChecked( Settings::instance().removeInactiveUsers() );
  mp_actRemoveInactiveUsers->setData( 33 );
  setMaxInactivityDaysInAction( mp_actRemoveInactiveUsers );
  mp_menuChatSettings = new QMenu( tr( "Chat" ), this );
  mp_menuChatSettings->setIcon( IconManager::instance().icon( "chat.png" ) );
  mp_menuSettings->addMenu( mp_menuChatSettings );
  mp_menuChatSaveSettings = new QMenu( tr( "Save messages" ) + QString( "..." ), this );
  mp_menuChatSaveSettings->setIcon( IconManager::instance().icon( "save-as.png" ) );
  mp_menuChatSettings->addMenu( mp_menuChatSaveSettings );
  act = mp_menuChatSaveSettings->addAction( tr( "Enable message saving" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().chatAutoSave() );
  act->setData( 18 );
  mp_menuChatSaveSettings->addSeparator();
  act = mp_menuChatSaveSettings->addAction( tr( "Save unsent messages" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().chatSaveUnsentMessages() );
  act->setData( 81 );
  mp_actSaveFileTransferMessages = mp_menuChatSaveSettings->addAction( tr( "Save file transfer messages" ), this, SLOT( settingsChanged() ) );
  mp_actSaveFileTransferMessages->setCheckable( true );
  mp_actSaveFileTransferMessages->setChecked( Settings::instance().chatSaveFileTransfers() );
  mp_actSaveFileTransferMessages->setData( 82 );
  mp_actSaveSystemMessages = mp_menuChatSaveSettings->addAction( tr( "Save system messages" ), this, SLOT( settingsChanged() ) );
  mp_actSaveSystemMessages->setCheckable( true );
  mp_actSaveSystemMessages->setChecked( Settings::instance().chatSaveSystemMessages() );
  mp_actSaveSystemMessages->setData( 83 );
  mp_menuChatSettings->addSeparator();
  act = mp_menuChatSaveSettings->addAction( "", this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setData( 84 );
  setChatMaxLinesToSaveInAction( act );
  mp_menuChatSaveSettings->addSeparator();
  act = mp_menuChatSaveSettings->addAction( "", this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setData( 85 );
  setClearCacheAfterDaysInAction( act );
  mp_menuChatSettings->addSeparator();
  act = mp_menuChatSettings->addAction( tr( "Open chats in a single window" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().showChatsInOneWindow() );
  act->setData( 7 );
  act = mp_menuChatSettings->addAction( tr( "Clear all read messages on closing window" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().chatClearAllReadMessages() );
  act->setData( 47 );
  act = mp_menuChatSettings->addAction( tr( "Use high resolution emoticons" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().useHiResEmoticons() );
  act->setData( 105 );
#if QT_VERSION < 0x050000 || defined( BEEBEEP_FOR_RASPBERRY_PI ) || defined( Q_OS_OS2 )
  act->setEnabled( false );
#endif
  act = mp_menuChatSettings->addAction( tr( "Use font emoticons" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().useFontEmoticons() );
  act->setData( 31 );
  act = mp_menuChatSettings->addAction( tr( "Show chat toolbar" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().showChatToolbar() );
  act->setData( 42 );
  mp_menuChatSettings->addSeparator();
  act = mp_menuChatSettings->addAction( tr( "Send offline messages also to chat with all users" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().sendOfflineMessagesToDefaultChat() );
  act->setData( 66 );
  act = mp_menuChatSettings->addAction( "", this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  setChatMessagesToShowInAction( act );
  act->setData( 27 );
  act = mp_menuChatSettings->addAction( "", this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setData( 71 );
  setChatInactiveWindowOpacityLevelInAction( act );
  mp_menuChatSettings->addSeparator();
  QMenu* menu_on_sending_message = mp_menuChatSettings->addMenu( tr( "On sending message" ) + QString( "..." ) );
  menu_on_sending_message->setIcon( IconManager::instance().icon( "send.png" ) );
  mp_actGroupOnSendingMessage = new QActionGroup( this );
  mp_actGroupOnSendingMessage->setExclusive( true );
  act = menu_on_sending_message->addAction( tr( "Do nothing" ) + QString( " (%1)" ).arg( tr( "default" ) ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().chatOnSendingMessage() == Settings::SkipOnSendingMessage );
  act->setData( 91 );
  mp_actGroupOnSendingMessage->addAction( act );
  act = menu_on_sending_message->addAction( tr( "Minimize chat window" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().chatOnSendingMessage() == Settings::MinimizeChatOnSendingMessage );
  act->setData( 92 );
  mp_actGroupOnSendingMessage->addAction( act );
  act = menu_on_sending_message->addAction( tr( "Close chat window" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().chatOnSendingMessage() == Settings::CloseChatOnSendingMessage );
  act->setData( 93 );
  mp_actGroupOnSendingMessage->addAction( act );
  connect( mp_actGroupOnSendingMessage, SIGNAL( triggered( QAction* ) ), this, SLOT( settingsChanged( QAction* ) ) );
  mp_menuChatSettings->addSeparator();
  mp_menuChatColorSettings = new QMenu( tr( "Colors" ) + QString( "..." ), this );
  mp_menuChatColorSettings->setIcon( IconManager::instance().icon( "colors.png" ) );
  mp_menuChatSettings->addMenu( mp_menuChatColorSettings );
  act = mp_menuChatColorSettings->addAction( IconManager::instance().icon( "background-color.png" ), tr( "Select chat background color" ), this, SLOT( settingsChanged() ) );
  act->setData( 72 );
  act = mp_menuChatColorSettings->addAction( IconManager::instance().icon( "font-color.png" ), tr( "Select chat default text color" ), this, SLOT( settingsChanged() ) );
  act->setData( 73 );
  act = mp_menuChatColorSettings->addAction( IconManager::instance().icon( "log.png" ), tr( "Select chat system text color" ), this, SLOT( settingsChanged() ) );
  act->setData( 74 );
  act = mp_menuChatColorSettings->addAction( IconManager::instance().icon( "quote-text.png" ), tr( "Select quote text color" ), this, SLOT( settingsChanged() ) );
  act->setData( 89 );
  act = mp_menuChatColorSettings->addAction( IconManager::instance().icon( "quote-background.png" ), tr( "Select quote background color" ), this, SLOT( settingsChanged() ) );
  act->setData( 90 );
  mp_actSelectEmoticonSourcePath = mp_menuChatSettings->addAction( IconManager::instance().icon( "emoticon.png" ), tr( "Select emoticon theme" ) + QString( "..." ), this, SLOT( selectEmoticonSourcePath() ) );
  mp_actSelectEmoticonSourcePath->setEnabled( !Settings::instance().useFontEmoticons() );
  act = mp_menuChatSettings->addAction( IconManager::instance().icon( "icon-size.png" ), tr( "Change size of the emoticons" ) + QString( "..." ), this, SLOT( changeEmoticonSizeInChat() ) );
  act->setEnabled( !Settings::instance().useFontEmoticons() );
  mp_menuChatSettings->addSeparator();
  mp_menuChatSettings->addAction( IconManager::instance().icon( "dictionary.png" ), tr( "Dictionary" ) + QString( "..." ), this, SLOT( selectDictionatyPath() ) );
  mp_menuChatSettings->addSeparator();
  mp_menuChatSettings->addAction( IconManager::instance().icon( "refused-chat.png" ), tr( "Blocked chats" ) + QString( "..." ), this, SLOT( showRefusedChats() ) );

  mp_menuFileTransferSettings = new QMenu( tr( "File transfer" ), this );
  mp_menuFileTransferSettings->setIcon( IconManager::instance().icon( "file-transfer.png" ) );
  mp_menuFileTransferSettings->setDisabled( Settings::instance().disableFileTransfer() );
  mp_menuSettings->addMenu( mp_menuFileTransferSettings );
  act = mp_menuFileTransferSettings->addAction( tr( "Enable file transfer" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().enableFileTransfer() );
  act->setData( 12 );
  mp_actEnableFileSharing = mp_menuFileTransferSettings->addAction( tr( "Enable file sharing" ), this, SLOT( settingsChanged() ) );
  mp_actEnableFileSharing->setCheckable( true );
  mp_actEnableFileSharing->setChecked( Settings::instance().enableFileSharing() );
  mp_actEnableFileSharing->setData( 5 );
  mp_actEnableFileSharing->setDisabled( Settings::instance().disableFileSharing() );
  mp_menuFileTransferSettings->addSeparator();
  mp_actConfirmDownload = mp_menuFileTransferSettings->addAction( tr( "Prompt before downloading file" ), this, SLOT( settingsChanged() ) );
  mp_actConfirmDownload->setCheckable( true );
  mp_actConfirmDownload->setChecked( Settings::instance().confirmOnDownloadFile() );
  mp_actConfirmDownload->setData( 30 );
  act = mp_menuFileTransferSettings->addAction( tr( "Always download files into the folder with the user's name" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().downloadInUserFolder() );
  act->setData( 86 );
  mp_menuExistingFile = mp_menuFileTransferSettings->addMenu( tr( "If a file already exists" ) + QString( "..." ) );
  mp_actGroupExistingFile = new QActionGroup( this );
  mp_actGroupExistingFile->setExclusive( true );
  act = mp_menuExistingFile->addAction( tr( "Generate new file name" ) );
  act->setCheckable( true );
  act->setChecked( true );
  act->setData( (int)Settings::GenerateNewFileName );
  mp_actGroupExistingFile->addAction( act );
  act = mp_menuExistingFile->addAction( tr( "Skip" ) );
  act->setCheckable( true );
  act->setData( (int)Settings::SkipExistingFile );
  mp_actGroupExistingFile->addAction( act );
  act = mp_menuExistingFile->addAction( tr( "Overwrite" ) );
  act->setCheckable( true );
  act->setData( (int)Settings::OverwriteExistingFile );
  mp_actGroupExistingFile->addAction( act );
  act = mp_menuExistingFile->addAction( tr( "Overwrite older" ) );
  act->setCheckable( true );
  act->setData( (int)Settings::OverwriteOlderExistingFile );
  mp_actGroupExistingFile->addAction( act );
  foreach( QAction* act_to_select, mp_actGroupExistingFile->actions() )
  {
    if( act_to_select->data().toInt() == Settings::instance().onExistingFileAction() )
    {
      act_to_select->setChecked( true );
      break;
    }
  }
  connect( mp_actGroupExistingFile, SIGNAL( triggered( QAction* ) ), this, SLOT( onChangeSettingOnExistingFile( QAction* ) ) );

  act = mp_menuFileTransferSettings->addAction( tr( "Keep the original modification date of the transferred file" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().keepModificationDateOnFileTransferred() );
  act->setData( 101 );
  mp_actResumeFileTransfer = mp_menuFileTransferSettings->addAction( tr( "Resume file transfer" ) + QString( " (%1)" ).arg( tr( "when possible" ) ), this, SLOT( settingsChanged() ) );
  mp_actResumeFileTransfer->setCheckable( true );
  mp_actResumeFileTransfer->setChecked( Settings::instance().resumeFileTransfer() );
  mp_actResumeFileTransfer->setData( 97 );
  act = mp_menuFileTransferSettings->addAction( "", this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setData( 100 );
  setClearPartiallyDownloadedFilesAfterDaysInAction( act );
  mp_menuFileTransferSettings->addSeparator();
  act = mp_menuFileTransferSettings->addAction( "", this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  setMaxQueuedDownloadsInAction( act );
  act->setData( 88 );
  act = mp_menuFileTransferSettings->addAction( tr( "Always shows the progress of file transfer" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().alwaysShowFileTransferProgress() );
  act->setData( 78 );
  mp_menuFileTransferSettings->addSeparator();
  act = mp_menuFileTransferSettings->addAction( tr( "Use native file dialogs" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().useNativeDialogs() );
  act->setData( 4 );
  mp_menuFileTransferSettings->addSeparator();
  mp_actSelectDownloadFolder = mp_menuFileTransferSettings->addAction( IconManager::instance().icon( "download-folder.png" ), tr( "Select download folder" ) + QString( "..." ), this, SLOT( selectDownloadDirectory() ) );

#ifdef BEEBEEP_USE_VOICE_CHAT
  mp_menuVoiceMessage = new QMenu( tr( "Voice message" ), this );
  mp_menuVoiceMessage->setIcon( IconManager::instance().icon( "microphone.png" ) );
  mp_menuVoiceMessage->setDisabled( Settings::instance().disableVoiceMessages() );
  mp_menuSettings->addMenu( mp_menuVoiceMessage );
  mp_menuVoiceMessage->addAction( IconManager::instance().icon( "audio-settings.png" ), tr( "Voice encoder" ) + QString( "..." ), this, SLOT( showVoiceEncoderSettings() ) );
  mp_menuVoiceMessage->addSeparator();
  act = mp_menuVoiceMessage->addAction( IconManager::instance().icon( "timer.png" ), tr( "Maximum duration" ) + QString( "..." ), this, SLOT( settingsChanged() ) );
  act->setData( 94 );
  mp_menuVoiceMessage->addSeparator();
  act = mp_menuVoiceMessage->addAction( tr( "Use the integrated voice message player" ) + QString( " (beta)" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().useVoicePlayer() );
  act->setData( 95 );
#endif

  mp_menuNotificationSettings = new QMenu( tr( "Notifications" ), this );
  mp_menuNotificationSettings->setIcon( IconManager::instance().icon( "bell.png" ) );
  mp_menuSettings->addMenu( mp_menuNotificationSettings );
  mp_actBeepOnNewMessage = mp_menuNotificationSettings->addAction( tr( "Enable BEEP alert" ), this, SLOT( settingsChanged() ) );
  mp_actBeepOnNewMessage->setCheckable( true );
  mp_actBeepOnNewMessage->setChecked( Settings::instance().beepOnNewMessageArrived() );
  mp_actBeepOnNewMessage->setData( 34 );
  act = mp_menuNotificationSettings->addAction( tr( "Enable Buzz sound" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().playBuzzSound() );
  act->setData( 56 );
  act = mp_menuNotificationSettings->addAction( tr( "Enable visual notifications for chat windows" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().enableVisualNotificationsInChatWindow() );
  act->setData( 107 );
  act = mp_menuNotificationSettings->addAction( tr( "Enable notifications also for chat with all users" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().enableDefaultChatNotifications() );
  act->setData( 75 );
  act = mp_menuNotificationSettings->addAction( tr( "Enable BEEP alert also for active chat windows" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().beepInActiveWindowAlso() );
  act->setData( 98 );
  act = mp_menuNotificationSettings->addAction( tr( "Disable BEEP alert if your status is busy" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().disableBeepInUserStatusBusy() );
  act->setData( 96 );
  mp_menuNotificationSettings->addSeparator();
  act = mp_menuNotificationSettings->addAction( tr( "Raise main window on new message" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().raiseMainWindowOnNewMessageArrived() );
  act->setData( 80 );
  act = mp_menuNotificationSettings->addAction( tr( "Always open chat on new message" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().alwaysOpenChatOnNewMessageArrived() );
  act->setData( 70 );
  act = mp_menuNotificationSettings->addAction( tr( "Raise previously opened chat on new message" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().raiseOnNewMessageArrived() );
  act->setData( 15 );
  mp_menuNotificationSettings->addSeparator();
  mp_menuTrayIconSettings = new QMenu( tr( "System tray icon" ), this );
  mp_menuTrayIconSettings->setIcon( IconManager::instance().icon( "settings-tray-icon.png" ) );
  mp_menuNotificationSettings->addMenu( mp_menuTrayIconSettings );
  act = mp_menuTrayIconSettings->addAction( tr( "Enable tray icon notifications" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().showNotificationOnTray()  );
  act->setData( 19 );
  act = mp_menuTrayIconSettings->addAction( tr( "Show only message notifications" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().showOnlyMessageNotificationOnTray()  );
  act->setData( 40 );
  act = mp_menuTrayIconSettings->addAction( tr( "Show chat message preview" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().showChatMessageOnTray() );
  act->setData( 46 );
  act = mp_menuTrayIconSettings->addAction( tr( "Show file notifications" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().showFileTransferCompletedOnTray() );
  act->setData( 48 );
  mp_menuNotificationSettings->addSeparator();
  mp_menuNotificationSettings->addAction( IconManager::instance().icon( "file-beep.png" ), tr( "Select beep file..." ), this, SLOT( selectBeepFile() ) );
  mp_menuNotificationSettings->addAction( IconManager::instance().icon( "play.png" ), tr( "Play beep" ), this, SLOT( testBeepFile() ) );

#ifdef BEEBEEP_USE_SHAREDESKTOP
  QMenu* menu_share_desktop = new QMenu( tr( "Desktop sharing" ), this );
  menu_share_desktop->setIcon( IconManager::instance().icon( "desktop-share.png" ) );
  menu_share_desktop->setDisabled( Settings::instance().disableDesktopSharing() );
  mp_menuSettings->addMenu( menu_share_desktop );
  act = menu_share_desktop->addAction( tr( "Enable desktop sharing" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().enableShareDesktop() );
  act->setData( 60 );
  menu_share_desktop->addSeparator();
  act = menu_share_desktop->addAction( IconManager::instance().icon( "timer.png" ), tr( "Select screen capture interval" ) + QString( "..." ), this, SLOT( settingsChanged() ) );
  act->setData( 61 );
  act = menu_share_desktop->addAction( IconManager::instance().icon( "image-type.png" ), tr( "Select image type" ) + QString( "..." ), this, SLOT( settingsChanged() ) );
  act->setData( 62 );
  act = menu_share_desktop->addAction( IconManager::instance().icon( "image-quality.png" ), tr( "Select image quality" ) + QString( "..." ), this, SLOT( settingsChanged() ) );
  act->setData( 63 );
  menu_share_desktop->addSeparator();
  act = menu_share_desktop->addAction( tr( "Fit image to screen" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().shareDesktopFitToScreen() );
  act->setData( 64 );
#endif

  mp_menuSettings->addSeparator();
  act = mp_menuSettings->addAction( tr( "Always stay on top" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().stayOnTop() );
  act->setData( 14 );
#ifdef Q_OS_WIN
  act = mp_menuSettings->addAction( tr( "Start %1 automatically" ).arg( Settings::instance().programName() ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().hasStartOnSystemBoot() );
  act->setData( 16 );
#endif
  mp_actSaveWindowGeometry = mp_menuSettings->addAction( IconManager::instance().icon( "save-window.png" ), tr( "Save window's geometry" ), this, SLOT( askSaveGeometryAndState() ) );
  mp_actSaveWindowGeometry->setDisabled( Settings::instance().resetGeometryAtStartup() );

  /* User List Menu */
  mp_menuUserList = new QMenu( tr( "Options" ), this );
  act = mp_menuUserList->addAction( tr( "Sort users in ascending order" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().sortUsersAscending() );
  act->setData( 49 );
  QMenu* sorting_users_menu = mp_menuUserList->addMenu( tr( "Sorting mode" ) + QString( "..." ) );
  QActionGroup* sorting_users_action_group = new QActionGroup( this );
  sorting_users_action_group->setExclusive( true );
  act = sorting_users_menu->addAction( tr( "Default mode" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().userSortingMode() < 1 || Settings::instance().userSortingMode() > 3 );
  act->setData( 50 );
  sorting_users_action_group->addAction( act );
  act = sorting_users_menu->addAction( tr( "By user name" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().userSortingMode() == 1 );
  act->setData( 51 );
  sorting_users_action_group->addAction( act );
  act = sorting_users_menu->addAction( tr( "By user status" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().userSortingMode() == 2 );
  act->setData( 52 );
  sorting_users_action_group->addAction( act );
  act = sorting_users_menu->addAction( tr( "By unread messages" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().userSortingMode() == 3 );
  act->setData( 53 );
  sorting_users_action_group->addAction( act );
  connect( sorting_users_action_group, SIGNAL( triggered( QAction* ) ), this, SLOT( settingsChanged( QAction* ) ) );
  mp_menuUserList->addSeparator();
  mp_actShowOnlineUsersOnly = mp_menuUserList->addAction( tr( "Show online users only" ), this, SLOT( settingsChanged() ) );
  mp_actShowOnlineUsersOnly->setCheckable( true );
  mp_actShowOnlineUsersOnly->setChecked( Settings::instance().showOnlyOnlineUsers() );
  mp_actShowOnlineUsersOnly->setData( 6 );
  act = mp_menuUserList->addAction( tr( "Show users in their workgroups" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().showUsersInWorkgroups() );
  act->setData( 87 );
  if( mp_actShowUserFullName->isChecked() )
    mp_menuUserList->addAction( mp_actShowUserFullName );
  mp_menuUserList->addAction( mp_actShowUserFirstNameFirstInFullName );
  mp_menuUserList->addSeparator();
  act = mp_menuUserList->addAction( tr( "Show the user's picture" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().showUserPhoto() );
  act->setData( 21 );
  act = mp_menuUserList->addAction( tr( "Show the user's vCard on right click" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().showVCardOnRightClick() );
  act->setData( 25 );
  act = mp_menuUserList->addAction( tr( "Show status color in background" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().showUserStatusBackgroundColor() );
  act->setData( 38 );
  act = mp_menuUserList->addAction( tr( "Show the status description" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().showUserStatusDescription() );
  act->setData( 37 );
  mp_menuUserList->addSeparator();
  mp_actEditAvatarIconSize = mp_menuUserList->addAction( IconManager::instance().icon( "icon-size.png" ), tr( "Change size of the user's picture" ), this, SLOT( changeAvatarSizeInList() ) );
  mp_userList->setMenuSettings( mp_menuUserList );

  /* Status Menu */
  mp_menuStatus = new QMenu( tr( "Status" ), this );
  mp_menuStatus->setIcon( IconManager::instance().icon( "user-status.png" ) );
  for( int i = User::Online; i < User::NumStatus; i++ )
  {
    act = mp_menuStatus->addAction( QIcon( Bee::menuUserStatusIconFileName( i ) ), Bee::userStatusToString( i ), this, SLOT( statusSelected() ) );
    act->setData( i );
    act->setIconVisibleInMenu( true );
  }
  mp_menuStatus->addSeparator();
  mp_actSetAutoAway = mp_menuStatus->addAction( tr( "Set your status to away automatically" ), this, SLOT( settingsChanged() ) );
  mp_actSetAutoAway->setCheckable( true );
  mp_actSetAutoAway->setChecked( Settings::instance().autoUserAway() );
  mp_actSetAutoAway->setData( 20 );
  mp_menuStatus->addSeparator();
  mp_menuUserStatusList = new QMenu( tr( "Recently used" ), this );
  act = mp_menuStatus->addMenu( mp_menuUserStatusList );
  act->setIcon( IconManager::instance().icon( "recent.png" ) );
  loadUserStatusRecentlyUsed();
  mp_actChangeStatusDescription = mp_menuStatus->addAction( IconManager::instance().icon( "user-status.png" ), tr( "Change your status description..." ), this, SLOT( changeStatusDescription() ) );
  mp_menuStatus->addAction( IconManager::instance().icon( "clear.png" ), tr( "Clear all status descriptions" ), this, SLOT( clearRecentlyUsedUserStatus() ) );
  mp_menuStatus->addSeparator();
  mp_menuStatus->addAction( mp_actEditWorkgroups );
  mp_menuStatus->addSeparator();
  act = mp_menuStatus->addAction( QIcon( Bee::menuUserStatusIconFileName( User::Offline ) ), Bee::userStatusToString( User::Offline ), this, SLOT( statusSelected() ) );
  act->setData( User::Offline );
  act->setIconVisibleInMenu( true );
  act = mp_menuStatus->menuAction();
  connect( act, SIGNAL( triggered() ), this, SLOT( showLocalUserVCard() ) );

  /* Context Menu for user list view */
  QMenu* context_menu_users = new QMenu( "Menu", this );
  if( mp_actShowUserFullName->isChecked() )
    context_menu_users->addAction( mp_actShowUserFullName );
  context_menu_users->addAction( mp_actShowUserFirstNameFirstInFullName );
  context_menu_users->addSeparator();
  context_menu_users->addAction( mp_actVCard );
  context_menu_users->addAction( mp_actChangeStatusDescription );
  context_menu_users->addSeparator();
  context_menu_users->addAction( mp_actSetAutoAway );
  context_menu_users->addSeparator();
  context_menu_users->addAction( mp_actConfigureNetwork );
  context_menu_users->addAction( mp_actAddUsers );
  context_menu_users->addSeparator();
  context_menu_users->addAction( mp_actEditAvatarIconSize );
  context_menu_users->addSeparator();
  context_menu_users->addAction( mp_actShowOnlineUsersOnly );
  mp_userList->setContextMenuUsers( context_menu_users );

  /* Help Menu */
  mp_menuInfo = new QMenu( tr("?" ), this );
  mp_menuInfo->addAction( IconManager::instance().icon( "donate.png" ), tr( "Donate for %1" ).arg( Settings::instance().programName() ) + QString( "..." ), this, SLOT( openDonationPage() ) );
  mp_menuInfo->addSeparator();
  mp_menuInfo->addAction( mp_actAbout );
  mp_menuInfo->addAction( IconManager::instance().icon( "license.png" ), tr( "Show %1's license..." ).arg( Settings::instance().programName() ), this, SLOT( showLicense() ) );
  act = mp_menuInfo->addAction( IconManager::instance().icon( "qt.png" ), tr( "Qt Library..." ), this, SLOT( showAboutQt() ) );
  act->setMenuRole( QAction::AboutQtRole );
  mp_menuInfo->addSeparator();
  mp_menuInfo->addAction( IconManager::instance().icon( "beebeep.png" ), tr( "Open %1 official website..." ).arg( Settings::instance().programName() ), this, SLOT( openWebSite() ) );
  mp_menuInfo->addAction( IconManager::instance().icon( "update.png" ), tr( "Check for new version..." ), this, SLOT( checkNewVersion() ) );
  mp_menuInfo->addAction( IconManager::instance().icon( "plugin.png" ), tr( "Download plugins..." ), this, SLOT( openDownloadPluginPage() ) );
  mp_menuInfo->addSeparator();
  mp_menuInfo->addAction( IconManager::instance().icon( "info.png" ), tr( "Help online..." ), this, SLOT( openHelpPage() ) );
  mp_menuInfo->addAction( IconManager::instance().icon( "tip.png" ), tr( "Show tips..." ), this, SLOT( showTipOfTheDay() ) );
  mp_menuInfo->addAction( IconManager::instance().icon( "chat-small.png" ), tr( "Read FAQ..." ), this, SLOT( showFaq() ) );
  mp_menuInfo->addAction( IconManager::instance().icon( "fact.png" ), tr( "Discover the fact of the day..." ), this, SLOT( showFactOfTheDay() ) );
  mp_menuInfo->addSeparator();
  mp_menuInfo->addAction( IconManager::instance().icon( "star.png" ), tr( "Information about %1..." ).arg( "Marco Mastroddi" ), this, SLOT( openDeveloperWebSite() ) );
#ifdef BEEBEEP_DEBUG
  mp_menuInfo->addSeparator();
  act = mp_menuInfo->addAction( tr( "Add +1 user to anonymous usage statistics" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().postUsageStatistics() );
  act->setData( 44 );
#endif

  /* Tray icon menu */
  mp_menuTrayIcon = new QMenu( this );
  act = mp_menuTrayIcon->addAction( IconManager::instance().icon( "beebeep.png" ), tr( "Show" ), this, SLOT( showUp() ) );
  mp_menuTrayIcon->setDefaultAction( act );
  mp_menuTrayIcon->addSeparator();
  mp_menuTrayIcon->addAction( mp_menuStatus->menuAction() );
  mp_menuTrayIcon->addSeparator();
  mp_menuTrayIcon->addAction( mp_actViewNewMessage );
  mp_menuTrayIcon->addSeparator();
  mp_menuTrayIcon->addAction( IconManager::instance().icon( "quit.png" ), tr( "Quit" ), this, SLOT( forceShutdown() ) );

  mp_trayIcon->setContextMenu( mp_menuTrayIcon );
}

void GuiMain::createToolAndMenuBars()
{
  menuBar()->addMenu( mp_menuMain );
  if( Settings::instance().disableMenuSettings() )
    mp_menuSettings->setDisabled( true );
  menuBar()->addMenu( mp_menuSettings );
  menuBar()->addMenu( mp_menuInfo );
  QLabel *label_version = new QLabel( this );
  label_version->setTextFormat( Qt::RichText );
  label_version->setAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
  QString label_version_text = QString( "&nbsp;&nbsp;<b>%1%2</b> %3&nbsp;" )
                                .arg( Settings::instance().version( Settings::instance().isDevelopmentVersion(), false, false ),
                                      Settings::instance().isDevelopmentVersion() ? QString( "-dev" ) : "",
                                      IconManager::instance().toHtml( Settings::instance().operatingSystemIconPath(), "*", 12, 12 ) );
  QString label_tooltip = QString( "BeeBEEP %1 %2%3" ).arg( Settings::instance().version( true, true, true ), Settings::instance().operatingSystem( true ) );
  if( Settings::instance().isDevelopmentVersion() )
    label_tooltip.append( QString( " (%1)" ).arg( tr("Development version") ) );

  if( Settings::instance().enableReceivingHelpMessages() )
  {
    label_version_text.append( QString( "&nbsp;%1&nbsp;" ).arg( IconManager::instance().toHtml( "help.png", "H", 12, 12 ) ) );
    label_tooltip.append( QString( " [%1]" ).arg( tr("Help station") ) );
  }

  label_version->setText( label_version_text );
  label_version->setToolTip( label_tooltip );
  menuBar()->setCornerWidget( label_version );

  mp_barMain->addAction( mp_menuStatus->menuAction() );
  mp_barMain->addAction( mp_actVCard );
  mp_barMain->addSeparator();
  mp_barMain->addAction( mp_actBroadcast );
  mp_barMain->addSeparator();
  mp_barMain->addAction( mp_actViewNewMessage );
  mp_barMain->addAction( mp_actCreateMessage );
  mp_barMain->addAction( mp_actCreateGroupChat );
  mp_barMain->addSeparator();
  mp_barMain->addAction( mp_actViewFileTransfer );
  mp_barMain->addAction( mp_actViewFileSharing );
}

void GuiMain::createMainWidgets()
{
  int tab_index;
  QString tooltip_right_button = tr( "Right click to open menu" );

  mp_home = new GuiHome( this );
  connect( mp_home, SIGNAL( openUrlRequest( const QUrl& ) ), this, SLOT( openUrl( const QUrl& ) ) );
  connect( mp_home, SIGNAL( clearSystemMessagesRequest( VNumber ) ), this, SLOT( clearSystemMessagesInChat( VNumber ) ) );
  tab_index = mp_tabMain->addTab( mp_home, IconManager::instance().icon( "activities.png" ), "" );
  mp_tabMain->setTabToolTip( tab_index, tr( "Activities" ) );
  mp_home->setMainToolTip( QString( "%1\n(%2)" ).arg( mp_tabMain->tabToolTip( tab_index ), tooltip_right_button ) );

  mp_userList = new GuiUserList( this );
  tab_index = mp_tabMain->addTab( mp_userList, IconManager::instance().icon( "user-list.png" ), "" );
  mp_tabMain->setTabToolTip( tab_index, tr( "Users" ) );
  mp_userList->setMainToolTip( QString( "%1\n(%2)" ).arg( mp_tabMain->tabToolTip( tab_index ), tooltip_right_button ) );

  mp_chatList = new GuiChatList( this );
  tab_index = mp_tabMain->addTab( mp_chatList, IconManager::instance().icon( "chat-list.png" ), "" );
  mp_tabMain->setTabToolTip( tab_index, tr( "Chats" ) );
  mp_chatList->setMainToolTip( QString( "%1\n(%2)" ).arg( mp_tabMain->tabToolTip( tab_index ), tooltip_right_button ) );

  mp_groupList = new GuiGroupList( this );
  tab_index = mp_tabMain->addTab( mp_groupList, IconManager::instance().icon( "group.png" ), "" );
  mp_tabMain->setTabToolTip( tab_index, tr( "Groups" ) );
  mp_groupList->setMainToolTip( QString( "%1\n(%2)" ).arg( mp_tabMain->tabToolTip( tab_index ), tooltip_right_button ) );

  mp_savedChatList = new GuiSavedChatList( this );
  tab_index = mp_tabMain->addTab( mp_savedChatList, IconManager::instance().icon( "saved-chat-list.png" ), "" );
  mp_tabMain->setTabToolTip( tab_index, tr( "Saved chats" ) );
  mp_savedChatList->setMainToolTip( QString( "%1\n(%2)" ).arg( mp_tabMain->tabToolTip( tab_index ), tooltip_right_button ) );

#ifdef BEEBEEP_USE_WEBENGINE
  mp_webView = new GuiWebView();
  connect( mp_webView, SIGNAL( newsLoadFinished( bool ) ), this, SLOT( onNewsLoad( bool ) ) );
#endif

  mp_dockFileTransfers = new QDockWidget( tr( "File Transfers" ), this );
  mp_dockFileTransfers->setObjectName( "GuiFileTransferDock" );
  mp_fileTransfer = new GuiFileTransfer( this );
  mp_dockFileTransfers->setWidget( mp_fileTransfer );
  mp_dockFileTransfers->setAllowedAreas( Qt::AllDockWidgetAreas );
  addDockWidget( Qt::BottomDockWidgetArea, mp_dockFileTransfers );
  mp_actViewFileTransfer = mp_dockFileTransfers->toggleViewAction();
  mp_actViewFileTransfer->setIcon( IconManager::instance().icon( "file-transfer.png" ) );
  mp_actViewFileTransfer->setText( tr( "Show the file transfer panel" ) );
  mp_actViewFileTransfer->setData( 99 );
  mp_dockFileTransfers->hide();
}

void GuiMain::startExternalApplicationFromActionData()
{
  QAction* act = qobject_cast<QAction*>( sender() );
  if( !act )
    return;

  QString application_path = act->data().toString();
  qDebug() << "Starting external application:" << qPrintable( application_path );
  if( !QDesktopServices::openUrl( QUrl::fromLocalFile( application_path ) ) )
    QMessageBox::information( qApp->activeWindow(), Settings::instance().programName(), tr( "Unable to open %1" ).arg( application_path ), tr( "Ok" ) );
}

void GuiMain::settingsChanged()
{
  QAction* act = qobject_cast<QAction*>( sender() );
  if( act )
    settingsChanged( act );
}

void GuiMain::settingsChanged( QAction* act )
{
  bool refresh_users = false;
  bool refresh_chat = false;
  int settings_data_id = act->data().toInt();
  bool ok = false;

#ifdef BEEBEEP_DEBUG
  if( act->isCheckable() )
    qDebug() << "Settings changed for action id" << settings_data_id << "to" << static_cast<int>(act->isChecked());
#endif

  switch( settings_data_id )
  {
  case 1:
    Settings::instance().setChatCompact( act->isChecked() );
    refresh_chat = true;
    break;
  case 2:
    Settings::instance().setSaveGroupList( act->isChecked() );
    break;
  case 3:
    Settings::instance().setChatShowMessageTimestamp( act->isChecked() );
    refresh_chat = true;
    break;
  case 4:
    Settings::instance().setUseNativeDialogs( act->isChecked() );
    break;
  case 5:
    setFileSharingEnabled( act->isChecked() );
    break;
  case 6:
    Settings::instance().setShowOnlyOnlineUsers( act->isChecked() );
    refresh_users = true;
    refresh_chat = true;
    break;
  case 7:
    {
      Settings::instance().setShowChatsInOneWindow( act->isChecked() );
      if( Settings::instance().showChatsInOneWindow() )
      {
        foreach( GuiFloatingChat* fl_chat, m_floatingChats )
          fl_chat->close();
      }
    }
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
    Settings::instance().setCloseMinimizeInTray( act->isChecked() );
    break;
  case 12:
    setFileTransferEnabled( act->isChecked() );
    break;
  case 13:
    Settings::instance().setShowMessagesGroupByUser( act->isChecked() );
    refresh_chat = true;
    break;
  case 14:
    {
      Settings::instance().setStayOnTop( act->isChecked() );
      Bee::setWindowStaysOnTop( this, act->isChecked() );
      foreach( GuiFloatingChat* fl_chat, m_floatingChats )
        Bee::setWindowStaysOnTop( fl_chat, act->isChecked() );
      if( mp_fileSharing )
        Bee::setWindowStaysOnTop( mp_fileSharing, act->isChecked() );
      if( mp_log )
        Bee::setWindowStaysOnTop( mp_log, act->isChecked() );
      if( mp_screenShot )
        Bee::setWindowStaysOnTop( mp_screenShot, act->isChecked() );
      if( mp_networkTest )
        Bee::setWindowStaysOnTop( mp_networkTest, act->isChecked() );
    }
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
          QMessageBox::information( this, Settings::instance().programName(), tr( "Please save the network password in the next dialog if you want to use password without prompt." ) );
          promptConnectionPassword();
          return;
        }
      }
      else
        Settings::instance().setAskPasswordAtStartup( true );
    }
    break;
  case 18:
    {
      Settings::instance().setChatAutoSave( act->isChecked() );
      if( !Settings::instance().chatAutoSave() && (Settings::instance().chatSaveFileTransfers() || !Settings::instance().chatSaveSystemMessages()) )
      {
        if( QMessageBox::warning( this, Settings::instance().programName(),
                               tr( "There will be no saving of 'system' and 'file transfer' messages if this option is disabled." ),
                               tr( "Ok" ), tr( "Cancel" ), QString(), 0, 1 ) )
        {
          Settings::instance().setChatSaveFileTransfers( false );
          Settings::instance().setChatSaveSystemMessages( false );
          mp_actSaveFileTransferMessages->setChecked( false );
          mp_actSaveSystemMessages->setChecked( false );
        }
      }
    }
    break;
  case 19:
    Settings::instance().setShowNotificationOnTray( act->isChecked() );
    break;
  case 20:
    {
      Settings::instance().setAutoUserAway( act->isChecked() );
      if( act->isChecked() )
      {
        int away_timeout = QInputDialog::getInt( this, Settings::instance().programName(),
                              tr( "How many minutes of idle %1 can wait before changing status to away?" ).arg( Settings::instance().programName() ),
                              Settings::instance().userAwayTimeout(), 1, 30, 1, &ok );
        if( ok && away_timeout > 0 )
          Settings::instance().setUserAwayTimeout( away_timeout );

        if( beeApp )
          beeApp->setIdleTimeout( Settings::instance().userAwayTimeout() );
      }
    }
    break;
  case 21:
    Settings::instance().setShowUserPhoto( act->isChecked() );
    refresh_users = true;
    break;
  case 23:
    {
      Settings::instance().setChatFont( QApplication::font(), false );
      updateChatFont();
    }
    break;
  case 24:
    Settings::instance().setLoadOnTrayAtStartup( act->isChecked() );
    break;
  case 25:
    Settings::instance().setShowVCardOnRightClick( act->isChecked() );
    break;
  case 26:
    {
      Settings::instance().setResetGeometryAtStartup( act->isChecked() );
      mp_actSaveWindowGeometry->setDisabled( Settings::instance().resetGeometryAtStartup() );
      foreach( GuiFloatingChat* fl_chat, m_floatingChats )
        fl_chat->setSaveGeometryDisabled( Settings::instance().resetGeometryAtStartup() );
    }
    break;
  case 27:
    {
#if QT_VERSION >= 0x050000
      int num_messages = QInputDialog::getInt( qApp->activeWindow(), Settings::instance().programName(),
#else
      int num_messages = QInputDialog::getInteger( qApp->activeWindow(), Settings::instance().programName(),
#endif
                                                   tr( "Please select the maximum number of messages to be showed" ) + QString( "\n" )
                                                   + tr( "(current: %1, default: %2, all: -1, none: 0)" )
                                                       .arg( Settings::instance().chatMessagesToShow() )
                                                       .arg( Settings::instance().defaultChatMessagesToShow() ),
                                                   Settings::instance().chatMessagesToShow(),
                                                   -1, 2000, 10, &ok );
      if( ok )
      {
        Settings::instance().setChatMessagesToShow( num_messages );
        refresh_chat = true;
      }
      setChatMessagesToShowInAction( act );
    }
    break;
  case 28:
    {
      Settings::instance().setEnableMaximizeButton( act->isChecked() );
      if( Settings::instance().enableMaximizeButton() )
        setWindowFlags( windowFlags() | Qt::WindowMaximizeButtonHint );
      else
        setWindowFlags( windowFlags() & ~Qt::WindowMaximizeButtonHint );
      if( !isVisible() )
        show();
    }
    break;
  case 29:
    Settings::instance().setKeyEscapeMinimizeInTray( act->isChecked() );
    break;
  case 30:
    Settings::instance().setConfirmOnDownloadFile( act->isChecked() );
    break;
  case 31:
    Settings::instance().setUseFontEmoticons( act->isChecked() );
    mp_actSelectEmoticonSourcePath->setEnabled( !act->isChecked() );
    updateEmoticons();
    refresh_chat = true;
    break;
  case 32:
    Settings::instance().setSaveUserList( act->isChecked() );
    mp_actRemoveInactiveUsers->setEnabled( act->isChecked() );
    break;
  case 33:
    {
      Settings::instance().setRemoveInactiveUsers( act->isChecked() );
      if( act->isChecked() )
      {
#if QT_VERSION >= 0x050000
        int num_days = QInputDialog::getInt( qApp->activeWindow(), Settings::instance().programName(),
#else
        int num_days = QInputDialog::getInteger( qApp->activeWindow(), Settings::instance().programName(),
#endif
                                                     tr( "Please select the number of inactive days before user is removed" ),
                                                     Settings::instance().maxDaysOfUserInactivity(),
                                                     2, 365, 5, &ok );
        if( ok )
        {
          Settings::instance().setMaxDaysOfUserInactivity( num_days );
          setMaxInactivityDaysInAction( act );
        }
      }
    }
    break;
  case 34:
    Settings::instance().setBeepOnNewMessageArrived( act->isChecked() );
    break;
  case 35:
    Settings::instance().setShowMinimizedAtStartup( act->isChecked() );
    break;
  case 36:
    Settings::instance().setPromptOnCloseEvent( act->isChecked() );
    break;
  case 37:
    Settings::instance().setShowUserStatusDescription( act->isChecked() );
    refresh_users = true;
    break;
  case 38:
    Settings::instance().setShowUserStatusBackgroundColor( act->isChecked() );
    refresh_users = true;
    break;
  case 39:
    // FREE
    break;
  case 40:
    Settings::instance().setShowOnlyMessageNotificationOnTray( act->isChecked() );
    break;
  case 41:
    Settings::instance().setChatUseYourNameInsteadOfYou( act->isChecked() );
    refresh_chat = true;
    break;
  case 42:
    {
      Settings::instance().setShowChatToolbar( act->isChecked() );
      foreach( GuiFloatingChat* fl_chat, m_floatingChats )
        fl_chat->setChatToolbarVisible( Settings::instance().showChatToolbar() );
    }
    break;
  case 43:
    Settings::instance().setCheckNewVersionAtStartup( act->isChecked() );
    break;
  case 44:
    Settings::instance().setPostUsageStatistics( act->isChecked() );
    break;
  case 45:
    Settings::instance().setAskChangeUserAtStartup( act->isChecked() );
    break;
  case 46:
    Settings::instance().setShowChatMessageOnTray( act->isChecked() );
    break;
  case 47:
    Settings::instance().setChatClearAllReadMessages( act->isChecked() );
    break;
  case 48:
    Settings::instance().setShowFileTransferCompletedOnTray( act->isChecked() );
    break;
  case 49:
    Settings::instance().setSortUsersAscending( act->isChecked() );
    refresh_users = true;
    break;
  case 50:
    Settings::instance().setUserSortingMode( 0 );
    refresh_users = true;
    break;
  case 51:
    Settings::instance().setUserSortingMode( 1 );
    refresh_users = true;
    break;
  case 52:
    Settings::instance().setUserSortingMode( 2 );
    refresh_users = true;
    break;
  case 53:
    Settings::instance().setUserSortingMode( 3 );
    refresh_users = true;
    break;
  case 54:
    Settings::instance().setShowPresetMessages( act->isChecked() );
    break;
  case 55:
    Settings::instance().setShowTextInModeRTL( act->isChecked() );
    refresh_chat = true;
    break;
  case 56:
    Settings::instance().setPlayBuzzSound( act->isChecked() );
    break;
  case 57:
    Settings::instance().setUserRecognitionMethod( Settings::RecognizeByAccountAndDomain );
    showRestartConnectionAlertMessage();
    break;
  case 58:
    Settings::instance().setUserRecognitionMethod( Settings::RecognizeByAccount );
    showRestartConnectionAlertMessage();

    break;
  case 59:
    Settings::instance().setUserRecognitionMethod( Settings::RecognizeByNickname );
    showRestartConnectionAlertMessage();
    break;
#ifdef BEEBEEP_USE_SHAREDESKTOP
  case 60:
    {
      Settings::instance().setEnableShareDesktop( act->isChecked() );
      if( !act->isChecked() )
      {
        if( beeCore->shareDesktopIsActive( ID_INVALID ) )
          beeCore->stopShareDesktop();
      }
    }
    break;
  case 61:
    {
      int capture_delay = QInputDialog::getInt( this, Settings::instance().programName(), act->text() + QString( " (ms)" ),
                                                Settings::instance().shareDesktopCaptureDelay(), 1100, 8000, 300, &ok );
      if( ok )
        Settings::instance().setShareDesktopCaptureDelay( capture_delay );
    }
    break;
  case 62:
    {
      QString image_type = QInputDialog::getItem( this, Settings::instance().programName(),
                                                  act->text() + QString( "\n(%1)" ).arg( tr( "jpg for photo, png for presentation" ) ),
                                                  ImageOptimizer::instance().imageTypes(),
                                                  qMax( 0, ImageOptimizer::instance().imageTypes().indexOf( Settings::instance().shareDesktopImageType() ) ),
                                                  false, &ok );
      if( ok )
        Settings::instance().setShareDesktopImageType( image_type );
    }
    break;
  case 63:
    {
      int image_quality = QInputDialog::getInt( this, Settings::instance().programName(),
                                                act->text() + QString( "\n(%1)" ).arg( tr( "-1 default, 10 low, 60 medium, 100 high" ) ),
                                                Settings::instance().shareDesktopImageQuality(), -1, 100, 10, &ok );
      if( ok )
        Settings::instance().setShareDesktopImageQuality( image_quality );
    }
    break;
  case 64:
    Settings::instance().setShareDesktopFitToScreen( act->isChecked() );
    break;
#endif
  case 65:
    {
      int delay_connection = QInputDialog::getInt( this, Settings::instance().programName(),
                                              act->text() + QString( "\n(%1)" ).arg( tr( "milliseconds, 5000 default" ) ),
                                              Settings::instance().delayConnectionAtStartup(), 3000, 60000, 1000, &ok );
      if( ok )
        Settings::instance().setDelayConnectionAtStartup( delay_connection );
    }
    break;
  case 66:
    Settings::instance().setSendOfflineMessagesToDefaultChat( act->isChecked() );
    break;
  case 67:
    Settings::instance().setChatUseColoredUserNames( act->isChecked() );
    refresh_chat = true;
    break;
  case 68:
    Settings::instance().setSaveGeometryOnExit( act->isChecked() );
    break;
  case 69:
    Settings::instance().setShowUsersOnConnection( act->isChecked() );
    break;
  case 70:
    Settings::instance().setAlwaysOpenChatOnNewMessageArrived( act->isChecked() );
    break;
  case 71:
    {
 #if QT_VERSION >= 0x050000
      int opacity_level = QInputDialog::getInt( qApp->activeWindow(), Settings::instance().programName(),
#else
      int opacity_level = QInputDialog::getInteger( qApp->activeWindow(), Settings::instance().programName(),
#endif
                                                 tr( "Please select the opacity percentage of inactive chat window (default: %1%)" ).arg( Settings::instance().chatInactiveWindowDefaultOpacityLevel() ),
                                                 Settings::instance().chatInactiveWindowOpacityLevel(),
                                                 10, 100, 5, &ok );
      if( ok )
        Settings::instance().setChatInactiveWindowOpacityLevel( opacity_level );
      setChatInactiveWindowOpacityLevelInAction( act );
    }
    break;
  case 72:
    {
      QColor c = QColorDialog::getColor( QColor( Settings::instance().chatBackgroundColor() ), this );
      if( c.isValid() )
      {
        Settings::instance().setChatBackgroundColor( c.name() );
        updateChatColors();
      }
    }
    break;
  case 73:
    {
      QColor c = QColorDialog::getColor( QColor( Settings::instance().chatDefaultTextColor() ), this );
      if( c.isValid() )
      {
        Settings::instance().setChatDefaultTextColor( c.name() );
        updateChatColors();
      }
    }
    break;
  case 74:
    {
      QColor c = QColorDialog::getColor( QColor( Settings::instance().chatSystemTextColor() ), this );
      if( c.isValid() )
      {
        Settings::instance().setChatSystemTextColor( c.name() );
        updateChatColors();
        QMessageBox::information( this, Settings::instance().programName(), tr( "You must close and reopen the chat windows to see the changes applied." ), tr( "Ok" ) );
      }
    }
    break;
  case 75:
    Settings::instance().setEnableDefaultChatNotifications( act->isChecked() );
    break;
  case 76:
    Settings::instance().setUseMessageTimestampWithAP( act->isChecked() );
    refresh_chat = true;
    break;
  case 77:
    {
      if( QMessageBox::question( this, Settings::instance().programName(), tr( "Do you really want to apply the new theme?" ), tr( "Yes" ), tr( "No" ), QString(), 1, 1 ) == 0 )
      {
        Settings::instance().setUseDarkStyle( act->isChecked() );
        Settings::instance().resetAllColors();
        QTimer::singleShot( 0, this, SLOT( loadStyle() ) );
      }
    }
    break;
  case 78:
    Settings::instance().setalwaysShowFileTransferProgress( act->isChecked() );
    break;
  case 79:
    Settings::instance().setShowChatsOnConnection( act->isChecked() );
    break;
  case 80:
    Settings::instance().setRaiseMainWindowOnNewMessageArrived( act->isChecked() );
    break;
  case 81:
    {
      Settings::instance().setChatSaveUnsentMessages( act->isChecked() );
      if( Settings::instance().chatSaveUnsentMessages() && (!Settings::instance().saveUserList() || !Settings::instance().saveGroupList()) )
      {
        if( QMessageBox::question( this, Settings::instance().programName(),
                               tr( "Saving unsent messages may fail if 'Save users' and 'Save groups' options are not enabled. Do you want to enable them?" ),
                               tr( "Yes" ), tr( "No" ), QString(), 0, 1 ) == 0 )
        {
          Settings::instance().setSaveUserList( true );
          Settings::instance().setSaveGroupList( true );
          mp_actSaveUserList->setChecked( true );
          mp_actSaveGroupList->setChecked( true );
        }
      }
    }
    break;
  case 82:
    Settings::instance().setChatSaveFileTransfers( act->isChecked() );
    showCheckSaveChatMessages();
    break;
  case 83:
    Settings::instance().setChatSaveSystemMessages( act->isChecked() );
    showCheckSaveChatMessages();
    break;
  case 84:
    {
#if QT_VERSION >= 0x050000
      int save_max_lines = QInputDialog::getInt( qApp->activeWindow(), Settings::instance().programName(),
#else
      int save_max_lines = QInputDialog::getInteger( qApp->activeWindow(), Settings::instance().programName(),
#endif
                                                 tr( "Please select the maximum number of lines to be saved in the chat (current: %1)." ).arg( Settings::instance().chatMaxLineSaved() ),
                                                 Settings::instance().chatMaxLineSaved(),
                                                 100, 50000, 100, &ok );
      if( ok )
        Settings::instance().setChatMaxLineSaved( save_max_lines );
      setChatMaxLinesToSaveInAction( act );
    }
    break;
  case 85:
    {
#if QT_VERSION >= 0x050000
      int cc_days = QInputDialog::getInt( qApp->activeWindow(), Settings::instance().programName(),
#else
      int cc_days = QInputDialog::getInteger( qApp->activeWindow(), Settings::instance().programName(),
#endif
                                          tr( "Please select the number of days that items (such as images) can remain cached." ) +
                                          QString( "\n(%1)" ).arg( tr( "current: %1, never clear: -1, always clear: 0" ).arg( Settings::instance().clearCacheAfterDays() ) ),
                                          Settings::instance().clearCacheAfterDays(), -1, 999, 10, &ok );
      if( ok )
      {
        Settings::instance().setClearCacheAfterDays( cc_days );
        setClearCacheAfterDaysInAction( act );
      }
    }
    break;
  case 86:
    Settings::instance().setDownloadInUserFolder( act->isChecked() );
    break;
  case 87:
    Settings::instance().setShowUsersInWorkgroups( act->isChecked() );
    refresh_users = true;
    break;
  case 88:
    {
#if QT_VERSION >= 0x050000
      int max_files_in_queue = QInputDialog::getInt( qApp->activeWindow(), Settings::instance().programName(),
#else
      int max_files_in_queue = QInputDialog::getInteger( qApp->activeWindow(), Settings::instance().programName(),
#endif
                                  tr( "Please select the maximum number of files you can queue up for the transfer (current: %1)." ).arg( Settings::instance().maxQueuedDownloads() ),
                                  Settings::instance().maxQueuedDownloads(),
                                  1, 9999, 10, &ok );
      if( ok )
      {
        Settings::instance().setMaxQueuedDownloads( max_files_in_queue);
        setMaxQueuedDownloadsInAction( act );
      }
    }
    break;
  case 89:
    {
      QColor c = QColorDialog::getColor( QColor( Settings::instance().chatQuoteTextColor() ), this );
      if( c.isValid() )
      {
        Settings::instance().setChatQuoteTextColor( c.name() );
        refresh_chat = true;
      }
    }
    break;
  case 90:
    {
      QColor c = QColorDialog::getColor( QColor( Settings::instance().chatQuoteBackgroundColor() ), this );
      if( c.isValid() )
      {
        Settings::instance().setChatQuoteBackgroundColor( c.name() );
        refresh_chat = true;
      }
    }
    break;
  case 91:
    {
      Settings::instance().setChatOnSendingMessage( Settings::SkipOnSendingMessage );
      foreach( GuiFloatingChat* fl_chat, m_floatingChats )
        fl_chat->guiChat()->updateOnSendingMessage();
    }
    break;
  case 92:
    {
      Settings::instance().setChatOnSendingMessage( Settings::MinimizeChatOnSendingMessage  );
      foreach( GuiFloatingChat* fl_chat, m_floatingChats )
        fl_chat->guiChat()->updateOnSendingMessage();
    }
    break;
  case 93:
    {
      Settings::instance().setChatOnSendingMessage( Settings::CloseChatOnSendingMessage );
      foreach( GuiFloatingChat* fl_chat, m_floatingChats )
        fl_chat->guiChat()->updateOnSendingMessage();
    }
    break;
  case 94:
    {
#if QT_VERSION >= 0x050000
      int max_duration = QInputDialog::getInt( qApp->activeWindow(), Settings::instance().programName(),
#else
      int max_duration = QInputDialog::getInteger( qApp->activeWindow(), Settings::instance().programName(),
#endif
                                  tr( "Please select the maximum duration (in seconds) that a voice message can have (current: %1)." ).arg( Settings::instance().voiceMessageMaxDuration() ),
                                  Settings::instance().voiceMessageMaxDuration(),
                                  10, 900, 10, &ok );
      if( ok )
        Settings::instance().setVoiceMessageMaxDuration( max_duration );
    }
    break;
  case 95:
    Settings::instance().setUseVoicePlayer( act->isChecked() );
    break;
  case 96:
    Settings::instance().setDisableBeepInUserStatusBusy( act->isChecked() );
    break;
  case 97:
    Settings::instance().setResumeFileTransfer( act->isChecked() );
    break;
  case 98:
    Settings::instance().setBeepInActiveWindowAlso( act->isChecked() );
    break;
  case 99:
    break;
  case 100:
    {
#if QT_VERSION >= 0x050000
      int cc_days = QInputDialog::getInt( qApp->activeWindow(), Settings::instance().programName(),
#else
      int cc_days = QInputDialog::getInteger( qApp->activeWindow(), Settings::instance().programName(),
#endif
                                              tr( "Please select the number of days that partially downloaded files can remain cached." ) +
                                              QString( "\n(%1)" ).arg( tr( "current: %1, never clear: -1, always clear: 0" ).arg( Settings::instance().removePartiallyDownloadedFilesAfterDays() ) ),
                                              Settings::instance().removePartiallyDownloadedFilesAfterDays(), -1, 999, 10, &ok );
      if( ok )
      {
        Settings::instance().setRemovePartiallyDownloadedFilesAfterDays( cc_days );
        setClearPartiallyDownloadedFilesAfterDaysInAction( act );
      }
    }
    break;
  case 101:
    Settings::instance().setKeepModificationDateOnFileTransferred( act->isChecked() );
    break;
  case 102:
    Settings::instance().setUseUserFirstNameFirstInFullName( !act->isChecked() );
    break;
  case 103:
    Settings::instance().setResetMinimumWidthForStyle( act->isChecked() );
    setMinimumWidthForStyle();
    break;
  case 104:
    EmoticonManager::instance().clearRecentEmoticons();
    updateEmoticons();
    break;
  case 105:
    Settings::instance().setUseHiResEmoticons( act->isChecked() );
    updateEmoticons();
    break;
  case 106:
    EmoticonManager::instance().clearFavoriteEmoticons();
    updateEmoticons();
    break;
  case 107:
    Settings::instance().setEnableVisualNotificationsInChatWindow( act->isChecked() );
    break;
  default:
    qWarning() << "GuiMain::settingsChanged(): error in setting id" << act->data().toInt();
  }

  if( refresh_users )
    mp_userList->updateUsers();

  if( refresh_chat )
  {
    QApplication::setOverrideCursor( Qt::WaitCursor );
    QApplication::processEvents();
    mp_chatList->updateChats();
    foreach( GuiFloatingChat* fl_chat, m_floatingChats )
    {
      Chat c = ChatManager::instance().chat( fl_chat->guiChat()->chatId() );
      if( c.isValid() )
        fl_chat->setChat( c );
    }
    QApplication::restoreOverrideCursor();
  }

  if( settings_data_id > 0 && settings_data_id != 99 )
  {
    if( act->isCheckable() )
    {
      QApplication::processEvents(); // Menu is closing and status bar will be reset -> force events to display next messages
      if( act->isChecked() )
        showMessage( tr( "Option enabled." ), 3000 );
      else
        showMessage( tr( "Option disabled." ), 3000 );
    }
    Settings::instance().save();
  }
}

void GuiMain::showCheckSaveChatMessages()
{
  if( !Settings::instance().chatAutoSave() && (Settings::instance().chatSaveFileTransfers() || !Settings::instance().chatSaveSystemMessages()) )
  {
    if( QMessageBox::warning( this, Settings::instance().programName(),
                              tr( "There will be no saving of 'system' and 'file transfer' messages if this option is disabled." ),
                              tr( "Ok" ), tr( "Cancel" ), QString(), 0, 1 ) )
    {
      Settings::instance().setChatSaveFileTransfers( false );
      Settings::instance().setChatSaveSystemMessages( false );
      mp_actSaveFileTransferMessages->setChecked( false );
      mp_actSaveSystemMessages->setChecked( false );
    }
  }
}

void GuiMain::setChatMessagesToShowInAction( QAction* act )
{
  act->setText( tr( "Show only last %1 messages" ).arg( Settings::instance().chatMessagesToShow() >= 0 ? Settings::instance().chatMessagesToShow() : 800 ) );
  act->setChecked( Settings::instance().chatMessagesToShow() >= 0 );
}

void GuiMain::setMaxInactivityDaysInAction( QAction* act )
{
  act->setText( tr( "Remove users after %1 days of inactivity" ).arg( Settings::instance().maxDaysOfUserInactivity() ) );
  act->setEnabled( Settings::instance().saveUserList() );
}

void GuiMain::setChatInactiveWindowOpacityLevelInAction( QAction* act )
{
  act->setChecked( Settings::instance().chatInactiveWindowOpacityLevel() < 100 );
  act->setText( tr( "Show inactive chat window with %1% opacity" ).arg( Settings::instance().chatInactiveWindowOpacityLevel() ) );
}

void GuiMain::setChatMaxLinesToSaveInAction( QAction* act )
{
  act->setText( tr( "Save maximum %1 lines of chat" ).arg( Settings::instance().chatMaxLineSaved() ) );
  act->setEnabled( Settings::instance().chatAutoSave() );
  act->setChecked( Settings::instance().chatMaxLineSaved() > 0 );
}

void GuiMain::setClearCacheAfterDaysInAction( QAction* act )
{
  act->setText( tr( "Clean the cache from items older than %1 days" ).arg( Settings::instance().clearCacheAfterDays() >= 0 ? Settings::instance().clearCacheAfterDays() : 96 ) );
  act->setEnabled( Settings::instance().chatAutoSave() );
  act->setChecked( Settings::instance().clearCacheAfterDays() >= 0 );
}

void GuiMain::setClearPartiallyDownloadedFilesAfterDaysInAction( QAction* act )
{
  act->setText( tr( "Delete partially downloaded files after %1 days" ).arg( Settings::instance().removePartiallyDownloadedFilesAfterDays() >= 0 ? Settings::instance().removePartiallyDownloadedFilesAfterDays() : 5 ) );
  act->setChecked( Settings::instance().removePartiallyDownloadedFilesAfterDays() >= 0 );
}

void GuiMain::setMaxQueuedDownloadsInAction( QAction* act )
{
  act->setText( tr( "Add up to %1 files to the transfer queue" ).arg( Settings::instance().maxQueuedDownloads() ) );
  act->setChecked( Settings::instance().maxQueuedDownloads() > 0 );
}

void GuiMain::sendMessage( VNumber chat_id, const QString& msg, bool is_source_code )
{
#ifdef BEEBEEP_DEBUG
  int num_messages = beeCore->sendChatMessage( chat_id, msg, false, false, is_source_code );
  qDebug() << num_messages << "messages sent";
#else
  beeCore->sendChatMessage( chat_id, msg, false, false, is_source_code );
#endif
  mp_chatList->updateChat( ChatManager::instance().chat( chat_id ) ); // to sort the chats
}

void GuiMain::showAlertForMessage( const Chat& c, const ChatMessage& cm )
{
  if( cm.isImportant() )
  {
    playBuzz();
  }
  else
  {
    if( Settings::instance().beepOnNewMessageArrived() &&
        !(Settings::instance().localUser().status() == User::Busy && Settings::instance().disableBeepInUserStatusBusy()) )
    {
      playBeep();
    }
  }

  bool show_message_in_tray = true;

  GuiFloatingChat* fl_chat = floatingChat( c.id() );
  if( fl_chat )
  {
    fl_chat->setMainIcon( true );
    if( Settings::instance().enableVisualNotificationsInChatWindow() )
      QApplication::alert( fl_chat, 0 );
    if( Settings::instance().raiseOnNewMessageArrived() || cm.isImportant() )
    {
      fl_chat->raiseOnTop();
      show_message_in_tray = false;
    }
  }

  if( Settings::instance().raiseMainWindowOnNewMessageArrived() )
    raiseOnTop();

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

        if( c.isDefault() )
          msg = QString( "%1 %2: %3" ).arg( u.name(), tr( "to all" ), txt );
        else if( c.isGroup() )
          msg = QString( "%1 %2 %3: %4" ).arg( u.name(), tr( "to" ), c.name(), txt );
        else
          msg = QString( "%1 %2: %4" ).arg( u.name(), tr( "to you" ), txt );

        long_time_show = true;
      }
      else
      {
        QString pre_msg = tr( "New message from" );
        if( c.isDefault() )
          msg = QString( "%1 %2 %3" ).arg( pre_msg, u.name(), tr( "to all" ) );
        else if( c.isGroup() )
          msg = QString( "%1 %2 %3 %4" ).arg( pre_msg, u.name(), tr( "to" ), c.name() );
        else
          msg = QString( "%1 %2 %3" ).arg( pre_msg, u.name(), tr( "to you" ) );
      }
    }
    else
      msg = tr( "New message arrived" );

    mp_trayIcon->showNewMessageArrived( c.id(), msg, long_time_show );
  }
}

void GuiMain::onNewChatMessage( const Chat& c, const ChatMessage& cm )
{
  if( !c.isValid() )
  {
    qWarning() << "Invalid chat" << c.name() << "found in GuiMain::onNewChatMessage(...)";
    return;
  }

  if( c.isDefault() )
  {
    if( mp_home->addSystemMessage( cm ) && mp_tabMain->currentWidget() != mp_home )
    {
      m_unreadActivities++;
      updateTabTitles();
    }
  }

  bool floating_chat_created = false;
  bool alert_can_be_notified = false;

  if( c.isDefault() )
    alert_can_be_notified = cm.alertCanBeSent() && Settings::instance().enableDefaultChatNotifications();
  else if( c.isGroup() )
    alert_can_be_notified = cm.alertCanBeSent() && !Settings::instance().isNotificationDisabledForGroup( c.privateId() );
  else
    alert_can_be_notified = cm.alertCanBeSent();

  GuiFloatingChat* fl_chat = floatingChat( c.id() );
  bool open_chat_window = Settings::instance().alwaysOpenChatOnNewMessageArrived() || (!cm.isFromLocalUser() && !cm.isFromSystem() && cm.isImportant() && c.isPrivate());

  if( !fl_chat && open_chat_window && alert_can_be_notified )
  {
    fl_chat = createFloatingChat( c );
    floating_chat_created = true;
    fl_chat->show();
    fl_chat->guiChat()->ensureLastMessageVisible();
    if( !Settings::instance().raiseOnNewMessageArrived() && !cm.isImportant() )
      fl_chat->showMinimized();
  }

  if( fl_chat && !floating_chat_created )
    fl_chat->showChatMessage( c, cm );

  if( fl_chat && !floating_chat_created && fl_chat->isActiveWindow() )
  {
    if( alert_can_be_notified && (cm.isImportant() || Settings::instance().beepInActiveWindowAlso()) )
    {
      if( cm.isImportant() )
        playBuzz();
      else
        playBeep();
    }
    readAllMessagesInChat( c.id() );
  }
  else
  {
    if( alert_can_be_notified )
      showAlertForMessage( c, cm );
  }
}

void GuiMain::updateUser( const User& u )
{
  mp_userList->setUser( u, false );
  mp_chatList->updateUser( u );
  mp_groupList->updateUser( u );
  foreach( GuiFloatingChat* fl_chat, m_floatingChats )
    fl_chat->updateUser( u );
  mp_fileTransfer->updateUser( u );
}

void GuiMain::searchUsers()
{
  GuiNetwork gn( this );
  gn.setModal( true );
  gn.loadSettings();
  gn.setSizeGripEnabled( true );
  gn.show();

  if( gn.exec() != QDialog::Accepted )
    return;

  if( gn.restartConnection() )
  {
    showRestartConnectionAlertMessage();
    return;
  }

  if( !beeCore->isConnected() )
    return;

#ifdef BEEBEEP_USE_MULTICAST_DNS
  if( Settings::instance().useMulticastDns() )
    beeCore->startDnsMulticasting();
  else
    beeCore->stopDnsMulticasting();
#endif

  QMetaObject::invokeMethod( this, "sendBroadcastMessage", Qt::QueuedConnection );
}

void GuiMain::showWritingUser( const User& u, VNumber chat_id )
{
  QString msg = tr( "%1 is writing..." ).arg( u.name() );
  GuiFloatingChat* fl_chat = floatingChat( chat_id );
  if( fl_chat )
    fl_chat->showStatusMessage( msg, Settings::instance().writingTimeout() );
}

void GuiMain::setUserStatusSelected( int user_status )
{
  if( user_status == User::Offline )
  {
    if( beeCore->isConnected() )
    {
      if( !Settings::instance().promptOnCloseEvent() || QMessageBox::question( this, Settings::instance().programName(),
                                   tr( "Do you want to disconnect from %1 network?" ).arg( Settings::instance().programName() ),
                                   tr( "Yes" ), tr( "No" ), QString(), 1, 1 ) == 0 )
      {
        disconnectFromNetwork();
        return;
      }
    }
    updateStatusIcon();
  }
  else
  {
    if( beeCore->isConnected() )
    {
      beeCore->setLocalUserStatus( user_status );
    }
    else
    {
      Settings::instance().setLocalUserStatus( static_cast<User::Status>(user_status) );
      startCore();
    }
  }
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
  if( !beeCore->isConnected() )
    status_type = User::Offline;
  else
    status_type = Settings::instance().localUser().status();

  mp_menuStatus->setIcon( Bee::avatarForUser( Settings::instance().localUser(), Settings::instance().avatarIconSize(), true ) );
  QString tip = tr( "You are %1%2" ).arg( Bee::userStatusToString( status_type ) )
      .arg( (Settings::instance().localUser().statusDescription().isEmpty() ? QString( "" ) : QString( ": %1" ).arg( Settings::instance().localUser().statusDescription() ) ) );
  QAction* act = mp_menuStatus->menuAction();
  act->setToolTip( tip );
  act->setText( Bee::capitalizeFirstLetter( Bee::userStatusToString( status_type ), true ) );
  updateWindowTitle();
}

void GuiMain::changeStatusDescription()
{
  bool ok = false;
  QString status_description = QInputDialog::getText( this, Settings::instance().programName(),
                           tr( "Please insert the new status description" ), QLineEdit::Normal, Settings::instance().localUser().statusDescription(), &ok );
  if( !ok || status_description.isNull() )
    return;
  beeCore->setLocalUserStatusDescription( Settings::instance().localUser().status(), status_description, true );
  loadUserStatusRecentlyUsed();
  updateStatusIcon();
  updateLocalStatusMessage();
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

  beeCore->sendFilesFromChat( c.id(), files_path_selected );
}

void GuiMain::sendFilesFromChat( VNumber chat_id, const QStringList& file_path_list )
{
  Chat c = ChatManager::instance().chat( chat_id );
  if( !c.isValid() )
    return;

  if( c.isDefault() && !Settings::instance().chatWithAllUsersIsEnabled() )
    return;

  if( file_path_list.isEmpty() )
    return;

  beeCore->sendFilesFromChat( c.id(), file_path_list );
}

void GuiMain::sendFile( VNumber user_id )
{
  User u = UserManager::instance().findUser( user_id );
  QStringList files_path_selected = checkFilePath( "" );
  if( files_path_selected.isEmpty() )
    return;
  Chat c = ChatManager::instance().privateChatForUser( user_id );
  if( !c.isValid() )
    qWarning() << "Unable to send files to user" << user_id << "without a private chat in GuiMain::sendFile(...)";
  beeCore->sendFilesFromChat( c.id(), files_path_selected );
}

QStringList GuiMain::checkFilePath( const QString& file_path )
{
  QStringList files_path_selected;
  if( file_path.isEmpty() || !QFile::exists( file_path ) )
  {
    files_path_selected = FileDialog::getOpenFileNames( true, activeWindow(), tr( "%1 - Select a file" ).arg( Settings::instance().programName() ) + QString( " %1" ).arg( tr( "or more" ) ),
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

bool GuiMain::sendFile( const User& u, const QString& file_path, VNumber chat_id )
{
  if( !Settings::instance().enableFileTransfer() )
  {
    QMessageBox::information( activeWindow(), Settings::instance().programName(), tr( "File transfer is not enabled." ) );
    return false;
  }

  if( !beeCore->isConnected() )
  {
    QMessageBox::information( activeWindow(), Settings::instance().programName(), tr( "You are not connected." ) );
    return false;
  }

  User user_selected;

  if( !u.isValid() )
  {
    QStringList user_string_list;
    foreach( User u, UserManager::instance().userList().toList() )
    {
      if( u.isStatusConnected() )
        user_string_list.append( u.path() );
    }

    if( user_string_list.isEmpty() )
    {
      QMessageBox::information( activeWindow(), Settings::instance().programName(), tr( "There is no user connected." ) );
      return false;
    }

    bool ok = false;
    QString user_path = QInputDialog::getItem( activeWindow(), Settings::instance().programName(),
                                        tr( "Please select the user to whom you would like to send a file."),
                                        user_string_list, 0, false, &ok );
    if( !ok )
      return false;

    user_selected = UserManager::instance().findUserByPath( user_path );

    if( !user_selected.isValid() )
    {
      QMessageBox::warning( activeWindow(), Settings::instance().programName(), tr( "User not found." ) );
      return false;
    }

    Chat c = ChatManager::instance().privateChatForUser( user_selected.id() );
    chat_id = c.id();
  }
  else
    user_selected = u;

  return beeCore->sendFile( user_selected.id(), file_path, "", false, chat_id );
}

void GuiMain::sendFile( const QString& file_path )
{
  sendFile( User(), file_path, ID_INVALID );
}

bool GuiMain::askToDownloadFile( const User& u, const FileInfo& fi, const QString& download_path, bool make_questions )
{
  if( !Settings::instance().enableFileTransfer() )
  {
    QMessageBox::warning( activeWindow(), Settings::instance().programName(), tr( "File transfer is disabled. You cannot download %1." ).arg( fi.name() ) );
    return false;
  }

  int msg_result = make_questions ? 0 : 1;

  if( msg_result == 0 )
  {
    if( Settings::instance().confirmOnDownloadFile() )
    {
      if( isMinimized() || !isActiveWindow() )
      {
        if( Settings::instance().raiseMainWindowOnNewMessageArrived() )
          raiseOnTop();
        mp_actViewNewMessage->setEnabled( true );
      }
      QString msg = tr( "Do you want to download %1 (%2) from %3?" ).arg( fi.name(), Bee::bytesToString( fi.size() ), Bee::userNameToShow( u, false ) );
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
    QFileInfo qfile_info( download_path, fi.name() );
    qDebug() << "You accept to download" << fi.name() << "from" << qPrintable( u.path() );
    if( qfile_info.exists() && Settings::instance().onExistingFileAction() == Settings::GenerateNewFileName )
    {
      QString file_name = Bee::uniqueFilePath( qfile_info.absoluteFilePath(), true );
      qDebug() << "File" << qfile_info.absoluteFilePath() << "exists. Save it with new path (auto):" << qPrintable( file_name );
      qfile_info = QFileInfo( file_name );
    }
    FileInfo file_info = fi;
    file_info.setPath( qfile_info.absoluteFilePath() );
    return beeCore->downloadFile( u.id(), file_info, make_questions );
  }
  else
  {
    qDebug() << "You refuse to download" << fi.name() << "from" << u.path();
    return false;
  }
}

void GuiMain::downloadFile( const User& u, const FileInfo& fi )
{
  if( !askToDownloadFile( u, fi, Settings::instance().downloadDirectoryForUser( u ), true ) )
    beeCore->refuseToDownloadFile( u.id(), fi );
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
    if( QMessageBox::question( activeWindow(), Settings::instance().programName(),
                               tr( "You cannot download all these files at once. Do you want to download the first %1 files of the list?" )
                               .arg( Settings::instance().maxQueuedDownloads() ),
                               tr( "Yes" ), tr( "No" ), QString(), 1, 1 ) != 0 )
      return;
  }
  else if( share_file_info_list.size() > 100 )
  {
    if( QMessageBox::question( activeWindow(), Settings::instance().programName(),
                           tr( "Downloading %1 files is a hard duty. Maybe you have to wait a lot of minutes. Do yo want to continue?" ).arg( share_file_info_list.size() ),
                           tr( "Yes" ), tr( "No" ), QString(), 1, 1 ) != 0 )
      return;
  }

  foreach( SharedFileInfo sfi, share_file_info_list )
  {
    u = UserManager::instance().findUser( sfi.first );
    download_folder = Bee::convertToNativeFolderSeparator( QString( "%1/%2" ).arg( Settings::instance().downloadDirectoryForUser( u ), sfi.second.shareFolder() ) );
    if( !askToDownloadFile( u, sfi.second, download_folder, false ) )
      return;
    files_to_download++;
    if( files_to_download > Settings::instance().maxQueuedDownloads() )
      break;
  }

  showMessage( tr( "Downloading %1 files" ).arg( files_to_download ), 5000 );
}

void GuiMain::downloadSharedFile( VNumber user_id, VNumber file_id )
{
  User u = UserManager::instance().findUser( user_id );
  FileInfo file_info = FileShare::instance().networkFileInfo( user_id, file_id );

  if( u.isStatusConnected() && file_info.isValid() )
  {
    askToDownloadFile( u, file_info, Settings::instance().downloadDirectoryForUser( u ), true );
    return;
  }

  qWarning() << "Unable to download shared file" << file_id << "from user" << user_id;
  QString info_msg = tr( "File is not available for download." );
  if( u.isValid() && !u.isStatusConnected() )
    info_msg += QLatin1String( "\n" ) + tr( "%1 is not connected." ).arg( u.name() );
  info_msg += QLatin1String( "\n" ) + tr( "Please reload the list of shared files." );

  if( QMessageBox::information( activeWindow(), Settings::instance().programName(), info_msg,
                              tr( "Reload file list" ), tr( "Cancel" ), QString(), 1, 1 ) == 0 )
  {
    if( mp_fileSharing )
      mp_fileSharing->updateNetworkFileList();
  }
}

void GuiMain::downloadFolder( const User& u, const QString& folder_name, const QList<FileInfo>& file_info_list )
{
  if( !Settings::instance().enableFileTransfer() )
  {
    QMessageBox::warning( activeWindow(), Settings::instance().programName(), tr( "File transfer is disabled. You cannot download %1." ).arg( folder_name ) );
    return;
  }

  if( file_info_list.isEmpty() )
  {
    qWarning() << "Unable to download folder" << folder_name << "from user" << qPrintable( u.path() ) << "with empty file list";
    return;
  }

  int msg_result = Settings::instance().confirmOnDownloadFile() ? 0 : 1;

  if( msg_result == 0 )
  {
    QString msg = tr( "Do you want to download folder %1 (%2 files) from %3?" ).arg( folder_name ).arg( file_info_list.size() ).arg( u.name() );
    msg_result = QMessageBox::question( activeWindow(), Settings::instance().programName(), msg, tr( "No" ), tr( "Yes" ), tr( "Yes, and don't ask anymore" ), 0, 0 );
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
      download_folder = Bee::convertToNativeFolderSeparator( QString( "%1/%2" ).arg( Settings::instance().downloadDirectoryForUser( u ), fi.shareFolder() ) );
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
    beeCore->refuseToDownloadFolder( u.id(), folder_name, file_info_list.first().chatPrivateId() );
  }
}

void GuiMain::selectDownloadDirectory()
{
  QString download_directory_path = FileDialog::getExistingDirectory( activeWindow(),
                                                                       tr( "%1 - Select the download folder" )
                                                                       .arg( Settings::instance().programName() ),
                                                                       Settings::instance().downloadDirectory() );
  if( download_directory_path.isEmpty() )
    return;

  Settings::instance().setDownloadDirectory( download_directory_path );
  QMessageBox::information( this, Settings::instance().programName(),
                            QString( "%1<br>%2%3%4" ).arg( tr( "The files will be downloaded to the folder:" ) )
                                                     .arg( download_directory_path )
                                                     .arg( Bee::nativeFolderSeparator() )
                                                     .arg( Settings::instance().downloadInUserFolder() ? Bee::convertToNativeFolderSeparator( QString( "<user's name>" ) ) : "" ),
                            tr( "Ok" ) );
}

void GuiMain::showTipOfTheDay()
{
  openWebUrl( Settings::instance().tipsWebSite() );
}

void GuiMain::showFactOfTheDay()
{
  openWebUrl( Settings::instance().factWebSite() );
}

void GuiMain::showFaq()
{
  openWebUrl( Settings::instance().faqWebSite() );
}

void GuiMain::showChat( VNumber chat_id )
{
  Chat c = ChatManager::instance().chat( chat_id );
  if( !c.isValid() )
  {
    qWarning() << "Invalid chat" << chat_id << "found in GuiMain::showChat(...)";
    return;
  }

  GuiFloatingChat* fl_chat = createFloatingChat( c );
  if( !fl_chat )
    return;

  fl_chat->show();
  fl_chat->showUp();
  fl_chat->setFocusInChat();
}

void GuiMain::changeVCard()
{
  GuiEditVCard gvc;
  gvc.setModal( true );
  gvc.setUser( Settings::instance().localUser() );
  gvc.setSizeGripEnabled( true );
  gvc.show();
  if( gvc.exec() == QDialog::Accepted )
  {
    beeCore->setLocalUserVCard( gvc.userColor(), gvc.vCard() );
    if( gvc.regenerateUserHash() )
    {
      beeCore->regenerateLocalUserHash();
      QMessageBox::information( this, Settings::instance().programName(), tr( "You must restart %1 to apply these changes." ).arg( Settings::instance().programName() ) );
    }
  }
}

void GuiMain::showLocalUserVCard()
{
  showVCard( ID_LOCAL_USER );
}

void GuiMain::showVCard( VNumber user_id )
{
  User u = UserManager::instance().findUser( user_id );
  if( !u.isValid() )
    return;

  GuiVCard* gvc = new GuiVCard( this );
  connect( gvc, SIGNAL( showChat( VNumber ) ), this, SLOT( showChat( VNumber ) ) );
  connect( gvc, SIGNAL( sendFile( VNumber ) ), this, SLOT( sendFile( VNumber ) ) );
  connect( gvc, SIGNAL( changeUserColor( VNumber, const QString& ) ), this, SLOT( changeUserColor( VNumber, const QString& ) ) );
  connect( gvc, SIGNAL( toggleFavorite( VNumber ) ), this, SLOT( toggleUserFavorite( VNumber ) ) );
  connect( gvc, SIGNAL( removeUser( VNumber ) ), this, SLOT( removeUserFromList( VNumber ) ) );
  connect( gvc, SIGNAL( buzzUser( VNumber ) ), this, SLOT( sendBuzzToUser( VNumber ) ) );
  connect( gvc, SIGNAL( sendHelpRequestToUser( VNumber ) ), this, SLOT( sendHelpMessageToUser( VNumber ) ) );
  gvc->setVCard( u, ChatManager::instance().privateChatForUser( u.id() ).id(), beeCore->isConnected() );

  QPoint cursor_pos = QCursor::pos();
  QRect screen_rect = qApp->desktop()->availableGeometry( cursor_pos );
  int diff_margin = cursor_pos.x() + gvc->size().width() - screen_rect.width();
  if( diff_margin > 0 )
    cursor_pos.setX( cursor_pos.x() - gvc->size().width() - 16 );
  diff_margin = cursor_pos.y() + gvc->size().height() - screen_rect.height();
  if( diff_margin > 0 )
    cursor_pos.setY( cursor_pos.y() - gvc->size().height() + 24 );
  gvc->move( cursor_pos );
  gvc->show();
  gvc->adjustSize();
  gvc->setFixedSize( gvc->size() );
}

void GuiMain::updadePluginMenu()
{
  mp_menuPlugins->clear();
  QAction* act;
  mp_menuPlugins->addAction( IconManager::instance().icon( "plugin.png" ), tr( "Plugin Manager..." ), this, SLOT( showPluginManager() ) );

  QString help_data_ts = tr( "is a plugin developed by" );
  QString help_data_format = QString( "<p>%1 <b>%2</b> %3 <b>%4</b>.<br><i>%5</i></p><br>" );

  mp_menuPlugins->addAction( mp_actViewScreenShot );

  bool copymastro_available = false;
  QString copy_mastro_path = "";
#ifdef Q_OS_WIN
  copy_mastro_path = QString( "%1\\%2" ).arg( Settings::instance().pluginPath(), QString( "CopyMastro.exe" ) );
  copymastro_available = QFile::exists( copy_mastro_path );
  if( !copymastro_available )
  {
    copy_mastro_path = QString( "C:\\Program Files (x86)\\CopyMastro\\CopyMastro.exe" );
    copymastro_available = QFile::exists( copy_mastro_path );
  }
#endif

  if( copymastro_available )
  {
    qDebug() << "CopyMastro is found:" << qPrintable( copy_mastro_path );
    act = mp_menuPlugins->addAction( IconManager::instance().icon( "CopyMastro.png" ), "CopyMastro", this, SLOT( startExternalApplicationFromActionData() ) );
    act->setToolTip( tr( "Start the new application to copy file and folders by Marco Mastroddi" ) );
    act->setData( copy_mastro_path );
  }

  if( PluginManager::instance().textMarkers().size() > 0 )
  {
    mp_menuPlugins->addSeparator();

    foreach( TextMarkerInterface* text_marker, PluginManager::instance().textMarkers() )
    {
      act = mp_menuPlugins->addAction( text_marker->name(), this, SLOT( showPluginHelp() ) );

      act->setData( help_data_format
                  .arg( IconManager::instance().toHtml( (text_marker->icon().isNull() ? "images/plugin.png" : text_marker->iconFileName()), "*P*" ),
                        text_marker->name(), help_data_ts, text_marker->author(), text_marker->help() ) );
      act->setIcon( text_marker->icon() );
      act->setEnabled( text_marker->isEnabled() );
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
  GuiPluginManager gpm;
  gpm.setModal( true );
  gpm.setSizeGripEnabled( true );
  gpm.updatePlugins();
  gpm.show();
  gpm.exec();
  if( gpm.isChanged() )
    updadePluginMenu();
}

bool GuiMain::showWizard()
{
  GuiWizard gw( this );
  gw.setModal( true );
  gw.loadSettings();
  gw.show();
  gw.setFixedSize( gw.size() );
  if( gw.exec() == QDialog::Accepted )
  {
    if( !beeCore->changeLocalUser( gw.userName() ) )
    {
      if( QMessageBox::question( this, Settings::instance().programName(), tr( "Your name is not changed. Do you want to continue?" ),
                                 tr( "Yes" ), tr( "No" ), QString(), 1, 1 ) == 1 )
      {
        return false;
      }
    }
    return true;
  }
  else
    return false;
}

void GuiMain::hideToTrayIcon()
{
  Chat c = ChatManager::instance().firstChatWithUnreadMessages();
  if( c.isValid() )
    mp_trayIcon->setUnreadMessages( c.id(), c.unreadMessages() );
  else
    mp_trayIcon->setUnreadMessages( ID_INVALID, 0 );
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
    if( !isActiveWindow() || isMinimized() )
    {
#ifdef BEEBEEP_DEBUG
      qDebug() << "TrayIcon is activated with trigger click and main window will be showed";
#endif
      QTimer::singleShot( 0, this, SLOT( raiseOnTop() ) );
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
    if( !mp_menuTrayIcon->isVisible() )
      mp_menuTrayIcon->popup( QCursor::pos() );
  }
#endif
}

void GuiMain::trayMessageClicked()
{
  // QT 2019-06-27: Currently this signal is not sent on macOS.
  if( mp_trayIcon->chatId() != ID_INVALID && ChatManager::instance().chat( mp_trayIcon->chatId() ).isValid() )
  {
    VNumber chat_id = mp_trayIcon->chatId();
    showChat( chat_id );
    GuiFloatingChat* fl_chat = floatingChat( chat_id );
    if( fl_chat && !fl_chat->chatIsVisible() )
      QTimer::singleShot( 0, fl_chat, SLOT( raiseOnTop() ) );
  }
  else
    QTimer::singleShot( 0, this, SLOT( raiseOnTop() ) );
}

void GuiMain::addToShare( const QString& share_path )
{
  beeCore->addPathToShare( share_path );
}

void GuiMain::removeFromShare( const QString& share_path )
{
  beeCore->removePathFromShare( share_path );
}

void GuiMain::openUrl( const QUrl& file_url )
{
  openUrlFromChat( file_url, ID_INVALID );
}

void GuiMain::openUrlFromChat( const QUrl& file_url, VNumber chat_id )
{
#ifdef BEEBEEP_DEBUG
  qDebug() << "Opening url (not encoded):" << qPrintable( file_url.toString() );
#endif

  if( file_url.scheme() == QLatin1String( "beeshowsavedchat" ) )
  {
    QStringList sl_chat_id = file_url.toString().split( "#" );
    if( !sl_chat_id.isEmpty() )
    {
      bool chat_id_is_integer = false;
      VNumber chat_id = sl_chat_id.last().toULongLong( &chat_id_is_integer );
      if( chat_id_is_integer && chat_id > 0 )
      {
        QString chat_name = ChatManager::instance().chatName( chat_id );
        if( ChatManager::instance().chatHasSavedText( chat_name ) )
          showSavedChatSelected( chat_name );
        else
          QMessageBox::information( QApplication::activeWindow(), Settings::instance().programName(),
                                    tr( "There are no messages saved in the chat with %1." ).arg( chat_name ), tr( "Ok" ) );
      }
      else
        qWarning() << "Invalid chat id" << chat_id << "found parsing open history request in GuiMain::openUrl(...)";
    }
  }
  else if( file_url.scheme() == FileInfo::urlSchemeShowFileInFolder() )
  {
    QUrl adj_file_url = file_url;
    adj_file_url.setScheme( QLatin1String( "file" ) );
    if( !Bee::showFileInGraphicalShell( Bee::convertToNativeFolderSeparator( adj_file_url.toLocalFile() ) ) )
    {
      QFileInfo file_info_url( adj_file_url.toLocalFile() );
      adj_file_url = QUrl::fromLocalFile( Bee::convertToNativeFolderSeparator( file_info_url.absoluteDir().absolutePath() ) );
      openUrlFromChat( adj_file_url, chat_id );
      return;
    }
  }
  else if( file_url.scheme() == FileInfo::urlSchemeVoiceMessage() )
  {
    QUrl adj_file_url = file_url;
    adj_file_url.setScheme( QLatin1String( "file" ) );
#ifdef BEEBEEP_USE_VOICE_CHAT
    if( Settings::instance().useVoicePlayer() )
    {
      GuiFloatingChat* fl_chat = floatingChat( chat_id );
      if( fl_chat )
        fl_chat->guiChat()->guiVoicePlayer()->setFilePath( Bee::convertToNativeFolderSeparator( adj_file_url.toLocalFile() ), chat_id );
    }
    else
#endif
      openUrlFromChat( adj_file_url, chat_id );
  }
  else if( Bee::isLocalFile( file_url ) )
  {
    QString file_path = Bee::convertToNativeFolderSeparator( file_url.toLocalFile() );
    if( file_path.isEmpty() )
    {
      qWarning() << "Unable to open an empty file path";
      return;
    }

    QFileInfo fi( file_path );
    if( !fi.exists() )
    {
      QMessageBox::information( qApp->activeWindow(), Settings::instance().programName(),
                                tr( "%1: no such file or directory." ).arg( file_path.isEmpty() ? file_url.toString() : file_path ), tr( "Ok" ) );
      return;
    }

#ifdef Q_OS_MAC
    bool is_exe_file = fi.isBundle();
#else
    bool is_exe_file = fi.isExecutable() && !fi.isDir();
#endif
    if( is_exe_file && QMessageBox::question( qApp->activeWindow(), Settings::instance().programName(),
                             tr( "Do you want to open the file %1?" ).arg( file_path ),
                             tr( "Yes" ), tr( "No" ), QString(), 1, 1 ) != 0 )
      return;

    qDebug() << "Open file:" << file_path;
    if( !QDesktopServices::openUrl( QUrl::fromLocalFile( file_path ) ) )
      QMessageBox::information( qApp->activeWindow(), Settings::instance().programName(),
                              tr( "Unable to open %1" ).arg( file_path.isEmpty() ? file_url.toString() : file_path ), tr( "Ok" ) );
  }
  else
  {
    QString url_txt = file_url.toString();
    qDebug() << "Open url:" << url_txt;
    if( !QDesktopServices::openUrl( file_url ) )
      qWarning() << "Unable to open link url:" << url_txt;
  }
}

void GuiMain::selectBeepFile()
{
  QString file_path = FileDialog::getOpenFileName( false, this, Settings::instance().programName(), Settings::instance().beepFilePath(), tr( "Sound files (*.wav)" ) );
  if( file_path.isNull() || file_path.isEmpty() )
    return;

  Settings::instance().setBeepFilePath( file_path );
  qDebug() << "New sound file selected:" << qPrintable( file_path );

  AudioManager::instance().clearBeep();
  AudioManager::instance().playBeep();

  if( !Settings::instance().beepOnNewMessageArrived() )
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
  QString s_default_beep = tr( "The default BEEP will be used" );
  if( !AudioManager::instance().isAudioDeviceAvailable() )
  {
    qWarning() << "Sound device is not available";
    QMessageBox::warning( this, Settings::instance().programName(), QString( "%1. %2." ).arg( tr( "Sound module is not working" ), s_default_beep  ) );
  }
  else if( !QFile::exists( Settings::instance().beepFilePath() ) )
  {
    QString warn_text = QString( "%1\n%2. %3." ).arg( Settings::instance().beepFilePath() )
                                                  .arg( tr( "Sound file not found" ) )
                                                  .arg( s_default_beep );
    QMessageBox::warning( this, Settings::instance().programName(), warn_text );
  }

  playBeep();
}

void GuiMain::playBeep()
{
  AudioManager::instance().playBeep();
}

void GuiMain::playBuzz()
{
  AudioManager::instance().playBuzz();
}

void GuiMain::createGroupChat()
{
  if( !Settings::instance().canAddMembersToGroup() )
  {
    QMessageBox::information( this, Settings::instance().programName(),
                              tr( "You are not allowed create groups." ) + QString( " " ) + tr( "The option has been disabled by your system administrator." ),
                              tr( "Ok" ) );
    return;
  }

  GuiCreateGroup gcg( activeWindow() );
  gcg.loadData();
  gcg.setModal( true );
  gcg.show();
  gcg.setFixedSize( gcg.size() );
  if( gcg.exec() != QDialog::Accepted )
    return;

  if( gcg.leaveGroup() )
    return;

  Group g = gcg.group();

  Chat c = ChatManager::instance().findGroupChatByUsers( g.usersId() );
  if( c.isValid() )
  {
    if( QMessageBox::question( this, Settings::instance().programName(),
                                     QString( "%1\n%2" ).arg( tr( "There is a chat with the same members: %1." ).arg( c.name() ) )
                                                        .arg( "How do you want to continue?" ),
                                     tr( "Create new group chat" ), tr( "Cancel" ), QString(), 1, 1 ) == 1 )
    {
      return;
    }
  }

  c = beeCore->createGroupChat( Settings::instance().localUser(), g, true );
  if( c.isValid() )
    showChat( c.id() );
}

void GuiMain::editGroupChat( VNumber chat_id )
{
  Chat c = ChatManager::instance().chat( chat_id );
  if( !c.isValid() )
    return;

  if( !Settings::instance().canAddMembersToGroup() && !Settings::instance().canRemoveMembersFromGroup() )
  {
    QMessageBox::information( this, Settings::instance().programName(),
                              tr( "You are not allowed modify groups." ) + QString( " " ) + tr( "The option has been disabled by your system administrator." ),
                              tr( "Ok" ) );
    return;
  }

  GuiCreateGroup gcg( activeWindow() );
  gcg.init( c.group() );
  gcg.loadData();
  gcg.setModal( true );
  gcg.show();
  gcg.setFixedSize( gcg.size() );
  if( gcg.exec() == QDialog::Accepted )
  {
    if( gcg.leaveGroup() )
      removeChat( chat_id );
    else
      beeCore->changeGroupChat( Settings::instance().localUser(), gcg.group() );
  }
}

void GuiMain::checkAutoStartOnBoot( bool add_service )
{
  if( add_service )
  {
    if( Settings::instance().addStartOnSystemBoot() )
    {
      QString alert_message = tr( "Now %1 will start on windows boot." ).arg( Settings::instance().programName() );
#ifdef Q_OS_WIN
      if( QSysInfo::windowsVersion() >= QSysInfo::WV_WINDOWS8_1 )
        alert_message += QString( "\n%1" ).arg( "Note: you have to disable the alert window for unsigned software." );
#endif
      QMessageBox::information( this, Settings::instance().programName(), alert_message );
    }
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
  showMessage( tr( "Starting" ), 3000 );
  mp_tabMain->setCurrentWidget( mp_home );
  mp_home->loadSystemMessages();
  QTimer::singleShot( 100, beeCore, SLOT( buildSavedChatList() ) );

  QTimer::singleShot( Settings::instance().delayConnectionAtStartup(), this, SLOT( startCore() ) );
  if( Settings::instance().delayConnectionAtStartup() > 5000 )
    qDebug() << "Delay first connection for" << Settings::instance().delayConnectionAtStartup() << "ms";
}

void GuiMain::showSavedChatSelected( const QString& chat_name )
{
  if( chat_name.isEmpty() )
    return;

  foreach( QWidget* w, qApp->allWidgets() )
  {
    GuiSavedChat* gsv = qobject_cast<GuiSavedChat*>( w );
    if( gsv && gsv->savedChatName() == chat_name )
    {
      gsv->raise();
      return;
    }
  }

  GuiSavedChat* saved_chat = new GuiSavedChat( this );
  saved_chat->setAttribute( Qt::WA_DeleteOnClose, true );
  saved_chat->resize( qMin( (QApplication::desktop()->availableGeometry().width()-80), 680 ), 420 );
  connect( saved_chat, SIGNAL( deleteSavedChatRequest( const QString& ) ), this, SLOT( removeSavedChat( const QString& ) ) );
  connect( saved_chat, SIGNAL( openUrl( const QUrl& ) ), this, SLOT( openUrl( const QUrl& ) ) );
  saved_chat->showSavedChat( chat_name );
  saved_chat->show();
}

void GuiMain::removeSavedChat( const QString& chat_name )
{
  if( QMessageBox::question( this, Settings::instance().programName(), tr( "Do you want to delete saved chat with %1?" ).arg( chat_name ), tr( "Yes" ), tr( "No" ), QString(), 1, 1 ) == 1 )
    return;

  foreach( QWidget* w, qApp->allWidgets() )
  {
    GuiSavedChat* gsv = qobject_cast<GuiSavedChat*>( w );
    if( gsv && gsv->savedChatName() == chat_name )
    {
      gsv->close();
      break;
    }
  }
  beeCore->removeSavedChat( chat_name );
  mp_savedChatList->updateSavedChats();
  updateTabTitles();
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
               tr( "The chat '%1' selected has already a saved text.<br>"
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

  beeCore->linkSavedChat( chat_name, chat_name_selected, add_to_existing_saved_text );
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

void GuiMain::openDeveloperWebSite()
{
  openWebUrl( Settings::instance().developerWebSite() );
}

void GuiMain::setInIdle()
{
  if( !beeCore->isConnected() )
    return;

  if( !Settings::instance().autoUserAway() )
    return;

  if( Settings::instance().localUser().status() == User::Away )
    return;

  m_lastUserStatus = Settings::instance().localUser().status();
  beeCore->setLocalUserStatus( User::Away );
  updateStatusIcon();
}

void GuiMain::exitFromIdle()
{
  if( !beeCore->isConnected() )
    return;

  if( !Settings::instance().autoUserAway() )
    return;

  if( Settings::instance().localUser().status() != User::Away )
    return;

  beeCore->setLocalUserStatus( m_lastUserStatus );
  updateStatusIcon();
}

void GuiMain::updateLocalStatusMessage()
{
  showMessage( Settings::instance().localUser().statusDescription(), 0 );
}

void GuiMain::onStatusBarMessageChanged( const QString& msg )
{
  if( msg.isNull() && !Settings::instance().localUser().statusDescription().isEmpty() )
    QTimer::singleShot( 0, this, SLOT( updateLocalStatusMessage() ) );
}

void GuiMain::showMessage( const QString& status_msg, int time_out )
{
  statusBar()->showMessage( status_msg, time_out );
  if( status_msg == Settings::instance().localUser().statusDescription() )
    statusBar()->setToolTip( tr( "Your status description" ) );
  else
    statusBar()->setToolTip( "" );
}

void GuiMain::changeUserColor( VNumber user_id, const QString& user_color )
{
  QColor c = QColorDialog::getColor( QColor( user_color ), qApp->activeWindow() );
  if( c.isValid() )
    beeCore->changeUserColor( user_id, c.name() );
}

void GuiMain::onUserChanged( const User& u )
{
  mp_userList->setUser( u, true );
  mp_groupList->updateUser( u );
  mp_chatList->updateUser( u );
  foreach( GuiFloatingChat* fl_chat, m_floatingChats )
    fl_chat->updateUser( u );
  checkViewActions();
  if( u.isLocal() )
  {
    updateStatusIcon();
    updateWindowTitle();
  }
  else
  {
    if( m_changeTabToUserListOnFirstConnected && u.isStatusConnected() && mp_tabMain->currentWidget() == mp_home )
    {
      m_changeTabToUserListOnFirstConnected = false;
      mp_tabMain->setCurrentWidget( mp_userList );
    }
  }
#ifdef BEEBEEP_USE_SHAREDESKTOP
  foreach( GuiShareDesktop* gsd, m_desktops )
    gsd->onUserChanged( u );
#endif
}

void GuiMain::onUserRemoved( const User& u )
{
  showMessage( tr( "%1 removed" ).arg( u.name() ), 2000 );
  mp_userList->updateUsers();
  updateTabTitles();
}

void GuiMain::onChatChanged( const Chat& c )
{
#ifdef BEEBEEP_DEBUG
  showMessage( tr( "%1 updated" ).arg( c.name() ), 2000 );
#endif
  mp_userList->updateChat( c );
  mp_chatList->updateChat( c );
  mp_groupList->updateChat( c );
  GuiFloatingChat* fl_chat = floatingChat( c.id() );
  if( fl_chat )
    fl_chat->updateChat( c );
  mp_savedChatList->updateSavedChats();
  updateTabTitles();
  updateNewMessageAction();
}

void GuiMain::onChatRemoved( const Chat& c )
{
  showMessage( tr( "%1 removed" ).arg( c.name() ), 2000 );
  GuiFloatingChat* fl_chat = floatingChat( c.id() );
  if( fl_chat )
    fl_chat->close();
  mp_chatList->updateChats();
  mp_groupList->updateGroups();
  mp_savedChatList->updateSavedChats();
  if( mp_trayIcon->chatId() == c.id() )
    mp_trayIcon->setUnreadMessages( c.id(), 0 );
  updateTabTitles();
  updateNewMessageAction();
}

bool GuiMain::checkAllChatMembersAreConnected( const QList<VNumber>& users_id )
{
  if( !beeCore->areUsersConnected( users_id ) )
  {
    if( QMessageBox::question( activeWindow(), Settings::instance().programName(),
                               tr( "All the members of this chat are not online. The changes may not be permanent. Do you wish to continue?" ),
                               tr( "Yes" ), tr( "No" ), QString(), 1, 1 ) != 0 )
       return false;
  }
  return true;
}

void GuiMain::clearChat( VNumber chat_id )
{
  Chat c = ChatManager::instance().chat( chat_id );
  if( !c.isValid() )
    return;
  QString chat_name = c.isDefault() ? QObject::tr( "All users" ).toLower() : c.name();
  if( c.isEmpty() && !ChatManager::instance().chatHasSavedText( c.name() ) )
  {
    QMessageBox::information( activeWindow(), Settings::instance().programName(), tr( "Chat with %1 is empty." ).arg( chat_name ) );
    return;
  }

  QString question_txt = tr( "Do you want to clear messages with %1?" ).arg( chat_name );
  QString button_2_text;
  if( ChatManager::instance().chatHasSavedText( c.name() ) )
    button_2_text = QString( "  " ) + tr( "Yes and delete history" ) + QString( "  " );

  bool chat_cleared = false;
  switch( QMessageBox::information( activeWindow(), Settings::instance().programName(), question_txt, tr( "Yes" ), tr( "No" ), button_2_text, 1, 1 ) )
  {
  case 0:
    chat_cleared = beeCore->clearMessagesInChat( chat_id, false );
    break;
  case 2:
    chat_cleared = beeCore->clearMessagesInChat( chat_id, true );
    break;
  default:
    return;
  }

  if( chat_cleared )
  {
    GuiFloatingChat* fl_chat = floatingChat( chat_id );
    if( fl_chat )
    {
      Chat c = ChatManager::instance().chat( chat_id );
      if( c.isValid() )
        fl_chat->setChat( c );
    }

    if( chat_id == ID_DEFAULT_CHAT )
      mp_home->loadSystemMessages();
  }
}

void GuiMain::clearSystemMessagesInChat( VNumber chat_id )
{
  if( beeCore->clearSystemMessagesInChat( chat_id ) )
  {
    GuiFloatingChat* fl_chat = floatingChat( chat_id );
    if( fl_chat )
    {
      Chat c = ChatManager::instance().chat( chat_id );
      if( c.isValid() )
        fl_chat->setChat( c );
    }

    if( chat_id == ID_DEFAULT_CHAT )
      mp_home->loadSystemMessages();
  }
}

void GuiMain::removeChat( VNumber chat_id )
{
  Chat c = ChatManager::instance().chat( chat_id );
  if( !c.isValid() )
  {
    qWarning() << "Invalid chat" << chat_id << "found in GuiMain::removeChat(...)";
    return;
  }

  QString button_2_text;
  if( ChatManager::instance().chatHasSavedText( c.name() ) )
    button_2_text = QString( "  " ) + tr( "Yes and delete history" ) + QString( "  " );
  QString question_txt = tr( "Do you want to leave the chat with %1?" ).arg( c.name() );
  int answer_id = QMessageBox::question( activeWindow(), Settings::instance().programName(), question_txt, tr( "Yes" ), tr( "No" ), button_2_text, 1, 1 );
  if( answer_id == 1 )
    return;

  if( !beeCore->removeChat( chat_id, answer_id == 0 ) )
    QMessageBox::warning( activeWindow(), Settings::instance().programName(), tr( "Unable to leave the chat with %1." ).arg( c.name() ) );
}

void GuiMain::showSharesForUser( const User& u )
{
  if( mp_fileSharing )
    mp_fileSharing->showUserFileList( u );
}

void GuiMain::selectLanguage()
{
  GuiLanguage gl;
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
                              QString( "%1<br>%2" ).arg( language_message ).arg( tr( "You must restart %1 to apply these changes." )
                                                                                    .arg( Settings::instance().programName() ) ) );

    Settings::instance().setLanguage( gl.languageSelected() );
    Settings::instance().setLanguagePath( gl.folderSelected() );
  }
}

void GuiMain::showAddUser()
{
  GuiAddUser gad;
  gad.loadUsers();
  gad.setModal( true );
  gad.setSizeGripEnabled( true );
  gad.show();

  if( gad.exec() == QDialog::Accepted )
  {
    if( !Settings::instance().networkAddressList().isEmpty() )
    {
      beeCore->updateUsersAddedManually();
      QMetaObject::invokeMethod( this, "sendBroadcastMessage", Qt::QueuedConnection );
    }
  }
}

void GuiMain::showChatSettingsMenu()
{
  mp_menuChat->clear();
  QAction* act = mp_menuChat->addAction( tr( "Use RTL mode to show text" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().showTextInModeRTL() );
  act->setData( 55 );
  mp_menuChat->addSeparator();
  act = mp_menuChat->addAction( tr( "Show the chat in compact view mode" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().chatCompact() );
  act->setData( 1 );
  act = mp_menuChat->addAction( tr( "Show the timestamp" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().chatShowMessageTimestamp() );
  act->setData( 3 );
  act = mp_menuChat->addAction( tr( "Show the time with the AM/PM notation" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().useMessageTimestampWithAP() );
  act->setData( 76 );
  act = mp_menuChat->addAction( tr( "Show messages grouped by user" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().showMessagesGroupByUser() );
  act->setData( 13 );
  act = mp_menuChat->addAction( tr( "Show your name instead of 'You'" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().chatUseYourNameInsteadOfYou() );
  act->setData( 41 );
  if( mp_actShowUserFullName->isChecked() )
  {
    mp_menuChat->addAction( mp_actShowUserFullName );
    mp_menuChat->addAction( mp_actShowUserFirstNameFirstInFullName );
  }
  act = mp_menuChat->addAction( tr( "Show username's color" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().chatUseColoredUserNames() );
  act->setData( 67 );
  mp_menuChat->addSeparator();
  act = mp_menuChat->addAction( tr( "Show emoticons" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().showEmoticons() );
  act->setData( 10 );
  act = mp_menuChat->addAction( tr( "Use HTML tags" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().chatUseHtmlTags() );
  act->setData( 8 );
  act = mp_menuChat->addAction( tr( "Use clickable links" ), this, SLOT( settingsChanged() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().chatUseClickableLinks() );
  act->setData( 9 );
  mp_menuChat->addSeparator();
  act = mp_menuChat->addAction( tr( "Clear recent emoticons" ), this, SLOT( settingsChanged() ) );
  act->setIcon( IconManager::instance().icon( "clear.png" ) );
  act->setData( 104 );
  act = mp_menuChat->addAction( tr( "Clear favorite emoticons" ), this, SLOT( settingsChanged() ) );
  act->setIcon( IconManager::instance().icon( "clear.png" ) );
  act->setData( 106 );
  act = mp_menuChat->addAction( tr( "Restore default font" ), this, SLOT( settingsChanged() ) );
  act->setIcon( IconManager::instance().icon( "font.png" ) );
  act->setData( 23 );
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
  QString multicast_group = tr( "none" );

  if( beeCore->isConnected() )
  {
    mp_menuNetworkStatus->setIcon( IconManager::instance().icon( "network-connected.png" ) );
    mp_actHostAddress->setIcon( IconManager::instance().icon( "network-connected.png" ) );
    mp_actHostAddress->setEnabled( true );
    mp_actPortBroadcast->setEnabled( true );
    mp_actPortListener->setEnabled( true );
    mp_actEncryptedConnectionByDefault->setEnabled( true );
    mp_actCompressedDataByDefault->setEnabled( true );

    host_address = Settings::instance().localUser().networkAddress().hostAddress().toString();
    broadcast_port = QString::number( Settings::instance().defaultBroadcastPort() );
    listener_port = QString::number( Settings::instance().localUser().networkAddress().hostPort() );

    QHostAddress multicast_host_address = beeCore->multicastGroupAddress();
    mp_actMulticastGroupAddress->setEnabled( !multicast_host_address.isNull() );
    if( !multicast_host_address.isNull() )
      multicast_group = multicast_host_address.toString();

    if( Settings::instance().enableFileTransfer() )
    {
      file_transfer_port = QString::number( beeCore->fileTransferPort() );
      mp_actPortFileTransfer->setEnabled( true );
      if( mp_networkTest )
        mp_networkTest->updateSettings( QString::number( beeCore->fileTransferPort() ) );
    }
    else
    {
      file_transfer_port = tr( "disabled" );
      mp_actPortFileTransfer->setEnabled( false );
      if( mp_networkTest )
        mp_networkTest->updateSettings( file_transfer_port );
    }

#ifdef BEEBEEP_USE_MULTICAST_DNS
    if( beeCore->dnsMulticastingIsActive() )
    {

      multicast_dns = tr( "active" );
      mp_actMulticastDns->setEnabled( true );
    }
    else
      mp_actMulticastDns->setEnabled( false );
#endif


  }
  else
  {
    mp_menuNetworkStatus->setIcon( IconManager::instance().icon( "network-disconnected.png" ) );
    mp_actHostAddress->setIcon( IconManager::instance().icon( "network-disconnected.png" ) );
    mp_actHostAddress->setEnabled( false );
    mp_actPortBroadcast->setEnabled( false );
    mp_actMulticastGroupAddress->setEnabled( false );
    mp_actPortListener->setEnabled( false );
    mp_actPortFileTransfer->setEnabled( false );
#ifdef BEEBEEP_USE_MULTICAST_DNS
    mp_actMulticastDns->setEnabled( false );
#endif
    mp_actEncryptedConnectionByDefault->setEnabled( false );
    mp_actCompressedDataByDefault->setEnabled( false );
  }

  mp_actHostAddress->setText( QString( "ip: %1 (%2)" ).arg( host_address ).arg( tr( "your IP address" ) ) );
  mp_actPortBroadcast->setText( QString( "udp: %1 (%2)" ).arg( broadcast_port ).arg( tr( "to search for users" ) ) );
  mp_actMulticastGroupAddress->setText( QString( "multicast: %1" ).arg( multicast_group ) );
  mp_actPortListener->setText( QString( "tcp1: %1 (%2)" ).arg( listener_port ).arg( tr( "chat messages" ) ) );
  mp_actPortFileTransfer->setText( QString( "tcp2: %1 (%2)" ).arg( file_transfer_port ).arg( tr( "file transfer" ) ) );
#ifdef BEEBEEP_USE_MULTICAST_DNS
  mp_actMulticastDns->setText( QString( "mdns: %1" ).arg( multicast_dns ) );
#endif
  if( Settings::instance().disableConnectionSocketEncryption() )
  {
    mp_actEncryptedConnectionByDefault->setIcon( IconManager::instance().icon( "warning.png" ) );
    mp_actEncryptedConnectionByDefault->setText( tr( "End-to-end encryption is disabled" ) );
  }
  else
  {
    mp_actEncryptedConnectionByDefault->setIcon( IconManager::instance().icon( "encryption-enabled.png" ) );
    mp_actEncryptedConnectionByDefault->setText( tr( "End-to-end encryption is enabled" ) );
  }

  if( Settings::instance().disableConnectionSocketEncryption() )
  {
    mp_actCompressedDataByDefault->setIcon( IconManager::instance().icon( "folder.png" ) );
    mp_actCompressedDataByDefault->setText( tr( "Data compression is disabled" ) );
  }
  else
  {
    mp_actCompressedDataByDefault->setIcon( IconManager::instance().icon( "file-compressed.png" ) );
    mp_actCompressedDataByDefault->setText( tr( "Data compression is enabled" ) );
  }
}

void GuiMain::sendBroadcastMessage()
{
  mp_actBroadcast->setDisabled( true );
  beeCore->sendBroadcastMessage();
#ifdef BEEBEEP_USE_MULTICAST_DNS
  beeCore->sendDnsMulticastingMessage();
#endif
  QTimer::singleShot( 61000, this, SLOT( enableBroadcastAction() ) );
}

void GuiMain::enableBroadcastAction()
{
  mp_actBroadcast->setEnabled( beeCore->isConnected() );
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
    beeCore->createPrivateChat( u );
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
  if( !beeCore->isConnected() )
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
                                          Settings::instance().avatarIconSize().height(), 16, 96, 4, &ok );
  if( !ok )
    return;

  Settings::instance().setAvatarIconSize( QSize( avatar_size, avatar_size ) );
  mp_userList->updateUsers();
  mp_chatList->updateChats();
  mp_groupList->updateGroups();
  mp_savedChatList->updateSavedChats();
  foreach( GuiFloatingChat* fl_chat, m_floatingChats )
  {
    Chat c = ChatManager::instance().chat( fl_chat->guiChat()->chatId() );
    if( c.isValid() )
      fl_chat->updateChatMembers( c );
  }
}

void GuiMain::toggleUserFavorite( VNumber user_id )
{
  beeCore->toggleUserFavorite( user_id );
}

void GuiMain::removeUserFromList( VNumber user_id )
{
  QString question_txt = tr( "Do you want to delete user %1?" ).arg( UserManager::instance().findUser( user_id ).name() );
  if( QMessageBox::question( activeWindow(), Settings::instance().programName(), question_txt, tr( "Yes" ), tr( "No" ), QString(), 1, 1 ) != 0 )
    return;
  beeCore->removeOfflineUser( user_id );
}

void GuiMain::openResourceFolder()
{
  QUrl folder_url = QUrl::fromLocalFile( Settings::instance().resourceFolder() );
  openUrl( folder_url );
}

void GuiMain::openDataFolder()
{
  QUrl folder_url = QUrl::fromLocalFile( Settings::instance().dataFolder() );
  openUrl( folder_url );
}

void GuiMain::openDownloadFolder()
{
  QUrl folder_url = QUrl::fromLocalFile( Settings::instance().downloadDirectory() );
  openUrl( folder_url );
}

GuiFloatingChat* GuiMain::floatingChat( VNumber chat_id ) const
{
  foreach( GuiFloatingChat* fl_chat, m_floatingChats )
  {
    if( fl_chat->guiChat()->chatId() == chat_id )
      return fl_chat;
  }
  return Q_NULLPTR;
}

void GuiMain::removeFloatingChatFromList( VNumber chat_id )
{
  GuiFloatingChat* fl_chat = floatingChat( chat_id );
  if( !fl_chat )
    return;

  if( mp_menuChat->isVisible() )
    mp_menuChat->close();

  m_floatingChats.removeOne( fl_chat );
  fl_chat->deleteLater();
#ifdef BEEBEEP_DEBUG
  qDebug() << "Floating chat" << chat_id << "closed and deleted";
#endif

#ifdef BEEBEEP_USE_SHAREDESKTOP
  onShareDesktopRequestFromChat( chat_id, false );
#endif
}

GuiFloatingChat* GuiMain::createFloatingChat( const Chat& c )
{
  GuiFloatingChat* fl_chat = floatingChat( c.id() );
  if( fl_chat )
    return fl_chat;

  if( Settings::instance().showChatsInOneWindow() && !m_floatingChats.isEmpty() )
    fl_chat = m_floatingChats.first();

  if( !fl_chat )
  {
    fl_chat = new GuiFloatingChat;
    setupChatConnections( fl_chat->guiChat() );
    connect( fl_chat, SIGNAL( chatIsAboutToClose( VNumber ) ), this, SLOT( removeFloatingChatFromList( VNumber ) ) );
    connect( fl_chat, SIGNAL( readAllMessages( VNumber ) ), this, SLOT( readAllMessagesInChat( VNumber ) ) );
    connect( fl_chat, SIGNAL( showVCardRequest( VNumber ) ), this, SLOT( showVCard( VNumber ) ) );
    connect( fl_chat, SIGNAL( updateChatFontRequest() ), this, SLOT( updateChatFont() ) );
    connect( fl_chat, SIGNAL( updateChatColorsRequest() ), this, SLOT( updateChatColors() ) );
#ifdef BEEBEEP_USE_VOICE_CHAT
    connect( fl_chat, SIGNAL( sendVoiceMessageRequest( VNumber, const QString&, qint64 ) ), this, SLOT( sendVoiceMessageToChat( VNumber, const QString&, qint64 ) ) );
#endif

    m_floatingChats.append( fl_chat );
    fl_chat->setWindowFlagsAndGeometry();
    fl_chat->setSaveGeometryDisabled( Settings::instance().resetGeometryAtStartup() );
  }

  if( !fl_chat->setChat( c ) )
  {
    qWarning() << "Unable to create floating window for not existing chat" << c.id() << c.name();
    fl_chat->deleteLater();
    return Q_NULLPTR;
  }

  return fl_chat;
}

QWidget* GuiMain::activeWindow()
{
  QWidget* active_window = QApplication::activeWindow();
  if( active_window )
    return active_window;
  else
    return qobject_cast<QWidget*>( this );
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
    beeCore->setLocalUserStatusDescription( usr.status(), usr.statusDescription(), false );
    setUserStatusSelected( usr.status() );
    loadUserStatusRecentlyUsed();
    updateLocalStatusMessage();
  }
}

void GuiMain::clearRecentlyUsedUserStatus()
{
  if( QMessageBox::question( this, Settings::instance().programName(),
                             tr( "Do you want to clear all saved status descriptions?" ),
                             tr( "Yes" ), tr( "No" ), QString(), 1, 1 ) != 0 )
    return;

  qDebug() << "User status description list is cleared";
  QStringList sl = Settings::instance().userStatusList();
  sl.clear();
  Settings::instance().setUserStatusList( sl );
  loadUserStatusRecentlyUsed();
}

void GuiMain::loadSavedChatsCompleted()
{
  mp_chatList->updateChats();
  mp_savedChatList->updateSavedChats();
  foreach( GuiFloatingChat* fl_chat, m_floatingChats )
    fl_chat->setChat( ChatManager::instance().chat( fl_chat->guiChat()->chatId() ) );
  updateTabTitles();
}

void GuiMain::editShortcuts()
{
  GuiShortcut gs;
  gs.setModal( true );
  gs.loadShortcuts();
  gs.setSizeGripEnabled( true );
  gs.show();

  if( gs.exec() == QDialog::Rejected )
    return;

  Settings::instance().setCustomShortcuts( ShortcutManager::instance().saveToStringList() );
  Settings::instance().save();
  updateShortcuts();
}

void GuiMain::updateShortcuts()
{
  foreach( GuiFloatingChat* fl_chat, m_floatingChats )
    fl_chat->guiChat()->updateShortcuts();

  QKeySequence ks = ShortcutManager::instance().shortcut( ShortcutManager::ShowFileTransfers );
  if( !ks.isEmpty() && Settings::instance().useCustomShortcuts() )
    mp_actViewFileTransfer->setShortcut( ks );
  else
    mp_actViewFileTransfer->setShortcut( QKeySequence() );

  ks = ShortcutManager::instance().shortcut( ShortcutManager::MinimizeAllChats );
  if( !ks.isEmpty() )
  {
    mp_scMinimizeAllChats->setKey( ks );
    mp_scMinimizeAllChats->setEnabled( Settings::instance().useCustomShortcuts() );
  }
  else
    mp_scMinimizeAllChats->setEnabled( false );

#ifdef BEEBEEP_USE_QXT
  ks = ShortcutManager::instance().shortcut( ShortcutManager::ShowAllChats );
  if( !ks.isEmpty() && Settings::instance().useCustomShortcuts() )
  {
    mp_scShowAllChats->setShortcut( ks );
    mp_scShowAllChats->setEnabled( true );
  }
  else
  {
    mp_scShowAllChats->unsetShortcut();
    mp_scShowAllChats->setEnabled( false );
  }

  ks = ShortcutManager::instance().shortcut( ShortcutManager::SendHelpMessage );
  if( !ks.isEmpty() && Settings::instance().useCustomShortcuts() )
  {
    mp_scSendHelpMessage->setShortcut( ks );
    mp_scSendHelpMessage->setEnabled( true );
  }
  else
  {
    mp_scSendHelpMessage->unsetShortcut();
    mp_scSendHelpMessage->setEnabled( false );
  }

  ks = ShortcutManager::instance().shortcut( ShortcutManager::SelectFirstChat );
  if( !ks.isEmpty() && Settings::instance().useCustomShortcuts() )
  {
    mp_scSelectFirstChat->setShortcut( ks );
    mp_scSelectFirstChat->setEnabled( true );
  }
  else
  {
    mp_scSelectFirstChat->unsetShortcut();
    mp_scSelectFirstChat->setEnabled( false );
  }
#endif

  ks = ShortcutManager::instance().shortcut( ShortcutManager::ShowNextUnreadMessage );
  if( !ks.isEmpty() )
  {
    mp_scShowNextUnreadMessage->setKey( ks );
    mp_scShowNextUnreadMessage->setEnabled( Settings::instance().useCustomShortcuts() );
  }
  else
    mp_scShowNextUnreadMessage->setEnabled( false );

  ks = ShortcutManager::instance().shortcut( ShortcutManager::Broadcast );
  if( !ks.isEmpty() && Settings::instance().useCustomShortcuts() )
    mp_actBroadcast->setShortcut( ks );
  else
    mp_actBroadcast->setShortcut( QKeySequence() );

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
        fl_chat->show();
      fl_chat->raiseOnTop();

      if( fl_chat == mp_lastActiveWindow )
        last_active_window_exists = true;
    }
  }

  if( isMinimized() )
    showNormal();
  else
    show();
  raiseOnTop();

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
    QMessageBox::information( this, Settings::instance().programName(), tr( "Dictionary selected: %1" ).arg( dictionary_path ) );
  else
    QMessageBox::warning( this, Settings::instance().programName(), tr( "Unable to set dictionary: %1" ).arg( dictionary_path ) );
#endif
  Settings::instance().setUseSpellChecker( true );
  Settings::instance().setUseWordCompleter( true );

  // update spellchecker and wordcompleter actions
  foreach( GuiFloatingChat* fl_chat, m_floatingChats )
    fl_chat->guiChat()->updateActionsOnFocusChanged();
}

void GuiMain::onNetworkInterfaceDown()
{
  if( beeCore->isConnected() )
    QMetaObject::invokeMethod( this, "stopCore", Qt::QueuedConnection );
}

void GuiMain::onNetworkInterfaceUp()
{
  if( m_userSelectOffline )
    qDebug() << "User selected offline mode and the connection to network is refused";
  else
    QMetaObject::invokeMethod( this, "startCore", Qt::QueuedConnection );
}

void GuiMain::onTickEvent( int ticks )
{
  int chat_tab_index = mp_tabMain->indexOf( mp_chatList );
  if( mp_actViewNewMessage->isEnabled() )
  {
    QIcon new_message_blinking_icon = IconManager::instance().icon( "beebeep-message.png" );
    mp_actViewNewMessage->setIcon( ticks % 2 == 0 ? new_message_blinking_icon : Bee::convertToGrayScale( new_message_blinking_icon, Settings::instance().mainBarIconSize() ) );
    mp_tabMain->setTabIcon( chat_tab_index, ticks % 2 == 0 ? new_message_blinking_icon : IconManager::instance().icon( "chat-list.png" ) );
    setWindowIcon( ticks % 2 == 0 ? new_message_blinking_icon : IconManager::instance().icon( "beebeep.png" )  );
  }

  mp_trayIcon->onTickEvent( ticks );
  mp_chatList->onTickEvent( ticks );
  mp_userList->onTickEvent( ticks );
  mp_groupList->onTickEvent( ticks );
  mp_fileTransfer->onTickEvent( ticks );
  if( mp_fileSharing )
    mp_fileSharing->onTickEvent( ticks );
  if( mp_networkTest )
    mp_networkTest->onTickEvent( ticks );

  if( beeCore->hasFileTransferInProgress() )
    mp_actViewFileTransfer->setIcon( ticks % 2 == 0 ? IconManager::instance().icon( "file-transfer-progress.png" ) : IconManager::instance().icon( "file-transfer.png" ) );

  if( ticks % 2 == 0 && mp_actViewNewMessage->isEnabled() )
  {
    Chat c = ChatManager::instance().firstChatWithUnreadMessages();
    GuiFloatingChat* fl_chat = floatingChat( c.id() );
    if( !fl_chat )
      QApplication::alert( this, 1000 );
    showMessage( tr( "You have new message" ), 1000 );
  }

  foreach( GuiFloatingChat* fl_chat, m_floatingChats )
    fl_chat->onTickEvent( ticks );

#ifdef BEEBEEP_USE_SHAREDESKTOP
  foreach( GuiShareDesktop* gsd, m_desktops )
    gsd->onTickEvent( ticks );
#endif

  if( beeApp->isInSleepMode() )
    return;

  if( !m_coreIsConnecting )
  {
    if( Bee::isTimeToCheck( ticks, Settings::instance().tickIntervalCheckIdle() ) )
    {
      if( beeApp->idleTimeout() > 0 )
        QMetaObject::invokeMethod( beeApp, "checkIdle", Qt::QueuedConnection );
    }

    beeCore->onTickEvent( ticks );
  }
}

void GuiMain::onChatReadByUser( const Chat& c, const User& u )
{
  GuiFloatingChat* fl_chat = floatingChat( c.id() );
  if( fl_chat )
    fl_chat->setChatReadByUser( c, u );
}

void GuiMain::readAllMessagesInChat( VNumber chat_id )
{
  beeCore->readAllMessagesInChat( chat_id );
}

void GuiMain::saveSession( QSessionManager& )
{
  qDebug() << "Session manager ask to save and close session";
  forceShutdown();
}

void GuiMain::updateEmoticons()
{
  if( m_floatingChats.isEmpty() )
    return;

  foreach( GuiFloatingChat* fl_chat, m_floatingChats )
    fl_chat->updateEmoticons();
}

void GuiMain::updateNewMessageAction()
{
  Chat c = ChatManager::instance().firstChatWithUnreadMessages();
  mp_trayIcon->setNextChatToRead( c );
  mp_actViewNewMessage->setEnabled( c.isValid() );
  int chat_tab_index = mp_tabMain->indexOf( mp_chatList );
  if( mp_actViewNewMessage->isEnabled() )
  {
    mp_actViewNewMessage->setStatusTip( tr( "You have new message" ) );
    mp_tabMain->setTabIcon( chat_tab_index, IconManager::instance().icon( "beebeep-message.png" ) );
    setWindowIcon( IconManager::instance().icon( "beebeep-message.png" ) );
  }
  else
  {
    mp_actViewNewMessage->setStatusTip( "" );
    mp_tabMain->setTabIcon( chat_tab_index, IconManager::instance().icon( "chat-list.png" ) );
    setWindowIcon( IconManager::instance().icon( "beebeep.png" )  );
  }
}

void GuiMain::saveGeometryAndState()
{
  if( isVisible() )
  {
    Settings::instance().setGuiGeometry( saveGeometry() );
    Settings::instance().setGuiState( saveState() );
    Settings::instance().save();
  }
}

void GuiMain::askSaveGeometryAndState()
{
  if( isVisible() )
  {
    QByteArray ba_state = saveState();
#if QT_VERSION == 0x050906
    int default_button = mp_dockFileTransfers->isVisible() ? 0 : 1;
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
    Settings::instance().setGuiGeometry( saveGeometry() );
    Settings::instance().setGuiState( ba_state );
    Settings::instance().save();
    if( ba_state.isEmpty() )
      showMessage( tr( "Window geometry saved" ), 3000 );
    else
      showMessage( tr( "Window geometry and state saved" ), 3000 );
  }
}

void GuiMain::restoreGeometryAndState()
{
  restoreGeometry( Settings::instance().guiGeometry() );
  if( !Settings::instance().guiState().isEmpty() )
    restoreState( Settings::instance().guiState() );
}

void GuiMain::resetGeometryAndState()
{
  if( mp_dockFileTransfers->isVisible() )
    mp_dockFileTransfers->hide();
  resize( qMax( width(), 280 ), qMin( 520, qMax( QApplication::desktop()->availableGeometry().height() - 120, 460 ) ) );

#ifdef Q_OS_WIN
  move( QApplication::desktop()->availableGeometry().width() - frameGeometry().width(),
        QApplication::desktop()->availableGeometry().height() - frameGeometry().height() );
#elif defined BEEBEEP_FOR_RASPBERRY_PI
  move( QApplication::desktop()->availableGeometry().width() - frameGeometry().width(), 70 );
#elif defined Q_OS_MAC
  #if QT_VERSION >= 0x050900
    move( QApplication::desktop()->availableGeometry().width() - frameGeometry().width(), 0 );
  #else
    move( QApplication::desktop()->availableGeometry().width() - frameGeometry().width() - 20, 0 );
  #endif
#else
  move( QApplication::desktop()->availableGeometry().width() - frameGeometry().width(), 0 );
#endif

  if( !Settings::instance().guiGeometry().isEmpty() )
    Settings::instance().setGuiGeometry( QByteArray() );
  if( !Settings::instance().guiState().isEmpty() )
    Settings::instance().setGuiState( QByteArray() );
}

void GuiMain::askResetGeometryAndState()
{
  if( QMessageBox::question( this, Settings::instance().programName(), tr( "Do you really want to reset window's geometry?" ), tr( "Yes" ), tr( "No" ), QString(), 1, 1 ) == 0 )
  {
    resetGeometryAndState();
    setMinimumWidthForStyle();
    if( !Settings::instance().floatingChatGeometry().isEmpty() )
      Settings::instance().setFloatingChatGeometry( QByteArray() );
    if( !Settings::instance().floatingChatState().isEmpty() )
      Settings::instance().setFloatingChatState( QByteArray() );
    if( !Settings::instance().floatingChatSplitterState().isEmpty() )
      Settings::instance().setFloatingChatSplitterState( QByteArray() );
    if( !Settings::instance().createMessageGeometry().isEmpty() )
      Settings::instance().setCreateMessageGeometry( QByteArray() );
    if( !Settings::instance().fileSharingGeometry().isEmpty() )
      Settings::instance().setFileSharingGeometry( QByteArray() );
    Settings::instance().setShowEmoticonMenu( false );
    Settings::instance().save();
    showMessage( tr( "Geometry of all windows has been reset" ), 3000 );
  }
}

void GuiMain::onChangeSettingOnExistingFile( QAction* act )
{
  if( !act )
    return;

  Settings::instance().setOnExistingFileAction( act->data().toInt() );
  Settings::instance().save();
}

void GuiMain::onFileTransferProgress( VNumber peer_id, const User& u, const FileInfo& fi, FileSizeType bytes, qint64 elapsed_time )
{
  mp_fileTransfer->setProgress( peer_id, u, fi, bytes, elapsed_time );
  if( Settings::instance().alwaysShowFileTransferProgress() )
  {
    if( !mp_dockFileTransfers->isVisible() )
    {
      mp_dockFileTransfers->show();
      raiseOnTop();
    }
  }
}

void GuiMain::onFileTransferMessage( VNumber peer_id, const User& u, const FileInfo& fi, const QString& msg, FileTransferPeer::TransferState ft_state )
{
  mp_fileTransfer->setMessage( peer_id, u, fi, msg, ft_state );
  if( Settings::instance().alwaysShowFileTransferProgress() )
  {
    if( !mp_dockFileTransfers->isVisible() )
    {
      mp_dockFileTransfers->show();
      raiseOnTop();
    }
  }

  if( ft_state == FileTransferPeer::Completed )
  {
    if( fi.isDownload() )
    {
      Chat c = ChatManager::instance().findChatByPrivateId( fi.chatPrivateId(), false, u.id() );
      if( !c.isValid() )
      {
        qWarning() << "Unable to find chat by private id" << qPrintable( fi.chatPrivateId() ) << "for user" << qPrintable( u.name() ) << "in onFileTransferCompleted(...)";
        return;
      }

      if( Settings::instance().showFileTransferCompletedOnTray() )
        mp_trayIcon->showNewFileArrived( c.id(), tr( "New file from %1" ).arg( u.name() ), false );

      GuiFloatingChat* fl_chat = floatingChat( c.id() );
      if( fl_chat && fl_chat->isActiveWindow() )
        return;
      onChatChanged( c );
    }
  }
}

void GuiMain::sendBuzzToUser( VNumber user_id )
{
  if( beeCore->isConnected() )
    beeCore->sendBuzzToUser( user_id );
}

void GuiMain::showBuzzFromUser( const User& u, VNumber chat_id )
{
  if( Settings::instance().playBuzzSound() )
    playBuzz();

  mp_trayIcon->showNewMessageArrived( chat_id, tr( "%1 is buzzing you!" ).arg( Bee::userNameToShow( u, false ) ), true );
}

void GuiMain::sendHelpMessage()
{
  if( !beeCore->sendHelpMessage() )
  {
    // TODO: alert / success message
  }
}

void GuiMain::sendHelpMessageToUser( VNumber user_id )
{
  if( !beeCore->sendHelpMessageToUser( user_id ) )
  {
    // TODO: alert / success message
  }
}

void GuiMain::showHelpRequestFromUser( const User& u, VNumber chat_id )
{
  if( Settings::instance().playBuzzSound() )
    playBuzz();

  mp_trayIcon->showNewMessageArrived( chat_id, tr( "%1 is asking for your help!" ).arg( Bee::userNameToShow( u, false ) ), true );
}

void GuiMain::showHelpAnswerFromUser( const User& u, VNumber chat_id )
{
  if( Settings::instance().playBuzzSound() )
    playBuzz();

  mp_trayIcon->showNewMessageArrived( chat_id, tr( "%1 got your call for help." ).arg( Bee::userNameToShow( u, false ) ), true );
}

void GuiMain::showFileSharingWindow()
{
  if( Settings::instance().disableFileSharing() )
    return;

  if( !mp_fileSharing )
  {
    mp_fileSharing = new GuiFileSharing( Q_NULLPTR );
    mp_fileSharing->setAttribute( Qt::WA_DeleteOnClose, true );
    Bee::setWindowStaysOnTop( mp_fileSharing, Settings::instance().stayOnTop() );
    mp_fileSharing->resize( qMin( (QApplication::desktop()->availableGeometry().width()-20), 860 ), 460 );
    mp_fileSharing->updateLocalFileList();
    connect( mp_fileSharing, SIGNAL( destroyed() ), this, SLOT( onFileSharingWindowClosed() ) );
    connect( mp_fileSharing, SIGNAL( openUrlRequest( const QUrl& ) ), this, SLOT( openUrl( const QUrl& ) ) );
    connect( mp_fileSharing, SIGNAL( sendFileRequest( const QString& ) ), this, SLOT( sendFile( const QString& ) ) );
    connect( mp_fileSharing, SIGNAL( downloadSharedFileRequest( VNumber, VNumber ) ), this, SLOT( downloadSharedFile( VNumber, VNumber ) ) );
    connect( mp_fileSharing, SIGNAL( downloadSharedFilesRequest( const QList<SharedFileInfo>& ) ), this, SLOT( downloadSharedFiles( QList<SharedFileInfo> ) ) );
    connect( beeCore, SIGNAL( userChanged( const User& ) ), mp_fileSharing, SLOT( onUserChanged( const User& ) ) );
  }
  mp_fileSharing->showUp();
}

void GuiMain::onFileSharingWindowClosed()
{
  if( mp_fileSharing )
    mp_fileSharing = Q_NULLPTR;
}

void GuiMain::showScreenShotWindow()
{
  if( !mp_screenShot )
  {
    mp_screenShot = new GuiScreenShot;
    mp_screenShot->setAttribute( Qt::WA_DeleteOnClose, true );
    Bee::setWindowStaysOnTop( mp_screenShot, Settings::instance().stayOnTop() );
    mp_screenShot->resize( 620,  460 );
    connect( mp_screenShot, SIGNAL( screenShotToSend( const QString& ) ), this, SLOT( sendFile( const QString& ) ) );
    connect( mp_screenShot, SIGNAL( destroyed() ), this, SLOT( onScreenShotWindowClosed() ) );
  }
  mp_screenShot->showUp();
}

void GuiMain::onScreenShotWindowClosed()
{
  if( mp_screenShot )
    mp_screenShot = Q_NULLPTR;
}

void GuiMain::showLogWindow()
{
  if( !mp_log )
  {
    mp_log = new GuiLog;
    Bee::setWindowStaysOnTop( mp_log, Settings::instance().stayOnTop() );
    mp_log->resize( qMin( (QApplication::desktop()->availableGeometry().width()-20), 760 ), 460 );
    connect( mp_log, SIGNAL( destroyed() ), this, SLOT( onLogWindowClosed() ) );
  }
  mp_log->showUp();
}

void GuiMain::onLogWindowClosed()
{
  if( mp_log )
    mp_log = Q_NULLPTR;
}

void GuiMain::onMainTabChanged( int tab_index )
{
  if( mp_tabMain->widget( tab_index ) == mp_home )
  {
    m_unreadActivities = 0;
    updateTabTitles();
  }

  if( m_changeTabToUserListOnFirstConnected )
    m_changeTabToUserListOnFirstConnected = false;
}

void GuiMain::setFileTransferEnabled( bool enable )
{
  if( Settings::instance().disableFileTransfer() )
    return;

  Settings::instance().setEnableFileTransfer( enable );
  if( !enable )
  {
    Settings::instance().setEnableFileSharing( false );
    Settings::instance().setUseShareBox( false );
    mp_actEnableFileSharing->setChecked( false );
    beeCore->stopFileTransferServer();
    QMetaObject::invokeMethod( beeCore, "buildLocalShareList", Qt::QueuedConnection );
  }
  else
  {
    if( !Bee::folderIsWriteable( Settings::instance().downloadDirectory(), false ) )
    {
      QMessageBox::warning( this, Settings::instance().programName(), tr( "The download folder does not exists or is not writable. Please select a new one." ), tr( "Ok" ) );
      selectDownloadDirectory();
    }
    beeCore->startFileTransferServer();
  }
  checkViewActions();
}

void GuiMain::setFileSharingEnabled( bool enable )
{
  if( Settings::instance().disableFileTransfer() || Settings::instance().disableFileSharing() )
    return;

  Settings::instance().setEnableFileSharing( enable );
  QMetaObject::invokeMethod( beeCore, "buildLocalShareList", Qt::QueuedConnection );
  checkViewActions();
  if( !enable )
  {
    if( mp_fileSharing )
      mp_fileSharing->close();
  }
}

void GuiMain::showWorkgroups()
{
  GuiWorkgroups gw( this );
  gw.loadWorkgroups();
  gw.setModal( true );
  gw.setSizeGripEnabled( true );
  gw.show();
  if( gw.exec() != QDialog::Accepted )
    return;

  if( Settings::instance().acceptConnectionsOnlyFromWorkgroups() && !Settings::instance().localUser().workgroups().isEmpty() )
    qDebug() << "You have selected to accept connections only from these workgroups:" << qPrintable( Settings::instance().localUser().workgroups().join( ", " ) );

  if( gw.restartConnection() )
    showRestartConnectionAlertMessage();
}

void GuiMain::showRefusedChats()
{
  GuiRefusedChat grc( this );
  int previous_refused_chats = grc.loadRefusedChats();
  grc.setModal( true );
  grc.setSizeGripEnabled( true );
  grc.show();
  if( grc.exec() == QDialog::Accepted )
  {
    showMessage( tr( "%1 blocked chats" ).arg( Settings::instance().refusedChats().size() ), 5000 );
    if( previous_refused_chats != ChatManager::instance().refusedChats().size() )
      showRestartConnectionAlertMessage();
  }
}

void GuiMain::selectIconSourcePath()
{
  int ret_code = 1;
  if( Settings::instance().iconSourcePath().isEmpty() )
  {
    ret_code = QMessageBox::information( this, Settings::instance().programName(),
                                         tr( "You are using the default icons." ),
                                         tr( "Select the icon folder" ), tr( "Cancel" ), QString(), 1, 1 );
  }
  else
  {
    ret_code = QMessageBox::information( this, Settings::instance().programName(),
                                         tr( "You are using custom icons in folder %1." ).arg( Settings::instance().iconSourcePath() ),
                                         tr( "Select the icon folder" ), tr( "Cancel" ), tr( "Restore default icons" ), 1, 1 );
  }

  if( ret_code == 1 )
    return;

  QString icon_source_path = "";
  if( ret_code == 0 )
  {
    icon_source_path = QFileDialog::getExistingDirectory( this, tr( "Select the icon folder" ),
                                                                Settings::instance().iconSourcePath().isEmpty() ? Settings::instance().resourceFolder() : Settings::instance().iconSourcePath() );
    if( icon_source_path.isEmpty() )
      return;

    icon_source_path = Bee::convertToNativeFolderSeparator( icon_source_path );
  }

  if( icon_source_path != Settings::instance().iconSourcePath() )
  {
    showRestartApplicationAlertMessage();
    Settings::instance().setIconSourcePath( icon_source_path );
  }
}

void GuiMain::selectEmoticonSourcePath()
{
  int ret_code = 1;
  if( Settings::instance().emoticonSourcePath().isEmpty() )
  {
    ret_code = QMessageBox::information( this, Settings::instance().programName(),
                                         tr( "You are using the default emoticons." ),
                                         tr( "Select the emoticon folder" ), tr( "Cancel" ), QString(), 1, 1 );
  }
  else
  {
    ret_code = QMessageBox::information( this, Settings::instance().programName(),
                                         tr( "You are using custom emoticons in folder %1." ).arg( Settings::instance().emoticonSourcePath() ),
                                         tr( "Select the emoticon folder" ), tr( "Cancel" ), tr( "Restore default emoticons" ), 1, 1 );
  }

  if( ret_code == 1 )
    return;

  QString emoticon_source_path = "";
  if( ret_code == 0 )
  {
    emoticon_source_path = QFileDialog::getExistingDirectory( this, tr( "Select the emoticon folder" ),
                                                                  Settings::instance().emoticonSourcePath().isEmpty() ? Settings::instance().resourceFolder() : Settings::instance().emoticonSourcePath() );
    if( emoticon_source_path.isEmpty() )
      return;

    emoticon_source_path = Bee::convertToNativeFolderSeparator( emoticon_source_path );
  }

  if( emoticon_source_path != Settings::instance().emoticonSourcePath() )
  {
    Settings::instance().setEmoticonSourcePath( emoticon_source_path );
    updateEmoticons();
  }
}

void GuiMain::showRestartConnectionAlertMessage()
{
  if( QMessageBox::information( this, Settings::instance().programName(), tr( "You have to restart your connection to apply changes." ),
                            tr( "Restart connection now" ), tr( "Restart later manually" ), QString(), 0, 1 ) == 0 )
  {
    QTimer::singleShot( 0, this, SLOT( restartCore() ) );
  }
}

void GuiMain::showRestartApplicationAlertMessage()
{
  QMessageBox::information( this, Settings::instance().programName(), tr( "You must restart %1 to apply these changes." ).arg( Settings::instance().programName() ), tr( "Ok" ) );
}

void GuiMain::onNewsAvailable( const QString& news )
{
#ifdef BEEBEEP_DEBUG
  qDebug() << "News from the website available:" << qPrintable( news );
#endif
  mp_home->setNews( news );
}

void GuiMain::createMessage()
{
  if( Settings::instance().disableCreateMessage() )
  {
    QMessageBox::information( this, Settings::instance().programName(),
                              tr( "You are not allowed create new message." ) + QString( " " ) + tr( "The option has been disabled by your system administrator." ),
                              tr( "Ok" ) );
    return;
  }
  GuiCreateMessage gcm( this );
  gcm.setModal( true );
  gcm.setSizeGripEnabled( true );
  gcm.show();
  if( gcm.exec() == QDialog::Accepted )
  {
    if( !gcm.toChatIdList().isEmpty() )
    {
      int max_chat_to_open = 6;
      int num_chat_opened = 0;
      foreach( VNumber chat_id, gcm.toChatIdList() )
      {
        beeCore->sendChatMessage( chat_id, gcm.message(), gcm.messageIsImportant(), num_chat_opened >= max_chat_to_open, false );
        if( gcm.openChat() && num_chat_opened < max_chat_to_open )
        {
          showChat( chat_id );
          num_chat_opened++;
        }
      }
      updateTabTitles();
      mp_chatList->updateChats();
    }
  }
}

void GuiMain::updateChatFont()
{
  if( m_floatingChats.isEmpty() )
    return;

  foreach( GuiFloatingChat* fl_chat, m_floatingChats )
    fl_chat->guiChat()->setChatFont( Settings::instance().chatFont() );
}

void GuiMain::updateChatColors()
{
  if( m_floatingChats.isEmpty() )
    return;

  foreach( GuiFloatingChat* fl_chat, m_floatingChats )
  {
    if( fl_chat->guiChat()->chatId() != ID_DEFAULT_CHAT )
      fl_chat->guiChat()->updateChatColors();
  }
}

void GuiMain::resetAllColors()
{
  if( QMessageBox::question( this, Settings::instance().programName(), tr( "Do you really want to restore the colors to the default ones?" ), tr( "Yes" ), tr( "No" ), QString(), 1, 1 ) == 0 )
  {
    Settings::instance().resetAllColors();
    Settings::instance().save();
    loadStyle();
  }
}

void GuiMain::onHideEmptyChatsRequest()
{
  Settings::instance().setHideEmptyChatsInList( !Settings::instance().hideEmptyChatsInList() );
  mp_chatList->updateChats();
}

void GuiMain::onApplicationFocusChanged( QWidget* old, QWidget* now )
{
  if( old == Q_NULLPTR && isAncestorOf( now )  )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Main window has grabbed focus";
#endif
    m_prevActivatedState = true;
    return;
  }

  if( isAncestorOf( old ) && now == Q_NULLPTR )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Main window has lost focus";
#endif
    m_prevActivatedState = false;
    return;
  }

  bool current_state = isActiveWindow();
  if( current_state != m_prevActivatedState )
  {
    m_prevActivatedState = current_state;
#ifdef BEEBEEP_DEBUG
    if( current_state )
      qDebug() << "Main window has grabbed focus (active)";
    else
      qDebug() << "Main window has lost focus (active)";
#endif
  }
}

#ifdef BEEBEEP_USE_WEBENGINE
void GuiMain::showWebView()
{
  mp_webView->show();
}

void GuiMain::onNewsLoad( bool ok )
{
  mp_actWebView->setEnabled( ok );
}
#endif

#ifdef BEEBEEP_USE_SHAREDESKTOP
void GuiMain::onShareDesktopImageAvailable( const User& u, const QImage& img, const QString& image_type, QRgb diff_color )
{
  foreach( GuiShareDesktop* gsd, m_desktops )
  {
    if( gsd->userId() == u.id() )
    {
      if( img.isNull() )
        gsd->close();
      else
        gsd->updateImage( img, image_type, diff_color );
      return;
    }
  }

  if( img.isNull() )
    return;

  int desktop_h = qApp->desktop()->availableGeometry().height() - 50;
  int desktop_w = qApp->desktop()->availableGeometry().width() - 80;
  int frame_w = frameGeometry().width() - geometry().width();
  int frame_h = frameGeometry().height() - geometry().height();
  int max_img_w = img.width()+ frame_w;
  int max_img_h = img.height()+ frame_h;

  GuiShareDesktop* new_gui = new GuiShareDesktop;
  connect( new_gui, SIGNAL( shareDesktopClosed( VNumber ) ), this, SLOT( onShareDesktopCloseEvent( VNumber ) ) );
  connect( new_gui, SIGNAL( shareDesktopDeleteRequest( VNumber ) ), this, SLOT( onShareDesktopDeleteRequest( VNumber ) ) );
  new_gui->setUser( u );
  new_gui->setGeometry( 10, 40, qMin( max_img_w, qMax( 640, desktop_w ) ), qMin( max_img_h, qMax( 480, desktop_h ) ) );
  new_gui->setMaximumSize( max_img_w, max_img_h );
  new_gui->show();
  new_gui->updateImage( img, image_type, diff_color );

  m_desktops.append( new_gui );
}

void GuiMain::onShareDesktopCloseEvent( VNumber user_id )
{
#ifdef BEEBEEP_DEBUG
  qDebug() << "You close the desktop shared viewer for user" << user_id;
#endif
  beeCore->refuseToViewShareDesktop( ID_LOCAL_USER, user_id );
}

void GuiMain::onShareDesktopDeleteRequest( VNumber user_id )
{
  QList<GuiShareDesktop*>::iterator it = m_desktops.begin();
  while( it != m_desktops.end() )
  {
    if( (*it)->userId() == user_id )
    {
#ifdef BEEBEEP_DEBUG
      qDebug() << "Delete GuiShareDesktop for user" << user_id;
#endif
      (*it)->disconnect();
      (*it)->deleteLater();
      m_desktops.erase( it );
      return;
    }
  }
}

void GuiMain::onShareDesktopRequestFromChat( VNumber chat_id, bool enable_desktop_sharing )
{
  if( chat_id == ID_DEFAULT_CHAT )
    return;

  if( beeCore->shareDesktopIsActive( ID_INVALID ) == enable_desktop_sharing )
    return;

  Chat c = ChatManager::instance().chat( chat_id );
  if( !c.isValid() )
  {
    qWarning() << "Invalid chat" << chat_id << "found in GuiMain::onShareDesktopRequestFromChat(...)";
    return;
  }

  if( enable_desktop_sharing )
  {
    if( QMessageBox::question( activeWindow(), Settings::instance().programName(),
                               tr( "Do you really want to share your desktop with %1?" ).arg( c.name() ),
                               tr( "Yes" ), tr( "No" ), QString(), 0, 1 ) == 1 )
      return;

    foreach( VNumber user_id, c.usersId() )
    {
      if( user_id != ID_LOCAL_USER )
        beeCore->startShareDesktop( user_id );
    }
  }
  else
  {
    foreach( VNumber user_id, c.usersId() )
    {
      if( user_id != ID_LOCAL_USER )
        beeCore->stopShareDesktop( user_id );
    }
  }
}

void GuiMain::onShareDesktopUpdate( const User& u )
{
  bool core_is_connected = beeCore->isConnected();
  int connected_users = beeCore->connectedUsers();
  bool file_transfer_is_active = beeCore->isFileTransferActive();
  QList<Chat> chat_list = ChatManager::instance().chatsWithUser( u.id() );
  foreach( Chat c, chat_list )
  {
    GuiFloatingChat* fl_chat = floatingChat( c.id() );
    if( fl_chat )
      fl_chat->guiChat()->updateActions( c, core_is_connected, connected_users, file_transfer_is_active );
  }
}

void GuiMain::sendScreenshotToChat( VNumber chat_id )
{
  QMetaObject::invokeMethod( beeCore, "sendScreenshotToChat", Qt::QueuedConnection, Q_ARG(VNumber, chat_id) );
  GuiFloatingChat* fl_chat = floatingChat( chat_id );
  if( fl_chat && fl_chat->isMinimized() )
    QTimer::singleShot( 200, fl_chat, SLOT( showNormal() ) );
}
#endif

void GuiMain::showNetworkTest()
{
  if( !mp_networkTest )
  {
    mp_networkTest = new GuiNetworkTest( this );
    mp_networkTest->updateSettings( Settings::instance().enableFileTransfer() ? QString::number( beeCore->fileTransferPort() ) : tr( "disabled" ) );
    connect( mp_networkTest, SIGNAL( destroyed() ), this, SLOT( onScreenShotWindowClosed() ) );
  }
  mp_networkTest->showUp();
}

void GuiMain::onNetworkTestWindowClosed()
{
  if( mp_networkTest )
    mp_networkTest = Q_NULLPTR;
}

void GuiMain::setMinimumWidthForStyle()
{
  if( Settings::instance().resetMinimumWidthForStyle() )
  {
    int old_w = width();
    int wasted_w = 20;
#if defined( Q_OS_MAC )
    int icon_wasted_w = m_useFusionStyle ? 4 : 8;
    int min_w = qMax( 300, mp_barMain->actions().size() * (mp_barMain->iconSize().width() + icon_wasted_w) + wasted_w );
#elif defined( Q_OS_UNIX )
    int min_w = qMax( 320, mp_barMain->actions().size() * (mp_barMain->iconSize().width()+4) + wasted_w );
#elif defined( Q_OS_WIN )
    int min_w = qMax( 320, mp_barMain->actions().size() * (mp_barMain->iconSize().width()+2) + wasted_w );
#else
    int min_w = qMax( 320, mp_barMain->actions().size() * (mp_barMain->iconSize().width()+2) + wasted_w );
#endif
    setMinimumWidth( min_w );
    if( min_w > old_w )
    {
      int new_pos_x = qMax( 1, pos().x() - (min_w - old_w) );
      move( new_pos_x, pos().y() );
    }
  }
  else
    setMinimumWidth( 90 );
}

void GuiMain::loadStyle()
{
  QStyle* p_style = QStyleFactory::create( "Fusion" );
  if( p_style )
  {
    beeApp->setStyle( p_style );
    m_useFusionStyle = true;
  }
  else
  {
    beeApp->resetStyle();
    m_useFusionStyle = false;
  }

  if( Settings::instance().useDarkStyle() )
  {
    qDebug() << "Stylesheet:" << (m_useFusionStyle ? "Fusion" : "System Default") << "Dark";
    beeApp->setPalette( Bee::darkPalette() );
  }
  else
  {
    qDebug() << "Stylesheet:" << (m_useFusionStyle ? "Fusion" : "System Default" );
    beeApp->resetPalette();
  }

  setMinimumWidthForStyle();

  mp_home->updateBackground();
  mp_userList->updateBackground();
  mp_chatList->updateBackground();
  mp_groupList->updateBackground();
  mp_savedChatList->updateBackground();
  updateChatColors();
  updateEmoticons();
}

#ifdef BEEBEEP_USE_VOICE_CHAT
void GuiMain::sendVoiceMessageToChat( VNumber chat_id, const QString& file_path, qint64 message_duration )
{
  beeCore->sendVoiceMessageToChat( chat_id, file_path, message_duration );
}

void GuiMain::showVoiceEncoderSettings()
{
  GuiRecordVoiceMessageSettings grvm( this );
  grvm.setModal( true );
  grvm.init();
  grvm.loadSettings();
  grvm.show();
  if( grvm.exec() == QDialog::Accepted )
  {
    Settings::instance().save();
    AudioManager::instance().checkDefaultAudioDevice();
  }
}
#endif

void GuiMain::resumeFileTransfer( VNumber user_id, const FileInfo& file_info )
{
  beeCore->resumeFileTransfer( user_id, file_info );
}

void GuiMain::changeEmoticonSizeInChat()
{
  bool ok = false;
  int max_size = Settings::instance().emoticonSourcePath().isEmpty() ? (Settings::instance().useHiResEmoticons() ? 190 : 24) : 248;
  int emoticon_size = QInputDialog::getInt( this, Settings::instance().programName(), tr( "Please select the new size of the emoticons in chat" ),
                                          Settings::instance().emoticonSizeInChat(), 16, max_size, 4, &ok );
  if( !ok )
    return;

  Settings::instance().setEmoticonSizeInChat( emoticon_size );
}

void GuiMain::selectFirstChat()
{
  if( isMinimized() )
    showNormal();
  else
    show();
  raiseOnTop();

  if( mp_tabMain->currentWidget() != mp_chatList )
    mp_tabMain->setCurrentWidget( mp_chatList );
  qApp->setActiveWindow( this );
  mp_chatList->selectFirstChat();

}
