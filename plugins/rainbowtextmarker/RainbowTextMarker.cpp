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
#include "RainbowTextMarker.h"
#include "Random.h"

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

QString RainbowTextMarker::parseText( QString txt )
{
  qDebug() << name() << "parsing text";
  txt.prepend( QLatin1Char( ' ' ) );
  txt.append( QLatin1Char( ' ' ) );
  QString parsed_text = "";
  int index = txt.indexOf( open_cmd, 0, Qt::CaseInsensitive );

  if( index >= 0 && txt.size() > index )
  {
    int color_index = Random::number( 0, (m_colors.size()-1) );

    qDebug() << name() << "has found open tag" << open_cmd;

    int last_index = txt.indexOf( close_cmd, index+1, Qt::CaseInsensitive );
    if( last_index > 0 )
    {
      qDebug() << name() << "has found close tag" << close_cmd;
      for( int i = 0; i < txt.size(); i++ )
      {
        if( i >= index && i < (index+open_cmd.size()) )
        {
          // skip open command
        }
        else if( i >= (index+open_cmd.size()) && i < last_index )
        {
          // rainbow
          parsed_text.append( QString( "<font color=%1>%2</font>" ).arg( m_colors.at( color_index ), txt.at( i ) ) );
          color_index++;
          if( color_index >= m_colors.size() )
            color_index = 0;
        }
        else if( i >= last_index && i < (last_index+close_cmd.size()) )
        {
          // skip close command
        }
        else
          parsed_text.append( txt.at( i ) );
      }

      if( parsed_text.contains( open_cmd ) )
        parsed_text = parseText( parsed_text );
    }
  }

  if( parsed_text.isEmpty() )
    parsed_text = txt;
  parsed_text.remove( 0, 1 );
  parsed_text.chop( 1 );
  qDebug() << name() << "text parsed";
  return parsed_text;
}

