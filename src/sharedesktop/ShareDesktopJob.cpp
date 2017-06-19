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

#include "ImageOptimizer.h"
#include "Settings.h"
#include "ShareDesktopJob.h"


ShareDesktopJob::ShareDesktopJob( QObject *parent )
  : QObject( parent ), m_timer(), m_lastImage(), m_stop( false )
{
  setObjectName( "ShareDesktopJob" );
  m_timer.setInterval( Settings::instance().shareDesktopCaptureDelay() );
  m_timer.setSingleShot( false );
  connect( &m_timer, SIGNAL( timeout() ), this, SLOT( makeScreenshot() ), Qt::QueuedConnection );
}

void ShareDesktopJob::startJob()
{
#ifdef BEEBEEP_DEBUG
  qDebug() << qPrintable( objectName() ) << "is starting its job";
#endif
  if( isRunning() )
  {
    qWarning() << qPrintable( objectName() ) << "is already running";
    return;
  }
  m_stop = false;
  m_lastImage = QImage();
  QMetaObject::invokeMethod( &m_timer, "start", Qt::QueuedConnection );
}

void ShareDesktopJob::stopJob()
{
#ifdef BEEBEEP_DEBUG
  qDebug() << qPrintable( objectName() ) << "is stopping its job";
#endif
  m_stop = true;
  QMetaObject::invokeMethod( &m_timer, "stop", Qt::QueuedConnection );
  emit jobCompleted();
}

bool ShareDesktopJob::isRunning() const
{
  if( !m_stop )
    return m_timer.isActive();
  else
    return false;
}

void ShareDesktopJob::makeScreenshot()
{
  if( !isRunning() )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << qPrintable( objectName() ) << "is not running... screen capture aborted";
#endif
    return;
  }

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

  if( device_pixel_ratio > 1.0 )
    screen_shot = screen_shot.scaled( QApplication::desktop()->width(), QApplication::desktop()->height(), Qt::KeepAspectRatio );

  QImage new_image = screen_shot.toImage();
  screen_shot = QPixmap();

  QImage diff_image = ImageOptimizer::instance().diffImage( m_lastImage, new_image );
  m_lastImage = new_image;
  QByteArray diff_image_data = ImageOptimizer::instance().saveImage( diff_image, "png", true );
  emit imageDataAvailable( diff_image_data );
}
