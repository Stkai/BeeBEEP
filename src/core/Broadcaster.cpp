//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2021 Marco Mastroddi
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

#include "Broadcaster.h"
#include "Hive.h"
#include "NetworkManager.h"
#include "Protocol.h"
#include "Settings.h"
#include "UserManager.h"


Broadcaster::Broadcaster( QObject *parent )
  : QObject( parent ), m_networkAddresses(), m_newBroadcastRequested( false ),
    m_networkAddressesWaitingForLoopback(), m_multicastGroupAddress(), m_isMulticastDatagramSent( false ),
    m_lastDatagramSentTimestamp(), m_networkAddressesIsSorted( false )
{
  mp_receiverSocket = new QUdpSocket( this );
  mp_senderSocket = new QUdpSocket( this );
}

bool Broadcaster::startBroadcastServer()
{
  if( !mp_receiverSocket->bind( Settings::instance().hostAddressToListen(), static_cast<quint16>(Settings::instance().defaultBroadcastPort()), QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint ) )
  {
    qWarning() << "Broadcast receiver cannot bind the broadcast port" << Settings::instance().defaultBroadcastPort();
    return false;
  }

  connect( mp_receiverSocket, SIGNAL( readyRead() ), this, SLOT( readBroadcastDatagram() ) );

#if QT_VERSION >= 0x040800
  if( Settings::instance().disableMulticast() )
  {
    qDebug() << "Broadcaster found multicast option disabled";
    m_multicastGroupAddress = QHostAddress();
  }
  else
  {
    m_multicastGroupAddress = Settings::instance().useDefaultMulticastGroupAddress() ? Settings::instance().defaultMulticastGroupAddress() : Settings::instance().multicastGroupAddress();
    if( !m_multicastGroupAddress.isNull() )
    {
      if( !Settings::instance().useIPv6() )
      {
        if( Settings::instance().ipMulticastTtl() > 0 && Settings::instance().ipMulticastTtl() < 256 )
        {
          qDebug() << "Broadcaster UDP Socket uses Multicast TTL Option" << Settings::instance().ipMulticastTtl();
          mp_senderSocket->setSocketOption( QAbstractSocket::MulticastTtlOption, Settings::instance().ipMulticastTtl() );
        }
        else
        {
          qWarning() << Settings::instance().ipMulticastTtl() << "is an invalid value for Multicast TTL option";
          qDebug() << "Broadcaster UDP Socket uses default Multicast TTL Option" << DEFAULT_IPV4_MULTICAST_TTL_OPTION;
          mp_senderSocket->setSocketOption( QAbstractSocket::MulticastTtlOption, DEFAULT_IPV4_MULTICAST_TTL_OPTION );
        }
      }

      int multicast_interfaces = 0;
      QList<QNetworkInterface> interface_list = QNetworkInterface::allInterfaces();
      foreach( QNetworkInterface if_net, interface_list )
      {
        if( NetworkManager::instance().networkInterfaceCanMulticast( if_net ) )
        {
          QString hardware_address = if_net.hardwareAddress();
          if( hardware_address.isEmpty() )
          {
            bool add_this_hw_address = true;
            hardware_address = "[unknown device";
            QList<QNetworkAddressEntry> address_entries = if_net.addressEntries();
            foreach( QNetworkAddressEntry address_entry, address_entries )
            {
              if( NetworkAddress::isLoopback( address_entry.ip() ) )
                add_this_hw_address = false;
              hardware_address += QString( " - %1" ).arg( address_entry.ip().toString() );
            }
            hardware_address += "]";

            if( !add_this_hw_address )
            {
              qDebug() << "Broadcaster skips to join to the multicast group" << qPrintable( m_multicastGroupAddress.toString() )
                       << "in network interface" << qPrintable( hardware_address );
              continue;
            }
          }

          if( !Settings::instance().isLocalHardwareAddressToSkip( hardware_address ) )
          {
            if( mp_receiverSocket->joinMulticastGroup( m_multicastGroupAddress, if_net ) )
            {
              qDebug() << "Broadcaster joins to the multicast group" << qPrintable( m_multicastGroupAddress.toString() )
                       << "in network interface" << qPrintable( hardware_address );
              multicast_interfaces++;
            }
          }
        }
      }

      if( multicast_interfaces == 0 )
      {
        if( !mp_receiverSocket->joinMulticastGroup( m_multicastGroupAddress ) )
        {
          qWarning() << "Broadcaster cannot join to the multicast group" << qPrintable( m_multicastGroupAddress.toString() ) << "in default network interface";
          m_multicastGroupAddress = QHostAddress();
        }
        else
          qDebug() << "Broadcaster joins to the multicast group" << qPrintable( m_multicastGroupAddress.toString() ) << "in default network interface";
      }
    }
  }
#endif

  if( !m_networkAddresses.isEmpty() )
    m_networkAddresses.clear();
  if( !m_networkAddressesWaitingForLoopback.isEmpty() )
    m_networkAddressesWaitingForLoopback.clear();
  m_newBroadcastRequested = false;
  return true;
}

