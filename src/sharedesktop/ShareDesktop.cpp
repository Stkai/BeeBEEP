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
  : QObject( parent ), m_chatId( ID_INVALID ), m_userIdList(), mp_job( 0 )
{
  setObjectName( "ShareDesktop" );
}

bool ShareDesktop::start( const Chat& c )
{
  if( mp_job )
  {
    qWarning() << "ShareDesktop is already running. Restart is aborted";
    return false;
  }

  if( !setChat( c ) )
    return false;

  mp_job = new ShareDesktopJob;
  connect( mp_job, SIGNAL( jobCompleted() ), this, SLOT( onJobCompleted() ), Qt::QueuedConnection );
  connect( mp_job, SIGNAL( imageAvailable( const QByteArray& ) ), this, SLOT( onImageDataAvailable( const QByteArray& ) ), Qt::QueuedConnection );

  BeeApplication* bee_app = (BeeApplication*)qApp;
  bee_app->addJob( mp_job );
  QMetaObject::invokeMethod( mp_job, "startJob", Qt::QueuedConnection );
  return true;
}

void ShareDesktop::stop()
{
  if( m_chatId != ID_INVALID )
  {
    m_userIdList.clear();
    m_chatId = ID_INVALID;
  }

  if( mp_job )
    QMetaObject::invokeMethod( mp_job, "stopJob", Qt::QueuedConnection );
}

bool ShareDesktop::setChat( const Chat& c )
{
  if( m_chatId != ID_INVALID )
    return false;

  if( !c.isValid() )
    return false;

  m_chatId = c.id();
  if( !m_userIdList.isEmpty() )
    m_userIdList.clear();

  foreach( VNumber user_id, c.usersId() )
    addUserId( user_id );

  return !m_userIdList.isEmpty();
}

bool ShareDesktop::addUserId( VNumber user_id )
{
  if( user_id == ID_INVALID || user_id == ID_LOCAL_USER || m_userIdList.contains( user_id ) )
    return false;

  m_userIdList.append( user_id );
  return true;
}

void ShareDesktop::onJobCompleted()
{
  ShareDesktopJob *sdj = qobject_cast<ShareDesktopJob*>( sender() );
  if( !sdj )
  {
    qWarning() << "ShareDesktop received a signal from invalid ShareDesktopJob instance";
    return;
  }

  BeeApplication* bee_app = (BeeApplication*)qApp;
  bee_app->removeJob( sdj );

  sdj->disconnect();
  sdj->deleteLater();
  mp_job = 0;
#ifdef BEEBEEP_DEBUG
  qDebug() << qPrintable( objectName() ) << "has completed its job";
#endif
}

void ShareDesktop::onImageDataAvailable( const QByteArray& pix_data )
{
#ifdef BEEBEEP_DEBUG
  qDebug() << qPrintable( objectName() ) << "has image data available";
#endif

  emit shareDesktopDataReady( pix_data );
}
