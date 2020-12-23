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

#ifndef BEEBEEP_MDNSRESOLVER_H
#define BEEBEEP_MDNSRESOLVER_H

#include "MDnsObject.h"


class MDnsResolver : public MDnsObject
{
  Q_OBJECT

public:
  MDnsResolver( QObject *parent );
  ~MDnsResolver();

  void resolve( const MDnsRecord& );

signals:
  void resolved( const QHostInfo&, int );

private slots:
  void lookedUp( const QHostInfo& );

protected:
  static void DNSSD_API MDnsResolveReply(DNSServiceRef sdRef, DNSServiceFlags flags,
                                  quint32 interfaceIndex, DNSServiceErrorType errorCode,
                                  const char *fullName, const char *hosttarget, quint16 port,
                                  quint16 txtLen, const char *txtRecord, void *context);

  inline void setServicePort( int );
  inline void setLookUpHostId( int );
  void cleanUp();

private:
  MDnsRecord m_record;
  int m_servicePort;
  int m_lookUpHostId;

};


// Inline Functions
inline void MDnsResolver::setServicePort( int new_value ) { m_servicePort = new_value; }
inline void MDnsResolver::setLookUpHostId( int new_value ) { m_lookUpHostId = new_value; }

#endif // BEEBEEP_MDNSRESOLVER_H
