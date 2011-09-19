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

private:
  QColor m_textColor;

};


// Inline Functions
inline const QColor& ChatMessageData::textColor() const { return m_textColor; }
inline void ChatMessageData::setTextColor( const QColor& new_value ) { m_textColor = new_value; }

#endif // BEEBEEP_CHATMESSAGEDATA_H
