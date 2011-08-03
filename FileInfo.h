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

#ifndef BEEBEEP_FILEINFO_H
#define BEEBEEP_FILEINFO_H

#include "Config.h"


class FileInfo
{
public:
  FileInfo();
  inline FileInfo( const FileInfo& );

  FileInfo& operator=( const FileInfo& );

  inline bool isValid() const;
  inline const QString& name() const;
  inline void setName( const QString& );
  inline const QString& path() const;
  inline void setPath( const QString& );
  inline int size() const;
  inline void setSize( int );
  inline const QHostAddress& hostAddress() const;
  inline void setHostAddress( const QHostAddress& );
  inline int hostPort() const;
  inline void setHostPort( int );
  inline const QString& password() const;
  inline void setPassword( const QString& );

private:
  QString m_name;
  QString m_path;
  int m_size;
  QHostAddress m_hostAddress;
  int m_hostPort;
  QString m_password;

};


// Inline Functions
inline FileInfo::FileInfo( const FileInfo& fi ) { (void)operator=( fi ); }
inline bool FileInfo::isValid() const { return m_name.size() > 0; }
inline const QString& FileInfo::name() const { return m_name; }
inline void FileInfo::setName( const QString& new_value ) { m_name = new_value; }
inline const QString& FileInfo::path() const { return m_path; }
inline void FileInfo::setPath( const QString& new_value ) { m_path = new_value; }
inline int FileInfo::size() const { return m_size; }
inline void FileInfo::setSize( int new_value ) { m_size = new_value; }
inline const QHostAddress& FileInfo::hostAddress() const { return m_hostAddress; }
inline void FileInfo::setHostAddress( const QHostAddress& new_value ) { m_hostAddress = new_value; }
inline int FileInfo::hostPort() const { return m_hostPort; }
inline void FileInfo::setHostPort( int new_value ) { m_hostPort = new_value; }
inline const QString& FileInfo::password() const { return m_password; }
inline void FileInfo::setPassword( const QString& new_value ) { m_password = new_value; }

#endif // BEEBEEP_FILEINFO_H