void Broadcaster::stopBroadcasting()
{
  disconnect( mp_receiverSocket, SIGNAL( readyRead() ), this, SLOT( readBroadcastDatagram() ) );

  if( mp_receiverSocket->state() == QAbstractSocket::BoundState )
  {
#if QT_VERSION >= 0x040800
    if( !m_multicastGroupAddress.isNull() )
    {
      int multicast_interfaces = 0;
      QList<QNetworkInterface> interface_list = QNetworkInterface::allInterfaces();
      foreach( QNetworkInterface if_net, interface_list )
      {
        if( NetworkManager::instance().networkInterfaceCanMulticast( if_net ) )
        {
          QString hardware_address = if_net.hardwareAddress();
          if( !Settings::instance().isLocalHardwareAddressToSkip( hardware_address ) )
          {
            if( mp_receiverSocket->leaveMulticastGroup( m_multicastGroupAddress, if_net ) )
            {
              multicast_interfaces++;
              qDebug() << "Leave from the multicast group" << qPrintable( m_multicastGroupAddress.toString() ) << "in network interface" << qPrintable( hardware_address );
            }
          }
        }
      }

      if( multicast_interfaces == 0 )
      {
        if( mp_receiverSocket->leaveMulticastGroup( m_multicastGroupAddress ) )
          qDebug() << "Leave from the multicast group" << qPrintable( m_multicastGroupAddress.toString() ) << "in default network interface";
        else
          qWarning() << "Unable to leave from the multicast group" << qPrintable( m_multicastGroupAddress.toString() ) << "in default network interface";
      }
    }
#endif
    mp_receiverSocket->close();
  }

  qDebug() << "Broadcaster stops to search users";

  if( !m_networkAddresses.isEmpty() )
    m_networkAddresses.clear();
  if( !m_networkAddressesWaitingForLoopback.isEmpty() )
    m_networkAddressesWaitingForLoopback.clear();
  m_newBroadcastRequested = false;
  m_isMulticastDatagramSent = false;
}

void Broadcaster::sendBroadcast()
{
  m_isMulticastDatagramSent = false;
  sendMulticastDatagram();
  m_newBroadcastRequested = true;
}

bool Broadcaster::addNetworkAddress( const NetworkAddress& network_address, bool split_ipv4_address )
{
  if( !network_address.isHostAddressValid() )
    return false;

  if( m_networkAddresses.contains( network_address ) )
    return false;

  int list_size = m_networkAddresses.size();

  if( split_ipv4_address && (network_address.isHostPortValid() || !NetworkManager::instance().isInLocalBroadcastAddresses( network_address.hostAddress() ) ))
  {
    QList<QHostAddress> host_addresses_to_add = NetworkManager::instance().splitInIPv4HostAddresses( network_address.hostAddress() );
    if( host_addresses_to_add.isEmpty() )
      return false;

    foreach( QHostAddress ha, host_addresses_to_add )
    {
      NetworkAddress na( ha, network_address.hostPort() );
      if( !m_networkAddresses.contains( na ) )
        m_networkAddresses.append( na );
    }
  }
  else
    m_networkAddresses.append( network_address );

  m_networkAddressesIsSorted = list_size == m_networkAddresses.size();
  return true;
}

