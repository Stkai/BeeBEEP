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
// Author: Marco Mastroddi (marco.mastroddi(AT)gmail.com)
//
// $Id: Message.h 24 2010-06-13 17:36:35Z mastroddi $
//
//////////////////////////////////////////////////////////////////////

#ifndef BEEBEEP_MESSAGE_H
#define BEEBEEP_MESSAGE_H

#include "Config.h"


class Message
{
public:
  enum Type { Undefined, Beep, Hello, Ping, Pong, Chat, System, File };
  enum Flag { Private, NumFlags };

  Message();
  Message( const Message& );
  Message( Type, const QString& );

  inline bool isValid() const;
  inline void setType( Type );
  inline void setFlags( int );
  inline void setData( const QString& );
  inline void setTimestamp( const QDateTime& );
  inline void setText( const QString& );
  inline void addFlag( Flag );
  inline Type type() const;
  inline int flags() const;
  inline const QString& data() const;
  inline const QDateTime& timestamp() const;
  inline const QString& text() const;
  inline bool hasFlag( Flag ) const;

  Message& operator=( const Message& );

private:
  Type m_type;
  int m_flags;
  QString m_data;
  QDateTime m_timestamp;
  QString m_text;

};


// Inline Functions

inline bool Message::isValid() const { return m_type != Message::Undefined; }
inline void Message::setType( Type new_value ) { m_type = new_value; }
inline void Message::setFlags( int new_value ) { m_flags = new_value; }
inline void Message::setData( const QString& new_value ) { m_data = new_value; }
inline void Message::setTimestamp( const QDateTime& new_value ) { m_timestamp = new_value; }
inline void Message::setText( const QString& new_value ) { m_text = new_value; }
inline void Message::addFlag( Flag f ) { m_flags |= (1 << f); }
inline Message::Type Message::type() const { return m_type; }
inline int Message::flags() const { return m_flags; }
inline const QString& Message::data() const { return m_data; }
inline const QDateTime& Message::timestamp() const { return m_timestamp; }
inline const QString& Message::text() const { return m_text; }
inline bool Message::hasFlag( Flag f ) const { return m_flags != 0 && m_flags & (1 << f); }



#endif // MESSAGE_H
