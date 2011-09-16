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

#ifndef BEEBEEP_PLUGINMANAGER_H
#define BEEBEEP_PLUGINMANAGER_H

#include "Interfaces.h"


class PluginManager
{
// Singleton Object
  static PluginManager* mp_instance;

public:
  inline const QList<TextMarkerInterface*>& textMarkers() const;
  inline int count() const;

  void loadPlugins();
  void setPluginEnabled( const QString& plugin_name, bool );
  void setPluginsEnabled( bool );

  static PluginManager& instance()
  {
    if( !mp_instance )
      mp_instance = new PluginManager();
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
  PluginManager();

  void addPlugin( const QString& );


private:
  QList<TextMarkerInterface*> m_textMarkers;


};


// Inline Functions
inline const QList<TextMarkerInterface*>& PluginManager::textMarkers() const { return m_textMarkers; }
inline int PluginManager::count() const { return m_textMarkers.size(); }

#endif // BEEBEEP_PLUGINMANAGER_H
