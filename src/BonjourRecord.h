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

#ifndef BEEBEEP_BONJOURRECORD_H
#define BEEBEEP_BONJOURRECORD_H

#include "Config.h"


class BonjourRecord
{
public:
  BonjourRecord();
  BonjourRecord( const BonjourRecord& );
  BonjourRecord( const char *service_name, const char *registered_type, const char *reply_domain );

  BonjourRecord& operator=( const BonjourRecord& );
  inline bool operator==( const BonjourRecord& ) const;

  inline bool isValid() const;
  inline const QString& serviceName() const;
  inline void setServiceName( const QString& );
  inline const QString& registeredType() const;
  inline void setRegisteredType( const QString& );
  inline const QString& replyDomain() const;
  inline void setReplyDomain( const QString& );

private:
  QString m_serviceName;
  QString m_registeredType;
  QString m_replyDomain;

};

// Inline Functions
inline bool BonjourRecord::isValid() const { return !m_serviceName.isEmpty() && !m_registeredType.isEmpty() && !m_replyDomain.isEmpty(); }
inline bool BonjourRecord::operator==( const BonjourRecord &br ) const { return m_serviceName == br.m_serviceName && m_registeredType == br.m_registeredType && m_replyDomain == br.m_replyDomain; }
inline const QString& BonjourRecord::serviceName() const { return m_serviceName; }
inline void BonjourRecord::setServiceName( const QString& new_value ) { m_serviceName = new_value; }
inline const QString& BonjourRecord::registeredType() const { return m_registeredType; }
inline void BonjourRecord::setRegisteredType( const QString& new_value ) { m_registeredType = new_value; }
inline const QString& BonjourRecord::replyDomain() const { return m_replyDomain; }
inline void BonjourRecord::setReplyDomain( const QString& new_value ) { m_replyDomain = new_value; }

#endif // BEEBEEP_BONJOURRECORD_H
