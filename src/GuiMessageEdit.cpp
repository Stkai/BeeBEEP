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

#include "BeeUtils.h"
#include "GuiMessageEdit.h"
#include "HistoryManager.h"
#include "Settings.h"


GuiMessageEdit::GuiMessageEdit( QWidget* parent )
  : QTextEdit( parent ), m_emoticonsAdded()
{
  setObjectName( "GuiMessageEdit" );
  mp_timer = new QTimer( this );
  mp_timer->setSingleShot( true );
  m_undoAvailable = false;
  m_redoAvailable = false;
  m_currentMessage = "";
  m_messageChanged = true;

  connect( mp_timer, SIGNAL( timeout() ), this, SLOT( checkWriting() ) );
  connect( this, SIGNAL( undoAvailable( bool) ), this, SLOT( setUndoAvailable( bool ) ) );
  connect( this, SIGNAL( redoAvailable( bool ) ), this, SLOT( setRedoAvailable( bool ) ) );
}

void GuiMessageEdit::createMessageToSend()
{
  QString text;

  if( !m_emoticonsAdded.isEmpty() )
  {
    QString html_text = toHtml();
    foreach( Emoticon e, m_emoticonsAdded )
      html_text.replace( e.toHtml( Settings::instance().emoticonSizeInEdit() ), e.textToMatch() );
    text = Bee::removeHtmlTag( html_text );
  }
  else
    text = toPlainText();

  m_currentMessage = text.trimmed().isEmpty() ? QLatin1String( "" ) : text;
  m_messageChanged = false;
}

QString GuiMessageEdit::message()
{
  if( m_messageChanged )
    createMessageToSend();

  return m_currentMessage;
}

void GuiMessageEdit::addEmoticon( const Emoticon& e )
{
  if( !isEnabled() )
    return;

  if( e.isInGroup() )
  {
    if( Settings::instance().useNativeEmoticons() )
    {
      insertPlainText( e.textToMatch() );
    }
    else
    {
      insertHtml( QString( "&nbsp;" ) + e.toHtml( Settings::instance().emoticonSizeInEdit() ) );
      if( !m_emoticonsAdded.contains( e ) )
        m_emoticonsAdded.append( e );
    }
  }
  else
    insertPlainText( QString( " " ) + e.textToMatch() );

  m_messageChanged = true;
}

void GuiMessageEdit::clearMessage()
{
  m_messageChanged = true;
  clear();
  m_emoticonsAdded.clear();
  setTextColor( QColor( Settings::instance().chatFontColor() ) );
  setFontPointSize( Settings::instance().chatFont().pointSize() );
}

void GuiMessageEdit::addMessageToHistory()
{
  HistoryMessage hm;
  hm.setMessage( message() );
  hm.setEmoticons( m_emoticonsAdded );
  HistoryManager::instance().addMessage( hm );
}

bool GuiMessageEdit::nextMessageFromHistory()
{
  if( !HistoryManager::instance().moveHistoryUp() )
    return false;
  setMessageFromHistory();
  return true;
}

bool GuiMessageEdit::prevMessageFromHistory()
{
  if( !HistoryManager::instance().moveHistoryDown() )
    return false;
  setMessageFromHistory();
  return true;
}

void GuiMessageEdit::setMessageFromHistory()
{
  QString current_message = message();

  if( !HistoryManager::instance().hasTemporaryMessage() && !current_message.isEmpty() )
  {
    HistoryMessage hm;
    hm.setMessage( current_message );
    hm.setEmoticons( m_emoticonsAdded );
    HistoryManager::instance().setTemporaryMessage( hm );
  }

  HistoryMessage message_from_history = HistoryManager::instance().message();

  QString message_txt = message_from_history.message();
  m_emoticonsAdded = message_from_history.emoticons();

  foreach( Emoticon e, m_emoticonsAdded )
    message_txt.replace( e.textToMatch(), e.toHtml( Settings::instance().emoticonSizeInEdit() ) );

  setText( message_txt );
  m_messageChanged = true;
}

void GuiMessageEdit::dropEvent( QDropEvent* e )
{
  QApplication::setOverrideCursor( Qt::WaitCursor );
  QTextEdit::dropEvent( e );
  QApplication::restoreOverrideCursor();
}

void GuiMessageEdit::keyPressEvent( QKeyEvent* e )
{
  bool reset_font = false;
  Qt::KeyboardModifiers mods = e->modifiers();

  if( e->key() == Qt::Key_Tab && (mods & Qt::ControlModifier || mods & Qt::ShiftModifier) ) // switch between chats
  {
    e->accept();
    emit tabPressed();
    return;
  }

  m_messageChanged = true;

  if( e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter )
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
  {
    reset_font = true;
  }

  QTextEdit::keyPressEvent( e );

  HistoryManager::instance().clearTemporaryMessage();

  // Fixed: when the text is fully cancelled the message box looses the color and the size... patched with the line below
  if( reset_font )
  {
    setTextColor( QColor( Settings::instance().chatFontColor() ) );
    setFontPointSize( Settings::instance().chatFont().pointSize() );
  }

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

