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

#include "Core.h"
#include "BeeApplication.h"
#include "BeeUtils.h"
#include "ChatManager.h"
#include "FileShare.h"
#include "GuiAskPassword.h"
#include "GuiChat.h"
#include "GuiChatList.h"
#include "GuiCreateGroup.h"
#include "GuiEditVCard.h"
#include "GuiGroupList.h"
#include "GuiLanguage.h"
#include "GuiLog.h"
#include "GuiPluginManager.h"
#include "GuiSavedChat.h"
#include "GuiSavedChatList.h"
#include "GuiScreenShot.h"
#include "GuiSearchUser.h"
#include "GuiShareLocal.h"
#include "GuiShareNetwork.h"
#include "GuiSystemTray.h"
#include "GuiTransferFile.h"
#include "GuiUserList.h"
#include "GuiMain.h"
#include "GuiVCard.h"
#include "GuiWizard.h"
#include "PluginManager.h"
#include "SaveChatList.h"
#include "Settings.h"
#include "UserManager.h"
#ifdef Q_OS_WIN
  #include "windows.h"
#endif


GuiMain::GuiMain( QWidget *parent )
 : QMainWindow( parent )
{
  setObjectName( "GuiMainWindow" );
  mp_core = new Core( this );
  mp_sound = 0;

  setWindowIcon( QIcon( ":/images/beebeep.png") );

  mp_stackedWidget = new QStackedWidget( this );
  createStackedWidgets();
  setCentralWidget( mp_stackedWidget );

  mp_barMain = addToolBar( tr( "Show the main tool bar" ) );
  mp_barMain->setObjectName( "GuiMainToolBar" );
  mp_barMain->setIconSize( Settings::instance().mainBarIconSize() );

  mp_barPlugins = addToolBar( tr( "Show the bar of plugins" ) );
  mp_barPlugins->setObjectName( "GuiPluginToolBar" );
  mp_barPlugins->setIconSize( Settings::instance().mainBarIconSize() );

  mp_trayIcon = new GuiSystemTray( this );

  m_lastUserStatus = User::Online;

  createActions();
  createDockWindows();
  createMenus();
  createToolAndMenuBars();
  createPluginWindows();
  updadePluginMenu();
  createStatusBar();

  connect( mp_core, SIGNAL( chatMessage( VNumber, const ChatMessage& ) ), this, SLOT( showChatMessage( VNumber, const ChatMessage& ) ) );
  connect( mp_core, SIGNAL( fileDownloadRequest( const User&, const FileInfo& ) ), this, SLOT( downloadFile( const User&, const FileInfo& ) ) );
  connect( mp_core, SIGNAL( userChanged( const User& ) ), this, SLOT( checkUser( const User& ) ) );
  connect( mp_core, SIGNAL( userIsWriting( const User& ) ), this, SLOT( showWritingUser( const User& ) ) );
  connect( mp_core, SIGNAL( fileTransferProgress( VNumber, const User&, const FileInfo&, FileSizeType ) ), mp_fileTransfer, SLOT( setProgress( VNumber, const User&, const FileInfo&, FileSizeType ) ) );
  connect( mp_core, SIGNAL( fileTransferMessage( VNumber, const User&, const FileInfo&, const QString& ) ), mp_fileTransfer, SLOT( setMessage( VNumber, const User&, const FileInfo&, const QString& ) ) );
  connect( mp_core, SIGNAL( fileShareAvailable( const User& ) ), this, SLOT( showSharesForUser( const User& ) ) );
  connect( mp_core, SIGNAL( updateChat( VNumber ) ), this, SLOT( checkChat( VNumber ) ) );
  connect( mp_core, SIGNAL( localShareListAvailable() ), mp_shareLocal, SLOT( updateFileSharedList() ) );
  connect( mp_core, SIGNAL( savedChatListAvailable() ), mp_savedChatList, SLOT( updateSavedChats() ) );
  connect( mp_core, SIGNAL( updateStatus( const QString&, int ) ), this, SLOT( showMessage( const QString&, int ) ) );
  connect( mp_core, SIGNAL( updateGroup( VNumber ) ), this, SLOT( checkGroup( VNumber ) ) );
  connect( mp_fileTransfer, SIGNAL( transferCancelled( VNumber ) ), mp_core, SLOT( cancelFileTransfer( VNumber ) ) );
  connect( mp_fileTransfer, SIGNAL( stringToShow( const QString&, int ) ), this, SLOT( showMessage( const QString&, int ) ) );
  connect( mp_fileTransfer, SIGNAL( fileTransferProgress( VNumber, VNumber, const QString& ) ), mp_shareNetwork, SLOT( showMessage( VNumber, VNumber, const QString& ) ) );
  connect( mp_fileTransfer, SIGNAL( fileTransferCompleted( VNumber, VNumber, const QString& ) ), mp_shareNetwork, SLOT( setFileTransferCompleted( VNumber, VNumber, const QString& ) ) );
  connect( mp_fileTransfer, SIGNAL( openFileCompleted( const QUrl& ) ), this, SLOT( openUrl( const QUrl& ) ) );
  connect( mp_chat, SIGNAL( newMessage( VNumber, const QString& ) ), this, SLOT( sendMessage( VNumber, const QString& ) ) );
  connect( mp_chat, SIGNAL( writing( VNumber ) ), mp_core, SLOT( sendWritingMessage( VNumber ) ) );
  connect( mp_chat, SIGNAL( nextChat() ), this, SLOT( showNextChat() ) );
  connect( mp_chat, SIGNAL( openUrl( const QUrl& ) ), this, SLOT( openUrl( const QUrl& ) ) );
  connect( mp_chat, SIGNAL( sendFileFromChatRequest( VNumber, const QString& ) ), this, SLOT( sendFileFromChat( VNumber, const QString& ) ) );
  connect( mp_chat, SIGNAL( createGroupChatRequest() ), this, SLOT( createGroupChat() ) );
  connect( mp_chat, SIGNAL( createGroupRequest() ), this, SLOT( createGroup() ) );
  connect( mp_chat, SIGNAL( editGroupRequest() ), this, SLOT( addUserToGroupChat() ) );
  connect( mp_chat, SIGNAL( chatToClear( VNumber ) ), this, SLOT( clearChat( VNumber ) ) );
  connect( mp_chat, SIGNAL( leaveThisChat( VNumber ) ), this, SLOT( leaveGroupChat( VNumber ) ) );

  connect( mp_shareLocal, SIGNAL( sharePathAdded( const QString& ) ), this, SLOT( addToShare( const QString& ) ) );
  connect( mp_shareLocal, SIGNAL( sharePathRemoved( const QString& ) ), this, SLOT( removeFromShare( const QString& ) ) );
  connect( mp_shareLocal, SIGNAL( openUrlRequest( const QUrl& ) ), this, SLOT( openUrl( const QUrl& ) ) );
  connect( mp_shareLocal, SIGNAL( updateListRequest() ), mp_core, SLOT( buildLocalShareList() ) );

  connect( mp_shareNetwork, SIGNAL( fileShareListRequested() ), mp_core, SLOT( sendFileShareRequestToAll() ) );
  connect( mp_shareNetwork, SIGNAL( downloadSharedFile( VNumber, VNumber ) ), this, SLOT( downloadSharedFile( VNumber, VNumber ) ) );
  connect( mp_shareNetwork, SIGNAL( openFileCompleted( const QUrl& ) ), this, SLOT( openUrl( const QUrl& ) ) );
  connect( mp_shareNetwork, SIGNAL( updateStatus( const QString&, int ) ), this, SLOT( showMessage( const QString&, int ) ) );

  connect( mp_userList, SIGNAL( chatSelected( VNumber ) ), this, SLOT( showChat( VNumber ) ) );
  connect( mp_userList, SIGNAL( menuToShow( VNumber ) ), this, SLOT( showUserMenu( VNumber ) ) );

  connect( mp_groupList, SIGNAL( openChatForGroupRequest( VNumber ) ), this, SLOT( showChatForGroup( VNumber ) ) );
  connect( mp_groupList, SIGNAL( createGroupRequest() ), this, SLOT( createGroup() ) );
  connect( mp_groupList, SIGNAL( editGroupRequest( VNumber ) ), this, SLOT( editGroup( VNumber ) ) );
  connect( mp_groupList, SIGNAL( showVCardRequest( VNumber ) ), this, SLOT( showUserMenu( VNumber ) ) );
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

  initGuiItems();
}