bool Broadcaster::broadcastToNetworkAddress( const NetworkAddress& na )
{
  if( !na.isHostAddressValid() )
    return false;

  QByteArray broadcast_data = Protocol::instance().broadcastMessage( na.hostAddress() );
  quint16 default_port = static_cast<quint16>( Settings::instance().defaultBroadcastPort() );
  quint16 host_port = na.isHostPortValid() ? na.hostPort() : default_port;
  if( mp_senderSocket->writeDatagram( broadcast_data, na.hostAddress(), host_port ) > 0 )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Broadcaster sends datagram to" << qPrintable( na.hostAddress().toString() ) << host_port;
#endif
    if( na.hostAddress() == NetworkManager::instance().localBroadcastAddress() && host_port == default_port )
    {
      m_networkAddressesWaitingForLoopback.append( QPair<NetworkAddress,QDateTime>( na, QDateTime::currentDateTime() ) );
#ifdef BEEBEEP_DEBUG
      qDebug() << "Waiting for loopback datagram from" << qPrintable( na.hostAddress().toString() );
#endif
    }
    m_lastDatagramSentTimestamp = QDateTime::currentDateTime();
    return true;
  }
  else
  {
    qWarning() << "Unable to send datagram to" << qPrintable( na.hostAddress().toString() ) << host_port;
    return false;
  }
}

void Broadcaster::checkLoopbackDatagram()
{
#ifdef BEEBEEP_DEBUG
  qDebug() << "Check" << m_networkAddressesWaitingForLoopback.size() << "loopback datagram";
#endif
  QList< QPair<NetworkAddress, QDateTime> >::iterator it = m_networkAddressesWaitingForLoopback.begin();
  while( it != m_networkAddressesWaitingForLoopback.end() )
  {
#if QT_VERSION >= 0x040700
    if( it->second.msecsTo( QDateTime::currentDateTime() ) > 5600 )
#else
    if( it->second.secsTo( QDateTime::currentDateTime() ) > 5 )
#endif
    {
      qWarning() << "Broadcaster didn't received yet a loopback datagram from" << qPrintable( it->first.toString() );
      if( !m_multicastGroupAddress.isNull() && it->first.hostAddress() == m_multicastGroupAddress && !Settings::instance().broadcastToLocalSubnetAlways() )
      {
        qDebug() << "Broadcaster also tries to send to datagram to local address" << qPrintable( NetworkManager::instance().localBroadcastAddress().toString() );
        addHostAddress( NetworkManager::instance().localBroadcastAddress() );
      }
      it = m_networkAddressesWaitingForLoopback.erase( it );
    }
    else
      ++it;
  }
}

void Broadcaster::removeHostAddressFromWaitingList( const QHostAddress& host_address )
{
  QList< QPair<NetworkAddress, QDateTime> >::iterator it = m_networkAddressesWaitingForLoopback.begin();
  while( it != m_networkAddressesWaitingForLoopback.end() )
  {
    if( it->first.hostAddress() == host_address )
    {
      qDebug() << "Broadcaster has received loopback datagram from" << qPrintable( it->first.toString() );
      it = m_networkAddressesWaitingForLoopback.erase( it );
    }
    else
      ++it;
  }
}

