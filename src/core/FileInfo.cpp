//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2019 Marco Mastroddi
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
// along with BeeBEEP. If not, see <http://www.gnu.org/licenses/>.
//
// Author: Marco Mastroddi <marco.mastroddi(AT)gmail.com>
//
// $Id$
//
//////////////////////////////////////////////////////////////////////

#include "FileInfo.h"


FileInfo::FileInfo()
  : m_transferType( FileInfo::Upload ), m_name( "" ), m_path( "" ), m_suffix( "" ),
    m_size( 0 ), m_shareFolder( "" ), m_isFolder( false ), m_networkAddress(),
    m_password( "" ), m_id( ID_INVALID ), m_fileHash(), m_lastModified(),
    m_isInShareBox( false ), m_chatPrivateId( "" ), m_mimeType( "" ),
    m_contentType( File ), m_startingPosition( 0 ), m_duration( -1 )
{
}

FileInfo::FileInfo( VNumber id, FileInfo::TransferType tt )
  : m_transferType( tt ), m_name( "" ), m_path( "" ), m_suffix( "" ),
    m_size( 0 ), m_shareFolder( "" ), m_isFolder( false ), m_networkAddress(),
    m_password( "" ), m_id( id ), m_fileHash(), m_lastModified(),
    m_isInShareBox( false ), m_chatPrivateId( "" ), m_mimeType( "" ),
    m_contentType( File ), m_startingPosition( 0 ), m_duration( -1 )
{
}

FileInfo& FileInfo::operator=( const FileInfo& fi )
{
  if( this != &fi )
  {
    m_transferType = fi.m_transferType;
    m_name = fi.m_name;
    m_path = fi.m_path;
    m_suffix = fi.m_suffix;
    m_size = fi.m_size;
    m_shareFolder = fi.m_shareFolder;
    m_isFolder = fi.m_isFolder;
    m_networkAddress = fi.m_networkAddress;
    m_password = fi.m_password;
    m_id =  fi.m_id;
    m_fileHash = fi.m_fileHash;
    m_lastModified = fi.m_lastModified;
    m_isInShareBox = fi.m_isInShareBox;
    m_chatPrivateId = fi.m_chatPrivateId;
    m_mimeType = fi.m_mimeType;
    m_contentType = fi.m_contentType;
    m_startingPosition = fi.m_startingPosition;
    m_duration = fi.m_duration;
  }
  return *this;
}

bool FileInfo::operator<( const FileInfo& fi ) const
{
  if( isFolder() && !fi.isFolder() )
    return false;
  if( !isFolder() && !fi.isFolder() )
    return true;
  if( m_path.isEmpty() || fi.m_path.isEmpty() )
    return m_name < fi.m_name;
  else
    return m_path < fi.m_path;
}
