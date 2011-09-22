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


bool Core::connectToNetworkAccount( const QString& jid, const QString& passwd )
{
  QString sHtmlMsg = Bee::iconToHtml( ":/images/network-account.png", "*@*" ) + QString( " " );

  if( mp_xmppManager->isConnected() )
  {
    sHtmlMsg += tr( "You are already connected to the network account." );
    dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER, sHtmlMsg, DispatchToChat );
    return false;
  }

  if( jid.isEmpty() )
  {
    sHtmlMsg += tr( "Unable to connect to the network account. Username is empty." );
    dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER, sHtmlMsg, DispatchToChat );
    return false;
  }

  if( passwd.isEmpty() )
  {
    sHtmlMsg += tr( "Unable to connect to the network account. Password is empty." );
    dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER, sHtmlMsg, DispatchToChat );
    return false;
  }

  sHtmlMsg += tr( "Connecting to %1..." ).arg( jid );
  dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER, sHtmlMsg, DispatchToChat );
  mp_xmppManager->connectToServer( jid, passwd );
  return true;
}

void Core::disconnectFromNetworkAccount()
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
                           QString( "%1 %2" ).arg( Bee::iconToHtml( ":/images/network-account.png", "*@*" ),
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

