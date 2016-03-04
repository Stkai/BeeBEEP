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

#ifndef BEEBEEP_SHORTCUTMANAGER_H
#define BEEBEEP_SHORTCUTMANAGER_H

#include "Config.h"


class ShortcutManager
{
// Singleton Object
  static ShortcutManager* mp_instance;

public:
  enum ShortcutType { Empty, SendFile, ShowFileTransfers, SetFocusInMessageBox, MinimizeAllChats,
                      ShowNextUnreadMessage, SendChatMessage, Print, Broadcast, FindTextInChat,
                      FindNextTextInChat, NumShortcut };

  void setDefaultShortcuts();

  inline const QKeySequence& shortcut( int ) const;
  inline QString shortcutKey( int ) const;
  QString shortcutKey( const QKeySequence& ) const;
  inline const QString& shortcutName( int ) const;
  bool setShortcut( int, const QString& );

  QStringList saveToStringList() const;
  void loadFromStringList( const QStringList& );


  static ShortcutManager& instance()
  {
    if( !mp_instance )
      mp_instance = new ShortcutManager();
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
  ShortcutManager();

private:
  QVector<QKeySequence> m_shortcuts;
  QVector<QString> m_shortcutNames;

};

// Inline Functions
inline const QKeySequence& ShortcutManager::shortcut( int st ) const { return st > 0 && st < NumShortcut ? m_shortcuts.at( st ) : m_shortcuts.at( Empty ); }
inline QString ShortcutManager::shortcutKey( int st ) const { return shortcutKey( shortcut( st ) ); }
inline const QString& ShortcutManager::shortcutName( int st ) const { return st > 0 && st < NumShortcut ? m_shortcutNames.at( st ) : m_shortcutNames.at( Empty ); }

#endif // BEEBEEP_SHORTCUTMANAGER_H
