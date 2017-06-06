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

#include "Connection.h"
#include "Core.h"
#include "Protocol.h"
#include "ShareDesktop.h"
#include "Settings.h"


void Core::addUserToDesktopShare( VNumber user_id )
{
  mp_shareDesktop->addUser( user_id );
}

void Core::removeUserToDesktopShare( VNumber user_id )
{
  mp_shareDesktop->removeUser( user_id );
  if( mp_shareDesktop->users().isEmpty() )
    QMetaObject::invokeMethod( this, "stopShareDesktop", Qt::QueuedConnection );
}

void Core::startShareDesktop()
{
  mp_shareDesktop->start();
}

void Core::stopShareDesktop()
{
  mp_shareDesktop->stop();
}

void Core::refuseDesktopShareFromUser( VNumber user_id )
{
  Connection* c = connection( user_id );
  if( c && c->isConnected() )
  {
    Message m = Protocol::instance().refuseToViewDesktopShared();
    c->sendMessage( m );
  }
}

void Core::onShareDesktopDataReady( const QByteArray& pix_data )
{
  const QList<VNumber>& user_id_list = mp_shareDesktop->users();
  if( user_id_list.isEmpty() )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Core received image data from desktop share but no user is present in list";
#endif
    return;
  }

  Message m = Protocol::instance().shareDesktopDataToMessage( pix_data );
  Connection* c;
  foreach( VNumber user_id, user_id_list )
  {
    if( user_id == ID_LOCAL_USER )
    {
#ifdef BEEBEEP_DEBUG
      qDebug() << "Core sends own image desktop share data to local user";
#endif
      QPixmap pix = Protocol::instance().pixmapFromShareDesktopMessage( m );
      emit shareDesktopImageAvailable( Settings::instance().localUser(), pix );
    }
    else
    {
      c = connection( user_id );
      if( c && c->isConnected() )
        c->sendMessage( m );
    }
  }
}
