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

#include "Core.h"


User Core::user( const QString& user_name, const QString& user_nick, const QHostAddress& user_address, int user_port ) const
{
  QList<User>::const_iterator it = m_users.begin();
  while( it != m_users.end() )
  {
    if( user_port == (*it).hostPort() && (*it).hostAddress() == user_address &&
        user_name == (*it).name() && user_nick == (*it).nickname() )
      return *it;
    ++it;
  }
  return User();
}

User Core::user( VNumber user_id ) const
{
  QList<User>::const_iterator it = m_users.begin();
  while( it != m_users.end() )
  {
    if( user_id == (*it).id() )
      return *it;
    ++it;
  }
  return User();
}

void Core::setUser( const User& u )
{
  QList<User>::iterator it = m_users.begin();
  while( it != m_users.end() )
  {
    if( (*it).id() == u.id() )
    {
      (*it) = u;
      return;
    }
    ++it
  }
  m_users.append( u );
}

void BeeBeep::setLocalUserStatus( int new_status )
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

void Core::setUserStatus( const User& u )
{
  QString sHtmlMsg = Bee::iconToHtml( Bee::userStatusIconFileName( u.status() ), "*S*" ) + QString( " " );
  if( Settings::instance().localUser() == u )
    sHtmlMsg += tr( "You are" );
  else
    sHtmlMsg += (Settings::instance().showUserNickname() ? u.nickname() : u.name()) + QString( " " ) + tr( "is" );

   sHtmlMsg += QString( " %2%3." ).arg( userStatusToString( u.status() ) )
                            .arg( u.statusDescription().isEmpty() ? "" : QString( ": %1").arg( u.statusDescription() ) );

  dispatchSystemMessage( privateChatForUser( u.id() ).id(), u.id(), sHtmlMsg );
  emit( userNewStatus( u ) );
}

static const char* UserStatusToString[] =
{
  QT_TRANSLATE_NOOP( "User", "offline" ),
  QT_TRANSLATE_NOOP( "User", "online" ),
  QT_TRANSLATE_NOOP( "User", "busy" ),
  QT_TRANSLATE_NOOP( "User", "away" )
};

QString BeeBeep::userStatusToString( int user_status )
{
  if( user_status < 0 || user_status >= User::NumStatus )
    return "";
  else
    return tr( UserStatusToString[ user_status ] );
}
