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
  setObjectName( "GuiMessageEdit" );
  mp_timer = new QTimer( this );
  mp_timer->setSingleShot( true );
  m_undoAvailable = false;
  m_redoAvailable = false;
  m_historyIndex = 0;
  m_history.append( "" );
  m_lastMessage = "";
  connect( mp_timer, SIGNAL( timeout() ), this, SLOT( checkWriting() ) );
  connect( this, SIGNAL( undoAvailable( bool) ), this, SLOT( setUndoAvailable( bool ) ) );
  connect( this, SIGNAL( redoAvailable( bool ) ), this, SLOT( setRedoAvailable( bool ) ) );
}

QString GuiMessageEdit::message() const
{
  QString text = toPlainText();
  return text.trimmed().isEmpty() ? QLatin1String( "" ) : text;
}

void GuiMessageEdit::clearMessage()
{
  clear();
  setTextColor( QColor( Settings::instance().chatFontColor() ) );
}

void GuiMessageEdit::addMessageToHistory()
{
  m_lastMessage = "";
  QString message_to_add = message();
  m_history.removeOne( message_to_add ); // no duplicates
  m_history.append( message_to_add );
  if( historySize() > Settings::instance().chatMessageHistorySize() )
    m_history.removeFirst();
  m_historyIndex = m_history.size(); // +1 from historySize
}

bool GuiMessageEdit::nextMessageFromHistory()
{
  if( m_historyIndex > historySize() )
    return false;
  m_historyIndex++;
  setMessageFromHistory();
  return true;
}

bool GuiMessageEdit::prevMessageFromHistory()
{
  if( m_historyIndex < 0 )
    return false;
  m_historyIndex--;
  setMessageFromHistory();
  return true;
}

void GuiMessageEdit::setMessageFromHistory()
{
  if( m_lastMessage.isEmpty() && !message().isEmpty() )
    m_lastMessage = message();

  QString txt;
  if( m_historyIndex < 0 || m_historyIndex > historySize() )
    txt = m_lastMessage;
  else
    txt = m_history.at( m_historyIndex );
  setText( txt );
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

      if( !message().isEmpty() )
      {
        addMessageToHistory();
        emit returnPressed();
      }
      e->accept();
      clearMessage();
      return;
    }
  }

  if( mods & Qt::ControlModifier )
  {
    if( e->key() == Qt::Key_Up && prevMessageFromHistory() )
    {
      e->accept();
      return;
    }

    if( e->key() == Qt::Key_Down && nextMessageFromHistory() )
    {
      e->accept();
      return;
    }
  }

  if( e->key() == Qt::Key_Backspace || e->key() == Qt::Key_Delete || e->key() == Qt::Key_Cancel )
    reset_font_color = true;

  QTextEdit::keyPressEvent( e );

  m_lastMessage = "";

  // Fixed: when the text is fully cancelled the message box loose the color... patched with the line below
  if( reset_font_color )
    setTextColor( QColor( Settings::instance().chatFontColor() ) );

  if( !mp_timer->isActive() )
  {
    mp_timer->start( Settings::instance().writingTimeout() );
  }
}

void GuiMessageEdit::checkWriting()
{
  if( !toPlainText().isEmpty() )
  {
    emit writing();
  }
}

void GuiMessageEdit::setUndoAvailable( bool new_value )
{
  m_undoAvailable = new_value;
}

void GuiMessageEdit::setRedoAvailable( bool new_value )
{
  m_redoAvailable = new_value;
}

void GuiMessageEdit::contextMenuEvent( QContextMenuEvent *event )
{
  QMenu custom_context_menu;
  QAction* act;
  act = custom_context_menu.addAction( QIcon( ":/images/undo.png" ), tr( "Undo" ), this, SLOT( undo() ), QKeySequence::Undo );
  act->setEnabled( m_undoAvailable );
  act = custom_context_menu.addAction( QIcon( ":/images/redo.png" ), tr( "Redo" ), this, SLOT( redo() ), QKeySequence::Redo );
  act->setEnabled( m_redoAvailable );
  custom_context_menu.addSeparator();
  custom_context_menu.addAction( QIcon( ":/images/cut.png" ), tr( "Cut" ), this, SLOT( cut() ), QKeySequence::Cut );
  custom_context_menu.addAction( QIcon( ":/images/copy.png" ), tr( "Copy" ), this, SLOT( copy() ), QKeySequence::Copy );
  act = custom_context_menu.addAction( QIcon( ":/images/paste.png" ), tr( "Paste" ), this, SLOT( paste() ), QKeySequence::Paste );
  act->setEnabled( canPaste() );
  custom_context_menu.addSeparator();
  custom_context_menu.addAction( QIcon( ":/images/select-all.png" ), tr( "Select All" ), this, SLOT( selectAll() ), QKeySequence::SelectAll );
  custom_context_menu.exec( event->globalPos() );
}

