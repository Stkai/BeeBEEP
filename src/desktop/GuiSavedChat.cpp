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

#include "GuiSavedChat.h"
#include "ChatManager.h"
#include "Settings.h"
#include "ShortcutManager.h"


GuiSavedChat::GuiSavedChat( QWidget* parent )
 : QWidget( parent )
{
  setupUi( this );
  setObjectName( "GuiSavedChat" );

  mp_scFindTextInChat = new QShortcut( this );
  mp_scFindTextInChat->setContext( Qt::WindowShortcut );
  connect( mp_scFindTextInChat, SIGNAL( activated() ), this, SLOT( showFindTextInChatDialog() ) );

  mp_scFindNextTextInChat = new QShortcut( this );
  mp_scFindNextTextInChat->setContext( Qt::WindowShortcut );
  connect( mp_scFindNextTextInChat, SIGNAL( activated() ), this, SLOT( findNextTextInChat() ) );

  mp_scPrint = new QShortcut( this );
  mp_scPrint->setContext( Qt::WindowShortcut );
  connect( mp_scPrint, SIGNAL( activated() ), this, SLOT( printChat() ) );

  mp_teSavedChat->setContextMenuPolicy( Qt::CustomContextMenu );
  connect( mp_teSavedChat, SIGNAL( customContextMenuRequested( const QPoint& ) ), this, SLOT( customContextMenu( const QPoint& ) ) );

}

void GuiSavedChat::showSavedChat( const QString& chat_name )
{
  m_savedChatName = chat_name;
  QString html_text = QString( "%1: <b>%2</b> <br />" ).arg( tr( "Saved chat" ) ).arg( chat_name );
  if( !ChatManager::instance().chatHasSavedText( chat_name ) )
    html_text += QString( "<br />*** %1 ***<br />" ).arg( tr( "Empty" ) );
  else
    html_text += QString( "<br />%1<br /><br /><br />" ).arg( ChatManager::instance().chatSavedText( chat_name ) );

  mp_teSavedChat->setText( html_text );

  QScrollBar *bar = mp_teSavedChat->verticalScrollBar();
  bar->setValue( bar->maximum() );
}

void GuiSavedChat::customContextMenu( const QPoint& p )
{
  QMenu custom_context_menu;

  QAction* act = custom_context_menu.addAction( QIcon( ":/images/search.png" ), tr( "Find text in chat" ), this, SLOT( showFindTextInChatDialog() ) );
  QKeySequence ks = ShortcutManager::instance().shortcut( ShortcutManager::FindTextInChat );
  if( !ks.isEmpty() && Settings::instance().useShortcuts() )
    act->setShortcut( ks );
  custom_context_menu.addSeparator();
  custom_context_menu.addAction( QIcon( ":/images/paste.png" ), tr( "Copy to clipboard" ), mp_teSavedChat, SLOT( copy() ), QKeySequence::Copy );
  custom_context_menu.addSeparator();
  custom_context_menu.addAction( QIcon( ":/images/select-all.png" ), tr( "Select All" ), mp_teSavedChat, SLOT( selectAll() ), QKeySequence::SelectAll );
  custom_context_menu.addSeparator();
  if( !mp_teSavedChat->textCursor().selectedText().isEmpty() )
  {
    custom_context_menu.addAction( QIcon( ":/images/connect.png" ), tr( "Open selected text as url" ), this, SLOT( openSelectedTextAsUrl() ) );
    custom_context_menu.addSeparator();
  }
  act = custom_context_menu.addAction( QIcon( ":/images/printer.png" ), tr( "Print..." ), this, SLOT( printChat() ) );
  ks = ShortcutManager::instance().shortcut( ShortcutManager::Print );
  if( !ks.isEmpty() && Settings::instance().useShortcuts() )
    act->setShortcut( ks );
  custom_context_menu.exec( mapToGlobal( p ) );
}

void GuiSavedChat::updateShortcuts()
{
  QKeySequence ks = ShortcutManager::instance().shortcut( ShortcutManager::FindNextTextInChat );
  if( !ks.isEmpty() )
  {
    mp_scFindNextTextInChat->setKey( ks );
    mp_scFindNextTextInChat->setEnabled( Settings::instance().useShortcuts() );
  }
  else
    mp_scFindNextTextInChat->setEnabled( false );

  ks = ShortcutManager::instance().shortcut( ShortcutManager::FindTextInChat );
  if( !ks.isEmpty() && Settings::instance().useShortcuts() )
  {
    mp_scFindTextInChat->setKey( ks );
    mp_scFindTextInChat->setEnabled( Settings::instance().useShortcuts() );
  }
  else
    mp_scFindTextInChat->setEnabled( false );

  ks = ShortcutManager::instance().shortcut( ShortcutManager::Print );
  if( !ks.isEmpty() )
  {
    mp_scPrint->setKey( ks );
    mp_scPrint->setEnabled( Settings::instance().useShortcuts() );
  }
  else
    mp_scPrint->setEnabled( false );
}

void GuiSavedChat::printChat()
{
  QPrinter printer( QPrinter::HighResolution );
  printer.setFullPage( true );
  QPrintDialog *dlg = new QPrintDialog( &printer, this );
  dlg->setOptions( QAbstractPrintDialog::PrintSelection | QAbstractPrintDialog::PrintPageRange |
                   QAbstractPrintDialog::PrintShowPageSize |  QAbstractPrintDialog::PrintCollateCopies |
#if QT_VERSION >= 0x040700
                   QAbstractPrintDialog::PrintCurrentPage |
#endif
                   QAbstractPrintDialog::PrintToFile );

  if( dlg->exec() == QDialog::Accepted)
    mp_teSavedChat->print( dlg->printer() );

  dlg->deleteLater();
}

void GuiSavedChat::showFindTextInChatDialog()
{
  QString label = tr( "Find text in chat" );
  bool ok = false;
  QString text_to_search = QInputDialog::getText( this, Settings::instance().programName(), label,
                                                  QLineEdit::Normal, m_lastTextFound, &ok );
  if( ok )
    findTextInChat( text_to_search.simplified() );
}

void GuiSavedChat::findNextTextInChat()
{
  findTextInChat( m_lastTextFound );
}

void GuiSavedChat::findTextInChat( const QString& txt )
{
  if( txt.isEmpty() )
    return;

  QTextDocument::FindFlags find_flags = 0;
  bool search_from_start = false;
  if( txt != m_lastTextFound )
  {
    mp_teSavedChat->moveCursor( QTextCursor::Start );
    search_from_start = true;
  }

  if( !mp_teSavedChat->find( txt, find_flags ) )
  {
    if( !search_from_start )
    {
      mp_teSavedChat->moveCursor( QTextCursor::Start );
      if( mp_teSavedChat->find( txt, find_flags ) )
      {
        m_lastTextFound = txt;
        return;
      }
    }

    QMessageBox::information( this, Settings::instance().programName(), tr( "%1 not found in chat." ).arg( QString( "\"%1\"" ).arg( txt ) ) );
  }
  else
    m_lastTextFound = txt;
}

void GuiSavedChat::openSelectedTextAsUrl()
{
  QString selected_text = mp_teSavedChat->textCursor().selectedText();
  if( !selected_text.isEmpty() )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Try to open selected text as url:" << selected_text;
#endif
    QUrl url = QUrl::fromUserInput( selected_text );
    emit openUrl( url );
  }
}
