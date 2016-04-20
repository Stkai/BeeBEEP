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

#include "User.h"


User::User()
  : m_id( ID_INVALID ), m_vCard(), m_hostAddress(), m_hostPort( 0 ),
    m_status( User::Offline ), m_statusDescription( "" ), m_color( "#000000" ), m_accountName( "" ),
    m_version( "" ), m_sessionId( "" ), m_isFavorite( false ), m_qtVersion( "0" ), m_protocolVersion( 0 )
{
}

User::User( VNumber new_id )
  : m_id( new_id ), m_vCard(), m_hostAddress( "127.0.0.1" ), m_hostPort( DEFAULT_LISTENER_PORT ),
    m_status( User::Offline ), m_statusDescription( "" ), m_color( "#000000" ), m_accountName( "" ),
    m_version( "" ), m_sessionId( "" ), m_isFavorite( false ), m_qtVersion( "0" ), m_protocolVersion( 0 )
{
  setName( QString( "Bee%1" ).arg( QString::number( new_id ) ) );
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
    m_hostAddress = u.m_hostAddress;
    m_hostPort = u.m_hostPort;
    m_status = u.m_status;
    m_statusDescription = u.m_statusDescription;
    m_color = u.m_color;
    m_accountName = u.m_accountName;
    m_version = u.m_version;
    m_sessionId = u.m_sessionId;
    m_isFavorite = u.m_isFavorite;
    m_qtVersion = u.m_qtVersion;
    m_protocolVersion = u.m_protocolVersion;
  }
  return *this;
}

bool User::isBirthDay() const
{
  if( m_vCard.birthday().isNull() )
    return false;
  QDate current_date = QDateTime::currentDateTime().date();
  return current_date.month() == m_vCard.birthday().month() && current_date.day() == m_vCard.birthday().day();
}

QString User::nameFromPath( const QString& user_path )
{
  QStringList sl = user_path.split( "@" );
  if( sl.size() > 2 )
  {
    sl.removeLast();
    return sl.join( "@" );
  }
  else if( sl.size() == 2 )
    return sl.first();
  else
    return QString();
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