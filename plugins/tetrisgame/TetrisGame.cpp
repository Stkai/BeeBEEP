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
#include "GuiTetris.h"
#include "TetrisGame.h"
#include "Version.h"


QString TetrisGame::name() const
{
  return tr( "Tetris" );
}

QString TetrisGame::version() const
{
  return "0.6.1";
}

QString TetrisGame::author() const
{
  return "Marco Mastroddi";
}

QString TetrisGame::help() const
{
  return tr( "Use <b>Left</b> and <b>Right Arrow</b> to move the pieces on the board. <b>Up</b> and <b>Down Arrow</b> to rotate left and right. <b>Space</b> to drop down the piece. <b>D</b> to drop the piece only one line. <b>P</b> to pause the game." );
}

QIcon TetrisGame::icon() const
{
  return QIcon( iconFileName() );
}

QString TetrisGame::iconFileName() const
{
  return QLatin1String( ":/plugins/tetris.png" );
}

int TetrisGame::priority() const
{
  return 1000;
}

QString TetrisGame::coreVersion() const
{
  return QString( BEEBEEP_VERSION );
}


QWidget* TetrisGame::mainWindow() const
{
  return mp_tetris;
}

TetrisGame::TetrisGame()
  : QObject()
{
  setEnabled( true );
  qDebug() << "Tetris game plugin loaded";
  mp_tetris = new GuiTetris();
}

TetrisGame::~TetrisGame()
{
  delete mp_tetris;
}

void TetrisGame::pause()
{
  mp_tetris->board()->pause();
}

bool TetrisGame::isPaused() const
{
  return mp_tetris->board()->isPaused();
}

#if QT_VERSION >= 0x050000
Q_PLUGIN_METADATA(IID "beebeep.plugin.GameInterface/2.0")
#else
Q_EXPORT_PLUGIN2( beetetris, TetrisGame )
#endif


