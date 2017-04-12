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
#include "FileDialog.h"
#include "GuiChat.h"
#include "GuiChatMessage.h"
#include "Protocol.h"
#include "Settings.h"
#include "ShortcutManager.h"
#ifdef BEEBEEP_USE_HUNSPELL
  #include "SpellChecker.h"
#endif
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
  grid_layout->setContentsMargins( 4, 4, 4, 4 );

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
#ifdef BEEBEEP_USE_HUNSPELL
  mp_teMessage->setCompleter( SpellChecker::instance().completer() );
#endif

  mp_teChat->setObjectName( "GuiChatViewer" );
  m_defaultChatPalette = mp_teChat->palette();
  m_defaultChatPalette.setColor( QPalette::Highlight, Qt::yellow );
  m_defaultChatPalette.setColor( QPalette::HighlightedText, Qt::black );
  mp_teChat->setPalette( m_defaultChatPalette );

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
  m_lastTextFound = "";

  mp_menuContext = new QMenu( this );
  mp_menuFilters = new QMenu( this );

  mp_menuMembers = new QMenu( tr( "Members" ), this );
  mp_menuMembers->setIcon( QIcon( ":/images/group.png" ) );
  connect( mp_menuMembers->menuAction(), SIGNAL( triggered() ), this, SLOT( showMembersMenu() ) );

  mp_scFocusInChat = new QShortcut( this );
  mp_scFocusInChat->setContext( Qt::WindowShortcut );
  connect( mp_scFocusInChat, SIGNAL( activated() ), this, SLOT( ensureFocusInChat() ) );

  mp_scFindNextTextInChat = new QShortcut( this );
  mp_scFindNextTextInChat->setContext( Qt::WindowShortcut );
  connect( mp_scFindNextTextInChat, SIGNAL( activated() ), this, SLOT( findNextTextInChat() ) );

  mp_scViewEmoticons = new QShortcut( this );
  mp_scViewEmoticons->setContext( Qt::WindowShortcut );
  connect( mp_scViewEmoticons, SIGNAL( activated() ), this, SIGNAL( toggleVisibilityEmoticonsPanelRequest() ) );

  mp_actSelectBackgroundColor = new QAction( QIcon( ":/images/background-color.png" ), tr( "Change background color" ), this );
  connect( mp_actSelectBackgroundColor, SIGNAL( triggered() ), this, SLOT( selectBackgroundColor() ) );

  mp_actSaveAs = new QAction( QIcon( ":/images/save-as.png" ), tr( "Save chat" ), this );
  connect( mp_actSaveAs, SIGNAL( triggered() ), this, SLOT( saveChat() ) );

  mp_actPrint = new QAction( QIcon( ":/images/printer.png" ), tr( "Print..." ), this );
  mp_actPrint->setShortcut( QKeySequence::Print );
  connect( mp_actPrint, SIGNAL( triggered() ), this, SLOT( printChat() ) );

  mp_actClear = new QAction( QIcon( ":/images/clear.png" ), tr( "Clear messages" ), this );
  connect( mp_actClear, SIGNAL( triggered() ), this, SLOT( clearChat() ) );

  mp_actFindTextInChat = new QAction( QIcon( ":/images/search.png" ), tr( "Find text in chat" ), this );
  connect( mp_actFindTextInChat, SIGNAL( triggered() ), this, SLOT( showFindTextInChatDialog() ) );

  connect( mp_teChat, SIGNAL( customContextMenuRequested( const QPoint& ) ), this, SLOT( customContextMenu( const QPoint& ) ) );
  connect( mp_teChat, SIGNAL( anchorClicked( const QUrl& ) ), this, SLOT( checkAnchorClicked( const QUrl&  ) ) );
  connect( mp_teMessage, SIGNAL( returnPressed() ), this, SLOT( sendMessage() ) );
  connect( mp_teMessage, SIGNAL( writing() ), this, SLOT( checkWriting() ) );
  connect( mp_teMessage, SIGNAL( urlsToCheck( const QMimeData* ) ), this, SLOT( checkAndSendUrls( const QMimeData* ) ) );
  connect( mp_teMessage, SIGNAL( imageToCheck( const QMimeData* ) ), this, SLOT( checkAndSendImage( const QMimeData* ) ) );
  connect( mp_pbSend, SIGNAL( clicked() ), this, SLOT( sendMessage() ) );
  connect( mp_pbSaveState, SIGNAL( clicked() ), this, SIGNAL( saveStateAndGeometryRequest() ) );
}

