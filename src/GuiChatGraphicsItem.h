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

#ifndef BEEBEEP_GUICHATGRAPHICSITEM_H
#define BEEBEEP_GUICHATGRAPHICSITEM_H

#include <QGraphicsPathItem>
class ChatMessage;


class GuiChatGraphicsItem : public QGraphicsPathItem
{
public:
  GuiChatGraphicsItem(QGraphicsItem * parent = 0);

  void setChatMessage( const ChatMessage& );

  void setBoxMaxWidth( int );
  inline int boxMaxWidth() const;
  inline void setViewWidth( int );
  inline int boxStartLength() const;
  inline void setBoxStartLength( int );

  virtual QRectF boundingRect() const;

protected:
  void paint( QPainter*, const QStyleOptionGraphicsItem*, QWidget* );
  void createPath();
  inline int boxTextWidth() const;
  void calculateBoxWidth();
  void setText( const QString& );

private:
  int m_boxMaxWidth;
  int m_boxWidth;
  int m_boxHeight;
  int m_spikeWidth;
  int m_spikeHeight;
  int m_cornerRadius;
  int m_textSpacing;
  int m_boxStartLength;
  int m_timeStampWidth;

  QColor m_color;
  QString m_text;
  QString m_name;
  QString m_timeStamp;
  int m_length;

};


// Inline Functions
inline int GuiChatGraphicsItem::boxMaxWidth() const { return m_boxMaxWidth; }
inline int GuiChatGraphicsItem::boxTextWidth() const { return m_boxMaxWidth - m_spikeWidth - m_cornerRadius*2; }
inline void GuiChatGraphicsItem::setBoxStartLength( int value ) { m_boxStartLength = value; }
inline int GuiChatGraphicsItem::boxStartLength() const { return m_boxStartLength; }
inline void GuiChatGraphicsItem::setViewWidth( int w ) { m_boxMaxWidth = w - m_boxStartLength - 25; } // 25 for scrollbar

#endif // BEEBEEP_GUICHATGRAPHICSITEM_H

