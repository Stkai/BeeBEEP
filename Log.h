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


#ifndef BEEBEEP_LOG_H
#define BEEBEEP_LOG_H

#include <QTextStream>
#include <QDateTime>
#include <QtDebug>
#include <QFile>


namespace Log
{

  QTextStream *LogStream = NULL;
  QFile *LogFile = NULL;
  const QString LogFileName = "beebeep.log";

  void boot( const QString& log_dir )
  {
    if( !LogStream )
    {
      QString log_path = QString( "%1/%2" ).arg( log_dir, LogFileName );
      LogFile = new QFile( log_path, 0 );
      if( LogFile->open( QIODevice::WriteOnly ) )
	    LogStream = new QTextStream( LogFile );
	  else
	  {
	    qWarning() << "Unable to open" << log_path;
		delete LogFile;
		LogFile = NULL;
        LogStream = NULL;
	  }
    }
  }

  void close()
  {  
    if( LogStream )
    {
      qDebug() << "Log closed";
      LogStream->flush();
      delete LogStream;
      LogStream = NULL;
    }
	
    if( LogFile )
    {
      LogFile->flush();
      LogFile->close();
      delete LogFile;
      LogFile = NULL;
    }
  }

  void MessageHandler( QtMsgType type, const char *msg )
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

    if( LogStream )
      (*LogStream) << QDateTime::currentDateTime().toString( "dd/MM/yyyy hh:mm:ss" ) 
                   << sHeader 
			       << sMessage.remove( "\"" ) // FIXME: QT does something strange with peer_address toString
			       << endl << flush;
    else
      fprintf( stderr, msg );
  }
}

#endif // BEEBEEP_LOG_H
