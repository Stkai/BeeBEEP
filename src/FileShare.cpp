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

int FileShare::removePath( const QString& share_path )
{
  return m_local.remove( share_path );
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

FileInfo FileShare::localFileInfo( VNumber file_info_id ) const
{
  foreach( FileInfo fi, m_local )
  {
    if( fi.id() == file_info_id )
      return fi;
  }
  return FileInfo();
}

bool FileShare::userHasFileShareList( VNumber user_id ) const
{
  QList<FileInfo> file_info_list = m_network.values( user_id );
  if( !file_info_list.isEmpty() && file_info_list.first().isValid() )
    return true;
  else
    return false;
}
