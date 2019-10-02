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


EmoticonManager* EmoticonManager::mp_instance = Q_NULLPTR;


EmoticonManager::EmoticonManager()
 : m_emoticons(), m_oneCharEmoticons(), m_uniqueKeys(),
   m_maxTextSize( 2 ), m_recentEmoticons()
{
#ifdef BEEBEEP_DEBUG
  //createEmojiFiles();
#endif

  addTextEmoticon();
  addEmojis();
  m_uniqueKeys = m_emoticons.uniqueKeys();

#ifdef BEEBEEP_DEBUG
  QString s_debug = "";

  qSort( m_uniqueKeys );
  foreach( QChar c, m_uniqueKeys )
  {
    s_debug.append( c );
    s_debug.append( " " );
  }
  qDebug() << "Emoticon manager has" << m_uniqueKeys.size() << "unique keys:" << qPrintable( s_debug );
  qDebug() << "Emoticon manager has" << m_oneCharEmoticons.size() << "single char emoticons";
#endif

  qDebug() << "Emoticon manager loads" << m_emoticons.size() << "emojis";
}

void EmoticonManager::addTextEmoticon()
{
  addEmoticon( ":)", "1f603", Emoticon::Text );
  addEmoticon( ":-)", "1f603", Emoticon::Text );
  addEmoticon( ":p", "1f61c", Emoticon::Text );
  addEmoticon( ":-p", "1f61c", Emoticon::Text );
  addEmoticon( ":P", "1f61d", Emoticon::Text );
  addEmoticon( ":-P", "1f61d", Emoticon::Text );
  addEmoticon( ":'", "1f62d", Emoticon::Text );
  addEmoticon( ":'(", "1f62d", Emoticon::Text );
  addEmoticon( ":D", "1f604", Emoticon::Text );
  addEmoticon( ":-D", "1f604", Emoticon::Text );
  addEmoticon( ":@", "1f621", Emoticon::Text );
  addEmoticon( ":x", "1f61a", Emoticon::Text );
  addEmoticon( ":*", "1f618", Emoticon::Text );
  addEmoticon( ":-*", "1f618", Emoticon::Text );
  addEmoticon( ":z", "1f634", Emoticon::Text );
  addEmoticon( ":o", "1f628", Emoticon::Text );
  addEmoticon( ":O", "1f631", Emoticon::Text );
  addEmoticon( ":|", "1f613", Emoticon::Text );
  addEmoticon( ":L", "1f60d", Emoticon::Text );
  addEmoticon( ":w", "whistle", Emoticon::Unknown );
  addEmoticon( ":$", "bandit", Emoticon::Unknown );
  addEmoticon( ":!", "wizard", Emoticon::Unknown );
  addEmoticon( ";)", "1f609", Emoticon::Text );
  addEmoticon( ";-)", "1f609", Emoticon::Text );
  addEmoticon( ":(", "1f614", Emoticon::Text );
  addEmoticon( ":-(", "1f614", Emoticon::Text );
  addEmoticon( ":T", "1f60b", Emoticon::Text );
  addEmoticon( ":%", "1f616", Emoticon::Text );
  addEmoticon( ":\"D", "1f602", Emoticon::Text );
  addEmoticon( ":&quot;D", "1f602", Emoticon::Text ); // for html
  addEmoticon( "B)", "1f60e", Emoticon::Text );
  addEmoticon( "B-)", "1f60e", Emoticon::Text );
  addEmoticon( "<3", "2764", Emoticon::Text );
  addEmoticon( "&lt;3", "2764", Emoticon::Text );  // for html
  addEmoticon( "</3", "1f494", Emoticon::Text );
  addEmoticon( "&lt;/3", "1f494", Emoticon::Text ); // for html
  addEmoticon( ">_<", "1f616", Emoticon::Text );
  addEmoticon( "&gt;_&lt;", "1f616", Emoticon::Text ); // for html
  addEmoticon( "=)", "1f60a", Emoticon::Text );
  addEmoticon( "}:)", "1f608", Emoticon::Text );
  addEmoticon( "o:)", "1f608", Emoticon::Text );
  addEmoticon( "x(", "1f637", Emoticon::Text );
  addEmoticon( "x-(", "1f637", Emoticon::Text );
  addEmoticon( "X|", "1f632", Emoticon::Text );
  addEmoticon( "X-|", "1f632", Emoticon::Text );
  addEmoticon( "^_^", "1f601", Emoticon::Text );
  addEmoticon( "O.o", "1f633", Emoticon::Text );
  addEmoticon( "o.O", "1f633", Emoticon::Text );

}

