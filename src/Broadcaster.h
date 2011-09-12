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
// $Id: Broadcaster.h 74 2011-08-30 15:17:34Z mastroddi $
//
//////////////////////////////////////////////////////////////////////

#ifndef BEEBEEP_BROADCASTER_H
#define BEEBEEP_BROADCASTER_H

#include "Config.h"


class Broadcaster : public QObject
{
  Q_OBJECT

public:
  explicit Broadcaster( QObject* );
  bool startBroadcasting();
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
  QByteArray m_broadcastData;

};


#endif // BEEBEEP_BROADCASTER_H
