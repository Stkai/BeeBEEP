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

#include "GuiMessageEdit.h"
#include "Settings.h"


GuiMessageEdit::GuiMessageEdit( QWidget* parent )
  : QTextEdit( parent )
{
  mp_timer = new QTimer( this );
  mp_timer->setSingleShot( true );
  connect( mp_timer, SIGNAL( timeout() ), this, SLOT( checkWriting() ) );
}

void GuiMessageEdit::keyPressEvent( QKeyEvent* e )
{
  bool reset_font_color = false;
  Qt::KeyboardModifiers mods = e->modifiers();

  if( e->key() == Qt::Key_Tab && (mods & Qt::ControlModifier || mods & Qt::ShiftModifier) ) // switch between chats
  {
    e->accept();
    emit tabPressed();
    return;
  }

  if( e->key() == Qt::Key_Return )
  {
    if( mods & Qt::ControlModifier || mods & Qt::ShiftModifier )
    {
      insertPlainText( "\n" );
      e->accept();
      return;
    }
    else
    {
      mp_timer->stop();
      emit returnPressed();
      e->accept();
      return;
    }
  }

  if( e->key() == Qt::Key_Backspace || e->key() == Qt::Key_Delete || e->key() == Qt::Key_Cancel )
    reset_font_color = true;

  QTextEdit::keyPressEvent( e );

  // Fixed: when the text is fully cancelled the message box loose the color... patched with the line below
  if( reset_font_color )
    setTextColor( QColor( Settings::instance().chatFontColor() ) );

  if( !mp_timer->isActive() )
  {
    mp_timer->start( WRITING_MESSAGE_TIMEOUT );
  }
}

void GuiMessageEdit::checkWriting()
{
  if( !toPlainText().isEmpty() )
  {
#if defined( BEEBEEP_DEBUG )
    qDebug() << "You are writing";
#endif
    emit writing();
  }
}

void GuiMessageEdit::contextMenuEvent( QContextMenuEvent *event )
{
  QMenu custom_context_menu;
  custom_context_menu.addAction( QIcon( ":/images/undo.png" ), tr( "Undo" ), this, SLOT( undo() ), QKeySequence::Undo );
  custom_context_menu.addAction( QIcon( ":/images/redo.png" ), tr( "Redo" ), this, SLOT( redo() ), QKeySequence::Redo );
  custom_context_menu.addSeparator();
  custom_context_menu.addAction( QIcon( ":/images/cut.png" ), tr( "Cut" ), this, SLOT( cut() ), QKeySequence::Cut );
  custom_context_menu.addAction( QIcon( ":/images/copy.png" ), tr( "Copy" ), this, SLOT( copy() ), QKeySequence::Copy );
  custom_context_menu.addAction( QIcon( ":/images/paste.png" ), tr( "Paste" ), this, SLOT( paste() ), QKeySequence::Paste );
  custom_context_menu.addSeparator();
  custom_context_menu.addAction( QIcon( ":/images/select-all.png" ), tr( "Select All" ), this, SLOT( selectAll() ), QKeySequence::SelectAll );
  custom_context_menu.exec( event->globalPos() );
}

