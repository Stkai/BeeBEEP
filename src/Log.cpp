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

#include "Log.h"
#include <QApplication>
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QtDebug>
#if QT_VERSION >= 0x050000
  #include <QStandardPaths>
#else
  #include <QDesktopServices>
#endif


Log* Log::mp_instance = NULL;

LogNode& LogNode::operator=( const LogNode& ln )
{
  if( this != &ln )
  {
    m_type = ln.m_type;
    m_text = ln.m_text;
    m_note = ln.m_note;
  }
  return *this;
}

Log::Log()
 : m_logFile(), m_logStream()
{
  m_logStream.setDevice( &m_logFile );
}

Log::~Log()
{
  closeFileStream();
}

bool Log::isLoggingToFile() const
{
  return m_logFile.isOpen();
}

void Log::rebootFileStream( const QString& log_path, bool force_reboot )
{
  if( m_logFile.isOpen() )
  {
    if( !force_reboot && m_logFile.fileName() == log_path )
      return;

    closeFileStream();
  }

  bootFileStream( log_path );
}

bool Log::bootFileStream( const QString& log_path )
{
  m_logFile.setFileName( log_path );

  if( !m_logFile.open( QIODevice::WriteOnly ) )
  {
    qWarning() << "Unable to open the log file" << log_path;
    return false;
  }

  qDebug() << "Logging to file" << log_path;

  dumpLogToFile();

  return true;
}

void Log::closeFileStream()
{
  if( m_logFile.isOpen() )
  {
    qDebug() << "Log file closed";
    m_logStream.flush();
    m_logFile.flush();
    m_logFile.close();
  }
}

QString Log::messageTypeToString( QtMsgType mt ) const
{
  switch( mt )
  {
  case QtWarningMsg: return QString( "[WARNING]" );
  case QtCriticalMsg: return QString( "[CRITIC]" );
  case QtFatalMsg: return QString( "[FATAL]" );
  default:
    return QString( "" );
  }
}

bool Log::dumpLogToFile()
{
  if( !m_logFile.isOpen() )
  {
    qWarning() << "Unable to dump log to the file because it is not opened";
    return false;
  }

  if( m_logList.isEmpty() )
    return false;

  foreach( LogNode ln, m_logList )
  {
    m_logStream << (QString)logNodeToString( ln );
    m_logStream << endl;
  }

  return true;
}

QString Log::logNodeToString( const LogNode& ln ) const
{
  QString sHeader = messageTypeToString( ln.type() );
  return QString( "%1%2%3%4" ).arg( QDateTime::currentDateTime().toString( "hh:mm:ss" ) )
                                      .arg( sHeader.isEmpty() ? " " : QString( " %1 " ).arg( sHeader ) )
                                      .arg( ln.text() )
                                      .arg( ln.note().isEmpty() ? "" : QString( " (%1)" ).arg( ln.note() ) );
}

void Log::add( QtMsgType mt, const QString& log_txt, const QString& log_note )
{
  if( log_txt.isNull() || log_txt.isEmpty() )
    return;

  LogNode ln( mt, log_txt, log_note );

  QString sTmp = logNodeToString( ln );

  if( m_logFile.isOpen() )
    m_logStream << sTmp << endl;

#ifdef BEEBEEP_DEBUG
  fprintf( stderr, "%s\n", log_txt.toLatin1().constData() );
  fflush( stderr );
#endif

  if( mt == QtFatalMsg )
  {
    closeFileStream();
    abort();
  }

  m_logList.append( ln );
}

#if QT_VERSION >= 0x050000
void LogMessageHandler( QtMsgType type, const QMessageLogContext &context, const QString &msg )
{
  if( msg.isNull() || msg.isEmpty() )
    return;

  /*QString sNote = QString( "%1:%2, %3" )
                     .arg( context.file )
                     .arg( context.line )
                     .arg( context.function );
  Log::instance().add( type, msg, sNote );*/

  Q_UNUSED( context );
  Log::instance().add( type, msg, "" );

}

#else

void LogMessageHandler( QtMsgType type, const char *msg )
{
  QString sMessage = msg;

  if( sMessage.isNull() || sMessage.isEmpty() )
    return;

  Log::instance().add( type, sMessage, "" );
}

#endif

void Log::installMessageHandler()
{
#if QT_VERSION >= 0x050000
  qInstallMessageHandler( LogMessageHandler );
#else
  qInstallMsgHandler( LogMessageHandler );
#endif
}

