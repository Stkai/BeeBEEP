//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2019 Marco Mastroddi
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

#include "GuiAudioLevel.h"


GuiAudioLevel::GuiAudioLevel( QWidget *parent )
  : QWidget(parent), m_level( 0.0 )
{
  setMinimumHeight( 18 );
  setMaximumHeight( 32 );
  setMinimumWidth( 200 );
}

void GuiAudioLevel::setLevel( qreal level )
{
  if( m_level == level )
   return;
  m_level = level;
  update();
}

void GuiAudioLevel::paintEvent( QPaintEvent* event )
{
  Q_UNUSED(event);
  QPainter painter(this);
  qreal widthLevel = m_level * width();
  painter.fillRect( 0, 0, widthLevel, height(), Qt::darkRed );
  painter.fillRect( widthLevel, 0, width(), height(), Qt::gray );
}
