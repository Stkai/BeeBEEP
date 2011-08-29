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

#include "User.h"


User::User()
  : m_id( ID_INVALID ), m_name( "" ), m_hostAddress(), m_hostPort( 0 ),
    m_status( 0 ), m_statusDescription( "" ), m_color( "#000000" ), m_protoVersion( 0 )
{
}

User::User( VNumber new_id )
  : m_id( new_id ), m_name(), m_hostAddress( "127.0.0.1" ), m_hostPort( LISTENER_DEFAULT_PORT ),
    m_status( 0 ), m_statusDescription( "" ), m_color( "#000000" ), m_protoVersion( 0 )
{
  m_name = QString( "Bee%1" ).arg( QString::number( new_id ) );
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
    m_name = u.m_name;
    m_hostAddress = u.m_hostAddress;
    m_hostPort = u.m_hostPort;
    m_status = u.m_status;
    m_statusDescription = u.m_statusDescription;
    m_color = u.m_color;
    m_protoVersion = u.m_protoVersion;
  }
  return *this;
}
