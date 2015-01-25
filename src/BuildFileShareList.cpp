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
// $Id: Broadcaster.cpp 267 2014-11-19 18:56:34Z mastroddi $
//
//////////////////////////////////////////////////////////////////////

#include "BuildFileShareList.h"
#include "Protocol.h"


BuildFileShareList::BuildFileShareList( QObject *parent )
  : QObject( parent )
{
  setObjectName( "BuildFileShareList" );
  m_broadcastList = false;
}

void BuildFileShareList::buildList()
{
  m_stopBuilding = false;
  addPathToList( m_path, m_path );
  if( m_stopBuilding )
    qDebug() << "Building list of shared files cancelled by user";
  else
    emit listCompleted();
}

void BuildFileShareList::stopBuilding()
{
  m_stopBuilding = true;
}

void BuildFileShareList::addPathToList( const QString& share_key, const QString& share_path )
{
  if( m_stopBuilding )
    return;

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

    foreach( QString fp, dir_path.entryList() )
    {
      if( m_stopBuilding )
        break;
      addPathToList( share_key, QDir::toNativeSeparators( share_path + QString( "/" ) + fp ) );
    }
  }
  else if( file_info.isFile() )
  {
    FileInfo fi = Protocol::instance().fileInfo( file_info );
    m_shareList.insert( share_key, fi );
  }
  else
    qWarning() << "Unable to share path" << share_path;
}

