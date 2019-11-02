//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2019 Marco Mastroddi
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
// along with BeeBEEP. If not, see <http://www.gnu.org/licenses/>.
//
// Author: Marco Mastroddi <marco.mastroddi(AT)gmail.com>
//
// $Id$
//
//////////////////////////////////////////////////////////////////////

#include "BeeUtils.h"
#include "ChatManager.h"
#include "FileDialog.h"
#include "GuiChatItem.h"
#include "GuiSavedChat.h"
#include "IconManager.h"
#include "Settings.h"
#include "ShortcutManager.h"


GuiSavedChat::GuiSavedChat( QWidget* parent )
 : QMainWindow( parent )
{
  setObjectName( "GuiSavedChat" );
  Bee::removeContextHelpButton( this );
  setWindowFlags( windowFlags() | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint );
  setWindowIcon( IconManager::instance().icon( "saved-chat.png" ) );

  mp_teSavedChat = new QTextBrowser( this );
  mp_teSavedChat->setToolTip( tr( "Right click to open menu" ) );
  QPalette p = mp_teSavedChat->palette();
  p.setColor( QPalette::Highlight, Qt::yellow );
  p.setColor( QPalette::HighlightedText, Qt::black );
  mp_teSavedChat->setPalette( p );
  mp_teSavedChat->setFocusPolicy( Qt::StrongFocus ); // need focus for keyboard events like CTRL+c
  mp_teSavedChat->setReadOnly( true );
  mp_teSavedChat->setUndoRedoEnabled( false );
  mp_teSavedChat->setOpenExternalLinks( false );
  mp_teSavedChat->setOpenLinks( false );
  mp_teSavedChat->setAcceptRichText( false );
  mp_teSavedChat->setContextMenuPolicy( Qt::CustomContextMenu );
  connect( mp_teSavedChat, SIGNAL( customContextMenuRequested( const QPoint& ) ), this, SLOT( customContextMenu( const QPoint& ) ) );
  connect( mp_teSavedChat, SIGNAL( anchorClicked( const QUrl& ) ), this, SLOT( checkAnchorClicked( const QUrl&  ) ) );

  setCentralWidget( mp_teSavedChat );

  mp_barHistory = new QToolBar( tr( "Show the toolbar" ), this );
  mp_barHistory->setObjectName( "GuiHistoryToolBar" );
  addToolBar( Qt::BottomToolBarArea, mp_barHistory );
  mp_barHistory->setAllowedAreas( Qt::BottomToolBarArea | Qt::TopToolBarArea );
  mp_barHistory->setFloatable( false );
  mp_barHistory->setIconSize( Settings::instance().mainBarIconSize() );
  setupToolBar( mp_barHistory );
  mp_barHistory->toggleViewAction()->setVisible( false );

  mp_menuContext = new QMenu( this );
}

void GuiSavedChat::setupToolBar( QToolBar* bar )
{
  /* filter by keywords */
  QLabel* label = new QLabel( bar );
  label->setObjectName( "GuiLabelFilterTextHistory" );
  label->setAlignment( Qt::AlignRight | Qt::AlignTrailing | Qt::AlignVCenter );
  label->setText( QString( "   " ) + tr( "Search" ) + QString( " " ) );
  bar->addWidget( label );
  mp_leFilter = new QLineEdit( bar );
  mp_leFilter->setObjectName( "GuiLineEditFilterHistory" );
#if QT_VERSION >= 0x040700
  mp_leFilter->setPlaceholderText( tr( "keyword" ) );
#endif
  bar->addWidget( mp_leFilter );
  connect( mp_leFilter, SIGNAL( returnPressed() ), this, SLOT( findTextInHistory() ) );

  /* search button */
  bar->addAction( IconManager::instance().icon( "search.png" ), tr( "Find" ), this, SLOT( findTextInHistory() ) );

  /* flags */
  mp_cbCaseSensitive = new QCheckBox( bar );
  mp_cbCaseSensitive->setObjectName( "GuiCheckBoxFindCaseSensitiveInLog" );
  mp_cbCaseSensitive->setText( tr( "Case sensitive" ) );
  bar->addWidget( mp_cbCaseSensitive );

  mp_cbWholeWordOnly = new QCheckBox( bar );
  mp_cbWholeWordOnly->setObjectName( "GuiCheckBoxFindWholeWordOnlyInLog" );
  mp_cbWholeWordOnly->setText( tr( "Whole word" ) );
  bar->addWidget( mp_cbWholeWordOnly );

  /* save as button */
  bar->addSeparator();
  bar->addAction( IconManager::instance().icon( "save-as.png" ), tr( "Save as" ), this, SLOT( saveHistoryAs() ) );
  bar->addSeparator();
  bar->addAction( IconManager::instance().icon( "printer.png" ), tr( "Print..." ), this, SLOT( printChat() ) );
  bar->addSeparator();
  bar->addAction( IconManager::instance().icon( "delete.png" ), tr( "Clear messages" ), this, SLOT( deleteSavedChat() ) );
}

