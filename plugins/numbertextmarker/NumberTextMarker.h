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
// $Id: RainbowTextMarker.h 101 2011-09-19 11:59:48Z mastroddi $
//
//////////////////////////////////////////////////////////////////////

#ifndef NUMBERTEXTMARKER_H
#define NUMBERTEXTMARKER_H

#include "numbertextmarker_global.h"
#include "Interfaces.h"
#include <QObject>
#include <QStringList>


class NUMBERTEXTMARKERSHARED_EXPORT NumberTextMarker : public QObject, public TextMarkerInterface
{
  Q_OBJECT
  Q_INTERFACES( TextMarkerInterface )

public:
  NumberTextMarker();

  QString name() const;
  QString version() const;
  QString help() const;
  QString author() const;
  QIcon icon() const;
  QString iconFileName() const;
  int priority() const;
  QString coreVersion() const;

  bool parseBeforeSending() const;
  bool parseText( QString* );

};

#endif // RAINBOWTEXTMARKER_H
