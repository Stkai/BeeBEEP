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

#ifndef BEEBEEP_FILESHARE_H
#define BEEBEEP_FILESHARE_H

#include "FileInfo.h"


class FileShare
{
// Singleton Object
  static FileShare* mp_instance;

public:
  inline const QMultiMap<QString, FileInfo>& local() const;
  inline const QMultiMap<VNumber, FileInfo>& network() const;

  void addToLocal( const QString&, const QList<FileInfo>&, FileSizeType );
  void clearLocal();
  int removePath( const QString& );
  FileInfo networkFileInfo( VNumber user_id, VNumber file_info_id ) const;
  FileInfo localFileInfo( VNumber file_info_id ) const;

  int addToNetwork( VNumber, const QList<FileInfo>& );
  int removeFromNetwork( VNumber );
  bool userHasFileShareList( VNumber ) const;

  static FileShare& instance()
  {
    if( !mp_instance )
      mp_instance = new FileShare();
    return *mp_instance;
  }

  static void close()
  {
    if( mp_instance )
    {
      delete mp_instance;
      mp_instance = NULL;
    }
  }

protected:
  FileShare();

private:
  QMultiMap<QString, FileInfo> m_local;
  QMap<QString, FileSizeType> m_localSize;
  QMultiMap<VNumber, FileInfo> m_network;

};


// Inline Functions
inline const QMultiMap<QString, FileInfo>& FileShare::local() const { return m_local; }
inline const QMultiMap<VNumber, FileInfo>& FileShare::network() const { return m_network; }

#endif // BEEBEEP_FILESHARE_H
