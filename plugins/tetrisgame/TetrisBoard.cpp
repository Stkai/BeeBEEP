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

#include <QKeyEvent>
#include <QPainter>
#include "TetrisBoard.h"


TetrisBoard::TetrisBoard( QWidget *parent )
 : QFrame( parent )
{
  setFrameStyle( QFrame::StyledPanel );
  setFocusPolicy( Qt::StrongFocus );

  setStyleSheet( "background-image: url(:/plugins/tetris-background.png);"
                 "background-repeat: repeat-y;" );

  m_isStarted = false;
  m_isPaused = false;
  m_isGameOver = false;

  clearBoard();

  m_nextPiece.setRandomShape();
}

void TetrisBoard::start()
{
  if( m_isPaused )
    return;

  m_isStarted = true;
  m_isGameOver = false;
  m_isWaitingAfterLine = false;
  m_numLinesRemoved = 0;
  m_numPiecesDropped = 0;
  m_score = 0;
  m_level = 1;

  clearBoard();

  emit linesRemovedChanged( m_numLinesRemoved );
  emit scoreChanged( m_score );
  emit levelChanged( m_level );

  newPiece();

  m_timer.start( timeoutTime(), this );

  emit( started() );
}

void TetrisBoard::pause()
{
  if( !m_isStarted )
    return;

  m_isPaused = !m_isPaused;

  if( m_isPaused )
    m_timer.stop();
  else
    m_timer.start( timeoutTime(), this );

  update();

  emit( paused() );
}

void TetrisBoard::setGameOver()
{
  m_isGameOver = true;
  mp_nextPieceLabel->setPixmap( QPixmap() );
  update();
  emit( gameOver() );
}

void TetrisBoard::paintEvent( QPaintEvent *event )
{
  QFrame::paintEvent( event );

  QPainter painter(this);
  QRect rect = contentsRect();

  if( m_isPaused || m_isGameOver )
  {
    QFont font = painter.font();
    font.setBold( true );
    font.setPointSize( font.pointSize() + 6 );
    painter.setFont( font );
    painter.drawText( rect, Qt::AlignCenter, m_isPaused ? tr( "Pause" ) : tr( "Game Over" ) );
    return;
  }

  int board_top = rect.bottom() - BoardHeight * squareHeight();

  for( int i = 0; i < BoardHeight; i++ )
  {
    for( int j = 0; j < BoardWidth; j++ )
    {
      TetrisPiece::Shape shape = shapeAt( j, BoardHeight - i - 1 );
      if( shape != TetrisPiece::NoShape )
        drawSquare( painter, rect.left() + j * squareWidth(), board_top + i * squareHeight(), shape );
    }
  }

  if( m_curPiece.shape() != TetrisPiece::NoShape )
  {
    for( int i = 0; i < 4; ++i )
    {
      int x = m_curX + m_curPiece.x( i );
      int y = m_curY - m_curPiece.y( i );

      drawSquare( painter, rect.left() + x * squareWidth(), board_top + (BoardHeight - y - 1) * squareHeight(), m_curPiece.shape() );
    }
  }
}

void TetrisBoard::keyPressEvent( QKeyEvent *event )
{
  if( !m_isStarted || m_isGameOver || m_curPiece.shape() == TetrisPiece::NoShape )
  {
    QFrame::keyPressEvent( event );
    return;
  }

  if( m_isPaused )
  {
    if( event->key() == Qt::Key_P )
      pause();
    else
      QFrame::keyPressEvent( event );
    return;
  }

  switch( event->key() )
  {
  case Qt::Key_Left:
    tryMove( m_curPiece, m_curX - 1, m_curY );
	break;
  case Qt::Key_Right:
    tryMove( m_curPiece, m_curX + 1, m_curY );
	break;
  case Qt::Key_Down:
    tryMove( m_curPiece.rotatedRight(), m_curX, m_curY );
	break;
  case Qt::Key_Up:
    tryMove( m_curPiece.rotatedLeft(), m_curX, m_curY );
	break;
  case Qt::Key_Space:
	dropDown();
	break;
  case Qt::Key_D:
	oneLineDown();
	break;
  case Qt::Key_P:
    pause();
    break;
  default:
    QFrame::keyPressEvent( event );
  }
}

void TetrisBoard::timerEvent( QTimerEvent *event )
{
  if( event->timerId() == m_timer.timerId() )
  {
    if( m_isWaitingAfterLine )
    {
      m_isWaitingAfterLine = false;
      newPiece();
      m_timer.start( timeoutTime(), this );
    }
    else
      oneLineDown();
  }
  else
    QFrame::timerEvent(event);

}

void TetrisBoard::clearBoard()
{
  for( int i = 0; i < BoardHeight * BoardWidth; i++ )
    m_board[ i ] = TetrisPiece::NoShape;
}

void TetrisBoard::dropDown()
{
  int drop_height = 0;
  int newY = m_curY;

  while( newY > 0 )
  {
    if( !tryMove( m_curPiece, m_curX, newY - 1 ) )
      break;
    --newY;
    ++drop_height;
  }

  pieceDropped( drop_height );
}

