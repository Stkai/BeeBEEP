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

#include "PluginManager.h"
#include "Settings.h"


PluginManager* PluginManager::mp_instance = Q_NULLPTR;


PluginManager::PluginManager()
  : m_textMarkers()
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
}

void PluginManager::loadPlugins( const QString& plugin_folder_path, bool search_also_in_default_folders )
{
  clearPlugins();
  qDebug() << "Loading plugins from path" << qPrintable( plugin_folder_path );
  QDir plugin_dir( plugin_folder_path );
  if( !plugin_dir.exists() )
  {
    qWarning() << "Plugins folder" << qPrintable( plugin_folder_path ) << "not exists";
    return;
  }

  foreach( QString file_name, plugin_dir.entryList() )
    addPlugin( plugin_dir.absoluteFilePath( file_name ) );

  if( count() < 1 && search_also_in_default_folders )
  {
    qDebug() << "No plugins found... checking default folders";
    QString plugin_default_path = Settings::instance().defaultPluginFolderPath();
    if( plugin_default_path != plugin_folder_path )
    {
      loadPlugins( plugin_default_path, false );
      if( count() > 0 )
        Settings::instance().setPluginPath( plugin_default_path );
      return;
    }
  }
  qDebug() << m_textMarkers.size() << "text marker plugins found";
  sortPlugins();
}

void PluginManager::addPlugin( const QString& file_path )
{
  if( file_path.endsWith( "." ) )
    return;

  QFileInfo file_info( file_path );
  if( file_info.isSymLink() )
    return;

#if defined( Q_OS_WIN )
  if( file_info.suffix() != "dll" )
    return;
#elif defined( Q_OS_LINUX )
  if( !file_info.completeSuffix().contains( "so" ) )
    return;
#elif defined( Q_OS_MAC )
  if( !file_info.completeSuffix().contains( "dylib" ) )
    return;
#else
  if( file_info.suffix() == "rc" || file_info.suffix() == "dat" || file_info.suffix() == "wav"
    || file_info.suffix() == "ini" || file_info.suffix() == "qm" )
    return;
#endif

  if( fileCanBeSkipped( file_info.fileName() ) )
    return;

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
    qDebug() << qPrintable( file_path ) << "is not a valid plugin";
  }
#ifdef BEEBEEP_DEBUG
  else
    qDebug() << qPrintable( file_path ) << "is not a plugin";
#endif
}

void PluginManager::setPluginEnabled( const QString& plugin_name, bool enabled )
{
  foreach( TextMarkerInterface* tm, m_textMarkers )
  {
    if( tm->name() == plugin_name )
      tm->setEnabled( enabled );
  }
}

void PluginManager::setPluginsEnabled( bool enabled )
{
  foreach( TextMarkerInterface* tm, m_textMarkers )
    tm->setEnabled( enabled );
}

static bool TextMarkerForPriority( TextMarkerInterface* tm1, TextMarkerInterface* tm2 )
{
  return tm1->priority() < tm2->priority();
}

void PluginManager::sortPlugins()
{
  std::sort( m_textMarkers.begin(), m_textMarkers.end(), TextMarkerForPriority );
}

TextMarkerInterface* PluginManager::textMarker( const QString& text_marker_name ) const
{
  foreach( TextMarkerInterface* tmi, m_textMarkers )
  {
    if( tmi->name() == text_marker_name )
      return tmi;
  }
  return Q_NULLPTR;
}

bool PluginManager::parseText( QString* p_txt, bool before_sending ) const
{
#ifdef BEEBEEP_DEBUG
  qDebug() << "Plugins parsing text:" << qPrintable( *p_txt );
#endif
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
#ifdef BEEBEEP_DEBUG
      qWarning() << text_marker->name() << "has break text marker plugins loop";
#endif
      return false;
    }
  }
#ifdef BEEBEEP_DEBUG
  qDebug() << "Plugins has parsed:" << qPrintable( *p_txt );
#endif
  return true;
}

