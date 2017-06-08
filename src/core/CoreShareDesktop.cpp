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


void Core::addChatToDesktopShare( VNumber chat_id )
{
  if( !mp_shareDesktop->addChat( chat_id ) )
    return;
  Chat c = ChatManager::instance().chat( chat_id );
  QString sHtmlMsg = tr( "%1 You start to share desktop with %2." ).arg( IconManager::instance().toHtml( "desktop-share.png", "*G*" ), c.name() );
  dispatchSystemMessage( chat_id, ID_LOCAL_USER, sHtmlMsg, DispatchToChat, ChatMessage::System );
  qDebug() << "Start to share desktop with" << qPrintable( c.name() );
}

void Core::removeChatFromDesktopShare( VNumber chat_id )
{
  if( !mp_shareDesktop->removeChat( chat_id ) )
    return;

  Chat c = ChatManager::instance().chat( chat_id );
  QString sHtmlMsg = tr( "%1 You stop to share desktop with %2." ).arg( IconManager::instance().toHtml( "desktop-share.png", "*G*" ), c.name() );
  dispatchSystemMessage( chat_id, ID_LOCAL_USER, sHtmlMsg, DispatchToChat, ChatMessage::System );
  qDebug() << "Stop to share desktop with" << qPrintable( c.name() );

  if( mp_shareDesktop->chatIdList().isEmpty() )
    QMetaObject::invokeMethod( this, "stopShareDesktop", Qt::QueuedConnection );
}

bool Core::chatIsInDesktopShare( VNumber chat_id ) const
{
  return mp_shareDesktop->hasChat( chat_id );
}

void Core::startShareDesktop()
{
  mp_shareDesktop->start();
}

void Core::stopShareDesktop()
{
  mp_shareDesktop->stop();
}

void Core::refuseDesktopShare( const User& u, const Chat& c )
{
  /*
  Connection* c = connection( user_id );
  if( c && c->isConnected() )
  {
    Message m = Protocol::instance().refuseToViewDesktopShared( cmd );
    c->sendMessage( m );
  }*/
}

void Core::onShareDesktopDataReady( const QByteArray& pix_data )
{
  const QList<VNumber>& chat_id_list = mp_shareDesktop->chatIdList();
  if( chat_id_list.isEmpty() )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Core received image data from desktop share but no chat is present in list";
#endif
    return;
  }

  QList<VNumber> contacted_user_id_list;
  foreach( VNumber chat_id, chat_id_list )
  {
    if( chat_id == ID_DEFAULT_CHAT )
      continue;

    Chat c_tmp = ChatManager::instance().chat( chat_id );
    if( !c_tmp.isValid() )
    {
      removeChatFromDesktopShare( chat_id );
      continue;
    }

    Message m = Protocol::instance().shareDesktopDataToMessage( c_tmp, pix_data );

    foreach( VNumber user_id, c_tmp.usersId() )
    {
      if( user_id == ID_LOCAL_USER )
        continue;

      if( !contacted_user_id_list.contains( user_id ) )
      {
        contacted_user_id_list.append( user_id );
        Connection* c = connection( user_id );
        if( c && c->isConnected() )
          c->sendMessage( m );
      }
    }
  }

  if( contacted_user_id_list.isEmpty() )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Core received image data from desktop share but no user is present in list";
#endif
    mp_shareDesktop->clearChats();
    return;
  }
}
