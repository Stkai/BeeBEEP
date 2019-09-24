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

#include "NetworkAddress.h"


NetworkAddress::NetworkAddress()
 : m_hostAddress(), m_hostPort( 0 ), m_info( "" )
{
}

NetworkAddress::NetworkAddress( const NetworkAddress& na )
{
  (void)operator=( na );
}

NetworkAddress::NetworkAddress( const QHostAddress& ha, quint16 hp )
 : m_hostAddress( ha ), m_hostPort( hp ), m_info( "" )
{
}

NetworkAddress& NetworkAddress::operator=( const NetworkAddress& na )
{
  if( this != &na )
  {
    m_hostAddress = na.m_hostAddress;
    m_hostPort = na.m_hostPort;
    m_info = na.m_info;
  }
  return *this;
}

bool NetworkAddress::isLinkLocal() const
{
  if( isIPv6Address() )
  {
    QPair<QHostAddress, int> ipv6_range_link_local = QHostAddress::parseSubnet( "fe80::/10" );
    return m_hostAddress.isInSubnet( ipv6_range_link_local );
  }
  else
  {
    QPair<QHostAddress, int> ipv4_range_link_local = QHostAddress::parseSubnet( "169.254.0.0/16" );
    if( m_hostAddress.isInSubnet( ipv4_range_link_local ) )
    {
      QPair<QHostAddress, int> ipv4_pre_link_local = QHostAddress::parseSubnet( "169.254.1.0/24" );
      QPair<QHostAddress, int> ipv4_post_link_local = QHostAddress::parseSubnet( "169.254.255.0/24" );
      if( !m_hostAddress.isInSubnet( ipv4_pre_link_local ) && !m_hostAddress.isInSubnet( ipv4_post_link_local ) )
        return true;
    }
    return false;
  }
}

QString NetworkAddress::toString() const
{
  if( isIPv6Address() && m_hostPort > 0 )
    return QString( "[%1]:%2" ).arg( m_hostAddress.toString() ).arg( m_hostPort );
  else if( m_hostPort > 0 )
    return QString( "%1:%2" ).arg( m_hostAddress.toString() ).arg( m_hostPort );
  else
    return m_hostAddress.toString();
}

NetworkAddress NetworkAddress::fromString( const QString& address_string )
{
  if( address_string.isEmpty() )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Invalid network address found in a empty string";
#endif
    return NetworkAddress();
  }

  QString host_address_string;
  int host_port = 0;
  bool port_is_ok = false;

  if( address_string.startsWith( "[" ) )
  {
    // ipv6 address [2001:db8:1f70::999:de8:7648:6e8]:6475
    QStringList sl_ipv6 = address_string.split( "]" );
    if( sl_ipv6.size() == 2 )
    {
      QString s_tmp = sl_ipv6.at( 1 );
      if( s_tmp.startsWith( ":" ) )
        s_tmp.remove( 0, 1 );
      host_port = s_tmp.toInt( &port_is_ok );
      if( !port_is_ok )
      {
  #ifdef BEEBEEP_DEBUG
        qDebug() << "Invalid network ipv6 port found in" << address_string;
  #endif
        return NetworkAddress();
      }

      host_address_string = sl_ipv6.at( 0 );
      if( host_address_string.startsWith( "[" ) )
        host_address_string.remove( 0, 1 );
    }
    else
    {
#ifdef BEEBEEP_DEBUG
      qDebug() << "Invalid network ipv6 address and port found in" << address_string;
#endif
      return NetworkAddress();
    }
  }
  else if( address_string.contains( ":" ) )
  {
    QStringList sl_ipv4 = address_string.split( ":" );
    if( sl_ipv4.size() == 2 )
    {
      host_port = sl_ipv4.at( 1 ).toInt( &port_is_ok );
      if( !port_is_ok )
      {
#ifdef BEEBEEP_DEBUG
        qDebug() << "Invalid network ipv4 port found in" << address_string;
#endif
        return NetworkAddress();
      }
      host_address_string = sl_ipv4.at( 0 );
    }
    else
      host_address_string = address_string; // ipv6
  }
  else
    host_address_string = address_string; // ipv4

  QHostAddress host_address( host_address_string );
  if( host_address.isNull() )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Invalid network address found in" << address_string;
#endif
    return NetworkAddress();
  }


  NetworkAddress na;
  na.setHostAddress( host_address );
  if( host_port > 0 && host_port <= MAX_SOCKET_PORT )
  {
    na.setHostPort( static_cast<quint16>(host_port) );
  }
#ifdef BEEBEEP_DEBUG
  else {
    qDebug() << "Host port" << host_port << "is outside the range [1-65535] found in" << address_string;
  }
#endif

  return na;
}
