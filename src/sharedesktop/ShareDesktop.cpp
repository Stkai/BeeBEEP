//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2021 Marco Mastroddi
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
// along with BeeBEEP. If not, see <http://www.gnu.org/licenses/>.
//
// Author: Marco Mastroddi <marco.mastroddi(AT)gmail.com>
//
// $Id$
//
//////////////////////////////////////////////////////////////////////

#include "BeeApplication.h"
#include "Chat.h"
#include "Screenshot.h"
#include "Settings.h"
#include "ShareDesktop.h"
#include "ShareDesktopJob.h"


ShareDesktop::ShareDesktop( QObject *parent )
  : QObject( parent ), m_userIdList(), m_timer(), mp_job( 0 ),
    m_lastImageData(), m_userIdReadList()
{
  setObjectName( "ShareDesktop" );
  m_timer.setObjectName( "ShareDesktopTimer" );
  m_timer.setSingleShot( false );
  connect( &m_timer, SIGNAL( timeout() ), this, SLOT( onScreenshotTimeout() ), Qt::QueuedConnection );
}

bool ShareDesktop::removeUserId( VNumber user_id )
{
  resetUserReadImage( user_id );
  return m_userIdList.removeOne( user_id );
}

void ShareDesktop::requestImageFromUser( VNumber user_id )
{
  if( !isActive() )
    return;

  if( user_id == ID_INVALID || user_id == ID_LOCAL_USER )
    return;

  if( !m_userIdList.contains( user_id ) )
    return;

  if( !m_userIdReadList.contains( user_id ) )
    m_userIdReadList.append( user_id );
}

void ShareDesktop::resetUserReadImage( VNumber user_id )
{
  m_userIdReadList.removeOne( user_id );
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

  m_lastImageData = ShareDesktopData();

  if( !mp_job )
  {
    mp_job = new ShareDesktopJob;
    connect( this, SIGNAL( imageAvailable( const QImage& ) ), mp_job, SLOT( processNewImage( const QImage& ) ), Qt::QueuedConnection );
    connect( mp_job, SIGNAL( imageDataAvailable( const QByteArray&, const QString&, bool, unsigned int ) ), this, SLOT( onImageDataAvailable( const QByteArray&, const QString&, bool, unsigned int ) ), Qt::QueuedConnection );
    if( beeApp )
      beeApp->addJob( mp_job );
  }

  m_timer.start( Settings::instance().shareDesktopCaptureDelay() );
  return true;
}

void ShareDesktop::stop()
{
  if( !isActive() )
    return;
  m_timer.stop();
  m_lastImageData = ShareDesktopData();
  m_userIdList.clear();
  if( beeApp )
    beeApp->removeJob( mp_job );
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
  m_userIdReadList.append( user_id );
  return true;
}

void ShareDesktop::onImageDataAvailable( const QByteArray& img_data, const QString& image_type, bool use_compression, unsigned int diff_color )
{
  if( !isActive() )
    return;
#ifdef BEEBEEP_DEBUG
  qDebug() << "ShareDesktop has image data available with size" << img_data.size() << "and diff color" << diff_color;
#endif
  m_lastImageData.setImageData( img_data );
  m_lastImageData.setImageType( image_type );
  m_lastImageData.setIsCompressed( use_compression );
  m_lastImageData.setDiffColor( diff_color );
  emit imageDataAvailable( m_lastImageData );
}

void ShareDesktop::onScreenshotTimeout()
{
  if( !isActive() )
    return;

  Screenshot screen_shot;
  screen_shot.grabPrimaryScreen();
  if( screen_shot.isValid() )
  {
    emit imageAvailable( screen_shot.toImage() );
    screen_shot.reset();
  }
}
