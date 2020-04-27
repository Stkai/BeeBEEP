//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2020 Marco Mastroddi
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

#ifndef BEEBEEP_MDNSOBJECT_H
#define BEEBEEP_MDNSOBJECT_H

#include "Config.h"
#include "MDnsRecord.h"
#include "dns_sd.h"

// using Apple Bonjour SDK 3

class MDnsObject : public QObject
{
  Q_OBJECT

public:
  MDnsObject( QObject* parent = Q_NULLPTR );
  ~MDnsObject();

  inline const MDnsRecord& record() const;
  inline void setRecord( const MDnsRecord& );

signals:
  void error( int );

protected:
  virtual void cleanUp();
  virtual bool checkErrorAndReadSocket( DNSServiceErrorType );

protected slots:
  void socketIsReadyRead( int );

protected:
 DNSServiceRef mp_dnss;
 QSocketNotifier* mp_socket;
 MDnsRecord m_record;

};

// Inline Functions
inline const MDnsRecord& MDnsObject::record() const { return m_record; }
inline void MDnsObject::setRecord( const MDnsRecord& new_value ) { m_record = new_value; }

#endif // BEEBEEP_MDNSOBJECT_H
