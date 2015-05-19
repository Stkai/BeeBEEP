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

#ifndef BEEBEEP_BONJOURBROWSER_H
#define BEEBEEP_BONJOURBROWSER_H

#include "BonjourObject.h"


class BonjourBrowser : public BonjourObject
{
  Q_OBJECT

public:
  BonjourBrowser( QObject *parent = 0 );

  void browseForService( const QString& );
  void stop();

signals:
  void newRecordFound( const BonjourRecord& );
  void recordToRemove( const BonjourRecord& );

protected:
  static void DNSSD_API BonjourBrowseReply( DNSServiceRef, DNSServiceFlags flags, quint32,
                                 DNSServiceErrorType error_code, const char *service_name,
                                 const char *registered_type, const char *reply_domain, void *browser_service_ref );


};

#endif // BEEBEEP_BONJOURBROWSER_H
