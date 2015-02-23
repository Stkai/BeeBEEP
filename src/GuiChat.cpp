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

#include "ChatManager.h"
#include "ChatMessage.h"
#include "EmoticonManager.h"
#include "GuiChat.h"
#include "GuiChatMessage.h"
#include "Settings.h"
#include "UserManager.h"


GuiChat::GuiChat( QWidget *parent )
 : QWidget( parent )
{
  setupUi( this );
  setObjectName( "GuiChat" );

  setAcceptDrops( true );

  mp_teMessage->setFocusPolicy( Qt::StrongFocus );
  mp_teChat->setObjectName( "GuiChatViewer" );
  mp_teChat->setFocusPolicy( Qt::NoFocus );
  mp_teChat->setReadOnly( true );
  mp_teChat->setContextMenuPolicy( Qt::CustomContextMenu );
  mp_teChat->setOpenExternalLinks( false );
  mp_teChat->setOpenLinks( false );
  mp_lPix->setPixmap( QPixmap( ":/images/chat.png" ) );

  mp_lPixSecure->setPixmap( QPixmap( ":/images/secure.png" ) );
  mp_lPixSecure->setToolTip( tr( "%1 Secure Mode" ).arg( Settings::instance().programName() ) );

  setChatFont( Settings::instance().chatFont() );
  setChatFontColor( Settings::instance().chatFontColor() );

  m_lastMessageUserId = 0;
  m_lastEmoticonSelected = ":)";

  connect( mp_teChat, SIGNAL( customContextMenuRequested( const QPoint& ) ), this, SLOT( customContextMenu( const QPoint& ) ) );
  connect( mp_teChat, SIGNAL( anchorClicked( const QUrl& ) ), this, SLOT( checkAnchorClicked( const QUrl&  ) ) );
  connect( mp_teMessage, SIGNAL( returnPressed() ), this, SLOT( sendMessage() ) );
  connect( mp_teMessage, SIGNAL( writing() ), this, SLOT( checkWriting() ) );
  connect( mp_teMessage, SIGNAL( tabPressed() ), this, SIGNAL( nextChat() ) );
}

void GuiChat::setupToolBar( QToolBar* bar )
{
  QAction* act;

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
  mp_actEmoticons = mp_menuEmoticons->menuAction();
  connect( mp_actEmoticons, SIGNAL( triggered() ), this, SLOT( lastEmoticonSelected() ) );

  bar->addAction( mp_actEmoticons );
  act = bar->addAction( QIcon( ":/images/font.png" ), tr( "Change font style" ), this, SLOT( selectFont() ) );
  act->setStatusTip( tr( "Select your favourite chat font style" ) );
  act = bar->addAction( QIcon( ":/images/font-color.png" ), tr( "Change font color" ), this, SLOT( selectFontColor() ) );
  act->setStatusTip( tr( "Select your favourite font color for the chat messages" ) );
  bar->addSeparator();
  mp_actSendFile = bar->addAction( QIcon( ":/images/send-file.png" ), tr( "Send file" ), this, SLOT( sendFile() ) );
  mp_actSendFile->setStatusTip( tr( "Send a file to a user or a group" ) );
  act = bar->addAction( QIcon( ":/images/save-as.png" ), tr( "Save chat" ), this, SLOT( saveChat() ) );
  act->setStatusTip( tr( "Save the messages of the current chat to a file" ) );
  bar->addSeparator();
  mp_actCreateGroupChat = bar->addAction( QIcon( ":/images/chat-create.png" ), tr( "Create group chat" ), this, SIGNAL( createGroupChatRequest() ) );
  mp_actCreateGroupChat->setStatusTip( tr( "Create a group chat with two or more users" ) );
  mp_actGroupAdd = bar->addAction( QIcon( ":/images/group-edit.png" ), tr( "Edit group chat" ), this, SIGNAL( editGroupRequest() ) );
  mp_actGroupAdd->setStatusTip( tr( "Change the name of the group or add and remove users" ) );
  mp_actClear = bar->addAction( QIcon( ":/images/clear.png" ), tr( "Clear messages" ), this, SLOT( clearChat() ) );
  mp_actClear->setStatusTip( tr( "Clear all the messages of the chat" ) );
  bar->addSeparator();
  mp_actCreateGroup = bar->addAction( QIcon( ":/images/group-add.png" ), tr( "Create group" ), this, SIGNAL( createGroupRequest() ) );
  mp_actCreateGroup->setStatusTip( tr( "Create a group with two or more users" ) );
  mp_actLeave = bar->addAction( QIcon( ":/images/group-remove.png" ), tr( "Leave the group" ), this, SLOT( leaveThisGroup() ) );
  mp_actLeave->setStatusTip( tr( "Leave the group" ) );

}

