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
// Author: Marco Mastroddi <marco.mastroddi(AT)gmail.com>
//
// $Id$
//
//////////////////////////////////////////////////////////////////////

#include "BeeUtils.h"
#include "FileShare.h"
#include "Protocol.h"
#include "Settings.h"


FileShare* FileShare::mp_instance = NULL;

FileShare::FileShare()
  : m_local(), m_localSize(), m_network(), m_downloadedFiles()
{
}

int FileShare::removePath( const QString& share_path )
{
  m_localSize.remove( share_path );
  return m_local.remove( share_path );
}

void FileShare::clearLocal()
{
  m_local.clear();
  m_localSize.clear();
}

void FileShare::addToLocal( const QString& sp, const QList<FileInfo>& share_list, FileSizeType share_size )
{
  QString share_path = Bee::convertToNativeFolderSeparator( sp );
  m_localSize.insert( share_path, share_size );
  if( m_local.contains( share_path ) )
    m_local.remove( share_path );
  if( share_list.isEmpty() )
  {
    qWarning() << "Fileshare can not add empty file list for path" << share_path;
    return;
  }
  else
    qDebug() << "FileShare shares" << share_list.size() << "files for path" << share_path;

  foreach( FileInfo fi, share_list )
    m_local.insert( share_path, fi );
}

void FileShare::addToLocal( const FileInfo& file_info )
{
  QString share_path = Bee::convertToNativeFolderSeparator( file_info.path() );
  m_localSize.insert( share_path, file_info.size() );
  if( m_local.contains( share_path ) )
    m_local.remove( share_path );
  m_local.insert( share_path, file_info );
}

int FileShare::addToNetwork( VNumber user_id, const QList<FileInfo>& file_info_list )
{
  removeFromNetwork( user_id );
  int num_files = 0;
  foreach( FileInfo fi, file_info_list )
  {
    m_network.insert( user_id, fi );
    num_files++;
  }
  return num_files;
}

int FileShare::removeFromNetwork( VNumber user_id )
{
  return m_network.remove( user_id );
}

FileInfo FileShare::networkFileInfo( VNumber user_id, VNumber file_info_id ) const
{
  QList<FileInfo> file_info_list = m_network.values( user_id );
  foreach( FileInfo fi, file_info_list )
  {
    if( fi.id() == file_info_id )
      return fi;
  }
  return FileInfo();
}

QList<FileInfo> FileShare::networkFolder( VNumber user_id, const QString& folder_name ) const
{
  QList<FileInfo> folder_file_info;
  QMultiMap<VNumber, FileInfo>::const_iterator it = m_network.find( user_id );
  while( it != m_network.end() && it.key() == user_id )
  {
    if( it.value().shareFolder() == folder_name )
      folder_file_info.append( it.value() );
     ++it;
  }

  return folder_file_info;
}

FileInfo FileShare::localFileInfo( VNumber file_info_id ) const
{
  QMultiMap<QString, FileInfo>::const_iterator it = m_local.begin();
  while( it != m_local.end() )
  {
    if( it.value().id() == file_info_id )
      return it.value();
    ++it;
  }
  return FileInfo();
}

bool FileShare::userHasFileShareList( VNumber user_id ) const
{
  QMultiMap<VNumber, FileInfo>::const_iterator it = m_network.find( user_id );
  return it != m_network.end() && it.value().isValid();
}

void FileShare::addDownloadedFile( const FileInfo& file_info )
{
  QList<FileInfo>::iterator it = m_downloadedFiles.begin();
  while( it != m_downloadedFiles.end() )
  {
    if( it->fileHash() == file_info.fileHash() )
    {
      *it = file_info;
      return;
    }
    ++it;
  }
  m_downloadedFiles.append( file_info );
}

FileInfo FileShare::downloadedFile( const QString& file_info_hash ) const
{
  foreach( FileInfo fi, m_downloadedFiles )
  {
    if( fi.fileHash() == file_info_hash )
      return fi;
  }
  return FileInfo();
}
