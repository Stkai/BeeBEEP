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
  bool startBroadcastServer();
  void stopBroadcasting();

  void onTickEvent( int );
  inline void setNewBroadcastRequested( bool );
  inline void setAddOfflineUsersInNetworkAddresses( bool );

  void updateUsersAddedManually();
  inline const QHostAddress& multicastGroupAddress() const;

public slots:
  void sendBroadcast();

signals:
  void newPeerFound( const QHostAddress&, int );
  void newPeerFoundFromDatagram( const QHostAddress&, const QHostAddress&, int );

private slots:
  void readBroadcastDatagram();
  void contactNetworkAddresses();

protected:
  void sendMulticastDatagram();
  bool contactNetworkAddress( const NetworkAddress& );

  bool addNetworkAddress( const NetworkAddress&, bool split_ipv4_address );
  inline bool addHostAddress( const QHostAddress& );
  void updateAddresses();

  void checkLoopbackDatagram();
  void removeHostAddressFromWaitingList( const QHostAddress& );

private:
  QUdpSocket* mp_receiverSocket;
  QUdpSocket* mp_senderSocket;
  QList<NetworkAddress> m_networkAddresses;
  bool m_newBroadcastRequested;
  QList< QPair<NetworkAddress, QDateTime> > m_networkAddressesWaitingForLoopback;
  bool m_addOfflineUsersInNetworkAddresses;
  QHostAddress m_multicastGroupAddress;
  QNetworkInterface m_multicastInterface;

};

// Inline Functions
inline bool Broadcaster::addHostAddress( const QHostAddress& ha ) { return addNetworkAddress( NetworkAddress( ha, 0 ), false ); }
inline void Broadcaster::setNewBroadcastRequested( bool new_value ) { m_newBroadcastRequested = new_value; }
inline void Broadcaster::setAddOfflineUsersInNetworkAddresses( bool new_value ) { m_addOfflineUsersInNetworkAddresses = new_value; }
inline const QHostAddress& Broadcaster::multicastGroupAddress() const { return m_multicastGroupAddress; }

#endif // BEEBEEP_BROADCASTER_H
