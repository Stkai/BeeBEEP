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

#ifndef BEEBEEP_GUISESSIONMANAGER_H
#define BEEBEEP_GUISESSIONMANAGER_H

#include "Config.h"


class GuiSessionManager
{
// Singleton Object
  static GuiSessionManager* mp_instance;
  QMap<QString,QString> m_chatMap;

public:
  bool load();
  bool save();
  
  QString chatStoredText( const QString& );
  inline bool chatHasStoredText( const QString& ) const;

  static GuiSessionManager& instance()
  {
    if( !mp_instance )
      mp_instance = new GuiSessionManager();
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
  GuiSessionManager();

  void saveChats( QDataStream* );
  void loadChats( QDataStream* );

};


// Inline Functions
inline bool GuiSessionManager::chatHasStoredText( const QString& chat_name ) const { return m_chatMap.contains( chat_name ); }

#endif // BEEBEEP_GUISESSIONMANAGER_H
