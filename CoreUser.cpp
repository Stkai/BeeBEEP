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

namespace
{
  QString GetUserNameFromSystemEnvinroment()
  {
#if defined( BEEBEEP_DEBUG )
    return QString( "Bee%1" ).arg( QTime::currentTime().toString( "ss" ) );
#else
    qDebug() << "Checking local user system environment";
    QProcessEnvironment pe = QProcessEnvironment::systemEnvironment();
    QString sTmp = pe.value( "USERNAME" );
    if( sTmp.isNull() )
      sTmp = pe.value( "USER" );
    if( sTmp.isNull() )
      sTmp = QString( "Bee%1" ).arg( QTime::currentTime().toString( "ss" ) );
    return sTmp;
#endif
  }
}

void Core::createLocalUser()
{
  qDebug() << "Creating local user";
  QString sName = GetUserNameFromSystem();
  m_localUser.setName( sName );

  qDebug() << "Loading local user settings";
  QSettings sets( SETTINGS_FILE_NAME, SETTINGS_FILE_FORMAT );
  sets.beginGroup( "LocalUser" );
  m_localUser.setNickname( sets.value( "Nickname", "" ).toString() );
  m_localUser.setStatus( sets.value( "LastStatus", m_localUser.status() ).toInt() );
  m_localUser.setStatusDescription( sets.value( "LastStatusDescription", m_localUser.statusDescription() ).toString() );
  m_localUser.setHostPort( sets.value( "HostPort", LISTENER_DEFAULT_PORT ) );
  sets.endGroup();
  if( m_localUser.nickname().isEmpty() )
    m_localUser.setNickname( sName );
  qDebug() << "Local user:" << m_localUser.path();
}

void UserManager::saveLocalUser()
{
  QSettings sets( SETTINGS_FILE_NAME, SETTINGS_FILE_FORMAT );
  sets.beginGroup( "LocalUser" );
  sets.setValue( "Nickname", m_localUser.nickname() );
  sets.setValue( "LastStatus", m_localUser.status() );
  sets.setValue( "LastStatusDescription", m_localUser.statusDescription() );
  sets.setValue( "HostPort", m_localUser.hostPort() );
  sets.endGroup();
  sets.sync();
  qDebug() << "Local user saved";
}
