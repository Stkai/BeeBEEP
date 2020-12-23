//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2021 Marco Mastroddi
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
  Emoticon emoticonSelected( const QString& ); // add one to count
  Emoticon textEmoticon( const QString& ) const;
  Emoticon emoticonByFile( const QString& ) const;

  QList<Emoticon> textEmoticons( bool remove_names_duplicated ) const;
  QList<Emoticon> emoticonsByGroup( int ) const;

  inline const QList<Emoticon>& favoriteEmoticons() const;
  bool addToRecentEmoticons( const Emoticon& );
  inline const QList<Emoticon>& recentEmoticons() const;

  void clearFavoriteEmoticons();
  inline void clearRecentEmoticons();

  inline void setRecentEmoticonsCount( int );
  inline int recentEmoticonsCount() const;

  int loadFavoriteEmoticons( const QStringList& );
  QStringList saveFavoriteEmoticons() const;
  int loadRecentEmoticons( const QStringList& );
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

  bool setEmoticonCount( const QString&, int );
  QList<Emoticon> favoriteEmoticonsToSort() const;

private:
  QMultiHash<QChar, Emoticon> m_emoticons;
  QList<QChar> m_oneCharEmoticons;
  QList<QChar> m_uniqueKeys;
  int m_maxTextSize;
  QList<Emoticon> m_favoriteEmoticons;
  QList<Emoticon> m_recentEmoticons;
  int m_recentEmoticonsCount;

};

// Inline Functions
inline void EmoticonManager::setRecentEmoticonsCount( int new_value ) { m_recentEmoticonsCount = new_value; }
inline int EmoticonManager::recentEmoticonsCount() const { return m_recentEmoticonsCount; }
inline const QList<Emoticon>& EmoticonManager::favoriteEmoticons() const { return m_favoriteEmoticons; }
inline const QList<Emoticon>& EmoticonManager::recentEmoticons() const { return m_recentEmoticons; }
inline bool EmoticonManager::isOneCharEmoticon( const QChar& c ) const { return m_oneCharEmoticons.contains( c ); }
inline void EmoticonManager::clearRecentEmoticons() { m_recentEmoticons.clear(); }

#endif // BEEBEEP_EMOTICONMANAGER_H
