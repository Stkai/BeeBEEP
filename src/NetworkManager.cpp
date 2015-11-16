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

#include "NetworkManager.h"
#include "Settings.h"


NetworkManager* NetworkManager::mp_instance = NULL;


NetworkManager::NetworkManager()
  : m_localHostAddress(), m_localBroadcastAddress(), m_localHostAddressScopeId( "" ),
    m_ipv4Addresses(), m_ipv6Addresses()
{
}

bool NetworkManager::searchLocalHostAddress()
{
  QList<QNetworkInterface> interface_list = QNetworkInterface::allInterfaces();
  QList<QHostAddress> interface_addresses;
  QList<QHostAddress> skipped_addresses;

  m_localHostAddress = QHostAddress( "127.0.0.1" );
  m_localBroadcastAddress = QHostAddress();
  m_localHostAddressScopeId = "";
  if( !m_ipv4Addresses.isEmpty() )
    m_ipv4Addresses.clear();
  if( !m_ipv6Addresses.isEmpty() )
    m_ipv6Addresses.clear();

  // Collect the list
  int interface_id = 0;
  foreach( QNetworkInterface if_net, interface_list )
  {
    if( (if_net.flags() & QNetworkInterface::IsUp) &&
         (if_net.flags() & QNetworkInterface::IsRunning) &&
         (if_net.flags() & ~QNetworkInterface::IsLoopBack) )
    {
       interface_addresses = if_net.allAddresses();
       interface_id++;
       foreach( QHostAddress host_address, interface_addresses )
       {
         if( host_address != QHostAddress::LocalHost && host_address != QHostAddress::LocalHostIPv6 )
         {
           if( isLinkLocal( host_address ) )
           {
             if( !skipped_addresses.contains( host_address) )
             {
               qDebug() << "In network interface" << interface_id << "skips link local:" << host_address.toString();
               skipped_addresses.append( host_address );
             }
           }
           else if( isIpv6Address( host_address ) )
           {
             if( !m_ipv6Addresses.contains( host_address ) )
             {
               qDebug() << "In network interface" << interface_id << "adds ipv6:" << host_address.toString();
               m_ipv6Addresses.append( host_address );
             }
           }
           else if( isIpv4Address( host_address ) )
           {
             if( !m_ipv4Addresses.contains( host_address ) )
             {
               qDebug() << "In network interface" << interface_id << "adds ipv4:" << host_address.toString();
               m_ipv4Addresses.append( host_address );
             }
           }
           else
           {
             if( !skipped_addresses.contains( host_address) )
             {
               qDebug() << "In network interface" << interface_id << "skips undefined:" << host_address.toString();
               skipped_addresses.append( host_address );
             }
           }
         }
         else
         {
           if( !skipped_addresses.contains( host_address) )
           {
             qDebug() << "In network interface" << interface_id << "skips local:" << host_address.toString();
             skipped_addresses.append( host_address );
           }
         }
       }
    }
  }

  // check forced ip
  if( forceLocalHostAddress( Settings::instance().localHostAddressForced() ) )
    return true;

  // check forced subnet
  if( !Settings::instance().localSubnetForced().isEmpty() )
  {
    qDebug() << "Checking local subnet forced:" << qPrintable( Settings::instance().localSubnetForced() );
    if( forceLocalSubnet( Settings::instance().localSubnetForced() ) )
      return true;
  }

  // check preferred subnets in RC
  if( !Settings::instance().preferredSubnets().isEmpty() )
  {
    QStringList preferred_subnets = Settings::instance().preferredSubnets().split( "," );
    if( !preferred_subnets.isEmpty() )
    {
       foreach( QString preferred_subnet, preferred_subnets )
      {
        qDebug() << "Checking preferred subnet from RC:" << preferred_subnet;
        if( forceLocalSubnet( preferred_subnet.trimmed() ) )
          return true;
      }
    }
  }

  if( !m_ipv4Addresses.isEmpty() )
  {
    setLocalHostAddress( m_ipv4Addresses.first() );
    return true;
  }

  if( !m_ipv6Addresses.isEmpty() )
  {
    setLocalHostAddress( m_ipv6Addresses.first() );
    return true;
  }

  qWarning() << "Local host address not found";
  return false;
}

void NetworkManager::setLocalHostAddress( const QHostAddress& host_address )
{
  if( isIpv4Address( host_address ) )
  {
    qDebug() << "Local host address IPV4 selected:" << host_address.toString();
    m_localHostAddress = host_address;
    m_localBroadcastAddress = broadcastSubnetFromIpv4HostAddress( host_address );
  }
  else if( isIpv6Address( host_address ) )
  {
    qDebug() << "Local host address IPV6 selected:" << host_address.toString();
    m_localHostAddress = host_address;
    m_localHostAddressScopeId = host_address.scopeId();
  }
}

bool NetworkManager::forceLocalHostAddress( const QHostAddress& local_host_address_forced )
{
  if( local_host_address_forced.isNull() )
    return false;

  qDebug() << "Checking forced IP:" << local_host_address_forced.toString();

  if( isIpv4Address( local_host_address_forced ) )
  {
    if( m_ipv4Addresses.isEmpty() )
      return false;

    foreach( QHostAddress host_address, m_ipv4Addresses )
    {
      if( host_address == local_host_address_forced )
      {
        setLocalHostAddress( host_address );
        return true;
      }
    }

    return false;
  }

  if( isIpv6Address( local_host_address_forced ) )
  {
    if( m_ipv6Addresses.isEmpty() )
      return false;

    foreach( QHostAddress host_address, m_ipv6Addresses )
    {
      if( host_address == local_host_address_forced )
      {
        setLocalHostAddress( host_address );
        return true;
      }
    }

    return false;
  }

  return false;
}

