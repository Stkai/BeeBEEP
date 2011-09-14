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

#ifndef RAINBOWTEXTMARKER_H
#define RAINBOWTEXTMARKER_H

#include "rainbowtextmarker_global.h"
#include "Interfaces.h"
#include <QObject>
#include <QStringList>


class RAINBOWTEXTMARKERSHARED_EXPORT RainbowTextMarker : public QObject, public TextMarkerInterface
{
  Q_OBJECT
  Q_INTERFACES( TextMarkerInterface )

public:
  RainbowTextMarker();

  QString name() const;
  QString version() const;
  QString help() const;
  QString author() const;
  QString parseText( QString );

private:
  QStringList m_colors;

};

#endif // RAINBOWTEXTMARKER_H
