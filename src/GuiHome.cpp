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

#include "GuiHome.h"
#include "GuiChatMessage.h"
#include "ChatMessage.h"
#include "Settings.h"


GuiHome::GuiHome( QWidget* parent )
  : QWidget( parent )
{
  setupUi( this );
  setObjectName( "GuiHome" );

  //mp_teSystem->setStyleSheet( "background-color: rgb( 250, 250, 250 )" );

  mp_lTitle->setText( QString( "<b>%1 %2</b>" ).arg( Settings::instance().programName(), tr( "Activity" ) ) );

  mp_lNote->setText( QString( "<b>%1...</b>" ).arg( tr( "Select a user you want to chat with or" ) ) );

  mp_cbShowHomeAtStartup->setChecked( Settings::instance().showHomeAsDefaultPage() );

  connect( mp_pbOpenDefaultChat, SIGNAL( clicked() ), this, SLOT( openDefaultChat() ) );
  connect( mp_cbShowHomeAtStartup, SIGNAL( toggled( bool ) ), this, SLOT( toggleShowHomeAtStartUp( bool ) ) );
}

bool GuiHome::messageCanBeShowed( const ChatMessage& cm )
{
  switch( cm.type() )
  {
  case ChatMessage::System:
  case ChatMessage::Connection:
  case ChatMessage::UserStatus:
  case ChatMessage::FileTransfer:
  case ChatMessage::Other:
    return true;
  default:
    return false;
  }
}

void GuiHome::addSystemMessage( const ChatMessage& cm )
{
  if( !messageCanBeShowed( cm ) )
    return;

  QString sys_message = GuiChatMessage::formatSystemMessage( cm, true );

  if( sys_message.isEmpty() )
    return;

  QTextCursor cursor( mp_teSystem->textCursor() );
  cursor.movePosition( QTextCursor::End );
  cursor.insertHtml( sys_message );
  QScrollBar *bar = mp_teSystem->verticalScrollBar();
  if( bar )
    bar->setValue( bar->maximum() );
}

void GuiHome::openDefaultChat()
{
  emit openDefaultChatRequest();
}

void GuiHome::toggleShowHomeAtStartUp( bool checked )
{
  Settings::instance().setShowHomeAsDefaultPage( checked );
}
