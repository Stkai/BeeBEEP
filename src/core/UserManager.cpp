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

#include "UserManager.h"
#include "Settings.h"

UserManager* UserManager::mp_instance = NULL;


UserManager::UserManager()
  : m_users()
{
}

void UserManager::setUser( const User& u )
{
  if( u.id() == ID_LOCAL_USER )
    Settings::instance().setLocalUser( u );
  else
    m_users.set( u );
}

void UserManager::setGroup( const Group& g )
{
  if( m_groups.contains( g ) )
    m_groups.removeOne( g );
  m_groups.append( g );
}

Group UserManager::group( VNumber group_id ) const
{
  foreach( Group g, m_groups )
  {
    if( g.id() == group_id )
      return g;
  }

#ifdef BEEBEEP_DEBUG
  qDebug() << "Unable to find group with id" << group_id;
#endif
  return Group();
}

bool UserManager::hasGroupName( const QString& group_name ) const
{
  foreach( Group g, m_groups )
  {
    if( g.name() == group_name )
      return true;
  }
  return false;
}

Group UserManager::findGroupByPrivateId( const QString& group_private_id ) const
{
  if( group_private_id.isEmpty() )
    return Group();

  foreach( Group g, m_groups )
  {
    if( g.privateId() == group_private_id )
      return g;
  }
  return Group();
}

bool UserManager::removeGroup( VNumber group_id )
{
  Group g = group( group_id );
  if( g.isValid() && m_groups.removeOne( g ) )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << g.name() << "is removed from group list";
#endif
    return true;
  }
  else
    return false;
}

bool UserManager::isUserInGroups( VNumber user_id ) const
{
  if( user_id == ID_LOCAL_USER )
    return true;

  foreach( Group g, m_groups )
  {
    if( g.hasUser( user_id ) )
      return true;
  }

  return false;
}

User UserManager::findUserByPath( const QString& user_path ) const
{
  if( user_path.isEmpty() )
    return User();

  if( user_path == Settings::instance().localUser().path() )
    return Settings::instance().localUser();

  foreach( User u, m_users.toList() )
  {
    if( u.path() == user_path )
      return u;
  }

#ifdef BEEBEEP_DEBUG
  qDebug() << "Unable to find user with path" << user_path;
#endif

  QString host_and_port = User::hostAddressAndPortFromPath( user_path );

  foreach( User u, m_users.toList() )
  {
    if( u.hostAddressAndPort() == host_and_port )
      return u;
  }

#ifdef BEEBEEP_DEBUG
  qDebug() << "Unable to find user with host and port" << host_and_port;
#endif

  if( Settings::instance().trustNickname() )
  {
    QString user_nickname = User::nameFromPath( user_path );
    foreach( User u, m_users.toList() )
    {
      if( u.name() == user_nickname )
        return u;
    }
#ifdef BEEBEEP_DEBUG
    qDebug() << "Unable to find user with name" << user_nickname;
#endif
  }

  return User();
}

User UserManager::findUserByAccountName( const QString& user_account_name ) const
{
  if( user_account_name.isEmpty() )
    return User();

  if( user_account_name.toLower() == Settings::instance().localUser().accountName().toLower() )
    return Settings::instance().localUser();

  foreach( User u, m_users.toList() )
  {
    if( u.accountName().toLower() == user_account_name.toLower() )
      return u;
  }

#ifdef BEEBEEP_DEBUG
  qDebug() << "Unable to find user with account name" << user_account_name;
#endif

  return User();
}

User UserManager::findUserBySessionId( const QString& user_session_id ) const
{
  if( user_session_id.isEmpty() )
    return User();

  if( user_session_id == Settings::instance().localUser().sessionId() )
    return Settings::instance().localUser();

  foreach( User u, m_users.toList() )
  {
    if( u.sessionId() == user_session_id )
      return u;
  }

#ifdef BEEBEEP_DEBUG
  qDebug() << "Unable to find user with session id" << user_session_id;
#endif

  return User();
}

User UserManager::findUserByHostAddressAndPort( const QHostAddress& host_address, int host_port )
{
  if( host_address.isNull() )
    return User();

  foreach( User u, m_users.toList() )
  {
    if( u.hostAddress() == host_address && u.hostPort() == host_port )
      return u;
  }

#ifdef BEEBEEP_DEBUG
  qDebug() << "Unable to find user with IP address" << qPrintable( host_address.toString() ) << "and port" << host_port;
#endif

  return User();
}
