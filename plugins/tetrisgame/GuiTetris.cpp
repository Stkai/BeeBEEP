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
// $Id$
//
//////////////////////////////////////////////////////////////////////

#include "GuiTetris.h"


GuiTetris::GuiTetris( QWidget *parent )
  : QWidget( parent ), m_record( 0 )
{
  setupUi( this );

  mp_board->setNextPieceLabel( mp_labelNewPiece );

  connect( mp_pbStart, SIGNAL( clicked() ), mp_board, SLOT( start() ) );
  connect( mp_pbPause, SIGNAL( clicked() ), mp_board, SLOT( pause() ) );
  connect( mp_board, SIGNAL( levelChanged( int ) ), this, SLOT( updateLevel( int ) ) );
  connect( mp_board, SIGNAL( scoreChanged( int ) ), this, SLOT( updateScore( int ) ) );
  connect( mp_board, SIGNAL( linesRemovedChanged( int ) ), this, SLOT( updateLines( int ) ) );
  connect( mp_board, SIGNAL( started() ), this, SLOT( gameStarted() ) );
  connect( mp_board, SIGNAL( paused() ), this, SLOT( gamePaused() ) );
  connect( mp_board, SIGNAL( gameOver() ), this, SLOT( gameOver() ) );

  mp_pbPause->setEnabled( false );
}

void GuiTetris::updateScore( int new_value )
{
  mp_lcdScore->display( new_value );

  if( new_value > m_record )
    setNewRecord( new_value );
}

void GuiTetris::setNewRecord( int new_value )
{
  m_record = new_value;
  mp_lcdRecord->display( m_record );
}

void GuiTetris::updateLevel( int new_value )
{
  mp_lcdLevel->display( new_value );
}

void GuiTetris::updateLines( int new_value )
{
  mp_lcdLines->display( new_value );
}

void GuiTetris::gameStarted()
{
  mp_pbStart->setEnabled( false );
  mp_pbPause->setEnabled( true );
}

void GuiTetris::gamePaused()
{
  if( mp_board->isPaused() )
    mp_pbPause->setText( tr( "Continue" ) );
  else
    mp_pbPause->setText( tr( "Pause" ) );
}

void GuiTetris::gameOver()
{
  mp_pbStart->setEnabled( true );
  mp_pbPause->setEnabled( false );
}
