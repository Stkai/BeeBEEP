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

#ifndef BEE_LIFEBOARD_H
#define BEE_LIFEBOARD_H

#include <QBasicTimer>
#include <QFrame>
#include <QLabel>
#include <QPointer>


class LifeBoard : public QFrame
{
  Q_OBJECT

public:
  enum { BoardWidth = 120, BoardHeight = 60 };

  LifeBoard( QWidget *parent = 0 );

  inline bool isPaused() const;
  int aliveCount() const;
  inline int steps() const;
  int diedCount() const;
  int visitedCount() const;
  void pause();
  void restart();

  QString status() const;
  void setStatus( int, const QString& );

signals:
  void paused();
  void running();
  void evolved();
  void completed();

protected:
  void paintEvent( QPaintEvent* );
  void timerEvent( QTimerEvent* );
  void keyPressEvent( QKeyEvent* );

  void startOrPause();
  void clearBoard();
  void evolve();
  void bigBang();

private:
  inline int squareWidth() const;
  inline int squareHeight() const;
  inline QSize sizeHint() const;
  inline QSize minimumSizeHint() const;
  inline int stepTimeout() const;

  void drawSquare( QPainter&, int, int, bool, bool );

  QBasicTimer m_timer;

  bool m_isPaused;

  bool m_board[ BoardWidth ][ BoardHeight ];
  bool m_visited[ BoardWidth ][ BoardHeight ];
  int m_steps;
  int m_evolutionCycle;
};


// Inline Functions
inline QSize LifeBoard::sizeHint() const { return QSize( BoardWidth * 12 + frameWidth() * 2, BoardHeight * 12 + frameWidth() * 2 ); }
inline QSize LifeBoard::minimumSizeHint() const { return QSize( BoardWidth * 4 + frameWidth() * 2, BoardHeight * 4 + frameWidth() * 2 ); }
inline int LifeBoard::squareWidth() const { return frameGeometry().width() / BoardWidth; }
inline int LifeBoard::squareHeight() const { return frameGeometry().height() / BoardHeight; }
inline int LifeBoard::stepTimeout() const { return 150; }
inline bool LifeBoard::isPaused() const { return m_isPaused; }
inline int LifeBoard::steps() const { return m_steps; }

#endif // BEE_LIFEBOARD_H

