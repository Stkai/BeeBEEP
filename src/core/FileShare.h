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

  int addToLocal( const QString&, const QList<FileInfo>& );
  int addToLocal( const FileInfo& );
  void clearLocal();
  int removePath( const QString& );
  FileInfo networkFileInfo( VNumber user_id, VNumber file_info_id ) const;
  QList<FileInfo> networkFolder( VNumber user_id, const QString& ) const;
  inline QList<FileInfo> fileSharedFromUser( VNumber ) const;
  FileInfo localFileInfo( VNumber file_info_id ) const;
  QList<FileInfo> localFolder( const QString& ) const;
  inline FileSizeType localSize( const QString& ) const;
  inline QList<FileInfo> fileSharedFromLocalUser() const;

  int addToNetwork( VNumber, const QList<FileInfo>& );
  int removeFromNetwork( VNumber );
  bool userHasFileShareList( VNumber ) const;
  void addDownloadedFile( const FileInfo& );
  FileInfo downloadedFile( const QString& ) const;
  inline bool isFileDownloaded( const QString& ) const;

  int addToShareBoxes( VNumber, const QList<FileInfo>& );
  int removeFromShareBoxes( VNumber );
  inline QList<FileInfo> shareBox( VNumber ) const;

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
  QMultiMap<VNumber, FileInfo> m_shareBoxes;
  QList<FileInfo> m_downloadedFiles;

};


// Inline Functions
inline const QMultiMap<QString, FileInfo>& FileShare::local() const { return m_local; }
inline const QMultiMap<VNumber, FileInfo>& FileShare::network() const { return m_network; }
inline FileSizeType FileShare::localSize( const QString& share_path ) const { return m_localSize.contains( share_path ) ? m_localSize.value( share_path ) : 0; }
inline QList<FileInfo> FileShare::fileSharedFromUser( VNumber user_id ) const { return m_network.values( user_id ); }
inline QList<FileInfo> FileShare::fileSharedFromLocalUser() const { return m_local.values(); }
inline bool FileShare::isFileDownloaded( const QString& file_info_hash ) const { return downloadedFile( file_info_hash ).isValid(); }
inline QList<FileInfo> FileShare::shareBox( VNumber user_id ) const { return m_shareBoxes.values( user_id ); }

#endif // BEEBEEP_FILESHARE_H
