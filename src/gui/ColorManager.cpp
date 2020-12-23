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

#include "ColorManager.h"
#include "Random.h"


ColorManager* ColorManager::mp_instance = NULL;


QString ColorManager::randomQString()
{
  return m_colors.at( Random::number32( 0, (m_colors.size()-1) ) );
}

QString ColorManager::unselectedQString()
{
  if( m_unselectedColors.size() < 1 )
    clearSelectedColors();
  return m_unselectedColors.takeAt( Random::number32( 0, (m_unselectedColors.size()-1) ) );
}

void ColorManager::setColorSelected( const QString& s_color )
{
  m_unselectedColors.removeOne( s_color );
}

bool ColorManager::isValidColor( const QString& s_color )
{
#if QT_VERSION >= 0x040700
  if( s_color != QString( "#000000" ) && QColor::isValidColor( s_color ) )
#else
  if( s_color != QString( "#000000" ) && QColor( s_color ).isValid() )
#endif
    return true;
  else
    return false;
}

ColorManager::ColorManager()
  : m_colors(), m_unselectedColors()
{
  m_colors << "#400000";
  m_colors << "#404000";
  m_colors << "#800000";
  m_colors << "#804000";
  m_colors << "#804040";
  m_colors << "#808000";
  m_colors << "#808040";
  m_colors << "#C00000";
  m_colors << "#C04000";
  m_colors << "#C04040";
  m_colors << "#C08000";
  m_colors << "#C08040";
  m_colors << "#C08080";
  m_colors << "#C0C000";
  m_colors << "#C0C040";
  m_colors << "#C0C080";
  m_colors << "#FF0000";
  m_colors << "#FF4000";
  m_colors << "#FF4040";
  m_colors << "#FF8000";
  m_colors << "#FF8040";
  m_colors << "#FF8080";
  m_colors << "#FFC000";
  m_colors << "#FFC040";
  m_colors << "#FFC080";
  m_colors << "#FFC0C0";
  m_colors << "#004000";
  m_colors << "#008000";
  m_colors << "#408000";
  m_colors << "#00C000";
  m_colors << "#40C000";
  m_colors << "#80C000";
  m_colors << "#00FF00";
  m_colors << "#80FF80";
  m_colors << "#40FFC0";
  m_colors << "#80FFC0";
  m_colors << "#004040";
  m_colors << "#008080";
  m_colors << "#408080";
  m_colors << "#00C0C0";
  m_colors << "#40C0C0";
  m_colors << "#80C0C0";
  m_colors << "#000040";
  m_colors << "#000080";
  m_colors << "#004080";
  m_colors << "#404080";
  m_colors << "#0000C0";
  m_colors << "#0040C0";
  m_colors << "#4040C0";
  m_colors << "#0080C0";
  m_colors << "#4080C0";
  m_colors << "#8080C0";
  m_colors << "#0000FF";
  m_colors << "#0040FF";
  m_colors << "#4040FF";
  m_colors << "#0080FF";
  m_colors << "#4080FF";
  m_colors << "#8080FF";
  m_colors << "#00C0FF";
  m_colors << "#40C0FF";
  m_colors << "#80C0FF";
  m_colors << "#C0C0FF";
  m_colors << "#400040";
  m_colors << "#400080";
  m_colors << "#800080";
  m_colors << "#804080";
  m_colors << "#4000C0";
  m_colors << "#8000C0";
  m_colors << "#8040C0";
  m_colors << "#C000C0";
  m_colors << "#C040C0";
  m_colors << "#C080C0";
  m_colors << "#4000FF";
  m_colors << "#8000FF";
  m_colors << "#8040FF";
  m_colors << "#C000FF";
  m_colors << "#C040FF";
  m_colors << "#C080FF";
  m_colors << "#FF00FF";
  m_colors << "#FF40FF";
  m_colors << "#FF80FF";
  m_colors << "#FFC0FF";
  m_colors << "#202020";
  m_colors << "#404040";
  m_colors << "#606060";
  m_colors << "#808080";
  m_colors << "#A0A0A0";
  m_colors << "#C0C0C0";

  clearSelectedColors();
  qDebug() << "ColorManager loads" << m_colors.size() << "colors";
}


