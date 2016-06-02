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
#include "BuildFileList.h"
#include "Protocol.h"


BuildFileList::BuildFileList( QObject *parent )
  : QObject( parent ), m_folderPath( "" ), m_folderName( "" ),
    m_toUserId( ID_LOCAL_USER ), m_fileList(), m_elapsedTime( 0 ),
    m_errorFound( false )
{
  setObjectName( "BuildFileList" );
}

void BuildFileList::init( const QString& folder_name, const QString folder_path, VNumber to_user_id )
{
  m_folderName = folder_name;
  m_folderPath = folder_path;
  m_toUserId = to_user_id;
#ifdef BEEBEEP_DEBUG
  qDebug() << "Building file list of folder" << m_folderName << "with path" << m_folderPath << "of user" << to_user_id;
#endif
}

void BuildFileList::buildList()
{
  if( !m_fileList.isEmpty() )
    m_fileList.clear();
  QTime elapsed_time;
  elapsed_time.start();
  m_errorFound = true;

  if( !m_folderPath.isEmpty() )
  {
    QFileInfo box_info( m_folderPath );
    if( Protocol::instance().fileCanBeShared( box_info ) )
    {
      if( box_info.isDir() )
      {
        QDir dir_path( m_folderPath );
        foreach( QFileInfo fi, dir_path.entryInfoList() )
        {
          if( Protocol::instance().fileCanBeShared( fi ) )
          {
            FileInfo file_info = Protocol::instance().fileInfo( fi, "", true );
            if( file_info.isValid() )
              m_fileList.append( file_info );
          }
        }
        qSort( m_fileList );
      }
      else
      {
        FileInfo file_info = Protocol::instance().fileInfo( box_info, "", true );
        if( file_info.isValid() )
          m_fileList.append( file_info );
      }
      m_errorFound = false;
    }
  }

  m_elapsedTime = elapsed_time.elapsed();
  emit listCompleted();
}
