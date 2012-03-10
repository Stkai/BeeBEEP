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

#include "FileShare.h"
#include "Protocol.h"


FileShare* FileShare::mp_instance = NULL;

FileShare::FileShare()
  : m_local(), m_network()
{
}

void FileShare::addPath( const QString& file_path )
{
  QFileInfo file_info( file_path );
  if( file_info.isSymLink() )
  {
    qDebug() << "FileShare: skip symbolic link" << file_path;
    return;
  }
  else if( file_info.isDir() )
  {
    foreach( QString fp, file_info.dir().entryList() )
      addPath( fp );
  }
  else if( file_info.isFile() )
  {
    addFileInfo( file_path );
  }
  else
    qDebug() << "FileShare: invalid file type from path" << file_path;

}

void FileShare::addFileInfo( const QFileInfo& fi )
{
  if( hasFilePath( fi.absoluteFilePath() ) )
  {
    qDebug() << "FileShare:" << fi.absoluteFilePath() << "is already in share list";
    return;
  }
  FileInfo file_info = Protocol::instance().fileInfo( fi );
  qDebug() << "FileShare: adding file" << file_info.path();
  m_local.append( file_info );
}

bool FileShare::hasFilePath( const QString& file_path )
{
  foreach( FileInfo fi, m_local )
  {
    if( fi.path() == file_path )
      return true;
  }
  return false;
}
