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

#include "Avatar.h"
#include "BeeUtils.h"
#include "ChatManager.h"
#include "ChatMessage.h"
#include "GuiChat.h"
#include "GuiChatMessage.h"
#include "Protocol.h"
#include "Settings.h"
#include "ShortcutManager.h"
#include "UserManager.h"


GuiChat::GuiChat( QWidget *parent )
 : QWidget( parent )
{
  setupUi( this );
  setObjectName( "GuiChat" );
  setAcceptDrops( true );

  QGridLayout* grid_layout = new QGridLayout( this );
  grid_layout->setSpacing( 0 );
  grid_layout->setObjectName( QString::fromUtf8( "grid_layout" ) );
  grid_layout->setContentsMargins( 0, 0, 0, 0 );

  grid_layout->addWidget( mp_frameHeader, 0, 0, 1, 1 );

  mp_splitter = new QSplitter( this );
  mp_splitter->setOrientation( Qt::Vertical );
  mp_splitter->setChildrenCollapsible( false );
  mp_splitter->addWidget( mp_teChat );
  mp_splitter->addWidget( mp_frameMessage );

  grid_layout->addWidget( mp_splitter, 1, 0, 1, 1);
  QList<int> widget_sizes;
  widget_sizes.append( 200 );
  widget_sizes.append( 80 );
  mp_splitter->setSizes( widget_sizes );

  mp_teMessage->setFocusPolicy( Qt::StrongFocus );
  mp_teMessage->setAcceptRichText( false );

  mp_teChat->setObjectName( "GuiChatViewer" );
  m_defaultChatPalette = mp_teChat->palette();
  mp_teChat->setFocusPolicy( Qt::ClickFocus );
  mp_teChat->setReadOnly( true );
  mp_teChat->setUndoRedoEnabled( false );
  mp_teChat->setContextMenuPolicy( Qt::CustomContextMenu );
  mp_teChat->setOpenExternalLinks( false );
  mp_teChat->setOpenLinks( false );
  mp_teChat->setAcceptRichText( false );

  setChatFont( Settings::instance().chatFont() );
  setChatFontColor( Settings::instance().chatFontColor() );

  m_chatId = ID_DEFAULT_CHAT;
  m_lastMessageUserId = 0;
  m_isFloating = false;

  mp_scFocusInChat = new QShortcut( this );
  mp_scFocusInChat->setContext( Qt::WindowShortcut );
  connect( mp_scFocusInChat, SIGNAL( activated() ), this, SLOT( ensureFocusInChat() ) );

  connect( mp_teChat, SIGNAL( customContextMenuRequested( const QPoint& ) ), this, SLOT( customContextMenu( const QPoint& ) ) );
  connect( mp_teChat, SIGNAL( anchorClicked( const QUrl& ) ), this, SLOT( checkAnchorClicked( const QUrl&  ) ) );
  connect( mp_teMessage, SIGNAL( returnPressed() ), this, SLOT( sendMessage() ) );
  connect( mp_teMessage, SIGNAL( writing() ), this, SLOT( checkWriting() ) );
  connect( mp_teMessage, SIGNAL( urlsToCheck( const QMimeData* ) ), this, SLOT( checkAndSendUrls( const QMimeData* ) ) );
  connect( mp_teMessage, SIGNAL( imageToCheck( const QMimeData* ) ), this, SLOT( checkAndSendImage( const QMimeData* ) ) );
  connect( mp_pbSend, SIGNAL( clicked() ), this, SLOT( sendMessage() ) );
  connect( mp_pbDetach, SIGNAL( clicked() ), this, SLOT( detachThisChat() ) );
  connect( mp_pbSaveState, SIGNAL( clicked() ), this, SIGNAL( saveStateAndGeometryRequest() ) );

}

void GuiChat::enableDetachButtons()
{
  if( m_chatId == ID_DEFAULT_CHAT )
  {
    mp_pbDetach->setEnabled( false );
    mp_pbDetach->setVisible( false );
    mp_pbSaveState->setEnabled( false );
    mp_pbSaveState->setVisible( false );
  }
  else
  {
    mp_pbDetach->setEnabled( !m_isFloating );
    mp_pbDetach->setVisible( !m_isFloating );
    mp_pbSaveState->setEnabled( m_isFloating );
    mp_pbSaveState->setVisible( m_isFloating );
  }
}

