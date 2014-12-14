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
// $Id: GuiChatList.cpp 268 2014-11-20 18:42:24Z mastroddi $
//
//////////////////////////////////////////////////////////////////////

#include "BeeApplication.h"


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

bool BeeApplication::notify( QObject* receiver, QEvent* event )
{
  if( event->type() == QEvent::MouseMove || event->type() == QEvent::KeyPress )
  {
    m_lastEventDateTime = QDateTime::currentDateTime();
    if( m_isInIdle )
    {
      emit( exitingFromIdle() );
      m_isInIdle = false;
    }
  }

  return QApplication::notify( receiver, event );
}

void BeeApplication::checkIdle()
{
  if( m_lastEventDateTime.secsTo( QDateTime::currentDateTime() ) > m_idleTimeout )
  {
    if( !m_isInIdle )
    {
      m_isInIdle = true;
      emit( checkIdleRequest() );
    }
  }
}

void BeeApplication::cleanUp()
{
  if( m_timer.isActive() )
    m_timer.stop();
}


