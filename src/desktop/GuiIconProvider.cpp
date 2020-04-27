//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2020 Marco Mastroddi
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

#include "BeeUtils.h"
#include "FileInfo.h"
#include "GuiIconProvider.h"
#include "IconManager.h"


GuiIconProvider* GuiIconProvider::mp_instance = Q_NULLPTR;


GuiIconProvider::GuiIconProvider()
  : m_cache(), m_provider()
{
  clearCache();
}

void GuiIconProvider::clearCache()
{
  if( !m_cache.isEmpty() )
    m_cache.clear();
  m_cache.insert( "exe", iconFromFileType( Bee::FileExe ) );
  m_cache.insert( "app", iconFromFileType( Bee::FileBundle ) );
  m_cache.insert( "dmg", iconFromFileType( Bee::FileBundle ) );
}

QIcon GuiIconProvider::iconFromFileType( int file_type )
{
  switch( file_type )
  {
  case Bee::FileAudio:
    return IconManager::instance().icon( "file-audio.png" );
  case Bee::FileVideo:
    return IconManager::instance().icon( "file-video.png" );
  case Bee::FileImage:
    return IconManager::instance().icon( "file-image.png" );
  case Bee::FileDocument:
    return IconManager::instance().icon( "file-document.png" );
  case Bee::FileExe:
    return IconManager::instance().icon( "file-exe.png" );
  case Bee::FileBundle:
    return IconManager::instance().icon( "file-bundle.png" );
  case Bee::FileCompressed:
    return IconManager::instance().icon( "file-compressed.png" );
  default:
    return IconManager::instance().icon( "file-other.png" );
  };
}

QIcon GuiIconProvider::findIcon( const FileInfo& file_info )
{
#ifdef Q_OS_WIN
  if( m_cache.contains( file_info.suffix() ) )
    return m_cache.value( file_info.suffix() );
#endif

#if QT_VERSION >= 0x050000
  #ifdef Q_OS_LINUX
    if( !file_info.mimeType().isEmpty() )
    {
      QMimeDatabase mime_db;
      QMimeType file_mt = mime_db.mimeTypeForName( file_info.mimeType() );
      if( file_mt.isValid() )
      {
        QIcon mime_icon = QIcon::fromTheme( file_mt.iconName() );
        if( !mime_icon.isNull() )
          return mime_icon;
      }
    }
  #endif
#endif

  QFileInfo fi( file_info.path() );
  if( fi.exists() )
  {
 #ifdef Q_OS_WIN
    QIcon icon_value = m_provider.icon( fi );
    if( !icon_value.isNull() )
    {
      if( fi.suffix().size() > 0 )
        m_cache.insert( fi.suffix(), icon_value );
      return icon_value;
    }
#endif
    if( fi.isBundle() )
      return iconFromFileType( Bee::FileBundle );
    if( fi.isExecutable() )
      return iconFromFileType( Bee::FileExe );
  }

  return iconFromFileType( Bee::fileTypeFromSuffix( file_info.suffix() ) );
}