void GuiChat::setupToolBar( QToolBar* bar )
{
  QAction* act;

  act = bar->addAction( QIcon( ":/images/font.png" ), tr( "Change font style" ), this, SLOT( selectFont() ) );
  act->setStatusTip( tr( "Select your favourite chat font style" ) );
  act = bar->addAction( QIcon( ":/images/font-color.png" ), tr( "Change font color" ), this, SLOT( selectFontColor() ) );
  act->setStatusTip( tr( "Select your favourite font color for the chat messages" ) );
  mp_actSelectBackgroundColor = bar->addAction( QIcon( ":/images/background-color.png" ), tr( "Change background color" ), this, SLOT( selectBackgroundColor() ) );
  mp_actSelectBackgroundColor->setStatusTip( tr( "Select your favourite background color for the chat window" ) );
  act = bar->addAction( QIcon( ":/images/filter.png" ), tr( "Filter message" ), this, SLOT( showChatMessageFilterMenu() ) );
  act->setStatusTip( tr( "Select the message types which will be showed in chat" ) );
  act = bar->addAction( QIcon( ":/images/settings.png" ), tr( "Chat settings" ), this, SIGNAL( showChatMenuRequest() ) );
  act->setStatusTip( tr( "Click to show the settings menu of the chat" ) );
  bar->addSeparator();

  mp_menuMembers = new QMenu( tr( "Members" ), this );
  mp_menuMembers->setStatusTip( tr( "Show the members of the chat" ) );
  mp_menuMembers->setIcon( QIcon( ":/images/group.png" ) );
  connect( mp_menuMembers->menuAction(), SIGNAL( triggered() ), this, SLOT( showMembersMenu() ) );
  bar->addAction( mp_menuMembers->menuAction() );
  bar->addSeparator();

  mp_actSendFile = bar->addAction( QIcon( ":/images/send-file.png" ), tr( "Send file" ), this, SLOT( sendFile() ) );
  mp_actSendFile->setStatusTip( tr( "Send a file to a user or a group" ) );
  act = bar->addAction( QIcon( ":/images/save-as.png" ), tr( "Save chat" ), this, SLOT( saveChat() ) );
  act->setStatusTip( tr( "Save the messages of the current chat to a file" ) );
  mp_actClear = bar->addAction( QIcon( ":/images/clear.png" ), tr( "Clear messages" ), this, SLOT( clearChat() ) );
  mp_actClear->setStatusTip( tr( "Clear all the messages of the chat" ) );
  bar->addSeparator();

  mp_actCreateGroupChat = bar->addAction( QIcon( ":/images/chat-create.png" ), tr( "Create chat" ), this, SIGNAL( createChatRequest() ) );
  mp_actCreateGroupChat->setStatusTip( tr( "Create a chat with two or more users" ) );
  mp_actGroupWizard = bar->addAction( QIcon( ":/images/group-wizard.png" ), tr( "Create group from chat" ), this, SLOT( showGroupWizard() ) );
  mp_actGroupWizard->setStatusTip( tr( "Create a group from this chat" ) );
  mp_actCreateGroup = bar->addAction( QIcon( ":/images/group-add.png" ), tr( "Create group" ), this, SIGNAL( createGroupRequest() ) );
  mp_actCreateGroup->setStatusTip( tr( "Create a group with two or more users" ) );
  mp_actGroupAdd = bar->addAction( QIcon( ":/images/group-edit.png" ), tr( "Edit group" ), this, SLOT( editChatMembers() ) );
  mp_actGroupAdd->setStatusTip( tr( "Change the name of the group or add and remove users" ) );
  mp_actLeave = bar->addAction( QIcon( ":/images/group-remove.png" ), tr( "Leave the group" ), this, SLOT( leaveThisGroup() ) );
  mp_actLeave->setStatusTip( tr( "Leave the group" ) );
}

void GuiChat::updateAction( bool is_connected, int connected_users )
{
  Chat c = ChatManager::instance().chat( m_chatId );
  if( !c.isValid() )
    qWarning() << "Invalid chat id" << m_chatId << "found in GuiChat::updateAction";

  bool local_user_is_member = isActiveUser( c, Settings::instance().localUser() );
  bool is_group_chat = c.isGroup();
  int user_in_list = UserManager::instance().userList().toList().size();
  mp_actSendFile->setEnabled( local_user_is_member && is_connected && connected_users > 0 );
  mp_actCreateGroup->setEnabled( is_connected && user_in_list > 1 );
  mp_actCreateGroupChat->setEnabled( is_connected && user_in_list > 1 );
  mp_actGroupAdd->setEnabled( local_user_is_member && is_connected && is_group_chat );
  mp_actLeave->setEnabled( local_user_is_member && is_connected && is_group_chat );
  mp_teMessage->setEnabled( is_connected );
  mp_pbSend->setEnabled( is_connected );
}

