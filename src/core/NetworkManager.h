//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2020 Marco Mastroddi
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
// along with BeeBEEP. If not, see <http://www.gnu.org/licenses/>.
//
// Author: Marco Mastroddi <marco.mastroddi(AT)gmail.com>
//
// $Id$
//
//////////////////////////////////////////////////////////////////////

#ifndef BEEBEEP_NETWORKMANAGER_H
#define BEEBEEP_NETWORKMANAGER_H

#include "Config.h"
#include "NetworkEntry.h"


class NetworkManager
{
// Singleton Object
  static NetworkManager* mp_instance;

public:
  bool searchLocalHostAddress();
  bool isMainInterfaceUp() const;
  bool isInterfaceUp( const QHostAddress& ) const;
  inline bool isMainInterfaceUnavailable() const;

  inline const QHostAddress& localHostAddress() const;
  inline const QHostAddress& localBroadcastAddress() const;
  inline const QString& localHostAddressScopeId() const;
  inline const QString& localInterfaceHardwareAddress() const;
  QNetworkInterface localNetworkInterface() const;

  QList<QHostAddress> splitInIPv4HostAddresses( const QHostAddress& ) const;
  bool isLocalHostAddress( const QHostAddress& ) const;
  QList<QHostAddress> localBroadcastAddresses() const;
  inline bool isInLocalBroadcastAddresses( const QHostAddress& ) const;

  inline bool networkInterfaceCanBroadcast( const QNetworkInterface& ) const;
  inline bool networkInterfaceCanMulticast( const QNetworkInterface& ) const;

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
      mp_instance = Q_NULLPTR;
    }
  }

protected:
  NetworkManager();

  inline bool isNetworkEntryAvailable( const NetworkEntry& ) const;
  inline bool isNetworkInterfaceAvailable( const QNetworkInterface& ) const;
  bool isHostAddressInBroadcastSubnet( const QHostAddress&, const QString& );
  bool checkPreferredSubnets();
  bool forceLocalHostAddress( const QHostAddress& );
  bool forceLocalSubnet( const QString& );
  void setLocalHostAddress( const NetworkEntry& );
  NetworkEntry firstNetworkEntry( bool use_ipv4 ) const;
  QList<NetworkEntry> availableNetworkEntries() const;
  QHostAddress broadcastSubnetFromIPv4HostAddress( const QHostAddress& ) const;

private:
  QHostAddress m_localHostAddress;
  QHostAddress m_localBroadcastAddress;
  QString m_localHostAddressScopeId;
  QString m_localInterfaceHardwareAddress;
  QList<NetworkEntry> m_networkEntries;

};


// Inline Function
inline const QHostAddress& NetworkManager::localHostAddress() const { return m_localHostAddress; }
inline const QHostAddress& NetworkManager::localBroadcastAddress() const { return m_localBroadcastAddress; }
inline const QString& NetworkManager::localHostAddressScopeId() const { return m_localHostAddressScopeId; }
inline const QString& NetworkManager::localInterfaceHardwareAddress() const { return m_localInterfaceHardwareAddress; }
inline bool NetworkManager::isNetworkEntryAvailable( const NetworkEntry& ne ) const { return ne.isValid() && !ne.isLoopback() && ne.isProtocolValid() && !ne.isLinkLocal(); }
inline bool NetworkManager::isNetworkInterfaceAvailable( const QNetworkInterface& ni ) const { return (ni.flags() & QNetworkInterface::IsUp) && (ni.flags() & QNetworkInterface::IsRunning) && (ni.flags() & ~QNetworkInterface::IsLoopBack); }
inline bool NetworkManager::networkInterfaceCanBroadcast( const QNetworkInterface& ni ) const { return isNetworkInterfaceAvailable( ni ) && (ni.flags() & QNetworkInterface::CanBroadcast); }
inline bool NetworkManager::networkInterfaceCanMulticast( const QNetworkInterface& ni ) const { return isNetworkInterfaceAvailable( ni ) && (ni.flags() & QNetworkInterface::CanMulticast); }
inline bool NetworkManager::isMainInterfaceUnavailable() const { return m_localInterfaceHardwareAddress.isEmpty(); }
inline bool NetworkManager::isInLocalBroadcastAddresses( const QHostAddress& host_address ) const { return localBroadcastAddresses().contains( host_address ); }

#endif // BEEBEEP_NETWORKMANAGER_H
