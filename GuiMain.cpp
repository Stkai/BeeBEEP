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

#include "BeeBeep.h"
#include "BeeUtils.h"
#include "EmoticonManager.h"
#include "GuiChat.h"
#include "GuiUserList.h"
#include "GuiMain.h"
#include "Settings.h"



GuiMain::GuiMain( QWidget *parent )
 : QMainWindow( parent )
{
  mp_beeBeep = new BeeBeep( this );

  setWindowIcon( QIcon( ":/images/beebeep.png") );
  mp_defaultChat = new GuiChat( this );
  setCentralWidget( mp_defaultChat );
  mp_barMain = addToolBar( tr( "Show the ToolBar" ) );
  mp_barMain->setIconSize( Settings::instance().mainBarIconSize() );
  if( !Settings::instance().showToolBar() )
    mp_barMain->hide();

  createActions();
  createDockWindows();
  createMenus();
  createToolBars();
  createStatusBar();

  connect( mp_beeBeep, SIGNAL( newMessage( const QString&, const ChatMessage& ) ), this, SLOT( showMessage( const QString&, const ChatMessage& ) ) );
  connect( mp_beeBeep, SIGNAL( newUser( const User& ) ), this, SLOT( newUser( const User& ) ) );
  connect( mp_beeBeep, SIGNAL( removeUser( const User& ) ), this, SLOT( removeUser( const User& ) ) );
  connect( mp_beeBeep, SIGNAL( userIsWriting( const User& ) ), this, SLOT( showWritingUser( const User& ) ) );
  connect( mp_beeBeep, SIGNAL( userNewStatus( const User& ) ), this, SLOT( showNewUserStatus( const User& ) ) );
  connect( mp_defaultChat, SIGNAL( newMessage( const QString&, const QString& ) ), this, SLOT( sendMessage( const QString&, const QString& ) ) );
  connect( mp_defaultChat, SIGNAL( writing( const QString& ) ), mp_beeBeep, SLOT( sendWritingMessage( const QString& ) ) );
  connect( mp_defaultChat, SIGNAL( nextChat() ), this, SLOT( showNextChat() ) );
  connect( mp_userList, SIGNAL( chatSelected( int, const QString& ) ), this, SLOT( chatSelected( int, const QString& ) ) );
  connect( mp_userList, SIGNAL( stringToShow( const QString&, int ) ), statusBar(), SLOT( showMessage( const QString&, int ) ) );

  mp_defaultChat->setChat( mp_beeBeep->chat( Settings::instance().defaultChatName(), true, false ) );
  refreshTitle();
}

void GuiMain::refreshTitle()
{
  setWindowTitle( QString( "%1 - %2" ).arg( Settings::instance().programName() ).arg( mp_beeBeep->id() ) );
}

void GuiMain::closeEvent( QCloseEvent* e )
{
  if( !mp_beeBeep->isWorking() ||
      QMessageBox::question( this, tr( "Bye Bye Bee" ), tr( "Do you really want to quit %1?" ).arg( Settings::instance().programName() ),
                             QMessageBox::Yes | QMessageBox::No, QMessageBox::No ) == QMessageBox::Yes )
  {
#ifndef Q_OS_SYMBIAN
    Settings::instance().setGuiGeometry( saveGeometry() );
#endif
    Settings::instance().setUserListWidth( mp_userList->size().width() );
    Settings::instance().setShowMenuBar( mp_actMenuBar->isChecked() );
    Settings::instance().setShowToolBar( mp_actToolBar->isChecked() );
    if( mp_beeBeep->isWorking() )
      mp_beeBeep->stop();
    e->accept();
  }
  else
    e->ignore();
}

void GuiMain::showNextChat()
{
#if defined( BEEBEEP_DEBUG )
  qDebug() << "Change chat view";
#endif
  if( !mp_userList->nextUserWithUnreadMessages() )
    statusBar()->showMessage( tr( "No new message available" ) );
}