void GuiChat::customContextMenu( const QPoint& p )
{
  QMenu custom_context_menu;
  custom_context_menu.addAction( QIcon( ":/images/paste.png" ), tr( "Copy to clipboard" ), mp_teChat, SLOT( copy() ), QKeySequence::Copy );
  custom_context_menu.addSeparator();
  custom_context_menu.addAction( QIcon( ":/images/select-all.png" ), tr( "Select All" ), mp_teChat, SLOT( selectAll() ), QKeySequence::SelectAll );
  custom_context_menu.exec( mapToGlobal( p ) );
}

bool GuiChat::messageCanBeShowed( const ChatMessage& cm )
{
  if( m_chatId == ID_DEFAULT_CHAT && Settings::instance().showOnlyMessagesInDefaultChat() )
    return GuiChatMessage::messageCanBeShowedInDefaultChat( cm ) && !Settings::instance().chatMessageFilter().testBit( (int)cm.type() );
  else
    return !Settings::instance().chatMessageFilter().testBit( (int)cm.type() );
}

bool GuiChat::historyCanBeShowed()
{
  return !Settings::instance().chatMessageFilter().testBit( (int)ChatMessage::History );
}

void GuiChat::showChatMessageFilterMenu()
{
  QMenu filter_menu;
  QAction* act;

  act = filter_menu.addAction( tr( "Show only messages in default chat" ), this, SLOT( changeChatMessageFilter() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().showOnlyMessagesInDefaultChat() );
  act->setData( (int)ChatMessage::NumTypes );
  filter_menu.addSeparator();

  for( int i = ChatMessage::System; i < ChatMessage::NumTypes; i++ )
  {
    act = filter_menu.addAction( Bee::chatMessageTypeToString( i ), this, SLOT( changeChatMessageFilter() ) );
    act->setCheckable( true );
    act->setChecked( !Settings::instance().chatMessageFilter().testBit( i ) );
    act->setData( i );
  }

  filter_menu.exec( QCursor::pos() );
}

void GuiChat::changeChatMessageFilter()
{
  QAction* act = qobject_cast<QAction*>(sender());
  if( !act )
    return;

  if( act->data().toInt() == (int)ChatMessage::NumTypes )
  {
    Settings::instance().setShowOnlyMessagesInDefaultChat( act->isChecked() );
    if( m_chatId == ID_DEFAULT_CHAT )
      reloadChat();
    return;
  }

  QBitArray filter_array = Settings::instance().chatMessageFilter();
  filter_array.setBit( act->data().toInt(), !act->isChecked() );
  Settings::instance().setChatMessageFilter( filter_array );
  reloadChat();
}

void GuiChat::setLastMessageTimestamp( const QDateTime& dt )
{
  if( dt.isValid() && !Settings::instance().chatShowMessageTimestamp() )
    mp_lTimestamp->setText( QString( "  " ) + tr( "Last message %1" ).arg( dt.toString( "hh:mm" )) + QString( "  " ) );
  else
    mp_lTimestamp->setText( "" );
}

void GuiChat::sendMessage()
{
  emit newMessage( m_chatId, mp_teMessage->message() );
  mp_teMessage->clearMessage();
  ensureFocusInChat();
}

void GuiChat::checkWriting()
{
  emit writing( m_chatId );
}

void GuiChat::checkAnchorClicked( const QUrl& url )
{
  emit openUrl( url );
}

QString GuiChat::chatMessageToText( const ChatMessage& cm )
{
  QString s = "";

  if( !messageCanBeShowed( cm ) )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "This chat message is filtered out:" << cm.message();
#endif
    return s;
  }

  if( cm.isFromSystem() )
  {
    s = GuiChatMessage::formatSystemMessage( cm, false );
    m_lastMessageUserId = 0;
  }
  else
  {
    s = GuiChatMessage::formatMessage( findUser( cm.userId() ), cm, Settings::instance().showMessagesGroupByUser() ? m_lastMessageUserId : 0 );
    m_lastMessageUserId = cm.userId();
  }

  return s;
}

