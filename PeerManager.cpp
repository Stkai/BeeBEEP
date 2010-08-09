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
// $Id: PeerManager.cpp 34 2010-07-13 18:45:23Z mastroddi $
//
//////////////////////////////////////////////////////////////////////

#include "PeerManager.h"
#include "Protocol.h"
#include "Settings.h"


PeerManager::PeerManager( QObject *parent )
 : QObject( parent )
{
  updateAddresses();

  connect( &m_broadcastSocket, SIGNAL( readyRead() ), this, SLOT( readBroadcastDatagram() ) );

  m_broadcastTimer.setInterval( BROADCAST_INTERVAL );
  connect( &m_broadcastTimer, SIGNAL( timeout() ), this, SLOT( sendBroadcastDatagram() ) );
}

void PeerManager::startBroadcasting()
{
  m_broadcastSocket.bind( QHostAddress::Any, BROADCAST_PORT, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint );
  m_broadcastTimer.start();
}

void PeerManager::stopBroadcasting()
{
  m_broadcastTimer.stop();
  m_broadcastSocket.abort();
}

bool PeerManager::isLocalHostAddress( const QHostAddress& address )
{
  foreach( QHostAddress localAddress, m_ipAddresses )
  {
    if( address == localAddress )
      return true;
  }
  return false;
}

bool PeerManager::sendDatagramToHost( const QHostAddress& host_address )
{
  QByteArray datagram = Protocol::instance().broadcastMessage().toUtf8();
  return m_broadcastSocket.writeDatagram( datagram, host_address, BROADCAST_PORT ) > 0;
}

void PeerManager::sendBroadcastDatagram()
{
  QByteArray datagram = Protocol::instance().broadcastMessage().toUtf8();
  bool validBroadcastAddresses = true;
  foreach( QHostAddress address, m_broadcastAddresses )
  {
    if( m_broadcastSocket.writeDatagram( datagram, address, BROADCAST_PORT ) == -1 )
      validBroadcastAddresses = false;
  }

  if( !validBroadcastAddresses )
    updateAddresses();
}

void PeerManager::readBroadcastDatagram()
{
  while( m_broadcastSocket.hasPendingDatagrams() )
  {
    QHostAddress sender_ip;
    quint16 sender_port;
    QByteArray datagram;
    datagram.resize( m_broadcastSocket.pendingDatagramSize() );
    if( m_broadcastSocket.readDatagram( datagram.data(), datagram.size(), &sender_ip, &sender_port ) == -1 )
      continue;
    if( datagram.size() <= Protocol::instance().messageMinimumSize() )
    {
      qWarning() << "Invalid datagram size:" << datagram;
      continue;
    }
    Message m = Protocol::instance().toMessage( QString::fromUtf8( datagram ) );
    if( !m.isValid() || m.type() != Message::Beep )
      continue;

    bool ok = false;
    int senderServerPort = m.text().toInt( &ok );
    if( !ok )
      continue;

    if( isLocalHostAddress( sender_ip ) && senderServerPort == Settings::instance().listenerPort() )
      continue;

    emit newPeerFound( sender_ip, senderServerPort );
  }
}

void PeerManager::updateAddresses()
{
  m_broadcastAddresses.clear();
  m_ipAddresses.clear();
  foreach( QNetworkInterface interface, QNetworkInterface::allInterfaces() )
  {
    foreach( QNetworkAddressEntry entry, interface.addressEntries() )
    {
      QHostAddress broadcastAddress = entry.broadcast();
      if( broadcastAddress != QHostAddress::Null && entry.ip() != QHostAddress::LocalHost )
      {
        m_broadcastAddresses << broadcastAddress;
        m_ipAddresses << entry.ip();
      }
    }
  }
}
