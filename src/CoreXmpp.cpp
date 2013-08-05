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


#ifdef USE_QXMPP
bool Core::isXmppServerConnected( const QString& xmpp_service ) const
#else
bool Core::isXmppServerConnected( const QString& ) const
#endif
{
#ifdef USE_QXMPP
  XmppClient* xmpp_client = mp_xmppManager->client( xmpp_service );
  return xmpp_client && xmpp_client->isActive();
#else
  return false;
#endif
}

#ifdef USE_QXMPP
bool Core::connectToXmppServer( const NetworkAccount& na )
#else
bool Core::connectToXmppServer( const NetworkAccount& )
#endif
{
#ifdef USE_QXMPP
  ServiceInterface* s = PluginManager::instance().service( na.service() );
  if( s && s->isEnabled() )
    return mp_xmppManager->connectToServer( na.service(), na.user(), na.password() );
  else
#endif
    return false;
}

#ifdef USE_QXMPP
void Core::disconnectFromXmppServer( const QString& service )
#else
void Core::disconnectFromXmppServer( const QString& )
#endif
{
#ifdef USE_QXMPP
  if( service.isEmpty() )
    mp_xmppManager->disconnectFromServer();
  else
    mp_xmppManager->disconnectFromServer( service );
#endif
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
    dispatchSystemMessage( service, ID_DEFAULT_CHAT, u.id(), m.text(), DispatchToService );
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

#ifdef USE_QXMPP
void Core::sendXmppChatMessage( const User& u, const Message& msg )
#else
void Core::sendXmppChatMessage( const User&, const Message& )
#endif
{
#ifdef USE_QXMPP
  mp_xmppManager->sendMessage( u, msg );
#endif
}

#ifdef USE_QXMPP
void Core::setXmppUserSubscription( const QString& service, const QString& bare_jid, bool accepted )
#else
void Core::setXmppUserSubscription( const QString&, const QString&, bool )
#endif
{
#ifdef USE_QXMPP
  mp_xmppManager->subscribeUser( service, bare_jid, accepted );
#endif
}

#ifdef USE_QXMPP
bool Core::removeXmppUser( const User& u )
#else
bool Core::removeXmppUser( const User& )
#endif
{
#ifdef USE_QXMPP
  if( UserManager::instance().removeUser( u ) )
  {
    mp_xmppManager->removeUser( u );
    return true;
  }
  else
#endif
    return false;
}

void Core::sendLocalUserStatusToXmppServer()
{
#ifdef USE_QXMPP
  mp_xmppManager->sendLocalUserPresence();
#endif
}

void Core::setXmppVCard( const QString& service, const QString& bare_jid, const VCard& vc )
{
  User u = UserManager::instance().userList().find( service, bare_jid );
  if( u.isValid() )
  {
    if( u.vCard() == vc )
      return;
    u.setVCard( vc );
    UserManager::instance().setUser( u );
    showUserVCardChanged( u );
  }
}

void Core::sendXmppUserComposing( const User& u )
{
  if( !u.isConnected() )
    return;
#ifdef USE_QXMPP
  mp_xmppManager->sendComposingMessage( u );
#endif
}

void Core::sendLocalVCardToXmppServer()
{
#ifdef USE_QXMPP
  if( mp_xmppManager->isConnected() )
    mp_xmppManager->sendLocalUserVCard();
#endif
}
