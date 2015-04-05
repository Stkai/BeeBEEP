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

#include <QMessageBox>
#include "GuiLife.h"


GuiLife::GuiLife( QWidget *parent )
  : QWidget( parent )
{
  setupUi( this );

  connect( mp_pbRestart, SIGNAL( clicked() ), this, SLOT( restart() ) );
  connect( mp_pbClear, SIGNAL( clicked() ), this, SLOT( clear() ) );
  connect( mp_sliderSpeed, SIGNAL( valueChanged( int ) ), this, SLOT( setNewSpeed( int ) ) );
  connect( mp_life, SIGNAL( paused() ), this, SLOT( gamePaused() ) );
  connect( mp_life, SIGNAL( running() ), this, SLOT( gameRunning() ) );
  connect( mp_life, SIGNAL( evolved() ), this, SLOT( updateCounter() ) );
  connect( mp_life, SIGNAL( completed() ), this, SLOT( gameCompleted() ) );

  updateCounter();
}

void GuiLife::enableControls( bool enable )
{
  mp_sliderSpeed->setEnabled( enable );
  mp_labelFast->setEnabled( enable );
  mp_labelSlow->setEnabled( enable );
}

void GuiLife::gamePaused()
{
  mp_labelPause->setText( tr( "Paused (press space bar to continue)" ) );
  mp_labelPause->show();
  enableControls( true );
}

void GuiLife::gameRunning()
{
  mp_labelPause->setText( "" );
  mp_labelPause->hide();
  enableControls( false );
}

void GuiLife::updateCounter()
{
  mp_labelCount->setText( tr( "Alive: %1 - Died: %2 - Spaces: %3 - Step: %4" )
    .arg( mp_life->aliveCount(), 4, 10, QLatin1Char( ' ' ) )
    .arg( mp_life->diedCount(), 4, 10, QLatin1Char( ' ' ) )
    .arg( mp_life->visitedCount(), 4, 10, QLatin1Char( ' ' ) )
    .arg( mp_life->steps() ) );
}

void GuiLife::restart()
{
  if( QMessageBox::question( this, tr( "BeeLife" ),
    tr( "Do you really want to randomize and restart?" ), tr( "Yes" ), tr( "No" ), QString(), 1, 1 ) == 0 )
  {
    mp_life->restart();
  }
}

void GuiLife::clear()
{
  if( QMessageBox::question( this, tr( "BeeLife" ),
    tr( "Do you really want to clear all?" ), tr( "Yes" ), tr( "No" ), QString(), 1, 1 ) == 0 )
  {
    mp_life->clear();
  }
}

void GuiLife::gameCompleted()
{
  mp_labelPause->setText( tr( "??? Evolution Completed ??? ... (or press space bar to continue)" ) );
  mp_labelPause->show();
  enableControls( true );
}

void GuiLife::setNewSpeed( int new_value )
{
  mp_life->setStepTimeout( new_value );
}

void GuiLife::setStepTimeout( int new_value )
{
  mp_sliderSpeed->setValue( new_value );
  mp_life->setStepTimeout( new_value );
}
