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

#ifndef BEEBEEP_COLORMANAGER_H
#define BEEBEEP_COLORMANAGER_H

#include "Config.h"


class ColorManager
{
// Singleton Object
  static ColorManager* mp_instance;

public:
  QString randomQString();
  inline QColor randomQColor();
  QString unselectedQString();
  inline QColor unselectedQColor();
  void setColorSelected( const QString& );
  inline void clearSelectedColors();
  bool isValidColor( const QString& );

  static ColorManager& instance()
  {
    if( !mp_instance )
      mp_instance = new ColorManager();
    return *mp_instance;
  }

  static void close()
  {
    if( mp_instance )
    {
      delete mp_instance;
      mp_instance = NULL;
    }
  }

protected:
  ColorManager();

private:
  QStringList m_colors;
  QStringList m_unselectedColors;

};


// Inline Function
inline QColor ColorManager::randomQColor() { return QColor( randomQString() ); }
inline QColor ColorManager::unselectedQColor() { return QColor( unselectedQString() ); }
inline void ColorManager::clearSelectedColors() { m_unselectedColors = m_colors; }

#endif // BEEBEEP_COLORMANAGER_H
