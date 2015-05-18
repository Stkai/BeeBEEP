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

#ifndef BEEBEEP_BONJOURREGISTER_H
#define BEEBEEP_BONJOURREGISTER_H

#include "Config.h"
#include "BonjourRecord.h"
#include "dns_sd.h"


class BonjourRegister : public QObject
{
  Q_OBJECT

public:
  BonjourRegister( QObject *parent = 0 );
  ~BonjourRegister();

  void registerService( const BonjourRecord&, int );
  inline const BonjourRecord& registeredService() const;

signals:
  void error( int );
  void serviceRegistered();

private slots:
  void socketIsReadyRead();

protected:
  static void DNSSD_API BonjourRegisterService( DNSServiceRef, DNSServiceFlags flags,
                                                DNSServiceErrorType error_code, const char *service_name,
                                                const char *registered_type, const char *reply_domain,
                                                void *register_service_ref );
  inline void setRegisteredService( const BonjourRecord& );

private:
 DNSServiceRef mp_dnss;
 QSocketNotifier* mp_socket;
 BonjourRecord m_registeredService;

};

// Inline Functions
inline const BonjourRecord& BonjourRegister::registeredService() const { return m_registeredService; }
inline void BonjourRegister::setRegisteredService( const BonjourRecord& br ) { m_registeredService = br; }


#endif // BEEBEEP_BONJOURREGISTER_H
