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
// $Id: ShellCommand.h 89 2011-09-11 19:23:38Z mastroddi $
//
//////////////////////////////////////////////////////////////////////

#ifndef BEEBEEP_SHELLCOMMAND_H
#define BEEBEEP_SHELLCOMMAND_H

#include "Config.h"


class ShellCommand : public QObject
{
  Q_OBJECT

public:
  ShellCommand( const QString&, QObject* parent );
  virtual ~ShellCommand();

  inline const QString& command() const;
  inline const QStringList& arguments() const;
  inline void setArguments( const QStringList& );

public slots:
  virtual void start();

signals:
  void message( const QString& );

protected:
  virtual bool checkArguments();
  virtual void execute() = 0;
  virtual void print( const QString& );
  virtual void usage() = 0;

private:
  QString m_cmd;
  QStringList m_args;

};


// Inline Functions
inline const QString& ShellCommand::command() const { return m_cmd; }
inline const QStringList& ShellCommand::arguments() const { return m_args; }
inline void ShellCommand::setArguments( const QStringList& new_value ) { m_args = new_value; }

#endif // BEEBEEP_SHELLCOMMAND_H
