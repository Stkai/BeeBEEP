//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2019 Marco Mastroddi
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
// along with BeeBEEP. If not, see <http://www.gnu.org/licenses/>.
//
// Author: Marco Mastroddi <marco.mastroddi(AT)gmail.com>
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

  bool has( VNumber ) const;
  User find( VNumber ) const;

  void set( const User& );
  void set( const UserList& );
  bool remove( const User& );
  inline int size() const;
  inline bool isEmpty() const;

  UserList fromUsersId( const QList<VNumber>& ) const;
  QList<VNumber> toUsersId() const;
  inline const QList<User>& toList() const;

  void sort();

private:
  QList<User> m_users;

};


// Inline Functions
inline int UserList::size() const { return m_users.size(); }
inline bool UserList::isEmpty() const { return m_users.isEmpty(); }
inline const QList<User>& UserList::toList() const { return m_users; }


#endif // BEEBEEP_USERLIST_H
