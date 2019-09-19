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
// Author: Marco Mastroddi <marco.mastroddi(AT)gmail.com>
//
// $Id$
//
//////////////////////////////////////////////////////////////////////

#include "GuiAudioLevel.h"


GuiAudioLevel::GuiAudioLevel( QWidget *parent )
  : QWidget(parent), m_level( 0.0 )
{
  setBackgroundRole( QPalette::Base );
  setAutoFillBackground( true );
  setMinimumHeight( 30 );
  setMinimumWidth( 200 );
}

void GuiAudioLevel::setLevel( qreal level )
{
  m_level = level;
  update();
}

void GuiAudioLevel::paintEvent( QPaintEvent* event )
{
  Q_UNUSED(event);
  QPainter painter( this );
  painter.setPen( Qt::black );
  painter.drawRect( QRect( painter.viewport().left()+10,
                           painter.viewport().top()+10,
                           painter.viewport().right()-20,
                           painter.viewport().bottom()-20
                         ) );
  if( m_level == 0.0 )
    return;

  int pos = ( ( painter.viewport().right() - 20 ) - ( painter.viewport().left() + 11 ) ) * m_level;
  painter.fillRect( painter.viewport().left()+11,
                    painter.viewport().top()+10,
                    pos,
                    painter.viewport().height()-21,
                    Qt::red
                  );
}
