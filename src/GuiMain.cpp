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
#include "FileInfo.h"
#include "GuiChat.h"
#include "GuiEditVCard.h"
#include "GuiNetwork.h"
#include "GuiNetworkLogin.h"
#include "GuiPluginManager.h"
#include "GuiTransferFile.h"
#include "GuiUserList.h"
#include "GuiMain.h"
#include "GuiVCard.h"
#include "PluginManager.h"
#include "Settings.h"
#include "UserManager.h"


GuiMain::GuiMain( QWidget *parent )
 : QMainWindow( parent )
{
  setObjectName( "GuiMainWindow" );
  mp_core = new Core( this );

  setWindowIcon( QIcon( ":/images/beebeep.png") );
  mp_defaultChat = new GuiChat( this );
  setCentralWidget( mp_defaultChat );
  mp_barMain = addToolBar( tr( "Show the ToolBar" ) );
  mp_barMain->setObjectName( "GuiMainToolBar" );
  mp_barMain->setIconSize( Settings::instance().mainBarIconSize() );

  createActions();
  createDockWindows();
  createMenus();
  createToolAndMenuBars();
  createStatusBar();

  connect( mp_core, SIGNAL( chatMessage( VNumber, const ChatMessage& ) ), this, SLOT( showChatMessage( VNumber, const ChatMessage& ) ) );
  connect( mp_core, SIGNAL( fileDownloadRequest( const User&, const FileInfo& ) ), this, SLOT( downloadFile( const User&, const FileInfo& ) ) );
  connect( mp_core, SIGNAL( userChanged( const User& ) ), this, SLOT( checkUser( const User& ) ) );
  connect( mp_core, SIGNAL( userIsWriting( const User& ) ), this, SLOT( showWritingUser( const User& ) ) );
  connect( mp_core, SIGNAL( xmppUserSubscriptionRequest( const QString&, const QString& ) ), this, SLOT( showUserSubscriptionRequest( const QString&, const QString& ) ) );

  connect( mp_core, SIGNAL( fileTransferProgress( VNumber, const User&, const FileInfo&, FileSizeType ) ), mp_fileTransfer, SLOT( setProgress( VNumber, const User&, const FileInfo&, FileSizeType ) ) );
  connect( mp_core, SIGNAL( fileTransferMessage( VNumber, const User&, const FileInfo&, const QString& ) ), mp_fileTransfer, SLOT( setMessage( VNumber, const User&, const FileInfo&, const QString& ) ) );
  connect( mp_fileTransfer, SIGNAL( transferCancelled( VNumber ) ), mp_core, SLOT( cancelFileTransfer( VNumber ) ) );
  connect( mp_fileTransfer, SIGNAL( stringToShow( const QString&, int ) ), statusBar(), SLOT( showMessage( const QString&, int ) ) );

  connect( mp_defaultChat, SIGNAL( newMessage( VNumber, const QString& ) ), this, SLOT( sendMessage( VNumber, const QString& ) ) );
  connect( mp_defaultChat, SIGNAL( writing( VNumber ) ), mp_core, SLOT( sendWritingMessage( VNumber ) ) );
  connect( mp_defaultChat, SIGNAL( nextChat() ), this, SLOT( showNextChat() ) );

  connect( mp_userList, SIGNAL( chatSelected( VNumber ) ), this, SLOT( showChat( VNumber ) ) );
  connect( mp_userList, SIGNAL( menuToShow( VNumber ) ), this, SLOT( showUserMenu( VNumber ) ) );

  showChat( ID_DEFAULT_CHAT );

  refreshTitle();
}

void GuiMain::refreshTitle()
{
  setWindowTitle( QString( "%1 - %2 (%3)" ).arg(
                    Settings::instance().programName(),
                    Settings::instance().localUser().name(),
                    (mp_core->isConnected() ?
                       Bee::userStatusToString( Settings::instance().localUser().status() ) :
                       tr( "offline" ) ) ) );
}

void GuiMain::closeEvent( QCloseEvent* e )
{
  if( !mp_core->isConnected() ||
      QMessageBox::question( this, Settings::instance().programName(), tr( "Do you really want to quit %1?" ).arg( Settings::instance().programName() ),
                             QMessageBox::Yes | QMessageBox::No, QMessageBox::No ) == QMessageBox::Yes )
  {
#ifndef Q_OS_SYMBIAN
    Settings::instance().setGuiGeometry( saveGeometry() );
    Settings::instance().setGuiState( saveState() );
#endif
    if( mp_core->isConnected() )
      mp_core->stop();
    e->accept();
  }
  else
    e->ignore();
}

