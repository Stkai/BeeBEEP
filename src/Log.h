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
#include "Settings.h"

namespace Log
{

  QTextStream *LogStream = NULL;
  QFile *LogFile = NULL;
  const QString LogFileName = "beebeep.log";

  void boot()
  {
    if( !Settings::instance().logToFile() )
      return;

    if( !LogStream )
    {
      QString log_path = QString( "%1/%2" ).arg( Settings::instance().logPath(), LogFileName );
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

    if( sHeader.size() < 3 && !Settings::instance().debugMode() )
      return;

    QString sTmp = QString( "%1%2%3" ).arg( QDateTime::currentDateTime().toString( "dd/MM/yyyy hh:mm:ss" ) )
                                      .arg( sHeader )
                                      .arg( sMessage );

    if( LogStream )
    {
      (*LogStream) << sTmp << endl;
    }
    else
    {
      sTmp += QLatin1Char( '\n' );
      fprintf( stderr, sTmp.toLatin1().data() );
      fflush( stderr );
    }
  }
}

#if 0
void make_test()
{
  QByteArray ba16( "1234567890987654" );
  QByteArray ba32( "12345678909876541234567890987654" );
  QByteArray ba39( "abcdefghilmnopgrstuvwyz31234567890hgfdt" );
  QByteArray enc_ba;
  QByteArray dec_ba;

  QList<QByteArray> blist;
  blist << ba16 << ba32 << ba39;

  foreach( QByteArray ba, blist )
  {
    qDebug() << "Encrypt:" << ba;
    enc_ba = Protocol::instance().encryptByteArray( ba );
    qDebug() << "Encrypted:" << enc_ba;
    dec_ba = Protocol::instance().decryptByteArray( enc_ba );
    qDebug() << "Decrypted:" << dec_ba;
    if( dec_ba == ba )
      qDebug() << "OK";
  }
}
#endif

#endif // BEEBEEP_LOG_H
