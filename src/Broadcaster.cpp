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
#include "NetworkManager.h"
#include "Protocol.h"
#include "Settings.h"


Broadcaster::Broadcaster( QObject *parent )
  : QObject( parent ), m_broadcastData()
{
  m_broadcastTimer.setSingleShot( false );
  m_datagramSentToBaseBroadcastAddress = 0;

  connect( &m_broadcastSocket, SIGNAL( readyRead() ), this, SLOT( readBroadcastDatagram() ) );
  connect( &m_broadcastTimer, SIGNAL( timeout() ), this, SLOT( sendBroadcastDatagram() ) );
}

bool Broadcaster::startBroadcasting()
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

  m_baseBroadcastAddress = NetworkManager::instance().localBroadcastAddress();
  updateAddresses();
#ifdef BEEBEEP_DEBUG
  qDebug() << "Broadcaster generates broadcast message data";
#endif
  m_broadcastData = Protocol::instance().broadcastMessage();

  if( Settings::instance().broadcastInterval() > 0 )
  {
    m_broadcastTimer.setInterval( Settings::instance().broadcastInterval() < 5000 ? 5000 : Settings::instance().broadcastInterval() );
    m_broadcastTimer.start();
  }

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
  m_datagramSentToBaseBroadcastAddress = 0;
  if( m_broadcastTimer.isActive() )
    m_broadcastTimer.stop();
  m_broadcastSocket.close();
}

void Broadcaster::sendBroadcastDatagram()
{
  int addresses_contacted = 0;

#ifdef BEEBEEP_DEBUG
  QStringList sl_host;
  foreach( QHostAddress ha, m_broadcastAddresses )
    sl_host << ha.toString();
  qDebug() << "Sending datagram to hosts:" << qPrintable( sl_host.join( ", " ) );
#endif

  QList<QHostAddress>::iterator it = m_broadcastAddresses.begin();
  while( it != m_broadcastAddresses.end() )
  {
    if( !sendDatagramToHost( *it ) )
    {
      it = m_broadcastAddresses.erase( it );
    }
    else
    {
      addresses_contacted++;
      ++it;
    }
  }

  if( !m_baseBroadcastAddress.isNull() && sendDatagramToHost( m_baseBroadcastAddress ) )
  {
    qDebug() << "Broadcaster has contacted default network:" << qPrintable( m_baseBroadcastAddress.toString() );
    addresses_contacted++;
    m_datagramSentToBaseBroadcastAddress++;
    QTimer::singleShot( Settings::instance().broadcastLoopbackInterval(), this, SLOT( checkLoopback() ) );
  }

  qDebug() << "Broadcaster has contacted" << addresses_contacted << "networks";

  QTimer::singleShot( 0, this, SLOT( searchInPeerAddresses() ) );
}

bool Broadcaster::isLocalHostAddress( const QHostAddress& address_to_check )
{
  foreach( QHostAddress local_address, m_ipAddresses )
  {
    if( address_to_check == local_address )
      return true;
  }
  return false;
}

