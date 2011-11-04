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
// Author: Marco Mastroddi (marco.mastroddi(AT)gmail.com)
//
// $Id$
//
//////////////////////////////////////////////////////////////////////

#include "BeeUtils.h"
#include "ColorManager.h"
#include "Core.h"
#include "NetworkAccount.h"
#include "Protocol.h"
#include "UserManager.h"
#include "XmppManager.h"
#include "PluginManager.h"


bool Core::isXmppServerConnected( const QString& xmpp_service ) const
{
  XmppClient* xmpp_client = mp_xmppManager->client( xmpp_service );
  return xmpp_client && xmpp_client->isActive();
}

bool Core::connectToXmppServer( const NetworkAccount& na )
{
  ServiceInterface* s = PluginManager::instance().service( na.service() );
  if( s && s->isEnabled() )
    return mp_xmppManager->connectToServer( na.service(), na.user(), na.password() );
  else
    return false;
}

void Core::disconnectFromXmppServer( const QString& service )
{
  if( service.isEmpty() )
    mp_xmppManager->disconnectFromServer();
  else
    mp_xmppManager->disconnectFromServer( service );
}

void Core::parseXmppMessage( const QString& service, const QString& bare_jid, const Message& m )
{
  User u = UserManager::instance().userList().find( service, bare_jid );
  if( !u.isValid() )
  {
    qWarning() << "XMPP> invalid user" << bare_jid << "found while parsing message";
    return;
  }

  switch( m.type() )
  {
  case Message::System:
    dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), m.text(), DispatchToAllChatsWithUser );
    break;
  case Message::User:
  case Message::Chat:
    parseMessage( u, m );
    break;
  default:
    qWarning() << "XMPP> core cannot parse the message with type" << m.type();
    break;
  }
}

void Core::checkXmppUser( const User& user_to_check )
{
  User u = UserManager::instance().userList().find( user_to_check.path() );
  if( u.isValid() )
  {
    qDebug() << "XMPP>" << u.path() << "already exists";
    return;
  }

  u = user_to_check;
  u.setId( Protocol::instance().newId() );
  u.setColor( ColorManager::instance().unselectedQString() );
  qDebug() << "XMPP> new user connected:" << u.path() << "with color" << u.color();
  createPrivateChat( u );
  UserManager::instance().setUser( u );
  emit userChanged( u );
}

void Core::sendXmppChatMessage( const User& u, const Message& msg )
{
  mp_xmppManager->sendMessage( u, msg );
}

void Core::setXmppUserSubscription( const QString& service, const QString& bare_jid, bool accepted )
{
  mp_xmppManager->subscribeUser( service, bare_jid, accepted );
}

bool Core::removeXmppUser( const User& u )
{
  if( UserManager::instance().removeUser( u ) )
  {
    mp_xmppManager->removeUser( u );
    return true;
  }
  else
    return false;
}

void Core::sendLocalUserStatusToXmppServer()
{
  mp_xmppManager->sendLocalUserPresence();
}

void Core::checkXmppUserVCard( const QString& service, const QString& bare_jid )
{
  User u = UserManager::instance().userList().find( service, bare_jid );
  if( u.isValid() )
  {
    if( u.vCard().hasOnlyNickName() )
      mp_xmppManager->requestVCard( service, bare_jid );
    else
      qDebug() << "User" << u.path() << "has already a vCard. Update not needed";
  }
}

void Core::setXmppVCard( const QString& service, const QString& bare_jid, const VCard& vc )
{
  User u = UserManager::instance().userList().find( service, bare_jid );
  if( u.isValid() )
  {
    u.setVCard( vc );
    UserManager::instance().setUser( u );
    showUserVCardChanged( u );
  }
}
