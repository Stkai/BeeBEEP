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
  : QObject( parent ), m_path( "" ), m_shareFolder( "" ),
    m_broadcastList( false ), m_shareList(), m_shareSize( 0 ),
    m_elapsedTime( 0 ), m_userId( ID_LOCAL_USER )
{
  setObjectName( "BuildFileShareList" );
}

void BuildFileShareList::setPath( const QString& path_to_share )
{
  m_path = path_to_share;
  m_shareFolder = "";
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
  m_shareSize = addPathToList( m_path );
  m_elapsedTime = elapsed_time.elapsed();
  qSort( m_shareList );
#ifdef BEEBEEP_DEBUG
  foreach( FileInfo fi, m_shareList )
    qDebug() << "File shared" << fi.id() << "with path" << fi.path() << "and folder" << fi.shareFolder();
#endif
  emit listCompleted();
}

FileSizeType BuildFileShareList::addPathToList( const QString& share_path )
{
  QFileInfo path_info( share_path );

  if( !Protocol::instance().fileCanBeShared( path_info ) )
    return 0;

  if( path_info.isDir() )
  {
    FileSizeType path_size = 0;

    if( share_path == m_path )
    {
      m_shareFolder = path_info.fileName();
      if( m_shareFolder.isEmpty() )
        m_shareFolder = path_info.dir().dirName();
#ifdef BEEBEEP_DEBUG
      qDebug() << "Share folder found:" << m_shareFolder;
#endif
    }

    QDir dir_path( share_path );
    foreach( QString fp, dir_path.entryList() )
    {
      path_size += addPathToList( QDir::toNativeSeparators( share_path + QString( "/" ) + fp ) );
    }

    return path_size;
  }
  else if( path_info.isFile() )
  {
    FileInfo fi = Protocol::instance().fileInfo( path_info, m_shareFolder );
    m_shareList.append( fi );
    return fi.size();
  }
  else
  {
    qWarning() << "Path" << share_path << "is niether a file nor a folder (what is it?) and cannot be shared";
    return 0;
  }
}

