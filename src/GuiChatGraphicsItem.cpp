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

#include "ChatMessage.h"
#include "GuiChatGraphicsItem.h"
#include "UserManager.h"
#include <QPainter>
#include <QTextDocument>
#include <QTime>


QLinearGradient getGradient(const QColor &col, const QRectF &rect)
{
    QLinearGradient g(rect.topLeft(), rect.bottomLeft());

    qreal hue = col.hueF();
    qreal value = col.valueF();
    qreal saturation = col.saturationF();

    QColor c = col;
    c.setHsvF(hue, 0.42 * saturation, 0.98 * value);
    g.setColorAt(0, c);
    c.setHsvF(hue, 0.58 * saturation, 0.95 * value);
    g.setColorAt(0.25, c);
    c.setHsvF(hue, 0.70 * saturation, 0.93 * value);
    g.setColorAt(0.5, c);

    c.setHsvF(hue, 0.95 * saturation, 0.9 * value);
    g.setColorAt(0.501, c);
    c.setHsvF(hue * 0.95, 0.95 * saturation, 0.95 * value);
    g.setColorAt(0.75, c);
    c.setHsvF(hue * 0.90, 0.95 * saturation, 1 * value);
    g.setColorAt(1.0, c);

    return g;
}

QLinearGradient darken(const QLinearGradient &gradient)
{
    QGradientStops stops = gradient.stops();
    for (int i = 0; i < stops.size(); ++i) {
        QColor color = stops.at(i).second;
        stops[i].second = color.darker(160);
    }

    QLinearGradient g = gradient;
    g.setStops(stops);
    return g;
}

static void drawPath(QPainter *p, const QPainterPath &path, const QColor &col, bool dark = false)
{
    const QRectF pathRect = path.boundingRect();

    const QLinearGradient baseGradient = getGradient(col, pathRect);
    const QLinearGradient darkGradient = darken(baseGradient);

    p->save();

   // p->setOpacity(0.25);

    //glow
//    if (dark)
//        p->strokePath(path, QPen(darkGradient, 6));
//    else
//        p->strokePath(path, QPen(baseGradient, 6));

    p->setOpacity(1.0);

    //fill
    if (dark)
        p->fillPath(path, darkGradient);
    else
        p->fillPath(path, baseGradient);

    QLinearGradient g(pathRect.topLeft(), pathRect.topRight());
    g.setCoordinateMode(QGradient::ObjectBoundingMode);

    p->setOpacity(0.2);
    p->fillPath(path, g);

    p->setOpacity(0.5);

    // highlight
//    if (dark)
//        p->strokePath(path, QPen(col.lighter(160).darker(160), 2));
//    else
//        p->strokePath(path, QPen(col.lighter(160), 2));

    p->setOpacity(1.0);

    p->restore();
}

GuiChatGraphicsItem::GuiChatGraphicsItem( QGraphicsItem* parent )
  : QGraphicsPathItem( parent ), m_spikeWidth(9), m_spikeHeight(6), m_cornerRadius(10), m_textSpacing(4), m_color(Qt::yellow)
{
  createPath();
//    setFlags(QGraphicsItem::ItemIsMovable);

    QFont font;
    QFontMetrics fm(font);
    m_timeStampWidth = fm.width( QString( "hh:mm" ) ) + 4;
}

