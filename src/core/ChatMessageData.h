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

#ifndef BEEBEEP_CHATMESSAGEDATA_H
#define BEEBEEP_CHATMESSAGEDATA_H

#include "Config.h"


class ChatMessageData
{
public:
  ChatMessageData();
  ChatMessageData( const ChatMessageData& );
  ChatMessageData& operator=( const ChatMessageData& );

  inline const QColor& textColor() const;
  inline void setTextColor( const QColor& );
  inline const QString& groupId() const;
  inline void setGroupId( const QString& );
  inline const QString& groupName() const;
  inline void setGroupName( const QString& );
  inline const QDateTime& groupLastModified() const;
  inline void setGroupLastModified( const QDateTime& );

private:
  QColor m_textColor;
  QString m_groupId;
  QString m_groupName;
  QDateTime m_groupLastModified;

};


// Inline Functions
inline const QColor& ChatMessageData::textColor() const { return m_textColor; }
inline void ChatMessageData::setTextColor( const QColor& new_value ) { m_textColor = new_value; }
inline const QString& ChatMessageData::groupId() const { return m_groupId; }
inline void ChatMessageData::setGroupId( const QString& new_value ) { m_groupId = new_value; }
inline const QString& ChatMessageData::groupName() const { return m_groupName; }
inline void ChatMessageData::setGroupName( const QString& new_value) { m_groupName = new_value; }
inline const QDateTime& ChatMessageData::groupLastModified() const { return m_groupLastModified; }
inline void ChatMessageData::setGroupLastModified( const QDateTime& new_value ) { m_groupLastModified = new_value; }

#endif // BEEBEEP_CHATMESSAGEDATA_H
