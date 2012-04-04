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
#include "Settings.h"


FileShare* FileShare::mp_instance = NULL;

FileShare::FileShare()
  : m_local(), m_network()
{
}

int FileShare::addPath( const QString& share_path )
{
  int num_files = 0;

  if( m_local.size() >= Settings::instance().maxFileShared() )
  {
    qDebug() << "FileShare: max file shared reached" << m_local.size();
    return num_files;
  }

  QFileInfo file_info( share_path );
  if( file_info.isSymLink() )
  {
    qDebug() << "FileShare: skip symbolic link" << share_path;
    return num_files;
  }
  else if( file_info.isDir() )
  {
    if( share_path.endsWith( "." ) )
    {
      qDebug() << "FileShare: skip dir" << share_path;
      return num_files;
    }

    QDir dir_path( share_path );

    foreach( QString fp, dir_path.entryList() )
      num_files += addPath( QDir::toNativeSeparators( share_path + QString( "/" ) + fp ) );
  }
  else if( file_info.isFile() )
  {
    if( addFileInfo( share_path ) )
      num_files++;
  }
  else
    qDebug() << "FileShare: invalid file type from path" << share_path;

  return num_files;
}

bool FileShare::addFileInfo( const QFileInfo& fi )
{
  if( hasPath( fi.absoluteFilePath() ) )
  {
    qDebug() << "FileShare:" << fi.absoluteFilePath() << "is already in share list";
    return false;
  }
  FileInfo file_info = Protocol::instance().fileInfo( fi );
  qDebug() << "FileShare: adding file" << file_info.path();
  m_local.append( file_info );
  return true;
}

bool FileShare::hasPath( const QString& share_path )
{
  foreach( FileInfo fi, m_local )
  {
    if( fi.path() == share_path )
      return true;
  }
  return false;
}

void FileShare::addToNetwork( VNumber user_id, const QList<FileInfo>& file_info_list )
{
  removeFromNetwork( user_id );
  foreach( FileInfo fi, file_info_list )
  {
    m_network.insert( user_id, fi );
  }
}

void FileShare::removeFromNetwork( VNumber user_id )
{
  m_network.remove( user_id );
}
