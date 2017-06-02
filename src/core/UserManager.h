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

#ifndef BEEBEEP_USERMANAGER_H
#define BEEBEEP_USERMANAGER_H

#include "UserList.h"
#include "Group.h"

class UserManager
{
// Singleton Object
  static UserManager* mp_instance;

public:
  void setUser( const User& );
  inline bool removeUser( const User& );
  inline const UserList& userList() const;
  inline User findUser( VNumber ) const;
  User findUserByPath( const QString& ) const;
  User findUserByAccountNameAndDomainName( const QString& account_name, const QString& domain_name ) const;
  User findUserByAccountName( const QString& ) const;
  User findUserByHash( const QString& ) const;
  User findUserByNickname( const QString& ) const;
  User findUserByNetworkAddress( const NetworkAddress& ) const;

  void addNewConnectedUserId( VNumber );
  bool removeNewConnectedUserId( VNumber );
  inline void clearNewConnectedUserIdList();
  UserList newConnectedUserList() const;
  inline bool hasNewConnectedUsers() const;

  static UserManager& instance()
  {
    if( !mp_instance )
      mp_instance = new UserManager();
    return *mp_instance;
  }

  static void close()
  {
    if( mp_instance )
    {
      delete mp_instance;
      mp_instance = NULL;
    }
  }

protected:
  UserManager();

private:
  UserList m_users;
  QList<VNumber> m_newConnectedUserIdList;

};


// Inline Function
inline User UserManager::findUser( VNumber user_id ) const { return m_users.find( user_id ); }
inline bool UserManager::removeUser( const User& u ) { return u.isLocal() ? false : m_users.remove( u ); }
inline const UserList& UserManager::userList() const { return m_users; }
inline bool UserManager::removeNewConnectedUserId( VNumber user_id ) { return m_newConnectedUserIdList.removeOne( user_id ); }
inline void UserManager::clearNewConnectedUserIdList() { m_newConnectedUserIdList.clear(); }
inline UserList UserManager::newConnectedUserList() const { return m_users.fromUsersId( m_newConnectedUserIdList ); }
inline bool UserManager::hasNewConnectedUsers() const { return m_newConnectedUserIdList.size() > 0; }

#endif // BEEBEEP_USERMANAGER_H
