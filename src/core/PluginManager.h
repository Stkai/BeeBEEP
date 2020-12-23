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

#ifndef BEEBEEP_PLUGINMANAGER_H
#define BEEBEEP_PLUGINMANAGER_H

#include "Config.h"
#include "Interfaces.h"


class PluginManager
{
// Singleton Object
  static PluginManager* mp_instance;

public:
  inline const QList<TextMarkerInterface*>& textMarkers() const;
  inline int count() const;

  void loadPlugins( const QString& plugin_folder_path, bool search_also_in_default_folders = true );
  void setPluginEnabled( const QString& plugin_name, bool );
  void setPluginsEnabled( bool );

  TextMarkerInterface* textMarker( const QString& ) const;
  bool parseText( QString*, bool before_sending ) const;

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
      mp_instance->clearPlugins();
      delete mp_instance;
      mp_instance = Q_NULLPTR;
    }
  }

protected:
  PluginManager();

  void addPlugin( const QString& );
  void sortPlugins();
  void clearPlugins();

  bool parseTextWithPlugin( QString*, TextMarkerInterface* ) const;

  bool fileCanBeSkipped( const QString& ) const;

private:
  QList<TextMarkerInterface*> m_textMarkers;

};


// Inline Functions
inline const QList<TextMarkerInterface*>& PluginManager::textMarkers() const { return m_textMarkers; }
inline int PluginManager::count() const { return m_textMarkers.size(); }

#endif // BEEBEEP_PLUGINMANAGER_H
