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
#include "Connection.h"
#include "Core.h"
#include "PeerManager.h"
#include "Protocol.h"
#include "Settings.h"


void Core::setLocalUserStatus( int new_status )
{
  if( Settings::instance().localUser().status() == new_status )
    return;
  User u = Settings::instance().localUser();
  u.setStatus( new_status );
  Settings::instance().setLocalUser( u );
  sendUserStatus();
  setUserStatus( u );
}

void Core::setLocalUserStatusDescription( const QString& new_status_description )
{
  if( Settings::instance().localUser().statusDescription() == new_status_description )
    return;
  User u = Settings::instance().localUser();
  u.setStatusDescription( new_status_description );
  Settings::instance().setLocalUser( u );
  sendUserStatus();
  setUserStatus( u );
}

void Core::setLocalUserName( const QString& user_name )
{
  if( Settings::instance().localUser().name() == user_name )
    return;

  User u = Settings::instance().localUser();
  QString old_user_name = u.name();
  u.setName( user_name.trimmed() );
  Settings::instance().setLocalUser( u );
  sendUserName();
  setUserName( u, old_user_name );
}

void Core::setUserStatus( const User& u )
{
  QString sHtmlMsg = Bee::iconToHtml( Bee::userStatusIconFileName( u.status() ), "*S*" ) + QString( " " );
  if( u.isLocal() )
    sHtmlMsg += tr( "You are" );
  else
    sHtmlMsg += (Settings::instance().showOnlyUsername() ? u.name() : u.path()) + QString( " " ) + tr( "is" );
   sHtmlMsg += QString( " " );
   sHtmlMsg += QString( "%1%2." ).arg( Bee::userStatusToString( u.status() ) )
                            .arg( u.statusDescription().isEmpty() ? "" : QString( ": %1").arg( u.statusDescription() ) );

  dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), sHtmlMsg, DispatchToAllChatsWithUser );
  emit userChanged( u );
}

void Core::setUserName( const User& u, const QString& old_user_name )
{
  QString sHtmlMsg = Bee::iconToHtml( ":/images/profile.png", "*N*" ) + QString( " " );
  if( u.isLocal() )
    sHtmlMsg += tr( "You have changed your nickname from %1 to %2." ).arg( old_user_name, u.name() );
  else
    sHtmlMsg += tr( "%1 has changed the nickname in %2." ).arg( old_user_name, u.name() );

  dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), sHtmlMsg, DispatchToAllChatsWithUser );
  emit userChanged( u );
}


void Core::searchUsers( const QHostAddress& host_address )
{
  mp_peerManager->sendDatagramToHost( host_address );
  QString sHtmlMsg = tr( "%1 Looking for the available users in the network address %2..." )
      .arg( Bee::iconToHtml( ":/images/search.png", "*b*" ), host_address.toString() );
  dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER, sHtmlMsg, DispatchToChat );
}

void Core::sendUserStatus()
{
  QByteArray user_status_message = Protocol::instance().localUserStatusMessage();
  foreach( Connection *c, m_connections )
    c->sendData( user_status_message );
}

void Core::sendUserName()
{
  QByteArray user_name_message = Protocol::instance().localUserNameMessage();
  foreach( Connection *c, m_connections )
    c->sendData( user_name_message );
}
