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
// $Id: Shell.h 94 2011-09-12 13:11:56Z mastroddi $
//
//////////////////////////////////////////////////////////////////////

#ifndef BEEBEEP_SHELL_H
#define BEEBEEP_SHELL_H

#include "Config.h"
#include "ShellCommand.h"


class Shell : public QObject
{
  Q_OBJECT

public:
  explicit Shell( QObject* );

  static QString defaultCommandPrefix();

  bool parseCommand( const QString& );

signals:
  void message( const QString& );

protected:
  void loadCommands();

private:
  void addCommand( const QString& );

private:
  QMap<QString, ShellCommand*> m_commands;

};


#endif // BEEBEEP_SHELL_H
