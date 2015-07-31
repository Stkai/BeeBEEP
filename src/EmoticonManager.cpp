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
// Author: Marco Mastroddi <marco.mastroddi(AT)gmail.com>
//
// $Id$
//
//////////////////////////////////////////////////////////////////////

#include "EmoticonManager.h"


EmoticonManager* EmoticonManager::mp_instance = NULL;


EmoticonManager::EmoticonManager()
  : m_emoticons(), m_maxTextSize( 2 )
{
  addEmoticon( ":)", "smile" );
  addEmoticon( ":-)", "smile" );
  addEmoticon( ":p", "tongue" );
  addEmoticon( ":-p", "tongue" );
  addEmoticon( ":P", "big-tongue" );
  addEmoticon( ":-P", "big-tongue" );
  addEmoticon( ":'", "cry" );
  addEmoticon( ":'(", "cry" );
  addEmoticon( ":D", "laugh" );
  addEmoticon( ":-D", "laugh" );
  addEmoticon( ":@", "angry" );
  addEmoticon( ":x", "kiss" );
  addEmoticon( ":*", "big-kiss" );
  addEmoticon( ":-*", "big-kiss" );
  addEmoticon( ":z", "sleep" );
  addEmoticon( ":o", "surprise" );
  addEmoticon( ":O", "big-surprise" );
  addEmoticon( ":|", "pouty" );
  addEmoticon( ":L", "love" );
  addEmoticon( ":w", "whistle" );
  addEmoticon( ":$", "bandit" );
  addEmoticon( ":!", "wizard" );
  addEmoticon( ";)", "wink" );
  addEmoticon( ";-)", "wink" );
  addEmoticon( ":(", "disappointed" );
  addEmoticon( ":-(", "disappointed" );
  addEmoticon( ":T", "slurp" );
  addEmoticon( ":§", "bazinga" );
  addEmoticon( "B)", "cool" );
  addEmoticon( "B-)", "cool" );
  addEmoticon( "<3", "heart" );
  addEmoticon( "&lt;3", "heart" );  // for html
  addEmoticon( "</3", "heart-broken" );
  addEmoticon( "&lt;/3", "heart-broken" ); // for html
  addEmoticon( "=)", "sideways" );
  addEmoticon( "}:)", "devil" );
  addEmoticon( "o:)", "angel" );
  addEmoticon( "x(", "sick" );
  addEmoticon( "x-(", "sick" );
  addEmoticon( "X|", "pinched" );
  addEmoticon( "X-|", "pinched" );
  addEmoticon( "^_^", "happy" );
  addEmoticon( "o.O", "confused" );

#ifdef BEEBEEP_USE_EMOJI
  loadEmojis();
#endif

  qDebug() << "Emoticon manager loads" << emoticons( true ).size() << "images with" << m_emoticons.size() << "keys";
}

void EmoticonManager::addEmoticon( const QString& e_text, const QString& e_name )
{
  m_emoticons.insert( e_text.at( 0 ), Emoticon( e_text, e_name ) );
  if( e_text.size() > m_maxTextSize )
    m_maxTextSize = e_text.size();
}

static bool EmoticonForName( const Emoticon& e1, const Emoticon& e2 )
{
  return e1.name() < e2.name();
}

QList<Emoticon> EmoticonManager::emoticons( bool remove_duplicates ) const
{
  QList<Emoticon> emoticon_list;
  bool emoticon_to_add = false;
  QMultiHash<QChar, Emoticon>::const_iterator it = m_emoticons.begin();
  while( it != m_emoticons.end() )
  {
    if( !remove_duplicates )
      emoticon_list << *it;
    else
    {
      emoticon_to_add = true;
      QList<Emoticon>::iterator it2 = emoticon_list.begin();
      while( it2 != emoticon_list.end() )
      {
        if( (*it2).name() == (*it).name() )
        {
          emoticon_to_add = false;
          if( (*it2).textToMatch().size() > (*it).textToMatch().size() )
            *it2 = *it;
        }
        ++it2;
      }
      if( emoticon_to_add )
        emoticon_list << *it;
    }
    ++it;
  }

  qSort( emoticon_list.begin(), emoticon_list.end(), EmoticonForName );

  return emoticon_list;
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
      if( (*it).textToMatch().toLower() == e_text.toLower() )
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
  bool parse_emoticons = true;

  for( int pos = 0; pos < msg.size(); pos++ )
  {
    c = msg[ pos ];

    if( c.isSpace() )
    {
      if( text_to_match.size() > 0 )
      {
        s += text_to_match;
        text_to_match = "";
      }
      s += c;
      parse_emoticons = true;
    }
    else if( text_to_match.size() > 0 )
    {
      text_to_match += c;
      Emoticon e = emoticon( text_to_match );
      if( e.isValid() )
      {
        s += e.toHtml();
        text_to_match = "";
        parse_emoticons = true;
      }
      else
      {
        if( text_to_match.size() >= m_maxTextSize )
        {
          s += text_to_match;
          text_to_match = "";
          parse_emoticons = false;
        }
      }
    }
    else if( m_emoticons.contains( c ) )
    {
      if( parse_emoticons )
      {
        text_to_match = c;
        parse_emoticons = false;
      }
      else
        s += c;
    }
    else
    {
      s += c;
      parse_emoticons = false;
    }
  }

  if( text_to_match.size() > 0 )
    s += text_to_match;

  return s;
}

#ifdef BEEBEEP_USE_EMOJI
void EmoticonManager::loadEmojis()
{
  QFile emoji_list_file( "emoji_list.txt" );
  if( !emoji_list_file.open( QFile::ReadOnly ) )
    return;

  QString emoji_data = emoji_list_file.readAll();

  QStringList emoji_data_list = emoji_data.split( "\n", QString::SkipEmptyParts );
  QStringList emoji_parts;
  QString emoji_key;
  QString emoji_file;
  foreach( QString emoji_line, emoji_data_list )
  {
    emoji_parts = emoji_line.split( "\t", QString::SkipEmptyParts );
    if( emoji_parts.count() < 2 )
      continue;

    emoji_file = emoji_parts.at( 0 ).trimmed();
    emoji_file.remove( ".png" );
    emoji_key = emoji_parts.at( 1 ).trimmed();

    Emoticon emoji( emoji_key, emoji_file );
    m_emojis.insert( emoji_key.at( 0 ), emoji );

    qDebug() << "Load Emoji: char" << qPrintable( emoji_key ) << "and file" << emoji_file;
  }

  emoji_list_file.close();
}
#endif

