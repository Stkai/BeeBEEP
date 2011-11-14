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
// $Id: NumberTextMarker.cpp 101 2011-09-19 11:59:48Z mastroddi $
//
//////////////////////////////////////////////////////////////////////

#include <QtPlugin>
#include <QtDebug>
#include <QtCore/qmath.h>
#include "NumberTextMarker.h"
#include "Version.h"


const QString open_cmd = " #";
const QString close_cmd = "# ";


QString NumberTextMarker::name() const
{
  return tr( "Number Text Marker" );
}

QString NumberTextMarker::version() const
{
  return "0.9b";
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


bool NumberTextMarker::parseText( QString* p_txt )
{
  qDebug() << "Plugin" << name() << "starts to parse the text";

  bool space_added_at_begin = false;
  bool space_added_at_end = false;
  QString parsed_text = "";

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

  int index = p_txt->indexOf( open_cmd, 0, Qt::CaseInsensitive );

  if( index >= 0 && p_txt->size() > index )
  {
    int last_index = p_txt->indexOf( close_cmd, index+1, Qt::CaseInsensitive );
    if( last_index > 0 )
    {
      qDebug() << name() << "string to encode found. Replace chars with numbers";

      bool is_in_tag = false;
      QString code_text = "";
      QString str_to_append;
      QChar c; 

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
            parsed_text.append( " " );
          }
        }
        else if( i >= (index+open_cmd.size()) && i < last_index )
        {
          QString str_to_append = "";
          if( code_text.size() > 0 )
          {
            str_to_append += code_text;
            i += code_text.size() - 1;
            code_text = "";
          }
          else
            str_to_append.append( GetCharToNumber( c ) );
          parsed_text.append( str_to_append );
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

NumberTextMarker::NumberTextMarker()
  : QObject()
{
  setEnabled( true );
  qDebug() << "NumberTextMarker plugin loaded";
}

Q_EXPORT_PLUGIN2( numbertextmarker, NumberTextMarker )