User GuiChat::findUser( VNumber user_id )
{
  User u = m_chatUsers.find( user_id );
  if( u.isValid() )
    return u;

  u = UserManager::instance().findUser( user_id );
  if( u.isValid() )
    m_chatUsers.set( u );

  return u;
}

bool GuiChat::hasUser( VNumber user_id )
{
  return m_chatUsers.find( user_id ).isValid();
}

void GuiChat::updateUser( const User& u )
{
  if( m_chatUsers.find( u.id() ).isValid() )
  {
    if( UserManager::instance().findUser( u.id() ).isValid() )
      m_chatUsers.set( u );
    else
      m_chatUsers.remove( u );
    setChatUsers();
  }
}

bool GuiChat::isActiveUser( const Chat& c, const User& u ) const
{
  return c.isValid() && c.usersId().contains( u.id() );
}

void GuiChat::setChatUsers()
{
  QString chat_users;
  mp_pbProfile->disconnect();
  mp_pbProfile->setToolTip( QString( "" ) );

  bool chat_has_members = false;
  Chat c = ChatManager::instance().chat( m_chatId );

  if( c.isDefault() )
  {
    chat_has_members = true;
    mp_pbProfile->setIcon( QIcon( ":images/default-chat-online.png" ) );
    chat_users = QString( "<b>%1</b>" ).arg( tr( "All Lan Users" ) ) ;
    mp_menuMembers->setEnabled( false );
    connect( mp_pbProfile, SIGNAL( clicked() ), this, SLOT( showLocalUserVCard() ) );
  }
  else
  {
    mp_menuMembers->setEnabled( true );
    mp_menuMembers->clear();
    QAction* act = 0;
    QStringList sl;
    m_chatUsers.sort();

    foreach( User u, m_chatUsers.toList() )
    {
      act = mp_menuMembers->addAction( QIcon( Bee::userStatusIcon( u.status() ) ), u.isLocal() ? tr( "You" ) : u.name() );
      act->setData( u.id() );
      act->setIconVisibleInMenu( true );
      if( u.isStatusConnected() && isActiveUser( c, u ) )
      {
        act->setEnabled( true  );
        connect( act, SIGNAL( triggered() ), this, SLOT( showUserVCard() ) );
      }
      else
        act->setEnabled( false );

      if( u.isLocal() )
      {
        if( !isActiveUser( c, u ) )
          sl.append( tr( "(You have left)" ).toLower() );
        else if( u.isStatusConnected() )
          sl.append( QString( "<b>%1</b>" ).arg( tr( "You" ) ).toLower() );
        else
          sl.append( QString( "%1 [%2]" ).arg( u.name() ).arg( tr( "offline" ) ) );
      }
      else
      {
        if( !chat_has_members )
          chat_has_members = true;

        if( !isActiveUser( c, u ) )
          sl.append( QString( "(%1 has left)" ).arg( u.name() ) );
        else if( u.isStatusConnected() )
          sl.append( QString( "<b>%1</b>" ).arg( u.name() ) );
        else
          sl.append( QString( "%1 [%2]" ).arg( u.name() ).arg( tr( "offline" ) ) );

        if( u.vCard().photo().isNull() )
          mp_pbProfile->setIcon( Avatar::create( u.name(), u.color(), QSize( mp_pbProfile->width()-1, mp_pbProfile->height()-1 ) ) );
        else
          mp_pbProfile->setIcon( u.vCard().photo() );

        connect( mp_pbProfile, SIGNAL( clicked() ), act, SIGNAL( triggered() ) );
        mp_pbProfile->setToolTip( tr( "Show profile" ) );
      }
    }

    if( c.isGroup() )
    {
      chat_users = QString( "<b>%1</b>" ).arg( m_chatName );
      mp_pbProfile->setIcon( QIcon( ":/images/group.png" ) );
      mp_pbProfile->disconnect();
      connect( mp_pbProfile, SIGNAL( clicked() ), this, SLOT( showMembersMenu() ) );
      mp_pbProfile->setToolTip( tr( "Show members" ) );
    }
    else
    {
      chat_users = sl.size() == 0 ? tr( "Nobody" ) : (c.isPrivate() ? sl.join( QString( " %1 " ).arg( tr( "and" ) ) ) : sl.join( ", " ) );
    }
  }

#ifdef BEEBEEP_DEBUG
  qDebug() << "Chat members:" << m_chatUsers.toStringList( false, false ).join( ", " );
#endif

  mp_lTitle->setText( chat_users );
  mp_teMessage->setEnabled( isActiveUser( c, Settings::instance().localUser() ) && chat_has_members );
}

