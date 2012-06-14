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

#ifndef BEE_TETRISBOARD_H
#define BEE_TETRISBOARD_H

#include <QBasicTimer>
#include <QFrame>
#include <QLabel>
#include <QPointer>
#include "TetrisPiece.h"


class TetrisBoard : public QFrame
{
  Q_OBJECT

public:
  TetrisBoard( QWidget *parent = 0 );

  inline void setNextPieceLabel( QLabel* );
  inline QSize sizeHint() const;
  inline QSize minimumSizeHint() const;
  inline bool isPaused() const;

public slots:
  void start();
  void pause();

signals:
  void scoreChanged( int );
  void levelChanged( int );
  void linesRemovedChanged( int );
  void started();
  void paused();
  void gameOver();

protected:
  void paintEvent( QPaintEvent* );
  void keyPressEvent( QKeyEvent* );
  void timerEvent( QTimerEvent* );
  void focusOutEvent( QFocusEvent* );

private:
  enum { BoardWidth = 10, BoardHeight = 22 };

  inline TetrisPiece::Shape& shapeAt( int, int );
  inline int timeoutTime() const;
  inline int squareWidth() const;
  inline int squareHeight() const;

  void clearBoard();
  void dropDown();
  void oneLineDown();
  void pieceDropped( int );
  void removeFullLines();
  void newPiece();
  void showNextPiece();
  void setGameOver();

  bool tryMove( const TetrisPiece&, int, int );
  void drawSquare( QPainter&, int, int, TetrisPiece::Shape );

  QBasicTimer m_timer;
  QPointer<QLabel> mp_nextPieceLabel;
  bool m_isStarted;
  bool m_isPaused;
  bool m_isGameOver;
  bool m_isWaitingAfterLine;
  TetrisPiece m_curPiece;
  TetrisPiece m_nextPiece;
  int m_curX;
  int m_curY;
  int m_numLinesRemoved;
  int m_numPiecesDropped;
  int m_score;
  int m_level;

  TetrisPiece::Shape m_board[ BoardWidth * BoardHeight ];

};


// Inline Functions
inline void TetrisBoard::setNextPieceLabel( QLabel *label ) { mp_nextPieceLabel = label; }
inline QSize TetrisBoard::sizeHint() const { return QSize( BoardWidth * 15 + frameWidth() * 2, BoardHeight * 15 + frameWidth() * 2 ); }
inline QSize TetrisBoard::minimumSizeHint() const { return QSize( BoardWidth * 5 + frameWidth() * 2, BoardHeight * 5 + frameWidth() * 2 ); }
inline TetrisPiece::Shape& TetrisBoard::shapeAt( int x, int y ) { return m_board[ (y * BoardWidth) + x ]; }
inline int TetrisBoard::timeoutTime() const { return 1000 / (1 + m_level); }
inline int TetrisBoard::squareWidth() const { return contentsRect().width() / BoardWidth; }
inline int TetrisBoard::squareHeight() const { return contentsRect().height() / BoardHeight; }
inline bool TetrisBoard::isPaused() const { return m_isPaused; }

#endif // BEE_TETRISBOARD_H

