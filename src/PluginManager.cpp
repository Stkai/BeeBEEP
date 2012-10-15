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

void PluginManager::clearPlugins()
{
  if( m_textMarkers.size() > 0 )
  {
    qDebug() << "Unload" << m_textMarkers.size() << "text marker plugins";
    qDeleteAll( m_textMarkers.begin(), m_textMarkers.end() );
    m_textMarkers.clear();
  }

  if( m_services.size() > 0 )
  {
    qDebug() << "Unload" << m_services.size() << "service plugins";
    qDeleteAll( m_services.begin(), m_services.end() );
    m_services.clear();
  }

  if( m_games.size() > 0 )
  {
    qDebug() << "Unload" << m_games.size() << "game plugins";
    qDeleteAll( m_games.begin(), m_games.end() );
    m_games.clear();
  }
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
  qDebug() << m_games.size() << "game plugins found";
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
      if( !textMarker( text_marker_plugin->name() ) )
        m_textMarkers.append( text_marker_plugin );
      else
        qDebug() << text_marker_plugin->name() << "already load... skip it";
      return;
    }
    else
      qDebug() << file_path << "is not a text marker plugin";

 #ifdef USE_QXMPP
    ServiceInterface* service_plugin = qobject_cast<ServiceInterface*>( plugin );
    if( service_plugin )
    {
      qDebug() << service_plugin->name() << "is a service plugin";
      if( !service( service_plugin->name() ) )
        m_services.append( service_plugin );
      else
        qDebug() << service_plugin->name() << "already load... skip it";
      return;
    }
    else
      qDebug() << file_path << "is not a service plugin";
#endif

    GameInterface* game_plugin = qobject_cast<GameInterface*>( plugin );
    if( game_plugin )
    {
      qDebug() << game_plugin->name() << "is a game plugin";
      if( !game( game_plugin->name() ) )
        m_games.append( game_plugin );
      else
        qDebug() << game_plugin->name() << "already load... skip it";
      return;
    }
    else
      qDebug() << file_path << "is not a game plugin";
  }
  else
    qDebug() << file_path << "is not a plugin";
}

void PluginManager::setPluginEnabled( const QString& plugin_name, bool enabled )
{
  foreach( TextMarkerInterface* tm, m_textMarkers )
  {
    if( tm->name() == plugin_name )
      tm->setEnabled( enabled );
  }

  foreach( ServiceInterface* s, m_services )
  {
    if( s->name() == plugin_name )
      s->setEnabled( enabled );
  }

  foreach( GameInterface* g, m_games )
  {
    if( g->name() == plugin_name )
      g->setEnabled( enabled );
  }
}

void PluginManager::setPluginsEnabled( bool enabled )
{
  foreach( TextMarkerInterface* tm, m_textMarkers )
    tm->setEnabled( enabled );

  foreach( ServiceInterface* s, m_services )
    s->setEnabled( enabled );

  foreach( GameInterface* g, m_games )
    g->setEnabled( enabled );
}

static bool TextMarkerForPriority( TextMarkerInterface* tm1, TextMarkerInterface* tm2 )
{
  return tm1->priority() < tm2->priority();
}

static bool ServiceForName( ServiceInterface* s1, ServiceInterface* s2 )
{
  return s1->name() < s2->name();
}

static bool GameForName( GameInterface* g1, GameInterface* g2 )
{
  return g1->name() < g2->name();
}

void PluginManager::sortPlugins()
{
  qSort( m_textMarkers.begin(), m_textMarkers.end(), TextMarkerForPriority );
  qSort( m_services.begin(), m_services.end(), ServiceForName );
  qSort( m_games.begin(), m_games.end(), GameForName );
}

