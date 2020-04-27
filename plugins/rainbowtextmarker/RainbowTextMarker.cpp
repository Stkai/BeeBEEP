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
// $Id: RainbowTextMarker.cpp 346 2015-04-05 16:12:37Z mastroddi $
//
//////////////////////////////////////////////////////////////////////

#include <QtPlugin>
#include <QtDebug>
#include <QtCore/qmath.h>
#include <QTextDocument>
#include "RainbowTextMarker.h"
#include "Version.h"


#undef COLOR_GRADIENT_DEBUG


QString RainbowTextMarker::name() const
{
  return tr( "Rainbow Text Marker" );
}

QString RainbowTextMarker::version() const
{
  return "0.9.7";
}

QString RainbowTextMarker::author() const
{
  return "Marco Mastroddi";
}

QString RainbowTextMarker::help() const
{
  return tr( "If you want a <font color=#FF0000>r</font><font color=#FF8000>a</font><font color=#FFff00>i</font><font color=#7Fff00>n</font><font color=#00ff00>b</font><font color=#00ff80>o</font><font color=#00ffff>w</font><font color=#0080ff> </font><font color=#0000ff>t</font><font color=#7F00ff>e</font><font color=#FF00ff>x</font><font color=#FF0080>t</font> write a ~rainbow text~ ." );
}

QIcon RainbowTextMarker::icon() const
{
  return QIcon( iconFileName() );
}

QString RainbowTextMarker::iconFileName() const
{
  return QLatin1String( ":/plugins/rainbow.png" );
}

int RainbowTextMarker::priority() const
{
  return 1000;
}

QString RainbowTextMarker::coreVersion() const
{
  return QString( BEEBEEP_VERSION );
}

bool RainbowTextMarker::parseBeforeSending() const
{
  return false;
}

namespace
{

  QColor GetRainbowColor( double k, bool reverse_color )
  {
    double r = 127.0 + 127.0 * qCos( k - 0.5 );
    double g = 127.0 + 127.0 * qCos( k - 2.5 );
    double b = 127.0 + 127.0 * qCos( k - 4.5 );

    if( reverse_color )
    {
      r = 255.0 - r;
      g = 255.0 - g;
      b = 255.0 - b;
    }

#if defined( COLOR_GRADIENT_DEBUG )
    qDebug() << "RGB color:" << r << g << b;
#endif

    // Make fully saturated
    double min = qMin( r, qMin( g, b ) );
    // Subtract the minimum from all values so that the darkest becomes zero
    r -= min;
    g -= min;
    b -= min;

#if defined( COLOR_GRADIENT_DEBUG )
    qDebug() << "RGB color (satured):" << r << g << b;
#endif

    // Now find the highest value
    double max = qMax( r, qMax( g, b ) );

    // Scale the RGB values up so that the brightest equals the brightness input value.
    max = 255.0 / max;
    r *= max;
    g *= max;
    b *= max;

#if defined( COLOR_GRADIENT_DEBUG )
    qDebug() << "RGB color (brigth and contrast):" << r << g << b;
#endif

    // Adjust for brightness and contrast
    /*
    double tekBright = 200.0;
    double tekContrast = 255.0;

    max = (tekBright / 255.0) * (tekContrast / 255.0);
    min = (255.0 - tekContrast) * (tekBright / 255.0);

    r = r*max + min;
    g = g*max + min;
    b = b*max + min;
*/
    int color_r = qRound( qMax( 0.0, qMin( 255.0, r ) ) );
    int color_g = qRound( qMax( 0.0, qMin( 255.0, g ) ) );
    int color_b = qRound( qMax( 0.0, qMin( 255.0, b ) ) );

 #if defined( COLOR_GRADIENT_DEBUG )
    qDebug() << "RGB color (end):" << color_r << color_g << color_b;
#endif

    QColor color( color_r, color_g, color_b );
#if defined( COLOR_GRADIENT_DEBUG )
    qDebug() << "RGB color (end):" << color.name();
#endif
    return color.isValid() ? color : QColor( 0, 0, 0 );
  }

} // end of namespace


QString RainbowTextMarker::openCommand() const
{
  return QLatin1String( " ~" );
}

QString RainbowTextMarker::closeCommand() const
{
  return QLatin1String( "~ " );
}

QString RainbowTextMarker::openString() const
{
  return QLatin1String( " " );
}

QString RainbowTextMarker::closeString() const
{
  return QLatin1String( " " );
}

void RainbowTextMarker::initParser( const QString& str_to_parse )
{
  // Get the length and scale. For rainbows, the scale must be such that one cycle comes out to almost 2pi.
  m_rainbowIndex = 0;
  QTextDocument text_doc;
  text_doc.setHtml( str_to_parse );
  QString plain_text = text_doc.toPlainText();
  m_scale = 3.14 * (2.0-0.21) / qMax( 1, plain_text.size() );
}

QString RainbowTextMarker::parseString( const QString& str )
{
  if( str.size() == 1 && str.at( 0 ).isSpace() )
    return str;

  m_rainbowIndex++;
  double k = m_scale * m_rainbowIndex;
  return QString( "<font color=""%1"">%2</font>" ).arg( GetRainbowColor( k, false ).name(), str );
}

RainbowTextMarker::RainbowTextMarker()
  : QObject()
{
  setEnabled( true );
  qDebug() << "RainbowTextMarker plugin loaded";
}

#if QT_VERSION >= 0x050000
Q_PLUGIN_METADATA(IID "beebeep.plugin.TextMarkerInterface/2.0")
#else
Q_EXPORT_PLUGIN2( rainbowtextmarker, RainbowTextMarker )
#endif