bool Broadcaster::sendDatagramToHost( const QHostAddress& host_address )
{
  if( m_broadcastSocket.writeDatagram( m_broadcastData, host_address, Settings::instance().defaultBroadcastPort() ) > 0 )
  {
    return true;
  }
  else
  {
    qWarning() << "Broadcaster does not reach the network:" << host_address.toString();
    return false;
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

    if( isLocalHostAddress( sender_ip ) && sender_listener_port == Settings::instance().localUser().hostPort() )
    {
      if( m_datagramSentToBaseBroadcastAddress > 0 )
        m_datagramSentToBaseBroadcastAddress--;
      qDebug() << "Broadcaster skip datagram received from himself:" << m_datagramSentToBaseBroadcastAddress << "pendings";
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

int Broadcaster::updateAddresses()
{
#ifdef BEEBEEP_DEBUG
  qDebug() << "Broadcaster updates the addresses";
#endif
  m_broadcastAddresses.clear();
  m_ipAddresses.clear();
  QHostAddress ha_broadcast;

  if( !Settings::instance().broadcastAddressesInFileHosts().isEmpty() )
  {
    foreach( QString s_address, Settings::instance().broadcastAddressesInFileHosts() )
    {
      NetworkAddress na = NetworkAddress::fromString( s_address );
      if( na.isHostAddressValid() )
      {
#ifdef BEEBEEP_DEBUG
        qDebug() << "Network address in hosts parsed:" << na.toString();
#endif
        if( na.hostPort() > 0 )
          addPeerAddress( na );
        else
          addAddressToList( na.hostAddress() );
      }
    }
  }

  if( !Settings::instance().broadcastOnlyToHostsIni() && !Settings::instance().broadcastAddressesInSettings().isEmpty() )
  {
    foreach( QString s_address, Settings::instance().broadcastAddressesInSettings() )
    {
      ha_broadcast = QHostAddress( s_address );
      if( !ha_broadcast.isNull() )
        addAddressToList( ha_broadcast );
    }
  }

  if( !Settings::instance().broadcastOnlyToHostsIni() )
  {
    QList<NetworkEntry> available_broadcast_entries = NetworkManager::instance().availableNetworkEntries();

    foreach( NetworkEntry available_broadcast_entry, available_broadcast_entries )
    {
      if( !available_broadcast_entry.broadcast().isNull() )
        addAddressToList( available_broadcast_entry.broadcast() );

      if( !m_ipAddresses.contains( available_broadcast_entry.hostAddress() ) )
      {
        m_ipAddresses.append( available_broadcast_entry.hostAddress() );
        qDebug() << "Broadcaster adds" << qPrintable( available_broadcast_entry.hostAddress().toString() ) << "to local IP list";
      }
    }
  }

  return m_broadcastAddresses.size();
}

bool Broadcaster::addAddressToList( const QHostAddress& host_address )
{
  if( m_broadcastAddresses.contains( host_address ) )
    return false;

  if( host_address == m_baseBroadcastAddress )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Broadcaster skips base address:" << qPrintable( host_address.toString() );
#endif
    return false;
  }

  if( !Settings::instance().parseBroadcastAddresses() )
  {
    qDebug() << "Broadcaster add network" << qPrintable( host_address.toString() );
    m_broadcastAddresses.append( host_address );
    return true;
  }

  QList<QHostAddress> host_address_list = NetworkManager::instance().splitBroadcastSubnetToIPv4HostAddresses( host_address );
  if( host_address_list.isEmpty() )
    return false;

  foreach( QHostAddress ha, host_address_list )
  {
    if( !m_broadcastAddresses.contains( ha ) )
      m_broadcastAddresses.append( ha );
  }

  qDebug() << "Broadcaster adds network" << qPrintable( host_address.toString() ) << "with" << host_address_list.size() << "addresses";

  return true;
}

void Broadcaster::checkLoopback()
{
  if( m_datagramSentToBaseBroadcastAddress > 0 )
  {
    m_datagramSentToBaseBroadcastAddress--;
    qWarning() << "Broadcaster UDP port" <<  Settings::instance().defaultBroadcastPort() << "is blocked by firewall." << m_datagramSentToBaseBroadcastAddress << "datagram pendings";
    emit udpPortBlocked();
  }
}

void Broadcaster::addPeerAddress( const NetworkAddress& peer_address )
{
  if( !m_peerAddresses.contains( peer_address ) )
    m_peerAddresses.append( peer_address );
}

void Broadcaster::searchInPeerAddresses()
{
  if( m_peerAddresses.isEmpty() )
    return;

  qDebug() << "Broadcaster is also searching in" << m_peerAddresses.size() << "peer addresses";

  foreach( NetworkAddress na, m_peerAddresses )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Try with network address:" << qPrintable( na.toString() );
#endif
    emit newPeerFound( na.hostAddress(), na.hostPort() );
  }
}
