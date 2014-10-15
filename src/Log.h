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

#include <QFile>
#include <QStringList>
#include <QTextStream>


class Log
{
public:
  static void installMessageHandler();

  void rebootFileStream( bool force_reboot );
  bool bootFileStream();
  void closeFileStream();

  void add( const QString& );
  inline void clear();
  inline const QStringList& toList() const;

  QString filePathFromSettings() const;

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

private:
  static Log* mp_instance;

  QFile m_logFile;
  QTextStream m_logStream;
  QStringList m_logList;

};


// Inline Functions
inline void Log::clear() { m_logList.clear(); }
inline const QStringList& Log::toList() const { return m_logList; }


#endif // BEEBEEP_LOG_H
