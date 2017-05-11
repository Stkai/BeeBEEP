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
// $Id: UserRecord.cpp 874 2017-05-07 17:04:05Z mastroddi $
//
//////////////////////////////////////////////////////////////////////

#include "ChatRecord.h"


ChatRecord::ChatRecord()
 : m_name( "" ), m_privateId( "" )
{
}

ChatRecord::ChatRecord( const ChatRecord& cr )
{
  (void)operator=( cr );
}

ChatRecord::ChatRecord( const QString& chat_name, const QString& chat_private_id )
 : m_name( chat_name ), m_privateId( chat_private_id )
{
}

ChatRecord& ChatRecord::operator=( const ChatRecord& cr )
{
  if( this != &cr )
  {
    m_name = cr.m_name;
    m_privateId = cr.m_privateId;
  }
  return *this;
}

bool ChatRecord::operator==( const ChatRecord& cr )
{
  if( m_privateId.isEmpty() )
  {
    if( m_name.isEmpty() )
      return false;
    else
      return m_name == cr.name();
  }
  else
    return m_privateId == cr.m_privateId;
}
