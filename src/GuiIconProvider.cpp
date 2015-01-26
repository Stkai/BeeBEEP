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

#include "BeeUtils.h"
#include "FileInfo.h"
#include "GuiIconProvider.h"


GuiIconProvider* GuiIconProvider::mp_instance = NULL;


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
  m_cache.insert( "", iconFromFileType( Bee::FileOther ) );
}

QIcon GuiIconProvider::iconFromFileType( int file_type )
{
  switch( file_type )
  {
  case Bee::FileAudio:
    return QIcon( ":/images/file-audio.png" );
  case Bee::FileVideo:
    return QIcon( ":/images/file-video.png" );
  case Bee::FileImage:
    return QIcon( ":/images/file-image.png" );
  case Bee::FileDocument:
    return QIcon( ":/images/file-document.png" );
  case Bee::FileExe:
    return QIcon( ":/images/file-exe.png" );
  default:
    return QIcon( ":/images/file-other.png" );
  };
}

QIcon GuiIconProvider::findIcon( const FileInfo& file_info )
{
  // fixme for MacOSX
  if( m_cache.contains( file_info.suffix() ) )
    return m_cache.value( file_info.suffix() );

  QFileInfo fi( file_info.path() );
  if( fi.exists() )
  {
    QIcon icon_value = m_provider.icon( fi );
    if( !icon_value.isNull() )
    {
      m_cache.insert( file_info.suffix(), icon_value );
      return icon_value;
    }
  }

  return iconFromFileType( Bee::fileTypeFromSuffix( file_info.suffix() ) );
}

