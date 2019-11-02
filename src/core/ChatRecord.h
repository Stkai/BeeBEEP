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

#ifndef BEEBEEP_CHATRECORD_H
#define BEEBEEP_CHATRECORD_H

#include "Config.h"


class ChatRecord
{
public:
  ChatRecord();
  ChatRecord( const ChatRecord& );
  ChatRecord( const QString& chat_name, const QString& chat_private_id );

  ChatRecord& operator=( const ChatRecord& );
  bool operator==( const ChatRecord& );

  inline bool isValid() const;
  inline void setName( const QString& );
  inline const QString& name() const;
  inline void setPrivateId( const QString& );
  inline const QString& privateId() const;

private:
  QString m_name;
  QString m_privateId;

};


// Inline Functions
inline bool ChatRecord::isValid() const { return !m_name.isEmpty() || !m_privateId.isEmpty(); }
inline void ChatRecord::setName( const QString& new_value ) { m_name = new_value; }
inline const QString& ChatRecord::name() const { return m_name; }
inline void ChatRecord::setPrivateId( const QString& new_value ) { m_privateId = new_value; }
inline const QString& ChatRecord::privateId() const { return m_privateId; }

#endif // BEEBEEP_CHATRECORD_H
