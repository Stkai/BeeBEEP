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

#ifndef BEEBEEP_BONJOURMANAGER_H
#define BEEBEEP_BONJOURMANAGER_H

#include "UserRecord.h"
#include "BonjourRecord.h"
class BonjourRegister;
class BonjourBrowser;


class BonjourManager : public QObject
{
  Q_OBJECT

public:
  BonjourManager( QObject* parent = 0 );

  inline const QList<UserRecord>& userRecords() const;

  void start( const QString& service_base_name, const QString& service_type, const QString& listener_address, int listener_port );
  void stop();

signals:
  void newUserFound( const UserRecord& );

protected slots:
  void serviceIsRegistered();
  void addBonjourRecord( const BonjourRecord& );
  void removeBonjourRecord( const BonjourRecord& );
  void serviceResolved( const QHostInfo&, int );

protected:
  void addUserRecord( const UserRecord& );

private:
  BonjourRegister* mp_register;
  BonjourBrowser* mp_browser;
  QString m_serviceName;
  QList<BonjourRecord> m_bonjourRecords;
  QList<UserRecord> m_userRecords;


};

// Inline Functions
inline const QList<UserRecord>& BonjourManager::userRecords() const { return m_userRecords; }

#endif // BEEBEEP_BONJOURMANAGER_H