void GuiChat::setupToolBar( QToolBar* bar )
{
  bar->addAction( QIcon( ":/images/font.png" ), tr( "Change font style" ), this, SLOT( selectFont() ) );
  bar->addAction( QIcon( ":/images/font-color.png" ), tr( "Change font color" ), this, SLOT( selectFontColor() ) );
  bar->addAction( QIcon( ":/images/filter.png" ), tr( "Filter message" ), this, SLOT( showChatMessageFilterMenu() ) );
  bar->addAction( QIcon( ":/images/settings.png" ), tr( "Chat settings" ), this, SIGNAL( showChatMenuRequest() ) );
  mp_actSpellChecker = bar->addAction( QIcon( ":/images/spellchecker.png" ), tr( "Spell checking" ), this, SLOT( onSpellCheckerActionClicked() ) );
  mp_actSpellChecker->setCheckable( true );
  mp_actCompleter = bar->addAction( QIcon( ":/images/dictionary.png" ), tr( "Word completer" ), this, SLOT( onCompleterActionClicked() ) );
  mp_actCompleter->setCheckable( true );
  mp_actUseReturnToSendMessage = bar->addAction( QIcon( ":/images/key-return.png" ), tr( "Use Return key to send message" ), this, SLOT( onUseReturnToSendMessageClicked() ) );
  mp_actUseReturnToSendMessage->setCheckable( true );
  updateActionsOnFocusChanged();
  bar->addSeparator();

  bar->addAction( mp_menuMembers->menuAction() );
  bar->addSeparator();
  mp_actSendFile = bar->addAction( QIcon( ":/images/send-file.png" ), tr( "Send file" ), this, SLOT( sendFile() ) );
  mp_actSendFolder = bar->addAction( QIcon( ":/images/send-folder.png" ), tr( "Send folder" ), this, SLOT( sendFolder() ) );
  bar->addSeparator();
  mp_actGroupWizard = bar->addAction( QIcon( ":/images/group-wizard.png" ), tr( "Create group from chat" ), this, SLOT( showGroupWizard() ) );
  mp_actGroupAdd = bar->addAction( QIcon( ":/images/group-edit.png" ), tr( "Edit group" ), this, SLOT( editChatMembers() ) );
  mp_actLeave = bar->addAction( QIcon( ":/images/group-remove.png" ), tr( "Leave the group" ), this, SLOT( leaveThisGroup() ) );
  mp_teMessage->addActionToContextMenu( mp_actSendFile );
  mp_teMessage->addActionToContextMenu( mp_actSendFolder );
}

void GuiChat::updateActions( bool is_connected, int connected_users )
{
  Chat c = ChatManager::instance().chat( m_chatId );
  if( !c.isValid() )
    qWarning() << "Invalid chat id" << m_chatId << "found in GuiChat::updateAction";

  bool local_user_is_member = isActiveUser( c, Settings::instance().localUser() );
  bool is_group_chat = c.isGroup();

  mp_actSendFile->setEnabled( Settings::instance().fileTransferIsEnabled() && local_user_is_member && is_connected && connected_users > 0 );
  mp_actSendFolder->setEnabled( Settings::instance().fileTransferIsEnabled() && local_user_is_member && is_connected && connected_users > 0 );
  mp_actGroupAdd->setEnabled( local_user_is_member && is_connected && is_group_chat );
  mp_actLeave->setEnabled( local_user_is_member && is_connected && is_group_chat );

  if( !is_connected )
  {
    mp_teMessage->setEnabled( false );
    mp_pbSend->setEnabled( false );
    mp_teMessage->setToolTip( tr( "You are not connected" ) );
  }
  else
    checkChatDisabled( c );
}

