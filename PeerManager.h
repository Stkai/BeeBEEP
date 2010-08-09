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
// $Id: PeerManager.h 24 2010-06-13 17:36:35Z mastroddi $
//
//////////////////////////////////////////////////////////////////////

#ifndef BEEBEEP_PEERMANAGER_H
#define BEEBEEP_PEERMANAGER_H

#include "Config.h"


class PeerManager : public QObject
{
  Q_OBJECT

public:
  PeerManager( QObject* );
  void startBroadcasting();
  void stopBroadcasting();
  bool isLocalHostAddress( const QHostAddress& );
  bool sendDatagramToHost( const QHostAddress& );

signals:
  void newPeerFound( const QHostAddress&, int );

private slots:
  void sendBroadcastDatagram();
  void readBroadcastDatagram();

private:
  void updateAddresses();

  QList<QHostAddress> m_broadcastAddresses;
  QList<QHostAddress> m_ipAddresses;
  QUdpSocket m_broadcastSocket;
  QTimer m_broadcastTimer;

};


#endif // BEEBEEP_PEERMANAGER_H
