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
  LifeBoard( QWidget *parent = 0 );

  bool isPaused() const;

public slots:
  void start();
  void pause();

signals:
  void started();
  void paused();

protected:
  void paintEvent( QPaintEvent* );
  void timerEvent( QTimerEvent* );

private:
  enum { BoardWidth = 10, BoardHeight = 10 };

  inline int squareWidth() const;
  inline int squareHeight() const;
  inline QSize sizeHint() const;
  inline QSize minimumSizeHint() const;

  void clearBoard();
 
 
  void drawSquare( QPainter&, int, int, bool );

  QBasicTimer m_timer;

  bool m_isStarted;
  bool m_isPaused;

  bool m_board[ BoardWidth ][ BoardHeight ];

};


// Inline Functions
inline QSize LifeBoard::sizeHint() const { return QSize( BoardWidth * 15 + frameWidth() * 2, BoardHeight * 15 + frameWidth() * 2 ); }
inline QSize LifeBoard::minimumSizeHint() const { return QSize( BoardWidth * 5 + frameWidth() * 2, BoardHeight * 5 + frameWidth() * 2 ); }
inline int LifeBoard::squareWidth() const { return contentsRect().width() / BoardWidth; }
inline int LifeBoard::squareHeight() const { return contentsRect().height() / BoardHeight; }
inline bool LifeBoard::isPaused() const { return m_isPaused; }

#endif // BEE_LIFEBOARD_H

