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
// $Id: GuiChat.h 113 2011-09-26 18:01:56Z mastroddi $
//
//////////////////////////////////////////////////////////////////////


#ifndef BEEBEEP_GUICHATGRAPHICSITEM_H
#define BEEBEEP_GUICHATGRAPHICSITEM_H

#include "Config.h"
class ChatMessage;


class GuiChatGraphicsItem : public QGraphicsPathItem
{
public:
  enum Alignment { BoxOnLeft = 0, BoxOnRight };

  GuiChatGraphicsItem( QGraphicsItem * parent = 0 );

  void setChatMessage( const ChatMessage& );
  void setMaxWidth(int );

  inline int maxWidth() const;
  inline void setViewWidth( int );
  inline void setBoxStartLength( int );
  inline int boxStartLength() const;

  QRectF boundingRect() const;

protected:
  void paint( QPainter*, const QStyleOptionGraphicsItem*, QWidget* );
  QPainterPath createPath();
  int textWidth() const;
  void calculateWidth();

private:
  // max width of bubble including the spike
  int m_maxWidth;
  // actual width
  int m_width;

  // height of bubble
  int m_height;
  int m_spikeWidth;
  int m_spikeHeight;
  int m_cornerRadius;
  int m_textSpacing;
  int m_boxStartLength;
  int m_timeStampWidth;

  int m_length;
  Alignment m_alignment;

  QColor m_color;

  QString m_name;
  QString m_text;
  QString m_timeStamp;

};

// Inline Functions
inline int GuiChatGraphicsItem::textWidth() const { return m_maxWidth - m_spikeWidth - m_cornerRadius * 2; }
inline int GuiChatGraphicsItem::maxWidth() const { return m_maxWidth; }
inline void GuiChatGraphicsItem::setBoxStartLength( int new_value ) { m_boxStartLength = new_value; }
inline int GuiChatGraphicsItem::boxStartLength() const { return m_boxStartLength; }
inline void GuiChatGraphicsItem::setViewWidth( int new_value ) { setMaxWidth( new_value - m_boxStartLength - 25 ); } // 25 for scrollbar

#endif // BEEBEEP_GUICHATGRAPHICSITEM_H
