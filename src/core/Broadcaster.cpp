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
  : QObject( parent ), m_broadcastSocket(), m_networkAddresses(), m_newBroadcastRequested( false ),
    m_networkAddressesWaitingForLoopback(), m_addOfflineUsersInNetworkAddresses( false )
{
  connect( &m_broadcastSocket, SIGNAL( readyRead() ), this, SLOT( readBroadcastDatagram() ) );
}

bool Broadcaster::startBroadcastServer()
{
  if( !m_broadcastSocket.bind( Settings::instance().hostAddressToListen(), Settings::instance().defaultBroadcastPort(), QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint ) )
  {
    qWarning() << "Broadcaster cannot bind the broadcast port" << Settings::instance().defaultBroadcastPort();
    return false;
  }

#if QT_VERSION >= 0x040800
  if( !Settings::instance().multicastGroupAddress().isNull() )
  {
    if( m_broadcastSocket.joinMulticastGroup( Settings::instance().multicastGroupAddress() ) )
      qDebug() << "Join to the multicast group" << qPrintable( Settings::instance().multicastGroupAddress().toString() );
    else
      qWarning() << "Unable to join to the multicast group" << qPrintable( Settings::instance().multicastGroupAddress().toString() );
  }
#endif

  if( !m_networkAddresses.isEmpty() )
    m_networkAddresses.clear();
  if( !m_networkAddressesWaitingForLoopback.isEmpty() )
    m_networkAddressesWaitingForLoopback.clear();
  m_newBroadcastRequested = false;
  m_addOfflineUsersInNetworkAddresses = false;

  return true;
}

void Broadcaster::stopBroadcasting()
{
#if QT_VERSION >= 0x040800
  if( !Settings::instance().multicastGroupAddress().isNull() )
  {
    if( m_broadcastSocket.leaveMulticastGroup( Settings::instance().multicastGroupAddress() ) )
      qDebug() << "Leave from the multicast group" << qPrintable( Settings::instance().multicastGroupAddress().toString() );
    else
      qWarning() << "Unable to leave from the multicast group" << qPrintable( Settings::instance().multicastGroupAddress().toString() );
  }
#endif

  qDebug() << "Broadcaster stops broadcasting";

  if( !m_networkAddresses.isEmpty() )
    m_networkAddresses.clear();
  if( !m_networkAddressesWaitingForLoopback.isEmpty() )
    m_networkAddressesWaitingForLoopback.clear();
  m_broadcastSocket.close();
  m_newBroadcastRequested = false;
  m_addOfflineUsersInNetworkAddresses = false;
}

void Broadcaster::sendBroadcast()
{
  if( m_broadcastSocket.state() != QAbstractSocket::BoundState )
  {
#ifdef BEEBEEP_DEBUG
    qWarning() << "Unable to send broadcast with a closed socket";
#endif
    return;
  }

  if( m_networkAddresses.isEmpty() )
    updateAddresses();
  else
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
    qDebug() << "Broadcaster sends to core this network address:" << qPrintable( na.toString() );
#endif
    emit newPeerFound( na.hostAddress(), na.hostPort() );
    return true;
  }
  else
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Broadcaster sends datagram this network address:" << qPrintable( na.hostAddress().toString() ) << Settings::instance().defaultBroadcastPort();
#endif
    QByteArray broadcast_data = Protocol::instance().broadcastMessage( na.hostAddress() );
    m_networkAddressesWaitingForLoopback.append( QPair<NetworkAddress,QDateTime>( na, QDateTime::currentDateTime() ) );
    return m_broadcastSocket.writeDatagram( broadcast_data, na.hostAddress(), Settings::instance().defaultBroadcastPort() ) > 0;
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
#ifdef BEEBEEP_DEBUG
      qDebug() << "Broadcaster has received loopback datagram from host:" << qPrintable( it->first.toString() );
#endif
      it = m_networkAddressesWaitingForLoopback.erase( it );
    }
    else
      ++it;
  }
}

void Broadcaster::readBroadcastDatagram()
{
  int num_datagram_read = 0;
  while( m_broadcastSocket.hasPendingDatagrams() && num_datagram_read < MAX_NUM_OF_LOOP_IN_CONNECTON_SOCKECT )
  {
    num_datagram_read++;
    QHostAddress sender_ip;
    quint16 sender_port;
    QByteArray datagram;
    datagram.resize( m_broadcastSocket.pendingDatagramSize() );
    if( m_broadcastSocket.readDatagram( datagram.data(), datagram.size(), &sender_ip, &sender_port ) == -1 )
      continue;
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

    bool ok = false;
    int sender_listener_port = m.text().toInt( &ok );
    if( !ok )
    {
      qWarning() << "Broadcaster has received an invalid listener port" << datagram;
      continue;
    }

    if( sender_listener_port == Settings::instance().localUser().networkAddress().hostPort() && NetworkManager::instance().isLocalHostAddress( sender_ip ) )
    {
#ifdef BEEBEEP_DEBUG
      qDebug() << "Broadcaster has received LOCAL datagram from ip address:" << qPrintable( Protocol::instance().hostAddressFromBroadcastMessage( m ).toString() );
#endif
      continue;
    }

#ifdef BEEBEEP_DEBUG
    qDebug() << "Broadcaster has found new peer" << qPrintable( sender_ip.toString() ) << sender_listener_port;
#endif
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

void Broadcaster::updateAddresses()
{
#ifdef BEEBEEP_DEBUG
  qDebug() << "Broadcaster updates the network addresses to search users";
#endif

  m_networkAddressesWaitingForLoopback.clear();
  m_networkAddresses.clear();
  m_newBroadcastRequested = false;

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
#else
  qDebug() << "Broadcaster will contact" << m_networkAddresses.size() << "network addresses";
#endif
}

void Broadcaster::onTickEvent( int )
{
  if( !m_networkAddressesWaitingForLoopback.isEmpty() )
  {
    checkLoopbackDatagram();
  }

  if( m_networkAddresses.isEmpty() )
  {
    if( m_newBroadcastRequested )
    {
      setAddOfflineUsersInNetworkAddresses( true );
      updateAddresses();
      setAddOfflineUsersInNetworkAddresses( false );
    }
    return;
  }

  if( m_broadcastSocket.state() != QAbstractSocket::BoundState )
  {
#ifdef BEEBEEP_DEBUG
    qWarning() << "Broadcaster has not the socket in BoundState and cannot contact other addresses";
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

  contactNetworkAddresses();
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
#ifdef BEEBEEP_DEBUG
  qDebug() << "Broadcaster has contacted" << contacted_users << "network addresses";
#endif
}
