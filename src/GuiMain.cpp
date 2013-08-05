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
// Author: Marco Mastroddi (marco.mastroddi(AT)gmail.com)
//
// $Id$
//
//////////////////////////////////////////////////////////////////////

#include "Core.h"
#include "BeeUtils.h"
#include "ChatManager.h"
#include "EmoticonManager.h"
#include "FileShare.h"
#include "GuiChat.h"
#include "GuiChatList.h"
#include "GuiEditVCard.h"
#include "GuiNetwork.h"
#include "GuiNetworkLogin.h"
#include "GuiPluginManager.h"
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
#include "Settings.h"
#include "UserManager.h"


GuiMain::GuiMain( QWidget *parent )
 : QMainWindow( parent )
{
  setObjectName( "GuiMainWindow" );
  mp_core = new Core( this );

  setWindowIcon( QIcon( ":/images/beebeep.png") );

  mp_stackedWidget = new QStackedWidget( this );
  createStackedWidgets();
  setCentralWidget( mp_stackedWidget );

  mp_barMain = addToolBar( tr( "Show the ToolBar" ) );
  mp_barMain->setObjectName( "GuiMainToolBar" );
  mp_barMain->setIconSize( Settings::instance().mainBarIconSize() );

  mp_trayIcon = new GuiSystemTray( this );

  createActions();
  createDockWindows();
  createMenus();
  createToolAndMenuBars();
  createPluginWindows();
  createStatusBar();

  connect( mp_core, SIGNAL( chatMessage( VNumber, const ChatMessage& ) ), this, SLOT( showChatMessage( VNumber, const ChatMessage& ) ) );
  connect( mp_core, SIGNAL( fileDownloadRequest( const User&, const FileInfo& ) ), this, SLOT( downloadFile( const User&, const FileInfo& ) ) );
  connect( mp_core, SIGNAL( userChanged( const User& ) ), this, SLOT( checkUser( const User& ) ) );
  connect( mp_core, SIGNAL( userIsWriting( const User& ) ), this, SLOT( showWritingUser( const User& ) ) );
  connect( mp_core, SIGNAL( xmppUserSubscriptionRequest( const QString&, const QString& ) ), this, SLOT( showUserSubscriptionRequest( const QString&, const QString& ) ) );
  connect( mp_core, SIGNAL( serviceConnected( const QString& ) ), this, SLOT( serviceConnected( const QString& ) ) );
  connect( mp_core, SIGNAL( serviceDisconnected( const QString& ) ), this, SLOT( serviceDisconnected( const QString& ) ) );
  connect( mp_core, SIGNAL( fileTransferProgress( VNumber, const User&, const FileInfo&, FileSizeType ) ), mp_fileTransfer, SLOT( setProgress( VNumber, const User&, const FileInfo&, FileSizeType ) ) );
  connect( mp_core, SIGNAL( fileTransferMessage( VNumber, const User&, const FileInfo&, const QString& ) ), mp_fileTransfer, SLOT( setMessage( VNumber, const User&, const FileInfo&, const QString& ) ) );
  connect( mp_core, SIGNAL( fileShareAvailable( const User& ) ), mp_shareNetwork, SLOT( loadShares( const User& ) ) );
  connect( mp_fileTransfer, SIGNAL( transferCancelled( VNumber ) ), mp_core, SLOT( cancelFileTransfer( VNumber ) ) );
  connect( mp_fileTransfer, SIGNAL( stringToShow( const QString&, int ) ), statusBar(), SLOT( showMessage( const QString&, int ) ) );
  connect( mp_fileTransfer, SIGNAL( fileTransferProgress( VNumber, VNumber, const QString& ) ), mp_shareNetwork, SLOT( showMessage( VNumber, VNumber, const QString& ) ) );
  connect( mp_fileTransfer, SIGNAL( fileTransferCompleted( VNumber, VNumber, const QString& ) ), mp_shareNetwork, SLOT( setFileTransferCompleted( VNumber, VNumber, const QString& ) ) );
  connect( mp_fileTransfer, SIGNAL( openFileCompleted( const QUrl& ) ), this, SLOT( openUrl( const QUrl& ) ) );
  connect( mp_defaultChat, SIGNAL( newMessage( VNumber, const QString& ) ), this, SLOT( sendMessage( VNumber, const QString& ) ) );
  connect( mp_defaultChat, SIGNAL( writing( VNumber ) ), mp_core, SLOT( sendWritingMessage( VNumber ) ) );
  connect( mp_defaultChat, SIGNAL( nextChat() ), this, SLOT( showNextChat() ) );
  connect( mp_defaultChat, SIGNAL( openUrl( const QUrl& ) ), this, SLOT( openUrl( const QUrl& ) ) );

  connect( mp_shareLocal, SIGNAL( sharePathAdded( const QString& ) ), this, SLOT( addToShare( const QString& ) ) );
  connect( mp_shareLocal, SIGNAL( sharePathRemoved( const QString& ) ), this, SLOT( removeFromShare( const QString& ) ) );

  connect( mp_shareNetwork, SIGNAL( fileShareListRequested() ), mp_core, SLOT( sendFileShareRequestToAll() ) );
  connect( mp_shareNetwork, SIGNAL( downloadSharedFile( VNumber, VNumber ) ), this, SLOT( downloadSharedFile( VNumber, VNumber ) ) );
  connect( mp_shareNetwork, SIGNAL( openFileCompleted( const QUrl& ) ), this, SLOT( openUrl( const QUrl& ) ) );

  connect( mp_userList, SIGNAL( chatSelected( VNumber ) ), this, SLOT( showChat( VNumber ) ) );
  connect( mp_userList, SIGNAL( menuToShow( VNumber ) ), this, SLOT( showUserMenu( VNumber ) ) );

  connect( mp_chatList, SIGNAL( chatSelected( VNumber ) ), this, SLOT( showChat( VNumber ) ) );

  connect( mp_trayIcon, SIGNAL( activated( QSystemTrayIcon::ActivationReason ) ), this, SLOT( trayIconClicked( QSystemTrayIcon::ActivationReason ) ) );
  connect( mp_trayIcon, SIGNAL( messageClicked() ), this, SLOT( trayMessageClicked() ) );

  initGuiItems();
  raiseChatView();
}

