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

#ifndef BEEBEEP_HISTORYMESSAGE_H
#define BEEBEEP_HISTORYMESSAGE_H

#include "Config.h"
#include "Emoticon.h"


class HistoryMessage
{
public:
  HistoryMessage();
  HistoryMessage( const HistoryMessage& );

  HistoryMessage& operator=( const HistoryMessage& );
  inline bool operator==( const HistoryMessage& ) const;

  inline void setMessage( const QString& );
  inline const QString& message() const;
  inline void setEmoticons( const QList<Emoticon>& );
  inline const QList<Emoticon>& emoticons() const;
  inline bool isEmpty() const;
  void clear();

private:
  QString m_message;
  QList<Emoticon> m_emoticons;

};


//Inline Functions
inline bool HistoryMessage::operator==( const HistoryMessage& hn ) const { return m_message == hn.m_message; }
inline void HistoryMessage::setMessage( const QString& new_value ) { m_message = new_value; }
inline const QString& HistoryMessage::message() const { return m_message; }
inline void HistoryMessage::setEmoticons( const QList<Emoticon>& new_value ) { m_emoticons = new_value; }
inline const QList<Emoticon>& HistoryMessage::emoticons() const { return m_emoticons; }
inline bool HistoryMessage::isEmpty() const { return m_message.isEmpty(); }

#endif // BEEBEEP_HISTORYMESSAGE_H
