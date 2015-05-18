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

#ifndef BEEBEEP_BONJOURRESOLVER_H
#define BEEBEEP_BONJOURRESOLVER_H

#include "Config.h"
#include "BonjourRecord.h"
#include "dns_sd.h"


class BonjourResolver : public QObject
{
  Q_OBJECT

public:
  BonjourResolver( QObject *parent );
  ~BonjourResolver();

  void resolve( const BonjourRecord& );

signals:
  void resolved( const QHostInfo&, int );
  void error( int );

private slots:
  void socketIsReadyRead();
  void cleanupAfterResolve();
  void completeConnection( const QHostInfo& );

protected:
  static void DNSSD_API BonjourResolveReply(DNSServiceRef sdRef, DNSServiceFlags flags,
                                  quint32 interfaceIndex, DNSServiceErrorType errorCode,
                                  const char *fullName, const char *hosttarget, quint16 port,
                                  quint16 txtLen, const char *txtRecord, void *context);

  inline void setServicePort( int );

private:
  DNSServiceRef mp_dnss;
  QSocketNotifier* mp_socket;
  int m_servicePort;

};


// Inline Functions
inline void BonjourResolver::setServicePort( int new_value ) { m_servicePort = new_value; }


#endif // BEEBEEP_BONJOURRESOLVER_H
