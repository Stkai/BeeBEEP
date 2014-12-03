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

#include <QtPlugin>
#include <QtDebug>
#include "GuiLife.h"
#include "LifeGame.h"
#include "Version.h"


QString LifeGame::name() const
{
  return tr( "BeeLife" );
}

QString LifeGame::version() const
{
  return "0.1.2";
}

QString LifeGame::author() const
{
  return "Marco Mastroddi";
}

QString LifeGame::help() const
{
  return tr( "<b>The Game of Life</b>, also known simply as Life, is a cellular automaton devised by the British mathematician <b>John Horton Conway</b> in 1970."
  "The universe of the Game of Life is an infinite two-dimensional orthogonal grid of square cells, each of which is in one of two possible states, "
  "alive or dead. Every cell interacts with its eight neighbours, which are the cells that are horizontally, vertically, or diagonally adjacent. "
  "At each step in time, the following transitions occur:"
  "<ul>"
  "<li>Any live cell with less than two live neighbours dies, as if caused by under-population.</li>"
  "<li>Any live cell with two or three live neighbours lives on to the next generation.</li>"
  "<li>Any live cell with more than three live neighbours dies, as if by overcrowding.</li>"
  "<li>Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.</li>"
  "</ul>"
  "The rules continue to be applied repeatedly to create further generations.<br />"
  "For more info please visit <a href=""http://en.wikipedia.org/wiki/Conway%27s_Game_of_Life"">Conway's Game of Life</a>."
   );
}

QIcon LifeGame::icon() const
{
  return QIcon( iconFileName() );
}

QString LifeGame::iconFileName() const
{
  return QLatin1String( ":/plugins/life.png" );
}

int LifeGame::priority() const
{
  return 1000;
}

QString LifeGame::coreVersion() const
{
  return QString( BEEBEEP_VERSION );
}


QWidget* LifeGame::mainWindow() const
{
  return mp_life;
}

LifeGame::LifeGame()
  : QObject()
{
  setEnabled( true );
  qDebug() << name() <<  "game plugin loaded";
  mp_life = new GuiLife();
}

LifeGame::~LifeGame()
{
  delete mp_life;
}

void LifeGame::pause()
{
  mp_life->board()->pause();
}

bool LifeGame::isPaused() const
{
  return mp_life->board()->isPaused();
}

void LifeGame::setSettings( QStringList settings_list )
{
  if( settings_list.size() < 2 )
    return;

  bool ok = false;
  int record_version = settings_list.first().toInt( &ok );
  if( !ok )
    return;
  if( record_version > 1 )
    return;
  settings_list.removeFirst();
  int record_tmp = settings_list.first().toInt( &ok );
  if( !ok )
    return;

}

QStringList LifeGame::settings() const
{
  QStringList settings_list;
  settings_list << QString::number( 1 ); // version of record
  settings_list << QString::number( 20 );
  return settings_list;
}

#if QT_VERSION >= 0x050000
Q_PLUGIN_METADATA(IID "beebeep.plugin.GameInterface/2.0")
#else
Q_EXPORT_PLUGIN2( beelife, LifeGame )
#endif


