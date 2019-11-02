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
// $Id: NumberTextMarker.cpp 101 2011-09-19 11:59:48Z mastroddi $
//
//////////////////////////////////////////////////////////////////////

#include <QtPlugin>
#include <QtDebug>
#include <QtCore/qmath.h>
#include "NumberTextMarker.h"
#include "Version.h"


QString NumberTextMarker::name() const
{
  return tr( "Number Text Marker" );
}

QString NumberTextMarker::version() const
{
  return "0.9.7";
}

QString NumberTextMarker::author() const
{
  return "Marco Mastroddi";
}

QString NumberTextMarker::help() const
{
  return tr( "If you want to encode your message with numbers write a #text to encode# ." );
}

QIcon NumberTextMarker::icon() const
{
  return QIcon( iconFileName() );
}

QString NumberTextMarker::iconFileName() const
{
  return QLatin1String( ":/plugins/number.png" );
}

int NumberTextMarker::priority() const
{
  return 1000;
}

QString NumberTextMarker::coreVersion() const
{
  return QString( BEEBEEP_VERSION );
}

bool NumberTextMarker::parseBeforeSending() const
{
  return true;
}

namespace
{

  QChar GetCharToNumber( const QChar& c_to_parse )
  {
    char c = c_to_parse.toLower().toLatin1();
    switch( c )
    {
    case 'a':
      return '4';
    case 'z':
      return '2';
    case 'o':
      return '0';
    case 'e':
      return '3';
    case 'i':
      return '1';
    case 's':
      return '5';
    case 't':
      return '7';
    default:
      return c_to_parse;
    }
  }

} // end of namespace


QString NumberTextMarker::openCommand() const
{
  return QLatin1String( " #" );
}

QString NumberTextMarker::closeCommand() const
{
  return QLatin1String( "# " );
}

QString NumberTextMarker::openString() const
{
  return QLatin1String( " " );
}

QString NumberTextMarker::closeString() const
{
  return QLatin1String( " " );
}

void NumberTextMarker::initParser( const QString& )
{
  // do nothing
}

QString NumberTextMarker::parseString( const QString& str )
{
  if( str.size() == 1 )
    return GetCharToNumber( str.at( 0 ) );
  else
    return str;
}

NumberTextMarker::NumberTextMarker()
  : QObject()
{
  setEnabled( true );
  qDebug() << "NumberTextMarker plugin loaded";
}

#if QT_VERSION >= 0x050000
Q_PLUGIN_METADATA(IID "beebeep.plugin.TextMarkerInterface/2.0")
#else
Q_EXPORT_PLUGIN2( numbertextmarker, NumberTextMarker )
#endif
