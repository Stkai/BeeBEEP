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

#include "Config.h"
#include "BonjourRecord.h"
#include "dns_sd.h"


class BonjourBrowser : public QObject
{
  Q_OBJECT

public:
  BonjourBrowser( QObject *parent = 0 );
  ~BonjourBrowser();

  void browseForServiceType( const QString& );

  inline const QList<BonjourRecord>& records() const;
  inline const QString& serviceType() const;

signals:
  void recordsChanged();
  void error( int );

private slots:
  void socketIsReadyRead();

protected:
  static void DNSSD_API BonjourBrowseReply( DNSServiceRef, DNSServiceFlags flags, quint32,
                                 DNSServiceErrorType error_code, const char *service_name,
                                 const char *registered_type, const char *reply_domain, void *browser_service_ref );

  bool addRecord( const BonjourRecord& );
  inline bool removeRecord( const BonjourRecord& );

private:
  DNSServiceRef mp_dnss;
  QSocketNotifier *mp_socket;
  QList<BonjourRecord> m_records;
  QString m_serviceType;

};

// Inline Functions
inline const QList<BonjourRecord>& BonjourBrowser::records() const { return m_records; }
inline bool BonjourBrowser::removeRecord( const BonjourRecord& br ) { return m_records.removeOne( br ); }
inline const QString& BonjourBrowser::serviceType() const { return m_serviceType; }


#endif // BEEBEEP_BONJOURBROWSER_H
