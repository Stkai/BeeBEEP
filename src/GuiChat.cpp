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
#include "ChatMessageData.h"
#include "EmoticonManager.h"
#include "GuiChat.h"
#include "PluginManager.h"
#include "Protocol.h"
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

  setChatFont( Settings::instance().chatFont() );
  setChatFontColor( Settings::instance().chatFontColor() );

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
  qDebug() << "Open url:" << url.toString();
  if( !QDesktopServices::openUrl( url ) )
    QMessageBox::information( this, Settings::instance().programName(), tr( "Unable to open %1").arg( url.toString( QUrl::RemoveScheme ) ), tr( "Ok" ) );
}


namespace // begin of empty namespace
{

QString Linkify( QString text )
{
  if( !text.contains( QLatin1Char( '.' ) ) )
    return text;
  text.prepend( " " ); // for matching www.miosito.it
  text.replace( QRegExp( "(((f|ht){1}tp(s:|:){1}//)[-a-zA-Z0-9@:%_\\+.,~#?&//=\\(\\)]+)" ), "<a href=\"\\1\">\\1</a>" );
  text.replace( QRegExp( "([\\s()[{}])(www.[-a-zA-Z0-9@:%_\\+.,~#?&//=\\(\\)]+)" ), "\\1<a href=\"http://\\2\">\\2</a>" );
  text.replace( QRegExp( "([_\\.0-9a-z-]+@([0-9a-z][0-9a-z-]+\\.)+[a-z]{2,3})" ), "<a href=\"mailto:\\1\">\\1</a>" );
  text.remove( 0, 1 ); // remove the space added
  qDebug() << "Linkify:" << text;
  return text;
}

QString FormatHtmlText( const QString& text )
{
  QString text_formatted = "";
  int last_semicolon_index = -1;

  for( int i = 0; i < text.length(); i++ )
  {
    if( text.at( i ) == QLatin1Char( ' ' ) )
    {
      if( (i + 1) < text.length() && text.at( (i+1) ) == QLatin1Char( ' ' ) )
        text_formatted += QLatin1String( "&nbsp;" );
      else
        text_formatted += QLatin1Char( ' ' );
    }
    else if( text.at( i ) == QLatin1Char( '\n' ) )
      text_formatted += QLatin1String( "<br /> " ); // space added to match url after a \n
    else if( text.at( i ) == QLatin1Char( '<' ) )
    {
      if( Settings::instance().chatUseHtmlTags() )
      {
        if( last_semicolon_index >= 0 )
          text_formatted.replace( last_semicolon_index, 1, QLatin1String( "&lt;" ) );

        // preserve heart emoticon
        if( (i+1) < text.size() && text.at( i+1 ) != QLatin1Char( '3' ) )
          last_semicolon_index = text_formatted.size();

        text_formatted += QLatin1Char( '<' );
      }
      else
        text_formatted += QLatin1String( "&lt;" );
    }
    else if( text.at( i ) == QLatin1Char( '>' ) )
    {
      if( Settings::instance().chatUseHtmlTags() )
      {
        text_formatted += QLatin1Char( '>' );
        if( last_semicolon_index >= 0 )
          last_semicolon_index = -1;
      }
      else
        text_formatted += QLatin1String( "&gt;" );
    }
    else if( text.at( i ) == QLatin1Char( '"' ) )
    {
      if( last_semicolon_index >= 0 )
        text_formatted += QLatin1Char( '"' );
      else
        text_formatted += QLatin1String( "&quot;" );
    }
    else if( text.at( i ) == QLatin1Char( '&' ) )
    {
      text_formatted += QLatin1Char( '&' ); // not &amp; for Linkify
    }
    else
      text_formatted += text.at( i );
  }

  if( last_semicolon_index >= 0 )
    text_formatted.replace( last_semicolon_index, 1, QLatin1String( "&lt;" ) );

  PluginManager::instance().parseText( &text_formatted, false );

  text_formatted.replace( QRegExp("(^|\\s|>)_(\\S+)_(<|\\s|$)"), "\\1<u>\\2</u>\\3" );
  text_formatted.replace( QRegExp("(^|\\s|>)\\*(\\S+)\\*(<|\\s|$)"), "\\1<b>\\2</b>\\3" );
  text_formatted.replace( QRegExp("(^|\\s|>)\\/(\\S+)\\/(<|\\s|$)"), "\\1<i>\\2</i>\\3" );

  if( Settings::instance().chatUseClickableLinks() )
    text_formatted = Linkify( text_formatted );

  return EmoticonManager::instance().parseEmoticons( text_formatted );
}

QString FormatMessage( const User& u, const ChatMessage& cm )
{
  QString text_formatted = FormatHtmlText( cm.message().text() );
  ChatMessageData cm_data = Protocol::instance().dataFromChatMessage( cm.message() );
  if( cm_data.textColor().isValid() )
  {
    text_formatted.prepend( QString( "<font color=%1>" ).arg( cm_data.textColor().name() ) );
    text_formatted.append( QString( "</font>" ) );
  }

  QString sHtmlMessage = QString( "%1<font color=%2><b>%3</b>%4%5</font>" )
            .arg( Settings::instance().chatShowMessageTimestamp() ? QString( "<font color=#808080>%1</font> " ).arg( cm.message().timestamp().toString( "(hh:mm:ss)" ) ) : "" )
            .arg( Settings::instance().showUserColor() ? u.color() : "#000000" )
            .arg( u.isLocal() ? QObject::tr( "Me" ) : u.name() )
            .arg( Settings::instance().chatCompact() ? ":&nbsp;" : ":<br />" )
            .arg( text_formatted );
  return sHtmlMessage;
}

QString FormatSystemMessage( const ChatMessage& cm )
{
  QString sHtmlMessage = QString( "<font color=#808080>%1 %2</font>" )
            .arg( Settings::instance().chatShowMessageTimestamp() ? cm.message().timestamp().toString( "(hh:mm:ss) " ) : "" )
            .arg( cm.message().text() );
  return sHtmlMessage;
}

} // end of empty namespace

