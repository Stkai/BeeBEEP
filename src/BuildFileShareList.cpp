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

#include "BuildFileShareList.h"
#include "Protocol.h"


BuildFileShareList::BuildFileShareList( QObject *parent )
  : QObject( parent ), m_path( "" ), m_folder( "" ), m_broadcastList( false ),
    m_shareList(), m_shareSize( 0 ), m_elapsedTime( 0 )
{
  setObjectName( "BuildFileShareList" );
}

void BuildFileShareList::setPath( const QString& path_to_share )
{
  QFileInfo path_info( path_to_share );
  if( !path_info.exists() )
  {
    qWarning() << "Path" << path_to_share << "not exists and cannot be shared";
    m_path = "";
    return;
  }
  if( !path_info.isReadable() )
  {
    qWarning() << "Path" << path_to_share << "is not readable and cannot be shared";
    m_path = "";
    return;
  }

  m_path = path_to_share;

  if( path_info.isDir() )
    m_folder = path_info.fileName();
  else
    m_folder = "";
}

void BuildFileShareList::buildList()
{
  if( m_path.isEmpty() )
    return;
  if( !m_shareList.isEmpty() )
    m_shareList.clear();
  m_shareSize = 0;
  QTime elapsed_time;
  elapsed_time.start();
  addPathToList( m_path );
  m_elapsedTime = elapsed_time.elapsed();
  emit listCompleted();
}

void BuildFileShareList::addPathToList( const QString& share_path )
{
  QFileInfo file_info( share_path );
  if( file_info.isSymLink() )
  {
    // skip symbolic link
    return;
  }
  else if( file_info.isDir() )
  {
    if( share_path.endsWith( "." ) )
    {
      // skip folder . and folder ..
      return;
    }

    QDir dir_path( share_path );
    if( dir_path.exists() && dir_path.isReadable() )
    {
      foreach( QString fp, dir_path.entryList() )
        addPathToList( QDir::toNativeSeparators( share_path + QString( "/" ) + fp ) );
    }
  }
  else if( file_info.isFile() )
  {
    FileInfo fi = Protocol::instance().fileInfo( file_info );
    if( !m_folder.isEmpty() )
      fi.setFolder( m_folder );
    m_shareList.append( fi );
    m_shareSize += fi.size();
  }
  else
    qWarning() << "Unable to share path" << share_path;
}