void GuiMain::showNextChat()
{
  qDebug() << "Show next chat in list with unread messages";
  if( !mp_userList->nextUserWithUnreadMessages() )
    statusBar()->showMessage( tr( "No new message available" ) );
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
  bool enable = mp_core->isConnected();

  refreshTitle();
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

  mp_menuStatus->setEnabled( enable );
  mp_actSendFile->setEnabled( enable );
  mp_actSearch->setEnabled( enable );
}

void GuiMain::showAbout()
{
  QMessageBox::about( this, Settings::instance().programName(),
                      QString( "<b>%1</b> - %2<br /><br />%3 %4<br />%5<br />%6<br /><br />%7" )
                      .arg( Settings::instance().programName() )
                      .arg( tr( "Secure Network Chat" ) )
                      .arg( tr( "Version" ) )
                      .arg( Settings::instance().version( true ) )
                      .arg( tr( "developed by Marco Mastroddi" ) )
                      .arg( tr( "e-mail: marco.mastroddi@gmail.com") )
                      .arg( QString( "XMPP: QXmpp 0.3.0 (Manjeet Dahiya)") )
                      );

}

void GuiMain::createActions()
{
  mp_actStartStopCore = new QAction( QIcon( ":/images/connect.png"), tr( "&Connect"), this );
  mp_actStartStopCore->setStatusTip( tr( "Connect to %1 network").arg( Settings::instance().programName() ) );
  connect( mp_actStartStopCore, SIGNAL( triggered() ), this, SLOT( startStopCore() ) );

  mp_actSearch = new QAction( QIcon( ":/images/search.png"), tr( "Search &users..."), this );
  mp_actSearch->setStatusTip( tr( "Search the users outside the %1 local network").arg( Settings::instance().programName() ) );
  mp_actSearch->setEnabled( false );
  connect( mp_actSearch, SIGNAL( triggered() ), this, SLOT( searchUsers() ) );

  mp_actSaveChat = new QAction( QIcon( ":/images/save-as.png" ), tr( "&Save chat..." ), this );
  mp_actSaveChat->setShortcuts( QKeySequence::Quit );
  mp_actSaveChat->setStatusTip( tr( "Save the messages of the current chat to a file" ) );
  connect( mp_actSaveChat, SIGNAL( triggered() ), this, SLOT( saveChat() ) );

  mp_actQuit = new QAction( QIcon( ":/images/quit.png" ), tr( "&Quit" ), this );
  mp_actQuit->setShortcuts( QKeySequence::Quit );
  mp_actQuit->setStatusTip( tr( "Close the chat and quit %1" ).arg( Settings::instance().programName() ) );
  connect( mp_actQuit, SIGNAL( triggered() ), this, SLOT( close() ) );

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
  mp_menuMain->addSeparator();
  act = mp_menuMain->addAction( QIcon( ":/images/network-account.png" ), tr( "Network account..."), this, SLOT( showNetworkAccount() ) );
  act->setStatusTip( tr( "Show the network account dialog" ) );
  act = mp_menuMain->addAction( QIcon( ":/images/network-settings.png" ), tr( "Network settings..."), this, SLOT( showNetworkManager() ) );
  act->setStatusTip( tr( "Show the network settings dialog" ) );
  act = mp_menuMain->addAction( QIcon( ":/images/download-folder.png" ), tr( "Download folder..."), this, SLOT( selectDownloadDirectory() ) );
  act->setStatusTip( tr( "Select the download folder" ) );
  mp_menuMain->addSeparator();
  mp_menuMain->addAction( mp_actQuit );

  /* Settings Menu */
  mp_menuSettings = new QMenu( tr( "&Settings" ), this );
  mp_menuSettings->addAction( mp_actToolBar );
  mp_menuSettings->addSeparator();

  act = mp_menuSettings->addAction( tr( "Show only the nicknames" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled only the nickname of the connected users are shown" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().showOnlyUsername() );
  act->setData( 6 );

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

  mp_menuSettings->addSeparator();

  act = mp_menuSettings->addAction( tr( "Beep on new message arrived" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled when a new message is arrived a sound is emitted" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().beepOnNewMessageArrived() );
  act->setData( 4 );

  mp_menuSettings->addSeparator();

  act = mp_menuSettings->addAction( tr( "Generate automatic filename" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If the file to be downloaded already exists a new filename is automatically generated" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().automaticFileName() );
  act->setData( 7 );

  /* Emoticons Menu for ToolBar */
  mp_menuEmoticons = new QMenu( tr( "Emoticons" ), this );
  mp_menuEmoticons->setStatusTip( tr( "Add your preferred emoticon to the message" ) );
  mp_menuEmoticons->setIcon( QIcon( ":/images/emoticon.png" ) );
  QMultiHash<QChar, Emoticon>::const_iterator it = EmoticonManager::instance().emoticons().begin();
  while( it != EmoticonManager::instance().emoticons().end() )
  {
    act = mp_menuEmoticons->addAction( QIcon( (*it).pixmap() ), (*it).name(), this, SLOT( emoticonSelected() ) );
    act->setData( (*it).textToMatch() );
    act->setStatusTip( QString( " %1   < %2 >" ).arg( (*it).textToMatch() ).arg( (*it).name() ) );
    act->setIconVisibleInMenu( true );
    ++it;
  }

  /* Status Menu */
  mp_menuStatus = new QMenu( tr( "Status" ), this );
  mp_menuStatus->setStatusTip( tr( "Select your status" ) );
  mp_menuStatus->setIcon( QIcon( ":/images/user-status.png" ) );
  for( int i = User::Online; i < User::NumStatus; i++ )
  {
    act = mp_menuStatus->addAction( Bee::userStatusIcon( i ), Bee::userStatusToString( i ), this, SLOT( statusSelected() ) );
    act->setData( i );
    act->setStatusTip( tr( "Your status will be %1" ).arg( Bee::userStatusToString( i ) ) );
    act->setIconVisibleInMenu( true );
  }

  mp_menuStatus->addSeparator();
  act = mp_menuStatus->addAction( QIcon( ":/images/user-status.png" ), tr( "Add a status description..." ), this, SLOT( changeStatusDescription() ) );

  /* Help Menu */
  mp_menuInfo = new QMenu( tr("&?" ), this );
  act = mp_menuInfo->addAction( QIcon( ":/images/tip.png" ), tr( "Tips of the day" ), this, SLOT( showTipOfTheDay() ) );
  act->setStatusTip( tr( "Show me the tip of the day" ) );
  mp_menuInfo->addSeparator();
  mp_menuInfo->addAction( mp_actAbout );
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
  mp_barMain->addAction( mp_actViewFileTransfer );
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

  QDockWidget* dock_widget = new QDockWidget( tr( "File Transfers" ), this );
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

void GuiMain::checkUser( const User& u )
{
  if( !u.isValid() )
  {
    qDebug() << "Invalid user found in GuiMain::checkUser( const User& u )";
    return;
  }

  if( u.isLocal() )
    refreshTitle();

  qDebug() << "User" << u.path() << "has changed his info. Check it";
  if( u.status() == User::Offline )
  {
    mp_userList->removeUser( u, false );
    return;
  }

  Chat private_chat = ChatManager::instance().privateChatForUser( u.id() );
  if( !private_chat.isValid() )
    return;
  mp_userList->setUser( u, private_chat.id(), private_chat.unreadMessages() );
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
  mp_userList->updateUsers();
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
    Settings::instance().setShowOnlyUsername( act->isChecked() );
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
    QApplication::alert( this, 2000 );
    if( Settings::instance().beepOnNewMessageArrived() && !(isVisible() && is_current_chat) )
    {
      qDebug() << "New message arrived in background: play BEEP sound";
      if( QSound::isAvailable() )
      {
        QSound beep_sound( "beep.wav" );
        beep_sound.play();
      }
      else
        QApplication::beep();
    }
  }

  if( is_current_chat )
  {
    mp_defaultChat->appendChatMessage( chat_id, cm );
    statusBar()->clearMessage();
  }
  else
  {
    Chat chat_hidden = ChatManager::instance().chat( chat_id );
    mp_userList->setUnreadMessages( chat_id, chat_hidden.unreadMessages() );
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
  if( !mp_core->isConnected() )
    return;

  bool ok = false;
  QString s = QInputDialog::getText( this, Settings::instance().programName(),
                           tr( "Please insert the Host Address or Broadcast Address to contact\n(ex. 10.184.15.186 or 10.184.15.255)\n" ) +
                           tr( "or insert a valid jabber id (ex: user@gmail.com)" ), QLineEdit::Normal, "", &ok );
  if( !ok || s.isEmpty() || s.isNull() )
    return;

  QString service = "GTalk"; // FIXME!!!

  if( s.contains( "@gmail.com" ) || s.contains( "@jabber.org" ) )
  {
    mp_core->setXmppUserSubscription( service, s.trimmed(), true );
    return;
  }

  QHostAddress host_address( s );
  if( host_address.isNull() )
  {
    QMessageBox::warning( this, QString( "%1 - %2" ).arg( Settings::instance().programName() ).arg( tr( "Warning" ) ),
      tr( "You have selected an invalid host address." ), QMessageBox::Ok );
    return;
  }

  // Message is showed only in default chat
  showChat( ID_DEFAULT_CHAT );
  mp_core->searchUsers( host_address );
}

void GuiMain::showWritingUser( const User& u )
{
  QString msg = tr( "%1 is writing..." ).arg( Settings::instance().showOnlyUsername() ? u.name() : u.path() );
  statusBar()->showMessage( msg, Settings::instance().writingTimeout() );
}

void GuiMain::statusSelected()
{
  QAction* act = qobject_cast<QAction*>( sender() );
  if( !act )
    return;
  mp_core->setLocalUserStatus( act->data().toInt() );
  refreshTitle();
}

void GuiMain::changeStatusDescription()
{
  bool ok = false;
  QString status_description = QInputDialog::getText( this, Settings::instance().programName(),
                           tr( "Please insert the new status description" ), QLineEdit::Normal, Settings::instance().localUser().statusDescription(), &ok );
  if( !ok || status_description.isNull() )
    return;
  mp_core->setLocalUserStatusDescription( status_description );
  refreshTitle();
}

void GuiMain::sendFile()
{
  bool ok = false;
  QStringList user_string_list = UserManager::instance().userList().serviceUserList( "Lan" ).toStringList( false, true );
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

  User user_selected = UserManager::instance().userList().find( user_path );
  if( !user_selected.isValid() )
  {
    QMessageBox::information( this, Settings::instance().programName(), tr( "User %1 not found." ).arg( user_path ) );
    return;
  }

  QString file_path = QFileDialog::getOpenFileName( this, tr( "%1 - Send a file to %2" ).arg( Settings::instance().programName(), user_selected.name() ),
                                                    Settings::instance().lastDirectorySelected() );
  if( file_path.isEmpty() || file_path.isNull() )
    return;

  mp_core->sendFile( user_selected, file_path );
}

void GuiMain::sendFile( VNumber user_id )
{
  User user_selected = UserManager::instance().userList().find( user_id );
  if( !user_selected.isValid() )
  {
    QMessageBox::warning( this, Settings::instance().programName(), tr( "User not found." ) );
    return;
  }

  QString file_path = QFileDialog::getOpenFileName( this, tr( "%1 - Send a file to %2" ).arg( Settings::instance().programName(), user_selected.name() ),
                                                    Settings::instance().lastDirectorySelected() );
  if( file_path.isEmpty() || file_path.isNull() )
    return;

  mp_core->sendFile( user_selected, file_path );
}

void GuiMain::downloadFile( const User& u, const FileInfo& fi )
{
  QString msg = tr( "Do you want to download from %1\n%2 (%3)?" ).arg( Settings::instance().showOnlyUsername() ? u.name() : u.path() )
                                                                 .arg( fi.name() ).arg( Bee::bytesToString( fi.size() ) );
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
          return;
      }
      qfile_info = QFileInfo( file_name );
    }
    FileInfo file_info = fi;
    file_info.setName( qfile_info.fileName() );
    file_info.setPath( qfile_info.absoluteFilePath() );
    mp_core->downloadFile( u, file_info );
  }
  else
    mp_core->refuseToDownloadFile( u, fi );
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
    mp_userList->setUnreadMessages( chat_id, 0 );
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

  mp_menuPlugins->addSeparator();

  foreach( TextMarkerInterface* text_marker, PluginManager::instance().textMarkers() )
  {
    act = mp_menuPlugins->addAction( text_marker->name(), this, SLOT( showTextMarkerPluginHelp() ) );
    QString help_data_ts = tr( "is a plugin developed by" );
    act->setData( QString( "<p>%1 <b>%2</b> %3 <b>%4</b>.<br /><i>%5</i></p><br />" )
                  .arg( Bee::iconToHtml( (text_marker->icon().isNull() ? ":/images/plugin.png" : text_marker->iconFileName()), "*P*" ),
                        text_marker->name(), help_data_ts, text_marker->author(), text_marker->help() ) );
    act->setIcon( text_marker->icon() );
    act->setEnabled( text_marker->isEnabled() );
  }
}

void GuiMain::showTextMarkerPluginHelp()
{
  QAction* act = qobject_cast<QAction*>(sender());
  if( act )
    mp_defaultChat->appendMessage( act->data().toString() );
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
  GuiNetworkLogin gnl( this );
  gnl.setModal( true );
  gnl.loadSettings();
  gnl.show();
  gnl.setFixedSize( gnl.size() );
  int result = gnl.exec();
  if( result == QDialog::Accepted )
    mp_core->connectToXmppServer( "GTalk", gnl.user(), gnl.password() );
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
      mp_userList->removeUser( u, true );
    }
    else
      QMessageBox::warning( this, Settings::instance().programName(),
                            tr( "Unable to remove %1 from the contact list." ).arg( u.path() ) );
  }
}
