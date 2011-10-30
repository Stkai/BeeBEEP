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
#include <QtDebug>
#include <QtCore/qmath.h>
#include "FacebookService.h"
#include "Version.h"


#undef COLOR_GRADIENT_DEBUG

const QString open_cmd = " ~";
const QString close_cmd = "~ ";


QString RainbowTextMarker::name() const
{
  return tr( "Rainbow Text Marker" );
}

QString RainbowTextMarker::version() const
{
  return "0.9b";
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

namespace
{

  QColor GetRainbowColor( qreal k, bool reverse_color )
  {
    qreal r = 127.0 + 127.0 * qCos( k - 0.5 );
    qreal g = 127.0 + 127.0 * qCos( k - 2.5 );
    qreal b = 127.0 + 127.0 * qCos( k - 4.5 );

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
    qreal min = qMin( r, qMin( g, b ) );
    // Subtract the minimum from all values so that the darkest becomes zero
    r -= min;
    g -= min;
    b -= min;

#if defined( COLOR_GRADIENT_DEBUG )
    qDebug() << "RGB color (satured):" << r << g << b;
#endif

    // Now find the highest value
    qreal max = qMax( r, qMax( g, b ) );

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
    qreal tekBright = 200.0;
    qreal tekContrast = 255.0;

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


bool RainbowTextMarker::parseText( QString* p_txt )
{
  qDebug() << "Plugin" << name() << "starts to parse the text";

  bool space_added_at_begin = false;
  bool space_added_at_end = false;

  if( p_txt->startsWith( open_cmd.trimmed() ) )
  {
    p_txt->prepend( QLatin1Char( ' ' ) );
    space_added_at_begin = true;
  }

  if( p_txt->endsWith( close_cmd.trimmed() ) )
  {
    p_txt->append( QLatin1Char( ' ' ) );
    space_added_at_end = true;
  }

  QString parsed_text = "";
  int index = p_txt->indexOf( open_cmd, 0, Qt::CaseInsensitive );

  if( index >= 0 && p_txt->size() > index )
  {
    int last_index = p_txt->indexOf( close_cmd, index+1, Qt::CaseInsensitive );
    if( last_index > 0 )
    {
      qDebug() << name() << "rainbow found. Create HTML tags";

      bool is_in_tag = false;
      QString code_text = "";
      QChar c;

      // Get the length and scale. For rainbows, the scale must be such that one cycle comes out to almost 2pi.
      qreal k;
      int rainbow_index = 0;
      qreal scale = 3.14 * (2.0-0.21) / qMax( 1, (last_index-index-open_cmd.size()) );

      for( int i = 0; i < p_txt->size(); i++ )
      {
        c = p_txt->at( i );
        if( c == QLatin1Char( '<' ) )
          is_in_tag = true;

        if( is_in_tag )
        {
          parsed_text.append( c );

          if( c == QLatin1Char( '>' ) )
            is_in_tag = false;

          continue;
        }

        if( c == QLatin1Char( '&' ) )
        {
          QChar c_tmp;
          // Search forward until either a semicolon, tag, or space is found
          for( int j=(i+1); j < p_txt->size(); j++ )
          {
            c_tmp = p_txt->at( j );
            if( c_tmp == QLatin1Char( '<' ) || c_tmp == QLatin1Char( '>' )
                || c_tmp == QLatin1Char( ';' ) || c_tmp.isSpace() )
            {
              if( c_tmp == QLatin1Char( ';' ) )
              {
                code_text = p_txt->mid( i, j-i+1 );
                qDebug() << "Code text found" << code_text << "... skip it";
              }
              break;
            }
          }
        }

        if( i >= index && i < (index+open_cmd.size()) )
        {
          // skip open command
          if( i == index )
          {
            rainbow_index = 1;
            parsed_text.append( " " );
          }
        }
        else if( i >= (index+open_cmd.size()) && i < last_index )
        {
          k = scale * rainbow_index;
          QString str_to_append = QString( "<font color=""%1"">" ).arg( GetRainbowColor( k, false ).name() );
          if( code_text.size() > 0 )
          {
            str_to_append += code_text;
            i += code_text.size() - 1;
            code_text = "";
          }
          else
            str_to_append.append( c );
          str_to_append.append( "</font>" );
          parsed_text.append( str_to_append );
          rainbow_index++;
        }
        else if( i >= last_index && i < (last_index+close_cmd.size()) )
        {
          // skip close command
          if( i == last_index )
            parsed_text.append( " " );
        }
        else
          parsed_text.append( c );
      }

      if( parsed_text.contains( open_cmd ) )
        parseText( &parsed_text );
    }
  }

  if( !parsed_text.isEmpty() )
    *p_txt = parsed_text;

  if( space_added_at_begin )
    p_txt->remove( 0, 1 );
  if( space_added_at_end )
    p_txt->chop( 1 );
  return true;
}

RainbowTextMarker::RainbowTextMarker()
  : QObject()
{
  setEnabled( true );
  qDebug() << "RainbowTextMarker plugin loaded";
}

Q_EXPORT_PLUGIN2( beefacebook, FacebookService )
