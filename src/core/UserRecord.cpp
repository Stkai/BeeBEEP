//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2020 Marco Mastroddi
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

#include "UserRecord.h"


UserRecord::UserRecord()
 : m_name( "" ), m_account( "" ), m_networkAddress( QHostAddress(), DEFAULT_LISTENER_PORT ),
   m_isFavorite( false ), m_color( "#000000" ), m_hash( "" ), m_domainName( "" ), m_lastConnection(),
   m_birthday(),  m_firstName( "" ), m_lastName( "" ), m_email( "" ), m_phoneNumber( "" ), m_localHostName( "" )
{
}

UserRecord::UserRecord( const UserRecord& ur )
{
  (void)operator=( ur );
}

UserRecord::UserRecord( const QString& user_name, const QString& user_account, const QString& user_hash, const QString& domain_name )
 : m_name( user_name ), m_account( user_account ), m_networkAddress( QHostAddress(), DEFAULT_LISTENER_PORT ),
   m_isFavorite( false ), m_color( "#000000" ), m_hash( user_hash ), m_domainName( domain_name ), m_lastConnection(),
   m_birthday(), m_firstName( "" ), m_lastName( "" ), m_email( "" ), m_phoneNumber( "" ), m_localHostName( "" )
{
}

UserRecord& UserRecord::operator=( const UserRecord& ur )
{
  if( this != &ur )
  {
    m_name = ur.m_name;
    m_account = ur.m_account;
    m_networkAddress = ur.m_networkAddress;
    m_isFavorite = ur.m_isFavorite;
    m_color = ur.m_color;
    m_hash = ur.m_hash;
    m_domainName = ur.m_domainName;
    m_lastConnection = ur.m_lastConnection;
    m_birthday = ur.m_birthday;
    m_firstName = ur.m_firstName;
    m_lastName = ur.m_lastName;
    m_email = ur.m_email;
    m_phoneNumber = ur.m_phoneNumber;
    m_localHostName = ur.m_localHostName;
  }
  return *this;
}