RainbowTextMarker::RainbowTextMarker()
  : QObject()
{
  m_colors << "#151B8D";
  m_colors << "#15317E";
  m_colors << "#342D7E";
  m_colors << "#2B60DE";
  m_colors << "#306EFF";
  m_colors << "#2B65EC";
  m_colors << "#2554C7";
  m_colors << "#3BB9FF";
  m_colors << "#38ACEC";
  m_colors << "#357EC7";
  m_colors << "#3090C7";
  m_colors << "#25587E";
  m_colors << "#1589FF";
  m_colors << "#157DEC";
  m_colors << "#1569C7";
  m_colors << "#153E7E";
  m_colors << "#2B547E";
  m_colors << "#4863A0";
  m_colors << "#6960EC";
  m_colors << "#8D38C9";
  m_colors << "#7A5DC7";
  m_colors << "#8467D7";
  m_colors << "#9172EC";
  m_colors << "#9E7BFF";
  m_colors << "#728FCE";
  m_colors << "#488AC7";
  m_colors << "#56A5EC";
  m_colors << "#5CB3FF";
  m_colors << "#659EC7";
  m_colors << "#41627E";
  m_colors << "#737CA1";
  m_colors << "#737CA1";
  m_colors << "#98AFC7";
  m_colors << "#F6358A";
  m_colors << "#F6358A";
  m_colors << "#E4317F";
  m_colors << "#F52887";
  m_colors << "#E4287C";
  m_colors << "#C12267";
  m_colors << "#7D053F";
  m_colors << "#CA226B";
  m_colors << "#C12869";
  m_colors << "#800517";
  m_colors << "#7D0541";
  m_colors << "#7D0552";
  m_colors << "#810541";
  m_colors << "#C12283";
  m_colors << "#E3319D";
  m_colors << "#F535AA";
  m_colors << "#FF00FF";
  m_colors << "#F433FF";
  m_colors << "#E238EC";
  m_colors << "#C031C7";
  m_colors << "#B048B5";
  m_colors << "#D462FF";
  m_colors << "#C45AEC";
  m_colors << "#A74AC7";
  m_colors << "#6A287E";
  m_colors << "#8E35EF";
  m_colors << "#893BFF";
  m_colors << "#7F38EC";
  m_colors << "#6C2DC7";
  m_colors << "#461B7E";
  m_colors << "#571B7e";
  m_colors << "#7D1B7E";
  m_colors << "#842DCE";
  m_colors << "#8B31C7";
  m_colors << "#A23BEC";
  m_colors << "#B041FF";
  m_colors << "#7E587E";
  m_colors << "#D16587";
  m_colors << "#F778A1";
  m_colors << "#E56E94";
  m_colors << "#C25A7C";
  m_colors << "#7E354D";
  m_colors << "#B93B8F";
  m_colors << "#F9B7FF";
  m_colors << "#E6A9EC";
  m_colors << "#C38EC7";
  m_colors << "#D2B9D3";
  m_colors << "#E3E4FA";
  m_colors << "#C6DEFF";
  m_colors << "#ADDFFF";
  m_colors << "#BDEDFF";
  m_colors << "#E0FFFF";
  m_colors << "#C2DFFF";
  m_colors << "#B4CFEC";
  m_colors << "#B7CEEC";
  m_colors << "#52F3FF";
  m_colors << "#00FFFF";
  m_colors << "#57FEFF";
  m_colors << "#50EBEC";
  m_colors << "#4EE2EC";
  m_colors << "#48CCCD";
  m_colors << "#43C6DB";
  m_colors << "#9AFEFF";
  m_colors << "#8EEBEC";
  m_colors << "#78c7c7";
  m_colors << "#46C7C7";
  m_colors << "#43BFC7";
  m_colors << "#77BFC7";
  m_colors << "#92C7C7";
  m_colors << "#AFDCEC";
  m_colors << "#3B9C9C";
  m_colors << "#307D7E";
  m_colors << "#3EA99F";
  m_colors << "#82CAFA";
  m_colors << "#A0CFEC";
  m_colors << "#87AFC7";
  m_colors << "#82CAFF";
  m_colors << "#79BAEC";
  m_colors << "#566D7E";
  m_colors << "#6698FF";
  m_colors << "#736AFF";
  m_colors << "#CFECEC";
  m_colors << "#AFC7C7";
  m_colors << "#717D7D";
  m_colors << "#95B9C7";
  m_colors << "#5E767E";
  m_colors << "#5E7D7E";
  m_colors << "#617C58";
  m_colors << "#348781";
  m_colors << "#306754";
  m_colors << "#4E8975";
  m_colors << "#254117";
  m_colors << "#387C44";
  m_colors << "#4E9258";
  m_colors << "#347235";
  m_colors << "#347C2C";
  m_colors << "#667C26";
  m_colors << "#437C17";
  m_colors << "#347C17";
  m_colors << "#348017";
  m_colors << "#4AA02C";
  m_colors << "#41A317";
  m_colors << "#4AA02C";
  m_colors << "#8BB381";
  m_colors << "#99C68E";
  m_colors << "#4CC417";
  m_colors << "#6CC417";
  m_colors << "#52D017";
  m_colors << "#4CC552";
  m_colors << "#54C571";
  m_colors << "#57E964";
  m_colors << "#5EFB6E";
  m_colors << "#64E986";
  m_colors << "#6AFB92";
  m_colors << "#B5EAAA";
  m_colors << "#C3FDB8";
  m_colors << "#00FF00";
  m_colors << "#87F717";
  m_colors << "#5FFB17";
  m_colors << "#59E817";
  m_colors << "#7FE817";
  m_colors << "#8AFB17";
  m_colors << "#B1FB17";
  m_colors << "#CCFB5D";
  m_colors << "#BCE954";
  m_colors << "#A0C544";
  m_colors << "#FFFF00";
  m_colors << "#FFF380";
  m_colors << "#EDE275";
  m_colors << "#EDDA74";
  m_colors << "#EAC117";
  m_colors << "#FDD017";
  m_colors << "#FBB917";
  m_colors << "#E9AB17";
  m_colors << "#D4A017";
  m_colors << "#C7A317";
  m_colors << "#C68E17";
  m_colors << "#AF7817";
  m_colors << "#ADA96E";
  m_colors << "#C9BE62";
  m_colors << "#827839";
  m_colors << "#FBB117";
  m_colors << "#E8A317";
  m_colors << "#C58917";
  m_colors << "#F87431";
  m_colors << "#E66C2C";
  m_colors << "#F88017";
  m_colors << "#F87217";
  m_colors << "#E56717";
  m_colors << "#C35617";
  m_colors << "#C35817";
  m_colors << "#8A4117";
  m_colors << "#7E3517";
  m_colors << "#7E2217";
  m_colors << "#7E3117";
  m_colors << "#7E3817";
  m_colors << "#7F5217";
  m_colors << "#806517";
  m_colors << "#805817";
  m_colors << "#7F462C";
  m_colors << "#C85A17";
  m_colors << "#C34A2C";
  m_colors << "#E55B3C";
  m_colors << "#F76541";
  m_colors << "#E18B6B";
  m_colors << "#F88158";
  m_colors << "#E67451";
  m_colors << "#C36241";
  m_colors << "#C47451";
  m_colors << "#E78A61";
  m_colors << "#F9966B";
  m_colors << "#EE9A4D";
  m_colors << "#F660AB";
  m_colors << "#F665AB";
  m_colors << "#E45E9D";
  m_colors << "#C25283";
  m_colors << "#7D2252";
  m_colors << "#E77471";
  m_colors << "#F75D59";
  m_colors << "#E55451";
  m_colors << "#C24641";
  m_colors << "#FF0000";
  m_colors << "#F62217";
  m_colors << "#E41B17";
  m_colors << "#F62817";
  m_colors << "#E42217";
  m_colors << "#C11B17";
  m_colors << "#FAAFBE";
  m_colors << "#FBBBB9";
  m_colors << "#E8ADAA";
  m_colors << "#E7A1B0";
  m_colors << "#FAAFBA";
  m_colors << "#F9A7B0";
  m_colors << "#E799A3";
  m_colors << "#C48793";
  m_colors << "#C5908E";
  m_colors << "#B38481";
  m_colors << "#C48189";
  m_colors << "#7F5A58";
  m_colors << "#7F4E52";
  m_colors << "#7F525D";
  m_colors << "#817679";
  m_colors << "#817339";
  m_colors << "#827B60";

  qDebug() << name() << "has" << m_colors.size() << "colors to make a rainbow";
}

Q_EXPORT_PLUGIN2( rainbowtextmarker, RainbowTextMarker )
