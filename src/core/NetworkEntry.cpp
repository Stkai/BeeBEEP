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

#include "NetworkEntry.h"


NetworkEntry::NetworkEntry()
 : m_hardware( "" ), m_address(), m_broadcast(), m_netmask(), m_subnet()
{
}

NetworkEntry::NetworkEntry( const NetworkEntry& ne )
{
  (void)operator=( ne );
}

NetworkEntry::NetworkEntry( const QString& hw, const QNetworkAddressEntry& nae )
 : m_hardware( hw ), m_address( nae.ip(), 0 ), m_broadcast( nae.broadcast() ),
   m_netmask( nae.netmask() ), m_subnet()
{
  if( m_address.isIPv4Address() )
  {
    QPair<QHostAddress, int> subnet_host_address;
    if( !nae.broadcast().toString().endsWith( ".255" ) )
    {
      QStringList sl = nae.ip().toString().split( "." );
      sl.removeLast();
      sl.append( QString( "0" ) );
      subnet_host_address = QHostAddress::parseSubnet( QString( "%1/%2" ).arg( sl.join( "." ), nae.netmask().toString() ) );
    }
    else
      subnet_host_address = QHostAddress::parseSubnet( QString( "%1/%2" ).arg( nae.broadcast().toString().replace( ".255", ".0" ), nae.netmask().toString() ) );

    if( !subnet_host_address.first.isNull() )
      m_subnet = subnet_host_address;
  }
}

NetworkEntry& NetworkEntry::operator=( const NetworkEntry& ne )
{
  if( this != &ne )
  {
    m_hardware = ne.m_hardware;
    m_address = ne.m_address;
    m_broadcast = ne.m_broadcast;
    m_netmask = ne.m_netmask;
    m_subnet = ne.m_subnet;
  }
  return *this;
}

bool NetworkEntry::hasHostAddress( const QHostAddress& host_address ) const
{
  return host_address.isInSubnet( m_subnet.first, m_subnet.second );
}

