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
// Author: Marco Mastroddi (marco.mastroddi(AT)gmail.com)
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
  updateAddresses();

  m_broadcastTimer.setSingleShot( false );

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
  qDebug() << "Broadcaster generates broadcast message data";
  m_broadcastData = Protocol::instance().broadcastMessage();
  qDebug() << "Broadcaster starts broadcasting with listener port" << Settings::instance().localUser().hostPort();
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
  if( m_broadcastTimer.isActive() )
    m_broadcastTimer.stop();
  m_broadcastSocket.abort();
}

bool Broadcaster::sendBroadcastMessage()
{
  int addresses_contacted = 0;
  bool addresses_are_valid = true;
  foreach( QHostAddress host_address, m_broadcastAddresses )
  {
    if( !sendDatagramToHost( host_address ) )
      addresses_are_valid = false;
    else
      addresses_contacted++;
  }

  QList<QHostAddress>::iterator it = m_broadcastAddressesAdded.begin();
  while( it != m_broadcastAddressesAdded.end() )
  {
    if( !sendDatagramToHost( *it ) )
    {
      it = m_broadcastAddressesAdded.erase( it );
    }
    else
    {
      addresses_contacted++;
      ++it;
    }
  }

  if( !addresses_are_valid )
    updateAddresses();

  return addresses_contacted > 0;
}

bool Broadcaster::isLocalHostAddress( const QHostAddress& address )
{
  foreach( QHostAddress localAddress, m_ipAddresses )
  {
    if( address == localAddress )
      return true;
  }
  return false;
}

bool Broadcaster::sendDatagramToHost( const QHostAddress& host_address )
{
  qDebug() << "Broadcaster casts to network:" << host_address.toString();
  return m_broadcastSocket.writeDatagram( m_broadcastData, host_address, Settings::instance().broadcastPort() ) > 0;
}

void Broadcaster::sendBroadcastDatagram()
{
  sendBroadcastMessage();
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
      qDebug() << "Broadcaster has received an invalid data size:" << datagram;
      continue;
    }
    Message m = Protocol::instance().toMessage( datagram );
    if( !m.isValid() || m.type() != Message::Beep )
    {
      qDebug() << "Broadcaster has received an invalid data:" << datagram;
      continue;
    }
    bool ok = false;
    int sender_listener_port = m.text().toInt( &ok );
    if( !ok )
      continue;

    if( isLocalHostAddress( sender_ip ) && sender_listener_port == Settings::instance().localUser().hostPort() )
      continue;

    emit newPeerFound( sender_ip, sender_listener_port );
  }

  if( num_datagram_read > 1 )
    qDebug() << "Broadcaster read" << num_datagram_read << "datagrams";
}

void Broadcaster::updateAddresses()
{
  qDebug() << "Broadcaster updates the addresses";
  m_broadcastAddresses.clear();
  m_ipAddresses.clear();
  QHostAddress broadcast_address;

  foreach( QNetworkInterface interface, QNetworkInterface::allInterfaces() )
  {
    foreach( QNetworkAddressEntry entry, interface.addressEntries() )
    {
      broadcast_address = entry.broadcast();
      if( broadcast_address != QHostAddress::Null && entry.ip() != QHostAddress::LocalHost )
      {
        m_broadcastAddresses << broadcast_address;
        m_ipAddresses << entry.ip();
        qDebug() << "Broadcaster adds" << broadcast_address.toString() << "and" << entry.ip().toString();
      }
    }
  }

  foreach( QString address_string, Settings::instance().broadcastAddresses() )
  {
    broadcast_address = QHostAddress( address_string );
    if( !broadcast_address.isNull() && !m_broadcastAddresses.contains( broadcast_address ) )
    {
      qDebug() << "Broadcaster adds" << address_string;
      m_broadcastAddresses << QHostAddress( address_string );
    }
  }
}

bool Broadcaster::addAddress( const QHostAddress& host_address )
{
  if( m_broadcastAddressesAdded.contains( host_address ) )
    return false;
  m_broadcastAddressesAdded.append( host_address );
  return true;
}
