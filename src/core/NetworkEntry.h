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

#ifndef BEEBEEP_NETWORKENTRY_H
#define BEEBEEP_NETWORKENTRY_H

#include "Config.h"
#include "NetworkAddress.h"


class NetworkEntry
{
public:
  NetworkEntry();
  NetworkEntry( const NetworkEntry& );
  NetworkEntry( const QString&, const QNetworkAddressEntry& );

  NetworkEntry& operator=( const NetworkEntry& );
  inline bool operator==( const NetworkEntry& ) const;

  inline bool isValid() const;
  inline bool isIPv4Address() const;
  inline bool isIPv6Address() const;
  inline bool isLoopback() const;
  inline bool isLinkLocal() const;
  inline bool isProtocolValid() const;

  inline const QString& hardware() const;
  inline const QHostAddress& hostAddress() const;
  inline const QHostAddress& broadcast() const;
  inline const QHostAddress& netmask() const;
  inline const QPair<QHostAddress, int> subnet() const;

  bool hasHostAddress( const QHostAddress& ) const;

private:
  QString m_hardware;
  NetworkAddress m_address;
  QHostAddress m_broadcast;
  QHostAddress m_netmask;
  QPair<QHostAddress, int> m_subnet;

};

// Inline Functions
inline bool NetworkEntry::operator==( const NetworkEntry& ne ) const { return m_hardware == ne.m_hardware && m_address == ne.m_address; }
inline bool NetworkEntry::isValid() const { return !m_hardware.isEmpty() && m_address.isHostAddressValid(); }
inline bool NetworkEntry::isIPv4Address() const { return m_address.isIPv4Address(); }
inline bool NetworkEntry::isIPv6Address() const { return m_address.isIPv6Address(); }
inline bool NetworkEntry::isLoopback() const { return m_address.isLoopback(); }
inline bool NetworkEntry::isLinkLocal() const { return m_address.isLinkLocal(); }
inline bool NetworkEntry::isProtocolValid() const { return m_address.isProtocolValid(); }
inline const QString& NetworkEntry::hardware() const { return m_hardware; }
inline const QHostAddress& NetworkEntry::hostAddress() const { return m_address.hostAddress(); }
inline const QHostAddress& NetworkEntry::broadcast() const { return m_broadcast; }
inline const QHostAddress& NetworkEntry::netmask() const { return m_netmask; }
inline const QPair<QHostAddress, int> NetworkEntry::subnet() const { return m_subnet; }

#endif // BEEBEEP_NETWORKENTRY_H