void GuiChat::reloadChatUsers()
{
  Chat c = ChatManager::instance().chat( m_chatId );
  if( !c.isValid() )
    return;
  m_chatUsers = UserManager::instance().userList().fromUsersId( c.usersId() );
  setChatUsers();
}

bool GuiChat::setChatId( VNumber chat_id, bool is_floating )
{
  Chat c = ChatManager::instance().chat( chat_id );
  if( !c.isValid() )
    return false;

#ifdef BEEBEEP_DEBUG
  qDebug() << "Setting chat" << chat_id << "in default chat window";
#endif

  QApplication::setOverrideCursor( Qt::WaitCursor );

  if( c.unreadMessages() )
  {
    c.readAllMessages();
    ChatManager::instance().setChat( c );
  }

  m_chatId = c.id();
  m_chatName = c.name();
  m_chatUsers = UserManager::instance().userList().fromUsersId( c.usersId() );
  m_isFloating = is_floating;

  if( c.isDefault() )
  {
    setChatBackgroundColor( Settings::instance().defaultChatBackgroundColor() );
    mp_actSelectBackgroundColor->setEnabled( true );
  }
  else
  {
    mp_teChat->setPalette( m_defaultChatPalette );
    mp_actSelectBackgroundColor->setEnabled( false );
  }

  enableDetachButtons();

  mp_actGroupWizard->setEnabled( c.isGroup() && !UserManager::instance().hasGroupName( c.name() ) );
  bool chat_has_history = ChatManager::instance().chatHasSavedText( c.name() );
  bool chat_is_empty = c.isEmpty() && !chat_has_history;
  QString html_text = "";

  if( ChatManager::instance().isLoadHistoryCompleted() && chat_has_history && historyCanBeShowed() )
  {
    html_text += ChatManager::instance().chatSavedText( c.name() );
    if( !html_text.isEmpty() )
      html_text.append( "<br />" );
  }

  int num_lines = c.messages().size();
  bool max_lines_message_written = false;

  foreach( ChatMessage cm, c.messages() )
  {
    num_lines--;

    if( Settings::instance().chatMaxMessagesToShow() && num_lines > Settings::instance().chatMessagesToShow() )
    {
      if( !max_lines_message_written )
      {
        html_text += QString( "&nbsp;&nbsp;&nbsp;<font color=gray><i>... %1 ...</i></font><br /><br />" ).arg( tr( "last %1 messages" ).arg( Settings::instance().chatMessagesToShow() ) );
        max_lines_message_written = true;
      }
      continue;
    }
    else
      html_text += chatMessageToText( cm );
  }

#ifdef BEEBEEP_DEBUG
  QTime time_to_open;
  time_to_open.start();
#endif

  bool updates_is_enabled = mp_teChat->updatesEnabled();
  mp_teChat->setUpdatesEnabled( false );
  mp_teChat->setHtml( html_text );
  mp_teChat->setUpdatesEnabled( updates_is_enabled );

#ifdef BEEBEEP_DEBUG
  qDebug() << "Elapsed time to set HTML text in chat:" << time_to_open.elapsed() << "ms";
#endif

  mp_actClear->setDisabled( chat_is_empty );
  ensureLastMessageVisible();
  setLastMessageTimestamp( c.lastMessageTimestamp() );
  setChatUsers();

  QApplication::restoreOverrideCursor();
  return true;
}

void GuiChat::ensureFocusInChat()
{
  if( mp_teMessage->isEnabled() )
    mp_teMessage->setFocus();
}

void GuiChat::ensureLastMessageVisible()
{
  QScrollBar *bar = mp_teChat->verticalScrollBar();
  if( bar )
  {
    if( !bar->isSliderDown() )
      bar->setValue( bar->maximum() );
  }
  else
    mp_teChat->ensureCursorVisible();
}

