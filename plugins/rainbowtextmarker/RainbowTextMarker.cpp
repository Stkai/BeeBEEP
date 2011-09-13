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

#include <QtPlugin>
#include "RainbowTextMarker.h"


QString RainbowTextMarker::name() const
{
  return tr( "RainbowTextMarker" );
}

QString RainbowTextMarker::parseText( const QString& txt ) const
{
  QString parsed_text = txt;
  parsed_text.prepend( QString( "[%1]" ).arg( name() ) );
  return parsed_text;
}

Q_EXPORT_PLUGIN2( rainbowtextmarker, RainbowTextMarker );
