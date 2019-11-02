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

#include "UserStatusRecord.h"


UserStatusRecord::UserStatusRecord()
  : m_status( -1 ), m_statusDescription( "" )
{
}

UserStatusRecord::UserStatusRecord( const UserStatusRecord& usr )
{
  (void)operator=( usr );
}

UserStatusRecord& UserStatusRecord::operator=( const UserStatusRecord& usr )
{
  if( this != &usr )
  {
    m_status = usr.m_status;
    m_statusDescription = usr.m_statusDescription;
  }
  return *this;
}

bool UserStatusRecord::operator<( const UserStatusRecord& usr ) const
{
  if( m_status == usr.m_status )
    return m_statusDescription < usr.m_statusDescription;
  else
    return m_status < usr.m_status;
}
