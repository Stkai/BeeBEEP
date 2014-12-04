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

#ifndef BEEBEEP_GUILIFE_H
#define BEEBEEP_GUILIFE_H

#if QT_VERSION >= 0x050000
#include <QtWidgets>
#endif
#include "ui_GuiLife.h"
class LifeBoard;


class GuiLife : public QWidget, private Ui::GuiLife
{
  Q_OBJECT

public:
  explicit GuiLife( QWidget *parent = 0 );

  inline LifeBoard* board();

protected slots:
  void gamePaused();
  void gameRunning();
  void updateCounter();
  void restart();
  void gameCompleted();


};


// Inline Functions
inline LifeBoard* GuiLife::board() { return mp_life; }


#endif // BEEBEEP_GUILIFE_H
