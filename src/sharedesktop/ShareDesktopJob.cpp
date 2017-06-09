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

#include "Settings.h"
#include "ShareDesktopJob.h"


ShareDesktopJob::ShareDesktopJob( QObject *parent )
  : QObject( parent ), m_timer(), m_lastImageHash( "" )
{
  setObjectName( "ShareDesktopJob" );
  m_timer.setInterval( 2000 );
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

  m_lastImageHash = "";
  QMetaObject::invokeMethod( &m_timer, "start", Qt::QueuedConnection );
}

void ShareDesktopJob::stopJob()
{
#ifdef BEEBEEP_DEBUG
  qDebug() << qPrintable( objectName() ) << "is stopping its job";
#endif
  QMetaObject::invokeMethod( &m_timer, "stop", Qt::QueuedConnection );
  emit jobCompleted();
}

bool ShareDesktopJob::isRunning() const
{
  return m_timer.isActive();
}

void ShareDesktopJob::makeScreenshot()
{
  if( !isRunning() )
  {
    qDebug() << qPrintable( objectName() ) << "is not running... screen capture aborted";
    return;
  }

#ifdef BEEBEEP_DEBUG
  qDebug() << qPrintable( objectName() ) << "is making a screen capture";
#endif

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

  QByteArray pix_bytes;
  QBuffer buffer( &pix_bytes );
  buffer.open( QIODevice::WriteOnly );
  screen_shot.save( &buffer, Settings::instance().shareDesktopImageType() );

  screen_shot = QPixmap();
  QByteArray pix_hash = QCryptographicHash::hash( pix_bytes, QCryptographicHash::Sha1 );

  if( pix_hash != m_lastImageHash )
  {
    m_lastImageHash = pix_hash;
    emit imageAvailable( pix_bytes );
  }
}