bool NetworkManager::forceLocalSubnet( const QString& local_subnet_forced )
{
  if( local_subnet_forced.simplified().isEmpty() )
    return false;

  if( !m_ipv4Addresses.isEmpty() )
  {
    foreach( QHostAddress host_address, m_ipv4Addresses )
    {
      if( hostAddressIsInBroadcastSubnet( host_address, local_subnet_forced ) )
      {
        qDebug() << "Ipv4 host address" << qPrintable( host_address.toString() ) << "is in subnet" << local_subnet_forced;
        setLocalHostAddress( host_address );
        return true;
      }
    }
  }

  if( !m_ipv6Addresses.isEmpty() )
  {
    foreach( QHostAddress host_address, m_ipv6Addresses )
    {
      if( hostAddressIsInBroadcastSubnet( host_address, local_subnet_forced ) )
      {
        qDebug() << "Ipv6 host address" << qPrintable( host_address.toString() ) << "is in subnet" << local_subnet_forced;
        setLocalHostAddress( host_address );
        return true;
      }
    }
  }

  return false;
}

QHostAddress NetworkManager::broadcastSubnetFromIpv4HostAddress( const QHostAddress& host_address ) const
{
  if( host_address.isNull() )
    return QHostAddress();

  if( !isIpv4Address( host_address ) )
    return QHostAddress();

  if( isLoopback( host_address ) )
    return QHostAddress();

  if( isLinkLocal( host_address) )
    return QHostAddress();

  QString s_host_address = host_address.toString();

  QStringList sl_host_address = s_host_address.split( "." );
  if( sl_host_address.size() != 4 )
    return QHostAddress();

  if( s_host_address.contains( QLatin1String( "255" ) ) )
    return host_address;

  sl_host_address.removeLast();
  sl_host_address.append( QLatin1String( "255" ) );
  return QHostAddress( sl_host_address.join( "." ) );
}

bool NetworkManager::isLinkLocal( const QHostAddress& host_address ) const
{
  if( isIpv6Address( host_address ) )
  {
    QPair<QHostAddress, int> ipv6_range_link_local = QHostAddress::parseSubnet( "fe80::/10" );
    return host_address.isInSubnet( ipv6_range_link_local );
  }
  else
  {
    QPair<QHostAddress, int> ipv4_range_link_local = QHostAddress::parseSubnet( "169.254.0.0/16" );
    if( host_address.isInSubnet( ipv4_range_link_local ) )
    {
      QPair<QHostAddress, int> ipv4_pre_link_local = QHostAddress::parseSubnet( "169.254.1.0/24" );
      QPair<QHostAddress, int> ipv4_post_link_local = QHostAddress::parseSubnet( "169.254.255.0/24" );
      if( !host_address.isInSubnet( ipv4_pre_link_local ) && !host_address.isInSubnet( ipv4_post_link_local ) )
        return true;
    }
    return false;
  }
}

bool NetworkManager::isLoopback( const QHostAddress& host_address ) const
{
#if QT_VERSION >= 0x050000
  return host_address.isLoopback();
#else
  return host_address == QHostAddress::LocalHost || host_address == QHostAddress::LocalHostIPv6;
#endif
}

bool NetworkManager::hostAddressIsInBroadcastSubnet( const QHostAddress& host_address, const QString& broadcast_subnet )
{
  if( broadcast_subnet.simplified().isEmpty() )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Unable to check host address" << host_address.toString() << "in a empty broadcast subnet";
#endif
    return false;
  }

#ifdef BEEBEEP_DEBUG
  qDebug() << "Checking host address" << host_address.toString() << "in subnet" << broadcast_subnet << "...";
#endif

  QPair<QHostAddress, int> subnet_parsed = QHostAddress::parseSubnet( broadcast_subnet );

  if( subnet_parsed.first.isNull() )
  {
#ifdef BEEBEEP_DEBUG
    qWarning() << "Subnet" << broadcast_subnet << "has null host address";
#endif
    return false;
  }

  if( host_address.isInSubnet( subnet_parsed ) )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Host address" << host_address.toString() << "is in parsed subnet" << broadcast_subnet;
#endif
    return true;
  }

  if( isIpv6Address( host_address ) )
    return false;

  if( broadcast_subnet.contains( "/" ) )
    return false;

  if( !broadcast_subnet.contains( "255" ) )
    return false;

  QStringList sl_ha = host_address.toString().split( "." );
  QStringList sl_bs = broadcast_subnet.split( "." );

  if( sl_ha.size() != 4 )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Host address" << host_address.toString() << "is invalid ipv4 address";
#endif
    return false;
  }

  if( sl_bs.size() != 4 )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Broadcast subnet" << broadcast_subnet << "is invalid ipv4 address";
#endif
    return false;
  }

  for( int i = 3; i >= 0; i-- )
  {
    if( sl_bs.at( i ) != QString( "255" ) && sl_ha.at( i ) != sl_bs.at( i ) )
    {
#ifdef BEEBEEP_DEBUG
      qDebug() << "Host address" << host_address.toString() << "is not in broadcast subnet" << broadcast_subnet;
#endif
      return false;
    }
  }

#ifdef BEEBEEP_DEBUG
  qDebug() << "Host address" << host_address.toString() << "is in broadcast subnet" << broadcast_subnet;
#endif

  return true;

}
