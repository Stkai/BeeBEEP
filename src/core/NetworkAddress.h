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

#ifndef BEEBEEP_NETWORKADDRESS_H
#define BEEBEEP_NETWORKADDRESS_H

#include "Config.h"


class NetworkAddress
{
public:
  NetworkAddress();
  NetworkAddress( const NetworkAddress& );
  NetworkAddress( const QHostAddress&, quint16 );

  NetworkAddress& operator=( const NetworkAddress& );
  inline bool operator==( const NetworkAddress& ) const;

  inline bool isHostAddressValid() const;
  inline bool isHostPortValid() const;
  inline bool isIPv4Address() const;
  inline bool isIPv6Address() const;
  inline bool isLoopback() const;
  static bool isLoopback( const QHostAddress& );
  bool isLinkLocal() const;
  inline bool isProtocolValid() const;

  inline void setHostAddress( const QHostAddress& );
  inline const QHostAddress& hostAddress() const;
  inline void setHostPort( quint16 );
  inline quint16 hostPort() const;

  inline void setInfo( const QString& );
  inline const QString& info() const;

  static NetworkAddress fromString( const QString& );
  QString toString() const;

private:
  QHostAddress m_hostAddress;
  quint16 m_hostPort;
  QString m_info;

};

// Inline Functions
inline bool NetworkAddress::operator==( const NetworkAddress& na ) const { return m_hostAddress == na.m_hostAddress && m_hostPort == na.m_hostPort; }
inline bool NetworkAddress::isHostAddressValid() const { return !m_hostAddress.isNull(); }
inline bool NetworkAddress::isHostPortValid() const { return m_hostPort > 0; }
inline bool NetworkAddress::isIPv4Address() const { return m_hostAddress.protocol() == QAbstractSocket::IPv4Protocol; }
inline bool NetworkAddress::isIPv6Address() const { return m_hostAddress.protocol() == QAbstractSocket::IPv6Protocol; }
#if QT_VERSION >= 0x050000
  inline bool NetworkAddress::isLoopback() const { return m_hostAddress.isLoopback() || m_hostAddress.toString() == QString( "127.0.0.1" ) || m_hostAddress.toString() == QString( "::1" ) || m_hostAddress.toString() == QString( "127.0.1.1" ); }
#else
  inline bool NetworkAddress::isLoopback() const { return m_hostAddress == QHostAddress::LocalHost || m_hostAddress == QHostAddress::LocalHostIPv6 || m_hostAddress.toString() == QString( "127.0.0.1" ) || m_hostAddress.toString() == QString( "::1" ) || m_hostAddress.toString() == QString( "127.0.1.1" ); }
#endif
inline bool NetworkAddress::isProtocolValid() const { return isIPv4Address() || isIPv6Address(); }
inline void NetworkAddress::setHostAddress( const QHostAddress& new_value ) { m_hostAddress = new_value; }
inline const QHostAddress& NetworkAddress::hostAddress() const { return m_hostAddress; }
inline void NetworkAddress::setHostPort( quint16 new_value ) { m_hostPort = new_value; }
inline quint16 NetworkAddress::hostPort() const { return m_hostPort; }
inline void NetworkAddress::setInfo( const QString& new_value ) { m_info = new_value; }
inline const QString& NetworkAddress::info() const { return m_info; }

#endif // BEEBEEP_NETWORKADDRESS_H
