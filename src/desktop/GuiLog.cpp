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
#include "FileDialog.h"
#include "GuiLog.h"
#include "Log.h"
#include "Settings.h"


GuiLog::GuiLog( QWidget* parent )
  : QMainWindow( parent )
{
  setObjectName( "GuiLog" );
  setWindowTitle( QString( "%1 - %2" ).arg( tr( "Log" ), Settings::instance().programName() ) );
  setWindowIcon( QIcon( ":/images/log.png" ) );

  mp_teLog = new QPlainTextEdit( this );
  mp_teLog->setUndoRedoEnabled( false );
  mp_teLog->setReadOnly( true );
  mp_teLog->setContextMenuPolicy( Qt::CustomContextMenu );
  mp_teLog->setPlainText( QString( " \n" ) );

  QPalette p = mp_teLog->palette();
  p.setColor( QPalette::Highlight, Qt::yellow );
  p.setColor( QPalette::HighlightedText, Qt::black );
  mp_teLog->setPalette( p );
  connect( mp_teLog, SIGNAL( customContextMenuRequested( const QPoint& ) ), this, SLOT( openLogMenu( const QPoint& ) ) );

  setCentralWidget( mp_teLog );

  mp_actOpenLogFilePath = new QAction( QIcon( ":/images/log.png" ), tr( "Show log file" ), this );
  connect( mp_actOpenLogFilePath, SIGNAL( triggered() ), this, SLOT( openLogFilePath() ) );

  mp_barLog = new QToolBar( tr( "Show the log tool bar" ), this );
  mp_barLog->setObjectName( "GuiLogToolBar" );
  addToolBar( Qt::BottomToolBarArea, mp_barLog );
  mp_barLog->setAllowedAreas( Qt::BottomToolBarArea | Qt::TopToolBarArea );
  mp_barLog->setFloatable( false );
  mp_barLog->setIconSize( Settings::instance().mainBarIconSize() );
  setupToolBar( mp_barLog );

  mp_logMenu = new QMenu( tr( "Log menu" ), this );

  m_timer.setInterval( 1000 );
  connect( &m_timer, SIGNAL( timeout() ), this, SLOT( refreshLog() ) );

}

void GuiLog::setupToolBar( QToolBar* bar )
{
  /* save as button */
  bar->addAction( QIcon( ":/images/save-as.png" ), tr( "Save log as" ), this, SLOT( saveLogAs() ) );
  bar->addSeparator();
  mp_cbLogToFile = new QCheckBox( bar );
  mp_cbLogToFile->setObjectName( "GuiCheckBoxLogToFileInLog" );
  mp_cbLogToFile->setText( tr( "Log to file" ) );
  mp_cbLogToFile->setChecked( Settings::instance().logToFile() );
  mp_cbLogToFile->setToolTip( Settings::instance().logFilePath() );
  connect( mp_cbLogToFile, SIGNAL( clicked( bool ) ), this, SLOT( logToFile( bool ) ) );
  bar->addWidget( mp_cbLogToFile );
  bar->addAction( mp_actOpenLogFilePath );
  mp_actOpenLogFilePath->setEnabled( Settings::instance().logToFile() );
  bar->addSeparator();

  /* filter by keywords */
  QLabel* label = new QLabel( bar );
  label->setObjectName( "GuiLabelFilterTextLog" );
  label->setAlignment( Qt::AlignRight | Qt::AlignTrailing | Qt::AlignVCenter );
  label->setText( QString( "   " ) + tr( "Search" ) + QString( " " ) );
  bar->addWidget( label );
  mp_leFilter = new QLineEdit( bar );
  mp_leFilter->setObjectName( "GuiLineEditFilterLog" );
  mp_leFilter->setMaximumWidth( 140 );
#if QT_VERSION >= 0x040700
  mp_leFilter->setPlaceholderText( tr( "keyword" ) );
#endif
  bar->addWidget( mp_leFilter );
  connect( mp_leFilter, SIGNAL( returnPressed() ), this, SLOT( findTextInLog() ) );

  /* search button */
  bar->addAction( QIcon( ":/images/search.png" ), tr( "Find" ), this, SLOT( findTextInLog() ) );

  /* flags */
  mp_cbCaseSensitive = new QCheckBox( bar );
  mp_cbCaseSensitive->setObjectName( "GuiCheckBoxFindCaseSensitiveInLog" );
  mp_cbCaseSensitive->setText( tr( "Case sensitive" ) );
  bar->addWidget( mp_cbCaseSensitive );

  mp_cbWholeWordOnly = new QCheckBox( bar );
  mp_cbWholeWordOnly->setObjectName( "GuiCheckBoxFindWholeWordOnlyInLog" );
  mp_cbWholeWordOnly->setText( tr( "Whole word" ) );
  bar->addWidget( mp_cbWholeWordOnly );
}

