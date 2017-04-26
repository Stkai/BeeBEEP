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

#include "UserRecord.h"


UserRecord::UserRecord()
  : m_name( "" ), m_account( "" ), m_networkAddress( QHostAddress(), DEFAULT_LISTENER_PORT ),
    m_comment( "" ), m_isFavorite( false ), m_color( "#000000" )
{
}

UserRecord::UserRecord( const UserRecord& ur )
{
  (void)operator=( ur );
}

UserRecord& UserRecord::operator=( const UserRecord& ur )
{
  if( this != &ur )
  {
    m_name = ur.m_name;
    m_account = ur.m_account;
    m_networkAddress = ur.m_networkAddress;
    m_comment = ur.m_comment;
    m_isFavorite = ur.m_isFavorite;
    m_color = ur.m_color;
  }
  return *this;
}

bool UserRecord::operator<( const UserRecord& ur ) const
{
  if( !ur.m_networkAddress.isHostAddressValid() )
    return false;
  else if( !m_networkAddress.isHostAddressValid() )
    return true;
  else
    return m_networkAddress.toString() < ur.m_networkAddress.toString();
}
