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

#ifndef BEEBEEP_GUIUTILS_H
#define BEEBEEP_GUIUTILS_H

#include "Config.h"
class User;


namespace Bee
{
  enum FileType { FileAudio, FileVideo, FileImage, FileDocument, FileOther, FileExe, FileBundle, NumFileType };

  inline QIcon userStatusIcon( int );
  QColor userStatusColor( int );
  QColor userStatusBackgroundColor( int );
  QColor userStatusForegroundColor( int );
  QString userStatusIconFileName( int );
  QString menuUserStatusIconFileName( int );
  QString userStatusToString( int );
  inline QString iconToHtml( const QString& icon_path, const QString& icon_alt, int icon_width = 0, int icon_height = 0 );
  inline VNumber qVariantToVNumber( const QVariant& );
  QString bytesToString( FileSizeType, int precision = -1 );
  QString elapsedTimeToString( int msec );
  QString uniqueFilePath( const QString&, bool add_date_time );
  QString suffixFromFile( const QString& );
  FileType fileTypeFromSuffix( const QString& );
  bool isFileTypeAudio( const QString& );
  bool isFileTypeVideo( const QString& );
  bool isFileTypeImage( const QString& );
  bool isFileTypeDocument( const QString& );
  bool isFileTypeExe( const QString& );
  bool isFileTypeBundle( const QString& );
  QString fileTypeToString( FileType );
  QString dateTimeStringSuffix( const QDateTime& );
  QString capitalizeFirstLetter( const QString&, bool all_chars_after_space );
  QColor invertColor( const QColor& );
  bool isColorNear( const QColor&, const QColor& );
  QString removeHtmlTags( const QString& );
  inline int toLittleEndianFromBig( int );
  QString chatMessageTypeToString( int );
  QBrush defaultTextBrush();
  QBrush defaultBackgroundBrush();
  QBrush defaultHighlightedTextBrush();
  QBrush defaultHighlightBrush();
  QBrush userStatusBackgroundBrush( int );
  QPixmap convertToGrayScale( const QPixmap& );
  QChar naviveFolderSeparator();
  QString convertToNativeFolderSeparator( const QString& );
  QString folderCdUp( const QString& );
  bool setLastModifiedToFile( const QString&, const QDateTime& );
  bool showFileInGraphicalShell( const QString& );
  bool folderIsWriteable( const QString& );
  QPixmap avatarForUser( const User&, const QSize&, bool use_available_user_image );
  QString toolTipForUser( const User&, bool only_status );
  void setWindowStaysOnTop( QWidget*, bool );
  QString stringListToTextString( const QStringList& );
}


// Inline Functions
inline QIcon Bee::userStatusIcon( int user_status ) { return QIcon( userStatusIconFileName( user_status ) ); }
inline QString Bee::iconToHtml( const QString& icon_path, const QString& icon_alt, int icon_width, int icon_height ) { return QString( "<img src='%1' width=%2 height=%3 border=0 alt=' %4 ' />" ).arg( icon_path ).arg( icon_width > 0 ? icon_width : 16 ).arg( icon_height > 0 ? icon_height : 16 ).arg( icon_alt ); }
inline VNumber Bee::qVariantToVNumber( const QVariant& v ) { return v.toULongLong(); }
inline int Bee::toLittleEndianFromBig( int big_endian_int ) { return (int) (0 | ((big_endian_int & 0x00ff) << 8) | ((big_endian_int & 0xff00) >> 8)); }

#endif // BEEBEEP_GUIUTILS_H
