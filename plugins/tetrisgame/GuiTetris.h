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
// Author: Marco Mastroddi <marco.mastroddi(AT)gmail.com>
//
// $Id: GuiTetris.h 346 2015-04-05 16:12:37Z mastroddi $
//
//////////////////////////////////////////////////////////////////////

#ifndef BEEBEEP_GUITETRIS_H
#define BEEBEEP_GUITETRIS_H

#if QT_VERSION >= 0x050000
#include <QtWidgets>
#endif
#include "ui_GuiTetris.h"
class TetrisBoard;


class GuiTetris : public QWidget, private Ui::GuiTetris
{
  Q_OBJECT

public:
  explicit GuiTetris( QWidget *parent = 0 );

  inline TetrisBoard* board();

  inline int record() const;
  void setNewRecord( int );

protected slots:
  void updateScore( int );
  void updateLevel( int );
  void updateLines( int );
  void gameStarted();
  void gamePaused();
  void gameOver();

private:
  int m_record;

};


// Inline Functions
inline TetrisBoard* GuiTetris::board() { return mp_board; }
inline int GuiTetris::record() const { return m_record; }


#endif // BEEBEEP_GUITETRIS_H