void GuiChat::updateAction( bool is_connected, int connected_users )
{
  bool local_user_is_member = isActiveUser( Settings::instance().localUser() );
  mp_actSendFile->setEnabled( local_user_is_member && is_connected && connected_users > 0 );
  mp_actCreateGroup->setEnabled( is_connected && connected_users > 1 );
  mp_actCreateGroupChat->setEnabled( is_connected && connected_users > 1 );
  mp_actGroupAdd->setEnabled( local_user_is_member && is_connected && ChatManager::instance().isGroupChat( m_chatId ) );
  mp_actLeave->setEnabled( local_user_is_member && is_connected && ChatManager::instance().isGroupChat( m_chatId ) );
}

void GuiChat::customContextMenu( const QPoint& p )
{
  QMenu custom_context_menu;
  custom_context_menu.addAction( QIcon( ":/images/paste.png" ), tr( "Copy to clipboard" ), mp_teChat, SLOT( copy() ) );
  custom_context_menu.addSeparator();
  custom_context_menu.addAction( QIcon( ":/images/select-all.png" ), tr( "Select All" ), mp_teChat, SLOT( selectAll() ) );
  custom_context_menu.exec( mapToGlobal( p ) );
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
  QString s;

  if( cm.isSystem() )
  {
    s = GuiChatMessage::formatSystemMessage( cm );
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

  u = UserManager::instance().userList().find( user_id );
  if( u.isValid() )
    m_chatUsers.set( u );

  return u;
}

void GuiChat::updateUser( const User& u )
{
  if( m_chatUsers.find( u.id() ).isValid() )
  {
    m_chatUsers.set( u );
    setChatUsers();
  }
}

bool GuiChat::isActiveUser( const User& u ) const
{
  Chat c = ChatManager::instance().chat( m_chatId );
  return c.isValid() && c.usersId().contains( u.id() );
}

void GuiChat::setChatUsers()
{
  QString chat_users;

  if( m_chatId == ID_DEFAULT_CHAT )
  {
    chat_users = tr( "All Lan Users" );
  }
  else
  {
    QStringList sl;
    foreach( User u, m_chatUsers.toList() )
    {
      if( u.isLocal() )
      {
        if( !isActiveUser( u ) )
          sl.append( tr( "(You have left)" ) );
        else
          sl.append( QString( "<b>%1</b>" ).arg( tr( "You" ) ) );
      }
      else
      {
        if( !isActiveUser( u ) )
          sl.append( QString( "(%1 has left)" ).arg( u.name() ) );
        else if( u.isConnected() )
          sl.append( QString( "<b>%1</b>" ).arg( u.name() ) );
        else
          sl.append( QString( "%1 [%2]" ).arg( u.name() ).arg( tr( "offline" ) ) );
      }
    }
    chat_users = sl.size() == 0 ? tr( "Nobody" ) : (sl.size() == 2 ? sl.join( QString( " %1 " ).arg( tr( "and" ) ) ) : sl.join( ", " ));
  }

#ifdef BEEBEEP_DEBUG
  qDebug() << "Chat members:" << m_chatUsers.toStringList( false, false ).join( ", " );
#endif
  mp_lTitle->setText( tr( "To" ) + QString( ": %1" ).arg( chat_users ) );
  mp_teMessage->setEnabled( isActiveUser( Settings::instance().localUser() ) );
}

bool GuiChat::setChatId( VNumber chat_id )
{
  Chat c = ChatManager::instance().chat( chat_id, true );
  if( !c.isValid() )
    return false;

#ifdef BEEBEEP_DEBUG
  qDebug() << "Setting chat" << chat_id << "in default chat window";
#endif
  m_chatId = c.id();
  m_chatName = c.name();

  mp_actClear->setDisabled( c.isEmpty() );

  m_chatUsers = UserManager::instance().userList().fromUsersId( c.usersId() );

  QString html_text = "";

  if( ChatManager::instance().isLoadHistoryCompleted() && ChatManager::instance().chatHasSavedText( c.name() ) )
    html_text += ChatManager::instance().chatSavedText( c.name() );

  if( !html_text.isEmpty() )
    html_text.append( "<br />" );

  foreach( ChatMessage cm, c.messages() )
    html_text += chatMessageToText( cm );
  mp_teChat->setHtml( html_text );

  QScrollBar *bar = mp_teChat->verticalScrollBar();
  if( bar )
    bar->setValue( bar->maximum() );

  setLastMessageTimestamp( c.lastMessageTimestamp() );
  setChatUsers();

  mp_teMessage->setFocus();
  return true;
}

void GuiChat::appendChatMessage( VNumber chat_id, const ChatMessage& cm )
{
  if( m_chatId != chat_id )
  {
    qWarning() << "Trying to append chat message of chat id" << chat_id << "in chat shown with id" << m_chatId << "... skip it";
    return;
  }

  bool read_all_messages = !cm.isFromLocalUser() && !cm.isSystem();
  Chat c = ChatManager::instance().chat( m_chatId, read_all_messages );
  if( !c.isValid() )
    return;

  mp_actClear->setDisabled( c.isEmpty() );

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

  QTextCursor cursor( mp_teChat->textCursor() );
  cursor.movePosition( QTextCursor::End );
  cursor.insertHtml( chatMessageToText( cm ) );
  QScrollBar *bar = mp_teChat->verticalScrollBar();
  bar->setValue( bar->maximum() );

  if( read_all_messages )
    setLastMessageTimestamp( cm.message().timestamp() );
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

void GuiChat::selectFontColor()
{
  QColor c = QColorDialog::getColor( QColor( Settings::instance().chatFontColor() ), this );
  if( c.isValid() )
  {
    Settings::instance().setChatFontColor( c.name() );
    setChatFontColor( c.name() );
  }
}

void GuiChat::lastEmoticonSelected()
{
  mp_teMessage->insertPlainText( m_lastEmoticonSelected );
}

void GuiChat::emoticonSelected()
{
  QAction* act = qobject_cast<QAction*>( sender() );
  if( act )
  {
    m_lastEmoticonSelected = act->data().toString();
    mp_teMessage->insertPlainText( m_lastEmoticonSelected );
    mp_actEmoticons->setIcon( EmoticonManager::instance().emoticon( m_lastEmoticonSelected ).pixmap() );
  }
}

void GuiChat::saveChat()
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

  file.write( mp_teChat->toHtml().toLatin1() );
  file.close();
  QMessageBox::information( this, QString( "%1 - %2" ).arg( Settings::instance().programName(), tr( "Information" ) ),
    tr( "%1: save completed." ).arg( file_name ), QMessageBox::Ok );
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

void GuiChat::dropEvent( QDropEvent *event )
{
  if( event->mimeData()->hasUrls() )
  {
    if( QMessageBox::question( this, Settings::instance().programName(),
                               tr( "Do you really want to send %1 %2 to all?" ).arg( event->mimeData()->urls().size() )
                               .arg( event->mimeData()->urls().size() == 1 ? tr( "file" ) : tr( "files" ) ),
                               tr( "Yes" ), tr( "No" ), QString(), 1, 1 ) == 1 )
    {
       return;
    }

    foreach( QUrl url, event->mimeData()->urls() )
    {
      if( url.isLocalFile() )
        emit sendFileFromChatRequest( m_chatId, url.toLocalFile() );
    }
  }
}
