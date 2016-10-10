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
  : QObject( parent ), m_broadcastSocket(), m_networkAddresses(), m_newBroadcastRequested( false ), m_verbose( false )
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

  m_networkAddresses.clear();
  m_broadcastSocket.close();
  m_newBroadcastRequested = false;
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

  if( network_address.isHostPortValid() && split_ipv4_address )
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
    emit newPeerFound( na.hostAddress(), na.hostPort() );
    return true;
  }
  else
  {
    QByteArray broadcast_data = Protocol::instance().broadcastMessage( na.hostAddress() );
    return m_broadcastSocket.writeDatagram( broadcast_data, na.hostAddress(), Settings::instance().defaultBroadcastPort() ) > 0;
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
    Message m = Protocol::instance().toMessage( datagram );
    if( !m.isValid() || m.type() != Message::Beep )
    {
      qWarning() << "Broadcaster has received an invalid data:" << datagram;
      continue;
    }
    bool ok = false;
    int sender_listener_port = m.text().toInt( &ok );
    if( !ok )
    {
      qWarning() << "Broadcaster has received an invalid listener port" << datagram;
      continue;
    }

    if( sender_listener_port == Settings::instance().localUser().hostPort() && NetworkManager::instance().isLocalHostAddress( sender_ip ) )
    {
#ifdef BEEBEEP_DEBUG
      qDebug() << "Broadcaster has received LOCAL datagram from ip address:" << qPrintable( Protocol::instance().hostAddressFromBroadcastMessage( m ).toString() );
#endif
      continue;
    }

#ifdef BEEBEEP_DEBUG
    qDebug() << "Broadcaster has found new peer" << sender_ip.toString() << sender_listener_port;
#endif
    emit newPeerFound( sender_ip, sender_listener_port );
  }

  if( num_datagram_read > 1 )
    qDebug() << "Broadcaster read" << num_datagram_read << "datagrams";
}

void Broadcaster::updateAddresses()
{
#ifdef BEEBEEP_DEBUG
  qDebug() << "Broadcaster updates the network addresses to search users";
#endif

  m_networkAddresses.clear();
  m_newBroadcastRequested = false;

  foreach( QString s_address, Settings::instance().broadcastAddressesInFileHosts() )
  {
    NetworkAddress na = NetworkAddress::fromString( s_address );
    if( na.isHostAddressValid() )
    {
#ifdef BEEBEEP_DEBUG
      qDebug() << "Network address saved in file hosts added:" << na.toString();
#endif
      addNetworkAddress( na, true );
    }
    else
      qWarning() << "Broadcaster has found error in network address saved in file hosts:" << na.toString();
  }

  if( Settings::instance().broadcastOnlyToHostsIni() )
  {
    if( m_networkAddresses.isEmpty() )
      qWarning() << "Broadcast only to hosts in INI file option enabled but the list is empty";

#ifdef BEEBEEP_DEBUG
    qDebug() << "Broadcast only to hosts in INI file option enabled";
#endif
    return;
  }

  addHostAddress( NetworkManager::instance().localBroadcastAddress() );
  if( m_verbose )
    addNetworkAddress( NetworkAddress( NetworkManager::instance().localBroadcastAddress(), Settings::instance().defaultListenerPort() ), true );

  foreach( QHostAddress ha, NetworkManager::instance().localBroadcastAddresses() )
  {
    if( ha == NetworkManager::instance().localBroadcastAddress() )
      continue;
    addHostAddress( ha );
    if( m_verbose )
      addNetworkAddress( NetworkAddress( ha, Settings::instance().defaultListenerPort() ), true );
  }

  foreach( User u, UserManager::instance().userList().toList() )
  {
    if( !u.isStatusConnected() )
      addNetworkAddress( NetworkAddress( u.hostAddress(), u.hostPort() ), false );
  }

  foreach( QString s_address, Settings::instance().broadcastAddressesInSettings() )
  {
    NetworkAddress na = NetworkAddress::fromString( s_address );
    if( na.isHostAddressValid() )
    {
#ifdef BEEBEEP_DEBUG
      qDebug() << "Network address saved in file settings added:" << na.toString();
#endif
      addNetworkAddress( na, true );
    }
    else
      qWarning() << "Broadcaster has found error in network address saved in file settings:" << na.toString();
  }

  if( !Settings::instance().userPathList().isEmpty() )
  {
    foreach( QString user_path, Settings::instance().userPathList() )
    {
      UserRecord ur = Protocol::instance().loadUserRecord( user_path );
      if( ur.isValid() )
      {
        NetworkAddress na( ur.hostAddress(), ur.hostPort() );
#ifdef BEEBEEP_DEBUG
        qDebug() << "Network address saved in user path list parsed:" << na.toString();
#endif
        addNetworkAddress( na, false );
      }
      else
        qWarning() << "Broadcaster has found error in user record saved in file settings:" << user_path;
    }
  }

  foreach( NetworkAddress na, Hive::instance().networkAddresses() )
    addNetworkAddress( na, false );

#ifdef BEEBEEP_DEBUG
  QStringList sl;
  foreach( NetworkAddress na, m_networkAddresses )
    sl << na.toString();
  qDebug() << "Broadcaster is contacting the followings addresses:" << qPrintable( sl.join( ", " ) );
#endif
}

void Broadcaster::onTickEvent( int )
{
  if( m_networkAddresses.isEmpty() )
  {
    if( m_newBroadcastRequested )
      updateAddresses();
    return;
  }

  if( m_broadcastSocket.state() != QAbstractSocket::BoundState )
    return;

  for( int i = 0; i < Settings::instance().maxUsersToConnectInATick(); i++ )
  {
    if( m_networkAddresses.isEmpty() )
      break;
    NetworkAddress na = m_networkAddresses.takeFirst();
    contactNetworkAddress( na );
  }
}
