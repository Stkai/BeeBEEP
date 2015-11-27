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

#ifdef BEEBEEP_USE_HUNSPELL

#include "SpellChecker.h"
#ifdef Q_OS_WIN32
  #include "hunspell.hxx"
#elif defined Q_OS_MAC
  #include "hunspell/hunspell.hxx"
#else
  #include "hunspell/hunspell.hxx"
#endif

SpellChecker* SpellChecker::mp_instance = NULL;

SpellChecker::SpellChecker()
 : mp_hunspell( 0 ), m_dictionary( "" ), m_userDictionary( "" ), m_encoding( "ISO8859-1" ), mp_codec( 0 ), m_baseWordList()
{
  m_baseWordList << "Marco" << "Mastroddi" << "BeeBEEP";
  mp_completer = new QCompleter;
  mp_completer->setModelSorting( QCompleter::CaseInsensitivelySortedModel );
  mp_completer->setCaseSensitivity( Qt::CaseInsensitive );
  mp_completer->setWrapAround( false );
  updateCompleter( "" );
}

SpellChecker::~SpellChecker()
{
  if( mp_hunspell )
    delete mp_hunspell;
  delete mp_completer;
}

void SpellChecker::clearDictionary()
{
  if( mp_hunspell )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "SpellChecker clear dictionary";
#endif
    delete mp_hunspell;
    m_dictionary = "";
    mp_hunspell = 0;
  }
}

bool SpellChecker::setDictionary( const QString& dictionary_path )
{
  if( !QFile::exists( dictionary_path ) )
  {
    qWarning() << "Unable to find dictionary in path:" << dictionary_path;
    return false;
  }

  clearDictionary();

  m_dictionary = dictionary_path;
  m_encoding = "ISO8859-1";  // default
  qDebug() << "SpellChecker is loading dictionary from path:" << m_dictionary;

  QString affix_path = dictionary_path;
  affix_path.replace( QRegExp( QLatin1String( "\\.dic$" ) ), QLatin1String( ".aff" ) );

  QByteArray ba_dict_path = m_dictionary.toLocal8Bit();
  QByteArray ba_affix_path = affix_path.toLocal8Bit();

  mp_hunspell = new Hunspell( ba_affix_path.constData(), ba_dict_path.constData() );

  QFile affix_file( affix_path );
  if( affix_file.open( QIODevice::ReadOnly ) )
  {
    QTextStream affix_stream( &affix_file );
    QRegExp re_encoding_detector( "^\\s*SET\\s+([A-Z0-9\\-]+)\\s*", Qt::CaseInsensitive );
    QString affix_line;
    while( !affix_stream.atEnd() )
    {
      affix_line = affix_stream.readLine();
      if( re_encoding_detector.indexIn( affix_line ) > -1 )
      {
        m_encoding = re_encoding_detector.cap( 1 );
        qDebug() << "SpellChecker has set encoding to:" << m_encoding;
        break;
      }
    }
    affix_file.close();
  }
  else
    qWarning() << "Unable to open dictionary affix file:" << affix_path;

  mp_codec = QTextCodec::codecForName( m_encoding.toLatin1().constData() );
  if( mp_codec )
    return true;

  qWarning() << "Unable to set codec for dictionary in path" << m_dictionary;
  clearDictionary();
  return false;
}

bool SpellChecker::setUserDictionary( const QString& user_dictionary )
{
  m_userDictionary = user_dictionary;

  if( m_userDictionary.isEmpty() )
  {
    qDebug() << "User dictionary is now disabled";
    return true;
  }

  QString word_in_line;
  QFile user_dictionary_file( m_userDictionary );
  if( user_dictionary_file.open( QIODevice::ReadOnly ) )
  {
    qDebug() << "Loading user dictionary from path:" << m_userDictionary;
    QTextStream user_dictionary_stream( &user_dictionary_file );

    while( !user_dictionary_stream.atEnd() )
    {
      word_in_line = user_dictionary_stream.readLine();
      if( !word_in_line.isEmpty() )
        addWord( word_in_line );
    }
    user_dictionary_file.close();
    return true;
  }

  qWarning() << "Unable to find user dictionary in path:" << m_userDictionary;
  return false;
}

bool SpellChecker::isGoodWord( const QString& word )
{
  if( mp_hunspell )
    return mp_hunspell->spell( mp_codec->fromUnicode( word ).constData() ) != 0;
  else
    return true;
}

QStringList SpellChecker::suggest( const QString& word )
{
  QStringList suggest_list;
  if( !mp_hunspell )
  {
    qWarning() << "SpellChecker has not HUNSPELL instance";
    return suggest_list;
  }

  char **suggest_word_list;
  int num_suggestions = mp_hunspell->suggest( &suggest_word_list, mp_codec->fromUnicode( word ).constData() );

  for( int i = 0; i < num_suggestions; i++ )
  {
    suggest_list.append( mp_codec->toUnicode( suggest_word_list[ i ] ) );
    free( suggest_word_list[ i ] );
  }

  return suggest_list;
}

void SpellChecker::ignoreWord( const QString& word )
{
#ifdef BEEBEEP_DEBUG
  qDebug() << "SpellChecker has added" << word << "to ignore list";
#endif
  addWord( word );
  // temporary
}

bool SpellChecker::addWord( const QString& word )
{
  if( !mp_hunspell )
  {
    qWarning() << "SpellChecker has not HUNSPELL instance. Unable to add word:" << word;
    return false;
  }

  mp_hunspell->add( mp_codec->fromUnicode( word ).constData() );
  return true;
}

void SpellChecker::addToUserDictionary( const QString& word )
{
  addWord( word );

  if( !m_userDictionary.isEmpty() )
  {
    QFile user_dictonary_file( m_userDictionary );
    if( user_dictonary_file.open( QIODevice::Append ) )
    {
      QTextStream user_dictonary_stream( &user_dictonary_file );
      user_dictonary_stream << word << "\n";
      user_dictonary_file.close();
      qDebug() << "SpellChecker has added" << word << "to user dictionary:" << m_userDictionary;
    }
    else
      qWarning() << "Unable to open user dictionary for appending a new word in path:" << m_userDictionary << "could not be opened for appending a new word";
  }
  else
    qWarning() << "User dictionary path is empty";
}

void SpellChecker::updateCompleter( const QString& word_to_complete )
{
  QStringList sl;
  foreach( QString s, m_baseWordList )
  {
    if( s.startsWith( word_to_complete, Qt::CaseInsensitive ) )
      sl << s;
  }

  if( !word_to_complete.isEmpty() )
    sl.append( suggest( word_to_complete ) );

  QStringListModel* slm = new QStringListModel( mp_completer );
  slm->setStringList( sl );
  mp_completer->setModel( slm );
  mp_completer->setCompletionPrefix( word_to_complete );
}

#endif // BEEBEEP_USE_HUNSPELL
