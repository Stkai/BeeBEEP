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

#include "PluginManager.h"
#include "Settings.h"


PluginManager* PluginManager::mp_instance = NULL;


PluginManager::PluginManager()
  : m_textMarkers(), m_services()
{
}

void PluginManager::loadPlugins()
{
  qDebug() << "Loading plugins from path:" << Settings::instance().pluginPath();
  QDir plugin_dir( Settings::instance().pluginPath() );
  if( !plugin_dir.exists() )
  {
    qWarning() << "Plugin folder" << Settings::instance().pluginPath() << "not found";
    return;
  }

  foreach( QString file_name, plugin_dir.entryList() )
    addPlugin( plugin_dir.absoluteFilePath( file_name ) );

  qDebug() << m_textMarkers.size() << "text marker plugins found";
  qDebug() << m_services.size() << "service plugins found";
  sortPlugins();
}

void PluginManager::addPlugin( const QString& file_path )
{
  QPluginLoader plugin_loader( file_path );
  QObject* plugin = plugin_loader.instance();
  if( plugin )
  {
    TextMarkerInterface* text_marker_plugin = qobject_cast<TextMarkerInterface*>( plugin );
    if( text_marker_plugin )
    {
      qDebug() << text_marker_plugin->name() << "is a text marker plugin";
      m_textMarkers.append( text_marker_plugin );
      return;
    }
    else
      qDebug() << file_path << "is not a text marker plugin";

    ServiceInterface* service_plugin = qobject_cast<ServiceInterface*>( plugin );
    if( service_plugin )
    {
      qDebug() << service_plugin->name() << "is a service plugin";
      m_services.append( service_plugin );
      return;
    }
    else
      qDebug() << file_path << "is not a service plugin";
  }
  else
    qDebug() << file_path << "is not a plugin";
}

void PluginManager::setPluginEnabled( const QString& plugin_name, bool enabled )
{
  foreach( TextMarkerInterface* text_marker, m_textMarkers )
  {
    if( text_marker->name() == plugin_name )
      text_marker->setEnabled( enabled );
  }

  foreach( ServiceInterface* service, m_services )
  {
    if( service->name() == plugin_name )
      service->setEnabled( enabled );
  }
}

void PluginManager::setPluginsEnabled( bool enabled )
{
  foreach( TextMarkerInterface* text_marker, m_textMarkers )
    text_marker->setEnabled( enabled );

  foreach( ServiceInterface* service, m_services )
    service->setEnabled( enabled );
}

static bool TextMarkerForPriority( TextMarkerInterface* tm1, TextMarkerInterface* tm2 )
{
  return tm1->priority() < tm2->priority();
}

static bool ServiceForName( ServiceInterface* s1, ServiceInterface* s2 )
{
  return s1->name() < s2->name();
}

void PluginManager::sortPlugins()
{
  qSort( m_textMarkers.begin(), m_textMarkers.end(), TextMarkerForPriority );
  qSort( m_services.begin(), m_services.end(), ServiceForName );
}

ServiceInterface* PluginManager::service( const QString& service_name ) const
{
  foreach( ServiceInterface* si, m_services )
  {
    if( si->name() == service_name )
      return si;
  }
  return 0;
}
