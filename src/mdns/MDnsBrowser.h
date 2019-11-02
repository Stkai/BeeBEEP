//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2019 Marco Mastroddi
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

#ifndef BEEBEEP_MDNSBROWSER_H
#define BEEBEEP_MDNSBROWSER_H

#include "MDnsObject.h"


class MDnsBrowser : public MDnsObject
{
  Q_OBJECT

public:
  MDnsBrowser( QObject *parent = Q_NULLPTR );

  bool browseForService( const QString& );
  void stop();
  inline bool isActive() const;

signals:
  void newRecordFound( const MDnsRecord& );
  void recordToRemove( const MDnsRecord& );

protected:
  static void DNSSD_API MDnsBrowseReply( DNSServiceRef, DNSServiceFlags flags, quint32,
                                 DNSServiceErrorType error_code, const char *service_name,
                                 const char *registered_type, const char *reply_domain, void *browser_service_ref );

private:
  bool m_isActive;

};


// Inline Functions
inline bool MDnsBrowser::isActive() const { return m_isActive; }

#endif // BEEBEEP_MDNSBROWSER_H
