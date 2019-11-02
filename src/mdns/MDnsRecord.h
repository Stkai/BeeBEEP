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

#ifndef BEEBEEP_MDNSRECORD_H
#define BEEBEEP_MDNSRECORD_H

#include "Config.h"


class MDnsRecord
{
public:
  MDnsRecord();
  MDnsRecord( const MDnsRecord& );
  MDnsRecord( const char *service_name, const char *registered_type, const char *reply_domain );

  MDnsRecord& operator=( const MDnsRecord& );
  inline bool operator==( const MDnsRecord& ) const;

  inline bool isValid() const;
  inline const QString& serviceName() const;
  inline void setServiceName( const QString& );
  inline const QString& registeredType() const;
  inline void setRegisteredType( const QString& );
  inline const QString& replyDomain() const;
  inline void setReplyDomain( const QString& );

  inline QString name() const;

private:
  QString m_serviceName;
  QString m_registeredType;
  QString m_replyDomain;

};

// Inline Functions
inline bool MDnsRecord::isValid() const { return !m_serviceName.isEmpty() && !m_registeredType.isEmpty() && !m_replyDomain.isEmpty(); }
inline bool MDnsRecord::operator==( const MDnsRecord &br ) const { return m_serviceName == br.m_serviceName && m_registeredType == br.m_registeredType && m_replyDomain == br.m_replyDomain; }
inline const QString& MDnsRecord::serviceName() const { return m_serviceName; }
inline void MDnsRecord::setServiceName( const QString& new_value ) { m_serviceName = new_value; }
inline const QString& MDnsRecord::registeredType() const { return m_registeredType; }
inline void MDnsRecord::setRegisteredType( const QString& new_value ) { m_registeredType = new_value; }
inline const QString& MDnsRecord::replyDomain() const { return m_replyDomain; }
inline void MDnsRecord::setReplyDomain( const QString& new_value ) { m_replyDomain = new_value; }
inline QString MDnsRecord::name() const { return QString( "%1|%2|%3" ).arg( m_serviceName, m_registeredType, m_replyDomain ); }

#endif // BEEBEEP_MDNSRECORD_H
