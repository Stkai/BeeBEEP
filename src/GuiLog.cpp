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

#include "GuiLog.h"
#include "Log.h"

GuiLog::GuiLog( QWidget* parent )
  : QWidget( parent )
{
  setupUi( this );
  setObjectName( "GuiLog" );

  m_timer.setInterval( 1000 );
  connect( &m_timer, SIGNAL( timeout() ), this, SLOT( refreshLog() ) );
}

void GuiLog::startCheckingLog()
{
  if( !m_timer.isActive() )
  {
    refreshLog();
    m_timer.start();
  }
}

void GuiLog::stopCheckingLog()
{
  if( m_timer.isActive() )
    m_timer.stop();
}

void GuiLog::refreshLog()
{
  foreach( LogNode ln, Log::instance().toList() )
  {
    mp_teLog->appendPlainText( Log::instance().logNodeToString( ln ) );
  }

  Log::instance().clear();
}
