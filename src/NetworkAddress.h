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

#ifndef BEEBEEP_NETWORKADDRESS_H
#define BEEBEEP_NETWORKADDRESS_H

#include "Config.h"


class NetworkAddress
{
public:
  NetworkAddress();
  NetworkAddress( const NetworkAddress& );

  NetworkAddress& operator=( const NetworkAddress& );
  inline bool operator==( const NetworkAddress& ) const;

  inline bool isValid() const;
  inline bool isIpv4Address() const;
  inline bool isIpv6Address() const;
  bool isLoopback() const;

  inline void setHostAddress( const QHostAddress& );
  inline const QHostAddress& hostAddress() const;
  inline void setHostPort( int );
  inline int hostPort() const;

  static NetworkAddress fromString( const QString& );
  QString toString() const;

private:
  QHostAddress m_hostAddress;
  int m_hostPort;

};

// Inline Functions
inline bool NetworkAddress::operator==( const NetworkAddress& na ) const { return m_hostAddress == na.m_hostAddress && m_hostPort == na.m_hostPort; }
inline bool NetworkAddress::isValid() const { return !m_hostAddress.isNull(); }
inline bool NetworkAddress::isIpv4Address() const { return m_hostAddress.protocol() == QAbstractSocket::IPv4Protocol; }
inline bool NetworkAddress::isIpv6Address() const { return m_hostAddress.protocol() == QAbstractSocket::IPv6Protocol; }
inline void NetworkAddress::setHostAddress( const QHostAddress& new_value ) { m_hostAddress = new_value; }
inline const QHostAddress& NetworkAddress::hostAddress() const { return m_hostAddress; }
inline void NetworkAddress::setHostPort( int new_value ) { m_hostPort = new_value; }
inline int NetworkAddress::hostPort() const { return m_hostPort; }

#endif // BEEBEEP_NETWORKADDRESS_H
