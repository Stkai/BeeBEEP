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


NetworkManager* NetworkManager::mp_instance = Q_NULLPTR;


NetworkManager::NetworkManager()
  : m_localHostAddress(), m_localBroadcastAddress(), m_localHostAddressScopeId( "" ),
    m_localInterfaceHardwareAddress( "" ), m_networkEntries()
{
}

QList<NetworkEntry> NetworkManager::availableNetworkEntries() const
{
  QList<NetworkEntry> network_entries;
  QList<QNetworkAddressEntry> network_address_entries;
  NetworkEntry network_entry;
  QString hardware_address;

  QList<QNetworkInterface> interface_list = QNetworkInterface::allInterfaces();

  foreach( QNetworkInterface if_net, interface_list )
  {
    if( networkInterfaceCanBroadcast( if_net ) )
    {
      hardware_address = if_net.hardwareAddress();
      if( !Settings::instance().isLocalHardwareAddressToSkip( hardware_address ) )
      {
        network_address_entries = if_net.addressEntries();
        foreach( QNetworkAddressEntry nae, network_address_entries )
        {
          network_entry = NetworkEntry( hardware_address, nae );
          if( isNetworkEntryAvailable( network_entry ) && !network_entries.contains( network_entry ) )
            network_entries.append( network_entry );
        }
      }
    }
  }
  return network_entries;
}

QNetworkInterface NetworkManager::localNetworkInterface() const
{
  if( !m_localInterfaceHardwareAddress.isEmpty() )
  {
    QList<QNetworkInterface> interface_list = QNetworkInterface::allInterfaces();
    foreach( QNetworkInterface if_net, interface_list )
    {
      if( isNetworkInterfaceAvailable( if_net ) && if_net.hardwareAddress() == m_localInterfaceHardwareAddress )
        return if_net;
    }
  }
  return QNetworkInterface();
}

bool NetworkManager::isMainInterfaceUp() const
{
  if( isMainInterfaceUnavailable() )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Unable to check if local interface hardware is up. Main interface is unavailable";
#endif
    return false;
  }

  QList<NetworkEntry> network_entries = availableNetworkEntries();
  foreach( NetworkEntry ne, network_entries )
  {
    if( m_localInterfaceHardwareAddress == ne.hardware() )
      return true;
  }

  return false;
}

bool NetworkManager::isLocalHostAddress( const QHostAddress& host_address ) const
{
  QList<QNetworkInterface> interface_list = QNetworkInterface::allInterfaces();
  foreach( QNetworkInterface if_net, interface_list )
  {
    QList<QNetworkAddressEntry> address_entries = if_net.addressEntries();
    foreach( QNetworkAddressEntry address_entry, address_entries)
    {
      if( address_entry.ip() == host_address )
        return true;
    }
  }
  return false;
}

QList<QHostAddress> NetworkManager::localBroadcastAddresses() const
{
  QList<QHostAddress> host_address_list;
  QList<NetworkEntry> network_entries = NetworkManager::instance().availableNetworkEntries();
  foreach( NetworkEntry ne, network_entries )
    host_address_list.append( ne.broadcast() );
  return host_address_list;
}

NetworkEntry NetworkManager::firstNetworkEntry( bool use_ipv4 ) const
{
  if( m_networkEntries.isEmpty() )
    return NetworkEntry();

  foreach( NetworkEntry network_entry, m_networkEntries )
  {
    if( use_ipv4 && network_entry.isIPv4Address() )
      return network_entry;

    if( !use_ipv4 && network_entry.isIPv6Address() )
      return network_entry;
  }

  return NetworkEntry();
}

