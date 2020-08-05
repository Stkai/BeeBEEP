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
// $Id$
//
//////////////////////////////////////////////////////////////////////

#ifndef BEEBEEP_EMOTICONMANAGER_H
#define BEEBEEP_EMOTICONMANAGER_H

#include "Emoticon.h"

class EmoticonManager
{
// Singleton Object
  static EmoticonManager* mp_instance;

public:
  QString parseEmoticons( const QString&, int emoticon_size, bool use_native_emoticons ) const;
  Emoticon emoticon( const QString& ) const;
  Emoticon textEmoticon( const QString& ) const;
  Emoticon emoticonByFile( const QString& ) const;

  QList<Emoticon> textEmoticons( bool remove_names_duplicated ) const;
  QList<Emoticon> emoticonsByGroup( int ) const;
  bool addToRecentEmoticons( const Emoticon& );
  inline const QList<Emoticon>& recentEmoticons() const;

  inline void clearRecentEmoticons();

  int loadRecentEmoticons( const QStringList&, int );
  QStringList saveRencentEmoticons() const;

  static EmoticonManager& instance()
  {
    if( !mp_instance )
      mp_instance = new EmoticonManager();
    return *mp_instance;
  }

  static void close()
  {
    if( mp_instance )
    {
      delete mp_instance;
      mp_instance = Q_NULLPTR;
    }
  }

protected:
  EmoticonManager();

  void createEmojiFiles();

  void addEmoticon( const QString&, const QString&, int emoticon_group = Emoticon::Unknown, int sort_order = -1 );
  void addTextEmoticon();
  void addEmojis();

  inline bool isOneCharEmoticon( const QChar& ) const;

private:
  QMultiHash<QChar, Emoticon> m_emoticons;
  QList<QChar> m_oneCharEmoticons;
  QList<QChar> m_uniqueKeys;
  int m_maxTextSize;
  QList<Emoticon> m_recentEmoticons;
  int m_recentEmoticonsMaxSize;

};

// Inline Functions
inline const QList<Emoticon>& EmoticonManager::recentEmoticons() const { return m_recentEmoticons; }
inline bool EmoticonManager::isOneCharEmoticon( const QChar& c ) const { return m_oneCharEmoticons.contains( c ); }
inline void EmoticonManager::clearRecentEmoticons() { m_recentEmoticons.clear(); }

#endif // BEEBEEP_EMOTICONMANAGER_H
