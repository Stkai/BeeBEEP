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

#include "EmoticonManager.h"


EmoticonManager* EmoticonManager::mp_instance = NULL;


EmoticonManager::EmoticonManager()
 : m_emoticons()
{
  addEmoticon( ":)", "smile" );
  addEmoticon( ":P", "tongue" );
  addEmoticon( ":'", "cry" );
  addEmoticon( ":D", "laugh" );
  addEmoticon( ":*", "kiss" );
  addEmoticon( ":z", "sleep" );
  addEmoticon( ":o", "surprise" );
  addEmoticon( ":|", "pouty" );
  addEmoticon( ":L", "love" );
  addEmoticon( ":w", "whistle" );
  addEmoticon( ":$", "bandit" );
  addEmoticon( ":!", "wizard" );
  addEmoticon( ";)", "wink" );
  addEmoticon( "B)", "cool" );
  addEmoticon( "<3", "heart" );
  addEmoticon( "=]", "sideways" );
  addEmoticon( "})", "devil" );
  addEmoticon( "o:", "angel" );
  addEmoticon( "x(", "sick" );
  addEmoticon( "X|", "pinched" );
}

void EmoticonManager::addEmoticon( const QString& e_text, const QString& e_name )
{
  m_emoticons.insert( e_text.at( 0 ), Emoticon( e_text, e_name ) );
}

Emoticon EmoticonManager::emoticon( const QString& e_text ) const
{
  if( e_text.size() > 1 )
  {
    QChar c = e_text.at( 0 );
    QList<Emoticon> emoticon_list = m_emoticons.values( c );
    QList<Emoticon>::const_iterator it = emoticon_list.begin();
    while( it != emoticon_list.end() )
    {
      if( (*it).textToMatch() == e_text )
        return *it;
      ++it;
    }
  }
  return Emoticon();
}

QString EmoticonManager::parseEmoticons( const QString& msg ) const
{
  QString s = "";
  QString text_to_match = "";
  QChar c;

  for( int pos = 0; pos < msg.size(); pos++ )
  {
    c = msg[ pos ];

    if( text_to_match.size() > 0 )
    {
      text_to_match += c;
      Emoticon e = emoticon( text_to_match );
      if( e.isValid() )
        s += e.toHtml();
      else
        s += text_to_match;
      text_to_match = "";
    }
    else if( m_emoticons.contains( c ) )
      text_to_match = c;
    else
      s += c;
  }

  if( text_to_match.size() > 0 )
    s += text_to_match;

  return s;
}
