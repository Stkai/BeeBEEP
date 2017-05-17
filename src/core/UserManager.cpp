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

#include "Settings.h"
#include "UserManager.h"
#include "UserRecord.h"

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

User UserManager::findUserByPath( const QString& user_path ) const
{
  if( user_path.isEmpty() )
    return User();

  if( user_path.toLower() == Settings::instance().localUser().path().toLower() )
    return Settings::instance().localUser();

  foreach( User u, m_users.toList() )
  {
    if( u.path().toLower() == user_path.toLower() )
      return u;
  }

#ifdef BEEBEEP_DEBUG
  qDebug() << "Unable to find user with path:" << qPrintable( user_path );
#endif

  QString user_nickname = User::nameFromPath( user_path );
  User user_by_nickname = findUserByNickname( user_nickname );
  if( user_by_nickname.isValid() )
    return user_by_nickname;

#ifdef BEEBEEP_DEBUG
  qDebug() << "Unable to find user with nickname:" << qPrintable( user_nickname );
#endif

  QString host_and_port = User::hostAddressAndPortFromPath( user_path );

  if( host_and_port == Settings::instance().localUser().networkAddress().toString() )
    return Settings::instance().localUser();

  foreach( User u, m_users.toList() )
  {
    if( u.networkAddress().toString() == host_and_port )
      return u;
  }

#ifdef BEEBEEP_DEBUG
  qDebug() << "Unable to find user with host and port:" << qPrintable( host_and_port );
#endif

  return User();
}

User UserManager::findUserByAccountName( const QString& account_name, const QString& domain_name ) const
{
  if( account_name.isEmpty() )
    return User();

  bool user_found = false;
  if( account_name.toLower() == Settings::instance().localUser().accountName().toLower() )
  {
    user_found = true;
    if( !domain_name.isEmpty() && !Settings::instance().localUser().domainName().isEmpty() )
      user_found = domain_name.toLower() == Settings::instance().localUser().domainName().toLower();
    if( user_found )
      return Settings::instance().localUser();
  }

  foreach( User u, m_users.toList() )
  {
    if( u.accountName().toLower() == account_name.toLower() )
    {
      user_found = true;
      if( !domain_name.isEmpty() && !u.domainName().isEmpty() )
        user_found = domain_name.toLower() == u.domainName().toLower();
      if( user_found )
        return u;
    }
  }

#ifdef BEEBEEP_DEBUG
  qDebug() << "Unable to find user with account name" << qPrintable( account_name )
           << "and domain" << qPrintable( domain_name.isEmpty() ? "<vuoto>" : domain_name );
#endif

  return User();
}

User UserManager::findUserByNicknameAndHash( const QString& user_nickname, const QString& user_hash ) const
{
  if( user_nickname.isEmpty() || user_hash.isEmpty() )
    return User();

  if( user_nickname == Settings::instance().localUser().name() && user_hash == Settings::instance().localUser().hash() )
    return Settings::instance().localUser();

  foreach( User u, m_users.toList() )
  {
    if( u.name() == user_nickname && u.hash() == user_hash )
      return u;
  }

  #ifdef BEEBEEP_DEBUG
    qDebug() << "Unable to find user with nickname" << qPrintable( user_nickname ) << "and hash" << qPrintable( user_hash );
  #endif

    return User();
}

User UserManager::findUserByHash( const QString& user_hash ) const
{
  if( user_hash.isEmpty() )
    return User();

  if( user_hash == Settings::instance().localUser().hash() )
    return Settings::instance().localUser();

  foreach( User u, m_users.toList() )
  {
    if( u.hash() == user_hash )
      return u;
  }

#ifdef BEEBEEP_DEBUG
  qDebug() << "Unable to find user with hash:" << qPrintable( user_hash );
#endif

  return User();
}

User UserManager::findUserByNickname( const QString& user_nickname ) const
{
  if( user_nickname.isEmpty() )
    return User();

  if( Settings::instance().localUser().vCard().nickName().toLower() == user_nickname.toLower() )
    return Settings::instance().localUser();

  foreach( User u, m_users.toList() )
  {
    if( u.vCard().nickName().toLower() == user_nickname.toLower() )
      return u;
  }

#ifdef BEEBEEP_DEBUG
  qDebug() << "Unable to find user with nickname:" << qPrintable( user_nickname );
#endif
  return User();
}

User UserManager::findUserByUserRecord( const UserRecord& ur ) const
{
  if( Settings::instance().trustSystemAccount() )
  {
    return findUserByAccountName( ur.account(), ur.domainName() );
  }
  else
  {
    User u = findUserByNickname( ur.name() );
    if( !u.isValid() && Settings::instance().trustUserHash() )
      u = findUserByHash( ur.hash() );

    return u;
  }
}

User UserManager::findUserByNetworkAddress( const NetworkAddress& na ) const
{
  if( na.isHostAddressValid() && na.isHostPortValid() )
  {
    if( Settings::instance().localUser().networkAddress() == na )
      return Settings::instance().localUser();

    foreach( User u, m_users.toList() )
    {
      if( u.networkAddress() == na )
        return u;
    }
  }
  return User();
}
