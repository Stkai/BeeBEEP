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
    m_networkAddressesWaitingForLoopback(), m_addOfflineUsersInNetworkAddresses( false ),
    m_multicastGroupAddress()
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

    m_multicastInterface = NetworkManager::instance().localNetworkInterface();
    bool multicast_group_joined = false;
    if( m_multicastInterface.isValid() )
       multicast_group_joined = mp_receiverSocket->joinMulticastGroup( m_multicastGroupAddress, m_multicastInterface );
     else
       multicast_group_joined = mp_receiverSocket->joinMulticastGroup( m_multicastGroupAddress );

    if( multicast_group_joined )
      qDebug() << "Join to the multicast group" << qPrintable( m_multicastGroupAddress.toString() );
    else
      qWarning() << "Unable to join to the multicast group" << qPrintable( m_multicastGroupAddress.toString() );
  }
#endif

  if( !m_networkAddresses.isEmpty() )
    m_networkAddresses.clear();
  if( !m_networkAddressesWaitingForLoopback.isEmpty() )
    m_networkAddressesWaitingForLoopback.clear();
  m_addOfflineUsersInNetworkAddresses = false;
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
      bool multicast_group_left = false;
      if( m_multicastInterface.isValid() )
        multicast_group_left = mp_receiverSocket->leaveMulticastGroup( m_multicastGroupAddress, m_multicastInterface );
      else
        multicast_group_left = mp_receiverSocket->leaveMulticastGroup( m_multicastGroupAddress );
      if( multicast_group_left )
        qDebug() << "Leave from the multicast group" << qPrintable( m_multicastGroupAddress.toString() );
      else
        qWarning() << "Unable to leave from the multicast group" << qPrintable( m_multicastGroupAddress.toString() );
    }
#endif
    mp_receiverSocket->close();
  }

  qDebug() << "Broadcaster stops broadcasting";

  if( !m_networkAddresses.isEmpty() )
    m_networkAddresses.clear();
  if( !m_networkAddressesWaitingForLoopback.isEmpty() )
    m_networkAddressesWaitingForLoopback.clear();
  m_newBroadcastRequested = false;
  m_addOfflineUsersInNetworkAddresses = false;
}

void Broadcaster::sendBroadcast()
{
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

  if( na.isHostPortValid() )
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
    Message m = Protocol::instance().toMessage( datagram, 1 );
    if( !m.isValid() || m.type() != Message::Beep )
    {
      qWarning() << "Broadcaster has received an invalid data:" << datagram;
      continue;
    }

    QHostAddress sender_host_address = Protocol::instance().hostAddressFromBroadcastMessage( m );
    if( sender_host_address.isNull() )
      qWarning() << "Broadcaster has received and invalid host address in data:" << datagram;
    else
      removeHostAddressFromWaitingList( sender_host_address );
    if( !sender_ip.isNull() )
      removeHostAddressFromWaitingList( sender_ip );

    bool ok = false;
    int sender_listener_port = m.text().toInt( &ok );
    if( !ok )
    {
      qWarning() << "Broadcaster has received an invalid listener port from datagram:" << m.text();
      continue;
    }

    if( sender_listener_port == Settings::instance().localUser().networkAddress().hostPort() && NetworkManager::instance().isLocalHostAddress( sender_ip ) )
    {
#ifdef BEEBEEP_DEBUG
      qDebug() << "Broadcaster has received LOCAL datagram from ip address:" << qPrintable( Protocol::instance().hostAddressFromBroadcastMessage( m ).toString() );
#endif
      continue;
    }

    if( sender_host_address.isNull() )
      qDebug() << "Broadcaster has found new peer" << qPrintable( sender_ip.toString() ) << sender_listener_port;
    else
      qDebug() << "Broadcaster has found new peer" << qPrintable( sender_ip.toString() ) << sender_listener_port << "with datagram from" << qPrintable( sender_host_address.toString() );
    emit newPeerFoundFromDatagram( sender_host_address, sender_ip, sender_listener_port );
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

void Broadcaster::updateAddresses()
{
#ifdef BEEBEEP_DEBUG
  qDebug() << "Broadcaster updates the network addresses to search users";
#endif

  m_networkAddressesWaitingForLoopback.clear();
  m_networkAddresses.clear();

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

  addHostAddress( NetworkManager::instance().localBroadcastAddress() );

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

  if( m_addOfflineUsersInNetworkAddresses )
  {
    int offline_users_to_add = 0;

    if( Hive::instance().networkAddresses().isEmpty() )
    {
      foreach( User u, UserManager::instance().userList().toList() )
      {
        if( !u.isStatusConnected() && addNetworkAddress( u.networkAddress(), false ) )
          offline_users_to_add++;
      }
    }
    else
    {
      foreach( NetworkAddress na, Hive::instance().networkAddresses() )
      {
        if( addNetworkAddress( na, false ) )
          offline_users_to_add++;
      }
    }

#ifdef BEEBEEP_DEBUG
    qDebug() << "Broadcaster adds" << offline_users_to_add << "offline users to network addresses";
#endif
  }

#ifdef BEEBEEP_DEBUG
  QStringList sl;
  foreach( NetworkAddress na, m_networkAddresses )
    sl << na.toString();
  qDebug() << "Broadcaster is contacting the followings addresses:" << qPrintable( sl.join( ", " ) );
#endif
  qDebug() << "Broadcaster will contact" << m_networkAddresses.size() << "network addresses";

}

void Broadcaster::onTickEvent( int )
{
  if( !m_networkAddressesWaitingForLoopback.isEmpty() )
  {
    checkLoopbackDatagram();
  }

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
      if( Settings::instance().broadcastToOfflineUsers() )
        setAddOfflineUsersInNetworkAddresses( true );
      updateAddresses();
      if( Settings::instance().broadcastToOfflineUsers() )
        setAddOfflineUsersInNetworkAddresses( false );
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
      qDebug() << "Broadcaster sends multicast datagram to" << qPrintable( m_multicastGroupAddress.toString() ) << Settings::instance().defaultBroadcastPort();
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
