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
// $Id: RegularBoldTextMarker.h 346 2015-04-05 16:12:37Z mastroddi $
//
//////////////////////////////////////////////////////////////////////

#ifndef REGULARBOLDTEXTMARKER_H
#define REGULARBOLDTEXTMARKER_H

#include "regularboldtextmarker_global.h"
#include "Interfaces.h"
#include <QObject>
#include <QStringList>


class REGULARBOLDTEXTMARKERSHARED_EXPORT RegularBoldTextMarker : public QObject, public TextMarkerInterface
{
  Q_OBJECT
  Q_INTERFACES( TextMarkerInterface )
#if QT_VERSION >= 0x050000
  Q_PLUGIN_METADATA(IID "beebeep.plugin.TextMarkerInterface/2.0")
#endif
public:
  RegularBoldTextMarker();

  QString name() const;
  QString version() const;
  QString help() const;
  QString author() const;
  QIcon icon() const;
  QString iconFileName() const;
  int priority() const;
  QString coreVersion() const;

  bool parseBeforeSending() const;
  QString openCommand() const;
  QString closeCommand() const;
  QString openString() const;
  QString closeString() const;
  void initParser( const QString& );
  QString parseString( const QString& );

private:
  bool m_isBoldActive;
  bool m_lastCharIsSpace;

};

#endif // REGULARBOLDTEXTMARKER_H
