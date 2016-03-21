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

#ifndef BEEBEEP_MDNSREGISTER_H
#define BEEBEEP_MDNSREGISTER_H

#include "MDnsObject.h"


class MDnsRegister : public MDnsObject
{
  Q_OBJECT

public:
  MDnsRegister( QObject *parent = 0 );

  bool registerService( const MDnsRecord&, int );
  void unregisterService();
  inline int servicePort() const;
  inline void setServiceRegistered( bool );
  inline bool serviceIsRegistered() const;

signals:
  void serviceRegistered();

protected:
  static void DNSSD_API MDnsRegisterService( DNSServiceRef, DNSServiceFlags flags,
                                                DNSServiceErrorType error_code, const char *service_name,
                                                const char *registered_type, const char *reply_domain,
                                                void *register_service_ref );

private:
  int m_servicePort;
  bool m_serviceIsRegistered;

};


// Inline Functions
inline int MDnsRegister::servicePort() const { return m_servicePort; }
inline void MDnsRegister::setServiceRegistered( bool new_value ) { m_serviceIsRegistered = new_value; }
inline bool MDnsRegister::serviceIsRegistered() const { return m_serviceIsRegistered; }

#endif // BEEBEEP_MDNSREGISTER_H
