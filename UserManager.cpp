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

#include "UserManager.h"
#include "ColorManager.h"


UserManager* UserManager::mp_instance = NULL;


UserManager::UserManager()
  : m_id( ID_START ), m_localUser( ID_LOCAL_USER ), m_users()
{
}

QString UserManager::getUserName( const QProcessEnvironment& pe )
{
#if defined( BEEBEEP_DEBUG )
  return QString( "Bee%1" ).arg( QTime::currentTime().toString( "ss" ) );
#else
  QString sTmp = pe.value( "USERNAME" );
  if( sTmp.isNull() )
    sTmp = pe.value( "USER" );
  if( sTmp.isNull() )
    sTmp = QString( "Bee%1" ).arg( QTime::currentTime().toString( "ss" ) );
  return sTmp;
#endif
}

void UserManager::checkSystemEnvinroment()
{
#if defined( BEEBEEP_DEBUG )
  qDebug() << "Checking local user system environment";
#endif
  QProcessEnvironment pe = QProcessEnvironment::systemEnvironment();
  QString sName = getUserName( pe );
  m_localUser.setName( sName );
  if( m_localUser.nickname().isEmpty() )
    m_localUser.setNickname( sName );
}

void UserManager::load()
{
#if defined( BEEBEEP_DEBUG )
  qDebug() << "Loading local user";
#endif
  checkSystemEnvinroment();
  QSettings sets( SETTINGS_FILE_NAME, SETTINGS_FILE_FORMAT );
  sets.beginGroup( "LocalUser" );
  m_localUser.setNickname( sets.value( "Nickname", "" ).toString() );
  m_localUser.setStatus( sets.value( "LastStatus", m_localUser.status() ).toInt() );
  m_localUser.setStatusDescription( sets.value( "LastStatusDescription", m_localUser.statusDescription() ).toString() );
  m_localUser.setHostPort( sets.value( "HostPort", LISTENER_DEFAULT_PORT ) );
  sets.endGroup();
#if defined( BEEBEEP_DEBUG )
  qDebug() << "Local user:" << m_localUser.path();
#endif
}

void UserManager::save()
{
#if defined( BEEBEEP_DEBUG )
  qDebug() << "Saving local user";
#endif
  QSettings sets( SETTINGS_FILE_NAME, SETTINGS_FILE_FORMAT );
  sets.beginGroup( "LocalUser" );
  sets.setValue( "Nickname", m_localUser.nickname() );
  sets.setValue( "LastStatus", m_localUser.status() );
  sets.setValue( "LastStatusDescription", m_localUser.statusDescription() );
  sets.setValue( "HostPort", m_localUser.hostPort() );
  sets.endGroup();
  sets.sync();
#if defined( BEEBEEP_DEBUG )
  qDebug() << "Local user saved";
#endif
}

User UserManager::createUser( const QString& user_name, const QString& user_nickname, const QHostAddress& user_address, int user_port )
{
  User u;
  u.setName( user_name );
  u.setNickname( user_nickname );
  u.setHostAddress( user_address );
  u.setHostPort( user_port );
  u.setStatus( User::Online );
  return setUser( u );
}

User UserManager::user( VNumber user_id ) const
{
  if( user_id == m_localUser.id() )
    return m_localUser;
  // Because QList is implemented as an array of pointers, this operation is very fast
  for( int i = 0; i < m_users.size(); i++ )
  {
    if( m_users.at( i ).id() == user_id )
      return m_users.at( i );
  }
  return User();
}

User UserManager::setUser( const User& u )
{
  QList<User>::iterator it = m_users.begin();
  while( it != m_user.end() )
  {
    if( u.isValid() )
    {
      if( (*it).id() == u.id() )
      {
        (*it) = u;
        return *it;
      }
    }
    else
    {
      if( (*it).path() == u.path() )
      {
        u.setId( (*it).id() );
        (*it) = u;
        return *it;
      }
    }
    ++it;
  }

  User user_to_append = u;
  user_to_append.setId( newId() );
  user_to_append.setColor( ColorManager::instance().unselectedQString() );
  m_users.append( user_to_append );
  return user_to_append;
}

void UserManager::setLocalhost( const QHostAddress& user_host, int user_port )
{
  m_localUser.setHostAddress( user_host );
  m_localUser.setHostPort( user_port );
}
