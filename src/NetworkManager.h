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

#ifndef BEEBEEP_NETWORKMANAGER_H
#define BEEBEEP_NETWORKMANAGER_H

#include "Config.h"


class NetworkManager
{
// Singleton Object
  static NetworkManager* mp_instance;

public:
  bool searchLocalHostAddress();

  inline const QHostAddress& localHostAddress() const;
  inline const QHostAddress& localBroadcastAddress() const;
  inline const QString& localHostAddressScopeId() const;
  inline bool isIpv4Address( const QHostAddress& ) const;
  inline bool isIpv6Address( const QHostAddress& ) const;
  bool isLinkLocal( const QHostAddress& ) const;
  bool isLoopback( const QHostAddress& ) const;
  QHostAddress broadcastSubnetFromIpv4HostAddress( const QHostAddress& ) const;
  bool hostAddressIsInBroadcastSubnet( const QHostAddress&, const QString& );
  QList<QHostAddress> splitBroadcastSubnetToIpv4HostAddresses( const QHostAddress& ) const;

  static NetworkManager& instance()
  {
    if( !mp_instance )
      mp_instance = new NetworkManager();
    return *mp_instance;
  }

  static void close()
  {
    if( mp_instance )
    {
      delete mp_instance;
      mp_instance = NULL;
    }
  }

protected:
  NetworkManager();

  bool checkPreferredSubnets();
  bool forceLocalHostAddress( const QHostAddress& );
  bool forceLocalSubnet( const QString& );
  void setLocalHostAddress( const QHostAddress& );

private:
  QHostAddress m_localHostAddress;
  QHostAddress m_localBroadcastAddress;
  QString m_localHostAddressScopeId;

  QList<QHostAddress> m_ipv4Addresses;
  QList<QHostAddress> m_ipv6Addresses;

};


// Inline Function
inline const QHostAddress& NetworkManager::localHostAddress() const { return m_localHostAddress; }
inline const QHostAddress& NetworkManager::localBroadcastAddress() const { return m_localBroadcastAddress; }
inline const QString& NetworkManager::localHostAddressScopeId() const { return m_localHostAddressScopeId; }
inline bool NetworkManager::isIpv4Address( const QHostAddress& ha ) const { return ha.protocol() == QAbstractSocket::IPv4Protocol; }
inline bool NetworkManager::isIpv6Address( const QHostAddress& ha ) const { return ha.protocol() == QAbstractSocket::IPv6Protocol; }

#endif // BEEBEEP_NETWORKMANAGER_H
