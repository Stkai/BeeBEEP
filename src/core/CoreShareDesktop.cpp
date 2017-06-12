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


bool Core::startShareDesktop( VNumber chat_id )
{
  Chat c = ChatManager::instance().chat( chat_id );
  if( mp_shareDesktop->start( c ) )
  {
    QString sHtmlMsg = tr( "%1 You start to share desktop with %2." ).arg( IconManager::instance().toHtml( "desktop-share.png", "*G*" ), c.name() );
    dispatchSystemMessage( c.id(), ID_LOCAL_USER, sHtmlMsg, DispatchToChat, ChatMessage::System );
    qDebug() << "Start to share desktop with" << qPrintable( c.name() );
    return true;
  }
  else
    return false;
}

void Core::stopShareDesktop()
{
  if( !mp_shareDesktop->isActive() )
    return;

  if( mp_shareDesktop->hasChat() )
  {
    Chat c = ChatManager::instance().chat( mp_shareDesktop->chatId() );
    QString sHtmlMsg = tr( "%1 You stop to share desktop with %2." ).arg( IconManager::instance().toHtml( "desktop-share-refused.png", "*G*" ), c.name() );
    dispatchSystemMessage( c.id(), ID_LOCAL_USER, sHtmlMsg, DispatchToChat, ChatMessage::System );
    qDebug() << "Stop to share desktop with" << qPrintable( c.name() );
  }

  mp_shareDesktop->stop();
}

void Core::refuseToViewShareDesktop( VNumber chat_id, VNumber from_user_id, VNumber to_user_id )
{
  mp_shareDesktop->removeUserId( from_user_id );

  Chat c = ChatManager::instance().chat( chat_id );
  User to_user = UserManager::instance().findUser( to_user_id );
  User from_user = UserManager::instance().findUser( from_user_id );
  if( !c.isValid() || !to_user.isValid() || !from_user.isValid() )
  {
    qWarning() << "Invalid to user" << to_user_id << "or from user" << from_user_id << "or chat" << chat_id << "found in Core::refuseToViewShareDesktop(...)";
    return;
  }

  QString sHtmlMsg;
  if( from_user.isLocal() )
  {
    Message m = Protocol::instance().refuseToViewDesktopShared( c );
    sendMessageToLocalNetwork( to_user, m );
    sHtmlMsg = tr( "%1 You have refused to view shared desktop from %2." ).arg( IconManager::instance().toHtml( "desktop-share-refused.png", "*G*" ), to_user.name() );
    qDebug() << "You have refused to view desktop shared by" << qPrintable( to_user.name() );
  }
  else
  {
    sHtmlMsg = tr( "%1 %2 has closed the view of your shared desktop." ).arg( IconManager::instance().toHtml( "desktop-share-refused.png", "*G*" ), from_user.name() );
    qDebug() << qPrintable( from_user.name() ) << "has closed the view your shared desktop";
  }

  dispatchSystemMessage( chat_id, from_user.id(), sHtmlMsg, DispatchToChat, ChatMessage::System );
}

bool Core::shareDesktopIsActive( VNumber chat_id ) const
{
  if( chat_id == ID_INVALID )
    return mp_shareDesktop->isActive();
  else
    return mp_shareDesktop->isActive() && mp_shareDesktop->chatId() == chat_id;
}

void Core::onShareDesktopDataReady( const QByteArray& pix_data )
{
  if( !mp_shareDesktop->isActive() )
    return;

  if( mp_shareDesktop->userIdList().isEmpty() || !mp_shareDesktop->hasChat() )
  {
    stopShareDesktop();
    return;
  }

  Chat c_tmp = ChatManager::instance().chat( mp_shareDesktop->chatId() );
  if( !c_tmp.isValid() )
  {
    stopShareDesktop();
    return;
  }

  Message m = Protocol::instance().shareDesktopDataToMessage( c_tmp, pix_data );
  foreach( VNumber user_id, mp_shareDesktop->userIdList() )
  {
    Connection* c = connection( user_id );
    if( c && c->isConnected() )
      c->sendMessage( m );
  }
}