void GuiLog::showUp()
{
  if( isMinimized() )
    showNormal();

  if( !isVisible() )
    show();

#ifdef Q_OS_WIN
  SetWindowPos( (HWND)winId(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE );
  SetWindowPos( (HWND)winId(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE );
#else
  raise();
#endif

  static bool log_first_show = true;
  if( log_first_show )
  {
    QTimer::singleShot( 100, this, SLOT( refreshLog() ) );
    log_first_show = false;
  }

  startCheckingLog();
}

void GuiLog::saveLogAs()
{
  QString log_path = FileDialog::getSaveFileName( this,
                          tr( "Please select a file to save the log." ),
                          Settings::instance().logPath(), "TXT Log Files (*.txt)" );
  if( log_path.isEmpty() )
    return;

  QFile log_file( log_path );
  if( !log_file.open( QIODevice::WriteOnly | QIODevice::Text ) )
  {
    qWarning() << "Unable to write log in file:" << log_path;
    QMessageBox::warning( this, Settings::instance().programName(), tr( "Unable to save log in the file: %1" ).arg( log_path ), tr( "Ok" ) );
    return;
  }

  stopCheckingLog();
  refreshLog();

  QTextStream log_stream( &log_file );
  log_stream.setCodec( "UTF-8" );

  QString log_txt = QString( "BeeBEEP log saved in %1" ).arg( QDateTime::currentDateTime().toString( "dd.MM.yyyy-hh:mm:ss.zzz" ) );
  log_txt += QLatin1String( "\n" );
  log_txt += mp_teLog->toPlainText();
  log_txt += QLatin1String( "\n*END*\n" );
  log_stream << log_txt;
  log_stream.flush();

  log_file.close();
  log_txt = "";

  QFileInfo log_file_info( log_path );
  Settings::instance().setLogPath( Bee::convertToNativeFolderSeparator( log_file_info.absolutePath() ) );

  startCheckingLog();

  switch( QMessageBox::information( this, Settings::instance().programName(),
                                tr( "%1: save log completed." ).arg( log_path ),
                                tr( "Ok" ), tr( "Open file" ), tr( "Open folder" ), 0, 0 ) )
  {
  case 1:
    {
      if( !QDesktopServices::openUrl( QUrl::fromLocalFile( log_path ) ) )
        qWarning() << "Unable to open log file" << log_path;
    }
    break;

  case 2:
    {
      if( !QDesktopServices::openUrl( QUrl::fromLocalFile( Settings::instance().logPath() ) ) )
        qWarning() << "Unable to open log folder" << Settings::instance().logPath();
    }
    break;

  default:
    return;

  }
}

void GuiLog::closeEvent( QCloseEvent* e )
{
  stopCheckingLog();
  if( mp_logMenu->isVisible() )
    mp_logMenu->close();
  e->accept();
}

void GuiLog::findTextInLog()
{
  QString txt = mp_leFilter->text().simplified();
  if( txt.isEmpty() )
    return;

  QTextDocument::FindFlags find_flags = 0;
  if( mp_cbCaseSensitive->isChecked() )
    find_flags |= QTextDocument::FindCaseSensitively;
  if( mp_cbWholeWordOnly->isChecked() )
    find_flags |= QTextDocument::FindWholeWords;

  if( !mp_teLog->find( txt, find_flags ) )
  {
    mp_teLog->moveCursor( QTextCursor::Start );
    if( !mp_teLog->find( txt ) )
    {
      QMessageBox::information( this, Settings::instance().programName(), tr( "%1 not found" ).arg( txt ) + QString( "." ), tr( "Ok" ) );
      return;
    }
  }
}

void GuiLog::startCheckingLog()
{
  if( !m_timer.isActive() )
  {
    m_timer.start();
    mp_leFilter->setFocus();
  }
}

void GuiLog::stopCheckingLog()
{
  if( m_timer.isActive() )
    m_timer.stop();
}

void GuiLog::refreshLog()
{
  QString plain_text = "";
  foreach( QString log_line, Log::instance().toList() )
  {
    plain_text += log_line;
    plain_text += QLatin1String( "\n" );
  }

  Log::instance().clear();

  if( !plain_text.isEmpty() )
  {
    QTextCursor cursor( mp_teLog->textCursor() );
    cursor.movePosition( QTextCursor::End );
    cursor.insertText( plain_text );

    if( !m_blockScrolling )
    {
      QScrollBar *bar = mp_teLog->verticalScrollBar();
      if( bar )
      {
        if( !bar->isSliderDown() )
          bar->setValue( bar->maximum() );
      }
      else
        mp_teLog->ensureCursorVisible();
    }
  }
}

void GuiLog::logToFile( bool yes )
{
  Settings::instance().setLogToFile( yes );
  if( yes )
  {
    if( !Log::instance().isLoggingToFile() )
    {
      Log::instance().bootFileStream( Settings::instance().logFilePath() );
    }
  }
  else
  {
    if( Log::instance().isLoggingToFile() )
    {
      Log::instance().closeFileStream();
    }
  }

  mp_actOpenLogFilePath->setEnabled( Settings::instance().logToFile() );
}

void GuiLog::openLogMenu( const QPoint& )
{
  mp_logMenu->clear();
  mp_logMenu->addAction( QIcon( ":/images/select-all.png" ), tr( "Select All" ), mp_teLog, SLOT( selectAll() ), QKeySequence::SelectAll );
  mp_logMenu->addSeparator();
  QAction* act = mp_logMenu->addAction( QIcon( ":/images/copy.png" ), tr( "Copy to clipboard" ), mp_teLog, SLOT( copy() ), QKeySequence::Copy );
  act->setEnabled( !mp_teLog->textCursor().selectedText().isEmpty() );
  mp_logMenu->addSeparator();
  act = mp_logMenu->addAction( tr( "Block scrolling" ), this, SLOT( toggleBlockScrolling() ) );
  act->setCheckable( true );
  act->setChecked( m_blockScrolling );

  mp_logMenu->exec( QCursor::pos() );
}

void GuiLog::toggleBlockScrolling()
{
  m_blockScrolling = !m_blockScrolling;
}

void GuiLog::openLogFilePath()
{
  if( !Bee::showFileInGraphicalShell( Settings::instance().logFilePath() ) )
    QMessageBox::warning( this, Settings::instance().programName(), tr( "%1: log not found." ).arg( Settings::instance().logFilePath() ) );
}
