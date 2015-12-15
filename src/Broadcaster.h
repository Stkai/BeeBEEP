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

#ifndef BEEBEEP_BROADCASTER_H
#define BEEBEEP_BROADCASTER_H

#include "Config.h"
#include "NetworkAddress.h"


class Broadcaster : public QObject
{
  Q_OBJECT

public:
  explicit Broadcaster( QObject* );
  bool startBroadcasting();
  void stopBroadcasting();
  int updateAddresses();

  inline const QList<QHostAddress> contactedAddress() const;

public slots:
  void sendBroadcastDatagram();

signals:
  void newPeerFound( const QHostAddress&, int );
  void udpPortBlocked();

private slots:
  void readBroadcastDatagram();
  void checkLoopback();
  void searchInPeerAddresses();

protected:
  bool sendDatagramToHost( const QHostAddress& );
  bool addAddressToList( const QHostAddress& );
  bool isLocalHostAddress( const QHostAddress& );
  void addPeerAddress( const NetworkAddress& );

private:
  QHostAddress m_baseBroadcastAddress;
  QList<QHostAddress> m_broadcastAddresses;
  QList<QHostAddress> m_ipAddresses;
  QUdpSocket m_broadcastSocket;

  QTimer m_broadcastTimer;

  QList<NetworkAddress> m_peerAddresses;
  QList<QHostAddress> m_contactedAddresses;

};

// Inline Functions
inline const QList<QHostAddress> Broadcaster::contactedAddress() const { return m_contactedAddresses; }

#endif // BEEBEEP_BROADCASTER_H