void Broadcaster::readBroadcastDatagram()
{
  int num_datagram_read = 0;
  while( mp_receiverSocket->hasPendingDatagrams() && num_datagram_read < MAX_NUM_OF_LOOP_IN_CONNECTON_SOCKECT )
  {
    num_datagram_read++;
    QHostAddress sender_ip;
    quint16 sender_port;
    QByteArray datagram;

    datagram.resize( static_cast<int>(mp_receiverSocket->pendingDatagramSize()) );

    if( mp_receiverSocket->readDatagram( datagram.data(), datagram.size(), &sender_ip, &sender_port ) == -1 )
    {
      qWarning() << "Broadcasting has found and error reading datagram" << num_datagram_read;
      continue;
    }

    if( datagram.size() <= Protocol::instance().messageMinimumSize() )
    {
      qWarning() << "Broadcaster has received an invalid data size:" << datagram;
      continue;
    }

    if( sender_ip.isNull() )
    {
      qWarning() << "Broadcaster has received datagram from invalid host address";
      continue;
    }

    Message m = Protocol::instance().toMessage( datagram, 1 );
    if( !m.isValid() || m.type() != Message::Beep )
    {
      qWarning() << "Broadcaster has received an invalid datagram from" << qPrintable( sender_ip.toString());
      continue;
    }

    bool ok = false;
    int sender_listener_port = m.text().toInt( &ok );
    if( !ok )
    {
      qWarning() << "Broadcaster has received an invalid listener port from" << qPrintable( sender_ip.toString() );
      continue;
    }

    removeHostAddressFromWaitingList( sender_ip );
    // host_address_from_datagram it the host target of the message (so for multicast is 239.255.64.75,
    // for broadcast 192.168.1.255, 10.0.0.255, ...)
    QHostAddress host_address_from_datagram = Protocol::instance().hostAddressFromBroadcastMessage( m );
    if( host_address_from_datagram.isNull() )
      qWarning() << "Broadcaster has received an invalid host address in datagram from" << qPrintable( sender_ip.toString() );
    else
      removeHostAddressFromWaitingList( host_address_from_datagram );

    if( sender_listener_port == Settings::instance().localUser().networkAddress().hostPort() && NetworkManager::instance().isLocalHostAddress( sender_ip ) )
    {
#ifdef BEEBEEP_DEBUG
      qDebug() << "Broadcaster skips its own datagram received from" << qPrintable( sender_ip.toString() ) << "with source" << qPrintable( host_address_from_datagram.toString() );
#endif
      continue;
    }

    if( host_address_from_datagram.isNull() )
      qDebug() << "Broadcaster has found new peer" << qPrintable( sender_ip.toString() ) << sender_listener_port;
    else
      qDebug() << "Broadcaster has found new peer" << qPrintable( sender_ip.toString() ) << sender_listener_port << "with datagram from" << qPrintable( host_address_from_datagram.toString() );
    emit newPeerFound( sender_ip, sender_listener_port );
  }

  if( num_datagram_read > 1 )
    qDebug() << "Broadcaster read" << num_datagram_read << "datagrams";
}

void Broadcaster::updateUsersAddedManually()
{
#ifdef BEEBEEP_DEBUG
  qDebug() << "Broadcaster has found" << Settings::instance().networkAddressList().size() << "network address added by user";
#endif
  if( !Settings::instance().networkAddressList().isEmpty() )
  {
    foreach( QString s_network_address, Settings::instance().networkAddressList() )
    {
      NetworkAddress na = Protocol::instance().loadNetworkAddress( s_network_address );
      if( na.isHostAddressValid() && na.isHostPortValid() )
        addNetworkAddress( na, false );
      else
        qWarning() << "Broadcaster has found error in network address saved in file settings:" << qPrintable( s_network_address );
    }
  }
}

int Broadcaster::updateUsersFromHive()
{
  int hive_users_to_contact = 0;
  foreach( NetworkAddress na, Hive::instance().networkAddresses() )
  {
    User u = UserManager::instance().findUserByNetworkAddress( na );
    if( u.isValid() && u.isStatusConnected() )
      continue;
    if( addNetworkAddress( na, false ) )
      hive_users_to_contact++;
  }
  return hive_users_to_contact;
}

