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
#include "BeeUtils.h"


GuiChat::GuiChat( QWidget *parent )
 : QWidget( parent )
{
  setupUi( this );

  mp_teMessage->setFocusPolicy( Qt::StrongFocus );
  mp_teChat->setFocusPolicy( Qt::NoFocus );
  mp_teChat->setReadOnly( true );
  mp_lPix->setPixmap( QPixmap( ":/images/chat.png" ) );

  setChatFont( Settings::instance().chatFont() );
  setChatFontColor( Settings::instance().chatFontColor() );

  connect( mp_teMessage, SIGNAL( returnPressed() ), this, SLOT( sendMessage() ) );
  connect( mp_teMessage, SIGNAL( writing() ), this, SLOT( checkWriting() ) );
  connect( mp_teMessage, SIGNAL( tabPressed() ), this, SIGNAL( nextChat() ) );

#ifdef Q_OS_SYMBIAN
  connect( mp_buttonSend, SIGNAL( clicked() ), this, SLOT( sendMessage() ) );
#else
  mp_buttonSend->hide();
#endif
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

void GuiChat::setChat( const Chat& c )
{
  if( !c.isValid() )
    return;
  m_chatName = c.name();
  mp_lTitle->setText( tr( "To" ) + QString( ": <b>%1</b>" ).arg( m_chatName ) );
  mp_teChat->setText( Bee::chatMessagesToText( c ) );
  setLastMessageTimestamp( c.lastMessageTimestamp() );
  mp_teMessage->setFocus();
}

void GuiChat::appendMessage( const QString& chat_name, const QString& msg_to_append )
{
  if( chat_name != m_chatName )
    return;
  QTextCursor cursor( mp_teChat->textCursor() );
  cursor.movePosition( QTextCursor::End );
  QString msg = msg_to_append;
  msg.replace( "\n", "<br />" );
  cursor.insertHtml( QString( "%1<br />" ).arg( msg ) );
  QScrollBar *bar = mp_teChat->verticalScrollBar();
  bar->setValue( bar->maximum() );
}

void GuiChat::sendMessage()
{
  QString text = mp_teMessage->toPlainText();
  if( text.isEmpty() )
    return;
  emit newMessage( m_chatName, text );
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
  emit writing( m_chatName );
}
