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
 : m_id( 0 ), m_name( "" ), m_nickname( "" ), m_hostAddress(), m_status( 0 ), m_statusDescription( "" )
{
}

User::User( VNumber new_id )
 : m_id( new_id ), m_name( "Bee" ), m_nickname( "BeeBoh" ), m_hostAddress( "127.0.0.1" ), m_status( 0 ), m_statusDescription( "" )
{
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
    m_nickname = u.m_nickname;
    m_hostAddress = u.m_hostAddress;
    m_status = u.m_status;
    m_statusDescription = u.m_statusDescription;
  }
  return *this;
}
