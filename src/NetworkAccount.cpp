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

#include "NetworkAccount.h"


NetworkAccount::NetworkAccount()
  : m_service( "" ), m_user( "" ), m_password( "" ),
    m_autoConnect( false ), m_saveUser( true ),
    m_savePassword( false )
{
}


NetworkAccount::NetworkAccount( const NetworkAccount& na )
{
  (void)operator=( na );
}

NetworkAccount& NetworkAccount::operator=( const NetworkAccount& na )
{
  if( this != &na )
  {
    m_service = na.m_service;
    m_user = na.m_user;
    m_password = na.m_password;
    m_autoConnect = na.m_autoConnect;
    m_saveUser = na.m_saveUser;
    m_savePassword = na.m_savePassword;
  }
  return *this;
}

QString NetworkAccount::toString() const
{
  QStringList sl;
  sl << m_service;
  if( m_saveUser )
    sl << m_user;
  else
    sl << "";
  if( m_savePassword )
    sl << m_password;
  else
    sl << "";
  sl << QString::number( (int)m_autoConnect );
  return sl.join( QChar( QChar::LineSeparator ) );
}

bool NetworkAccount::fromString( const QString& account_data )
{
  QStringList sl = account_data.split( QChar( QChar::LineSeparator ) );
  if( sl.size() < 4 )
    return false;
  m_service = sl.at( 0 );
  m_user = sl.at( 1 );
  m_password = sl.at( 2 );
  m_autoConnect = (bool)sl.at( 3 ).toInt();
  m_saveUser = !m_user.isEmpty();
  m_savePassword = !m_password.isEmpty();
  return true;
}
