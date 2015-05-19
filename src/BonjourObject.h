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

#ifndef BEEBEEP_BONJOUROBJECT_H
#define BEEBEEP_BONJOUROBJECT_H

#include "Config.h"
#include "BonjourRecord.h"
#include "dns_sd.h"


class BonjourObject : public QObject
{
  Q_OBJECT

public:
  BonjourObject( QObject* parent = 0 );
  ~BonjourObject();

  inline const BonjourRecord& record() const;
  inline void setRecord( const BonjourRecord& );

signals:
  void error( int );

protected:
  virtual void cleanUp();
  virtual bool checkErrorAndReadSocket( DNSServiceErrorType error_code );

protected slots:
  void socketIsReadyRead();

protected:
 DNSServiceRef mp_dnss;
 QSocketNotifier* mp_socket;
 BonjourRecord m_record;

};

// Inline Functions
inline const BonjourRecord& BonjourObject::record() const { return m_record; }
inline void BonjourObject::setRecord( const BonjourRecord& new_value ) { m_record = new_value; }

#endif // BEEBEEP_BONJOUROBJECT_H