void TetrisBoard::oneLineDown()
{
  if( !tryMove( m_curPiece, m_curX, m_curY - 1 ) )
    pieceDropped( 0 );
}

void TetrisBoard::pieceDropped( int drop_height )
{
  for( int i = 0; i < 4; i++ )
  {
    int x = m_curX + m_curPiece.x(i);
    int y = m_curY - m_curPiece.y(i);
    shapeAt( x, y ) = m_curPiece.shape();
  }

  ++m_numPiecesDropped;

  if( m_numPiecesDropped % 25 == 0 )
  {
    ++m_level;
    m_timer.start( timeoutTime(), this );
    emit levelChanged( m_level );
  }

  m_score += drop_height + 7;
  emit scoreChanged( m_score );

  removeFullLines();

  if( !m_isWaitingAfterLine )
    newPiece();
}

void TetrisBoard::removeFullLines()
{
  int num_full_lines = 0;

  for( int i = BoardHeight - 1; i >= 0; i-- )
  {
    bool line_is_full = true;

    for( int j = 0; j < BoardWidth; j++ )
    {
      if( shapeAt( j, i ) == TetrisPiece::NoShape )
      {
        line_is_full = false;
        break;
      }
    }

    if( line_is_full )
    {
      ++num_full_lines;

      for( int k = i; k < BoardHeight - 1; k++ )
      {
        for( int j = 0; j < BoardWidth; j++ )
          shapeAt( j, k ) = shapeAt( j, k + 1 );
      }

      for( int j = 0; j < BoardWidth; j++ )
        shapeAt( j, BoardHeight - 1 ) = TetrisPiece::NoShape;
	}

  }

  if( num_full_lines > 0 )
  {
    m_numLinesRemoved += num_full_lines;
    m_score += 10 * num_full_lines;

    emit linesRemovedChanged( m_numLinesRemoved );
    emit scoreChanged( m_score );

    m_timer.start(500, this);
    m_isWaitingAfterLine = true;
    m_curPiece.setShape( TetrisPiece::NoShape );
    update();
  }
}

void TetrisBoard::newPiece()
{
  m_curPiece = m_nextPiece;
  m_nextPiece.setRandomShape();
  showNextPiece();
  m_curX = BoardWidth / 2 + 1;
  m_curY = BoardHeight - 1 + m_curPiece.minY();

  if( !tryMove( m_curPiece, m_curX, m_curY ) )
  {
    m_curPiece.setShape( TetrisPiece::NoShape );
    m_timer.stop();
    m_isStarted = false;
    setGameOver();
  }
}

void TetrisBoard::showNextPiece()
{
  if( !mp_nextPieceLabel )
    return;

  int dx = m_nextPiece.maxX() - m_nextPiece.minX() + 1;
  int dy = m_nextPiece.maxY() - m_nextPiece.minY() + 1;

  QPixmap pix( dx * squareWidth(), dy * squareHeight() );
  QPainter painter( &pix );
  painter.fillRect( pix.rect(), mp_nextPieceLabel->palette().background() );

  for( int i = 0; i < 4; i++ )
  {
    int x = m_nextPiece.x( i ) - m_nextPiece.minX();
    int y = m_nextPiece.y( i ) - m_nextPiece.minY();

    drawSquare( painter, x * squareWidth(), y * squareHeight(), m_nextPiece.shape() );
  }

  mp_nextPieceLabel->setPixmap( pix );
}

bool TetrisBoard::tryMove( const TetrisPiece& new_piece, int newX, int newY )
{
  for( int i = 0; i < 4; i++ )
  {
    int x = newX + new_piece.x( i );
    int y = newY - new_piece.y( i );

    if( x < 0 || x >= BoardWidth || y < 0 || y >= BoardHeight )
      return false;

    if( shapeAt( x, y ) != TetrisPiece::NoShape )
      return false;
  }

  m_curPiece = new_piece;
  m_curX = newX;
  m_curY = newY;
  update();
  return true;
}

void TetrisBoard::drawSquare( QPainter& painter, int x, int y, TetrisPiece::Shape shape )
{
  static const QRgb color_table[ TetrisPiece::NumShapes ] = {
        0x000000, 0xCC6666, 0x66CC66, 0x6666CC,
        0xCCCC66, 0xCC66CC, 0x66CCCC, 0xDAAA00
  };

  QColor color = color_table[ (int)shape ];
  painter.fillRect( x + 1, y + 1, squareWidth() - 2, squareHeight() - 2, color );

  painter.setPen( color.light() );
  painter.drawLine( x, y + squareHeight() - 1, x, y );
  painter.drawLine( x, y, x + squareWidth() - 1, y );

  painter.setPen( color.dark() );
  painter.drawLine( x + 1, y + squareHeight() - 1, x + squareWidth() - 1, y + squareHeight() - 1 );
  painter.drawLine( x + squareWidth() - 1, y + squareHeight() - 1, x + squareWidth() - 1, y + 1 );
}

