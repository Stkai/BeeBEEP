//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2021 Marco Mastroddi
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

#ifndef BEEBEEP_MESSAGE_H
#define BEEBEEP_MESSAGE_H

#include "Config.h"


class Message
{
public:
  // For backward compatibility do not change the order
  enum Type { Undefined, Beep, Hello, Ping, Pong, Chat, System, User, File, Share, Group,
              Folder, Read, Hive, ShareBox, ShareDesktop, Buzz, Test, Help, Received,
              NumTypes };
  enum Flag { Private, UserWriting, UserStatus, Create /* it was UserName in 3.0.9 */, UserVCard,
              Refused, List, Request, GroupChat, Delete, Auto, Important, VoiceMessage,
              EncryptionDisabled, Compressed, Delayed, SourceCode, NumFlags };

  Message();
  Message( const Message& );
  Message( Type, VNumber, const QString& );

  Message& operator=( const Message& );

  inline bool isValid() const;
  inline VNumber id() const;
  inline void setType( Type );
  inline void setId( VNumber );
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
  inline void setImportant();
  inline bool isImportant() const;
  inline void setDelayed();
  inline bool isDelayed() const;
  inline void setSourceCode();
  inline bool isSourceCode() const;

private:
  Type m_type;
  VNumber m_id;
  int m_flags;
  QString m_data;
  QDateTime m_timestamp;
  QString m_text;

};


// Inline Functions
inline bool Message::isValid() const { return m_type != Message::Undefined; }
inline VNumber Message::id() const { return m_id; }
inline void Message::setId( VNumber new_value ) { m_id = new_value; }
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
inline void Message::setImportant() { addFlag( Message::Important ); }
inline bool Message::isImportant() const { return hasFlag( Message::Important ); }
inline void Message::setDelayed() { addFlag( Message::Delayed ); }
inline bool Message::isDelayed() const { return hasFlag( Message::Delayed ); }
inline void Message::setSourceCode() { addFlag( Message::SourceCode ); }
inline bool Message::isSourceCode() const { return hasFlag( Message::SourceCode ); }

#endif // BEEBEEP_MESSAGE_H