void GuiChat::checkChatDisabled( const Chat& c )
{
  if( Settings::instance().disableSendMessage() )
  {
    mp_teMessage->setEnabled( false );
    mp_pbSend->setEnabled( false );
    mp_teMessage->setToolTip( tr( "Send messages is disabled" ) );
  }
  else if( c.isDefault() && !Settings::instance().chatWithAllUsersIsEnabled() )
  {
    mp_teMessage->setEnabled( false );
    mp_pbSend->setEnabled( false );
    mp_teMessage->setToolTip( tr( "Chat with all users is disabled" ) );
  }
  else if( c.isPrivate() && Settings::instance().disablePrivateChats() )
  {
    mp_teMessage->setEnabled( false );
    mp_pbSend->setEnabled( false );
    mp_teMessage->setToolTip( tr( "Private chat is disabled" ) );
  }
  else
  {
    bool local_user_is_active_in_chat = isActiveUser( c, Settings::instance().localUser() );
    mp_teMessage->setEnabled( local_user_is_active_in_chat );
    mp_pbSend->setEnabled( local_user_is_active_in_chat );
    if( local_user_is_active_in_chat )
      mp_teMessage->setToolTip( "" );
    else
      mp_teMessage->setToolTip( tr( "You have left this chat" ) );
  }
}