void GuiChatGraphicsItem::paint( QPainter *p, const QStyleOptionGraphicsItem*, QWidget* )
{
  p->setRenderHint( QPainter::Antialiasing );
  drawPath( p, path(), Qt::darkRed ); // FIXME: color!

  QFont font;
  font.setBold( true );

  QTextDocument text_doc( m_text );
  QTextOption text_opt;
  text_opt.setWrapMode( QTextOption::WrapAnywhere );
  text_opt.setAlignment( Qt::AlignLeft );
  text_doc.setDefaultTextOption( text_opt );
  text_doc.setTextWidth( boxTextWidth() );
  text_doc.setDefaultFont( font );

  p->setPen( Qt::darkGreen );
  p->setFont( font );

  int text_height = (int) text_doc.size().height();

  p->drawText( m_spikeWidth + m_cornerRadius, 4, boxTextWidth(), text_height, Qt::AlignLeft|Qt::TextWrapAnywhere, m_text );
  p->setPen(Qt::black);


  p->setFont( font );
  p->drawText( -m_boxStartLength, 0, m_boxStartLength, m_boxHeight, Qt::AlignRight|Qt::AlignBottom, m_name );
  font.setBold( false );
  p->setPen( Qt::gray );
  p->setFont( font );

  int time_width;
  if( m_timeStampWidth > m_boxStartLength )
    time_width = m_timeStampWidth;
  else
    time_width = m_boxStartLength;

  p->drawText( m_boxMaxWidth + 6, 0, time_width - 6, m_boxHeight, Qt::AlignBottom|Qt::AlignLeft, m_timeStamp );
}

void GuiChatGraphicsItem::setText( const QString& txt )
{
  m_text = txt;
  createPath();
}

void GuiChatGraphicsItem::setBoxMaxWidth( int w )
{
  m_boxMaxWidth = w;
  createPath();
}

void GuiChatGraphicsItem::createPath()
{
  calculateBoxWidth();
  int spike_x = m_spikeWidth;
  int spike_y = m_spikeHeight;
  int corner = m_cornerRadius;
  int length = m_boxWidth - spike_x;
  int offset = spike_x;

  QPainterPath box_path;
  box_path.moveTo(0 + offset, m_boxHeight - corner);
  QRectF rect(offset - 2*spike_x, m_boxHeight - corner - spike_y, 2*spike_x, 2*spike_y);
  box_path.arcMoveTo(rect, -90.0);
  box_path.arcTo(rect, 270, 90.0);
  box_path.lineTo(0 + offset, corner);
  box_path.arcTo(0 + offset, 0, 2*corner, 2*corner, 180, -90.0);
  box_path.lineTo(length - corner, 0);
  box_path.arcTo(length + offset - corner*2, 0, 2*corner, 2*corner, 90, -90.0);
  box_path.lineTo(length + offset, m_boxHeight - corner);
  box_path.arcTo(length + offset - corner*2, m_boxHeight - 2*corner, 2*corner, 2*corner, 0, -90.0);
  box_path.lineTo(offset + corner, m_boxHeight);
  box_path.arcTo(offset, m_boxHeight - 2*corner, 2*corner, 2*corner, 270, -45.0);
  box_path.closeSubpath();

  setPath( box_path );
}

void GuiChatGraphicsItem::calculateBoxWidth()
{
  QFont font;
  font.setBold( true );
  QTextDocument text_doc( m_text );
  text_doc.setDefaultFont( font );
  int ideal_width = (int)text_doc.size().width();
  text_doc.setTextWidth( boxTextWidth() );
  m_boxHeight = (int)text_doc.size().height();

  if( ideal_width < boxTextWidth() )
    m_boxWidth = ideal_width + m_spikeWidth + m_cornerRadius;
  else
    m_boxWidth = m_boxMaxWidth;
}

void GuiChatGraphicsItem::setChatMessage( const ChatMessage& cm )
{
  if( cm.isSystem() )
  {
    m_name = "Bee";
    m_color = QColor( 255, 165, 0 );
        m_color = QColor( 255, 222, 173 );
  }
  else if( cm.isFromLocalUser() )
  {
    m_name = "Me";
    m_color = QColor( 211, 211, 211 );
  }
  else
  {
    User u = UserManager::instance().userList().find( cm.userId() );
    m_name = u.name();
    m_color = QColor( 255, 222, 173 );
  }

  qDebug() << "Box for:" << cm.message().text();

  setText( cm.message().text() );
}

QRectF GuiChatGraphicsItem::boundingRect() const
{
  QRectF rect = QGraphicsPathItem::boundingRect();
  rect.setLeft( -m_boxStartLength );

  int time_width;
  if( m_timeStampWidth > m_boxStartLength )
    time_width = m_timeStampWidth;
  else
    time_width = m_boxStartLength;
  rect.setRight( m_boxMaxWidth + time_width );

  return rect;
}