void EmoticonManager::addEmoticon( const QString& e_text, const QString& e_name, int emoticon_group, int sort_order )
{
  int emoticon_key_size = e_text.size();
  QChar key_char = e_text.at( 0 );

  m_emoticons.insert( key_char, Emoticon( e_text, e_name, emoticon_group, sort_order ) );

  if( emoticon_key_size == 1 && !m_oneCharEmoticons.contains( key_char ) )
    m_oneCharEmoticons.append( key_char );

  if( emoticon_key_size > m_maxTextSize )
    m_maxTextSize = emoticon_key_size;
}

static bool SortEmoticon( const Emoticon& e1, const Emoticon& e2 )
{
  if( e1.sortOrder() < 0 || e2.sortOrder() < 0 )
    return e1.name() < e2.name();
  else
    return e1.sortOrder() < e2.sortOrder();
}

QList<Emoticon> EmoticonManager::textEmoticons( bool remove_names_duplicated ) const
{
  QList<Emoticon> emoticon_list;
  bool emoticon_to_add = false;
  QMultiHash<QChar, Emoticon>::const_iterator it = m_emoticons.begin();
  while( it != m_emoticons.end() )
  {
    if( !it.value().isInGroup() )
    {
      if( !remove_names_duplicated )
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
            break;
          }
          ++it2;
        }
        if( emoticon_to_add )
          emoticon_list << *it;
      }
    }
    ++it;
  }

  qSort( emoticon_list.begin(), emoticon_list.end(), SortEmoticon );

  return emoticon_list;
}

QList<Emoticon> EmoticonManager::emoticonsByGroup( int group_id ) const
{
  QList<Emoticon> emoticon_list;
  QMultiHash<QChar, Emoticon>::const_iterator it = m_emoticons.begin();
  while( it != m_emoticons.end() )
  {
    if( it.value().group() == group_id )
      emoticon_list << *it;
    ++it;
  }

  qSort( emoticon_list.begin(), emoticon_list.end(), SortEmoticon );

  return emoticon_list;
}

Emoticon EmoticonManager::emoticon( const QString& e_text ) const
{
  if( !e_text.isEmpty() )
  {
    QChar emoticon_key = e_text.at( 0 );
    QMultiHash<QChar, Emoticon>::const_iterator it = m_emoticons.find( emoticon_key );
    while( it != m_emoticons.end() && it.key() == emoticon_key )
    {
      if( it.value().textToMatch() == e_text )
        return it.value();
      ++it;
    }
  }
  return Emoticon();
}

Emoticon EmoticonManager::textEmoticon( const QString& e_text ) const
{
  if( !e_text.isEmpty() )
  {
    QChar emoticon_key = e_text.at( 0 );
    QMultiHash<QChar, Emoticon>::const_iterator it = m_emoticons.find( emoticon_key );
    while( it != m_emoticons.end() && it.key() == emoticon_key )
    {
      if( !it.value().isInGroup() && it.value().textToMatch() == e_text )
        return it.value();
      ++it;
    }
  }
  return Emoticon();
}

Emoticon EmoticonManager::emoticonByFile( const QString& e_file_name ) const
{
  if( !e_file_name.isEmpty() )
  {
    QMultiHash<QChar, Emoticon>::const_iterator it = m_emoticons.begin();
    while( it != m_emoticons.end() )
    {
      if( it.value().fileName() == e_file_name && it.value().isInGroup() )
        return it.value();
      ++it;
    }
  }
  return Emoticon();
}

