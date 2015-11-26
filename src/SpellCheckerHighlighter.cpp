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

#include "SpellCheckerHighlighter.h"
#include "Settings.h"
#ifdef BEEBEEP_USE_HUNSPELL
#include "SpellChecker.h"
#endif


SpellCheckerHighlighter::SpellCheckerHighlighter( QTextDocument* text_document )
 : QSyntaxHighlighter( text_document ), m_format()
{
  m_format.setUnderlineColor( Qt::red );
  m_format.setUnderlineStyle( QTextCharFormat::SpellCheckUnderline );
}

void SpellCheckerHighlighter::highlightWord( const QString& all_text, const QString& word )
{
  QRegExp re( word );
  int index = all_text.indexOf( re );
  while( index >= 0 )
  {
    int length = re.matchedLength();
    setFormat( index, length, m_format );
    index = all_text.indexOf( re, index + length );
  }
}

void SpellCheckerHighlighter::highlightBlock( const QString& txt )
{
  if( !Settings::instance().useSpellChecker() )
    return;

#ifdef BEEBEEP_USE_HUNSPELL

  QStringList words;
  QString word = "";
  foreach( QChar c, txt )
  {
    if( !c.isLetter() )
    {
      if( !word.isEmpty() )
        words << word;
      word = "";
    }
    else
      word.append( c );
  }

  if( !words.isEmpty() )
  {
    foreach( QString w, words )
    {
      if( !SpellChecker::instance().isGoodWord( w ) )
        highlightWord( txt, w );
    }
  }

#else

  Q_UNUSED( txt );

#endif // BEEBEEP_USE_HUNSPELL
}
