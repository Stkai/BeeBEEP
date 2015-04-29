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
  if( !m_broadcastSocket.bind( QHostAddress::Any, Settings::instance().broadcastPort(), QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint ) )
  {
    qWarning() << "Broadcaster cannot bind the broadcast port" << Settings::instance().broadcastPort();
    return false;
  }
  m_baseBroadcastAddress = Settings::instance().baseBroadcastAddress();
  updateAddresses();
  qDebug() << "Broadcaster generates broadcast message data";
  m_broadcastData = Protocol::instance().broadcastMessage();
  qDebug() << "Broadcaster starts broadcasting with tcp listener port" << Settings::instance().localUser().hostPort() << "and udp port" << Settings::instance().broadcastPort();
  QTimer::singleShot( 1000, this, SLOT( sendBroadcastDatagram() ) ); // first broadcast now!

  if( Settings::instance().broadcastInterval() > 0 )
  {
    m_broadcastTimer.setInterval( Settings::instance().broadcastInterval() < 5000 ? 5000 : Settings::instance().broadcastInterval() );
    m_broadcastTimer.start();
  }

  return true;
}

void Broadcaster::stopBroadcasting()
{
  qDebug() << "Broadcaster stops broadcasting";
  m_datagramSentToBaseBroadcastAddress = 0;
  if( m_broadcastTimer.isActive() )
    m_broadcastTimer.stop();
  m_broadcastSocket.close();
}

void Broadcaster::sendBroadcastDatagram()
{
  int addresses_contacted = 0;

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
    qDebug() << "Broadcaster has contacted default network:" << m_baseBroadcastAddress.toString();
    addresses_contacted++;
    m_datagramSentToBaseBroadcastAddress++;
    QTimer::singleShot( Settings::instance().broadcastLoopbackInterval(), this, SLOT( checkLoopback() ) );
  }

  qDebug() << "Broadcaster has contacted" << addresses_contacted << "networks";
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
  if( m_broadcastSocket.writeDatagram( m_broadcastData, host_address, Settings::instance().broadcastPort() ) > 0 )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Broadcaster has sent datagram to network:" << host_address.toString();
#endif
    return true;
  }
  else
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Broadcaster does not reach the network:" << host_address.toString();
#endif
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
      m_datagramSentToBaseBroadcastAddress--;
      qDebug() << "Broadcaster skip datagram received from himself:" << m_datagramSentToBaseBroadcastAddress << "pendings";
      continue;
    }

    emit newPeerFound( sender_ip, sender_listener_port );
  }

  if( num_datagram_read > 1 )
    qDebug() << "Broadcaster read" << num_datagram_read << "datagrams";
}

int Broadcaster::updateAddresses()
{
  qDebug() << "Broadcaster updates the addresses";
  m_broadcastAddresses.clear();
  m_ipAddresses.clear();
  QHostAddress ha_broadcast;

  if( !Settings::instance().broadcastAddressesInFileHosts().isEmpty() )
  {
    foreach( QString s_address, Settings::instance().broadcastAddressesInFileHosts() )
    {
      ha_broadcast = QHostAddress( s_address );
      if( !ha_broadcast.isNull() )
        addAddressToList( ha_broadcast );
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

  foreach( QNetworkInterface interface, QNetworkInterface::allInterfaces() )
  {
    foreach( QNetworkAddressEntry entry, interface.addressEntries() )
    {
      ha_broadcast = entry.broadcast();
      if( ha_broadcast != QHostAddress::Null && entry.ip() != QHostAddress::LocalHost )
      {
        if( entry.ip() == Settings::instance().localUser().hostAddress() )
          addAddressToList( ha_broadcast );
        else if( !Settings::instance().broadcastOnlyToHostsIni() )
          addAddressToList( ha_broadcast );
        else
          qDebug() << "Broadcaster skips" << ha_broadcast.toString();
        m_ipAddresses << entry.ip();
        qDebug() << "Broadcaster adds" << entry.ip().toString() << "to local IP list";
      }
    }
  }

  return m_broadcastAddresses.size();
}

bool Broadcaster::addAddressToList( const QHostAddress& host_address )
{
  if( m_broadcastAddresses.contains( host_address ) )
    return false;

  QList<QHostAddress> host_address_list = parseHostAddress( host_address );
  if( host_address_list.isEmpty() )
    return false;

  foreach( QHostAddress ha, host_address_list )
    m_broadcastAddresses << ha;

  qDebug() << "Broadcaster adds network" << host_address.toString() << "with" << host_address_list.size() << "addresses";

  return true;
}

void Broadcaster::checkLoopback()
{
  if( m_datagramSentToBaseBroadcastAddress > 0 )
  {
    m_datagramSentToBaseBroadcastAddress--;
    qWarning() << "Broadcaster UDP port" <<  Settings::instance().broadcastPort() << "is blocked by firewall." << m_datagramSentToBaseBroadcastAddress << "datagram pendings";
    emit udpPortBlocked();
  }
}

QList<QHostAddress> Broadcaster::parseHostAddress( const QHostAddress& host_address ) const
{
  QList<QHostAddress> ha_list;
  QString ha_string = host_address.toString();

  if( host_address == m_baseBroadcastAddress )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Broadcaster skips base address:" << ha_string;
#endif
    return ha_list;
  }

  if( ha_string.contains( ":" ) )
  {
    ha_list << host_address;
#ifdef BEEBEEP_DEBUG
    qDebug() << "Broadcaster has found IPV6 address:" << ha_string;
#endif
    return ha_list;
  }

  if( !ha_string.contains( QLatin1String( "255" ) ) )
  {
    ha_list << host_address;
#ifdef BEEBEEP_DEBUG
    qDebug() << "Broadcaster has found IPV4 address:" << ha_string;
#endif
    return ha_list;
  }

  if( !Settings::instance().parseBroadcastAddresses() )
  {
    ha_list << host_address;
#ifdef BEEBEEP_DEBUG
    qDebug() << "Broadcaster has found IPV4 subnet skipped:" << ha_string;
#endif
    return ha_list;
  }

  if( ha_string.count( QLatin1String( "255" ) ) > 1 )
  {
    ha_list << host_address;
#ifdef BEEBEEP_DEBUG
    qDebug() << "Broadcaster has found IPV4 subnet too big and skipped:" << ha_string;
#endif
    return ha_list;
  }

  QStringList ha_string_list = ha_string.split( "." );
  if( ha_string_list.size() != 4 )
  {
    qWarning() << "Broadcaster has found an invalid IPV4 address:" << ha_string;
    return ha_list;
  }

  if( ha_string_list.last() == QLatin1String( "255" ) )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Broadcaster has found IPV4 subnet and has parsed it:" << ha_string;
#endif
    ha_string_list.removeLast();
    ha_string = ha_string_list.join( "." );
    QString s_tmp;
    for( int i = 1; i < 255; i++ )
    {
      s_tmp = QString( "%1.%2" ).arg( ha_string ).arg( i );
      ha_list << QHostAddress( s_tmp );
    }
  }
  return ha_list;
}
