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

#include "GuiLife.h"


GuiLife::GuiLife( QWidget *parent )
  : QWidget( parent ), m_record( 0 )
{
  setupUi( this );

  connect( mp_pbStart, SIGNAL( clicked() ), mp_life, SLOT( start() ) );
  connect( mp_pbPause, SIGNAL( clicked() ), mp_life, SLOT( pause() ) );
  connect( mp_life, SIGNAL( levelChanged( int ) ), this, SLOT( updateLevel( int ) ) );
  connect( mp_life, SIGNAL( scoreChanged( int ) ), this, SLOT( updateScore( int ) ) );
  connect( mp_life, SIGNAL( linesRemovedChanged( int ) ), this, SLOT( updateLines( int ) ) );
  connect( mp_life, SIGNAL( started() ), this, SLOT( gameStarted() ) );
  connect( mp_life, SIGNAL( paused() ), this, SLOT( gamePaused() ) );
  connect( mp_life, SIGNAL( gameOver() ), this, SLOT( gameOver() ) );

  mp_pbPause->setEnabled( false );
}

void GuiLife::updateScore( int new_value )
{
  mp_lcdScore->display( new_value );

  if( new_value > m_record )
    setNewRecord( new_value );
}

void GuiLife::setNewRecord( int new_value )
{
  m_record = new_value;
  mp_lcdRecord->display( m_record );
}

void GuiLife::updateLevel( int new_value )
{
  mp_lcdLevel->display( new_value );
}

void GuiLife::updateLines( int new_value )
{
  mp_lcdLines->display( new_value );
}

void GuiLife::gameStarted()
{
  mp_pbStart->setEnabled( false );
  mp_pbPause->setEnabled( true );
}

void GuiLife::gamePaused()
{
  if( mp_life->isPaused() )
    mp_pbPause->setText( tr( "Continue" ) );
  else
    mp_pbPause->setText( tr( "Pause" ) );
}

void GuiLife::gameOver()
{
  mp_pbStart->setEnabled( true );
  mp_pbPause->setEnabled( false );
}