void GuiMain::checkWindowFlagsAndShow()
{
  if( Settings::instance().stayOnTop() )
  {
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
  }
  else
  {
    setWindowFlags(windowFlags() & ~Qt::WindowStaysOnTopHint);
  }

  show();
}

void GuiMain::refreshTitle( const User& )
{
  QString window_title;

  window_title = QString( "%1 - %2 (%3)" ).arg( Settings::instance().programName(),
                     Settings::instance().localUser().name(),
                     mp_core->isConnected( true ) ?
                     Bee::userStatusToString( Settings::instance().localUser().status() ) : tr( "offline" ) );

#if 0
  if( !mp_core->isConnected( true ) )
  {
    window_title = QString( "%1 - %2 (%3)" ).arg( Settings::instance().programName(),
                     Settings::instance().localUser().name(), tr( "offline" ) );
  }
  else
  {
    Chat c = ChatManager::instance().privateChatForUser( u.id() );
    if( c.id() == mp_defaultChat->chatId() && !isVisible() )
    {
      window_title = QString( "%1%2 (%3) - %4" ).arg(
                     c.unreadMessages() > 0 ? QString( "(%1) " ).arg( c.unreadMessages() ) : "",
                     u.name(),
                     Bee::userStatusToString( u.status() ),
                     Settings::instance().programName() );
    }
    else
      window_title = QString( "%1 - %2 (%3)" ).arg(
                       Settings::instance().programName(),
                       Settings::instance().localUser().name(),
                       Bee::userStatusToString( Settings::instance().localUser().status() ) );
  }
#endif

  setWindowTitle( window_title );
}

void GuiMain::forceExit()
{
  if( mp_core->isConnected( true ) )
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
  if( mp_core->isConnected( true ) )
  {
    if( Settings::instance().minimizeInTray() && QSystemTrayIcon::isSystemTrayAvailable() )
    {
      QTimer::singleShot( 0, this, SLOT( hideToTrayIcon() ) );
      e->ignore();
      return;
    }

    if( QMessageBox::question( this, Settings::instance().programName(), tr( "Do you really want to quit %1?" ).arg( Settings::instance().programName() ),
                             QMessageBox::Yes | QMessageBox::No, QMessageBox::No ) == QMessageBox::No )
    {
      e->ignore();
      return;
    }

    mp_core->stop();
  }

#ifndef Q_OS_SYMBIAN
  if( isVisible() )
  {
    Settings::instance().setGuiGeometry( saveGeometry() );
    Settings::instance().setGuiState( saveState() );
  }
#endif

  e->accept();
}

void GuiMain::showNextChat()
{
  qDebug() << "Show next chat in list with unread messages";
  if( !mp_userList->nextUserWithUnreadMessages() )
    statusBar()->showMessage( tr( "No new message available" ) );
}

void GuiMain::startStopCore()
{
  if( mp_core->isConnected( false ) )
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

  bool ok = false;
  QString pwd = QInputDialog::getText( this,
                                       Settings::instance().programName(),
                                       tr( "Please insert the chat password (or just press Enter)"),
                                       QLineEdit::Password,
                                       Settings::instance().defaultPassword(),
                                       &ok );
  if( !ok )
    return;
  Settings::instance().setPassword( pwd );
  mp_core->start();
  initGuiItems();
}

void GuiMain::stopCore()
{
  mp_core->stop();
  initGuiItems();
}

void GuiMain::initGuiItems()
{
  bool enable = mp_core->isConnected( false );
  bool enable_verbose = mp_core->isConnected( true );

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

  mp_actSendFile->setEnabled( enable_verbose );
  mp_actSearch->setEnabled( enable_verbose );
  mp_userList->setDefaultChatConnected( enable );

  updateStatusIcon();
  mp_shareLocal->loadSettings();

  refreshTitle( Settings::instance().localUser() );
}

void GuiMain::showAbout()
{
  QMessageBox::about( this, Settings::instance().programName(),
                      QString( "<b>%1</b> - %2<br /><br />%3 %4<br />%5<br />%6<br />%7" )
                      .arg( Settings::instance().programName() )
                      .arg( tr( "Secure Network Chat" ) )
                      .arg( tr( "Version" ) )
                      .arg( Settings::instance().version( true ) )
                      .arg( tr( "developed by Marco Mastroddi" ) )
                      .arg( tr( "e-mail: marco.mastroddi@gmail.com") )
                      .arg( PluginManager::instance().services().count() > 0 ? QString( "<br />XMPP: QXmpp 0.3.91 (Manjeet Dahiya)" ) : "" )
                      );

}

