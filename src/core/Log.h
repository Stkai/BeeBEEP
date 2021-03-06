//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2021 Marco Mastroddi
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


#ifndef BEEBEEP_LOG_H
#define BEEBEEP_LOG_H

#include <QFile>
#include <QStringList>
#include <QTextStream>
#include <list>


class LogNode
{
public:
  LogNode( QtMsgType mt = QtDebugMsg, const QString& txt = "", const QString& nt = "" )
    : m_type( mt ), m_text( txt ), m_note( nt ) {}
  LogNode( const LogNode& ln ) { (void)operator=( ln ); }

  LogNode& operator=( const LogNode& );

  inline QtMsgType type() const { return m_type; }
  inline const QString& text() const { return m_text; }
  inline const QString& note() const { return m_note; }

private:
  QtMsgType m_type;
  QString m_text;
  QString m_note;

};

class Log
{
public:
  static void installMessageHandler();

  void rebootFileStream( const QString& log_path, bool force_reboot );
  bool bootFileStream( const QString& log_path ); // after load Settings
  void closeFileStream();

  inline void setMaxLogLines( int );

  void add( QtMsgType, const QString& log_txt, const QString& log_note );
  inline void clear();
  inline const std::list<std::string>& toList() const;

  bool isLoggingToFile() const;

  static Log& instance()
  {
    if( !mp_instance )
      mp_instance = new Log();
    return *mp_instance;
  }

  static void close()
  {
    if( mp_instance )
    {
      delete mp_instance;
      mp_instance = NULL;
    }
  }

protected:
  Log();
  ~Log();

  bool dumpLogToFile();
  void checkFileSize();

  QString messageTypeToString( QtMsgType ) const;
  QString logNodeToString( const LogNode& ) const;

private:
  static Log* mp_instance;

  QFile m_logFile;
  QTextStream m_logStream;
  std::list<std::string> m_logList;
  std::list<std::string>::size_type m_maxLogLines;

};


// Inline Functions
inline void Log::setMaxLogLines( int new_value ) { m_maxLogLines = new_value; }
inline void Log::clear() { m_logList.clear(); }
inline const std::list<std::string>& Log::toList() const { return m_logList; }


#endif // BEEBEEP_LOG_H