QString GuiChat::chatMessageToText( const ChatMessage& cm )
{
  QString s;
  if( cm.isSystem() )
    s = FormatSystemMessage( cm );
  else
    s = FormatMessage( m_users.find( cm.userId() ), cm );
  s += Settings::instance().chatAddNewLineToMessage() ? "<br /><br />" : "<br />";
  return s;
}

void GuiChat::setChatUsers()
{
  QString chat_users;
  if( m_chatId == ID_DEFAULT_CHAT )
  {
    chat_users = tr( "All users" );
  }
  else
  {
    QStringList sl = m_users.toStringList( true, false );
    chat_users = sl.size() == 0 ? tr( "Nobody" ) : sl.join( ", " );
  }
  mp_lTitle->setText( tr( "To" ) + QString( ": <b>%1</b>" ).arg( chat_users ) );
}

bool GuiChat::setChatId( VNumber chat_id )
{
  qDebug() << "Setting chat" << chat_id << "in default chat window";
  Chat c = ChatManager::instance().chat( chat_id, true );
  if( !c.isValid() )
    return false;
  m_chatId = c.id();
  m_users = UserManager::instance().userList().fromUsersId( c.usersId() );

  bool is_secure = c.isDefault() || (m_users.toList().size() >= 1 && m_users.toList().last().isOnLan() && c.isPrivateForUser( m_users.toList().last().id() ));
  if( is_secure )
  {
    mp_lPixSecure->setPixmap( QPixmap( ":/images/secure.png" ) );
    mp_lPixSecure->setToolTip( tr( "%1 Secure Mode" ).arg( Settings::instance().programName() ) );
  }
  else
  {
    mp_lPixSecure->setPixmap( QPixmap() );
    mp_lPixSecure->setToolTip( "" );
  }

  setChatUsers();
  QString html_text;
  foreach( ChatMessage cm, c.messages() )
    html_text += chatMessageToText( cm );
  mp_teChat->setHtml( html_text );

  QScrollBar *bar = mp_teChat->verticalScrollBar();
  bar->setValue( bar->maximum() );
  setLastMessageTimestamp( c.lastMessageTimestamp() );
  mp_teMessage->setFocus();
  qDebug() << "Chat" << chat_id << "showed";
  return true;
}

void GuiChat::appendChatMessage( VNumber chat_id, const ChatMessage& cm )
{
  if( m_chatId != chat_id )
  {
    qWarning() << "Trying to append chat message of chat id" << chat_id << "in chat showed with id" << m_chatId << "... skip it";
    return;
  }

  bool read_all_messages = !cm.isFromLocalUser() && !cm.isSystem();
  Chat c = ChatManager::instance().chat( m_chatId, read_all_messages );
  if( !c.isValid() )
    return;

  User u = m_users.find( cm.userId() );
  if( !u.isValid() )
  {
    qDebug() << "User" << cm.userId() << "is not present in chat showed" << m_chatId << "... force update";
    m_users = UserManager::instance().userList().fromUsersId( c.usersId() );
    u = m_users.find( cm.userId() );
    if( !u.isValid() )
    {
      qWarning() << "User" << cm.userId() << "is not present in chat" << m_chatId << "... message is not showed";
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
