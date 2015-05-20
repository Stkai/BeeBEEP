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

#include "BonjourObject.h"


class BonjourRegister : public BonjourObject
{
  Q_OBJECT

public:
  BonjourRegister( QObject *parent = 0 );

  bool registerService( const BonjourRecord&, int );
  void unregisterService();
  inline int servicePort() const;

signals:
  void serviceRegistered();

protected:
  static void DNSSD_API BonjourRegisterService( DNSServiceRef, DNSServiceFlags flags,
                                                DNSServiceErrorType error_code, const char *service_name,
                                                const char *registered_type, const char *reply_domain,
                                                void *register_service_ref );

private:
  int m_servicePort;

};


// Inline Functions
inline int BonjourRegister::servicePort() const { return m_servicePort; }

#endif // BEEBEEP_BONJOURREGISTER_H
