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

#include "BeeApplication.h"
#include <QDebug>
#ifdef Q_OS_WIN
#include <windows.h>
#endif

BeeApplication::BeeApplication( int& argc, char** argv  )
  : QApplication( argc, argv )
{
  setObjectName( "BeeApplication" );
  m_idleTimeout = 0;
  m_timer.setObjectName( "BeeMainTimer" );
  m_timer.setInterval( 10000 );
  m_isInIdle = false;
#ifdef Q_OS_LINUX
  if( testAttribute( Qt::AA_DontShowIconsInMenus ) )
    setAttribute( Qt::AA_DontShowIconsInMenus, false );
#endif

  connect( &m_timer, SIGNAL( timeout() ), this, SLOT( checkIdle() ) );
}


void BeeApplication::setIdleTimeout( int new_value )
{
  m_idleTimeout = new_value * 60;
  if( m_timer.isActive() )
    return;
  m_timer.start();
}

void BeeApplication::setIdle()
{
  if( m_isInIdle )
    return;

  m_isInIdle = true;
  emit( enteringInIdle() );
}

void BeeApplication::removeIdle()
{
  if( !m_isInIdle )
    return;

  m_isInIdle = false;
  emit( exitingFromIdle() );
}

bool BeeApplication::notify( QObject* receiver, QEvent* event )
{
  if( event->type() == QEvent::MouseMove || event->type() == QEvent::KeyPress )
  {
    m_lastEventDateTime = QDateTime::currentDateTime();
    if( m_isInIdle )
      removeIdle();
  }

  return QApplication::notify( receiver, event );
}

void BeeApplication::checkIdle()
{
  if( isScreenSaverRunning() || idleTimeFromSystem() > m_idleTimeout )
    setIdle();
  else
    removeIdle();
}

void BeeApplication::cleanUp()
{
  if( m_timer.isActive() )
    m_timer.stop();
}

bool BeeApplication::isScreenSaverRunning()
{
#ifdef Q_OS_WIN
   BOOL is_running = FALSE;
   SystemParametersInfo( SPI_GETSCREENSAVERRUNNING, 0, &is_running, 0 );
   return (bool)is_running;
#endif
   return false;
}

int BeeApplication::idleTimeFromSystem()
{
  int idle_time = -1;
#ifdef Q_OS_WIN
  LASTINPUTINFO idle_info;
  idle_info.cbSize = sizeof( LASTINPUTINFO );
  if( ::GetLastInputInfo( &idle_info ) )
    idle_time = ::GetTickCount() - idle_info.dwTime;
  idle_time = qMax( 0, idle_time / 1000 );
#else
  idle_time = qMax( 0, m_lastEventDateTime.secsTo( QDateTime::currentDateTime() ) );
#endif

  return idle_time;
}



