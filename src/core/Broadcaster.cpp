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

#include "Broadcaster.h"
#include "Hive.h"
#include "NetworkManager.h"
#include "Protocol.h"
#include "Settings.h"
#include "UserManager.h"


Broadcaster::Broadcaster( QObject *parent )
  : QObject( parent ), m_networkAddresses(), m_newBroadcastRequested( false ),
    m_networkAddressesWaitingForLoopback(), m_multicastGroupAddress(), m_isMulticastDatagramSent( false )
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
            if( address_entry.ip().isLoopback() || address_entry.ip().toString() == QString( "127.0.0.1" ) || address_entry.ip().toString() == QString( "::1" ) )
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
          else
            qWarning() << "Broadcaster cannot join to the multicast group" << qPrintable( m_multicastGroupAddress.toString() )
                       << "in network interface" << qPrintable( hardware_address );
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
            else
              qWarning() << "Unable to join to the multicast group" << qPrintable( m_multicastGroupAddress.toString() ) << "in network interface" << qPrintable( hardware_address );
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

  return true;
}

bool Broadcaster::contactNetworkAddress( const NetworkAddress& na )
{
  if( !na.isHostAddressValid() )
    return false;

  if( na.isHostPortValid() && na.hostPort() != Settings::instance().defaultBroadcastPort() )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Broadcaster skips this network address" << qPrintable( na.toString() ) << "and sends it to the CORE";
#endif
    emit newPeerFound( na.hostAddress(), na.hostPort() );
    return true;
  }
  else
  {
    QByteArray broadcast_data = Protocol::instance().broadcastMessage( na.hostAddress() );
    if( mp_senderSocket->writeDatagram( broadcast_data, na.hostAddress(), static_cast<quint16>(Settings::instance().defaultBroadcastPort()) ) > 0 )
    {
      qDebug() << "Broadcaster sends datagram to" << qPrintable( na.hostAddress().toString() ) << Settings::instance().defaultBroadcastPort();
      if( na.hostAddress() == NetworkManager::instance().localBroadcastAddress() )
      {
        m_networkAddressesWaitingForLoopback.append( QPair<NetworkAddress,QDateTime>( na, QDateTime::currentDateTime() ) );
        qDebug() << "Waiting for loopback datagram from" << qPrintable( na.hostAddress().toString() );
      }
      return true;
    }
    else
    {
      qWarning() << "Unable to send datagram to" << qPrintable( na.hostAddress().toString() ) << Settings::instance().defaultBroadcastPort();
      return false;
    }
  }
}

void Broadcaster::checkLoopbackDatagram()
{
#ifdef BEEBEEP_DEBUG
  qDebug() << "Check" << m_networkAddressesWaitingForLoopback.size() << "loopback datagram";
#endif
  QList< QPair<NetworkAddress,QDateTime> >::iterator it = m_networkAddressesWaitingForLoopback.begin();
  while( it != m_networkAddressesWaitingForLoopback.end() )
  {
#if QT_VERSION >= 0x040700
    if( it->second.msecsTo( QDateTime::currentDateTime() ) > 5600 )
#else
    if( it->second.secsTo( QDateTime::currentDateTime() ) > 5 )
#endif
    {
      qWarning() << "Broadcaster didn't received yet a loopback datagram from host:" << qPrintable( it->first.toString() );
      it = m_networkAddressesWaitingForLoopback.erase( it );
    }
    else
      ++it;
  }
}

