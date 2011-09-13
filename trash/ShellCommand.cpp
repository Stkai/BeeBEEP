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
// $Id: ShellCommand.cpp 94 2011-09-12 13:11:56Z mastroddi $
//
//////////////////////////////////////////////////////////////////////

#include <QtDebug>
#include "ShellCommand.h"


ShellCommand::ShellCommand( const QString& cmd )
  : QObject(), m_cmd( cmd ), m_args(), m_usage( "" ), m_busy( false )
{
  qDebug() << "Shell command" << m_cmd << "created";
}

ShellCommand::~ShellCommand()
{
  qDebug() << "Shell command" << m_cmd << "deleted";
}

bool ShellCommand::checkArguments()
{
  return true;
}

void ShellCommand::start()
{
  m_busy = true;
  if( !checkArguments() )
    usage();
  else
    execute();
  m_busy = false;
}

void ShellCommand::print( const QString& msg )
{
  emit message( msg );
}

void ShellCommand::usage()
{
  print( tr( "Usage: %1" ).arg( m_usage ) );
}
