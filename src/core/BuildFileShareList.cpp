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
#include "BuildFileShareList.h"
#include "Protocol.h"


BuildFileShareList::BuildFileShareList( QObject *parent )
  : QObject( parent ), m_folderPath( "" ), m_folderName( "" ),
    m_broadcastList( false ), m_shareList(), m_shareSize( 0 ),
    m_elapsedTime( 0 ), m_userId( ID_LOCAL_USER )
{
  setObjectName( "BuildFileShareList" );
}

void BuildFileShareList::setFolderPath( const QString& path_to_share )
{
  QDir dir_shared( path_to_share );
  m_folderName = dir_shared.dirName();
  m_folderPath = dir_shared.absolutePath();
#ifdef BEEBEEP_DEBUG
  qDebug() << "Building file share list" << m_folderName << "with path" << m_folderPath;
#endif
}

void BuildFileShareList::buildList()
{
  if( !m_shareList.isEmpty() )
    m_shareList.clear();

  m_shareSize = 0;
  QTime elapsed_time;
  elapsed_time.start();

  if( !m_folderPath.isEmpty() )
  {
    m_shareSize = addPathToList( m_folderName, m_folderPath );
    qSort( m_shareList );
  }

  m_elapsedTime = elapsed_time.elapsed();
#ifdef BEEBEEP_DEBUG
  foreach( FileInfo fi, m_shareList )
    qDebug() << "File shared" << fi.id() << "with path" << fi.path() << "and folder" << fi.shareFolder();
#endif
  emit listCompleted();
}

FileSizeType BuildFileShareList::addPathToList( const QString& path_name, const QString& path_url )
{
  QFileInfo path_info( path_url );

  if( !Protocol::instance().fileCanBeShared( path_info ) )
    return 0;

  if( path_info.isDir() )
  {
    FileSizeType path_size = 0;

    QDir dir_path( path_url );
    QString subfolder_name = path_url == m_folderPath ? m_folderName : Bee::convertToNativeFolderSeparator( QString( "%1/%2" ).arg( path_name, dir_path.dirName() ) );
#ifdef BEEBEEP_DEBUG
    qDebug() << "Subfolder " << subfolder_name << "found with path" << path_url;
#endif

    foreach( QString fp, dir_path.entryList() )
    {
      path_size += addPathToList( subfolder_name, Bee::convertToNativeFolderSeparator( QString( "%1/%2" ).arg( path_url, fp ) ) );
    }

    return path_size;
  }
  else if( path_info.isFile() )
  {
    FileInfo fi = Protocol::instance().fileInfo( path_info, path_name, false );
    m_shareList.append( fi );
    return fi.size();
  }
  else
  {
    qWarning() << "Path" << path_url << "is niether a file nor a folder (what is it?) and cannot be shared";
    return 0;
  }
}

