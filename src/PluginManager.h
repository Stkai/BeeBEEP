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

#include "Config.h"
#include "Interfaces.h"


class PluginManager
{
// Singleton Object
  static PluginManager* mp_instance;

public:
  inline const QList<TextMarkerInterface*>& textMarkers() const;
  inline const QList<ServiceInterface*>& services() const;
  inline int count() const;

  void loadPlugins();
  void setPluginEnabled( const QString& plugin_name, bool );
  void setPluginsEnabled( bool );

  ServiceInterface* service( const QString& ) const;

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
      delete mp_instance;
      mp_instance = NULL;
    }
  }

protected:
  PluginManager();

  void addPlugin( const QString& );
  void sortPlugins();

  bool parseTextWithPlugin( QString*, TextMarkerInterface* ) const;


private:
  QList<TextMarkerInterface*> m_textMarkers;
  QList<ServiceInterface*> m_services;

};


// Inline Functions
inline const QList<TextMarkerInterface*>& PluginManager::textMarkers() const { return m_textMarkers; }
inline const QList<ServiceInterface*>& PluginManager::services() const { return m_services; }
inline int PluginManager::count() const { return m_textMarkers.size() + m_services.size(); }

#endif // BEEBEEP_PLUGINMANAGER_H
