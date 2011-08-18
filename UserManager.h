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

#ifndef BEEBEEP_USERMANAGER_H
#define BEEBEEP_USERMANAGER_H

#include "User.h"


class UserManager
{
public:
  inline const QList<User>& users() const;
  inline const User& localUser() const;
  inline void setLocalUser( const User& );
  void setLocalhost( const QHostAddress&, int );

  User createUser( const QString& user_name, const QString& user_nickname, const QHostAddress& user_address, int user_port );
  void setUser( const User& );
  User user( VNumber ) const;

  void load();
  void save();

protected:
  UserManager();
  inline VNumber newId();

private:
  VNumber m_id;
  User m_localUser;
  QList<User> m_users;

};


// Inline Function
inline VNumber UserManager::newId() { return ++m_id; }
inline const User& UserManager::localUser() const { return m_localUser; }
inline void UserManager::setLocalUser( const User& new_value ) { m_localUser = new_value; }
inline const QList<User>& UserManager::users() const { return m_users; }
inline QString UserManager::defaultChat() const { return chat( ID_LOCAL_USER ); }
inline QString UserManager::chat( VNumber user_id ) const { return user_id == ID_LOCAL_USER ? QObject::tr( "*All*" ) : user( user_id ).path(); }

#endif // BEEBEEP_USERMANAGER_H
