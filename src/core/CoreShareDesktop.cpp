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

#include "ChatManager.h"
#include "Connection.h"
#include "Core.h"
#include "IconManager.h"
#include "Protocol.h"
#include "ShareDesktop.h"
#include "Settings.h"
#include "UserManager.h"


bool Core::startShareDesktop( VNumber user_id )
{
  User u = UserManager::instance().findUser( user_id );
  if( !u.isValid() )
  {
    qWarning() << "Invalid user" << user_id << "found in Core::startShareDesktop(...)";
    return false;
  }

  if( !mp_shareDesktop->addUserId( user_id ) )
    return false;

  QString sHtmlMsg = tr( "%1 You start to share desktop with %2." ).arg( IconManager::instance().toHtml( "desktop-share.png", "*G*" ), u.name() );
  dispatchSystemMessage( ID_DEFAULT_CHAT, user_id, sHtmlMsg, DispatchToAllChatsWithUser, ChatMessage::System );
  qDebug() << "Start to share desktop with" << qPrintable( u.path() );
  if( !mp_shareDesktop->isActive() )
    mp_shareDesktop->start();
  emit shareDesktopUpdate( u );
  return true;
}

void Core::stopShareDesktop( VNumber user_id )
{
  if( !mp_shareDesktop->isActive() )
    return;

  if( !mp_shareDesktop->userIdList().contains( user_id ) )
    return;

  mp_shareDesktop->removeUserId( user_id );
  if( !mp_shareDesktop->hasUsers() )
    mp_shareDesktop->stop();

  User u = UserManager::instance().findUser( user_id );
  if( u.isValid() )
  {
    QString sHtmlMsg = tr( "%1 You stop to share desktop with %2." ).arg( IconManager::instance().toHtml( "desktop-share-refused.png", "*G*" ), u.name() );
    dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER, sHtmlMsg, DispatchToAllChatsWithUser, ChatMessage::System );
    emit shareDesktopUpdate( u );
  }
}

void Core::stopShareDesktop()
{
  foreach( VNumber user_id, mp_shareDesktop->userIdList() )
    stopShareDesktop( user_id );
}

void Core::refuseToViewShareDesktop( VNumber from_user_id, VNumber to_user_id )
{
  if( from_user_id != ID_LOCAL_USER )
  {
    if( !mp_shareDesktop->isActive() )
      return;
  }

  User from_user = UserManager::instance().findUser( from_user_id );
  if( !from_user.isValid() )
  {
    qWarning() << "Invalid from user" << from_user_id << "found in Core::refuseToViewShareDesktop(...)";
    return;
  }

  User to_user = UserManager::instance().findUser( to_user_id );
  if( !to_user.isValid() )
  {
    qWarning() << "Invalid from user" << to_user_id << "found in Core::refuseToViewShareDesktop(...)";
    return;
  }

  if( from_user.isLocal() )
  {
    qDebug() << "You have closed the view of the desktop shared by" << qPrintable( to_user.path() );
    Message m = Protocol::instance().refuseToViewDesktopShared();
    sendMessageToLocalNetwork( to_user, m );
  }
  else
  {
    qDebug() << qPrintable( from_user.path() ) << "has closed the view of your shared desktop";
    QString sHtmlMsg = tr( "%1 %2 has closed the view of your shared desktop." ).arg( IconManager::instance().toHtml( "desktop-share-refused.png", "*G*" ), from_user.name() );
    dispatchSystemMessage( ID_DEFAULT_CHAT, from_user.id(), sHtmlMsg, DispatchToAllChatsWithUser, ChatMessage::System );
    stopShareDesktop( from_user_id );
  }
}

bool Core::shareDesktopIsActive( VNumber user_id ) const
{
  if( user_id == ID_INVALID )
    return mp_shareDesktop->isActive() && mp_shareDesktop->userIdList().size() > 0;
  else
    return mp_shareDesktop->isActive() && mp_shareDesktop->userIdList().contains( user_id );
}

void Core::onShareDesktopDataReady( const QByteArray& pix_data )
{
  if( !mp_shareDesktop->isActive() )
    return;

  if( mp_shareDesktop->userIdList().isEmpty() )
  {
    stopShareDesktop();
    return;
  }

  Message m = Protocol::instance().shareDesktopDataToMessage( pix_data );
  foreach( VNumber user_id, mp_shareDesktop->userIdList() )
  {
    Connection* c = connection( user_id );
    if( c && c->isConnected() )
      c->sendMessage( m );
  }
}
