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
#include "Protocol.h"
#include "XmppManager.h"


bool Core::connectToXmppServer( const QString& jid, const QString& passwd )
{
  QString sHtmlMsg = Bee::iconToHtml( mp_xmppManager->iconPath(), "*@*" ) + QString( " " );

  if( mp_xmppManager->isConnected() )
  {
    sHtmlMsg += tr( "You are already connected to the %1 server." ).arg( mp_xmppManager->service() );
    dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER, sHtmlMsg, DispatchToChat );
    return false;
  }

  if( jid.isEmpty() )
  {
    sHtmlMsg += tr( "Unable to connect to the %1 server. Username is empty." ).arg( mp_xmppManager->service() );
    dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER, sHtmlMsg, DispatchToChat );
    return false;
  }

  if( passwd.isEmpty() )
  {
    sHtmlMsg += tr( "Unable to connect to the %1 server. Password is empty." ).arg( mp_xmppManager->service() );
    dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER, sHtmlMsg, DispatchToChat );
    return false;
  }

  mp_xmppManager->connectToServer( jid, passwd );

  return true;
}

void Core::disconnectFromXmppServer()
{
  if( mp_xmppManager->isConnected() )
    mp_xmppManager->disconnectFromServer();
}

void Core::parseXmppMessage( const QString& user_path, const Message& m )
{
  User u = m_users.find( user_path );
  if( !u.isValid() )
  {
    qWarning() << "XMPP> invalid user" << user_path << "found while parsing message";
    return;
  }

  switch( m.type() )
  {
  case Message::System:
    dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(),
                           QString( "%1 %2" ).arg( Bee::iconToHtml( mp_xmppManager->iconPath(), "*@*" ),
                                                   m.text() ),
                           DispatchToAllChatsWithUser );
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
  User u = m_users.find( user_to_check.path() );
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
  m_users.setUser( u );
}

void Core::sendXmppChatMessage( const QString& user_path, const Message& msg )
{
  mp_xmppManager->sendMessage( user_path, msg );
}

void Core::setXmppUserSubscription( const QString& user_path, bool accepted )
{
  mp_xmppManager->subscribeUser( user_path, accepted );
}

bool Core::removeXmppUser( const QString& user_path )
{
  if( m_users.removeUser( user_path ) )
  {
    mp_xmppManager->removeUser( user_path );
    return true;
  }
  else
    return false;
}

void Core::sendLocalUserStatusToXmppServer()
{
  mp_xmppManager->sendLocalUserPresence();
}

void Core::checkXmppUserVCard( const QString& user_path )
{
  User u = m_users.find( user_path );
  if( u.isValid() )
  {
    if( u.vCard().hasOnlyNickName() )
      mp_xmppManager->requestVCard( u.service(), user_path );
    else
      qDebug() << "User" << user_path << "has already a vCard. Update not needed";
  }
}

void Core::setXmppVCard( const QString& user_path, const VCard& vc )
{
  User u = m_users.find( user_path );
  if( u.isValid() )
  {
    u.setVCard( vc );
    m_users.setUser( u );
    showUserVCardChanged( u );
  }
}
