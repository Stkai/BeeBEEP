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

#include "User.h"


User::User()
 : m_id( ID_INVALID ), m_vCard(), m_networkAddress(), m_status( User::Offline ),
   m_statusDescription( "" ), m_color( "#000000" ), m_accountName( "" ),
   m_domainName( "" ), m_version( "" ), m_hash( "" ), m_isFavorite( false ),
   m_qtVersion( "0" ), m_protocolVersion( 0 ), m_statusChangedIn(), m_lastConnection(),
   m_workgroups(), m_localHostName( "" )
{
}

User::User( VNumber new_id )
 : m_id( new_id ), m_vCard(), m_networkAddress( QHostAddress( QHostAddress::LocalHost ), DEFAULT_LISTENER_PORT ),
   m_status( User::Offline ), m_statusDescription( "" ), m_color( "#000000" ), m_accountName( "" ),
   m_domainName( "" ), m_version( "" ), m_hash( "" ), m_isFavorite( false ), m_qtVersion( "0" ),
   m_protocolVersion( 0 ), m_statusChangedIn(), m_lastConnection(), m_workgroups(), m_localHostName( "" )
{
  setName( QString( "Bee %1" ).arg( QString::number( new_id ) ) );
}

User::User( VNumber new_id, const UserRecord& ur )
 : m_id( new_id ), m_vCard(), m_networkAddress( ur.networkAddress() ), m_status( User::Offline ),
   m_statusDescription( "" ), m_color( ur.color() ), m_accountName( ur.account() ),
   m_domainName( ur.domainName() ), m_version( "" ), m_hash( ur.hash() ), m_isFavorite( ur.isFavorite() ),
   m_qtVersion( "0" ), m_protocolVersion( 0 ), m_statusChangedIn(), m_lastConnection( ur.lastConnection() ),
   m_workgroups(), m_localHostName( ur.localHostName() )
{
  if( !ur.name().isEmpty() )
    setName( ur.name() );
  else if( !ur.account().isEmpty() )
    setName( ur.account() );
  else
    setName( QString( "Bee %1" ).arg( QString::number( new_id ) ) );
}

User::User( const User& u )
{
  (void)operator=( u );
}

User& User::operator=( const User& u )
{
  if( this != &u )
  {
    m_id = u.m_id;
    m_vCard = u.m_vCard;
    m_networkAddress = u.m_networkAddress;
    m_status = u.m_status;
    m_statusDescription = u.m_statusDescription;
    m_color = u.m_color;
    m_accountName = u.m_accountName;
    m_domainName = u.m_domainName;
    m_version = u.m_version;
    m_hash = u.m_hash;
    m_isFavorite = u.m_isFavorite;
    m_qtVersion = u.m_qtVersion;
    m_protocolVersion = u.m_protocolVersion;
    m_statusChangedIn = u.m_statusChangedIn;
    m_lastConnection = u.m_lastConnection;
    m_workgroups = u.m_workgroups;
    m_localHostName = u.m_localHostName;
  }
  return *this;
}

QString User::nameFromPath( const QString& user_path )
{
  if( !user_path.contains( "@" ) )
    return user_path;

  QStringList sl = user_path.split( "@" );
  if( sl.size() > 2 )
  {
    sl.removeLast();
    return sl.join( "@" );
  }
  else
    return sl.first();
}

QString User::hostAddressAndPortFromPath( const QString& user_path )
{
  QStringList sl = user_path.split( "@" );
  if( sl.size() < 2 )
    return QString();
  else
    return sl.last();
}

bool User::operator<( const User& u ) const
{
  if( isLocal() )
    return false;

  if( u.isLocal() )
    return true;

  if( u.isStatusConnected() && !isStatusConnected() )
    return false;

  if( !u.isStatusConnected() && isStatusConnected() )
    return true;

  if( u.isFavorite() && !isFavorite() )
    return false;

  if( !u.isFavorite() && isFavorite() )
    return true;

  return name() < u.name();
}

bool User::isBirthDay() const
{
  if( m_vCard.birthday().isNull() )
    return false;
  else
    return daysToBirthDay() == 0;
}

int User::daysToBirthDay() const
{
  if( m_vCard.birthday().isNull() )
    return -999;
  QDate d_today = QDate::currentDate();
  QDate next_birthday = QDate( d_today.year(), m_vCard.birthday().month(), m_vCard.birthday().day() );
  return static_cast<int>(d_today.daysTo( next_birthday ));
}