bool NetworkManager::searchLocalHostAddress()
{
  m_localHostAddress = QHostAddress( "127.0.0.1" );
  m_localBroadcastAddress = QHostAddress();
  m_localHostAddressScopeId = "";
  m_localInterfaceHardwareAddress = "";
  if( !m_networkEntries.isEmpty() )
    m_networkEntries.clear();

  qDebug() << "Searching local host address...";
  // Collect the list
  m_networkEntries = availableNetworkEntries();
  if( m_networkEntries.isEmpty() )
  {
    qWarning() << "There are not available network entries (check your network interfaces)";
    return false;
  }

  foreach( NetworkEntry ne, m_networkEntries )
    qDebug() << "Network entry found:" << qPrintable( ne.hardware() ) << "-" << qPrintable( ne.hostAddress().toString() ) << "-" << qPrintable( ne.broadcast().toString() );

  // check forced ip
  if( forceLocalHostAddress( Settings::instance().localHostAddressForced() ) )
    return true;

  // check forced subnet
  if( !Settings::instance().localSubnetForced().isEmpty() )
  {
    qDebug() << "Checking local subnet forced:" << qPrintable( Settings::instance().localSubnetForced() );
    if( forceLocalSubnet( Settings::instance().localSubnetForced().trimmed() ) )
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

  NetworkEntry network_entry = firstNetworkEntry( true );
  if( network_entry.isValid() )
  {
    setLocalHostAddress( network_entry );
    return true;
  }

  network_entry = firstNetworkEntry( false );
  if( network_entry.isValid() )
  {
    setLocalHostAddress( network_entry );
    return true;
  }

  qWarning() << "Unable to set local host address";
  return false;
}

void NetworkManager::setLocalHostAddress( const NetworkEntry& network_entry )
{
  m_localHostAddress = network_entry.hostAddress();
  m_localInterfaceHardwareAddress = network_entry.hardware();
  qDebug() << "Local interface hardware address:" << qPrintable( m_localInterfaceHardwareAddress );

  if( network_entry.isIPv4Address() )
  {
    m_localBroadcastAddress = network_entry.broadcast();
    m_localHostAddressScopeId = "";
    qDebug() << "Local host address IPv4 selected:" << qPrintable( m_localHostAddress.toString() );
    qDebug() << "Broadcast to IPv4 address:" << qPrintable( m_localBroadcastAddress.toString() );
  }
  else if( network_entry.isIPv6Address() )
  {
    m_localBroadcastAddress = QHostAddress();
    m_localHostAddressScopeId = m_localHostAddress.scopeId();
    qDebug() << "Local host address IPv6 selected:" << qPrintable( m_localHostAddress.toString() );
    qDebug() << "IPv6 scope id:" << qPrintable( m_localHostAddressScopeId );
  }
  else
    qWarning() << "Invalid network entry in local host address found";
}

bool NetworkManager::forceLocalHostAddress( const QHostAddress& local_host_address_forced )
{
  if( local_host_address_forced.isNull() )
    return false;

  qDebug() << "Checking forced IP:" << qPrintable( local_host_address_forced.toString() );

  foreach( NetworkEntry network_entry, m_networkEntries )
  {
    if( network_entry.hostAddress() == local_host_address_forced )
    {
      setLocalHostAddress( network_entry );
      return true;
    }
  }

  qWarning() << "No network entry found with forced IP:" << qPrintable( local_host_address_forced.toString() );
  return false;
}

bool NetworkManager::forceLocalSubnet( const QString& local_subnet_forced )
{
  if( local_subnet_forced.simplified().isEmpty() )
    return false;

  if( m_networkEntries.isEmpty() )
    return false;

  foreach( NetworkEntry network_entry, m_networkEntries )
  {
    if( network_entry.isIPv4Address() )
    {
      if( network_entry.broadcast().toString() == local_subnet_forced )
      {
        qDebug() << "IPv4 host address" << qPrintable( network_entry.hostAddress().toString() ) << "is in forced subnet:" << qPrintable( local_subnet_forced );
        setLocalHostAddress( network_entry );
        return true;
      }
    }
    else if( network_entry.isIPv6Address()  )
    {
      if( network_entry.hostAddress().scopeId() == local_subnet_forced )
      {
        qDebug() << "IPv6 host address" << qPrintable( network_entry.hostAddress().toString() ) << "is in forced scope id:" << qPrintable( local_subnet_forced );
        setLocalHostAddress( network_entry );
        return true;
      }
    }
    else
      qWarning() << "Skip invalid network entry for host address" << qPrintable( network_entry.hostAddress().toString() );
  }

  qWarning() << "No network entry found with forced subnet:" << qPrintable( local_subnet_forced );
  return false;
}

QHostAddress NetworkManager::broadcastSubnetFromIPv4HostAddress( const QHostAddress& host_address ) const
{
  if( host_address.isNull() )
    return host_address;

  if( isInLocalBroadcastAddresses( host_address ) )
    return host_address;

  NetworkAddress network_address( host_address, 0 );

  if( !network_address.isIPv4Address() || network_address.isLoopback() || network_address.isLinkLocal() )
    return QHostAddress();

  QString s_host_address = host_address.toString();
  if( s_host_address.contains( QLatin1String( "255" ) ) )
    return network_address.hostAddress();

  QStringList sl_host_address = s_host_address.split( "." );
  if( sl_host_address.size() != 4 )
    return QHostAddress();

  sl_host_address.removeLast();
  sl_host_address.append( QLatin1String( "255" ) );
  return QHostAddress( sl_host_address.join( "." ) );
}

bool NetworkManager::isHostAddressInBroadcastSubnet( const QHostAddress& host_address, const QString& broadcast_subnet )
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

  if( host_address.protocol() == QAbstractSocket::IPv6Protocol )
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

QList<QHostAddress> NetworkManager::splitInIPv4HostAddresses( const QHostAddress& host_address ) const
{
  QList<QHostAddress> ha_list;
  QString ha_string = host_address.toString();

  QStringList ha_string_list = ha_string.split( "." );
  if( ha_string_list.size() != 4 )
  {
    qWarning() << "NetworkManager has found an invalid IPv4 address in subnet:" << ha_string;
    return ha_list;
  }

  bool ok = false;
  int max_ip = ha_string_list.last().toInt( &ok );
  if( !ok )
  {
    qWarning() << "NetworkManager has found an invalid IPv4 field in address:" << ha_string;
    return ha_list;
  }

  ha_list << host_address;

  if( host_address.protocol() == QAbstractSocket::IPv6Protocol )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "NetworkManager has found IPv6 address in broadcast subnet:" << ha_string;
#endif
    return ha_list;
  }

  if( ha_string.count( QLatin1String( "255" ) ) > 1 )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "NetworkManager has found IPv4 subnet" << ha_string << "to wide and cannot split it";
#endif
    return ha_list;
  }

  if( max_ip > 254 )
  {
    max_ip = 254;
    ha_list.clear(); // remove host address with 255
  }

  ha_string_list.removeLast();
  ha_string = ha_string_list.join( "." );
  QString s_tmp;
  for( int i = 1; i <= max_ip; i++ )
  {
    s_tmp = QString( "%1.%2" ).arg( ha_string ).arg( i );
    ha_list << QHostAddress( s_tmp );
  }

#ifdef BEEBEEP_DEBUG
  qDebug() << "NetworkManager has found IPv4 broadcast subnet" << ha_string << "and has splitted it in" << ha_list.size() << "addresses";
#endif

  return ha_list;
}