void GuiMain::showLicense()
{
  QString license_txt = tr( "BeeBEEP is free software: you can redistribute it and/or modify<br>"
  "it under the terms of the GNU General Public License as published<br>"
  "by the Free Software Foundation, either version 3 of the License<br>"
  "or (at your option) any later version.<br><br>"
  "BeeBEEP is distributed in the hope that it will be useful,<br>"
  "but WITHOUT ANY WARRANTY; without even the implied warranty<br>"
  "of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.<br>"
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

  mp_actSaveChat = new QAction( QIcon( ":/images/save-as.png" ), tr( "&Save chat..." ), this );
  mp_actSaveChat->setShortcuts( QKeySequence::Quit );
  mp_actSaveChat->setStatusTip( tr( "Save the messages of the current chat to a file" ) );
  connect( mp_actSaveChat, SIGNAL( triggered() ), this, SLOT( saveChat() ) );

  mp_actQuit = new QAction( QIcon( ":/images/quit.png" ), tr( "&Quit" ), this );
  mp_actQuit->setShortcuts( QKeySequence::Quit );
  mp_actQuit->setStatusTip( tr( "Close the chat and quit %1" ).arg( Settings::instance().programName() ) );
  connect( mp_actQuit, SIGNAL( triggered() ), this, SLOT( forceExit() ) );

  mp_actVCard = new QAction( QIcon( ":/images/profile.png"), tr( "Profile..." ), this );
  mp_actVCard->setStatusTip( tr( "Change your profile data" ) );
  connect( mp_actVCard, SIGNAL( triggered() ), this, SLOT( changeVCard() ) );

  mp_actFont = new QAction( QIcon( ":/images/font.png"), tr( "Chat font style..." ), this );
  mp_actFont->setStatusTip( tr( "Select your favourite chat font style" ) );
  connect( mp_actFont, SIGNAL( triggered() ), this, SLOT( selectFont() ) );

  mp_actFontColor = new QAction( QIcon( ":/images/font-color.png"), tr( "My message font color..." ), this );
  mp_actFontColor->setStatusTip( tr( "Select your favourite font color for the chat messages" ) );
  connect( mp_actFontColor, SIGNAL( triggered() ), this, SLOT( selectFontColor() ) );

  mp_actSendFile = new QAction( QIcon( ":/images/send-file.png"), tr( "Send a file..." ), this );
  mp_actSendFile->setStatusTip( tr( "Send a file to a user" ) );
  connect( mp_actSendFile, SIGNAL( triggered() ), this, SLOT( sendFile() ) );

  mp_actToolBar = mp_barMain->toggleViewAction();
  mp_actToolBar->setStatusTip( tr( "Show the main tool bar with settings and emoticons" ) );
  mp_actToolBar->setData( 99 );

  mp_actAbout = new QAction( QIcon( ":/images/info.png" ), tr( "&About %1..." ).arg( Settings::instance().programName() ), this );
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

  mp_menuAccounts = new QMenu( tr( "Network account" ), mp_menuMain );
  mp_menuAccounts->setIcon( QIcon( ":/images/network-account.png" ) );

  if( PluginManager::instance().services().size() > 0 )
  {
    mp_menuMain->addSeparator();
    QList<ServiceInterface*>::const_iterator it = PluginManager::instance().services().begin();
    while( it != PluginManager::instance().services().end() )
    {
      act = mp_menuAccounts->addAction( (*it)->icon(), (*it)->name(), this, SLOT( showNetworkAccount() ) );
      act->setStatusTip( tr( "Show the %1 login" ).arg( (*it)->name() ) );
      act->setData( (*it)->name() );
      ++it;
    }

    mp_menuMain->addMenu( mp_menuAccounts );

    act = mp_menuMain->addAction( QIcon( ":/images/network-settings.png" ), tr( "Network settings..."), this, SLOT( showNetworkManager() ) );
    act->setStatusTip( tr( "Show the network settings dialog" ) );
    updateAccountMenu();
  }

  mp_menuMain->addSeparator();
  act = mp_menuMain->addAction( QIcon( ":/images/download-folder.png" ), tr( "Download folder..."), this, SLOT( selectDownloadDirectory() ) );
  act->setStatusTip( tr( "Select the download folder" ) );
  mp_menuMain->addSeparator();
  mp_menuMain->addAction( mp_actQuit );

  /* Settings Menu */
  mp_menuSettings = new QMenu( tr( "&Settings" ), this );
  mp_menuSettings->addAction( mp_actToolBar );
  mp_menuSettings->addSeparator();

  act = mp_menuSettings->addAction( tr( "Enable the compact mode in chat window" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled the sender's nickname and his message are in the same line" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().chatCompact() );
  act->setData( 1 );

  act = mp_menuSettings->addAction( tr( "Add a blank line between the messages" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled the messages in the chat window are separated by a blank line" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().chatAddNewLineToMessage() );
  act->setData( 2 );

  act = mp_menuSettings->addAction( tr( "Show the messages' timestamp" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled the message shows its timestamp in the chat window" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().chatShowMessageTimestamp() );
  act->setData( 3 );

  act = mp_menuSettings->addAction( tr( "Show the user's colors" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled the user's nickname in chat and in list is colored" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().showUserColor() );
  act->setData( 5 );

  act = mp_menuSettings->addAction( tr( "Show only the online users" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled only the online users are shown in the list" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().showOnlyOnlineUsers() );
  act->setData( 6 );

  act = mp_menuSettings->addAction( tr( "Use HTML tags" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled HTML tags are not removed from the message" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().chatUseHtmlTags() );
  act->setData( 8 );

  act = mp_menuSettings->addAction( tr( "Use clickable links" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled the links in the message are recognized and made clickable" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().chatUseClickableLinks() );
  act->setData( 9 );

  act = mp_menuSettings->addAction( tr( "Show emoticons" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled the emoticons will be recognized and showed as images" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().showEmoticons() );
  act->setData( 10 );

  act = mp_menuSettings->addAction( tr( "Show messages grouped by user" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled the messages will be showed grouped by user" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().showMessagesGroupByUser() );
  act->setData( 13 );

  mp_menuSettings->addSeparator();

  act = mp_menuSettings->addAction( tr( "Beep on new message arrived" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled when a new message is arrived a sound is emitted" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().beepOnNewMessageArrived() );
  act->setData( 4 );

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
  }

  act = mp_menuSettings->addAction( tr( "Stay on top" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled %1 stays on top of the other windows" ).arg( Settings::instance().programName() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().stayOnTop() );
  act->setData( 14 );

  act = mp_menuSettings->addAction( tr( "Enable file sharing" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled you can share files with the other users" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().fileShare() );
  act->setData( 12 );

  /* Emoticons Menu for ToolBar */
  mp_menuEmoticons = new QMenu( tr( "Emoticons" ), this );
  mp_menuEmoticons->setStatusTip( tr( "Add your preferred emoticon to the message" ) );
  mp_menuEmoticons->setIcon( QIcon( ":/images/emoticon.png" ) );

  QList<Emoticon> emoticon_list = EmoticonManager::instance().emoticons( true );
  QList<Emoticon>::const_iterator it = emoticon_list.begin();
  while( it != emoticon_list.end() )
  {
    act = mp_menuEmoticons->addAction( QIcon( (*it).pixmap() ), (*it).name(), this, SLOT( emoticonSelected() ) );
    act->setData( (*it).textToMatch() );
    act->setStatusTip( QString( "Insert [%1] emoticon %2" ).arg( (*it).name(), (*it).textToMatch() ) );
    act->setIconVisibleInMenu( true );
    ++it;
  }

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
  mp_menuView->addAction( mp_actViewUsers );
  mp_menuView->addAction( mp_actViewChats );
  mp_menuView->addAction( mp_actViewFileTransfer );
  mp_menuView->addSeparator();
  mp_actViewDefaultChat = mp_menuView->addAction( QIcon( ":/images/chat-view.png" ), tr( "Show the chat" ), this, SLOT( raiseChatView() ) );
  mp_actViewDefaultChat->setStatusTip( tr( "Show the chat view" ) );
  mp_actViewShareLocal = mp_menuView->addAction( QIcon( ":/images/upload.png" ), tr( "Show my shared files" ), this, SLOT( raiseLocalShareView() ) );
  mp_actViewShareLocal->setStatusTip( tr( "Show the list of the files which I have shared" ) );
  mp_actViewShareNetwork = mp_menuView->addAction( QIcon( ":/images/download.png" ), tr( "Show the network shared files" ), this, SLOT( raiseNetworkShareView() ) );
  mp_actViewShareNetwork->setStatusTip( tr( "Show the list of the network shared files" ) );

  /* Help Menu */
  mp_menuInfo = new QMenu( tr("&?" ), this );
  act = mp_menuInfo->addAction( QIcon( ":/images/tip.png" ), tr( "Tips of the day" ), this, SLOT( showTipOfTheDay() ) );
  act->setStatusTip( tr( "Show me the tip of the day" ) );
  mp_menuInfo->addSeparator();
  mp_menuInfo->addAction( mp_actAbout );
  act = mp_menuInfo->addAction( QIcon( ":/images/license.png" ), tr( "Show %1's license..." ).arg( Settings::instance().programName() ), this, SLOT( showLicense() ) );
  act->setStatusTip( tr( "Show the informations about %1's license" ).arg( Settings::instance().programName() ) );
  act = mp_menuInfo->addAction( QIcon( ":/images/qt.png" ), tr( "About &Qt..." ), qApp, SLOT( aboutQt() ) );
  act->setStatusTip( tr( "Show the informations about Qt library" ) );

  /* Plugins Menu */
  mp_menuPlugins = new QMenu( tr( "Plugins" ), this );
  updadePluginMenu();

}

void GuiMain::createToolAndMenuBars()
{
  menuBar()->addMenu( mp_menuMain );
  menuBar()->addMenu( mp_menuSettings );
  menuBar()->addMenu( mp_menuView );
  menuBar()->addMenu( mp_menuPlugins );
  menuBar()->addMenu( mp_menuInfo );

  mp_barMain->addAction( mp_menuStatus->menuAction() );
  mp_barMain->addSeparator();
  mp_barMain->addAction( mp_actSendFile );
  mp_barMain->addAction( mp_actSaveChat );
  mp_barMain->addSeparator();
  mp_barMain->addAction( mp_actFont );
  mp_barMain->addAction( mp_actFontColor );
  mp_barMain->addAction( mp_menuEmoticons->menuAction() );
  mp_barMain->addSeparator();
  mp_barMain->addAction( mp_actViewUsers );
  mp_barMain->addAction( mp_actViewChats );
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
  addDockWidget( Qt::RightDockWidgetArea, mp_dockUserList );
  mp_actViewUsers = mp_dockUserList->toggleViewAction();
  mp_actViewUsers->setIcon( QIcon( ":/images/user-list.png" ) );
  mp_actViewUsers->setText( tr( "Show online users and active chats" ) );
  mp_actViewUsers->setStatusTip( tr( "Show the list of the connected users and the active chats" ) );
  mp_actViewUsers->setData( 99 );

  QDockWidget* dock_widget = new QDockWidget( tr( "Chats" ), this );
  dock_widget->setObjectName( "GuiChatListDock" );
  mp_chatList = new GuiChatList( this );
  dock_widget->setWidget( mp_chatList );
  addDockWidget( Qt::RightDockWidgetArea, dock_widget );
  mp_actViewChats = dock_widget->toggleViewAction();
  mp_actViewChats->setIcon( QIcon( ":/images/chat-list.png" ) );
  mp_actViewChats->setText( tr( "Show the chat list" ) );
  mp_actViewChats->setStatusTip( tr( "Show the list of the chats" ) );
  mp_actViewChats->setData( 99 );
  dock_widget->hide();

  dock_widget = new QDockWidget( tr( "File Transfers" ), this );
  dock_widget->setObjectName( "GuiFileTransferDock" );
  mp_fileTransfer = new GuiTransferFile( this );
  dock_widget->setWidget( mp_fileTransfer );
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
  mp_defaultChat = new GuiChat( this );
  mp_stackedWidget->addWidget( mp_defaultChat );

  mp_shareLocal = new GuiShareLocal( this );
  mp_stackedWidget->addWidget( mp_shareLocal );

  mp_shareNetwork = new GuiShareNetwork( this );
  mp_stackedWidget->addWidget( mp_shareNetwork );
}

void GuiMain::createPluginWindows()
{
  QAction* act;
  int plugin_index;

  if( PluginManager::instance().games().size() > 0 )
    mp_menuView->addSeparator();

  foreach( GameInterface* gi, PluginManager::instance().games() )
  {
    act = mp_menuView->addAction( gi->icon(), gi->name(), this, SLOT( raisePluginView() ) );
    act->setStatusTip( gi->help() );
    plugin_index = mp_stackedWidget->addWidget( gi->mainWindow() );
    act->setData( plugin_index );
  }
}

void GuiMain::checkUser( const User& u )
{
  if( !u.isValid() )
  {
    qDebug() << "Invalid user found in GuiMain::checkUser( const User& u )";
    return;
  }

  refreshTitle( u );

  qDebug() << "User" << u.path() << "has updated his info. Check it";
  mp_userList->setUser( u );
  mp_defaultChat->updateUser( u );
}

void GuiMain::emoticonSelected()
{
  QAction* act = qobject_cast<QAction*>( sender() );
  if( act )
    mp_defaultChat->addToMyMessage( QString( "%1" ).arg( act->data().toString() ) );
}

void GuiMain::refreshUserList()
{
  qDebug() << "Refresh users";
  mp_userList->updateUsers( mp_core->isConnected( false ) );
}

void GuiMain::refreshChat()
{
  qDebug() << "Refresh chat";
  if( !mp_defaultChat->setChatId( mp_defaultChat->chatId() ) )
    qWarning() << "Chat" << mp_defaultChat->chatId() << "not found. Unable to refresh it";
}

void GuiMain::selectFont()
{
  bool ok = false;
  QFont f = QFontDialog::getFont( &ok, Settings::instance().chatFont(), this );
  if( ok )
  {
    Settings::instance().setChatFont( f );
    mp_defaultChat->setChatFont( f );
  }
}

void GuiMain::selectFontColor()
{
  QColor c = QColorDialog::getColor( QColor( Settings::instance().chatFontColor() ), this );
  if( c.isValid() )
  {
    Settings::instance().setChatFontColor( c.name() );
    mp_defaultChat->setChatFontColor( c.name() );
  }
}

void GuiMain::changeUserColor( VNumber user_id )
{
  User u = UserManager::instance().userList().find( user_id );
  if( !u.isValid() )
  {
    QMessageBox::warning( this, Settings::instance().programName(), tr( "User not found." ) );
    return;
  }
  QColor c = QColorDialog::getColor( QColor( Settings::instance().chatFontColor() ), this );
  if( c.isValid() )
  {
    if( mp_core->setUserColor( user_id, c.name() ) )
      refreshChat();
  }
}

void GuiMain::settingsChanged()
{
  QAction* act = qobject_cast<QAction*>( sender() );
  if( !act )
    return;

  bool refresh_users = false;
  bool refresh_chat = false;

  switch( act->data().toInt() )
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
    Settings::instance().setFileShare( act->isChecked() );
    break;
  case 13:
    Settings::instance().setShowMessagesGroupByUser( act->isChecked() );
    refresh_chat = true;
    break;
  case 14:
    Settings::instance().setStayOnTop( act->isChecked() );
    checkWindowFlagsAndShow();
    break;
  case 99:
    break;
  default:
    qWarning() << "GuiMain::settingsChanged(): error in setting id" << act->data().toInt();
  }

  if( refresh_users )
    refreshUserList();
  if( refresh_chat )
    refreshChat();
}

void GuiMain::sendMessage( VNumber chat_id, const QString& msg )
{
  int num_messages = mp_core->sendChatMessage( chat_id, msg );
  qDebug() << num_messages << "messages sent";
}

void GuiMain::showChatMessage( VNumber chat_id, const ChatMessage& cm )
{
  bool is_current_chat = chat_id == mp_defaultChat->chatId();

  if( !cm.isSystem() && !cm.isFromLocalUser() )
  {
    if( !(isVisible() && is_current_chat) )
    {
      QApplication::alert( this );
      if( Settings::instance().beepOnNewMessageArrived() )
      {
        qDebug() << "New message arrived in background: play BEEP sound";
        if( QFile::exists( "beep.wav" ) )
        {
          QSound beep_sound( "beep.wav" );
          beep_sound.play();
        }
        else
          QApplication::beep();
      }

      mp_trayIcon->addUnreadMessage( 1 );
    }
  }

  if( is_current_chat )
  {
    mp_defaultChat->appendChatMessage( chat_id, cm );
    statusBar()->clearMessage();
    mp_userList->setUnreadMessages( chat_id, 0 );
  }
  else
  {
    Chat chat_hidden = ChatManager::instance().chat( chat_id );
    if( chat_hidden.isValid() )
    {
      mp_userList->setUnreadMessages( chat_id, chat_hidden.unreadMessages() );
      mp_chatList->updateChat( chat_id );
      refreshTitle( UserManager::instance().userList().find( cm.userId() ) );
    }
  }
}

void GuiMain::saveChat()
{
  QString file_name = QFileDialog::getSaveFileName( this,
                        tr( "Please select a file to save the messages of the chat." ),
                        Settings::instance().chatSaveDirectory(), "HTML Chat Files (*.htm)" );
  if( file_name.isNull() || file_name.isEmpty() )
    return;
  QFileInfo file_info( file_name );
  Settings::instance().setChatSaveDirectory( file_info.absolutePath() );

  QFile file( file_name );
  if( !file.open( QFile::WriteOnly ) )
  {
    QMessageBox::warning( this, QString( "%1 - %2" ).arg( Settings::instance().programName() ).arg( tr( "Warning" ) ),
      tr( "%1: unable to save the messages.\nPlease check the file or the directories write permissions." ).arg( file_name ), QMessageBox::Ok );
    return;
  }

  file.write( mp_defaultChat->toHtml().toLatin1() );
  file.close();
  QMessageBox::information( this, QString( "%1 - %2" ).arg( Settings::instance().programName(), tr( "Information" ) ),
    tr( "%1: save completed." ).arg( file_name ), QMessageBox::Ok );
}

void GuiMain::searchUsers()
{
  if( !mp_core->isConnected( true ) )
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

  if( !gsu.userId().isEmpty() )
  {
    mp_core->setXmppUserSubscription( gsu.service(), gsu.userId(), true );
  }
}

void GuiMain::showWritingUser( const User& u )
{
  QString msg = tr( "%1 is writing..." ).arg( u.name() );
  statusBar()->showMessage( msg, Settings::instance().writingTimeout() );
}

void GuiMain::statusSelected()
{
  QAction* act = qobject_cast<QAction*>( sender() );
  if( !act )
    return;

  int user_status = act->data().toInt();

  if( user_status == User::Offline && mp_core->isConnected( true ) )
  {
    if( QMessageBox::question( this, Settings::instance().programName(),
                               tr( "Do you want to disconnect from %1 network?" ).arg( Settings::instance().programName() ),
                               QMessageBox::Yes, QMessageBox::No ) == QMessageBox::Yes )
      stopCore();
    return;
  }

  mp_core->setLocalUserStatus( user_status );

  if( !mp_core->isConnected( false ) )
    startCore();
  else
    updateStatusIcon();
}

void GuiMain::updateStatusIcon()
{
  int status_type;
  if( !mp_core->isConnected( true ) )
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

void GuiMain::sendFile()
{
  bool ok = false;
  QStringList user_string_list = UserManager::instance().userList().serviceUserList( "Lan" ).toStringList( false, true );
  foreach( ServiceInterface* si, PluginManager::instance().services() )
  {
    if( si->fileTransferIsEnabled() )
      user_string_list.append( UserManager::instance().userList().serviceUserList( si->name() ).toStringList( false, true ) );
  }

  if( user_string_list.isEmpty() )
  {
    QMessageBox::information( this, Settings::instance().programName(), tr( "There is no user connected." ) );
    return;
  }

  QString user_path = QInputDialog::getItem( this, Settings::instance().programName(),
                                             tr( "Please select the user to whom you would like to send a file."),
                                             user_string_list, 0, false, &ok );
  if( !ok )
    return;

  User u = UserManager::instance().userList().find( user_path );
  sendFile( u );
}

void GuiMain::sendFile( VNumber user_id )
{
  User u = UserManager::instance().userList().find( user_id );
  sendFile( u );
}

void GuiMain::sendFile( const User& u )
{
  if( !u.isValid() )
  {
    QMessageBox::warning( this, Settings::instance().programName(), tr( "User not found." ) );
    return;
  }

  QString file_path = QFileDialog::getOpenFileName( this, tr( "%1 - Send a file to %2" ).arg( Settings::instance().programName(), u.name() ),
                                                    Settings::instance().lastDirectorySelected() );
  if( file_path.isEmpty() || file_path.isNull() )
    return;

  Chat c = ChatManager::instance().privateChatForUser( u.id() );
  if( c.isValid() )
    showChat( c.id() );
  mp_core->sendFile( u, file_path );
}

bool GuiMain::askToDownloadFile( const User& u, const FileInfo& fi )
{
  QString msg = tr( "Do you want to download ""%1"" (%2) from the user %3?" ).arg( fi.name(), Bee::bytesToString( fi.size() ), u.name() );

  if( QMessageBox::information( this, Settings::instance().programName(), msg, tr( "Yes" ), tr( "No" ), QString(), 1, 1 ) == 0 )
  {
    // Accepted
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
    return false;
}

void GuiMain::downloadFile( const User& u, const FileInfo& fi )
{
  if( !askToDownloadFile( u, fi ) )
    mp_core->refuseToDownloadFile( u, fi );
}

void GuiMain::downloadSharedFile( VNumber user_id, VNumber file_id )
{
  FileInfo file_info = FileShare::instance().networkFileInfo( user_id, file_id );
  if( !file_info.isValid() )
    return;

  User u = UserManager::instance().userList().find( user_id );
  if( !u.isValid() )
    return;

  askToDownloadFile( u, file_info );
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
  // Tip of the day is showed only in default chat
  showChat( ID_DEFAULT_CHAT );
  mp_core->showTipOfTheDay();
}

void GuiMain::showChat( VNumber chat_id )
{
  if( mp_defaultChat->setChatId( chat_id ) )
  {
    mp_userList->setUnreadMessages( chat_id, 0 );
    mp_chatList->updateChat( chat_id );
    raiseChatView();
  }
}

void GuiMain::changeVCard()
{
  GuiEditVCard gvc( this );
  gvc.setModal( true );
  gvc.setVCard( Settings::instance().localUser().vCard() );
  gvc.setUserColor( Settings::instance().localUser().color() );
  gvc.show();
  gvc.setFixedSize( gvc.size() );
  if( gvc.exec() == QDialog::Accepted )
  {
    if( gvc.userColor() != Settings::instance().localUser().color() )
    {
      qDebug() << "Local user color changed";
      mp_core->setUserColor( Settings::instance().localUser().id(), gvc.userColor() );
    }

    if( gvc.vCard() == Settings::instance().localUser().vCard() )
    {
      qDebug() << "Ok pressed but vCard is not changed";
      return;
    }

    qDebug() << "vCard changed";
    mp_core->setLocalUserVCard( gvc.vCard() );
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
  connect( gvc, SIGNAL( removeUser( VNumber ) ), this, SLOT( removeUser( VNumber ) ) );
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
  QAction* act;
  act = mp_menuPlugins->addAction( QIcon( ":/images/plugin.png" ), tr( "Plugin Manager..." ), this, SLOT( showPluginManager() ) );
  act->setStatusTip( tr( "Open the plugin manager dialog and manage the installed plugins" ) );

  if( PluginManager::instance().count() <= 0 )
    return;

  QString help_data_ts = tr( "is a plugin developed by" );
  QString help_data_format = QString( "<p>%1 <b>%2</b> %3 <b>%4</b>.<br /><i>%5</i></p><br />" );

  if( PluginManager::instance().textMarkers().size() )
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

  if( PluginManager::instance().services().size() > 0 )
    mp_menuPlugins->addSeparator();

  foreach( ServiceInterface* service, PluginManager::instance().services() )
  {
    act = mp_menuPlugins->addAction( service->name(), this, SLOT( showPluginHelp() ) );

    act->setData( help_data_format
                  .arg( Bee::iconToHtml( (service->icon().isNull() ? ":/images/plugin.png" : service->iconFileName()), "*P*" ),
                        service->name(), help_data_ts, service->author(), service->help() ) );
    act->setIcon( service->icon() );
    act->setEnabled( service->isEnabled() );
  }

  QList<QAction*> account_list = mp_menuAccounts->actions();
  foreach( QAction* account_act, account_list )
  {
    ServiceInterface* si = PluginManager::instance().service( account_act->data().toString() );
    if( !si )
      continue;

    if( mp_core->isXmppServerConnected( si->name() ) )
      account_act->setIcon( Bee::userStatusIcon(  si->name(), User::Online ) );
    else
      account_act->setIcon( si->icon() );
  }

  if( PluginManager::instance().games().size() > 0 )
    mp_menuPlugins->addSeparator();

  foreach( GameInterface* game, PluginManager::instance().games() )
  {
    act = mp_menuPlugins->addAction( game->name(), this, SLOT( showPluginHelp() ) );

    act->setData( help_data_format
                  .arg( Bee::iconToHtml( (game->icon().isNull() ? ":/images/plugin.png" : game->iconFileName()), "*P*" ),
                        game->name(), help_data_ts, game->author(), game->help() ) );
    act->setIcon( game->icon() );
    act->setEnabled( game->isEnabled() );
  }
}

void GuiMain::updateAccountMenu()
{
  QList<QAction*> account_list = mp_menuAccounts->actions();
  foreach( QAction* account_act, account_list )
  {
    ServiceInterface* si = PluginManager::instance().service( account_act->data().toString() );
    if( !si )
      continue;

    if( mp_core->isXmppServerConnected( si->name() ) )
      account_act->setIcon( Bee::userStatusIcon(  si->name(), User::Online ) );
    else
      account_act->setIcon( si->icon() );
  }
}

void GuiMain::showPluginHelp()
{
  QAction* act = qobject_cast<QAction*>(sender());
  if( !act )
    return;

  mp_defaultChat->appendMessage( act->data().toString() );
  raiseChatView();
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
  {
    updadePluginMenu();
    QList<ServiceInterface*> service_list = PluginManager::instance().services();
    if( service_list.isEmpty() )
      return;
    foreach( ServiceInterface* si, service_list )
    {
      if( !si->isEnabled() )
      {
        if( mp_core->isXmppServerConnected( si->name() ) )
          mp_core->disconnectFromXmppServer( si->name() );
      }
    }
    updateAccountMenu();
  }
}

void GuiMain::showNetworkManager()
{
  GuiNetwork gn( this );
  gn.setModal( true );
  gn.loadSettings();
  gn.show();
  gn.setFixedSize( gn.size() );
  gn.exec();
}

void GuiMain::showNetworkAccount()
{
  QAction* act = qobject_cast<QAction*>( sender() );
  if( !act )
    return;

  QString xmpp_service = act->data().toString();
  bool is_connected =  mp_core->isXmppServerConnected( xmpp_service );

  GuiNetworkLogin gnl( this );
  gnl.setModal( true );
  gnl.setNetworkAccount( Settings::instance().networkAccount( xmpp_service ), xmpp_service, is_connected );
  gnl.show();
  gnl.setFixedSize( gnl.size() );
  int result = gnl.exec();

  if( result == QDialog::Accepted )
  {
    if( is_connected )
    {
      if( QMessageBox::question( this, Settings::instance().programName(),
                                 tr( "You are connected to %1. Do you want to disconnect?").arg( xmpp_service ),
                                 tr( "Yes"), tr( "No" ), QString::null, 1, 1 ) == 0 )
      {
        mp_core->disconnectFromXmppServer( xmpp_service );
      }
    }
    else
    {
      Settings::instance().setNetworkAccount( gnl.account() );
      if( !mp_core->connectToXmppServer( gnl.account() ) )
        QMessageBox::information( this, Settings::instance().programName(),
                                  tr( "Unable to connect to %1. Plugin is not present or is not enabled." ).arg( gnl.account().service() ) );
    }
  }
}

void GuiMain::showUserSubscriptionRequest( const QString& service, const QString& bare_jid )
{
  switch( QMessageBox::question( this, Settings::instance().programName(),
                                 tr( "%1 (%2) wants to add you to the contact list. Do you accept?" ).arg( bare_jid, service ),
                                 tr( "Yes"), tr( "No"), QString(), 1, 1 ) )
  {
  case 0:
    mp_core->setXmppUserSubscription( service, bare_jid, true );
    break;
  case 1:
    mp_core->setXmppUserSubscription( service, bare_jid, false );
    break;
  }
}

void GuiMain::removeUser( VNumber user_id )
{
  User u = UserManager::instance().userList().find( user_id );
  if( !u.isValid() )
    return;
  if( u.isOnLan() )
    return;
  int res = QMessageBox::question( this, Settings::instance().programName(),
                                 tr( "Do you really want to remove %1 from the contact list?" ).arg( u.path() ),
                                 tr( "Yes"), tr( "No"), QString(), 1, 1 );
  if( res == 0 )
  {
    // remove the user
    if( mp_core->removeXmppUser( u ) )
    {
      mp_userList->removeUser( u );
    }
    else
      QMessageBox::warning( this, Settings::instance().programName(),
                            tr( "Unable to remove %1 from the contact list." ).arg( u.path() ) );
  }
}

void GuiMain::serviceConnected( const QString& )
{
  updateAccountMenu();
}

void GuiMain::serviceDisconnected( const QString& )
{
  updateAccountMenu();
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
  if( Settings::instance().trayMessageTimeout() > 0 )
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
  if( mp_core->addPathToShare( share_path ) > 0 )
    mp_shareLocal->loadSettings();
}

void GuiMain::removeFromShare( const QString& share_path )
{
  if( mp_core->removePathFromShare( share_path ) > 0 )
    mp_shareLocal->loadSettings();
}

void GuiMain::raiseChatView()
{
  setGameInPauseMode();
  mp_stackedWidget->setCurrentWidget( mp_defaultChat );
  mp_actViewDefaultChat->setEnabled( false );
  mp_actViewShareLocal->setEnabled( true );
  mp_actViewShareNetwork->setEnabled( true );
}

void GuiMain::raiseLocalShareView()
{
  setGameInPauseMode();
  mp_stackedWidget->setCurrentWidget( mp_shareLocal );
  mp_actViewDefaultChat->setEnabled( true );
  mp_actViewShareLocal->setEnabled( false );
  mp_actViewShareNetwork->setEnabled( true );
}

void GuiMain::raiseNetworkShareView()
{
  setGameInPauseMode();
  mp_stackedWidget->setCurrentWidget( mp_shareNetwork );
  mp_actViewDefaultChat->setEnabled( true );
  mp_actViewShareLocal->setEnabled( true );
  mp_actViewShareNetwork->setEnabled( false );

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

  // FIXME!!!
  mp_actViewDefaultChat->setEnabled( true );
  mp_actViewShareLocal->setEnabled( true );
  mp_actViewShareNetwork->setEnabled( true );
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
