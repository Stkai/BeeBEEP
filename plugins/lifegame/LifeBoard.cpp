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
#include "LifeBoard.h"


LifeBoard::LifeBoard( QWidget *parent )
 : QFrame( parent )
{
  setFrameStyle( QFrame::StyledPanel );
  setFocusPolicy( Qt::StrongFocus );

  setStyleSheet( "background-image: url(:/plugins/life-background.png);"
                 "background-repeat: repeat-y;" );

  m_isStarted = false;
  m_isPaused = false;

  clearBoard();

}

void LifeBoard::start()
{
  if( m_isPaused )
    return;

  m_isStarted = true;

  clearBoard();

  m_timer.start( 500, this );

  emit( started() );
}

void LifeBoard::pause()
{
  if( !m_isStarted )
    return;

  m_isPaused = !m_isPaused;

  if( m_isPaused )
    m_timer.stop();
  else
    m_timer.start( 500, this );

  update();

  emit( paused() );
}

void LifeBoard::paintEvent( QPaintEvent *event )
{
  QFrame::paintEvent( event );

  QPainter painter(this);
  QRect rect = contentsRect();

  if( m_isPaused )
  {
    QFont font = painter.font();
    font.setBold( true );
    font.setPointSize( font.pointSize() + 6 );
    painter.setFont( font );
    painter.drawText( rect, Qt::AlignCenter, tr( "Pause" ) );
    return;
  }

  int board_top = rect.bottom() - BoardHeight * squareHeight();

  for( int i = 0; i < BoardHeight; i++ )
  {
    for( int j = 0; j < BoardWidth; j++ )
    {
      drawSquare( painter, rect.left() + j * squareWidth(), board_top + i * squareHeight(), (j % 2 == 0) );
    }
  }
}

void LifeBoard::timerEvent( QTimerEvent* event )
{
  if( event->timerId() == m_timer.timerId() )
  {

  }
  else
    QFrame::timerEvent(event);

}

void LifeBoard::clearBoard()
{

}

void LifeBoard::drawSquare( QPainter& painter, int x, int y, bool is_living )
{
  QColor color = is_living ? QColor( Qt::darkYellow ) : QColor( Qt::black );
  painter.fillRect( x + 1, y + 1, squareWidth() - 2, squareHeight() - 2, color );

  painter.setPen( color.light() );
  painter.drawLine( x, y + squareHeight() - 1, x, y );
  painter.drawLine( x, y, x + squareWidth() - 1, y );

  painter.setPen( color.dark() );
  painter.drawLine( x + 1, y + squareHeight() - 1, x + squareWidth() - 1, y + squareHeight() - 1 );
  painter.drawLine( x + squareWidth() - 1, y + squareHeight() - 1, x + squareWidth() - 1, y + 1 );
}