QList<NetworkAddress> Broadcaster::updateAddressesToSearchUsers()
{
#ifdef BEEBEEP_DEBUG
  qDebug() << "Broadcaster updates the network addresses to search users";
#endif

  QList<NetworkAddress> network_address_list;
  m_networkAddressesWaitingForLoopback.clear();
  m_networkAddresses.clear();

  if( Settings::instance().useOnlyMulticast() )
  {
    if( m_multicastGroupAddress.isNull() )
      qWarning() << "Multicast only option enabled but the multicast group address is empty";
    else
      qDebug() << "Multicast only to" << qPrintable( m_multicastGroupAddress.toString() ) << "(option in RC enabled)";
    return network_address_list;
  }

  quint16 default_broadcast_port = static_cast<quint16>( Settings::instance().defaultBroadcastPort() );
  bool split_ipv4_addresses = false;
  foreach( QString s_address, Settings::instance().broadcastAddressesInFileHosts() )
  {
    NetworkAddress na = NetworkAddress::fromString( s_address );
    if( na.isHostAddressValid() )
    {
      if( !na.isHostPortValid() )
      {
        na.setHostPort( default_broadcast_port );
        split_ipv4_addresses = false;
      }
      else
        split_ipv4_addresses = na.isIPv4Address() && na.hostPort() != default_broadcast_port && na.hostAddress().toString().endsWith( "255" );
#ifdef BEEBEEP_DEBUG
      qDebug() << "Network address saved in file hosts added:" << qPrintable( na.toString() );
#endif
      addNetworkAddress( na, split_ipv4_addresses );
      network_address_list.append( na );
    }
    else
      qWarning() << "Broadcaster has found error in network address saved in file hosts:" << qPrintable( na.toString() );
  }

  if( Settings::instance().broadcastOnlyToHostsIni() )
  {
    if( m_networkAddresses.isEmpty() )
      qWarning() << "Broadcast only to hosts in INI file option enabled but the list is empty";

#ifdef BEEBEEP_DEBUG
    else
      qDebug() << "Broadcast only to hosts in INI file option enabled";
#endif
    return network_address_list;
  }

  if( !m_isMulticastDatagramSent || Settings::instance().broadcastToLocalSubnetAlways() )
  {
    addHostAddress( NetworkManager::instance().localBroadcastAddress() );
    m_isMulticastDatagramSent = false;
    network_address_list.append( NetworkAddress( NetworkManager::instance().localBroadcastAddress(), static_cast<quint16>( Settings::instance().defaultBroadcastPort() ) ) );
  }

  foreach( QHostAddress ha, NetworkManager::instance().localBroadcastAddresses() )
  {
    if( ha != NetworkManager::instance().localBroadcastAddress() )
    {
      addHostAddress( ha );
      network_address_list.append( NetworkAddress( ha, static_cast<quint16>( Settings::instance().defaultBroadcastPort() ) ) );
#ifdef BEEBEEP_DEBUG
      qDebug() << "Broadcaster adds this address from local network:" << qPrintable( ha.toString() );
#endif
    }
  }

  updateUsersAddedManually();
  int offline_users_to_add = 0;
  if( Settings::instance().useHive() )
    offline_users_to_add = updateUsersFromHive();

  if( Settings::instance().broadcastToOfflineUsers() )
  {
    foreach( User u, UserManager::instance().userList().toList() )
    {
      if( !u.isStatusConnected() && addNetworkAddress( u.networkAddress(), false ) )
      {
        network_address_list.append( u.networkAddress() );
        offline_users_to_add++;
      }
    }
  }

  qDebug() << "Broadcaster will contact" << network_address_list.size() << "network addresses with" << m_networkAddresses.size() << "hosts and with" << offline_users_to_add << "offline users";
  return network_address_list;
}

void Broadcaster::onTickEvent( int )
{
  if( !m_networkAddressesWaitingForLoopback.isEmpty() )
    checkLoopbackDatagram();

  if( mp_receiverSocket->state() != QAbstractSocket::BoundState )
  {
#ifdef BEEBEEP_DEBUG
    qWarning() << "Broadcaster has not the receiver socket in BoundState";
#endif
    return;
  }

  if( !NetworkManager::instance().isMainInterfaceUp() )
  {
#ifdef BEEBEEP_DEBUG
    qWarning() << "Broadcaster cannot contact other addresses because main network interface is down";
#endif
    return;
  }

  if( m_networkAddresses.isEmpty() )
  {
    if( m_newBroadcastRequested )
    {
      QList<NetworkAddress> network_addresses = updateAddressesToSearchUsers();
      if( !network_addresses.isEmpty() )
      {
        QStringList sl_log_network_addresses;
        foreach( NetworkAddress na, network_addresses )
          sl_log_network_addresses.append( na.toString() );
        qDebug() << "Searching users in these addresses:" << qPrintable( sl_log_network_addresses.join( ", " ) );
      }
      m_newBroadcastRequested = false;
    }
  }
  else
    contactNetworkAddresses();
}