void Broadcaster::removeHostAddressFromWaitingList( const QHostAddress& host_address )
{
  QList< QPair<NetworkAddress,QDateTime> >::iterator it = m_networkAddressesWaitingForLoopback.begin();
  while( it != m_networkAddressesWaitingForLoopback.end() )
  {
    if( it->first.hostAddress() == host_address )
    {
      qDebug() << "Broadcaster has received loopback datagram from host:" << qPrintable( it->first.toString() );
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
      qWarning() << "Broadcaster has received an invalid data:" << datagram;
      continue;
    }

    bool ok = false;
    int sender_listener_port = m.text().toInt( &ok );
    if( !ok )
    {
      qWarning() << "Broadcaster has received an invalid listener port from datagram:" << m.text();
      continue;
    }

    removeHostAddressFromWaitingList( sender_ip );
    // host_address_from_datagram it the host target of the message (so for multicast is 239.255.64.75,
    // for broadcast 192.168.1.255, 10.0.0.255, ...)
    QHostAddress host_address_from_datagram = Protocol::instance().hostAddressFromBroadcastMessage( m );
    if( host_address_from_datagram.isNull() )
      qWarning() << "Broadcaster has received and invalid host address in data:" << datagram;
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

void Broadcaster::updateAddresses()
{
#ifdef BEEBEEP_DEBUG
  qDebug() << "Broadcaster updates the network addresses to search users";
#endif

  m_networkAddressesWaitingForLoopback.clear();
  m_networkAddresses.clear();

  if( Settings::instance().useOnlyMulticast() )
  {
    if( m_multicastGroupAddress.isNull() )
      qWarning() << "Multicast only option enabled but the multicast group address is empty";
#ifdef BEEBEEP_DEBUG
    else
      qDebug() << "Multicast only to" << qPrintable( m_multicastGroupAddress.toString() ) << "(option in RC enabled)";
#endif
    return;
  }

  foreach( QString s_address, Settings::instance().broadcastAddressesInFileHosts() )
  {
    NetworkAddress na = NetworkAddress::fromString( s_address );
    if( na.isHostAddressValid() )
    {
#ifdef BEEBEEP_DEBUG
      qDebug() << "Network address saved in file hosts added:" << qPrintable( na.toString() );
#endif
      addNetworkAddress( na, true );
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
    return;
  }

  if( !m_isMulticastDatagramSent || Settings::instance().broadcastToLocalSubnetAlways() )
  {
    addHostAddress( NetworkManager::instance().localBroadcastAddress() );
    m_isMulticastDatagramSent = false;
  }

  foreach( QHostAddress ha, NetworkManager::instance().localBroadcastAddresses() )
  {
    if( ha != NetworkManager::instance().localBroadcastAddress() )
    {
      addHostAddress( ha );
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
        offline_users_to_add++;
    }
  }

#ifdef BEEBEEP_DEBUG
  qDebug() << "Broadcaster adds" << offline_users_to_add << "offline users to network addresses";
#endif

#ifdef BEEBEEP_DEBUG
  QStringList sl;
  foreach( NetworkAddress na, m_networkAddresses )
    sl << na.toString();
  qDebug() << "Broadcaster is contacting the followings addresses:" << qPrintable( sl.join( ", " ) );
#endif
  qDebug() << "Broadcaster will contact" << m_networkAddresses.size() << "network addresses and" << offline_users_to_add << "offline users";

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
      updateAddresses();
      m_newBroadcastRequested = false;
    }
  }
  else
    contactNetworkAddresses();
}

void Broadcaster::sendMulticastDatagram()
{
  if( !m_multicastGroupAddress.isNull() )
  {
    QByteArray broadcast_data = Protocol::instance().broadcastMessage( m_multicastGroupAddress );
    if( mp_senderSocket->writeDatagram( broadcast_data, m_multicastGroupAddress, static_cast<quint16>(Settings::instance().defaultBroadcastPort()) ) > 0 )
    {
      qDebug() << "Broadcaster sends multicast datagram to" << qPrintable( m_multicastGroupAddress.toString() ) << Settings::instance().defaultBroadcastPort();
      m_isMulticastDatagramSent = true;
    }
    else
      qWarning() << "Unable to send multicast datagram to" << qPrintable( m_multicastGroupAddress.toString() ) << Settings::instance().defaultBroadcastPort();
  }
}

void Broadcaster::contactNetworkAddresses()
{
  int contacted_users = 0;
  while( !m_networkAddresses.isEmpty() )
  {
    NetworkAddress na = m_networkAddresses.takeFirst();
    if( contactNetworkAddress( na ) )
      contacted_users++;

    if( contacted_users >= Settings::instance().maxUsersToConnectInATick() )
      break;
  }
  qDebug() << "Broadcaster has contacted" << contacted_users << "network addresses";
}