void GuiSavedChat::showSavedChat( const QString& chat_name )
{
  m_savedChatName = chat_name;
  QString window_chat_title = chat_name == Settings::instance().defaultChatName() ? GuiChatItem::defaultChatName().toUpper() : chat_name;
  setWindowTitle( QString( "%1 - %2" ).arg( window_chat_title ).arg( tr( "Saved chat" ) ) );

  QString html_text = "";
  if( !ChatManager::instance().chatHasSavedText( chat_name ) )
    html_text += QString( "<br>*** %1 ***<br>" ).arg( tr( "Empty" ) );
  else
    html_text += ChatManager::instance().chatSavedText( chat_name );

  html_text += QString( "<br>*** %1: %2 ***<br><br><br>" ).arg( tr( "Saved chat" ) ).arg( tr( "end of messages" ) );
  bool updates_enabled = mp_teSavedChat->updatesEnabled();
  mp_teSavedChat->setUpdatesEnabled( false );
  mp_teSavedChat->setText( html_text );
  mp_teSavedChat->setUpdatesEnabled( updates_enabled );
  /* Performance issue: do not set the bar to maximum */
}

void GuiSavedChat::customContextMenu( const QPoint& )
{
  mp_menuContext->clear();
  mp_menuContext->addAction( IconManager::instance().icon( "select-all.png" ), tr( "Select All" ), mp_teSavedChat, SLOT( selectAll() ), QKeySequence::SelectAll );
  mp_menuContext->addSeparator();
  mp_menuContext->addSeparator();
  if( !mp_teSavedChat->textCursor().selectedText().isEmpty() )
  {
    mp_menuContext->addAction( IconManager::instance().icon( "copy.png" ), tr( "Copy to clipboard" ), mp_teSavedChat, SLOT( copy() ), QKeySequence::Copy );
    mp_menuContext->addSeparator();
    mp_menuContext->addAction( IconManager::instance().icon( "network.png" ), tr( "Open selected text as url" ), this, SLOT( openSelectedTextAsUrl() ) );
  }
  mp_menuContext->exec( QCursor::pos() );
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

void GuiSavedChat::deleteSavedChat()
{
  emit deleteSavedChatRequest( m_savedChatName );
  close();
}

void GuiSavedChat::checkAnchorClicked( const QUrl& url )
{
  emit openUrl( url );
}

void GuiSavedChat::findTextInHistory()
{
  QString txt = mp_leFilter->text().simplified();
  if( txt.isEmpty() )
    return;

  QTextDocument::FindFlags find_flags;
  if( mp_cbCaseSensitive->isChecked() )
    find_flags |= QTextDocument::FindCaseSensitively;
  if( mp_cbWholeWordOnly->isChecked() )
    find_flags |= QTextDocument::FindWholeWords;

  if( !mp_teSavedChat->find( txt, find_flags ) )
  {
    mp_teSavedChat->moveCursor( QTextCursor::Start );
    if( !mp_teSavedChat->find( txt ) )
    {
      QMessageBox::information( this, Settings::instance().programName(), tr( "%1 not found" ).arg( QString( "\"%1\"" ).arg( txt ) ) + QString( "." ), tr( "Ok" ) );
      return;
    }
  }
}

void GuiSavedChat::saveHistoryAs()
{
  QString file_name = FileDialog::getSaveFileName( this,
                          tr( "Please select a file where to save the messages showed in this window." ),
                          Settings::instance().dataFolder(), "PDF Chat Files (*.pdf)" );
  if( file_name.isEmpty() )
    return;

  if( !file_name.toLower().endsWith( QLatin1String( ".pdf" ) ) )
    file_name.append( QLatin1String( ".pdf" ) );

  QPrinter printer;
  printer.setOutputFormat( QPrinter::PdfFormat );
  printer.setOutputFileName( file_name );
  QTextDocument *doc = mp_teSavedChat->document();
  doc->print( &printer );
  QMessageBox::information( this, Settings::instance().programName(), tr( "%1: save completed." ).arg( file_name ), tr( "Ok" ) );
}
