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
#include "ShareDesktop.h"
#include "ShareDesktopJob.h"


ShareDesktop::ShareDesktop( QObject *parent )
  : QObject( parent ), m_userIdList(), mp_job( 0 )
{
  setObjectName( "ShareDesktop" );
}

bool ShareDesktop::isActive() const
{
  if( mp_job )
    return true;
  else
    return false;
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

  mp_job = new ShareDesktopJob;
  connect( mp_job, SIGNAL( jobCompleted() ), this, SLOT( onJobCompleted() ), Qt::QueuedConnection );
  connect( mp_job, SIGNAL( imageDataAvailable( const QByteArray& ) ), this, SLOT( onImageDataAvailable( const QByteArray& ) ), Qt::QueuedConnection );
  BeeApplication* bee_app = (BeeApplication*)qApp;
  bee_app->addJob( mp_job );
  QMetaObject::invokeMethod( mp_job, "startJob", Qt::QueuedConnection );
  return true;
}

void ShareDesktop::stop()
{
  if( !isActive() )
    return;
  QMetaObject::invokeMethod( mp_job, "stopJob", Qt::QueuedConnection );
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

void ShareDesktop::onJobCompleted()
{
#ifdef BEEBEEP_DEBUG
  qDebug() << qPrintable( objectName() ) << "has completed its job";
#endif
  m_userIdList.clear();
  BeeApplication* bee_app = (BeeApplication*)qApp;
  bee_app->removeJob( mp_job );
  mp_job->deleteLater();
  mp_job = 0;
}

void ShareDesktop::onImageDataAvailable( const QByteArray& img_data )
{
#ifdef BEEBEEP_DEBUG
  qDebug() << qPrintable( objectName() ) << "has image data available" << img_data.size();
#endif
  emit shareDesktopImageDataReady( img_data );
}
