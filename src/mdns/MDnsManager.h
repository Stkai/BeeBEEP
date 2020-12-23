//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2021 Marco Mastroddi
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

#ifndef BEEBEEP_MDNSMANAGER_H
#define BEEBEEP_MDNSMANAGER_H

#include "NetworkAddress.h"
#include "MDnsRecord.h"
class MDnsRegister;
class MDnsBrowser;
class MDnsResolver;


class MDnsManager : public QObject
{
  Q_OBJECT

public:
  MDnsManager( QObject* parent = Q_NULLPTR );

  inline const QList<NetworkAddress>& networkAddresses() const;

  bool start( const QString& service_base_name, const QString& service_type, const QString& listener_address, int listener_port );
  bool stop();
  inline bool isActive() const;

  bool browseForService();

signals:
  void serviceRegistered();
  void networkAddressFound( const NetworkAddress& );

protected slots:
  void serviceIsRegistered();
  void addMDnsRecord( const MDnsRecord& );
  void removeMDnsRecord( const MDnsRecord& );
  void serviceResolved( const QHostInfo&, int );

protected:
  void addNetworkAddress( const NetworkAddress& );

private:
  MDnsRegister* mp_register;
  MDnsBrowser* mp_browser;
  QString m_serviceName;
  QList<MDnsRecord> m_mdnsRecords;
  QList<NetworkAddress> m_networkAddresses;
  bool m_isActive;

};

// Inline Functions
inline const QList<NetworkAddress>& MDnsManager::networkAddresses() const { return m_networkAddresses; }
inline bool MDnsManager::isActive() const { return m_isActive; }

#endif // BEEBEEP_MDNSMANAGER_H
