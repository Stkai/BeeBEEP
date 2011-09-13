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
// $Id: ShellCommand.h 94 2011-09-12 13:11:56Z mastroddi $
//
//////////////////////////////////////////////////////////////////////

#ifndef BEEBEEP_SHELLCOMMAND_H
#define BEEBEEP_SHELLCOMMAND_H

#include <QObject>
#include <QStringList>
#include "shellcommand_global.h"


class SHELL_COMMAND_SHARED_EXPORT ShellCommand : public QObject
{
  Q_OBJECT

public:
  ShellCommand( const QString& );
  virtual ~ShellCommand();

  inline const QString& command() const;
  inline const QStringList& arguments() const;
  inline void setArguments( const QStringList& );

  inline bool isBusy() const;

public slots:
  void start();

signals:
  void message( const QString& );

protected:
  virtual bool checkArguments();
  virtual void execute() = 0;
  virtual void print( const QString& );
  virtual void usage();

protected:
  QString m_cmd;
  QStringList m_args;
  QString m_usage;
  bool m_busy;

};


// Inline Functions
inline const QString& ShellCommand::command() const { return m_cmd; }
inline const QStringList& ShellCommand::arguments() const { return m_args; }
inline void ShellCommand::setArguments( const QStringList& new_value ) { m_args = new_value; }
inline bool ShellCommand::isBusy() const { return m_busy; }

#endif // BEEBEEP_SHELLCOMMAND_H
