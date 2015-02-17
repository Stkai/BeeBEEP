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

#include "Settings.h"
#include "UserList.h"


UserList::UserList()
  : m_users()
{
}

UserList::UserList( const UserList& ul )
{
  (void)operator=( ul );
}

UserList& UserList::operator=( const UserList& ul )
{
  if( this != &ul )
    m_users = ul.m_users;
  return *this;
}

bool UserList::has( VNumber user_id ) const
{
  if( user_id == ID_LOCAL_USER )
    return true;
  QList<User>::const_iterator it = m_users.begin();
  while( it != m_users.end() )
  {
    if( user_id == (*it).id() )
      return true;
    ++it;
  }
  return false;
}

User UserList::find( VNumber user_id ) const
{
  if( user_id == ID_LOCAL_USER )
    return Settings::instance().localUser();
  QList<User>::const_iterator it = m_users.begin();
  while( it != m_users.end() )
  {
    if( user_id == (*it).id() )
      return *it;
    ++it;
  }
#ifdef BEEBEEP_DEBUG
  qDebug() << "Unable to find user with id" << user_id;
#endif
  return User();
}

void UserList::set( const User& u )
{
  bool user_found = false;
  QList<User>::iterator it = m_users.begin();
  while( it != m_users.end() )
  {
    if( (*it).id() == u.id() )
    {
      (*it) = u;
#ifdef BEEBEEP_DEBUG
      qDebug() << "User" << u.id() << "modified";
#endif
      user_found = true;
      break;
    }
    ++it;
  }
  if( !user_found )
    m_users.append( u );

  qSort( m_users );
}

QStringList UserList::toStringList( bool only_user_name, bool only_connected ) const
{
  QStringList sl;
  foreach( User u, m_users )
  {
    if( !only_connected || u.isConnected() )
      sl << (only_user_name ? u.name() : u.path() );
  }
  return sl;
}

UserList UserList::fromUsersId( const QList<VNumber>& users_id ) const
{
  UserList ul;
  foreach( VNumber user_id, users_id )
    ul.set( find( user_id ) );
  return ul;
}

QList<VNumber> UserList::toUsersId() const
{
  QList<VNumber> users_id;
  foreach( User u, m_users )
    users_id.append( u.id() );
  return users_id;
}

bool UserList::remove( const User& u )
{
  if( m_users.removeOne( u ) )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "User" << u.path() << "is removed from list";
#endif
    return true;
  }
  else
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Unable to remove user with path" << u.path();
#endif
    return false;
  }
}

void UserList::set( const UserList& ul )
{
  foreach( User u, ul.toList() )
    set( u );
}
