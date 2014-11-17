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
#include "GuiChat.h"
#include "GuiChatMessage.h"
#include "GuiSessionManager.h"
#include "Settings.h"
#include "UserManager.h"


GuiChat::GuiChat( QWidget *parent )
 : QWidget( parent )
{
  setupUi( this );
  setObjectName( "GuiChat" );
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

  connect( mp_teChat, SIGNAL( customContextMenuRequested( const QPoint& ) ), this, SLOT( customContextMenu( const QPoint& ) ) );
  connect( mp_teChat, SIGNAL( anchorClicked( const QUrl& ) ), this, SLOT( checkAnchorClicked( const QUrl&  ) ) );
  connect( mp_teMessage, SIGNAL( returnPressed() ), this, SLOT( sendMessage() ) );
  connect( mp_teMessage, SIGNAL( writing() ), this, SLOT( checkWriting() ) );
  connect( mp_teMessage, SIGNAL( tabPressed() ), this, SIGNAL( nextChat() ) );

#ifdef Q_OS_SYMBIAN
  connect( mp_buttonSend, SIGNAL( clicked() ), this, SLOT( sendMessage() ) );
#else
  mp_buttonSend->hide();
#endif
}

void GuiChat::customContextMenu( const QPoint& p )
{
  QMenu custom_context_menu;
  custom_context_menu.addAction( QIcon( ":/images/paste.png" ), tr( "Copy to clipboard" ), mp_teChat, SLOT( copy() ) );
  custom_context_menu.addSeparator();
  custom_context_menu.addAction( QIcon( ":/images/select-all.png" ), tr( "Select All" ), mp_teChat, SLOT( selectAll() ) );
  custom_context_menu.exec( mapToGlobal( p ) );
}

void GuiChat::setChatFont( const QFont& f )
{
  mp_teChat->setFont( f );
  mp_teMessage->setFont( f );
}

void GuiChat::setLastMessageTimestamp( const QDateTime& dt )
{
  if( dt.isValid() && !Settings::instance().chatShowMessageTimestamp() )
    mp_lTimestamp->setText( QString( " " ) + tr( "(Last message %1)" ).arg( dt.toString( "hh:mm" )));
  else
    mp_lTimestamp->setText( "" );
}

void GuiChat::sendMessage()
{
  emit newMessage( m_chatId, mp_teMessage->message() );
}

void GuiChat::addToMyMessage( const QString& msg_to_add )
{
  if( msg_to_add.isEmpty() )
    return;
  mp_teMessage->insertPlainText( msg_to_add );
}

void GuiChat::setChatFontColor( const QString& color_name )
{
  mp_teMessage->setTextColor( QColor( color_name ) );
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
    s = GuiChatMessage::formatMessage( m_users.find( cm.userId() ), cm, Settings::instance().showMessagesGroupByUser() ? m_lastMessageUserId : 0 );
    m_lastMessageUserId = cm.userId();
  }
  return s;
}

void GuiChat::updateUser( const User& u )
{
  if( m_users.find( u.id() ).isValid() )
  {
    m_users.set( u );
    setChatUsers();
  }
}

void GuiChat::setChatUsers()
{
  QString chat_users;
  if( m_chatId == ID_DEFAULT_CHAT )
  {
    chat_users = tr( "All Lan Users" );
#ifdef BEEBEEP_DEBUG
    qDebug() << m_users.toStringList( false, false).join( "," );
#endif
  }
  else
  {
    QStringList sl;
    foreach( User u, m_users.toList() )
    {
      if( !u.isLocal() )
      {
        if( u.isConnected() )
          sl.append( u.name() );
        else
          sl.append( QString( "[%1]" ).arg( u.name() ) );
      }
    }
    chat_users = sl.size() == 0 ? tr( "Nobody" ) : sl.join( ", " );
  }
  mp_lTitle->setText( tr( "To" ) + QString( ": <b>%1</b>" ).arg( chat_users ) );
}

bool GuiChat::setChatId( VNumber chat_id )
{
#ifdef BEEBEEP_DEBUG
  qDebug() << "Setting chat" << chat_id << "in default chat window";
#endif
  Chat c = ChatManager::instance().chat( chat_id, true );
  if( !c.isValid() )
    return false;
  m_chatId = c.id();

  m_users = UserManager::instance().userList().fromUsersId( c.usersId() );

  setChatUsers();

  QString html_text = GuiSessionManager::instance().chatHasStoredText( c.name() ) ? GuiSessionManager::instance().chatStoredText( c.name() ) : "";
  if( !html_text.isEmpty() )
    html_text.append( "<br />" );

  foreach( ChatMessage cm, c.messages() )
    html_text += chatMessageToText( cm );
  mp_teChat->setHtml( html_text );

  QScrollBar *bar = mp_teChat->verticalScrollBar();
  if( bar )
    bar->setValue( bar->maximum() );
  setLastMessageTimestamp( c.lastMessageTimestamp() );
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

  User u = m_users.find( cm.userId() );
  if( !u.isValid() )
  {
    qDebug() << "User" << cm.userId() << "is not present in chat shown" << m_chatId << "... force update";
    m_users = UserManager::instance().userList().fromUsersId( c.usersId() );
    u = m_users.find( cm.userId() );
    if( !u.isValid() )
    {
      qWarning() << "User" << cm.userId() << "is not present in chat" << m_chatId << "... message is not shown";
      return;
    }
    setChatUsers();
  }

  appendMessage( chatMessageToText( cm ) );

  if( read_all_messages )
    setLastMessageTimestamp( cm.message().timestamp() );
}

void GuiChat::appendMessage( const QString& msg )
{
  QTextCursor cursor( mp_teChat->textCursor() );
  cursor.movePosition( QTextCursor::End );
  cursor.insertHtml( msg );
  QScrollBar *bar = mp_teChat->verticalScrollBar();
  bar->setValue( bar->maximum() );
}
