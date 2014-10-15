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

#include "Log.h"
#include "Settings.h"
#include <QtDebug>

Log* Log::mp_instance = NULL;


Log::Log()
 : m_logFile(), m_logStream()
{
  m_logStream.setDevice( &m_logFile );
}

Log::~Log()
{
  closeFileStream();
}

QString Log::filePathFromSettings() const
{
  return QString( "%1/%2.log" ).arg( Settings::instance().logPath(), Settings::instance().programName().toLower() );
}

void Log::rebootFileStream( bool force_reboot )
{
  if( m_logFile.isOpen() )
  {
    if( !force_reboot && m_logFile.fileName() == filePathFromSettings() )
      return;

    closeFileStream();
  }

  bootFileStream();
}

bool Log::bootFileStream()
{
  QString log_path = filePathFromSettings();
  m_logFile.setFileName( log_path );

  if( !m_logFile.open( QIODevice::WriteOnly ) )
  {
    qWarning() << "Unable to open the log file" << log_path;
    return false;
  }

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

void Log::add( const QString& log_txt )
{
  m_logList << log_txt;
  if( m_logFile.isOpen() )
    m_logStream << log_txt << endl;

#ifdef BEEBEEP_DEBUG
  fprintf( stderr, log_txt.toLatin1().constData() );
  fprintf( stderr, "%c", '\n' );
  fflush( stderr );
#endif
}

#if QT_VERSION >= 0x050000
  void LogMessageHandler( QtMsgType type, const QMessageLogContext &context, const QString &msg )
  {
    if( msg.isNull() || msg.isEmpty() )
      return;

    QString sHeader = "";

    switch( type )
    {
    case QtWarningMsg:
      sHeader = " [WARN] ";
      break;
    case QtCriticalMsg:
      sHeader = " [CRIT] ";
      break;
    case QtFatalMsg:
      abort();
      break;
    default:
      sHeader = " ";
      break;
    }

    if( sHeader.size() < 3 && !Settings::instance().debugMode() )
      return;

    QString sTmp = QString( "%1%2%3 (%4:%5, %6)" )
                     .arg( QDateTime::currentDateTime().toString( "hh:mm:ss" ) )
                     .arg( sHeader )
                     .arg( msg )
                     .arg( context.file )
                     .arg( context.line )
                     .arg( context.function );

    Log::instance().add( sTmp );

  }
#else
  void LogMessageHandler( QtMsgType type, const char *msg )
  {
    QString sHeader = "";
    QString sMessage = msg;

    if( sMessage.isNull() || sMessage.isEmpty() )
      return;

    switch( type )
    {
    case QtWarningMsg:
      sHeader = " [WARN] ";
      break;
    case QtCriticalMsg:
      sHeader = " [CRIT] ";
      break;
    case QtFatalMsg:
      abort();
      break;
    default:
      sHeader = " ";
      break;
    }

    if( sHeader.size() < 3 && !Settings::instance().debugMode() )
      return;

    QString sTmp = QString( "%1%2%3" ).arg( QDateTime::currentDateTime().toString( "hh:mm:ss" ) )
                                      .arg( sHeader )
                                      .arg( sMessage );

    Log::instance().add( sTmp );

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

