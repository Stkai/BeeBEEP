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

#include "TickManager.h"


TickManager::TickManager( QObject *parent )
  : QObject( parent ), mp_timer( Q_NULLPTR ), m_ticks( 0 ), m_mutex()
{
  setObjectName( "TickManager" );
}

TickManager::~TickManager()
{
  if( isActive() )
    stopTicks();
}

bool TickManager::isActive() const
{
  QMutexLocker mutex_locker( &m_mutex );
  if( mp_timer && mp_timer->isActive() )
    return true;
  else
    return false;
}

void TickManager::startTicks()
{
  QMutexLocker mutex_locker( &m_mutex );
  m_ticks = 2;
  if( mp_timer )
    return;

#ifdef BEEBEEP_DEBUG
  qDebug() << "TickManager started:" << m_ticks << "ticks";
#endif
  mp_timer = new QTimer;
  mp_timer->setInterval( TICK_INTERVAL );
  connect( mp_timer, SIGNAL( timeout() ), this, SLOT( onTimerTimeout() ) );
  mp_timer->start();
}

void TickManager::stopTicks()
{
  QMutexLocker mutex_locker( &m_mutex );
  if( !mp_timer )
    return;

#ifdef BEEBEEP_DEBUG
  qDebug() << "TickManager stopped:" << m_ticks << "ticks";
#endif
  mp_timer->stop();
  mp_timer->deleteLater();
  mp_timer = Q_NULLPTR;
}

void TickManager::onTimerTimeout()
{
  if( mp_timer )
  {
    m_ticks++;
    emit tickEvent( m_ticks );
  }
}