void GuiMain::selectNickname()
{
  bool ok = false;
  User local_user = Settings::instance().localUser();
  QString nick = QInputDialog::getText( this,
                                        Settings::instance().programName(),
                                        tr( "Please insert your nickname"),
                                        QLineEdit::Normal,
                                        local_user.nickname(),
                                        &ok );
  if( !ok )
    return;
  if( nick.isNull() || nick.isEmpty() )
    return;
  local_user.setNickname( nick );
  Settings::instance().setLocalUser( local_user );
}

void GuiMain::startStopBeeBeep()
{
  if( mp_beeBeep->isWorking() )
    stopBeeBeep();
  else
    startBeeBeep();
}

void GuiMain::startBeeBeep()
{
  bool ok = false;
  QString pwd = QInputDialog::getText( this,
                                       Settings::instance().programName(),
                                       tr( "Please insert the chat password (or just press Enter)"),
                                       QLineEdit::Normal,
                                       Settings::instance().defaultPassword(),
                                       &ok );
  if( !ok )
    return;
  Settings::instance().setPassword( pwd.simplified() );
  mp_beeBeep->start();
  mp_actSearch->setEnabled( true );
  refreshTitle();
  mp_actStartStopBeeBeep->setIcon( QIcon( ":/images/disconnect.png") );
  mp_actStartStopBeeBeep->setText( tr( "&Disconnect" ) );
  mp_actStartStopBeeBeep->setStatusTip( tr( "Disconnect from %1 network").arg( Settings::instance().programName() ) );
}

void GuiMain::stopBeeBeep()
{
  mp_actSearch->setEnabled( false );
  mp_defaultChat->setChat( mp_beeBeep->chat( Settings::instance().defaultChatName(), true, true ) );
  mp_beeBeep->stop();
  mp_userList->clear();
  refreshTitle();
  mp_actStartStopBeeBeep->setIcon( QIcon( ":/images/connect.png") );
  mp_actStartStopBeeBeep->setText( tr( "&Connect" ) );
  mp_actStartStopBeeBeep->setStatusTip( tr( "Connect to %1 network").arg( Settings::instance().programName() ) );
}

void GuiMain::showAbout()
{
  QString sAbout = QString( "<b>%1</b> - " ).arg( Settings::instance().programName() );
  QMessageBox::about( this, Settings::instance().programName(), sAbout +
     tr( "Secure Network Chat version %1" ).arg( Settings::instance().version() ) +
     tr( "<br />developed by Marco \"Khelben\" Mastroddi<br />e-mail: marco.mastroddi@gmail.com<br /><br />" ) +
     tr( "&lt; Free is that mind guided by the fantasy &gt;" ) );
}