void Broadcaster::sendMulticastDatagram()
{
  if( !m_isMulticastDatagramSent && !m_multicastGroupAddress.isNull() && !Settings::instance().disableMulticast() )
  {
    QByteArray broadcast_data = Protocol::instance().broadcastMessage( m_multicastGroupAddress );
    if( mp_senderSocket->writeDatagram( broadcast_data, m_multicastGroupAddress, static_cast<quint16>(Settings::instance().defaultBroadcastPort()) ) > 0 )
    {
      qDebug() << "Broadcaster sends multicast datagram to" << qPrintable( m_multicastGroupAddress.toString() ) << Settings::instance().defaultBroadcastPort();
      m_networkAddressesWaitingForLoopback.append( QPair<NetworkAddress, QDateTime>( NetworkAddress( m_multicastGroupAddress, static_cast<quint16>( Settings::instance().defaultBroadcastPort() ) ), QDateTime::currentDateTime() ) );
      m_lastDatagramSentTimestamp = QDateTime::currentDateTime();
      m_isMulticastDatagramSent = true;
#ifdef BEEBEEP_DEBUG
      qDebug() << "Waiting for loopback datagram from" << qPrintable( m_multicastGroupAddress.toString() );
#endif
    }
    else
      qWarning() << "Unable to send multicast datagram to" << qPrintable( m_multicastGroupAddress.toString() ) << Settings::instance().defaultBroadcastPort();
  }
}

void Broadcaster::contactNetworkAddresses()
{
  if( !sortNetworkAddresses() )
    return;

  bool can_contact_users = m_lastDatagramSentTimestamp.isNull() || qAbs( m_lastDatagramSentTimestamp.msecsTo( QDateTime::currentDateTime() ) ) > Settings::instance().delayContactUsers();
  if( !can_contact_users )
    return;

  int contacted_users = 0;
  while( !m_networkAddresses.isEmpty() )
  {
    NetworkAddress na = m_networkAddresses.takeFirst();
    if( na.isHostAddressValid() )
    {
      if( isNetworkAddressForBroadcast( na ) )
      {
         if( broadcastToNetworkAddress( na ) )
           return;
      }
      else
      {
#ifdef BEEBEEP_DEBUG
        qDebug() << "Broadcaster skips this network address" << qPrintable( na.toString() ) << "and sends it to the CORE";
#endif
        emit newPeerFound( na.hostAddress(), na.hostPort() );
        contacted_users++;
      }
    }

    if( contacted_users >= Settings::instance().maxUsersToConnectInATick() )
      break;
  }

#ifdef BEEBEEP_DEBUG
  qDebug() << "Broadcaster has contacted" << contacted_users << "network addresses";
#endif
}

bool Broadcaster::isNetworkAddressForBroadcast( const NetworkAddress& na ) const
{
  return (na.isIPv4Address() && na.hostAddress().toString().contains( "255" )) || na.hostPort() == Settings::instance().defaultBroadcastPort();
}

bool Broadcaster::sortNetworkAddresses()
{
  if( m_networkAddresses.isEmpty() )
    return false;
  if( m_networkAddressesIsSorted )
    return true;
  QList<NetworkAddress> broadcast_network_addresses;
  QList<NetworkAddress> direct_network_addresses;
  foreach( NetworkAddress na, m_networkAddresses )
  {
    if( isNetworkAddressForBroadcast( na ) )
      broadcast_network_addresses.append( na );
    else
      direct_network_addresses.append( na );
  }

  if( !broadcast_network_addresses.isEmpty() )
  {
    m_networkAddresses = broadcast_network_addresses;
    if( !direct_network_addresses.isEmpty() )
      m_networkAddresses.append( direct_network_addresses );
  }
  else
    m_networkAddresses = direct_network_addresses;
  m_networkAddressesIsSorted = true;
  return true;
}
