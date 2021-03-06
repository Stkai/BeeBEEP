//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2021 Marco Mastroddi
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

#ifndef BEEBEEP_FILEINFO_H
#define BEEBEEP_FILEINFO_H

#include "NetworkAddress.h"


class FileInfo
{
public:
  enum TransferType { Upload, Download };
  enum ContentType { File, VoiceMessage, NumContentTypes };

  static QString urlSchemeShowFileInFolder() { return QLatin1String( "beeshowfileinfolder" ); }
  static QString urlSchemeVoiceMessage() { return QLatin1String( "beevoicemessage" ); }

  FileInfo();
  FileInfo( VNumber, FileInfo::TransferType );
  inline FileInfo( const FileInfo& );

  FileInfo& operator=( const FileInfo& );
  inline bool operator==( const FileInfo& );
  bool operator<( const FileInfo& ) const;

  inline bool isValid() const;
  inline bool isDownload() const;
  inline TransferType transferType() const;
  inline void setTransferType( TransferType );
  inline const QString& name() const;
  inline void setName( const QString& );
  void setNameAndSuffix( const QString& file_name );
  inline const QString& path() const;
  inline void setPath( const QString& );
  inline const QString& suffix() const;
  inline void setSuffix( const QString& );
  inline FileSizeType size() const;
  inline void setSize( FileSizeType );
  inline const QString& shareFolder() const;
  inline void setShareFolder( const QString& );
  inline bool isFolder() const;
  inline void setIsFolder( bool );
  inline void setNetworkAddress( const NetworkAddress& );
  inline void setHostAddress( const QHostAddress& );
  inline void setHostPort( quint16 );
  inline const NetworkAddress& networkAddress() const;
  inline const QByteArray& password() const;
  inline void setPassword( const QByteArray& );
  inline VNumber id() const;
  inline void setId( VNumber );
  inline const QString& fileHash() const;
  inline void setFileHash( const QString& );
  inline const QDateTime& lastModified() const;
  inline void setLastModified( const QDateTime& );
  inline bool isInShareBox() const;
  inline void setIsInShareBox( bool );
  inline const QString& chatPrivateId() const;
  inline void setChatPrivateId( const QString& );
  inline const QString& mimeType() const;
  inline void setMimeType( const QString& );
  inline ContentType contentType() const;
  inline void setContentType( ContentType );
  inline bool isVoiceMessage() const;
  inline void setStartingPosition( FileSizeType );
  inline FileSizeType startingPosition() const;
  inline void setDuration( qint64 );
  inline qint64 duration() const;

private:
  TransferType m_transferType;
  QString m_name;
  QString m_path;
  QString m_suffix;
  FileSizeType m_size;
  QString m_shareFolder;
  bool m_isFolder;
  NetworkAddress m_networkAddress;
  QByteArray m_password;
  VNumber m_id;
  QString m_fileHash;
  QDateTime m_lastModified;
  bool m_isInShareBox;
  QString m_chatPrivateId;
  QString m_mimeType;
  ContentType m_contentType;
  FileSizeType m_startingPosition;
  qint64 m_duration;

};

typedef QPair<VNumber, FileInfo> SharedFileInfo;

// Inline Functions
inline FileInfo::FileInfo( const FileInfo& fi ) { (void)operator=( fi ); }
inline bool FileInfo::operator==( const FileInfo& fi ) { return m_id == fi.m_id; }
inline bool operator==( const FileInfo& fi1, const FileInfo& fi2 ) { return fi1.id() == fi2.id(); }
inline bool FileInfo::isValid() const { return m_id != ID_INVALID && m_name.size() > 0; }
inline bool FileInfo::isDownload() const { return m_transferType == FileInfo::Download; }
inline FileInfo::TransferType FileInfo::transferType() const { return m_transferType; }
inline void FileInfo::setTransferType( FileInfo::TransferType new_value ) { m_transferType = new_value; }
inline const QString& FileInfo::name() const { return m_name; }
inline void FileInfo::setName( const QString& new_value ) { m_name = new_value; }
inline const QString& FileInfo::path() const { return m_path; }
inline void FileInfo::setPath( const QString& new_value ) { m_path = new_value; }
inline const QString& FileInfo::suffix() const { return m_suffix; }
inline void FileInfo::setSuffix( const QString& new_value ) { m_suffix = new_value.toLower(); }
inline FileSizeType FileInfo::size() const { return m_size; }
inline void FileInfo::setSize( FileSizeType new_value ) { m_size = new_value > 0 ? new_value : 0; }
inline const QString& FileInfo::shareFolder() const { return m_shareFolder; }
inline void FileInfo::setShareFolder( const QString& new_value ) { m_shareFolder = new_value; }
inline bool FileInfo::isFolder() const { return m_isFolder; }
inline void FileInfo::setIsFolder( bool new_value ) { m_isFolder = new_value; }
inline void FileInfo::setNetworkAddress( const NetworkAddress& new_value ) { m_networkAddress = new_value; }
inline const NetworkAddress& FileInfo::networkAddress() const { return m_networkAddress; }
inline void FileInfo::setHostAddress( const QHostAddress& new_value ) { m_networkAddress.setHostAddress( new_value ); }
inline void FileInfo::setHostPort( quint16 new_value ) { m_networkAddress.setHostPort( new_value ); }
inline const QByteArray& FileInfo::password() const { return m_password; }
inline void FileInfo::setPassword( const QByteArray& new_value ) { m_password = new_value; }
inline VNumber FileInfo::id() const { return m_id; }
inline void FileInfo::setId( VNumber new_value ) { m_id = new_value; }
inline const QString& FileInfo::fileHash() const { return m_fileHash; }
inline void FileInfo::setFileHash( const QString& new_value ) { m_fileHash = new_value; }
inline const QDateTime& FileInfo::lastModified() const { return m_lastModified; }
inline void FileInfo::setLastModified( const QDateTime& new_value ) { m_lastModified = new_value; }
inline bool FileInfo::isInShareBox() const { return m_isInShareBox; }
inline void FileInfo::setIsInShareBox( bool new_value ) { m_isInShareBox = new_value; }
inline const QString& FileInfo::chatPrivateId() const { return m_chatPrivateId; }
inline void FileInfo::setChatPrivateId( const QString& new_value ) { m_chatPrivateId = new_value; }
inline const QString& FileInfo::mimeType() const { return m_mimeType; }
inline void FileInfo::setMimeType( const QString& new_value ) { m_mimeType = new_value; }
inline FileInfo::ContentType FileInfo::contentType() const { return m_contentType; }
inline void FileInfo::setContentType( ContentType new_value ) { m_contentType = new_value; }
inline bool FileInfo::isVoiceMessage() const { return m_contentType == VoiceMessage; }
inline void FileInfo::setStartingPosition( FileSizeType new_value ) { m_startingPosition = new_value > 0 ? new_value : 0; }
inline FileSizeType FileInfo::startingPosition() const { return m_startingPosition; }
inline void FileInfo::setDuration( qint64 new_value ) { m_duration = new_value; }
inline qint64 FileInfo::duration() const { return m_duration; }

#endif // BEEBEEP_FILEINFO_H
