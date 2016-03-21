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

#include "NetworkEntry.h"


NetworkEntry::NetworkEntry()
 : m_hardware( "" ), m_address(), m_broadcast()
{
}

NetworkEntry::NetworkEntry( const NetworkEntry& ne )
{
  (void)operator=( ne );
}

NetworkEntry::NetworkEntry( const QString& hw, const QNetworkAddressEntry& nae )
 : m_hardware( hw ), m_address( nae.ip(), 0 ), m_broadcast( nae.broadcast() )
{
}

NetworkEntry& NetworkEntry::operator=( const NetworkEntry& ne )
{
  if( this != &ne )
  {
    m_hardware = ne.m_hardware;
    m_address = ne.m_address;
    m_broadcast = ne.m_broadcast;
  }
  return *this;
}