void GuiChat::customContextMenu( const QPoint& p )
{
  mp_menuContext->clear();

  if( mp_actSelectBackgroundColor->isEnabled() )
  {
    mp_menuContext->addAction( mp_actSelectBackgroundColor );
    mp_menuContext->addSeparator();
  }
  mp_menuContext->addAction( mp_actFindTextInChat );
  mp_menuContext->addSeparator();
  mp_menuContext->addAction( QIcon( ":/images/select-all.png" ), tr( "Select All" ), mp_teChat, SLOT( selectAll() ), QKeySequence::SelectAll );
  mp_menuContext->addSeparator();
  QAction* act = mp_menuContext->addAction( QIcon( ":/images/copy.png" ), tr( "Copy to clipboard" ), mp_teChat, SLOT( copy() ), QKeySequence::Copy );
  act->setEnabled( !mp_teChat->textCursor().selectedText().isEmpty() );
  act = mp_menuContext->addAction( QIcon( ":/images/connect.png" ), tr( "Open selected text as url" ), this, SLOT( openSelectedTextAsUrl() ) );
  act->setEnabled( !mp_teChat->textCursor().selectedText().isEmpty() );
  mp_menuContext->addSeparator();
  mp_menuContext->addAction( mp_actSaveAs );
  mp_menuContext->addAction( mp_actPrint );
  mp_menuContext->addSeparator();
  mp_menuContext->addAction( mp_actClear );
  mp_menuContext->addSeparator();
  mp_menuContext->addAction( mp_actSendFile );
  mp_menuContext->addAction( mp_actSendFolder );
  mp_menuContext->exec( mapToGlobal( p ) );
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
  mp_menuFilters->clear();
  QAction* act;

  act = mp_menuFilters->addAction( tr( "Show only messages in default chat" ), this, SLOT( changeChatMessageFilter() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().showOnlyMessagesInDefaultChat() );
  act->setData( (int)ChatMessage::NumTypes );
  mp_menuFilters->addSeparator();

  for( int i = ChatMessage::System; i < ChatMessage::NumTypes; i++ )
  {
    act = mp_menuFilters->addAction( Bee::chatMessageTypeToString( i ), this, SLOT( changeChatMessageFilter() ) );
    act->setCheckable( true );
    act->setChecked( !Settings::instance().chatMessageFilter().testBit( i ) );
    act->setData( i );
  }

  mp_menuFilters->exec( QCursor::pos() );
}

void GuiChat::changeChatMessageFilter()
{
  QAction* act = qobject_cast<QAction*>(sender());
  if( !act )
    return;

  if( act->data().toInt() == (int)ChatMessage::NumTypes )
  {
    Settings::instance().setShowOnlyMessagesInDefaultChat( act->isChecked() );
    if( m_chatId != ID_DEFAULT_CHAT )
      return;
  }
  else
  {
    QBitArray filter_array = Settings::instance().chatMessageFilter();
    filter_array.setBit( act->data().toInt(), !act->isChecked() );
    Settings::instance().setChatMessageFilter( filter_array );
  }

  updateChat();
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
  if( Settings::instance().disableSendMessage() )
    return;
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

QString GuiChat::chatMessageToText( const User& u, const ChatMessage& cm )
{
  QString s = "";

  if( !messageCanBeShowed( cm ) )
    return s;

  if( cm.isFromSystem() )
  {
    s = GuiChatMessage::formatSystemMessage( cm, Settings::instance().chatShowMessageTimestamp(), Settings::instance().chatShowMessageDatestamp() );
    m_lastMessageUserId = 0;
  }
  else
  {
    s = GuiChatMessage::formatMessage( u, cm, Settings::instance().showMessagesGroupByUser() ? m_lastMessageUserId : 0, Settings::instance().chatShowMessageTimestamp(), Settings::instance().chatShowMessageDatestamp() );
    m_lastMessageUserId = cm.userId();
  }

  return s;
}

bool GuiChat::isActiveUser( const Chat& c, const User& u ) const
{
  return c.isValid() && c.usersId().contains( u.id() );
}

void GuiChat::setChatTitle( const Chat& c )
{
  QString chat_title;
  mp_pbProfile->disconnect();
  mp_pbProfile->setToolTip( QString( "" ) );

  if( c.isDefault() )
  {
    mp_pbProfile->setIcon( QIcon( ":images/default-chat-online.png" ) );
    chat_title = QString( "<b>%1</b>" ).arg( tr( "All Lan Users" ) ) ;
  }
  else if( c.isPrivate() )
  {
    User u = UserManager::instance().findUser( c.privateUserId() );
    QPixmap user_avatar = Bee::avatarForUser( u, QSize( mp_pbProfile->width()-1, mp_pbProfile->height()-1 ), Settings::instance().showUserPhoto() );
    mp_pbProfile->setIcon( user_avatar );
    if( !u.isValid() )
    {
      chat_title = tr( "Unknown" );
      qWarning() << "Invalid user" << c.privateUserId() << "found in GuiChat::setChatTitle(...)";
    }
    else
    {
      chat_title = QString( "<b>%1</b>" ).arg( u.name() ) ;
      mp_pbProfile->setToolTip( QString( "%1\n(%2)" ).arg( Bee::toolTipForUser( u, false ) ).arg( tr( "Click here to show the user profile" ) ) );
      connect( mp_pbProfile, SIGNAL( clicked() ), this, SLOT( showUserVCard() ) );
    }
  }
  else
  {
    chat_title = QString( "<b>%1</b>" ).arg( c.name() );
    mp_pbProfile->setIcon( QIcon( ":/images/group.png" ) );
  }

  mp_lTitle->setText( chat_title );
}

void GuiChat::updateChat()
{
  Chat c = ChatManager::instance().chat( m_chatId );
  if( !c.isValid() )
  {
    qWarning() << "Invalid chat" << m_chatId << "found in GuiChat::updateChat(...)";
    return;
  }
  setChat( c );
}

bool GuiChat::setChat( const Chat& c )
{
  if( !c.isValid() )
    return false;

  QApplication::setOverrideCursor( Qt::WaitCursor );

  m_chatId = c.id();
  setChatTitle( c );

  if( c.isDefault() )
  {
    setChatBackgroundColor( Settings::instance().defaultChatBackgroundColor() );
    mp_actSelectBackgroundColor->setEnabled( true );
    mp_menuMembers->setEnabled( false );
  }
  else
  {
    mp_teChat->setPalette( m_defaultChatPalette );
    mp_actSelectBackgroundColor->setEnabled( false );
    mp_menuMembers->setEnabled( true );
  }

  mp_actGroupWizard->setEnabled( c.isGroup() && !UserManager::instance().hasGroupName( c.name() ) );

  QString html_text = "";

  if( ChatManager::instance().isLoadHistoryCompleted() && historyCanBeShowed() )
  {
    if( !ChatManager::instance().chatHasSavedText( c.name() ) )
    {
      if( c.isPrivate() && c.name().contains( "@" ) && ChatManager::instance().chatHasSavedText( User::nameFromPath( c.name() ) ) )
        html_text += ChatManager::instance().chatSavedText( User::nameFromPath( c.name() ) );
    }
    else
      html_text += ChatManager::instance().chatSavedText( c.name() );

    if( !html_text.isEmpty() )
      html_text.append( "<br />" );
  }

  bool chat_is_empty = c.isEmpty() && html_text.isEmpty();
  int num_lines = c.messages().size();
  bool max_lines_message_written = false;
  m_lastMessageUserId = 0;
  UserList chat_members = UserManager::instance().userList().fromUsersId( c.usersId() );

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
      html_text += chatMessageToText( chat_members.find( cm.userId() ), cm );
  }

#ifdef BEEBEEP_DEBUG
  QTime time_to_open;
  time_to_open.start();
#endif

  bool updates_is_enabled = mp_teChat->updatesEnabled();
  mp_teChat->setUpdatesEnabled( false );
  mp_teChat->clear();

  QTextDocument *text_document = mp_teChat->document();
  QTextOption text_option = text_document->defaultTextOption();
  text_option.setTextDirection( Settings::instance().showTextInModeRTL() ? Qt::RightToLeft : Qt::LeftToRight );
  text_document->setDefaultTextOption( text_option );
  mp_teChat->setDocument( text_document );

  text_document = mp_teMessage->document();
  text_option = text_document->defaultTextOption();
  text_option.setTextDirection( Settings::instance().showTextInModeRTL() ? Qt::RightToLeft : Qt::LeftToRight );
  text_document->setDefaultTextOption( text_option );
  mp_teMessage->setDocument( text_document );

  mp_teChat->setHtml( html_text );
  mp_teChat->setUpdatesEnabled( updates_is_enabled );

#ifdef BEEBEEP_DEBUG
  qDebug() << "Elapsed time to set HTML text in chat:" << time_to_open.elapsed() << "ms";
#endif

  updateMenuMembers( c );
  mp_actClear->setDisabled( chat_is_empty );
  ensureLastMessageVisible();
  setLastMessageTimestamp( c.lastMessageTimestamp() );
  checkChatDisabled( c );

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
  {
    qWarning() << "Invalid chat" << m_chatId << "found in GuiChat::appendChatMessage(...)";
    return;
  }

  bool show_timestamp_last_message = !cm.isFromLocalUser() && !cm.isFromSystem();
  mp_actClear->setDisabled( c.isEmpty() && !ChatManager::instance().chatHasSavedText( c.name() ) );

  User u = UserManager::instance().findUser( cm.userId() );
  if( !u.isValid() )
  {
    qWarning() << "Invalid user" << cm.userId() << "found in GuiChat::appendChatMessage(...)";
    return;
  }

  QString text_message = chatMessageToText( u, cm );

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
    Settings::instance().setChatFontColor( c.name() );
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

void GuiChat::addText( const QString& txt )
{
  mp_teMessage->addText( txt );
  ensureFocusInChat();
}

void GuiChat::saveChat()
{
  QString file_name = FileDialog::getSaveFileName( this,
                        tr( "Please select a file to save the messages of the chat." ),
                        Settings::instance().dataFolder(), "PDF Chat Files (*.pdf)" );
  if( file_name.isEmpty() )
    return;

  if( !file_name.toLower().endsWith( QLatin1String( ".pdf" ) ) )
    file_name.append( QLatin1String( ".pdf" ) );

  QPrinter printer;
  printer.setOutputFormat( QPrinter::PdfFormat );
  printer.setOutputFileName( file_name );
  QTextDocument *doc = mp_teChat->document();
  doc->print( &printer );

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

void GuiChat::sendFolder()
{
  QString folder_selected = FileDialog::getExistingDirectory( this, Settings::instance().programName(),
                                                              Settings::instance().lastDirectorySelected() );
  if( folder_selected.isEmpty() )
    return;

  emit sendFileFromChatRequest( m_chatId, folder_selected );
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
  }

  if( num_files <= 0 )
  {
    mp_teMessage->addPasted( source );
    return;
  }

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
  if( act )
  {
    VNumber user_id = Bee::qVariantToVNumber( act->data() );
    emit showVCardRequest( user_id, false );
  }
  else
    showMembersMenu();
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

  ks = ShortcutManager::instance().shortcut( ShortcutManager::FindNextTextInChat );
  if( !ks.isEmpty() )
  {
    mp_scFindNextTextInChat->setKey( ks );
    mp_scFindNextTextInChat->setEnabled( Settings::instance().useShortcuts() );
  }
  else
    mp_scFindNextTextInChat->setEnabled( false );

  ks = ShortcutManager::instance().shortcut( ShortcutManager::FindTextInChat );
  if( !ks.isEmpty() && Settings::instance().useShortcuts() )
    mp_actFindTextInChat->setShortcut( ks );
  else
    mp_actFindTextInChat->setShortcut( QKeySequence() );

  ks = ShortcutManager::instance().shortcut( ShortcutManager::SendFile );
  if( !ks.isEmpty() && Settings::instance().useShortcuts() )
    mp_actSendFile->setShortcut( ks );
  else
    mp_actSendFile->setShortcut( QKeySequence() );

  ks = ShortcutManager::instance().shortcut( ShortcutManager::SendFolder );
  if( !ks.isEmpty() && Settings::instance().useShortcuts() )
    mp_actSendFolder->setShortcut( ks );
  else
    mp_actSendFolder->setShortcut( QKeySequence() );

  ks = ShortcutManager::instance().shortcut( ShortcutManager::SendChatMessage );
  if( !ks.isEmpty() && Settings::instance().useShortcuts() )
    mp_pbSend->setShortcut( ks );
  else
    mp_pbSend->setShortcut( QKeySequence() );

  ks = ShortcutManager::instance().shortcut( ShortcutManager::Print );
  if( !ks.isEmpty() && Settings::instance().useShortcuts() )
    mp_actPrint->setShortcut( ks );
  else
    mp_actPrint->setShortcut( QKeySequence() );

  ks = ShortcutManager::instance().shortcut( ShortcutManager::ShowEmoticons );
  if( !ks.isEmpty() )
  {
    mp_scViewEmoticons->setKey( ks );
    mp_scViewEmoticons->setEnabled( Settings::instance().useShortcuts() );
  }
  else
    mp_scViewEmoticons->setEnabled( false );

}

void GuiChat::updateUseReturnKeyToSendMessageToolTip()
{
  if( Settings::instance().useReturnToSendMessage() )
    mp_actUseReturnToSendMessage->setToolTip( tr( "Use key Return to send message" ) );
  else
    mp_actUseReturnToSendMessage->setToolTip( tr( "Use key Return to make a carriage return" ) );
}

void GuiChat::onUseReturnToSendMessageClicked()
{
  Settings::instance().setUseReturnToSendMessage( mp_actUseReturnToSendMessage->isChecked() );
  updateUseReturnKeyToSendMessageToolTip();
  ensureFocusInChat();
}

void GuiChat::updateSpellCheckerToolTip()
{
  QString tool_tip = "";
  if( Settings::instance().useSpellChecker() )
    tool_tip = tr( "Spell checking is enabled" );
  else
    tool_tip = tr( "Spell checking is disabled" );

#ifdef BEEBEEP_USE_HUNSPELL
  if( !SpellChecker::instance().isValid() )
  {
    tool_tip.append( QString( " (%1)" ).arg( tr( "There is not a valid dictionary" ) ) );
    mp_actSpellChecker->setEnabled( false );
  }
  else
    mp_actSpellChecker->setEnabled( true );
#else
  mp_actSpellChecker->setEnabled( false );
#endif

  mp_actSpellChecker->setToolTip( tool_tip );
}

void GuiChat::updateCompleterToolTip()
{
  QString tool_tip = "";

  if( Settings::instance().useWordCompleter() )
    tool_tip = tr( "Word completer is enabled" );
  else
    tool_tip =  tr( "Word completer is disabled" );

#ifdef BEEBEEP_USE_HUNSPELL
  if( !SpellChecker::instance().isValid() )
  {
    tool_tip.append( QString( " (%1)" ).arg( tr( "There is not a valid dictionary" ) ) );
    mp_actCompleter->setEnabled( false );
  }
  else
    mp_actCompleter->setEnabled( true );
#else
  mp_actCompleter->setEnabled( false );
#endif

  mp_actCompleter->setToolTip( tool_tip );
}

void GuiChat::onSpellCheckerActionClicked()
{
#ifdef BEEBEEP_USE_HUNSPELL
  if( SpellChecker::instance().isValid() )
    Settings::instance().setUseSpellChecker( mp_actSpellChecker->isChecked() );
  else
    Settings::instance().setUseSpellChecker( false );
#else
  Settings::instance().setUseSpellChecker( mp_actSpellChecker->isChecked() );
#endif
  updateSpellCheckerToolTip();
  mp_teMessage->rehighlightMessage();
  ensureFocusInChat();
}

void GuiChat::onCompleterActionClicked()
{
#ifdef BEEBEEP_USE_HUNSPELL
  if( SpellChecker::instance().isValid() )
    Settings::instance().setUseWordCompleter( mp_actCompleter->isChecked() );
  else
    Settings::instance().setUseWordCompleter( false );
#else
  Settings::instance().setUseWordCompleter( mp_actCompleter->isChecked() );
#endif
  updateCompleterToolTip();
  ensureFocusInChat();
}

void GuiChat::updateActionsOnFocusChanged()
{
  mp_actUseReturnToSendMessage->setChecked( Settings::instance().useReturnToSendMessage() );
  updateUseReturnKeyToSendMessageToolTip();
  mp_actSpellChecker->setChecked( Settings::instance().useSpellChecker() );
  updateSpellCheckerToolTip();
  mp_actCompleter->setChecked( Settings::instance().useWordCompleter() );
  updateCompleterToolTip();
}

void GuiChat::printChat()
{
  QPrinter printer( QPrinter::HighResolution );
  printer.setFullPage( true );
  QPrintDialog *dlg = new QPrintDialog( &printer, this );
  dlg->setOptions( QAbstractPrintDialog::PrintSelection | QAbstractPrintDialog::PrintPageRange |
                   QAbstractPrintDialog::PrintShowPageSize |  QAbstractPrintDialog::PrintCollateCopies |
#if QT_VERSION >= 0x040700
                   QAbstractPrintDialog::PrintCurrentPage |
#endif
                   QAbstractPrintDialog::PrintToFile );

  if( dlg->exec() == QDialog::Accepted)
    mp_teChat->print( dlg->printer() );

  dlg->deleteLater();
}

void GuiChat::showFindTextInChatDialog()
{
  QString label = tr( "Find text in chat" );
  bool ok = false;
  QString text_to_search = QInputDialog::getText( this, Settings::instance().programName(), label,
                                                  QLineEdit::Normal, m_lastTextFound, &ok );
  if( ok )
    findTextInChat( text_to_search.simplified() );
}

void GuiChat::findNextTextInChat()
{
  findTextInChat( m_lastTextFound );
}

void GuiChat::findTextInChat( const QString& txt )
{
  if( txt.isEmpty() )
    return;

  QTextDocument::FindFlags find_flags = 0;
  bool search_from_start = false;
  if( txt != m_lastTextFound )
  {
    mp_teChat->moveCursor( QTextCursor::Start );
    search_from_start = true;
  }

  if( !mp_teChat->find( txt, find_flags ) )
  {
    if( !search_from_start )
    {
      mp_teChat->moveCursor( QTextCursor::Start );
      if( mp_teChat->find( txt, find_flags ) )
      {
        m_lastTextFound = txt;
        return;
      }
    }

    QMessageBox::information( this, Settings::instance().programName(), tr( "%1 not found in chat." ).arg( QString( "\"%1\"" ).arg( txt ) ) );
  }
  else
    m_lastTextFound = txt;
}

void GuiChat::openSelectedTextAsUrl()
{
  QString selected_text = mp_teChat->textCursor().selectedText();
  if( !selected_text.isEmpty() )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Try to open selected text as url:" << selected_text;
#endif
    QUrl url = QUrl::fromUserInput( selected_text );
    emit openUrl( url );
  }
}

void GuiChat::updateMenuMembers( const Chat& c )
{
  User u;
  QString s_tmp;
  QAction* act;
  mp_menuMembers->clear();

  foreach( VNumber user_id, c.usersId() )
  {
    u = UserManager::instance().findUser( user_id );

    if( u.isLocal() )
      s_tmp = tr( "You" );
    else if( c.userHasReadMessages( u.id() ) || u.protocolVersion() < 63 )
      s_tmp = u.name();
    else
      s_tmp = QString( "%1 (%2)" ).arg( u.name() ).arg( tr( "unread messages" ) );

    act = mp_menuMembers->addAction( QIcon( Bee::userStatusIcon( u.status() ) ), s_tmp );
    act->setData( u.id() );
    act->setIconVisibleInMenu( true );
    if( u.isStatusConnected() && isActiveUser( c, u ) )
    {
      act->setEnabled( true  );
      connect( act, SIGNAL( triggered() ), this, SLOT( showUserVCard() ) );
    }
    else
      act->setEnabled( false );
  }
}

void GuiChat::showMembersMenu()
{
  if( !mp_menuMembers->isEnabled() )
    return;
  updateMenuMembers( ChatManager::instance().chat( m_chatId ) );
  mp_menuMembers->exec( QCursor::pos() );
}

void GuiChat::setChatReadByUser( VNumber )
{
  updateMenuMembers( ChatManager::instance().chat( m_chatId ) );
}

void GuiChat::updateUsers( const Chat& c )
{
  if( c.id() != m_chatId )
    return;

  setChatTitle( c );
  updateMenuMembers( c );
}