QString EmoticonManager::parseEmoticons( const QString& msg, int emoticon_size, bool use_native_emoticons ) const
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

      Emoticon e;
      if( use_native_emoticons )
      {
        Emoticon text_emoticon = textEmoticon( text_to_match );
        if( text_emoticon.isValid() )
        {
          Emoticon native_emoticon = emoticonByFile( text_emoticon.fileName() );
          if( native_emoticon.isValid() )
          {
            s += native_emoticon.textToMatch();
            text_to_match = "";
            parse_emoticons = false;
          }
        }
      }
      else
        e = emoticon( text_to_match );

      if( e.isValid() )
      {
        s += e.toHtml( emoticon_size );
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
    else if( m_uniqueKeys.contains( c ) )
    {
      if( !use_native_emoticons && isOneCharEmoticon( c ) )
      {
        Emoticon e = emoticon( c );
        if( e.isValid() )
        {
          s += e.toHtml( emoticon_size );
          text_to_match = "";
          parse_emoticons = true;
        }
      }
      else if( parse_emoticons )
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

int EmoticonManager::loadRecentEmoticons( const QStringList& sl, int max_size )
{
  m_recentEmoticonsMaxSize = qMax( 6, max_size );
  int emoticons_load = 0;
  m_recentEmoticons.clear();
  Emoticon e;
  foreach( QString s, sl )
  {
    e = emoticon( s );
    if( e.isValid() )
    {
      m_recentEmoticons.append( e );
      emoticons_load++;
      if( m_recentEmoticons.size() == m_recentEmoticonsMaxSize )
        break;
    }
  }
  return emoticons_load;
}

QStringList EmoticonManager::saveRencentEmoticons() const
{
  QStringList sl;
  foreach( Emoticon e, m_recentEmoticons )
    sl.append( e.textToMatch() );
  return sl;
}

bool EmoticonManager::addToRecentEmoticons( const Emoticon& e )
{
  if( m_recentEmoticons.contains( e ) )
    return false;
  m_recentEmoticons.prepend( e );
  if( m_recentEmoticons.size() > m_recentEmoticonsMaxSize )
    m_recentEmoticons.removeLast();
  return true;
}

void EmoticonManager::createEmojiFiles()
{
  QFile emoji_list_file( "../misc/emoji_list.txt" );
  if( !emoji_list_file.open( QFile::ReadOnly | QFile::Text ) )
    return;

  QStringList emoji_parts;
  QString emoji_key;
  QString emoji_file;
  QString emoji_line;
  int sort_order = 0;
  QList<Emoticon> emoji_list;

  QTextStream text_stream_in( &emoji_list_file );
  text_stream_in.setCodec( "UTF-8" );
  while( !text_stream_in.atEnd() )
  {
    emoji_line = text_stream_in.readLine();
    emoji_parts = emoji_line.split( "\t", QString::SkipEmptyParts );
    if( emoji_parts.count() < 2 )
      continue;

    emoji_file = emoji_parts.at( 0 ).trimmed();
    emoji_file.remove( ".png" );
    emoji_key = emoji_parts.at( 1 ).trimmed();
    sort_order++;

    emoji_list.append( Emoticon( emoji_key, emoji_file, Emoticon::Unknown, sort_order ) );
#ifdef BEEBEEP_DEBUG
    qDebug() << "Load Emoji: char" << qPrintable( emoji_key ) << "and file" << emoji_file;
#endif
  }

  emoji_list_file.close();
  qDebug() << emoji_list.size() << "emojis load from list";

  QStringList emoji_group_names;
  emoji_group_names << "";
  emoji_group_names << "Text";
  emoji_group_names << "People";
  emoji_group_names << "Objects";
  emoji_group_names << "Nature";
  emoji_group_names << "Places";
  emoji_group_names << "Symbols";

  QList<Emoticon>::iterator emoji_it = emoji_list.begin();
  QString emoji_file_name;
  while( emoji_it != emoji_list.end() )
  {
    for( int i = Emoticon::People; i < Emoticon::NumGroups; i++ )
    {
      emoji_file_name = QString( "../src/emojis/" ) + emoji_group_names.at( i ).toLower() + QString( "/" ) + emoji_it->name() + ".png";
      if( QFile::exists( emoji_file_name ) )
      {
        qDebug() << "Found emoji" << qPrintable( emoji_it->textToMatch() ) << "in file" << emoji_file_name;
        emoji_it->setGroup( i );
      }
    }
    ++emoji_it;
  }

  int emoji_in_group = 0;
  int emoji_not_in_group = 0;
  int emoji_in_twitter = 0;
  qDebug() << "Checking missed emoji in twitter folder";
  foreach( Emoticon e, emoji_list )
  {
    if( !e.isInGroup() )
    {
      emoji_not_in_group++;
      emoji_file_name = QString( "../src/emojis/twitter/%1.png" ).arg( e.name() );
      if( QFile::exists( emoji_file_name ) )
      {
        qDebug() << qPrintable( QString( "cp twitter/%1.png ." ).arg( e.name() ) );
        emoji_in_twitter++;
      }
    }
    else
      emoji_in_group++;
  }

  qDebug() << "Emoji in group:" << emoji_in_group;
  qDebug() << "Emoji not in group:" << emoji_not_in_group;
  qDebug() << "Emoji missed in twitter:" << emoji_in_twitter;

  for( int i = Emoticon::People; i < Emoticon::NumGroups; i++ )
  {
    QString emoji_folder_name = QString( "../src/emojis/" ) + emoji_group_names.at( i ).toLower();
    QDir emoji_folder( emoji_folder_name );
    QStringList file_list = emoji_folder.entryList( QDir::NoDotAndDotDot | QDir::NoSymLinks );
    foreach( QString s, file_list )
    {
      if( !s.contains( ".png" ) )
        continue;

      bool emoji_exists = false;
      foreach( Emoticon e, emoji_list )
      {
        if( QString( "%1.png" ).arg( e.name() ) == s )
        {
          emoji_exists = true;
          break;
        }
      }
      if( !emoji_exists )
        qDebug() << "Emoji not in text list:" << qPrintable( QString( "%1/%2" ).arg( emoji_folder_name ).arg( s ) );
    }
  }

  QFile file_to_save( "../src/Emojis.cpp" );
  if( file_to_save.exists() )
    file_to_save.remove();

  if( !file_to_save.open( QFile::ReadWrite ) )
  {
    qWarning() << file_to_save.fileName() << "is not writeable";
    return;
  }

  QTextStream text_stream_out( &file_to_save );
  text_stream_out.setCodec( "UTF-8" );

  text_stream_out << "//////////////////////////////////////////////////////////////////////\n"
                     "//\n"
                     "// This file is part of BeeBEEP.\n"
                     "//\n"
                     "// BeeBEEP is free software: you can redistribute it and/or modify\n"
                     "// it under the terms of the GNU General Public License as published\n"
                     "// by the Free Software Foundation, either version 3 of the License,\n"
                     "// or (at your option) any later version.\n"
                     "//\n"
                     "// BeeBEEP is distributed in the hope that it will be useful,\n"
                     "// but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
                     "// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
                     "// GNU General Public License for more details.\n"
                     "//\n"
                     "// You should have received a copy of the GNU General Public License\n"
                     "// along with BeeBEEP.  If not, see <http://www.gnu.org/licenses/>.\n"
                     "//\n"
                     "// Author: Marco Mastroddi <marco.mastroddi(AT)gmail.com>\n"
                     "//\n"
                     "// Emojis.cpp is generated in " << QDateTime::currentDateTime().toString( "yyyy-MM-dd hh:mm:ss" ) << "\n"
                     "//\n"
                     "//////////////////////////////////////////////////////////////////////\n"
                     "\n";

  text_stream_out << "#include \"EmoticonManager.h\"\n\n\n";
  text_stream_out << "void EmoticonManager::addEmojis()\n{\n";

  foreach( Emoticon e, emoji_list )
  {
    if( e.isInGroup() )
      text_stream_out << "  addEmoticon( QString::fromUtf8( \"" << e.textToMatch() << "\" ), \"" << e.name() << "\", Emoticon::" << emoji_group_names.at( e.group() ) << ", " << e.sortOrder() << " ); \n";
  }

  text_stream_out << "\n}\n\n";
  file_to_save.close();

  QFile emoji_resource_file( "../src/emojis.qrc" );

  if( emoji_resource_file.exists() )
    emoji_resource_file.remove();

  if( !emoji_resource_file.open( QFile::ReadWrite ) )
  {
    qWarning() << emoji_resource_file.fileName() << "is not writeable";
    return;
  }

  QTextStream text_stream_resource( &emoji_resource_file );

  text_stream_resource << "<RCC>\n\t<qresource prefix=\"/\">\n";

  foreach( Emoticon e, emoji_list )
  {
    if( e.isInGroup() )
      text_stream_resource << "\t\t<file>emojis/" << emoji_group_names.at( e.group() ).toLower() << "/" << e.name() << ".png</file>\n";
  }

  text_stream_resource << "\t</qresource>\n</RCC>\n\n";
  emoji_resource_file.close();
}
