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
// $Id: EmoticonManager.h 24 2010-06-13 17:36:35Z mastroddi $
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
  QString parseEmoticons( const QString& ) const;
  Emoticon emoticon( const QString& ) const;
  inline const QMultiHash<QChar, Emoticon>& emoticons() const;

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
      mp_instance = NULL;
    }
  }

protected:
  EmoticonManager();

  void addEmoticon( const QString&, const QString& );

private:
  QMultiHash<QChar, Emoticon> m_emoticons;

};


// Inline Functions

inline const QMultiHash<QChar, Emoticon>& EmoticonManager::emoticons() const { return m_emoticons; }


#endif // BEEBEEP_EMOTICONMANAGER_H