TextMarkerInterface* PluginManager::textMarker( const QString& text_marker_name ) const
{
  foreach( TextMarkerInterface* tmi, m_textMarkers )
  {
    if( tmi->name() == text_marker_name )
      return tmi;
  }
  return 0;
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

GameInterface* PluginManager::game( const QString& game_name ) const
{
  foreach( GameInterface* g, m_games )
  {
    if( g->name() == game_name )
      return g;
  }
  return 0;
}

void PluginManager::setGamePauseOn()
{
  foreach( GameInterface* g, m_games )
  {
    if( !g->isPaused() )
      g->pause();
  }
}

bool PluginManager::parseText( QString* p_txt, bool before_sending ) const
{
  qDebug() << "Plugins parsing text:" << p_txt->toLatin1();
  if( p_txt->size() <= 0 )
  {
    *p_txt = "";
    return false;
  }

  foreach( TextMarkerInterface* text_marker, m_textMarkers )
  {
    if( !text_marker->isEnabled() )
      continue;

    if( before_sending != text_marker->parseBeforeSending() )
      continue;

    if( !parseTextWithPlugin( p_txt, text_marker ) )
    {
      qDebug() << text_marker->name() << "has break text marker plugins loop";
      return false;
    }
  }

  qDebug() << "Plugins has parsed:" << p_txt->toLatin1();
  return true;
}

bool PluginManager::parseTextWithPlugin( QString* p_txt, TextMarkerInterface* tmi ) const
{
  qDebug() << "Plugin" << tmi->name() << "starts to parse the text";

  bool space_added_at_begin = false;
  bool space_added_at_end = false;

  if( p_txt->startsWith( tmi->openCommand().trimmed() ) )
  {
    p_txt->prepend( QLatin1Char( ' ' ) );
    space_added_at_begin = true;
  }

  if( p_txt->endsWith( tmi->closeCommand().trimmed() ) )
  {
    p_txt->append( QLatin1Char( ' ' ) );
    space_added_at_end = true;
  }

  QString parsed_text = "";
  int open_cmd_index = p_txt->indexOf( tmi->openCommand(), 0, Qt::CaseInsensitive );
  int open_cmd_size = tmi->openCommand().size();
  int close_cmd_size = tmi->closeCommand().size();

  if( open_cmd_index >= 0 && open_cmd_index < p_txt->size() )
  {
    int close_cmd_index = p_txt->indexOf( tmi->closeCommand(), open_cmd_index+open_cmd_size, Qt::CaseInsensitive );
    if( close_cmd_index > open_cmd_index )
    {
      qDebug() << tmi->name() << "has found open/close command";

      tmi->initParser( p_txt->mid( open_cmd_index+open_cmd_size, close_cmd_index-open_cmd_index-open_cmd_size ) );

      bool is_in_tag = false;
      QString code_text = "";
      QChar c;

      for( int i = 0; i < p_txt->size(); i++ )
      {
        c = p_txt->at( i );
        if( c == QLatin1Char( '<' ) )
          is_in_tag = true;

        if( is_in_tag )
        {
          parsed_text.append( c );

          if( c == QLatin1Char( '>' ) )
            is_in_tag = false;

          continue;
        }

        if( c == QLatin1Char( '&' ) )
        {
          QChar c_tmp;
          // Search forward until either a semicolon, tag, or space is found
          for( int j=(i+1); j < p_txt->size(); j++ )
          {
            c_tmp = p_txt->at( j );
            if( c_tmp == QLatin1Char( '<' ) || c_tmp == QLatin1Char( '>' )
                || c_tmp == QLatin1Char( ';' ) || c_tmp.isSpace() )
            {
              if( c_tmp == QLatin1Char( ';' ) )
              {
                code_text = p_txt->mid( i, j-i+1 );
                qDebug() << "Code html found:" << code_text << "... skip it";
              }
              break;
            }
          }
        }

        if( i >= open_cmd_index && i < (open_cmd_index+open_cmd_size) )
        {
          // skip open command
          if( i == open_cmd_index )
          {
            parsed_text.append( " " );
          }
        }
        else if( i >= (open_cmd_index+open_cmd_size) && i < close_cmd_index )
        {
          if( code_text.size() > 0 )
          {
            parsed_text.append( tmi->parseString( code_text ) );
            i += code_text.size() - 1;
            code_text = "";
          }
          else
            parsed_text.append( tmi->parseString( QString( c ) ) );
        }
        else if( i >= close_cmd_index && i < (close_cmd_index+close_cmd_size) )
        {
          // skip close command
          if( i == close_cmd_index )
            parsed_text.append( " " );
        }
        else
          parsed_text.append( c );
      }

      if( parsed_text.contains( tmi->openCommand() ) )
        parseTextWithPlugin( &parsed_text, tmi );
    }
  }

  if( !parsed_text.isEmpty() )
    *p_txt = parsed_text;

  if( space_added_at_begin )
    p_txt->remove( 0, 1 );
  if( space_added_at_end )
    p_txt->chop( 1 );

  return true;
}
