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
#include "GuiHome.h"
#include "GuiChatMessage.h"
#include "ChatManager.h"
#include "ChatMessage.h"
#include "Settings.h"
#include "ShortcutManager.h"


GuiHome::GuiHome( QWidget* parent )
  : QWidget( parent )
{
  setupUi( this );
  setObjectName( "GuiHome" );
  m_prev_sys_mess = "";
  mp_lTitle->setText( QString( "<b>%1</b>" ).arg( tr( "Activities" ) ) );

  mp_teSystem->setObjectName( "GuiSystemViewer" );
  mp_teSystem->setFocusPolicy( Qt::ClickFocus );
  mp_teSystem->setReadOnly( true );
  mp_teSystem->setContextMenuPolicy( Qt::CustomContextMenu );
  mp_teSystem->setOpenExternalLinks( false );
  mp_teSystem->setOpenLinks( false );

  connect( mp_teSystem, SIGNAL( customContextMenuRequested( const QPoint& ) ), this, SLOT( customContextMenu( const QPoint& ) ) );
  connect( mp_teSystem, SIGNAL( anchorClicked( const QUrl& ) ), this, SLOT( checkAnchorClicked( const QUrl&  ) ) );

}

void GuiHome::addSystemMessage( const ChatMessage& cm )
{
  if( !GuiChatMessage::messageCanBeShowedInActivity( cm ) )
    return;

  QString sys_message = GuiChatMessage::formatSystemMessage( cm, Settings::instance().homeShowMessageTimestamp(), Settings::instance().homeShowMessageDatestamp() );

  if( sys_message.isEmpty() )
    return;

  if( sys_message == m_prev_sys_mess )
    return;

  m_prev_sys_mess = sys_message;

  QTextCursor cursor( mp_teSystem->textCursor() );
  cursor.movePosition( QTextCursor::End );
  cursor.insertHtml( sys_message );
  QScrollBar *bar = mp_teSystem->verticalScrollBar();
  if( bar )
    bar->setValue( bar->maximum() );
}

void GuiHome::checkAnchorClicked( const QUrl& url )
{
  emit openUrlRequest( url );
}

void GuiHome::customContextMenu( const QPoint& p )
{
  QMenu custom_context_menu;
  custom_context_menu.addAction( QIcon( ":/images/select-all.png" ), tr( "Select All" ), mp_teSystem, SLOT( selectAll() ), QKeySequence::SelectAll );
  custom_context_menu.addSeparator();
  QAction* act = custom_context_menu.addAction( QIcon( ":/images/copy.png" ), tr( "Copy to clipboard" ), mp_teSystem, SLOT( copy() ), QKeySequence::Copy );
  act->setEnabled( !mp_teSystem->textCursor().selectedText().isEmpty() );
  custom_context_menu.addSeparator();
  act = custom_context_menu.addAction( QIcon( ":/images/printer.png" ), tr( "Print..." ), this, SLOT( printActivities() ) );
  QKeySequence ks = ShortcutManager::instance().shortcut( ShortcutManager::Print );
  if( !ks.isEmpty() && Settings::instance().useShortcuts() )
    act->setShortcut( ks );
  else
    act->setShortcut( QKeySequence() );
  custom_context_menu.addSeparator();
  act = custom_context_menu.addAction( tr( "Show the datestamp" ), this, SLOT( onAddDatestampClicked() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().homeShowMessageDatestamp() );

  act = custom_context_menu.addAction( tr( "Show the timestamp" ), this, SLOT( onAddTimestampClicked() ) );
  act->setCheckable( true );
  act->setChecked( Settings::instance().homeShowMessageTimestamp() );

  custom_context_menu.exec( mapToGlobal( p ) );
}

void GuiHome::loadDefaultChat()
{
  Chat c = ChatManager::instance().defaultChat();
  foreach( ChatMessage cm, c.messages() )
  {
    if( cm.isFromSystem() )
      addSystemMessage( cm );
  }
}

void GuiHome::reloadMessages()
{
  QApplication::setOverrideCursor( Qt::WaitCursor );
  mp_teSystem->clear();
  loadDefaultChat();
  QApplication::restoreOverrideCursor();
}

void GuiHome::onAddDatestampClicked()
{
  QAction* act = qobject_cast<QAction*>( sender() );
  if( !act )
    return;

  Settings::instance().setHomeShowMessageDatestamp( act->isChecked() );
  QTimer::singleShot( 100, this, SLOT( reloadMessages() ) );
}

void GuiHome::onAddTimestampClicked()
{
  QAction* act = qobject_cast<QAction*>( sender() );
  if( !act )
    return;

  Settings::instance().setHomeShowMessageTimestamp( act->isChecked() );
  QTimer::singleShot( 100, this, SLOT( reloadMessages() ) );
}

void GuiHome::printActivities()
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
    mp_teSystem->print( dlg->printer() );

  dlg->deleteLater();
}