bool PluginManager::parseTextWithPlugin( QString* p_txt, TextMarkerInterface* tmi ) const
{
#ifdef BEEBEEP_DEBUG
  qDebug() << "Plugin" << tmi->name() << "starts to parse the text:" << qPrintable( *p_txt );
#endif
  bool space_added_at_begin = false;
  bool space_added_at_end = false;

  if( p_txt->startsWith( tmi->openCommand().trimmed() ) )
  {
    p_txt->prepend( QChar( ' ' ) );
    space_added_at_begin = true;
  }

  if( p_txt->endsWith( tmi->closeCommand().trimmed() ) )
  {
    p_txt->append( QChar( ' ' ) );
    space_added_at_end = true;
  }

  QString parsed_text = "";
  int open_cmd_index = p_txt->indexOf( tmi->openCommand(), 0, Qt::CaseInsensitive );
  int open_cmd_size = tmi->openCommand().size();
  int close_cmd_size = tmi->closeCommand().size();
  QString html_tag = "";

  if( open_cmd_index >= 0 && open_cmd_index < p_txt->size() )
  {
    int close_cmd_index = p_txt->indexOf( tmi->closeCommand(), open_cmd_index+open_cmd_size, Qt::CaseInsensitive );
    if( close_cmd_index > open_cmd_index )
    {
#ifdef BEEBEEP_DEBUG
      qDebug() << tmi->name() << "has found open/close command";
#endif

      tmi->initParser( p_txt->mid( open_cmd_index+open_cmd_size, close_cmd_index-open_cmd_index-open_cmd_size ) );

      bool is_in_tag = false;
      int is_in_tag_code = 0;
      QString code_text = "";
      QChar c;

      for( int i = 0; i < p_txt->size(); i++ )
      {
        c = p_txt->at( i );
        if( c == QChar( '<' ) )
        {
          is_in_tag = true;
          html_tag = "";
        }

        if( is_in_tag )
        {
          parsed_text.append( c );
          html_tag.append( c );
          if( c == QChar( '>' ) )
          {
            is_in_tag = false;
            if( html_tag == QLatin1String( "<code>" ) )
            {
              is_in_tag_code++;
              if( is_in_tag_code <= 0 )
                is_in_tag_code = 1;
            }
            else
            {
              if( html_tag == QLatin1String( "</code>" ) )
              {
                is_in_tag_code--;
                if( is_in_tag_code < 0 )
                  is_in_tag_code = 0;
              }
            }
          }
          continue;
        }

        if( is_in_tag_code > 0 )
        {
          parsed_text.append( c );
          continue;
        }

        if( c == QChar( '&' ) )
        {
          QChar c_tmp;
          // Search forward until either a semicolon, tag, or space is found
          for( int j=(i+1); j < p_txt->size(); j++ )
          {
            c_tmp = p_txt->at( j );
            if( c_tmp == QChar( '<' ) || c_tmp == QChar( '>' )
                || c_tmp == QChar( ';' ) || c_tmp.isSpace() )
            {
              if( c_tmp == QChar( ';' ) )
              {
                code_text = p_txt->mid( i, j-i+1 );
#ifdef BEEBEEP_DEBUG
                qDebug() << "Code html found:" << code_text << "... skip it";
#endif
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
            parsed_text.append( tmi->openString() );
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
            parsed_text.append( tmi->closeString() );
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

bool PluginManager::fileCanBeSkipped( const QString& file_name ) const
{
   QStringList file_names_to_skip;
#ifdef Q_OS_WIN
   file_names_to_skip << QString( "msvcp100.dll" );
   file_names_to_skip << QString( "msvcr100.dll" );
   file_names_to_skip << QString( "msvcp120.dll" );
   file_names_to_skip << QString( "msvcr120.dll" );
   file_names_to_skip << QString( "msvcp140.dll" );
   file_names_to_skip << QString( "vccorlib140.dll" );
   file_names_to_skip << QString( "vcruntime140.dll" );
   file_names_to_skip << QString( "libEGL.dll" );
   file_names_to_skip << QString( "libGLESv2.dll" );
   file_names_to_skip << QString( "Qt5Core.dll" );
   file_names_to_skip << QString( "Qt5Gui.dll" );
   file_names_to_skip << QString( "Qt5Multimedia.dll" );
   file_names_to_skip << QString( "Qt5Network.dll" );
   file_names_to_skip << QString( "Qt5PrintSupport.dll" );
   file_names_to_skip << QString( "Qt5Svg.dll" );
   file_names_to_skip << QString( "Qt5Widgets.dll" );
   file_names_to_skip << QString( "Qt5WinExtras.dll" );
   file_names_to_skip << QString( "libeay32.dll" );
   file_names_to_skip << QString( "ssleay32.dll" );
#endif

  if( file_names_to_skip.isEmpty() )
    return false;
  else
    return file_names_to_skip.contains( file_name, Qt::CaseInsensitive );
}

