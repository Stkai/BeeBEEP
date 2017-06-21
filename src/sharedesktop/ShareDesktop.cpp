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

#include "BeeApplication.h"
#include "Chat.h"
#include "Settings.h"
#include "ShareDesktop.h"
#include "ShareDesktopJob.h"


ShareDesktop::ShareDesktop( QObject *parent )
  : QObject( parent ), m_userIdList(), m_timer(), mp_job( 0 )
{
  setObjectName( "ShareDesktop" );
  m_timer.setObjectName( "ShareDesktopTimer" );
  m_timer.setSingleShot( false );
  connect( &m_timer, SIGNAL( timeout() ), this, SLOT( makeScreenshot() ), Qt::QueuedConnection );
}

bool ShareDesktop::start()
{
  if( isActive() )
  {
    qWarning() << "ShareDesktop is already running. Starting operation aborted";
    return false;
  }

  if( m_userIdList.isEmpty() )
  {
    qWarning() << "ShareDesktop is started with empty users. Starting operation aborted";
    return false;
  }

  if( !mp_job )
  {
    mp_job = new ShareDesktopJob;
    connect( this, SIGNAL( imageAvailable( const QImage& ) ), mp_job, SLOT( processNewImage( const QImage& ) ), Qt::QueuedConnection );
    connect( mp_job, SIGNAL( imageDataAvailable( const QByteArray&, const QString&, bool, unsigned int ) ), this, SLOT( onImageDataAvailable( const QByteArray&, const QString&, bool, unsigned int ) ), Qt::QueuedConnection );
    BeeApplication* bee_app = (BeeApplication*)qApp;
    bee_app->addJob( mp_job );
  }

  m_timer.start( Settings::instance().shareDesktopCaptureDelay() );
  return true;
}

void ShareDesktop::stop()
{
  if( !isActive() )
    return;
  m_timer.stop();
  m_userIdList.clear();
  BeeApplication* bee_app = (BeeApplication*)qApp;
  bee_app->removeJob( mp_job );
  mp_job->deleteLater();
  mp_job = 0;
#ifdef BEEBEEP_DEBUG
  qDebug() << "ShareDesktop is stopped";
#endif
}

bool ShareDesktop::addUserId( VNumber user_id )
{
  if( user_id == ID_INVALID || user_id == ID_LOCAL_USER )
    return false;

  if( m_userIdList.contains( user_id ) )
    return false;

  m_userIdList.append( user_id );
  return true;
}

void ShareDesktop::onImageDataAvailable( const QByteArray& img_data, const QString& image_type, bool use_compression, unsigned int diff_color )
{
  if( !isActive() )
    return;
#ifdef BEEBEEP_DEBUG
  qDebug() << "ShareDesktop has image data available with size" << img_data.size() << "and diff color" << diff_color;
#endif
  emit imageDataAvailable( img_data, image_type, use_compression, diff_color );
}

void ShareDesktop::makeScreenshot()
{
  if( !isActive() )
    return;

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

  emit imageAvailable( screen_shot.toImage() );
  screen_shot = QPixmap();
}
