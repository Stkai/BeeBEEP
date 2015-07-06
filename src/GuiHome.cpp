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

  mp_lTitle->setText( QString( "<b>%1</b>" ).arg( tr( "Home" ) ) );

  mp_teSystem->setObjectName( "GuiSystemViewer" );
  mp_teSystem->setFocusPolicy( Qt::ClickFocus );
  mp_teSystem->setReadOnly( true );
  mp_teSystem->setContextMenuPolicy( Qt::CustomContextMenu );
  mp_teSystem->setOpenExternalLinks( false );
  mp_teSystem->setOpenLinks( false );

  mp_lNote->setText( QString( "<b>%1...</b>" ).arg( tr( "Select a user you want to chat with or" ) ) );

  mp_cbShowHomeAtStartup->setChecked( Settings::instance().showHomeAsDefaultPage() );

  connect( mp_pbOpenDefaultChat, SIGNAL( clicked() ), this, SLOT( openDefaultChat() ) );
  connect( mp_cbShowHomeAtStartup, SIGNAL( toggled( bool ) ), this, SLOT( toggleShowHomeAtStartUp( bool ) ) );
  connect( mp_teSystem, SIGNAL( customContextMenuRequested( const QPoint& ) ), this, SLOT( customContextMenu( const QPoint& ) ) );
  connect( mp_teSystem, SIGNAL( anchorClicked( const QUrl& ) ), this, SLOT( checkAnchorClicked( const QUrl&  ) ) );
}

void GuiHome::addSystemMessage( const ChatMessage& cm )
{
  if( !GuiChatMessage::messageCanBeShowedInActivity( cm ) )
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

void GuiHome::checkAnchorClicked( const QUrl& url )
{
  emit openUrlRequest( url );
}

void GuiHome::customContextMenu( const QPoint& p )
{
  QMenu custom_context_menu;
  custom_context_menu.addAction( QIcon( ":/images/paste.png" ), tr( "Copy to clipboard" ), mp_teSystem, SLOT( copy() ), QKeySequence::Copy );
  custom_context_menu.addSeparator();
  custom_context_menu.addAction( QIcon( ":/images/select-all.png" ), tr( "Select All" ), mp_teSystem, SLOT( selectAll() ), QKeySequence::SelectAll );
  custom_context_menu.exec( mapToGlobal( p ) );
}
