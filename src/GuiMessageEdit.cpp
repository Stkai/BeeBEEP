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
#ifdef BEEBEEP_USE_HUNSPELL
  #include "SpellChecker.h"
  #include "SpellCheckerHighlighter.h"
#endif


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

#ifdef BEEBEEP_USE_HUNSPELL
  mp_scHighlighter = new SpellCheckerHighlighter( this->document() );
#endif
  mp_completer = 0;

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
    text = Bee::removeHtmlTags( html_text );
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

void GuiMessageEdit::addPasted( const QMimeData* source_data )
{
  if( !isEnabled() )
    return;

  QTextEdit::insertFromMimeData( source_data );

  m_messageChanged = true;
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

void GuiMessageEdit::completerKeyPressEvent( QKeyEvent* e )
{
#ifdef BEEBEEP_USE_HUNSPELL

  bool is_shortcut = e->modifiers() & Qt::ControlModifier; // CTRL+
  if( !is_shortcut ) // do not process the shortcut when we have a completer
    QTextEdit::keyPressEvent( e );

  const bool ctrl_or_shift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
  if( ctrl_or_shift && e->text().isEmpty() )
    return;

  static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-="); // end of word
  bool has_modifier = (e->modifiers() != Qt::NoModifier) && !ctrl_or_shift;
  QString completion_prefix = textUnderCursor();

  if( !is_shortcut && (has_modifier || e->text().isEmpty() || completion_prefix.length() < 3 || eow.contains( e->text().right( 1 ) ) ) )
  {
    mp_completer->popup()->hide();
    return;
  }

  if( completion_prefix != SpellChecker::instance().completerPrefix() )
  {
    SpellChecker::instance().updateCompleter( completion_prefix );
    mp_completer->popup()->setCurrentIndex( mp_completer->completionModel()->index( 0, 0 ) );
  }

  QRect cursor_rect = cursorRect();
  cursor_rect.setWidth( mp_completer->popup()->sizeHintForColumn(0) + mp_completer->popup()->verticalScrollBar()->sizeHint().width() );
  mp_completer->complete( cursor_rect );

#else

  QTextEdit::keyPressEvent( e );

#endif
}

void GuiMessageEdit::keyPressEvent( QKeyEvent* e )
{
  if( mp_completer && mp_completer->popup()->isVisible() )
  {
    // The following keys are forwarded by the completer to the widget
    switch( e->key() )
    {
      case Qt::Key_Enter:
      case Qt::Key_Return:
      case Qt::Key_Escape:
      case Qt::Key_Tab:
      case Qt::Key_Backtab:
      case Qt::Key_Down:
      case Qt::Key_Right:
      case Qt::Key_Left:
      case Qt::Key_Up:
      case Qt::Key_PageUp:
      case Qt::Key_PageDown:
        e->ignore();
        return; // let the completer do default behavior
      default:
        break;
    }
  }

  bool reset_font = false;
  Qt::KeyboardModifiers mods = e->modifiers();

  m_messageChanged = true;

  if( e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter )
  {
    if( !Settings::instance().useReturnToSendMessage() || mods & Qt::ControlModifier || mods & Qt::ShiftModifier )
    {
      insertPlainText( "\n" );
    }
    else
    {
      mp_timer->stop();

      if( !message().isEmpty() )
      {
        addMessageToHistory();
        emit returnPressed();
      }
    }

    e->accept();
    return;
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

  if( mp_completer && Settings::instance().useWordCompleter() )
    completerKeyPressEvent( e );
  else
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

bool GuiMessageEdit::canInsertFromMimeData( const QMimeData* source ) const
{
  return source->hasImage() || source->hasUrls() || QTextEdit::canInsertFromMimeData( source );
}

void GuiMessageEdit::insertFromMimeData( const QMimeData* source )
{
  if( source->hasImage() )
  {
    emit imageToCheck( source );
  }
  else if( source->hasUrls() )
  {
    emit urlsToCheck( source );
  }
  else
  {
    addPasted( source );
  }
}

void GuiMessageEdit::rehighlightMessage()
{
#ifdef BEEBEEP_USE_HUNSPELL
   mp_scHighlighter->rehighlight();
#endif
}

void GuiMessageEdit::setCompleter(QCompleter *completer)
{
  if( mp_completer )
    QObject::disconnect( mp_completer, 0, this, 0 );

  mp_completer = completer;

  if( !mp_completer )
    return;

  mp_completer->setWidget( this );
  connect( mp_completer, SIGNAL( activated( const QString& ) ), this, SLOT( insertCompletion( const QString& ) ) );
}

void GuiMessageEdit::insertCompletion( const QString& completion )
{
  if( mp_completer->widget() != this )
    return;

  QTextCursor tc = textCursor();
#ifdef BEEBEEP_USE_HUNSPELL
  int extra = completion.length() - SpellChecker::instance().completerPrefix().length();
#else
  int extra = 0;
#endif
  tc.movePosition( QTextCursor::Left );
  tc.movePosition( QTextCursor::EndOfWord );
  tc.insertText( completion.right( extra ) );
  tc.insertText( " " );
  setTextCursor( tc );
}

QString GuiMessageEdit::textUnderCursor() const
{
  QTextCursor tc = textCursor();
  tc.select( QTextCursor::WordUnderCursor );
  return tc.selectedText();
}

void GuiMessageEdit::focusInEvent( QFocusEvent* ev )
{
  if( mp_completer )
    mp_completer->setWidget( this );
  QTextEdit::focusInEvent( ev );
}
