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
// $Id: Shell.cpp 94 2011-09-12 13:11:56Z mastroddi $
//
//////////////////////////////////////////////////////////////////////

#include "Settings.h"
#include "Shell.h"


Shell::Shell( QObject* parent )
  : QObject( parent )
{
  loadCommands();
}

QString Shell::defaultCommandPrefix()
{
  return "/";
}

void Shell::loadCommands()
{
  qDebug() << "Shell loading commands from path:" << Settings::instance().pluginPath();
  QDir plugin_dir( Settings::instance().pluginPath() );
  if( !plugin_dir.exists() )
  {
    qWarning() << "Plugin folder" << Settings::instance().pluginPath() << "not found";
    return;
  }

  foreach( QString file_name, plugin_dir.entryList() )
    addCommand( plugin_dir.absoluteFilePath( file_name ) );

  qDebug() << "Shell has" << m_commands.size() << "plugins";
}

void Shell::addCommand( const QString& file_path )
{
  QPluginLoader plugin_loader( file_path );
  QObject* plugin = plugin_loader.instance();
  if( plugin )
  {
    ShellCommand* shell_cmd = qobject_cast<ShellCommand*>( plugin );
    if( shell_cmd )
    {
      connect( shell_cmd, SIGNAL( message( const QString& ) ), this, SIGNAL( message( const QString& ) ) );
      m_commands.insert( shell_cmd->command(), shell_cmd );
    }
    else
      qDebug() << file_path << "is not a shell plugin";
  }
  else
    qDebug() << file_path << "is not a plugin";
}

bool Shell::parseCommand( const QString& cmd_to_parse )
{
  QStringList sl = cmd_to_parse.simplified().split( ' ', QString::SkipEmptyParts );
  if( sl.size() == 0 )
    return false;

  QString cmd = sl.at( 0 );

  if( !cmd.startsWith( defaultCommandPrefix() ) )
    return false;

  cmd.remove( 0, defaultCommandPrefix().size() );

  ShellCommand* shell_cmd = m_commands.value( cmd, 0 );

  if( shell_cmd )
  {
    qDebug() << "Shell has found command:" << cmd;
    if( shell_cmd->isBusy() )
    {
      qDebug() << "Command is busy: try later";
    }

    sl.removeFirst();
    shell_cmd->setArguments( sl );
    QTimer::singleShot( 0, shell_cmd, SLOT( start() ) );
    return true;
  }
  return false;
}


