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
  enum FileType { FileAudio, FileVideo, FileImage, FileDocument, FileOther, FileExe, FileBundle, FileCompressed, NumFileType };

  inline QIcon userStatusIcon( int );
  QColor userStatusColor( int );
  QColor userStatusBackgroundColor( int );
  QColor userStatusForegroundColor( int );
  QString userStatusIconFileName( int );
  QString menuUserStatusIconFileName( int );
  QString userStatusToString( int );
  QString userBirthdayToText( const User& );
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
  bool isFileTypeCompressed( const QString& );
  QString fileTypeToString( FileType );
  QString dateTimeStringSuffix( const QDateTime& );
  QString capitalizeFirstLetter( const QString&, bool all_chars_after_space, bool lower_all_characters = true );
  QColor invertColor( const QColor& );
  bool isColorNear( const QColor&, const QColor& );
  QString removeHtmlTags( const QString& );
  QString replaceHtmlSpecialCharacters( const QString& );
  inline int toLittleEndianFromBig( int );
  QString chatMessageTypeToString( int );
  QBrush defaultTextBrush();
  QBrush defaultBackgroundBrush();
  QBrush defaultHighlightedTextBrush();
  QBrush defaultHighlightBrush();
  QBrush userStatusBackgroundBrush( int );
  QPixmap convertToGrayScale( const QPixmap& );
  QPixmap convertToGrayScale( const QIcon&, const QSize& pixmap_size );
  QChar naviveFolderSeparator();
  QString convertToNativeFolderSeparator( const QString& );
  QString folderCdUp( const QString& );
  bool setLastModifiedToFile( const QString&, const QDateTime& );
  bool showFileInGraphicalShell( const QString& );
  bool folderIsWriteable( const QString& );
  QPixmap avatarForUser( const User&, const QSize&, bool use_available_user_image, int user_status = -1 );
  QString toolTipForUser( const User&, bool only_status );
  void setWindowStaysOnTop( QWidget*, bool );
  QString stringListToTextString( const QStringList&, int max_items = -1 );
  void removeContextHelpButton( QWidget* );
  QColor selectColor( QWidget*, const QColor& default_color );
  void setBackgroundColor( QWidget*, const QColor& );
  inline bool isTimeToCheck( int ticks, int tick_for_check );
  void showUp( QWidget* );
  void raiseOnTop( QWidget* );
  bool areStringListEqual( const QStringList&, const QStringList&, Qt::CaseSensitivity cs = Qt::CaseInsensitive );
  QString dateTimeToString( const QDateTime& );
  QString beeColorsToHtmlText( const QString& );
  QString pluginFileExtension();
  QString removeInvalidCharactersForFilePath( const QString& );

}


// Inline Functions
inline QIcon Bee::userStatusIcon( int user_status ) { return QIcon( userStatusIconFileName( user_status ) ); }
inline VNumber Bee::qVariantToVNumber( const QVariant& v ) { return v.toULongLong(); }
inline int Bee::toLittleEndianFromBig( int big_endian_int ) { return static_cast<int>(0 | ((big_endian_int & 0x00ff) << 8) | ((big_endian_int & 0xff00) >> 8)); }
inline bool Bee::isTimeToCheck( int ticks, int tick_for_check ) { return ticks % tick_for_check == 0; }

#endif // BEEBEEP_GUIUTILS_H
