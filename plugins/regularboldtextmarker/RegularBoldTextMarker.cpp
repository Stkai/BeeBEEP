//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2020 Marco Mastroddi
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
// $Id: RegularBoldTextMarker.cpp 346 2015-04-05 16:12:37Z mastroddi $
//
//////////////////////////////////////////////////////////////////////

#include <QtPlugin>
#include <QtDebug>
#include "RegularBoldTextMarker.h"
#include "Version.h"


QString RegularBoldTextMarker::name() const
{
  return tr( "Regular Bold Text Marker" );
}

QString RegularBoldTextMarker::version() const
{
  return "0.9.7";
}

QString RegularBoldTextMarker::author() const
{
  return "Marco Mastroddi";
}

QString RegularBoldTextMarker::help() const
{
  return tr( "If you want to format your message with words in regular and bold font write a [text to format] ." );
}

QIcon RegularBoldTextMarker::icon() const
{
  return QIcon( iconFileName() );
}

QString RegularBoldTextMarker::iconFileName() const
{
  return QLatin1String( ":/plugins/regularbold.png" );
}

int RegularBoldTextMarker::priority() const
{
  return 1000;
}

QString RegularBoldTextMarker::coreVersion() const
{
  return QString( BEEBEEP_VERSION );
}

bool RegularBoldTextMarker::parseBeforeSending() const
{
  return false;
}

QString RegularBoldTextMarker::openCommand() const
{
  return QLatin1String( " [" );
}

QString RegularBoldTextMarker::closeCommand() const
{
  return QLatin1String( "] " );
}

QString RegularBoldTextMarker::openString() const
{
  return QLatin1String( " <b>" );
}

QString RegularBoldTextMarker::closeString() const
{
  return m_isBoldActive ? QLatin1String( "</b> " ) : QLatin1String( " " );
}

void RegularBoldTextMarker::initParser( const QString& )
{
  m_isBoldActive = true;
  m_lastCharIsSpace = false;
}

QString RegularBoldTextMarker::parseString( const QString& str )
{
  if( str.size() == 1 && str.at( 0 ).isSpace() )
  {
    if( m_lastCharIsSpace )
      return QLatin1String( "" );

    m_lastCharIsSpace = true;
    m_isBoldActive = !m_isBoldActive;

    return m_isBoldActive ? QLatin1String( "<b>" ) : QLatin1String( "</b>" ); // inverted due the code above
  }
  else
  {
    m_lastCharIsSpace = false;
    return str;
  }
}

RegularBoldTextMarker::RegularBoldTextMarker()
  : QObject()
{
  setEnabled( true );
  m_isBoldActive = false;
  m_lastCharIsSpace = false;
  qDebug() << "RegularBoldTextMarker plugin loaded";
}

#if QT_VERSION >= 0x050000
Q_PLUGIN_METADATA(IID "beebeep.plugin.TextMarkerInterface/2.0")
#else
Q_EXPORT_PLUGIN2( regularboldtextmarker, RegularBoldTextMarker )
#endif