void GuiChat::appendChatMessage( VNumber chat_id, const ChatMessage& cm )
{
  if( m_chatId != chat_id )
  {
    qWarning() << "Trying to append chat message of chat id" << chat_id << "in chat shown with id" << m_chatId << "... skip it";
    return;
  }

  Chat c = ChatManager::instance().chat( m_chatId );
  if( !c.isValid() )
    return;

  bool show_timestamp_last_message = !cm.isFromLocalUser() && !cm.isFromSystem();
  mp_actClear->setDisabled( c.isEmpty() && !ChatManager::instance().chatHasSavedText( c.name() ) );

  User u = m_chatUsers.find( cm.userId() );
  if( !u.isValid() )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "User" << cm.userId() << "is not present in current chat" << m_chatId << "... force update";
#endif
    m_chatUsers = UserManager::instance().userList().fromUsersId( c.usersId() );
    u = m_chatUsers.find( cm.userId() );
    if( !u.isValid() )
    {
      qWarning() << "User" << cm.userId() << "is not present in current chat" << m_chatId << "... message is not shown";
      return;
    }
    setChatUsers();
  }

  QString text_message = chatMessageToText( cm );

  if( !text_message.isEmpty() )
  {
#ifdef BEEBEEP_DEBUG
    QTime time_to_insert;
    time_to_insert.start();
#endif
    bool updates_enabled = mp_teChat->updatesEnabled();
    mp_teChat->setUpdatesEnabled( false );
    QTextCursor cursor( mp_teChat->textCursor() );
    cursor.movePosition( QTextCursor::End );
    cursor.insertHtml( text_message );
    mp_teChat->setUpdatesEnabled( updates_enabled );
#ifdef BEEBEEP_DEBUG
    qDebug() << "Elapsed time to insert HTML text in chat:" << time_to_insert.elapsed() << "ms";
#endif
    ensureLastMessageVisible();
  }

  if( show_timestamp_last_message )
    setLastMessageTimestamp( cm.timestamp() );
}

void GuiChat::setChatFont( const QFont& f )
{
  mp_teChat->setFont( f );
  mp_teMessage->setFont( f );
}

void GuiChat::selectFont()
{
  bool ok = false;
  QFont f = QFontDialog::getFont( &ok, Settings::instance().chatFont(), this );
  if( ok )
  {
    Settings::instance().setChatFont( f );
    setChatFont( f );
  }
}

void GuiChat::setChatFontColor( const QString& color_name )
{
  mp_teMessage->setTextColor( QColor( color_name ) );
}

void GuiChat::setChatBackgroundColor( const QString& color_name )
{
  QPalette pal = m_defaultChatPalette;
  pal.setBrush( QPalette::Base, QBrush( QColor( color_name ) ) );
  mp_teChat->setPalette( pal );
}

void GuiChat::selectFontColor()
{
  QColor c = QColorDialog::getColor( QColor( Settings::instance().chatFontColor() ), this );
  if( c.isValid() )
  {
    Settings::instance().setDefaultChatBackgroundColor( c.name() );
    setChatFontColor( c.name() );
  }
}

void GuiChat::selectBackgroundColor()
{
  QColor c = QColorDialog::getColor( QColor( Settings::instance().defaultChatBackgroundColor() ), this );
  if( c.isValid() )
  {
    Settings::instance().setDefaultChatBackgroundColor( c.name() );
    setChatBackgroundColor( c.name() );
  }
}

void GuiChat::addEmoticon( const Emoticon& e )
{
  mp_teMessage->addEmoticon( e );
  ensureFocusInChat();
}

void GuiChat::saveChat()
{
  QString file_name = QFileDialog::getSaveFileName( this,
                        tr( "Please select a file to save the messages of the chat." ),
                        Settings::instance().chatSaveDirectory(), "PDF Chat Files (*.pdf)" );
  if( file_name.isEmpty() )
    return;

  QPrinter printer;
  printer.setOutputFormat( QPrinter::PdfFormat );
  printer.setOutputFileName( file_name );
  QTextDocument *doc = mp_teChat->document();
  doc->print( &printer );

  QFileInfo file_info( file_name );
  Settings::instance().setChatSaveDirectory( file_info.absolutePath() );

  QMessageBox::information( this, Settings::instance().programName(), tr( "%1: save completed." ).arg( file_name ), tr( "Ok" ) );
}

void GuiChat::clearChat()
{
  emit chatToClear( m_chatId );
}

void GuiChat::leaveThisGroup()
{
  emit leaveThisChat( m_chatId );
}

void GuiChat::sendFile()
{
  emit sendFileFromChatRequest( m_chatId, QString( "" ) );
}

void GuiChat::dragEnterEvent( QDragEnterEvent *event )
{
  if( event->mimeData()->hasUrls() )
    event->acceptProposedAction();
}

