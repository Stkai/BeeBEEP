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

#include "GuiChat.h"
#include "User.h"
#include "Settings.h"



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

void GuiChat::setChat( const Chat& c, const QString& chat_users, const QString& chat_text )
{
  qDebug() << "Setting chat" << c.id() << "in default chat window";
  m_chatId = c.id();
  mp_lTitle->setText( tr( "To" ) + QString( ": <b>%1</b>" ).arg( chat_users ) );
  mp_teChat->setHtml( chat_text );
  QScrollBar *bar = mp_teChat->verticalScrollBar();
  bar->setValue( bar->maximum() );
  setLastMessageTimestamp( c.lastMessageTimestamp() );
  mp_teMessage->setFocus();
}

void GuiChat::appendMessage( VNumber chat_id, const QString& msg )
{
  if( chat_id != m_chatId )
    return;
  QTextCursor cursor( mp_teChat->textCursor() );
  cursor.movePosition( QTextCursor::End );
  cursor.insertHtml( msg );
  QScrollBar *bar = mp_teChat->verticalScrollBar();
  bar->setValue( bar->maximum() );
}

void GuiChat::sendMessage()
{
  QString text = mp_teMessage->toPlainText();
  if( text.isEmpty() )
    return;
  emit newMessage( m_chatId, text );
  mp_teMessage->clear();
  setChatFontColor( Settings::instance().chatFontColor() );
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
