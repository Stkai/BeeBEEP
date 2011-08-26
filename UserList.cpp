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
// $Id: UserManager.cpp 57 2011-08-19 16:24:06Z mastroddi $
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
  qDebug() << "Unable to find an user with id" << user_id;
  return User();
}

User UserList::find( const QString& user_path ) const
{
  if( user_path == Settings::instance().localUser().path() )
    return Settings::instance().localUser();
  QList<User>::const_iterator it = m_users.begin();
  while( it != m_users.end() )
  {
    if( user_path == (*it).path() )
      return *it;
    ++it;
  }
  qDebug() << "Unable to find an user with path" << user_path;
  return User();
}

void UserList::setUser( const User& u )
{
  if( u.id() == ID_LOCAL_USER )
  {
    Settings::instance().setLocalUser( u );
    return;
  }
  QList<User>::iterator it = m_users.begin();
  while( it != m_users.end() )
  {
    if( (*it).id() == u.id() )
    {
      (*it) = u;
      qDebug() << "User" << u.id() << "modified";
      return;
    }
    ++it;
  }
  m_users.append( u );
  qDebug() << "User" << u.path() << "added to user list with id" << u.id();
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
    ul.setUser( find( user_id ) );
  return ul;
}