void GuiMain::createActions()
{
  mp_actStartStopBeeBeep = new QAction( QIcon( ":/images/connect.png"), tr( "&Connect"), this );
  mp_actStartStopBeeBeep->setStatusTip( tr( "Connect to %1 network").arg( Settings::instance().programName() ) );
  connect( mp_actStartStopBeeBeep, SIGNAL( triggered() ), this, SLOT( startStopBeeBeep() ) );

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

  mp_actNickname = new QAction( QIcon( ":/images/profile.png"), tr( "Change your nickname..." ), this );
  mp_actNickname->setStatusTip( tr( "Select your favourite chat nickname" ) );
  connect( mp_actNickname, SIGNAL( triggered() ), this, SLOT( selectNickname() ) );

  mp_actFont = new QAction( QIcon( ":/images/font.png"), tr( "Chat font style..." ), this );
  mp_actFont->setStatusTip( tr( "Select your favourite chat font style" ) );
  connect( mp_actFont, SIGNAL( triggered() ), this, SLOT( selectFont() ) );

  mp_actFontColor = new QAction( QIcon( ":/images/font-color.png"), tr( "My message font color..." ), this );
  mp_actFontColor->setStatusTip( tr( "Select your favourite font color for the chat messages" ) );
  connect( mp_actFontColor, SIGNAL( triggered() ), this, SLOT( selectFontColor() ) );

  mp_actMenuBar = new QAction( tr( "Show the MenuBar" ), this );
  mp_actMenuBar->setStatusTip( tr( "Show the main menu bar with the %1 options" ).arg( Settings::instance().programName() ) );
  mp_actMenuBar->setCheckable( true );
  mp_actMenuBar->setChecked( Settings::instance().showMenuBar() );
  mp_actMenuBar->setData( 99 );
  toggleMenuBar( mp_actMenuBar->isChecked() );
  connect( mp_actMenuBar, SIGNAL( toggled( bool ) ), this, SLOT( toggleMenuBar( bool ) ) );

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
  QMenu* menu;

  /* Main Menu */
  menu = menuBar()->addMenu( tr( "&Main" ) );
  menu->addAction( mp_actStartStopBeeBeep );
  menu->addSeparator();
  menu->addAction( mp_actNickname );
  menu->addAction( mp_actSearch );
  menu->addSeparator();
  menu->addAction( mp_actSaveChat );
  menu->addSeparator();
  menu->addAction( mp_actQuit );

  /* Settings Menu */
  mp_menuSettingsIcon = new QMenu( tr( "Settings" ), this );
  mp_menuSettingsIcon->setStatusTip( tr( "Choose your preferred %1 settings" ).arg( Settings::instance().programName() ) );
  mp_menuSettingsIcon->setIcon( QIcon( ":/images/settings.png" ) );
  mp_menuSettings = menuBar()->addMenu( tr( "&Settings") );

  mp_menuSettings->addAction( mp_actFont );
  mp_menuSettings->addAction( mp_actFontColor );
  mp_menuSettings->addSeparator();
  mp_menuSettingsIcon->addAction( mp_actMenuBar );
  mp_menuSettings->addAction( mp_actMenuBar );
  mp_menuSettingsIcon->addAction( mp_actToolBar );
  mp_menuSettings->addAction( mp_actToolBar );
  mp_menuSettings->addSeparator();
  mp_menuSettingsIcon->addSeparator();

  act = mp_menuSettings->addAction( tr( "Enable the compact mode in chat window" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled the sender's nickname and his message are in the same line" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().chatCompact() );
  act->setData( 1 );
  mp_menuSettingsIcon->addAction( act );

  act = mp_menuSettings->addAction( tr( "Add a blank line between the messages" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled the messages in the chat window are separated by a blank line" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().chatAddNewLineToMessage() );
  act->setData( 2 );
  mp_menuSettingsIcon->addAction( act );

  act = mp_menuSettings->addAction( tr( "Show the messages' timestamp" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled the message shows its timestamp in the chat window" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().chatShowMessageTimestamp() );
  act->setData( 3 );
  mp_menuSettingsIcon->addAction( act );

  act = mp_menuSettings->addAction( tr( "Beep on new message arrived" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled when a new message is arrived a sound is emitted" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().beepOnNewMessageArrived() );
  act->setData( 4 );
  mp_menuSettingsIcon->addAction( act );

  mp_menuSettings->addSeparator();
  mp_menuSettingsIcon->addSeparator();

  mp_menuSettings->addAction( mp_actViewUsers );
  mp_menuSettingsIcon->addAction( mp_actViewUsers );

  act = mp_menuSettings->addAction( tr( "Show user's address IP" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled the IP addresses of the connected users are shown" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().showUserIp() );
  act->setData( 5 );
  mp_menuSettingsIcon->addAction( act );

  act = mp_menuSettings->addAction( tr( "Show user's nickname" ), this, SLOT( settingsChanged() ) );
  act->setStatusTip( tr( "If enabled the nickname of the connected users are shown" ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().showUserNickname() );
  act->setData( 6 );
  mp_menuSettingsIcon->addAction( act );

  /* Emoticons Menu for ToolBar and MenuBar */
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

  /* Help Menu */
  menu = menuBar()->addMenu( "&?" );
  menu->addAction( mp_actAbout );
  act = menu->addAction( QIcon( ":/images/qt.png" ), tr( "About &Qt..." ), qApp, SLOT( aboutQt() ) );
  act->setStatusTip( tr( "Show the informations about Qt library" ) );
}

void GuiMain::createToolBars()
{
  mp_barMain->addAction( mp_actStartStopBeeBeep );
  mp_barMain->addSeparator();
  mp_barMain->addAction( mp_actNickname );
  mp_barMain->addAction( mp_actSearch );
  mp_barMain->addSeparator();
  mp_barMain->addAction( mp_actSaveChat );
  mp_barMain->addSeparator();
  mp_barMain->addAction( mp_menuSettingsIcon->menuAction() );
  mp_barMain->addSeparator();
  mp_barMain->addAction( mp_actFont );
  mp_barMain->addAction( mp_actFontColor );
  mp_barMain->addAction( mp_menuEmoticons->menuAction() );
  mp_barMain->addSeparator();
  mp_barMain->addAction( mp_actAbout );
}

void GuiMain::createStatusBar()
{
  statusBar()->showMessage( tr( "Ready" ) );
}

void GuiMain::createDockWindows()
{
  QDockWidget *dock_widget = new QDockWidget( tr( "Users" ), this );
  dock_widget->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
  mp_userList = new GuiUserList( dock_widget );
  dock_widget->setWidget( mp_userList );
  addDockWidget( Qt::RightDockWidgetArea, dock_widget );
  mp_actViewUsers = dock_widget->toggleViewAction();
  mp_actViewUsers->setText( tr( "Show online users and active chats" ) );
  mp_actViewUsers->setStatusTip( tr( "Show the list of the connected users and the active chats" ) );
  mp_actViewUsers->setData( 99 );
}

void GuiMain::toggleMenuBar( bool is_enabled )
{
  if( is_enabled )
    menuBar()->show();
  else
    menuBar()->hide();
}

void GuiMain::newUser( const User& u )
{
  if( !u.isValid() )
    return;
  mp_userList->setUser( u, 0 );
}

void GuiMain::removeUser( const User& u )
{
  if( !u.isValid() )
    return;
  mp_userList->removeUser( u );
}

void GuiMain::emoticonSelected()
{
  QAction* act = qobject_cast<QAction*>( sender() );
  if( act )
    mp_defaultChat->addToMyMessage( QString( " %1 " ).arg( act->data().toString() ) );
}

void GuiMain::refreshUserList()
{
  mp_userList->updateUsers();
}

void GuiMain::refreshChat()
{
  Chat c = mp_beeBeep->chat( mp_defaultChat->chatName(), false, true );
  if( c.isValid() )
    mp_defaultChat->setChat( c );
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
    Settings::instance().setShowUserIp( act->isChecked() );
    refresh_users = true;
    break;
  case 6:
    Settings::instance().setShowUserNickname( act->isChecked() );
    refresh_users = true;
    refresh_chat = true;
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

void GuiMain::chatSelected( int user_id, const QString& chat_name )
{
  if( user_id == Settings::instance().localUser().id())
  {
    mp_defaultChat->setChat( mp_beeBeep->chat( Settings::instance().defaultChatName(), true, true ) );
    return;
  }
  Chat c = mp_beeBeep->chat( chat_name, true, true );
  mp_defaultChat->setChat( c );
}

void GuiMain::sendMessage( const QString& chat_name, const QString& msg )
{
  mp_beeBeep->sendMessage( chat_name, msg );
}

void GuiMain::showMessage( const QString& chat_name, const ChatMessage& cm )
{
  bool is_current_chat = chat_name == mp_defaultChat->chatName();

  if( !cm.isSystem() && !cm.isLocal() )
  {
    QApplication::alert( this, 2000 );
    if( Settings::instance().beepOnNewMessageArrived() && !(isVisible() && is_current_chat) )
    {
#if defined( BEEBEEP_DEBUG )
      qDebug() << "New message arrived: BEEP";
#endif
      QSound beep_sound( "beep.wav" );
      if( QSound::isAvailable() )
        beep_sound.play();
      else
        QApplication::beep();
    }
  }

  if( is_current_chat )
  {
    Chat chat_showed = mp_beeBeep->chat( chat_name, true, true );
    QString txt = "";
    if( cm.isSystem() )
      txt = Bee::formatSystemMessage( cm );
    else
      txt = Bee::formatMessage( cm );
    mp_defaultChat->appendMessage( chat_name, txt );
    mp_defaultChat->setLastMessageTimestamp( chat_showed.lastMessageTimestamp() );
    statusBar()->clearMessage();
    return;
  }
  Chat chat_hided = mp_beeBeep->chat( chat_name, true, false );
  mp_userList->setUnreadMessages( chat_name, chat_hided.unreadMessages() );
}

void GuiMain::saveChat()
{
  QString chat_name = mp_defaultChat->chatName();
  QString file_name = QFileDialog::getSaveFileName( this,
                        tr( "Please select a file to save the messages of 'Chat with %1'" ).arg( chat_name ),
                        Settings::instance().chatSaveDirectory(), "HTML Chat Files (*.htm)" );
  if( file_name.isNull() )
    return;
  QFileInfo file_info( file_name );
  Settings::instance().setChatSaveDirectory( file_info.absolutePath() );
  Chat c = mp_beeBeep->chat( chat_name, false, true );
  if( !c.isValid() )
    return;
  QFile file( file_name );
  if( !file.open( QFile::WriteOnly ) )
  {
    QMessageBox::warning( this, QString( "%1 - %2" ).arg( Settings::instance().programName() ).arg( tr( "Warning" ) ),
      tr( "%1: unable to save the messages.\nPlease check the file or the directories write permissions." ).arg( file_name ), QMessageBox::Ok );
    return;
  }
  QString sHeader = QString( tr( "<html><body><b>Chat with '%1' saved in date %2.</b><br /><br />" ).arg( chat_name )
                             .arg( QDateTime::currentDateTime().toString( Qt::SystemLocaleLongDate ) ) );
  QString sFooter = QString( "</body></html>" );
  file.write( sHeader.toLatin1() );
  file.write( Bee::chatMessagesToText( c ).toLatin1() );
  file.write( sFooter.toLatin1() );
  file.close();
  QMessageBox::information( this, QString( "%1 - %2" ).arg( Settings::instance().programName() ).arg( tr( "Information" ) ),
    tr( "%1: save completed." ).arg( file_name ), QMessageBox::Ok );
}

void GuiMain::searchUsers()
{
  if( !mp_beeBeep->isWorking() )
    return;

  bool ok = false;
  QString s = QInputDialog::getText( this, Settings::instance().programName(),
                           tr( "Please insert the Host Address or Broadcast Address to contact\n(ex. 10.184.15.186 or 10.184.15.255)" ), QLineEdit::Normal, "", &ok );
  if( !ok )
    return;

  if( s.isEmpty() || s.isNull() )
    return;

  QHostAddress host_address( s );
  if( host_address.isNull() )
  {
    QMessageBox::warning( this, QString( "%1 - %2" ).arg( Settings::instance().programName() ).arg( tr( "Warning" ) ),
      tr( "You have selected an invalid host address." ), QMessageBox::Ok );
    return;
  }

  mp_beeBeep->searchUsers( host_address );
}

void GuiMain::showWritingUser( const User& u )
{
  QString msg = tr( "%1 is writing" ).arg( Settings::instance().showUserNickname() ? u.nickname() : u.name() );
  statusBar()->showMessage( msg, WRITING_MESSAGE_TIMEOUT );
}

void GuiMain::showNewUserStatus( const User& u )
{
  QString msg = tr( "%1 is %2%3" ).arg( Settings::instance().showUserNickname() ? u.nickname() : u.name() )
                                  .arg( Bee::userStatusToString( u.status() ) )
                                  .arg( u.statusDescription().isEmpty() ? "" : QString( ":%1").arg( u.statusDescription() ) );
  statusBar()->showMessage( msg, WRITING_MESSAGE_TIMEOUT );
}