void GuiMain::checkWindowFlagsAndShow()
{
  if( Settings::instance().stayOnTop() )
  {
    setWindowFlags( windowFlags() | Qt::WindowStaysOnTopHint );
  }
  else
  {
    setWindowFlags( windowFlags() & ~Qt::WindowStaysOnTopHint );
  }

  checkViewActions();
  show();
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

void GuiMain::forceExit()
{
  if( mp_core->isConnected() )
    mp_core->stop();
  close();
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
    if( Settings::instance().minimizeInTray() && QSystemTrayIcon::isSystemTrayAvailable() )
    {
      QTimer::singleShot( 0, this, SLOT( hideToTrayIcon() ) );
      e->ignore();
      return;
    }

    if( QMessageBox::question( this, Settings::instance().programName(), tr( "Do you really want to quit %1?" ).arg( Settings::instance().programName() ),
                               tr( "Yes" ), tr( "No" ), QString::null, 1, 1 ) == 1 )
    {
      e->ignore();
      return;
    }

    mp_core->stop();
  }

  QSettings* sets = Settings::instance().objectSettings();
  sets->deleteLater();
  if( !sets->isWritable() )
  {
    if( QMessageBox::warning( this, Settings::instance().programName(),
                              QString( "%1<br />%2<br />%3<br />%4<br />%5" ).arg( tr( "<b>Settings can not be saved</b>. Path:" ) )
                                                                     .arg( sets->fileName() )
                                                                     .arg( tr( "<b>is not writable</b> by user:" ) )
                                                                     .arg( Settings::instance().localUser().accountPath() )
                                                                     .arg( tr( "Do you want to close anyway?" ) ),
                              tr( "Yes" ), tr( "No" ), QString::null, 1, 1 ) == 1 )
    {
      e->ignore();
      return;
    }
  }

  if( isVisible() )
  {
    Settings::instance().setGuiGeometry( saveGeometry() );
    Settings::instance().setGuiState( saveState() );
  }

  // maybe timer is active
  mp_logView->stopCheckingLog();

  e->accept();
}

void GuiMain::showNextChat()
{
#ifdef BEEBEEP_DEBUG
  qDebug() << "Show next chat in list with unread messages";
#endif
  if( !mp_userList->nextUserWithUnreadMessages() )
    showMessage( tr( "No new message available" ), 0 );
}

void GuiMain::startStopCore()
{
  if( mp_core->isConnected() )
    stopCore();
  else
    startCore();
}

