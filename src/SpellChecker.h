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

#ifndef BEEBEEP_SPELLCHECKER_H
#define BEEBEEP_SPELLCHECKER_H

#ifdef BEEBEEP_USE_HUNSPELL

#include "Config.h"
class Hunspell;


class SpellChecker
{
  // Singleton Object
  static SpellChecker* mp_instance;

public:
  inline bool isValid() const;

  bool setDictionary( const QString& );
  bool setUserDictionary( const QString& );

  bool isGoodWord( const QString& );
  void ignoreWord( const QString& );
  void addToUserDictionary(const QString& );

  inline QCompleter* completer() const;
  void updateCompleter( const QString& );
  inline const QString& completerPrefix() const;

  static SpellChecker& instance()
  {
    if( !mp_instance )
      mp_instance = new SpellChecker();
    return *mp_instance;
  }

  static void close()
  {
    if( mp_instance )
    {
      mp_instance->clearDictionary();
      delete mp_instance;
      mp_instance = NULL;
    }
  }

protected:
  SpellChecker();
  ~SpellChecker();
  bool addWord( const QString& );
  void clearDictionary();
  QStringList suggest( const QString& );

private:
  Hunspell* mp_hunspell;
  QString m_dictionary;
  QString m_userDictionary;
  QString m_encoding;
  QTextCodec* mp_codec;

  QCompleter* mp_completer;
  QString m_completerPrefix;

};

// Inline Functions
inline bool SpellChecker::isValid() const { return mp_hunspell != 0; }
inline QCompleter* SpellChecker::completer() const { return mp_completer; }
inline const QString& SpellChecker::completerPrefix() const { return m_completerPrefix; }

#endif // BEEBEEP_USE_HUNSPELL
#endif // BEEBEEP_SPELLCHECKER_H
