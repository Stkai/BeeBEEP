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

#include "ShareDesktopJob.h"


ShareDesktopJob::ShareDesktopJob( QObject *parent )
  : QObject( parent ), m_framePerSecond( 1 ), m_cancelJob( false ),
    m_msecToShot( 1000 ), m_toUserId( ID_INVALID )
{
  setObjectName( "ShareDesktopJob" );
}

void ShareDesktopJob::startJob()
{
  m_cancelJob = false;
  QTimer::singleShot( m_msecToShot, this, SLOT( makeScreenshot() ) );
}

void ShareDesktopJob::stopJob()
{
  m_cancelJob = true;
}

void ShareDesktopJob::makeScreenshot()
{
  QPixmap screen_shot;
  qreal device_pixel_ratio;

#if QT_VERSION >= 0x050000
  device_pixel_ratio = qApp->devicePixelRatio();
#else
  device_pixel_ratio = 1.0;
#endif

#if QT_VERSION >= 0x050000
  QScreen* primary_screen = QApplication::primaryScreen();
  if( primary_screen )
    screen_shot = primary_screen->grabWindow( 0 );
  screen_shot.setDevicePixelRatio( device_pixel_ratio );
#else
  screen_shot = QPixmap::grabWindow( QApplication::desktop()->winId(), 0, 0,
                                     QApplication::desktop()->width() * device_pixel_ratio,
                                     QApplication::desktop()->height() * device_pixel_ratio );
#endif

  if( m_toUserId != ID_INVALID )
    emit imageAvailable( screen_shot, m_toUserId );

  if( !m_cancelJob )
    QTimer::singleShot( m_msecToShot, this, SLOT( makeScreenshot() ) );
  else
    emit jobCompleted();
}