void GuiMain::startCore()
{
  if( Settings::instance().firstTime() )
  {
    showWizard();
    Settings::instance().setFirstTime( false );
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

  showChat( ID_DEFAULT_CHAT );

  if( enable )
  {
    mp_actStartStopCore->setIcon( QIcon( ":/images/disconnect.png") );
    mp_actStartStopCore->setText( tr( "&Disconnect" ) );
    mp_actStartStopCore->setStatusTip( tr( "Disconnect from %1 network").arg( Settings::instance().programName() ) );
  }
  else
  {
    mp_actStartStopCore->setIcon( QIcon( ":/images/connect.png") );
    mp_actStartStopCore->setText( tr( "&Connect" ) );
    mp_actStartStopCore->setStatusTip( tr( "Connect to %1 network").arg( Settings::instance().programName() ) );
  }

  mp_actSearch->setEnabled( enable );
  mp_userList->setDefaultChatConnected( enable );

  updateStatusIcon();

  refreshTitle( Settings::instance().localUser() );
}

void GuiMain::checkViewActions()
{
  bool is_connected = mp_core->isConnected();
  int connected_users = mp_core->connectedUsers();

  mp_actViewDefaultChat->setEnabled( mp_stackedWidget->currentWidget() != mp_chat );
  mp_actViewShareLocal->setEnabled( mp_stackedWidget->currentWidget() != mp_shareLocal );
  mp_actViewShareNetwork->setEnabled( mp_stackedWidget->currentWidget() != mp_shareNetwork && is_connected && connected_users > 0 );
  mp_actViewLog->setEnabled( mp_stackedWidget->currentWidget() != mp_logView );
  mp_actViewScreenShot->setEnabled( mp_stackedWidget->currentWidget() != mp_screenShot );

  if( mp_stackedWidget->currentWidget() == mp_chat )
  {
    mp_chat->updateAction( is_connected, connected_users );
    mp_barChat->show();
  }
  else
    mp_barChat->hide();

  if( mp_stackedWidget->currentWidget() == mp_shareNetwork )
    mp_barShareNetwork->show();
  else
    mp_barShareNetwork->hide();

  if( mp_stackedWidget->currentWidget() == mp_shareLocal )
    mp_barShareLocal->show();
  else
    mp_barShareLocal->hide();

  if( mp_stackedWidget->currentWidget() != mp_logView )
    mp_logView->stopCheckingLog();
  else
    mp_logView->startCheckingLog();

  if( mp_stackedWidget->currentWidget() == mp_screenShot )
    mp_barScreenShot->show();
  else
    mp_barScreenShot->hide();
}

void GuiMain::showAbout()
{
  QMessageBox::about( this, Settings::instance().programName(),
                      QString( "<b>%1</b> - %2<br /><br />%3 %4<br />%5 %6<br />%7<br />" )
                      .arg( Settings::instance().programName() )
                      .arg( tr( "Secure Lan Messenger" ) )
                      .arg( tr( "Version" ) )
                      .arg( Settings::instance().version( true ) )
                      .arg( tr( "developed by" ) )
                      .arg( QString( "<a href='http://it.linkedin.com/pub/marco-mastroddi/20/5a7/191'>Marco Mastroddi</a>" ) )
                      .arg( QString( "e-mail: marco.mastroddi@gmail.com" ) )
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
  mp_actStartStopCore = new QAction( QIcon( ":/images/connect.png"), tr( "&Connect"), this );
  mp_actStartStopCore->setStatusTip( tr( "Connect to %1 network").arg( Settings::instance().programName() ) );
  connect( mp_actStartStopCore, SIGNAL( triggered() ), this, SLOT( startStopCore() ) );

  mp_actSearch = new QAction( QIcon( ":/images/search.png"), tr( "Add or search &user..."), this );
  mp_actSearch->setStatusTip( tr( "Add or search an user in the %1 network" ).arg( Settings::instance().programName() ) );
  mp_actSearch->setEnabled( false );
  connect( mp_actSearch, SIGNAL( triggered() ), this, SLOT( searchUsers() ) );

  mp_actQuit = new QAction( QIcon( ":/images/quit.png" ), tr( "&Quit" ), this );
  mp_actQuit->setShortcuts( QKeySequence::Quit );
  mp_actQuit->setStatusTip( tr( "Close the chat and quit %1" ).arg( Settings::instance().programName() ) );
  connect( mp_actQuit, SIGNAL( triggered() ), this, SLOT( forceExit() ) );

  mp_actVCard = new QAction( QIcon( ":/images/profile-edit.png"), tr( "Profile..." ), this );
  mp_actVCard->setStatusTip( tr( "Change your profile data" ) );
  connect( mp_actVCard, SIGNAL( triggered() ), this, SLOT( changeVCard() ) );

  mp_actToolBar = mp_barMain->toggleViewAction();
  mp_actToolBar->setStatusTip( tr( "Show the main tool bar with settings" ) );
  mp_actToolBar->setData( 99 );

  mp_actPluginBar = mp_barPlugins->toggleViewAction();
  mp_actPluginBar->setStatusTip( tr( "Show the tool bar with plugin shortcuts" ) );
  mp_actPluginBar->setData( 99 );

  mp_actAbout = new QAction( QIcon( ":/images/beebeep.png" ), tr( "&About %1..." ).arg( Settings::instance().programName() ), this );
  mp_actAbout->setStatusTip( tr( "Show the informations about %1" ).arg( Settings::instance().programName() ) );
  connect( mp_actAbout, SIGNAL( triggered() ), this, SLOT( showAbout() ) );
}

void GuiMain::createMenus()
{
  QAction* act;

  /* Main Menu */
  mp_menuMain = new QMenu( tr( "&Main" ), this );
  mp_menuMain->addAction( mp_actStartStopCore );
  mp_menuMain->addSeparator();
  mp_menuMain->addAction( mp_actVCard );
  mp_menuMain->addAction( mp_actSearch );
  mp_menuMain->addSeparator();

  act = mp_menuMain->addAction( QIcon( ":/images/language.png" ), tr( "Select language..."), this, SLOT( selectLanguage() ) );
  act->setStatusTip( tr( "Select your preferred language" ) );
  act = mp_menuMain->addAction( QIcon( ":/images/download-folder.png" ), tr( "Download folder..."), this, SLOT( selectDownloadDirectory() ) );
  act->setStatusTip( tr( "Select the download folder" ) );
  mp_menuMain->addSeparator();

  act = mp_menuMain->addAction( QIcon( ":/images/file-beep.png" ), tr( "Select beep file..." ), this, SLOT( selectBeepFile() ) );
  act->setStatusTip( tr( "Select the file to play on new message arrived" ) );
  act = mp_menuMain->addAction( QIcon( ":/images/play.png" ), tr( "Play beep" ), this, SLOT( testBeepFile() ) );
  act->setStatusTip( tr( "Test the file to play on new message arrived" ) );
  mp_menuMain->addSeparator();

  mp_menuMain->addAction( mp_actQuit );

  /* Settings Menu */
  mp_menuSettings = new QMenu( tr( "&Settings" ), this );

  act = mp_menuSettings->addAction( tr( "Show only the online users" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled only the online users are shown in the list" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().showOnlyOnlineUsers() );
  act->setData( 6 );

  act = mp_menuSettings->addAction( tr( "Beep on new message arrived" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled when a new message is arrived a sound is emitted" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().beepOnNewMessageArrived() );
  act->setData( 4 );
  mp_actBeepOnNewMessage = act;

  act = mp_menuSettings->addAction( tr( "Raise on new message arrived" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled when a new message is arrived %1 is shown on top of all other windows" ).arg( Settings::instance().programName() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().raiseOnNewMessageArrived() );
  act->setData( 15 );

  act = mp_menuSettings->addAction( tr( "Generate automatic filename" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If the file to be downloaded already exists a new filename is automatically generated" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().automaticFileName() );
  act->setData( 7 );

  if( QSystemTrayIcon::isSystemTrayAvailable() )
  {
    act = mp_menuSettings->addAction( tr( "Close to tray icon" ), this, SLOT( settingsChanged() ) );
    act->setStatusTip( tr( "If enabled when the close button is clicked the window minimized to the system tray icon" ) );
    act->setCheckable( true );
    act->setChecked( Settings::instance().minimizeInTray() );
    act->setData( 11 );

    act = mp_menuSettings->addAction( tr( "Enable tray icon notification" ), this, SLOT( settingsChanged() ) );
    act->setStatusTip( tr( "If enabled tray icon shows some notification about status and message" ) );
    act->setCheckable( true );
    act->setChecked( Settings::instance().showNotificationOnTray()  );
    act->setData( 19 );
  }

  act = mp_menuSettings->addAction( tr( "Stay on top" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled %1 stays on top of the other windows" ).arg( Settings::instance().programName() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().stayOnTop() );
  act->setData( 14 );

  act = mp_menuSettings->addAction( tr( "Enable file transfer" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled you can transfer files with the other users" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().fileTransferIsEnabled() );
  act->setData( 12 );

  mp_actPromptPassword = mp_menuSettings->addAction( tr( "Prompts for network password on startup" ), this, SLOT( settingsChanged() ) );
  mp_actPromptPassword->setStatusTip( tr( "If enabled the password dialog will be shown on connection startup" ) );
  mp_actPromptPassword->setCheckable( true );
  mp_actPromptPassword->setChecked( Settings::instance().askPasswordAtStartup() );
  mp_actPromptPassword->setData( 17 );

  act = mp_menuSettings->addAction( tr( "Set away status when idle" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled %1 set your status to away after an idle of %2 minutes" ).arg( Settings::instance().programName() ).arg( Settings::instance().userAwayTimeout() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().autoUserAway() );
  act->setData( 20 );

#ifdef Q_OS_WIN
  act = mp_menuSettings->addAction( tr( "Load %1 on Windows startup" ).arg( Settings::instance().programName() ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled you can automatically load %1 at system startup" ).arg( Settings::instance().programName() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().hasStartOnSystemBoot() );
  act->setData( 16 );
#endif

  /* Chat Menu */
  mp_menuChat = new QMenu( tr( "&Chat" ), this );

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

  act = mp_menuChat->addAction( tr( "Show the messages' timestamp" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled the message shows its timestamp in the chat window" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().chatShowMessageTimestamp() );
  act->setData( 3 );

  act = mp_menuChat->addAction( tr( "Show the user's colors" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled the user's nickname in chat and in list is colored" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().showUserColor() );
  act->setData( 5 );

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

  act = mp_menuChat->addAction( tr( "Show emoticons" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled the emoticons will be recognized and shown as images" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().showEmoticons() );
  act->setData( 10 );

  act = mp_menuChat->addAction( tr( "Show messages grouped by user" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled the messages will be shown grouped by user" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().showMessagesGroupByUser() );
  act->setData( 13 );

  act = mp_menuChat->addAction( tr( "Save messages" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled the messages are saved when the program is closed" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().chatAutoSave() );
  act->setData( 18 );

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
  act = mp_menuStatus->addAction( QIcon( ":/images/user-status.png" ), tr( "Add a status description..." ), this, SLOT( changeStatusDescription() ) );
  mp_menuStatus->addSeparator();
  act = mp_menuStatus->addAction( QIcon( Bee::menuUserStatusIconFileName( User::Offline ) ), Bee::userStatusToString( User::Offline ), this, SLOT( statusSelected() ) );
  act->setData( User::Offline );
  act->setStatusTip( tr( "Your status will be %1" ).arg( Bee::userStatusToString( User::Offline ) ) );
  act->setIconVisibleInMenu( true );

  /* View Menu */
  mp_menuView = new QMenu( tr( "&View" ), this );
  mp_menuView->addAction( mp_actToolBar );
  mp_menuView->addAction( mp_actPluginBar );
  mp_menuView->addSeparator();
  mp_menuView->addAction( mp_actViewUsers );
  mp_menuView->addAction( mp_actViewGroups );
  mp_menuView->addAction( mp_actViewChats );
  mp_menuView->addAction( mp_actViewSavedChats );
  mp_menuView->addAction( mp_actViewFileTransfer );
  mp_menuView->addSeparator();
  mp_actViewDefaultChat = mp_menuView->addAction( QIcon( ":/images/chat-view.png" ), tr( "Show the chat" ), this, SLOT( showCurrentChat() ) );
  mp_actViewDefaultChat->setStatusTip( tr( "Show the chat view" ) );
  mp_actViewShareLocal = mp_menuView->addAction( QIcon( ":/images/upload.png" ), tr( "Show my shared files" ), this, SLOT( raiseLocalShareView() ) );
  mp_actViewShareLocal->setStatusTip( tr( "Show the list of the files which I have shared" ) );
  mp_actViewShareNetwork = mp_menuView->addAction( QIcon( ":/images/download.png" ), tr( "Show the network shared files" ), this, SLOT( raiseNetworkShareView() ) );
  mp_actViewShareNetwork->setStatusTip( tr( "Show the list of the network shared files" ) );

  /* Plugins Menu */
  mp_menuPlugins = new QMenu( tr( "Plugins" ), this );

  /* Help Menu */
  mp_menuInfo = new QMenu( tr("&?" ), this );
  act = mp_menuInfo->addAction( QIcon( ":/images/tip.png" ), tr( "Tip of the day" ), this, SLOT( showTipOfTheDay() ) );
  act->setStatusTip( tr( "Show me the tip of the day" ) );
  act = mp_menuInfo->addAction( QIcon( ":/images/fact.png" ), tr( "Fact of the day" ), this, SLOT( showFactOfTheDay() ) );
  act->setStatusTip( tr( "Show me the fact of the day" ) );
  mp_menuInfo->addSeparator();
  mp_menuInfo->addAction( mp_actAbout );
  act = mp_menuInfo->addAction( QIcon( ":/images/license.png" ), tr( "Show %1's license..." ).arg( Settings::instance().programName() ), this, SLOT( showLicense() ) );
  act->setStatusTip( tr( "Show the informations about %1's license" ).arg( Settings::instance().programName() ) );
  act = mp_menuInfo->addAction( QIcon( ":/images/qt.png" ), tr( "About &Qt..." ), qApp, SLOT( aboutQt() ) );
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
  act = mp_menuInfo->addAction( QIcon( ":/images/donate.png" ), tr( "Please donate for %1 :-)" ).arg( Settings::instance().programName() ), this, SLOT( openDonationPage() ) );
  act->setStatusTip( tr( "I'm so grateful and pleased about that" ) + QString( " :-)" ) );

}

void GuiMain::createToolAndMenuBars()
{
  menuBar()->addMenu( mp_menuMain );
  menuBar()->addMenu( mp_menuSettings );
  menuBar()->addMenu( mp_menuChat );
  menuBar()->addMenu( mp_menuView );
  menuBar()->addMenu( mp_menuPlugins );
  menuBar()->addMenu( mp_menuInfo );

  QLabel *label_version = new QLabel( this );
  label_version->setTextFormat( Qt::RichText );
  label_version->setAlignment( Qt::AlignCenter );
  QString label_version_text = QString( "&nbsp;<b>v %2</b>&nbsp;" )
#ifdef BEEBEEP_DEBUG
                            .arg( Settings::instance().version( true ) );
#else
                            .arg( Settings::instance().version( false ) );
#endif
  label_version->setText( label_version_text );
  menuBar()->setCornerWidget( label_version );

  mp_barMain->addAction( mp_menuStatus->menuAction() );
  mp_barMain->addSeparator();
  mp_barMain->addAction( mp_actViewUsers );
  mp_barMain->addAction( mp_actViewGroups );
  mp_barMain->addAction( mp_actViewChats );
  mp_barMain->addAction( mp_actViewSavedChats );
  mp_barMain->addAction( mp_actViewFileTransfer );
  mp_barMain->addSeparator();
  mp_barMain->addAction( mp_actViewDefaultChat );
  mp_barMain->addAction( mp_actViewShareLocal );
  mp_barMain->addAction( mp_actViewShareNetwork );

}

void GuiMain::createStatusBar()
{
  statusBar()->showMessage( tr( "Ready" ) );
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
  mp_actViewUsers->setText( tr( "Show online users" ) );
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
  mp_actViewGroups->setText( tr( "Show your groups" ) );
  mp_actViewGroups->setStatusTip( tr( "Show the list of your groups" ) );
  mp_actViewGroups->setData( 99 );

  QDockWidget* dock_widget = new QDockWidget( tr( "Chats" ), this );
  dock_widget->setObjectName( "GuiChatListDock" );
  mp_chatList = new GuiChatList( this );
  dock_widget->setWidget( mp_chatList );
  dock_widget->setAllowedAreas( Qt::AllDockWidgetAreas );
  addDockWidget( Qt::RightDockWidgetArea, dock_widget );
  mp_actViewChats = dock_widget->toggleViewAction();
  mp_actViewChats->setIcon( QIcon( ":/images/chat-list.png" ) );
  mp_actViewChats->setText( tr( "Show the chat list" ) );
  mp_actViewChats->setStatusTip( tr( "Show the list of the chats" ) );
  mp_actViewChats->setData( 99 );

  dock_widget = new QDockWidget( tr( "History" ), this );
  dock_widget->setObjectName( "GuiSavedChatListDock" );
  mp_savedChatList = new GuiSavedChatList( this );
  dock_widget->setWidget( mp_savedChatList );
  dock_widget->setAllowedAreas( Qt::AllDockWidgetAreas );
  addDockWidget( Qt::RightDockWidgetArea, dock_widget );
  mp_actViewSavedChats = dock_widget->toggleViewAction();
  mp_actViewSavedChats->setIcon( QIcon( ":/images/saved-chat-list.png" ) );
  mp_actViewSavedChats->setText( tr( "Show the saved chat list" ) );
  mp_actViewSavedChats->setStatusTip( tr( "Show the list of the saved chats" ) );
  mp_actViewSavedChats->setData( 99 );

  dock_widget = new QDockWidget( tr( "File Transfers" ), this );
  dock_widget->setObjectName( "GuiFileTransferDock" );
  mp_fileTransfer = new GuiTransferFile( this );
  dock_widget->setWidget( mp_fileTransfer );
  dock_widget->setAllowedAreas( Qt::AllDockWidgetAreas );
  addDockWidget( Qt::BottomDockWidgetArea, dock_widget );
  mp_actViewFileTransfer = dock_widget->toggleViewAction();
  mp_actViewFileTransfer->setIcon( QIcon( ":/images/file-transfer.png" ) );
  mp_actViewFileTransfer->setText( tr( "Show the file transfers" ) );
  mp_actViewFileTransfer->setStatusTip( tr( "Show the list of the file transfers" ) );
  mp_actViewFileTransfer->setData( 99 );
  dock_widget->hide();
}

void GuiMain::createStackedWidgets()
{
  mp_chat = new GuiChat( this );
  mp_stackedWidget->addWidget( mp_chat );
  mp_barChat = new QToolBar( tr( "Show the chat tool bar" ), this );
  addToolBar( Qt::BottomToolBarArea, mp_barChat );
  mp_barChat->setObjectName( "GuiChatToolBar" );
  mp_barChat->setIconSize( Settings::instance().mainBarIconSize() );
  mp_barChat->setAllowedAreas( Qt::AllToolBarAreas );
  mp_chat->setupToolBar( mp_barChat );

  mp_shareLocal = new GuiShareLocal( this );
  mp_stackedWidget->addWidget( mp_shareLocal );
  mp_barShareLocal= new QToolBar( tr( "Show the bar of local file sharing" ), this );
  addToolBar( Qt::BottomToolBarArea, mp_barShareLocal );
  mp_barShareLocal->setObjectName( "GuiShareLocalToolBar" );
  mp_barShareLocal->setIconSize( Settings::instance().mainBarIconSize() );
  mp_barShareLocal->setAllowedAreas( Qt::BottomToolBarArea | Qt::TopToolBarArea );
  mp_shareLocal->setupToolBar( mp_barShareLocal );

  mp_shareNetwork = new GuiShareNetwork( this );
  mp_stackedWidget->addWidget( mp_shareNetwork );
  mp_barShareNetwork = new QToolBar( tr( "Show the bar of network file sharing" ), this );
  addToolBar( Qt::BottomToolBarArea, mp_barShareNetwork );
  mp_barShareNetwork->setObjectName( "GuiShareNetworkToolBar" );
  mp_barShareNetwork->setIconSize( Settings::instance().mainBarIconSize() );
  mp_barShareNetwork->setAllowedAreas( Qt::BottomToolBarArea | Qt::TopToolBarArea );
  mp_shareNetwork->setupToolBar( mp_barShareNetwork );

  mp_logView = new GuiLog( this );
  mp_stackedWidget->addWidget( mp_logView );

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
}

QMenu* GuiMain::gameMenu( GameInterface* gi )
{
  if( m_mapGameMenu.contains( gi->name() ) )
    return m_mapGameMenu.value( gi->name() );

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

void GuiMain::checkUser( const User& u )
{
  if( !u.isValid() )
  {
    qWarning() << "Invalid user found in GuiMain::checkUser( const User& u )";
    return;
  }

#ifdef BEEBEEP_DEBUG
  qDebug() << "User" << u.path() << "has updated his info";
#endif
  mp_userList->setUser( u );
  mp_chat->updateUser( u );
  mp_groupList->updateUser( u );
  if( mp_stackedWidget->currentWidget() == mp_chat )
    mp_chat->reloadChat();
  checkViewActions();
}

void GuiMain::refreshUserList()
{
#ifdef BEEBEEP_DEBUG
  qDebug() << "Refresh users in GuiUserList";
#endif
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
    Settings::instance().setBeepOnNewMessageArrived( act->isChecked() );
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
    Settings::instance().setAutomaticFileName( act->isChecked() );
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
    Settings::instance().setFileTransferIsEnabled( act->isChecked() );
    break;
  case 13:
    Settings::instance().setShowMessagesGroupByUser( act->isChecked() );
    refresh_chat = true;
    break;
  case 14:
    Settings::instance().setStayOnTop( act->isChecked() );
    checkWindowFlagsAndShow();
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
        BeeApplication* bee_app = static_cast<BeeApplication*>( QApplication::instance() );
        bool ok = false;
        int away_timeout = QInputDialog::getInt( this, Settings::instance().programName(),
                              tr( "How many minutes of idle %1 can wait before changing status to away?" ).arg( Settings::instance().programName() ),
                              Settings::instance().userAwayTimeout(), 1, 30, 1, &ok );
        if( ok && away_timeout > 0 )
          Settings::instance().setUserAwayTimeout( away_timeout );

        bee_app->setIdleTimeout( Settings::instance().userAwayTimeout() );
      }
    }
    break;
  case 99:
    break;
  default:
    qWarning() << "GuiMain::settingsChanged(): error in setting id" << act->data().toInt();
  }

  if( refresh_users )
    refreshUserList();
  if( refresh_chat )
    mp_chat->reloadChat();
  if( settings_data_id > 0 && settings_data_id < 99 )
    Settings::instance().save();
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

bool GuiMain::showAlert()
{
  if( !isActiveWindow() || isMinimized() )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "BeeBEEP alert called";
#endif
    if( Settings::instance().beepOnNewMessageArrived() )
    {
#ifdef BEEBEEP_DEBUG
      qDebug() << "New message arrived in background: play BEEP sound";
#endif
      playBeep();
    }

    if( mp_trayIcon->isVisible() )
    {
      if( Settings::instance().raiseOnNewMessageArrived() )
        showFromTrayIcon();
      else
        mp_trayIcon->addUnreadMessage( 1 );
    }
    else
    {
      if( Settings::instance().raiseOnNewMessageArrived() && !Settings::instance().stayOnTop() )
        raiseOnTop();
      else
        QApplication::alert( this );
    }

    return true;
  }
  else
    return false;
}

void GuiMain::showChatMessage( VNumber chat_id, const ChatMessage& cm )
{
  bool show_alert = false;

  if( !cm.isSystem() && !cm.isFromLocalUser() )
    show_alert = showAlert();

  if( chat_id == mp_chat->chatId() && mp_chat == mp_stackedWidget->currentWidget() )
  {
    mp_chat->appendChatMessage( chat_id, cm );
    statusBar()->clearMessage();
    mp_userList->setUnreadMessages( chat_id, 0 );
  }
  else
  {
    Chat chat_hidden = ChatManager::instance().chat( chat_id );
    mp_userList->setUnreadMessages( chat_id, chat_hidden.unreadMessages() );

    if( show_alert )
    {
      if( Settings::instance().raiseOnNewMessageArrived() )
      {
        statusBar()->clearMessage();
        showChat( chat_id );
      }
      else
      {
        User u = UserManager::instance().userList().find( cm.userId() );
        if( u.isValid() )
          showMessage( tr( "New message from %1" ).arg( u.name() ), 0 );
        else
          showMessage( tr( "New message arrived" ), 0 );
      }
    }
  }

  mp_chatList->updateChat( chat_id );
  mp_groupList->updateChat( chat_id );
}

void GuiMain::searchUsers()
{
  if( !mp_core->isConnected() )
    return;

  GuiSearchUser gsu( this );
  gsu.loadSettings();
  gsu.show();
  gsu.setFixedSize( gsu.size() );
  if( gsu.exec() != QDialog::Accepted )
    return;

  showChat( ID_DEFAULT_CHAT );

  if( !gsu.addresses().isEmpty() )
  {
    foreach( QString s, gsu.addresses() )
      mp_core->addBroadcastAddress( QHostAddress( s ) );
    sendBroadcastMessage();
  }
}

void GuiMain::showWritingUser( const User& u )
{
  QString msg = tr( "%1 is writing..." ).arg( u.name() );
  showMessage( msg, Settings::instance().writingTimeout() );
}

void GuiMain::statusSelected()
{
  QAction* act = qobject_cast<QAction*>( sender() );
  if( !act )
    return;

  int user_status = act->data().toInt();

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
}

void GuiMain::changeStatusDescription()
{
  bool ok = false;
  QString status_description = QInputDialog::getText( this, Settings::instance().programName(),
                           tr( "Please insert the new status description" ), QLineEdit::Normal, Settings::instance().localUser().statusDescription(), &ok );
  if( !ok || status_description.isNull() )
    return;
  mp_core->setLocalUserStatusDescription( status_description );
}

void GuiMain::sendFileFromChat( VNumber chat_id, const QString& file_path )
{
  Chat c = ChatManager::instance().chat( chat_id );
  if( !c.isValid() )
    return;

  QString file_path_selected = checkFilePath( file_path );
  if( file_path_selected.isEmpty() )
    return;

  UserList chat_members = UserManager::instance().userList().fromUsersId( c.usersId() );
  foreach( User u, chat_members.toList() )
    sendFile( u, file_path_selected );
}

void GuiMain::sendFile( VNumber user_id )
{
  User u = UserManager::instance().userList().find( user_id );
  QString file_path_selected = checkFilePath( "" );
  if( file_path_selected.isEmpty() )
    return;
  sendFile( u, file_path_selected );
}

QString GuiMain::checkFilePath( const QString& file_path )
{
  QString file_path_selected = file_path;
  if( file_path.isEmpty() || !QFile::exists( file_path ) )
  {
    file_path_selected = QFileDialog::getOpenFileName( this, tr( "%1 - Select a file" ).arg( Settings::instance().programName() ),
                                                       Settings::instance().lastDirectorySelected() );
    if( file_path_selected.isEmpty() || file_path_selected.isNull() )
      return file_path_selected;

      Settings::instance().setLastDirectorySelectedFromFile( file_path_selected );
  }
  else
    file_path_selected = file_path;

  return file_path_selected;
}

bool GuiMain::sendFile( const User& u, const QString& file_path )
{
  User user_selected;

  if( !u.isValid() )
  {
    QStringList user_string_list = UserManager::instance().userList().toStringList( false, true );
    if( user_string_list.isEmpty() )
    {
      QMessageBox::information( this, Settings::instance().programName(), tr( "There is no user connected." ) );
      return false;
    }

    bool ok = false;
    QString user_path = QInputDialog::getItem( this, Settings::instance().programName(),
                                        tr( "Please select the user to whom you would like to send a file."),
                                        user_string_list, 0, false, &ok );
    if( !ok )
      return false;

    user_selected = UserManager::instance().findUserByPath( user_path );

    if( !user_selected.isValid() )
    {
      QMessageBox::warning( this, Settings::instance().programName(), tr( "User not found." ) );
      return false;
    }

    Chat c = ChatManager::instance().privateChatForUser( user_selected.id() );
    if( c.isValid() )
      showChat( c.id() );
  }
  else
    user_selected = u;

  return mp_core->sendFile( user_selected, file_path );
}

void GuiMain::sendFile( const QString& file_path )
{
  sendFile( User(), file_path );
}

bool GuiMain::askToDownloadFile( const User& u, const FileInfo& fi )
{
  if( !Settings::instance().fileTransferIsEnabled() )
  {
    QMessageBox::warning( this, Settings::instance().programName(), tr( "File transfer is disabled. You cannot download %1." ).arg( fi.name() ) );
    return false;
  }

  int msg_result = 1;

  if( Settings::instance().confirmOnDownloadFile() )
  {
    QString msg = tr( "Do you want to download %1 (%2) from %3?" ).arg( fi.name(), Bee::bytesToString( fi.size() ), u.name() );
    msg_result = QMessageBox::information( this, Settings::instance().programName(), msg, tr( "No" ), tr( "Yes" ), tr( "Yes, and don't ask anymore" ), 0, 0 );
  }

  if( msg_result == 2 )
    Settings::instance().setConfirmOnDownloadFile( false );

  if( msg_result > 0 )
  {
    // Accepted
    qDebug() << "You accept to download" << fi.name() << "from" << u.path();
    QFileInfo qfile_info( Settings::instance().downloadDirectory(), fi.name() );
    if( qfile_info.exists() )
    {
      QString file_name;
      if( Settings::instance().automaticFileName() )
      {
        file_name = Bee::uniqueFilePath( qfile_info.absoluteFilePath() );
        qDebug() << "File" << qfile_info.absoluteFilePath() << "exists. Save with" << file_name;
      }
      else
      {
        file_name = QFileDialog::getSaveFileName( this,
                            tr( "%1 already exists. Please select a new filename." ).arg( qfile_info.fileName() ),
                            qfile_info.absoluteFilePath() );
        if( file_name.isNull() || file_name.isEmpty() )
          return false;
      }
      qfile_info = QFileInfo( file_name );
    }
    FileInfo file_info = fi;
    file_info.setName( qfile_info.fileName() );
    file_info.setPath( qfile_info.absoluteFilePath() );
    file_info.setSuffix( qfile_info.suffix() );
    mp_core->downloadFile( u, file_info );
    return true;
  }
  else
  {
    qDebug() << "You refuse to download" << fi.name() << "from" << u.path();
    return false;
  }
}

void GuiMain::downloadFile( const User& u, const FileInfo& fi )
{
  if( !askToDownloadFile( u, fi ) )
    mp_core->refuseToDownloadFile( u, fi );
}

void GuiMain::downloadSharedFile( VNumber user_id, VNumber file_id )
{
  User u = UserManager::instance().userList().find( user_id );
  FileInfo file_info = FileShare::instance().networkFileInfo( user_id, file_id );

  if( u.isValid() && file_info.isValid() )
  {
    askToDownloadFile( u, file_info );
    return;
  }

  QString info_msg = tr( "File is not available for download." );
  if( !u.isConnected() )
    info_msg += QLatin1String( "\n" ) + tr( "%1 is not connected." ).arg( u.name() );
  info_msg += QLatin1String( "\n" ) + tr( "Please reload the list of shared files." );

  if( QMessageBox::information( this, Settings::instance().programName(), info_msg,
                              tr( "Reload file list" ), tr( "Cancel" ), QString::null, 1, 1 ) == 0 )
  {
    mp_shareNetwork->reloadList();
  }
}

void GuiMain::selectDownloadDirectory()
{
  QString download_directory_path = QFileDialog::getExistingDirectory( this,
                                                                       tr( "%1 - Select the download folder" )
                                                                       .arg( Settings::instance().programName() ),
                                                                       Settings::instance().downloadDirectory() );
  if( download_directory_path.isNull() )
    return;

  Settings::instance().setDownloadDirectory( download_directory_path );
}

void GuiMain::showTipOfTheDay()
{
  // Tip of the day is shown only in default chat
  showChat( ID_DEFAULT_CHAT );
  mp_core->showTipOfTheDay();
}

void GuiMain::showFactOfTheDay()
{
  // Fact of the day is shown only in default chat
  showChat( ID_DEFAULT_CHAT );
  mp_core->showFactOfTheDay();
}

void GuiMain::showCurrentChat()
{
  showChat( mp_chat->chatId() );
}

void GuiMain::showChat( VNumber chat_id )
{
  if( chat_id == ID_INVALID )
  {
    qWarning() << "Unable to show an invalid chat";
    return;
  }

  if( mp_stackedWidget->currentWidget() == mp_chat && mp_chat->chatId() == chat_id )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Chat" << chat_id << "is already shown... skip";
#endif
    return;
  }

  if( mp_chat->setChatId( chat_id ) )
  {
    mp_userList->setUnreadMessages( chat_id, 0 );
    mp_chatList->updateChat( chat_id );
    mp_groupList->updateChat( chat_id );
    raiseChatView();
    mp_chat->ensureLastMessageVisible();
  }
}

void GuiMain::changeVCard()
{
  GuiEditVCard gvc( this );
  gvc.setModal( true );
  gvc.setUser( Settings::instance().localUser() );
  gvc.show();
  gvc.setFixedSize( gvc.size() );
  if( gvc.exec() == QDialog::Accepted )
  {
    if( gvc.userColor() != Settings::instance().localUser().color() )
    {
#ifdef BEEBEEP_DEBUG
      qDebug() << "Local user color changed";
#endif
      User u = Settings::instance().localUser();
      u.setColor( gvc.userColor() );
      Settings::instance().setLocalUser( u );
    }

    if( gvc.vCard() == Settings::instance().localUser().vCard() )
    {
#ifdef BEEBEEP_DEBUG
      qDebug() << "Ok pressed but vCard is not changed";
#endif
      return;
    }

#ifdef BEEBEEP_DEBUG
    qDebug() << "vCard changed";
#endif
    mp_core->setLocalUserVCard( gvc.vCard() );
    refreshTitle( Settings::instance().localUser() );
    Settings::instance().save();
  }
}

void GuiMain::showUserMenu( VNumber user_id )
{
  User u = UserManager::instance().userList().find( user_id );
  if( !u.isValid() )
    return;

  GuiVCard* gvc = new GuiVCard( this );
  connect( gvc, SIGNAL( showChat( VNumber ) ), this, SLOT( showChat( VNumber ) ) );
  connect( gvc, SIGNAL( sendFile( VNumber ) ), this, SLOT( sendFile( VNumber ) ) );
  connect( gvc, SIGNAL( changeUserColor( VNumber ) ), this, SLOT( changeUserColor( VNumber) ) );
  gvc->setVCard( u, ChatManager::instance().privateChatForUser( user_id ).id() );

  if( dockWidgetArea( mp_dockUserList ) == Qt::RightDockWidgetArea )
  {
    // Ensure vCard visible
    QPoint pos = QCursor::pos();
    pos.setX( pos.x() - gvc->size().width() );
    gvc->move( pos );
  }
  else
    gvc->move( QCursor::pos() );
  gvc->show();
  gvc->setFixedSize( gvc->size() );
}

void GuiMain::updadePluginMenu()
{
  mp_menuPlugins->clear();
  mp_barPlugins->clear();
  QAction* act;

  act = mp_menuPlugins->addAction( QIcon( ":/images/plugin.png" ), tr( "Plugin Manager..." ), this, SLOT( showPluginManager() ) );
  act->setStatusTip( tr( "Open the plugin manager dialog and manage the installed plugins" ) );
  if( PluginManager::instance().count() <= 0 )
  {
    mp_barPlugins->addAction( act );
    mp_barPlugins->addSeparator();
  }

  /* Static Plugins */
  mp_menuPlugins->addSeparator();
  mp_actViewLog = mp_menuPlugins->addAction( QIcon( ":/images/log.png" ), tr( "Show the %1 log" ).arg( Settings::instance().programName() ), this, SLOT( raiseLogView() ) );
  mp_actViewLog->setStatusTip( tr( "Show the application log to see if an error occurred" ) );
  mp_barPlugins->addAction( mp_actViewLog );
  mp_actViewScreenShot = mp_menuPlugins->addAction( QIcon( ":/images/screenshot.png" ), tr( "Make a screenshot" ), this, SLOT( raiseScreenShotView() ) );
  mp_actViewScreenShot->setStatusTip( tr( "Show the utility to capture a screenshot" ) );
  mp_barPlugins->addAction( mp_actViewScreenShot );

  if( PluginManager::instance().count() <= 0 )
  {
    // mp_actViewLog and mp_actViewScreenshot must be created
    return;
  }

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
    mp_barPlugins->addSeparator();
    QMenu* game_menu;
    int game_widget_id;

    foreach( GameInterface* gi, PluginManager::instance().games() )
    {
      game_menu = gameMenu( gi );
      game_menu->setEnabled( gi->isEnabled() );
      gi->mainWindow()->setEnabled( gi->isEnabled() );
      mp_menuPlugins->addMenu( game_menu );

      act = mp_barPlugins->addAction( gi->icon(), tr( "Play %1" ).arg( gi->name() ), this, SLOT( raisePluginView() ) );
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

void GuiMain::sendBroadcastMessage()
{
  mp_core->sendBroadcastMessage();
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
  mp_trayIcon->setUnreadMessages( ChatManager::instance().unreadMessages() );
  mp_trayIcon->show();
  if( Settings::instance().showNotificationOnTray() )
    mp_trayIcon->showMessage( Settings::instance().programName(),
                            tr( "%1 will keep running in the background mode" )
                              .arg( Settings::instance().programName() ),
                            QSystemTrayIcon::Information, Settings::instance().trayMessageTimeout() );
  hide();
}

void GuiMain::showFromTrayIcon()
{
  showNormal(); // the window last state is minimized
  mp_trayIcon->hide();
}

void GuiMain::trayIconClicked( QSystemTrayIcon::ActivationReason )
{
  QTimer::singleShot( 0, this, SLOT( showFromTrayIcon() ) );
}

void GuiMain::trayMessageClicked()
{
  QTimer::singleShot( 0, this, SLOT( showFromTrayIcon() ) );
}

void GuiMain::addToShare( const QString& share_path )
{
  mp_core->addPathToShare( share_path, true );
}

void GuiMain::removeFromShare( const QString& share_path )
{
  mp_core->removePathFromShare( share_path );
}

void GuiMain::raiseChatView()
{
  setGameInPauseMode();
  mp_stackedWidget->setCurrentWidget( mp_chat );
  checkViewActions();
}

void GuiMain::raiseLocalShareView()
{
  setGameInPauseMode();
  mp_stackedWidget->setCurrentWidget( mp_shareLocal );
  checkViewActions();
}

void GuiMain::raiseNetworkShareView()
{
  setGameInPauseMode();
  mp_stackedWidget->setCurrentWidget( mp_shareNetwork );
  mp_shareNetwork->initShares();
  checkViewActions();
}

void GuiMain::raisePluginView()
{
  QAction* act = qobject_cast<QAction*>( sender() );
  if( !act )
    return;

  int widget_index = act->data().toInt();
  if( widget_index == mp_stackedWidget->currentIndex() )
    return;

  setGameInPauseMode();
  mp_stackedWidget->setCurrentIndex( widget_index );
  checkViewActions();
}

void GuiMain::raiseLogView()
{
  setGameInPauseMode();
  mp_stackedWidget->setCurrentWidget( mp_logView );
  checkViewActions();
}

void GuiMain::raiseScreenShotView()
{
  setGameInPauseMode();
  mp_stackedWidget->setCurrentWidget( mp_screenShot );
  checkViewActions();
}

void GuiMain::setGameInPauseMode()
{
  PluginManager::instance().setGamePauseOn();
}

void GuiMain::openUrl( const QUrl& file_url )
{
  QString file_path = file_url.toLocalFile();
  if( !file_path.isEmpty() )
  {
    if( QMessageBox::question( this, Settings::instance().programName(),
                               tr( "Do you really want to open the file %1?" ).arg( file_path ),
                               tr( "Yes" ), tr( "No" ), QString(), 1, 1 ) != 0 )
    return;
  }

  qDebug() << "Open url:" << file_url.toString();
  if( !QDesktopServices::openUrl( file_url ) )
    QMessageBox::information( this, Settings::instance().programName(),
                              tr( "Unable to open %1" ).arg( file_path.isEmpty() ? file_url.toString() : file_path ), tr( "Ok" ) );
}

void GuiMain::selectBeepFile()
{
  QString file_path = QFileDialog::getOpenFileName( this, Settings::instance().programName(), Settings::instance().beepFilePath(), tr( "Sound files (*.wav)" ) );
  if( file_path.isNull() || file_path.isEmpty() )
    return;

  Settings::instance().setBeepFilePath( file_path );
  qDebug() << "New sound file selected:" << file_path;
  if( mp_sound )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Delete previous sound object";
#endif
    mp_sound->deleteLater();
    mp_sound = 0;
  }

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
  if( !QSound::isAvailable() )
  {
    qWarning() << "QSound is not available";
    QMessageBox::warning( this, Settings::instance().programName(), tr( "Sound module is not working. The default BEEP will be used." ) );
    return;
  }

  if( !QFile::exists( Settings::instance().beepFilePath() ) )
  {
    QString warn_text = QString( "%1\n%2. %3." ).arg( Settings::instance().beepFilePath() )
                                                  .arg( tr( "Sound file not found" ) )
                                                  .arg( tr( "The default BEEP will be used" ) );
    QMessageBox::warning( this, Settings::instance().programName(), warn_text );
    return;
  }

  playBeep();
}

void GuiMain::playBeep()
{
  if( !mp_sound )
  {
    qDebug() << "Create sound object from" << Settings::instance().beepFilePath();
    mp_sound = new QSound( Settings::instance().beepFilePath(), this );
  }

  if( QFile::exists( Settings::instance().beepFilePath() ) )
    mp_sound->play();
  else
    QApplication::beep();
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

  GuiCreateGroup gcg( this );
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

void GuiMain::createGroupChat()
{
  GuiCreateGroup gcg( this );
  gcg.loadData( false );
  gcg.setModal( true );
  gcg.show();
  gcg.setFixedSize( gcg.size() );
  if( gcg.exec() == QDialog::Accepted )
    mp_core->createGroupChat( gcg.selectedName(), gcg.selectedUsersId(), "", true );
}

void GuiMain::addUserToGroupChat()
{
  Chat group_chat_tmp = ChatManager::instance().chat( mp_chat->chatId() );
  if( !group_chat_tmp.isGroup() )
  {
    QMessageBox::information( this, Settings::instance().programName(), tr( "Unable to add users in this chat. Please select a group one." ) );
    return;
  }

  GuiCreateGroup gcg( this );
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
  if( isMinimized() )
    showNormal();

#ifdef Q_OS_WIN
  SetWindowPos( (HWND)winId(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
  SetWindowPos( (HWND)winId(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
  SetActiveWindow( (HWND)winId() );
#else
  raise();
  qApp->setActiveWindow( this );
#endif

}

void GuiMain::checkAutoStartOnBoot( bool add_service )
{
  if( add_service )
  {
    Settings::instance().addStartOnSystemBoot();
    QMessageBox::information( this, Settings::instance().programName(), tr( "Now %1 will start on windows boot." ).arg( Settings::instance().programName() ) );
  }
  else
  {
    Settings::instance().removeStartOnSystemBoot();
    QMessageBox::information( this, Settings::instance().programName(), tr( "%1 will not start on windows boot." ).arg( Settings::instance().programName() ) );
  }
}

void GuiMain::loadSession()
{
  QTimer::singleShot( 200, mp_core, SLOT( buildSavedChatList() ) );
  mp_shareLocal->updatePaths();
  QTimer::singleShot( 2000, mp_core, SLOT( buildLocalShareList() ) );
  mp_core->loadGroups();
}

void GuiMain::saveSession()
{
  mp_core->saveGroups();
  SaveChatList scl;
  scl.save();
}

void GuiMain::showSavedChatSelected( const QString& chat_name )
{
  if( chat_name.isEmpty() )
    return;

  mp_savedChat->showSavedChat( chat_name );

  setGameInPauseMode();
  mp_stackedWidget->setCurrentWidget( mp_savedChat );
  checkViewActions();
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
}

void GuiMain::linkSavedChat( const QString& chat_name )
{
  // fixme
  bool ok = false;
  QStringList chat_names_string_list = ChatManager::instance().chatNamesToStringList( true );

  // remove chat_name == chat_name_selected case
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
  User u = UserManager::instance().userList().find( user_id );
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
  }
}

void GuiMain::clearChat( VNumber chat_id )
{
  Chat c = ChatManager::instance().chat( chat_id );
  if( !c.isValid() )
    return;
  QString chat_name = c.isDefault() ? QObject::tr( "All Lan Users" ).toLower() : c.name();
  if( c.isEmpty() )
  {
    QMessageBox::information( this, Settings::instance().programName(), tr( "Chat with %1 is empty." ).arg( chat_name ) );
    return;
  }

  QString question_txt = tr( "Do you really want to clear messages with %1?" ).arg( chat_name );
  QString button_2_text;
  if( ChatManager::instance().chatHasSavedText( c.name() ) )
    button_2_text = QString( "  " ) + tr( "Yes and delete history" ) + QString( "  " );

  switch( QMessageBox::warning( this, Settings::instance().programName(), question_txt, tr( "Yes" ), tr( "No" ), button_2_text, 1, 1 ) )
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
  if( mp_stackedWidget->currentWidget() == mp_chat && mp_chat->chatId() == chat_id )
  {
    if( chat_id == ID_DEFAULT_CHAT )
      mp_chat->reloadChat();
    else
      showChat( ID_DEFAULT_CHAT );
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
    Chat c = ChatManager::instance().chat( chat_id );
    if( c.isValid() )
      mp_chat->reloadChat();
    else
      showChat( ID_DEFAULT_CHAT );
  }
}

void GuiMain::leaveGroupChat( VNumber chat_id )
{
  Chat c = ChatManager::instance().chat( chat_id );
  if( !c.isValid() )
    return;
  if( !c.hasUser( ID_LOCAL_USER ) )
    return;

  Group g = UserManager::instance().findGroupByPrivateId( c.privateId() );
  if( g.isValid() )
  {
    if( QMessageBox::warning( this, Settings::instance().programName(),
                              tr( "%1 is a your group. You can not leave the chat." ).arg( g.name() ),
                              tr( "Delete this group" ), tr( "Cancel" ), QString(), 1, 1 ) == 1 )
        return;

    mp_core->removeGroup( g.id() );
  }

  if( mp_core->removeUserFromChat( Settings::instance().localUser(), chat_id ) )
  {
    mp_chat->updateUser( Settings::instance().localUser() );
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
    if( QMessageBox::question( this, Settings::instance().programName(),
                               tr( "Do you really want to delete group '%1'?" ).arg( g.name() ),
                               tr( "Yes" ), tr( "No" ), QString::null, 1, 1 ) == 0 )
    {
      mp_core->removeGroup( group_id );
    }
  }
}

void GuiMain::removeChat( VNumber chat_id )
{
  if( mp_core->removeChat( chat_id ) )
  {
    mp_chatList->reloadChatList();
    if( mp_chat->chatId() == chat_id )
    {
      if( mp_stackedWidget->currentWidget() == mp_chat )
        showChat( ID_DEFAULT_CHAT );
      else
        mp_chat->setChatId( ID_DEFAULT_CHAT );
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
    mp_core->createGroupChat( g, true );

  c = ChatManager::instance().findGroupChatByPrivateId( g.privateId() );
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
  gl.show();
  gl.setFixedSize( gl.size() );
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

