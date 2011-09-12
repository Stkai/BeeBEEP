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

#ifndef BEEBEEP_USERLIST_H
#define BEEBEEP_USERLIST_H

#include "Config.h"
#include "User.h"


class UserList
{
public:
  UserList();
  UserList( const UserList& );
  UserList& operator=( const UserList& );

  User find( VNumber ) const;
  User find( const QString& ) const;

  void setUser( const User& );

  QStringList toStringList( bool only_user_name, bool only_connected ) const;
  UserList fromUsersId( const QList<VNumber>& ) const;

private:
  QList<User> m_users;

};


#endif // BEEBEEP_USERLIST_H
