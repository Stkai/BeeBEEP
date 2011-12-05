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


#include "GuiChatGraphicsItem.h"
#include "ChatMessage.h"
#include "Settings.h"
#include "UserManager.h"


namespace
{

  QLinearGradient GetGradient( const QColor &col, const QRectF &rect )
  {
    QLinearGradient g( rect.topLeft(), rect.bottomLeft() );

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


  QLinearGradient Darken( const QLinearGradient &gradient )
  {
    QGradientStops stops = gradient.stops();
    for( int i = 0; i < stops.size(); ++i )
    {
      QColor color = stops.at(i).second;
      stops[i].second = color.darker(160);
    }

    QLinearGradient g = gradient;
    g.setStops(stops);
    return g;
  }

  void DrawPath( QPainter *p, const QPainterPath &path, const QColor &col,
                 const QString &name, int textWidth, bool dark = false )
  {
    const QRectF pathRect = path.boundingRect();

    const QLinearGradient baseGradient = GetGradient( col, pathRect );
    const QLinearGradient darkGradient = Darken( baseGradient );

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

} // end of namespace


GuiChatGraphicsItem::GuiChatGraphicsItem( QGraphicsItem* parent )
  : QGraphicsPathItem( parent ), m_spikeWidth( 9 ), m_spikeHeight( 6 ),
    m_cornerRadius( 10 ), m_textSpacing( 4 ), m_color( Qt::gray )
{

//    setFlags(QGraphicsItem::ItemIsMovable);

}

void GuiChatGraphicsItem::setChatMessage( const ChatMessage& cm )
{
  User u = UserManager::instance().userList().find( cm.userId() );
  if( u.isValid() )
    m_name = u.name();
  else
    m_name = QObject::tr( "Boh" );

  if( !u.isLocal() )
    m_color = u.color();

  m_text = cm.message().text();

  m_timeStamp = cm.message().timestamp().toString( "hh:mm" );
  QFont font;
  QFontMetrics fm( font );
  m_timeStampWidth = fm.width( m_timeStamp ) + 4;

  calculateWidth();
  setPath( createPath() );
}

void GuiChatGraphicsItem::paint( QPainter* p, const QStyleOptionGraphicsItem* opt, QWidget* w )
{
  p->setRenderHint( QPainter::Antialiasing );
  DrawPath( p, path(), m_color, m_text, textWidth());

  QFont font;
  font.setBold(true);
  QTextDocument textDoc( m_text );
  QTextOption textOpt;
  textOpt.setWrapMode( QTextOption::WrapAnywhere );
  textOpt.setAlignment( Qt::AlignLeft );
  textDoc.setDefaultTextOption( textOpt );
  textDoc.setTextWidth( textWidth() );
  textDoc.setDefaultFont( font );

  p->setPen( Qt::white );
  p->setFont( font );
  int h = (int) textDoc.size().height();
  p->drawText( m_spikeWidth + m_cornerRadius, 4, textWidth(), h, Qt::AlignLeft|Qt::TextWrapAnywhere, m_text );

  p->setPen(Qt::black);
  p->setFont(font);
  p->drawText( -m_boxStartLength, 0, m_boxStartLength, m_height, Qt::AlignRight|Qt::AlignBottom, m_name );

  font.setBold(false);
  p->setPen( Qt::gray );
  p->setFont( font );

  int time_width =  m_timeStampWidth > m_boxStartLength ? m_timeStampWidth : m_boxStartLength;

  p->drawText( maxWidth() + 6, 0, time_width - 6, m_height, Qt::AlignBottom|Qt::AlignLeft, m_timeStamp );
}

void GuiChatGraphicsItem::setMaxWidth( int w )
{
  m_maxWidth = w;
  setPath( createPath() );
}

QPainterPath GuiChatGraphicsItem::createPath()
{
  calculateWidth();
  int spike_x = m_spikeWidth;
  int spike_y = m_spikeHeight;
  int corner = m_cornerRadius;
  int length = m_width - spike_x;
  int offset = spike_x;

  QPainterPath message_box_path;
  message_box_path.moveTo( 0 + offset, m_height - corner );
  QRectF rect( offset - 2*spike_x, m_height - corner - spike_y, 2*spike_x, 2*spike_y );
  message_box_path.arcMoveTo( rect, -90.0 );
  message_box_path.arcTo( rect, 270, 90.0 );
  message_box_path.lineTo( 0 + offset, corner );
  message_box_path.arcTo( 0 + offset, 0, 2*corner, 2*corner, 180, -90.0 );
  message_box_path.lineTo( length - corner, 0 );
  message_box_path.arcTo( length + offset - corner*2, 0, 2*corner, 2*corner, 90, -90.0 );
  message_box_path.lineTo( length + offset, m_height - corner );
  message_box_path.arcTo( length + offset - corner*2, m_height - 2*corner, 2*corner, 2*corner, 0, -90.0 );
  message_box_path.lineTo( offset + corner, m_height );
  message_box_path.arcTo( offset, m_height - 2*corner, 2*corner, 2*corner, 270, -45.0 );
  message_box_path.closeSubpath();

  return message_box_path;
}

void GuiChatGraphicsItem::calculateWidth()
{
  QFont font;
  font.setBold(true);
  QTextDocument textDoc(m_text);
  textDoc.setDefaultFont(font);
  int idealWidth = (int)textDoc.size().width();
  textDoc.setTextWidth( textWidth());
  m_height = (int)textDoc.size().height();

  if(idealWidth < textWidth())
  {
    m_width = idealWidth + m_spikeWidth + m_cornerRadius;
  }
  else
    m_width = maxWidth();
}

QRectF GuiChatGraphicsItem::boundingRect() const
{
  QRectF rect = QGraphicsPathItem::boundingRect();
  rect.setLeft( -m_boxStartLength );
  rect.setRight( m_maxWidth + m_timeStampWidth );
  return rect;
}
