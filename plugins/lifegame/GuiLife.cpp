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

#include <QMessageBox>
#include "GuiLife.h"


GuiLife::GuiLife( QWidget *parent )
  : QWidget( parent )
{
  setupUi( this );

  connect( mp_pbRestart, SIGNAL( clicked() ), this, SLOT( restart() ) );
  connect( mp_life, SIGNAL( paused() ), this, SLOT( gamePaused() ) );
  connect( mp_life, SIGNAL( running() ), this, SLOT( gameRunning() ) );
  connect( mp_life, SIGNAL( evolved() ), this, SLOT( updateCounter() ) );

  updateCounter();
}

void GuiLife::gamePaused()
{
  mp_labelPause->setText( tr( "Paused (press space bar to continue)" ) );
  mp_labelPause->show();
}

void GuiLife::gameRunning()
{
  mp_labelPause->setText( "" );
  mp_labelPause->hide();
}

void GuiLife::updateCounter()
{
  mp_labelCount->setText( tr( "Bees: %1 (%2% - %3) Generation: %4" )
    .arg( mp_life->count(), 4, 10, QLatin1Char( ' ' ) )
    .arg( mp_life->percentage(), 5, 'g', 3, QLatin1Char( ' ' ) )
    .arg( (int)(LifeBoard::BoardWidth*LifeBoard::BoardHeight) )
    .arg( mp_life->steps() ) );
}

void GuiLife::restart()
{
  if( QMessageBox::question( this, tr( "BeeLife" ),
    tr( "Do you really want to restart?" ), tr( "Yes" ), tr( "No"), QString(), 1, 1 ) == 0 )
  {
    mp_life->restart();
  }
}