void GuiChat::checkAndSendImage( const QMimeData* source )
{
  if( !source->hasImage() )
    return;

  QString image_format = "png";
  QString image_initial_path = Settings::instance().dataFolder() +
                                    QString( "/beeimgtmp-%1." ).arg( Bee::dateTimeStringSuffix( QDateTime::currentDateTime() ) )
                                    + image_format;
  QString file_path = Bee::uniqueFilePath( image_initial_path );
  qDebug() << "Sending the dropped image as a file:" << file_path;

  QImage image = qvariant_cast<QImage>( source->imageData() );

  if( !image.save( file_path, image_format.toLatin1() ) )
  {
    QMessageBox::warning( this, Settings::instance().programName(),
      tr( "Unable to save temporary file: %1" ).arg( file_path ) );
    return;
  }

  Settings::instance().addTemporaryFilePath( file_path );

  emit sendFileFromChatRequest( m_chatId, file_path );
}


void GuiChat::checkAndSendUrls( const QMimeData* source )
{
  if( !source->hasUrls() )
    return;

  QStringList file_path_list;
  QString file_path;
  int num_files = 0;

  foreach( QUrl url, source->urls() )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Checking pasted url:" << qPrintable( url.toString() );
#endif

#if QT_VERSION >= 0x040800
    if( url.isLocalFile() )
#else
    if( url.scheme() == QLatin1String( "file" ) )
#endif
    {
      file_path = url.toLocalFile();
      num_files += Protocol::instance().countFilesCanBeSharedInPath( file_path );
      if( num_files > Settings::instance().maxQueuedDownloads() )
        break;
      file_path_list.append( file_path );
    }
    else
      mp_teMessage->addPasted( url.toString() );
  }

  if( num_files <= 0 )
    return;

  num_files = qMin( num_files, Settings::instance().maxQueuedDownloads() );

  if( QMessageBox::question( this, Settings::instance().programName(),
                             tr( "Do you really want to send %1 %2 to the members of this chat?" ).arg( num_files )
                             .arg( num_files == 1 ? tr( "file" ) : tr( "files" ) ),
                             tr( "Yes" ), tr( "No" ), QString(), 0, 1 ) == 1 )
  {
    return;
  }

  foreach( QString local_file, file_path_list )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Drag and drop: send file" << local_file << "to chat" << m_chatId;
#endif
    if( !QFile::exists( local_file ) )
    {
      QMessageBox::information( this, Settings::instance().programName(),
                                tr( "Qt library for this OS doesn't support Drag and Drop for files. You have to select again the file to send." ) );
      qWarning() << "Drag and drop has invalid file path" << local_file;
      return;
    }

    emit sendFileFromChatRequest( m_chatId, local_file );
  }
}

void GuiChat::dropEvent( QDropEvent *event )
{
  if( event->mimeData()->hasUrls() )
    checkAndSendUrls( event->mimeData() );
}

void GuiChat::showUserVCard()
{
  QAction *act = qobject_cast<QAction*>( sender() );
  if( !act )
    return;
  VNumber user_id = Bee::qVariantToVNumber( act->data() );
  emit showVCardRequest( user_id, false );
}

void GuiChat::showMembersMenu()
{
  mp_menuMembers->exec( QCursor::pos() );
}

void GuiChat::showLocalUserVCard()
{
  emit showVCardRequest( ID_LOCAL_USER, false );
}

void GuiChat::showGroupWizard()
{
  mp_actGroupWizard->setEnabled( false );
  emit createGroupFromChatRequest( m_chatId );
}

void GuiChat::detachThisChat()
{
  emit detachChatRequest( m_chatId );
}

void GuiChat::editChatMembers()
{
  emit editGroupRequestFromChat( m_chatId );
}

void GuiChat::updateShortcuts()
{
  QKeySequence ks = ShortcutManager::instance().shortcut( ShortcutManager::SetFocusInMessageBox );
  if( !ks.isEmpty() )
  {
    mp_scFocusInChat->setKey( ks );
    mp_scFocusInChat->setEnabled( Settings::instance().useShortcuts() );
  }
  else
    mp_scFocusInChat->setEnabled( false );

  ks = ShortcutManager::instance().shortcut( ShortcutManager::SendFile );
  if( !ks.isEmpty() && Settings::instance().useShortcuts() )
    mp_actSendFile->setShortcut( ks );
  else
    mp_actSendFile->setShortcut( QKeySequence() );
}
