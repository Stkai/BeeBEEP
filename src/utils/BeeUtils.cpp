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

#include "Avatar.h"
#include "BeeUtils.h"
#include "ChatMessage.h"
#include "IconManager.h"
#include "MessageManager.h"
#include "PluginManager.h"
#include "Protocol.h"
#include "User.h"
#include "Settings.h"
#if QT_VERSION < 0x050000
  #ifdef Q_OS_WIN
    #include <Windows.h>
  #endif
#endif
#ifndef Q_OS_WIN
  #include <utime.h>
  #include <errno.h>
#endif


QString Bee::userStatusIconFileName( int user_status )
{
  switch( user_status )
  {
  case User::Offline:
    return IconManager::instance().iconPath( "user-offline.png" );
  case User::Away:
    return IconManager::instance().iconPath( "user-away.png" );
  case User::Busy:
    return IconManager::instance().iconPath( "user-busy.png" );
  default:
    return IconManager::instance().iconPath( "user-online.png" );
  }
}

QString Bee::menuUserStatusIconFileName( int user_status )
{
  switch( user_status )
  {
  case User::Offline:
    return IconManager::instance().iconPath( "menu-user-offline.png" );
  case User::Away:
    return IconManager::instance().iconPath( "menu-user-away.png" );
  case User::Busy:
    return IconManager::instance().iconPath( "menu-user-busy.png" );
  default:
    return IconManager::instance().iconPath( "menu-user-online.png" );
  }
}

static const char* UserStatusToString[] =
{
  QT_TRANSLATE_NOOP( "User", "offline" ),
  QT_TRANSLATE_NOOP( "User", "available" ),
  QT_TRANSLATE_NOOP( "User", "busy" ),
  QT_TRANSLATE_NOOP( "User", "away" ),
  QT_TRANSLATE_NOOP( "User", "status error" ),
};

QString Bee::userStatusToString( int user_status )
{
  if( user_status < 0 || user_status > User::NumStatus )
    user_status = User::NumStatus;
  return qApp->translate( "User", UserStatusToString[ user_status ] );
}

QColor Bee::userStatusColor( int user_status )
{
  switch( user_status )
  {
  case User::Online:
    return QColor( Qt::green );
  case User::Away:
    return QColor( Qt::yellow );
  case User::Busy:
    return QColor( Qt::red );
  case User::Offline:
    return QColor( Qt::gray );
  default:
    return QColor( Qt::black );
  }
}

QColor Bee::userStatusBackgroundColor( int user_status )
{
  switch( user_status )
  {
  case User::Online:
    return defaultBackgroundBrush().color();
  case User::Away:
    return QColor( Qt::darkYellow );
  case User::Busy:
    return QColor( Qt::darkRed );
  case User::Offline:
    return QColor( Qt::gray );
  default:
    return defaultBackgroundBrush().color();
  }
}

QColor Bee::userStatusForegroundColor( int user_status )
{
  switch( user_status )
  {
  case User::Online:
    return defaultBackgroundBrush().color();
  case User::Away:
    return QColor( Qt::white );
  case User::Busy:
    return QColor( Qt::white );
  case User::Offline:
    return QColor( Qt::black );
  default:
    return defaultBackgroundBrush().color();
  }
}

QString Bee::userNameToShow( const User& u, bool to_html )
{
  QString user_name = Settings::instance().useUserFullName() && u.vCard().hasFullName() ? u.vCard().fullName( Settings::instance().useUserFirstNameFirstInFullName() ) : u.name();
  if( Settings::instance().appendHostNameToUserName() && !u.localHostName().isEmpty() )
    user_name.append( QString( " [%1]" ).arg( u.localHostName() ) );
  return to_html ? Bee::replaceHtmlSpecialCharacters( user_name ) : user_name;
}

QString Bee::uniqueFilePath( const QString& file_path, bool add_date_time )
{
  int counter = add_date_time ? 0 : 1;
  QFileInfo fi( file_path );
  QString dir_path = fi.absoluteDir().absolutePath();
  QString file_base_name = fi.completeBaseName();
  QString file_suffix = fi.suffix();
  QString new_file_name;

  while( fi.exists() )
  {
    new_file_name = QString( "%1%2%3%4" )
                      .arg( file_base_name )
                      .arg( add_date_time ? QString( "-%1" ).arg( QDateTime::currentDateTime().toString( "yyyyMMddHHmmss" ) ) : QString( "" ) )
                      .arg( counter > 0 ? ( add_date_time ? QString( "-%1" ).arg( counter ) : QString( " (%1)" ).arg( counter ) ) : QString( "" ) )
                      .arg( file_suffix.isEmpty() ? QString( "" ) : QString( ".%1" ) ).arg( file_suffix );
    fi.setFile( dir_path, new_file_name );
    counter++;

    if( counter > 98 )
    {
      qWarning() << "Unable to find a unique file name from path" << file_path << "(so overwrite the last one)";
      break;
    }
  }

  return QDir::toNativeSeparators( fi.absoluteFilePath() );
}

bool Bee::isLocalFile( const QUrl& file_url )
{
  if( file_url.scheme().toLower() == QLatin1String( "file" ) )
    return true;

  QString file_path = Bee::convertToNativeFolderSeparator( file_url.toLocalFile() );
#if defined( Q_OS_WIN ) || defined ( Q_OS_OS2 )
  if( file_path.size() > 2 && file_path.at( 1 ) == ":" )
    return true;
  if( file_path.startsWith( "\\" ) )
    return true;
#endif

#if defined( Q_OS_UNIX )
  if( file_path.startsWith( "/" ) )
    return true;
#endif

#if QT_VERSION >= 0x040800
  return file_url.isLocalFile();
#else
  return false;
#endif
}

QString Bee::suffixFromFile( const QString& file_path )
{
  if( file_path.isEmpty() )
    return "";
  QStringList sl = file_path.split( "." );
  if( sl.size() > 1 )
    return sl.last();
  else
    return "";
}

bool Bee::isFileTypeAudio( const QString& file_suffix )
{
  QString sx = file_suffix.toLower();
  return sx == "mp3" || sx == "wav" || sx == "wma" || sx == "flac" || sx == "aiff" || sx == "aac" || sx == "m4a" || sx == "m4p" ||
         sx == "ogg" || sx == "oga" || sx == "ra" || sx == "rm";
}

bool Bee::isFileTypeVideo( const QString& file_suffix )
{
  QString sx = file_suffix.toLower();
  return sx ==  "mpeg" || sx ==  "mpg" || sx == "mp4" || sx == "avi" || sx == "mkv" || sx == "wmv" || sx == "flv" || sx ==  "mov" ||
         sx ==  "3gp" || sx ==  "mpe";
}

bool Bee::isFileTypeImage( const QString& file_suffix )
{
  QString sx = file_suffix.toLower();
  return sx == "jpg" || sx == "jpeg" || sx == "gif" || sx == "bmp" || sx == "png" || sx == "tiff" || sx == "tif" || sx == "psd" ||
         sx == "nef" || sx == "cr2"  || sx == "dng" || sx == "dcr" || sx == "3fr" || sx == "raf" || sx == "orf" || sx == "pef" ||
         sx == "arw" || sx == "svg" || sx == "ico" || sx == "ppm" || sx == "pgm" || sx == "pbm" || sx == "pnm" || sx == "webp";
}

bool Bee::isFileTypeDocument( const QString& file_suffix )
{
  QString sx = file_suffix.toLower();
  return sx ==  "pdf" || sx.startsWith( "doc" ) || sx.startsWith( "xls" ) || sx.startsWith( "ppt" ) || sx.startsWith( "pps" ) ||
         sx ==  "rtf" || sx ==  "txt" || sx ==  "odt" || sx ==  "odp" || sx ==  "ods" || sx ==  "csv" || sx ==  "log" ||
         sx ==  "mobi" || sx ==  "epub";
}

bool Bee::isFileTypeExe( const QString& file_suffix )
{
  QString sx = file_suffix.toLower();
  return sx == "exe" || sx == "bat" || sx == "inf" || sx == "com" || sx == "sh" || sx == "cab" || sx == "cmd" || sx == "bin";
}

bool Bee::isFileTypeBundle( const QString& file_suffix )
{
  QString sx = file_suffix.toLower();
  return sx == "app" || sx == "dmg";
}

bool Bee::isFileTypeCompressed( const QString& file_suffix )
{
  QString sx = file_suffix.toLower();
  return sx == "zip" || sx == "rar" || sx == "7z" || sx == "gz";
}

Bee::FileType Bee::fileTypeFromSuffix( const QString& file_suffix )
{
  if( isFileTypeDocument( file_suffix ) )
    return Bee::FileDocument;

  if( isFileTypeImage( file_suffix ) )
    return Bee::FileImage;

  if( isFileTypeAudio( file_suffix ) )
    return Bee::FileAudio;

  if( isFileTypeVideo( file_suffix ) )
    return Bee::FileVideo;

  if( isFileTypeCompressed( file_suffix ) )
    return Bee::FileCompressed;

  if( isFileTypeExe( file_suffix ) )
    return Bee::FileExe;

  if( isFileTypeBundle( file_suffix ) )
    return Bee::FileBundle;

  return Bee::FileOther;
}

static const char* FileTypeToString[] =
{
  QT_TRANSLATE_NOOP( "File", "Audio" ),
  QT_TRANSLATE_NOOP( "File", "Video" ),
  QT_TRANSLATE_NOOP( "File", "Image" ),
  QT_TRANSLATE_NOOP( "File", "Document" ),
  QT_TRANSLATE_NOOP( "File", "Other" ),
  QT_TRANSLATE_NOOP( "File", "Executable" ),
  QT_TRANSLATE_NOOP( "File", "MacOSX" ),
  QT_TRANSLATE_NOOP( "File", "Compressed" )
};

QString Bee::fileTypeToString( Bee::FileType ft )
{
  if( static_cast<int>( ft ) < 0 || ft > Bee::NumFileType )
    ft = Bee::FileOther;
  return qApp->translate( "File", FileTypeToString[ ft ] );
}

static const char* ChatMessageTypeToString[] =
{
  QT_TRANSLATE_NOOP( "ChatMessage", "Header" ),
  QT_TRANSLATE_NOOP( "ChatMessage", "System" ),
  QT_TRANSLATE_NOOP( "ChatMessage", "Chat" ),
  QT_TRANSLATE_NOOP( "ChatMessage", "Connection" ),
  QT_TRANSLATE_NOOP( "ChatMessage", "User Information" ),
  QT_TRANSLATE_NOOP( "ChatMessage", "File Transfer" ),
  QT_TRANSLATE_NOOP( "ChatMessage", "History" ),
  QT_TRANSLATE_NOOP( "ChatMessage", "Other" ),
  QT_TRANSLATE_NOOP( "ChatMessage", "Image Preview" ),
  QT_TRANSLATE_NOOP( "ChatMessage", "Autoresponder" ),
  QT_TRANSLATE_NOOP( "ChatMessage", "Voice message" )
};

QString Bee::chatMessageTypeToString( int cmt )
{
  if( cmt < 0 || cmt > ChatMessage::NumTypes )
    cmt = ChatMessage::Other;
  return qApp->translate( "ChatMessage", ChatMessageTypeToString[ cmt ] );
}

QString Bee::dateTimeStringSuffix( const QDateTime& dt )
{
  QString s = dt.toString( "yyyyMMdd-hhmmss" );
  return s;
}

QString Bee::capitalizeFirstLetter( const QString& txt, bool all_chars_after_space, bool lower_all_characters )
{
  if( txt.isEmpty() )
    return txt;

  QString capitalized = "";
  QString tmp = lower_all_characters ? txt.toLower() : txt;
  if( all_chars_after_space )
  {
    QChar c;
    bool apply_title_case = true;
    for( int i = 0; i < tmp.size(); i++ )
    {
      c = tmp.at( i );
      if( c.isSpace() )
      {
        capitalized += c;
        apply_title_case = true;
      }
      else if( apply_title_case )
      {
        capitalized += c.toTitleCase();
        apply_title_case = false;
      }
      else
        capitalized += c;
    }
  }
  else
  {
    tmp.remove( 0, 1 );
    capitalized += txt.at( 0 ).toTitleCase();
    capitalized += tmp;
  }
  return capitalized;
}

QString Bee::lowerFirstLetter( const QString& txt )
{
  if( !txt.isEmpty() && txt.at( 0 ).isUpper() )
    return txt.at( 0 ).toLower() + txt.mid( 1 );
  else
    return txt;
}

QString Bee::removeHtmlTags( const QString& s )
{
  QTextDocument text_document;
  text_document.setHtml( s );
  return text_document.toPlainText();
}

QBrush Bee::defaultTextBrush()
{
  return qApp->palette().text();
}

QBrush Bee::defaultBackgroundBrush()
{
  return QBrush( Qt::transparent );
}

QBrush Bee::defaultHighlightedTextBrush()
{
  return qApp->palette().highlightedText();
}

QBrush Bee::defaultHighlightBrush()
{
  return qApp->palette().highlight();
}

QBrush Bee::userStatusBackgroundBrush( int user_status )
{
  if( user_status == User::Away || user_status == User::Busy )
    return QBrush( userStatusColor( user_status ) );
  else
    return defaultBackgroundBrush();
}

QPixmap Bee::convertToGrayScale( const QIcon& icon_to_convert, const QSize& pixmap_size )
{
  return convertToGrayScale( icon_to_convert.pixmap( pixmap_size ) );
}

QPixmap Bee::convertToGrayScale( const QPixmap& pix )
{
  QImage img = pix.toImage();
  if( img.isNull() )
    return QPixmap();

  int pixels = img.width() * img.height();
  if( pixels*static_cast<int>( sizeof( QRgb ) ) <= img.byteCount() )
  {
    QRgb *data = reinterpret_cast<QRgb*>( img.bits() );
    for (int i = 0; i < pixels; i++)
    {
      int val = qGray(data[i]);
      data[i] = qRgba(val, val, val, qAlpha(data[i]));
    }
  }

  QPixmap ret_pix;

#if QT_VERSION < 0x040700
  ret_pix = QPixmap::fromImage( img );
#else
  ret_pix.convertFromImage( img );
#endif
  return ret_pix;
}

QChar Bee::nativeFolderSeparator()
{
  return QDir::separator();
}

QString Bee::convertToNativeFolderSeparator( const QString& raw_path )
{
  QString path_converted( raw_path );
  QChar from_char;
#if defined( Q_OS_WIN ) || defined( Q_OS_OS2 ) || defined( Q_OS_OS2EMX ) || defined( Q_OS_SYMBIAN )
  from_char = QLatin1Char( '/' );
#else
  from_char = QLatin1Char( '\\' );
#endif
  QChar to_char = QDir::separator();

  for( int i = 0; i < path_converted.length(); i++ )
  {
    if( path_converted[ i ] == from_char )
      path_converted[ i ] = to_char;
  }
  // Do not remove "double slash" because some paths can start with them
  // Remove trailing slash
  if( path_converted.endsWith( QDir::separator() ) )
    path_converted.chop( 1 );
  return path_converted;
}

QString Bee::folderCdUp( const QString& folder_path )
{
  QStringList sl = Bee::convertToNativeFolderSeparator( folder_path ).split( nativeFolderSeparator() );
  if( sl.isEmpty() )
    return folder_path;

  sl.removeLast();
  return sl.join( nativeFolderSeparator() );
}

bool Bee::setLastModifiedToFile( const QString& to_path, const QDateTime& dt_last_modified )
{
  QFileInfo file_info_to( to_path );
  if( !file_info_to.exists() )
  {
    qWarning() << "Unable to set last modidified time to not existing file" << qPrintable( to_path );
    return false;
  }

  uint mod_time = dt_last_modified.toTime_t();
  uint ac_time = dt_last_modified.toTime_t();

  if( mod_time == (uint)-1 || ac_time == (uint)-1 )
  {
    qWarning() << "Unable to set invalid last modidified time to file" << qPrintable( to_path );
    return false;
  }

  bool ok = false;

#ifdef Q_OS_WIN

  uint cr_time = mod_time;

  FILETIME ft_modified, ft_creation, ft_access;

  LPCWSTR to_file_name = (const WCHAR*)to_path.utf16();

  HANDLE h_file = ::CreateFileW( to_file_name, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL );

  if( h_file != INVALID_HANDLE_VALUE )
  {
    LONGLONG ll = Int32x32To64( cr_time, 10000000) + 116444736000000000;
    ft_creation.dwLowDateTime = (DWORD) ll;
    ft_creation.dwHighDateTime = ll >> 32;
    LONGLONG ll2 = Int32x32To64( mod_time, 10000000) + 116444736000000000;
    ft_modified.dwLowDateTime = (DWORD) ll2;
    ft_modified.dwHighDateTime = ll2 >> 32;
    LONGLONG ll3 = Int32x32To64( ac_time, 10000000) + 116444736000000000;
    ft_access.dwLowDateTime = (DWORD) ll3;
    ft_access.dwHighDateTime = ll3 >> 32;

    if( !::SetFileTime( h_file, &ft_creation, &ft_access, &ft_modified ) )
    {
      QString s_error = QString( "0x%1" ).arg( (unsigned long)GetLastError() );
      qWarning() << "Function SetFileTime has error" << s_error << "for file" << qPrintable( to_path );
    }
    else
      ok = true;
  }
  else
    qWarning() << "Unable to get the HANDLE (CreateFile) of file" << qPrintable( to_path );

  CloseHandle( h_file );

#else

  struct utimbuf from_time_buffer;
  from_time_buffer.modtime = mod_time;
  from_time_buffer.actime = ac_time;

  const char *to_file_name = to_path.toUtf8().constData();
  ok = utime( to_file_name, &from_time_buffer ) == 0;
  if( !ok )
    qWarning() << "Function utime error" << errno << ":" << qPrintable( QString::fromLatin1( strerror( errno ) ) ) << "for file" << qPrintable( to_path );

#endif

  return ok;
}

bool Bee::showFileInGraphicalShell( const QString& file_path )
{
  QFileInfo file_info( file_path );
  if( !file_info.exists() )
    return false;

#ifdef Q_OS_WIN
  QString explorer_path = QLatin1String( "c:\\windows\\explorer.exe" );

  if( QFile::exists( explorer_path ) )
  {
    QStringList explorer_args;
    if( !file_info.isDir() )
      explorer_args += QLatin1String("/select,");
    explorer_args += Bee::convertToNativeFolderSeparator( file_info.canonicalFilePath() );
    if( QProcess::startDetached( explorer_path, explorer_args ) )
      return true;
    else
      qWarning() << "Unable to start process:" << qPrintable( explorer_path ) << qPrintable( explorer_args.join( " " ) );
  }
#endif

#ifdef Q_OS_MAC

  QStringList script_args;
  script_args << QLatin1String( "-e" )
              << QString::fromLatin1( "tell application \"Finder\" to reveal POSIX file \"%1\"" ).arg( file_info.canonicalFilePath() );
  QProcess::execute( QLatin1String( "/usr/bin/osascript" ), script_args );
  script_args.clear();
  script_args << QLatin1String( "-e" )
             << QLatin1String( "tell application \"Finder\" to activate" );
  QProcess::execute( QLatin1String( "/usr/bin/osascript" ), script_args );
  return true;

#endif

  return false;
}

bool Bee::folderIsWriteable( const QString& folder_path, bool create_folder_if_not_exists )
{
  QDir folder_to_test( folder_path );
  if( !folder_to_test.exists() )
  {
    if( create_folder_if_not_exists )
      return folder_to_test.mkpath( "." );
    else
      return false;
  }

  QFile test_file( QString( "%1/%2" ).arg( folder_path, QLatin1String( "beetestfile.txt" ) ) );
  if( test_file.open( QFile::WriteOnly ) )
  {
    bool ok = test_file.write( QByteArray( "BeeBEEP" ) ) > 0;
    test_file.close();
    return ok && test_file.remove();
  }
  else
    return false;
}

static int GetBoxSize( int pix_size )
{
  int box_size = pix_size > 10 ? pix_size / 10 : 1;
  if( box_size % 2 > 7 )
    box_size++;
  box_size = qMax( 1, box_size );
  return box_size;
}

QPixmap Bee::avatarForUser( const User& u, const QSize& avatar_size, bool use_available_user_image, int user_status )
{
  QPixmap user_avatar;
  bool default_avatar_used = false;
  if( user_status < 0 )
    user_status = u.status();
  if( u.vCard().photo().isNull() || !use_available_user_image )
  {
    default_avatar_used = true;
    Avatar av;
    av.setName( u.isValid() ? (Settings::instance().useUserFullName() && u.vCard().hasFullName() ? u.vCard().fullName( Settings::instance().useUserFirstNameFirstInFullName() ) : u.name()) : "??" );
    if( u.isStatusConnected() )
      av.setColor( u.color() );
    else
      av.setColor( QColor( Qt::gray ).name() );
    av.setSize( avatar_size );
    if( !av.create() )
    {
      user_avatar = QIcon( Bee::menuUserStatusIconFileName( user_status ) ).pixmap( avatar_size );
      return user_avatar;
    }
    else
      user_avatar = av.pixmap();
  }
  else
  {
    user_avatar = u.vCard().photo().scaled( avatar_size );
    if( !u.isStatusConnected() )
    {
      user_avatar = convertToGrayScale(user_avatar );
      return user_avatar;
    }
  }

  int pix_height = user_avatar.height();
  int pix_width = user_avatar.width();
  int box_height = GetBoxSize( pix_height );
  int box_width = GetBoxSize( pix_width );
  int box_start_height = qMax( 1, box_height / 2 );
  int box_start_width = qMax( 1, box_width / 2 );

  QPixmap pix( pix_width, pix_height );
  QPainter p( &pix );
  if( !default_avatar_used )
  {
    pix.fill( Bee::userStatusColor( user_status ) );
    p.drawPixmap( box_start_width, box_start_height, pix_width - box_width, pix_height - box_height, user_avatar.scaled( pix_width - box_width, pix_height - box_height ) );
  }
  else
  {
    p.drawPixmap( 0, 0, pix_width, pix_height, user_avatar );
    p.setPen( Bee::userStatusColor( user_status ) );
    for( int i = 0; i < box_height; i++ )
    {
      p.drawLine( 0, i, pix_width, i );
      p.drawLine( 0, pix_height-box_height+i, pix_width, pix_height-box_height+i );
    }

    for( int i = 0; i < box_width; i++ )
    {
      p.drawLine( i, 0, i, pix_height );
      p.drawLine( pix_width-box_width+i, 0, pix_width-box_width+i, pix_height );
    }
  }

  return pix;
}

QString Bee::toolTipForUser( const User& u, bool only_status )
{
  QString tool_tip = u.isLocal() ? QObject::tr( "You are %1" ).arg( Bee::userStatusToString( u.status() ) ) : QObject::tr( "%1 is %2" ).arg( Bee::userNameToShow( u, false ), Bee::userStatusToString( u.status() ) );

  if( only_status )
    return tool_tip;

  if( !u.vCard().birthday().isNull() )
  {
    QString text = userBirthdayToText( u );
    if( !text.isEmpty() )
      tool_tip +=  QString( "\n* %1 *" ).arg( text );
  }

  if( u.isStatusConnected() )
  {
    if( !u.statusDescription().isEmpty() )
      tool_tip += QString( "\n%1" ).arg( u.statusDescription() );

    if( !u.workgroups().isEmpty() )
      tool_tip += QString( "\n%1: %2" ).arg( QObject::tr( "Workgroups" ) ).arg( Bee::stringListToTextString( u.workgroups(), true, 9 ) );

    if( !u.vCard().info().isEmpty() )
    {
      tool_tip += QString( "\n~~~\n" );
      tool_tip += u.vCard().info();
      tool_tip += QString( "\n~~~" );
    }

    if( u.statusChangedIn().isValid() )
      tool_tip += QString( "\n%1: %2" ).arg( QObject::tr( "Last update" ) ).arg( Bee::dateTimeToString( u.statusChangedIn() ) );
  }
  else
  {
    if( u.lastConnection().isValid() )
      tool_tip += QString( "\n%1: %2" ).arg( QObject::tr( "Last connection" ) ).arg( Bee::dateTimeToString( u.lastConnection() ) );

    int unsent_messages = MessageManager::instance().countMessagesToSendToUserId( u.id() );
    if( unsent_messages > 0 )
      tool_tip += QString( "\n%1" ).arg( QObject::tr( "%n unsent message(s)", "", unsent_messages ) );
  }
  tool_tip += QString( "\n" );
  return tool_tip;
}

QString Bee::userBirthdayToText( const User& u )
{
  QString birthday_text;
  if( !u.isLocal() )
  {
    // Do not use Bee::userNameToShow( u ) to avoid computer name
    QString user_name = Settings::instance().useUserFullName() && u.vCard().hasFullName() ? u.vCard().fullName( Settings::instance().useUserFirstNameFirstInFullName() ) : u.name();
    user_name = Bee::removeHtmlTags( user_name );
    user_name = Bee::replaceHtmlSpecialCharacters( user_name );
    int days_to = u.daysToBirthDay();
    if( days_to == 0 )
      birthday_text = QObject::tr( "Today is %1's birthday" ).arg( user_name );
    else if( days_to == 1 )
      birthday_text =  QObject::tr( "Tomorrow is %1's birthday" ).arg( user_name );
    else if( days_to > 1 && days_to < 4 )
      birthday_text= QObject::tr( "%1's birthday is in %2 days" ).arg( user_name ).arg( days_to );
    else if( days_to == -1 )
      birthday_text = QObject::tr( "Yesterday was %1's birthday" ).arg( user_name );
    else
      birthday_text = "";
  }
  else
  {
    if( u.isBirthDay() )
      birthday_text = QObject::tr( "Happy Birthday to you!" );
    else
      birthday_text = "";
  }
  return birthday_text;
}

QString Bee::stringListToTextString( const QStringList& sl, bool strip_html_tags, int max_items )
{
  QStringList sl_parsed;
  foreach( QString s, sl )
  {
    if( !s.isEmpty() )
    {
      if( strip_html_tags )
        sl_parsed.append( Bee::removeHtmlTags( s ) );
      else
        sl_parsed.append( s );
    }
  }

  if( sl_parsed.isEmpty() )
    return "";
  if( sl_parsed.size() == 1 )
    return sl_parsed.first();
  if( sl_parsed.size() == 2 )
    return sl_parsed.join( QString( " %1 " ).arg( QObject::tr( "and" ) ) );

  QStringList sl_to_join;
  if( max_items < 1 || max_items > (sl_parsed.size()-1))
    max_items = sl_parsed.size()-1;
  int num_items = 0;

  foreach( QString s, sl_parsed )
  {
    if( num_items >= max_items )
      break;
    num_items++;
    if( strip_html_tags )
      sl_to_join.append( Bee::removeHtmlTags( s ) );
    else
      sl_to_join.append( s );
  }

  QString s_joined = sl_to_join.join( ", " );
  int diff_items = sl_parsed.size() - sl_to_join.size();
  if( diff_items == 1 )
  {
    if( !sl_parsed.last().isEmpty() )
    {
      s_joined.append( QString( " %1 " ).arg( QObject::tr( "and" ) ) );
      s_joined.append( sl_parsed.last() );
    }
  }
  else
    s_joined.append( QString( " %1" ).arg( QObject::tr( "and %1 others" ).arg( diff_items ) ) );

  return s_joined;
}

void Bee::removeContextHelpButton( QWidget* w )
{
  Qt::WindowFlags w_flags = w->windowFlags();
  w_flags &= ~Qt::WindowContextHelpButtonHint;
  w->setWindowFlags( w_flags );
}

void Bee::showUp( QWidget* w, bool force_show )
{
  bool is_showed = false;
  if( !w->isVisible() )
  {
    w->show();
    is_showed = true;
  }

  if( w->isMinimized() )
  {
    w->showNormal();
    is_showed = true;
  }

  if( force_show && !is_showed )
    w->show();

  w->raise();
}

void Bee::raiseOnTop( QWidget* w )
{
#if defined( Q_OS_WIN )
  Bee::showUp( w );
  if( !(w->windowFlags() & Qt::WindowStaysOnTopHint) )
  {
    ::SetWindowPos( (HWND)w->winId(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE );
    ::SetWindowPos( (HWND)w->winId(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE );
  }
#elif defined( Q_OS_LINUX ) && QT_VERSION >= 0x050000
  Bee::showUp( w );
  bool on_top_flag_added = false;
  if( !(w->windowFlags() & Qt::WindowStaysOnTopHint) )
  {
    Bee::setWindowStaysOnTop( w, true );
    on_top_flag_added = true;
    w->raise();
  }
  if( on_top_flag_added )
    Bee::setWindowStaysOnTop( w, false );
#else
  bool on_top_flag_added = false;
  if( !(w->windowFlags() & Qt::WindowStaysOnTopHint) )
  {
    Bee::setWindowStaysOnTop( w, true );
    on_top_flag_added = true;
  }
  Bee::showUp( w );
  if( on_top_flag_added )
    Bee::setWindowStaysOnTop( w, false );
#endif
}

void Bee::setWindowStaysOnTop( QWidget* w, bool enable )
{
  bool w_was_visible = w->isVisible();
  if( w_was_visible )
    w->hide();

  Qt::WindowFlags w_flags = w->windowFlags();
  if( enable )
    w_flags |= Qt::WindowStaysOnTopHint;
  else
    w_flags &= ~Qt::WindowStaysOnTopHint;
  w->setWindowFlags( w_flags );

  if( w_was_visible )
    QMetaObject::invokeMethod( w, "show", Qt::QueuedConnection );
}

bool Bee::areStringListEqual( const QStringList& sl1, const QStringList& sl2, Qt::CaseSensitivity cs )
{
  if( sl1.size() != sl2.size() )
    return false;

  foreach( QString s, sl1 )
  {
    if( !sl2.contains( s, cs ) )
      return false;
  }
  return true;
}

QString Bee::dateTimeToString( const QDateTime& dt )
{
  return dt.date() == QDate::currentDate() ? dt.time().toString( Qt::SystemLocaleShortDate ) : dt.toString( Qt::SystemLocaleShortDate );
}

void Bee::setBackgroundColor( QWidget* w, const QColor& c )
{
  QPalette pal = w->palette();
  pal.setBrush( QPalette::Base, QBrush( c ) );
  w->setPalette( pal );
}

QColor Bee::selectColor( QWidget* w, const QColor& default_color )
{
  return QColorDialog::getColor( default_color, w );
}

QString Bee::pluginFileExtension()
{
#if defined Q_OS_MAC
  return QLatin1String( "dylib" );
#elif defined Q_OS_UNIX
  return QLatin1String( "so" );
#else
  return QLatin1String( "dll" );
#endif
}

QString Bee::removeInvalidCharactersForFilePath( const QString& s )
{
  QString valid_path = s.simplified();
  valid_path.replace( QRegExp( "[" + QRegExp::escape( "\\/:*?\"<>|$[]+,;=" ) + "]" ), QString( "_" ) );
  return valid_path;
}

QString Bee::replaceHtmlSpecialCharacters( const QString& s )
{
  // It is different from Protocol::formatHtmlText(...)
  QString text = s.trimmed();
  QString html_text = "";
  bool there_is_a_space_before_it = false;
  QChar c;

  for( int i = 0; i < text.length(); i++ )
  {
    c = text.at( i );
    if( c == QLatin1Char( ' ' ) )
    {
      if( there_is_a_space_before_it )
        html_text += QLatin1String( "&nbsp;" );
      else
        html_text += QLatin1Char( ' ' );
      there_is_a_space_before_it = true;
    }
    else
    {
      there_is_a_space_before_it = false;
      if( c == QLatin1Char( '\n' ) )
        html_text += QLatin1String( "<br>" );
      else if( c == QLatin1Char( '<' ) )
        html_text += QLatin1String( "&lt;" );
      else if( c == QLatin1Char( '>' ) )
        html_text += QLatin1String( "&gt;" );
      else if( c == QLatin1Char( '\t' ) )
        html_text += QLatin1String( "&nbsp;&nbsp;" );
      else if( c == QLatin1Char( '\r' ) )
      { /* skip */ }
      else
        html_text += c;
    }
  }
  return html_text;
}

qint64 Bee::roundFromDouble( double d )
{
  qint64 i = static_cast<qint64>( d );
  return ((d - i) >= 0.5) ? ++i : i;
}

qint64 Bee::bytesPerSecond( FileSizeType transferred_byte_size, int time_elapsed_ms )
{
  double dtbs = static_cast<double>( transferred_byte_size );
  double dte = static_cast<double>( time_elapsed_ms );
  if( dtbs > 0 && dte > 0 )
    return qMax( static_cast<qint64>(1), roundFromDouble( (dtbs*1000.0)/dte ) );
  else
    return 1;
}

QString Bee::bytesToString( FileSizeType bytes, int precision )
{
  QString suffix;
  double result = 0;
  int prec = 1;
  if( bytes > 1000000000000 )
  {
    suffix = "Tb";
    result = bytes / 1000000000000.0;
    prec = 5;
  }
  else if( bytes > 1000000000 )
  {
    suffix = "Gb";
    result = bytes / 1000000000.0;
    prec = 3;
  }
  else if( bytes > 1000000 )
  {
    suffix = "Mb";
    result = bytes / 1000000.0;
    prec = 2;
  }
  else if( bytes > 1000 )
  {
    suffix = "kb";
    result = bytes / 1000.0;
    prec = result >= 10 ? 0 : 1;
  }
  else
  {
    suffix = "b";
    result = bytes;
    prec = 0;
  }
  return QString( "%1 %2").arg( result, 0, 'f', prec > 0 ? (precision >= 0 ? precision : prec) : 0 ).arg( suffix );
}

QString Bee::timeToString( qint64 ms )
{
  if( ms < 0 )
    return QT_TRANSLATE_NOOP( "Not available", "n.a." );
  if( ms == 0 )
    return QString( "0 s" );

  qint64 d = ms / 86400000;
  if( d > 0 )
    ms -= d*86400000;

  QTime t = QTime( 0, 0, 0, 0 );
  t = t.addMSecs( static_cast<int>( ms ) );
  qint64 h = d * 24 + t.hour();

  if( h < 24 )
  {
    if( h > 0 )
      return t.toString( "%1:mm:ss" ).arg( QString::number( h ).rightJustified( 2, QLatin1Char( '0' ) ) );
    else if( t.minute() > 0 )
      return t.toString( "m:ss" ) + QString( " m" );
    else if( t.second() > 0 )
      return QString( "%1 s" ).arg( (t.msec() > 500 && t.second() <= 59 ? t.second() + 1 : t.second()) );
    else
      return QString( "%1 ms" ).arg( t.msec() );
  }
  else
  {
    QStringList sl;
    if( d > 0 )
      sl.append( QString( "%1 d" ).arg( d ) );
    if( t.hour() > 0 )
      sl.append( QString( "%1 h" ).arg( t.hour() ) );
    if( t.minute() > 0 )
      sl.append( QString( "%1 m" ).arg( t.minute() ) );
    if( t.second() > 0 )
      sl.append( QString( "%1 s" ).arg( t.second() ) );
    if( sl.empty() )
      sl.append( QString( "%1 ms" ).arg( t.msec() ) );
    return sl.join( ", " );
  }
}

QString Bee::transferTimeLeft( FileSizeType bytes_transferred, FileSizeType total_bytes, FileSizeType starting_position, int elapsed_time )
{
  qint64 bytes_per_second;
  if( starting_position > 0 && bytes_transferred > starting_position )
    bytes_per_second = bytesPerSecond( bytes_transferred - starting_position, elapsed_time );
  else
    bytes_per_second = bytesPerSecond( bytes_transferred, elapsed_time );
  FileSizeType bytes_left = bytes_transferred >= total_bytes ? 0 : total_bytes - bytes_transferred;
  int ms_left = static_cast<int>( (bytes_left * 1000) / bytes_per_second );
  if( ms_left < 1000 )
    return timeToString( 1000 );
  else if( ms_left < 2000 )
    return timeToString( 2000 );
  else
    return timeToString( ms_left );
}

QString Bee::imagePreviewPath( const QString& source_image_path )
{
  QFileInfo fi( source_image_path );
  if( fi.exists() && fi.isReadable() )
  {
    QString file_png_path = QString( "%1-%2.png" ).arg( QLatin1String( "img" ), Protocol::instance().fileInfoHash( fi ) );
    QString image_preview_path = Bee::convertToNativeFolderSeparator( QString( "%1/%2" ).arg( Settings::instance().cacheFolder(), file_png_path ) );
    if( QFile::exists( image_preview_path ) )
    {
#ifdef BEEBEEP_DEBUG
      qDebug() << "Image preview of" << source_image_path << "cached in file" << qPrintable( image_preview_path );
#endif
      return image_preview_path;
    }
    QImage img;
    QImageReader img_reader( source_image_path );
    img_reader.setAutoDetectImageFormat( true );
    if( img_reader.read( &img ) )
    {
      if( img.height() > Settings::instance().imagePreviewHeight() )
      {
#ifdef BEEBEEP_DEBUG
        qDebug() << "Image preview scaled and saving to" << qPrintable( image_preview_path );
#endif
        // PNG for transparency (always)
        QImage img_scaled = img.scaledToHeight( Settings::instance().imagePreviewHeight(), Qt::SmoothTransformation );
        if( img_scaled.save( image_preview_path, "png" ) )
          return image_preview_path;
#ifdef BEEBEEP_DEBUG
        else
          qDebug() << "Unable to save scaled image preview to" << qPrintable( image_preview_path );
#endif
      }
      else
        return source_image_path;
    }
  }
  return QString( "" );
}

QVariant Bee::comboBoxData( const QComboBox* box )
{
  int box_index = box->currentIndex();
  return box_index == -1 ? QVariant() : box->itemData( box_index );
}

bool Bee::selectComboBoxData( QComboBox* box, const QVariant& item_data )
{
  int box_index = box->findData( item_data );
  if( box_index >= 0 )
  {
    box->setCurrentIndex( box_index );
    return true;
  }
  else
    return false;
}


QColor Bee::colorDarkGrey() { return QColor( 64, 64, 64 ); }
QColor Bee::colorGrey() { return QColor( 128, 128, 128 ); }
QColor Bee::colorBlack() { return QColor( 25, 25, 25 ); }
QColor Bee::colorBlue() { return QColor( 42, 130, 218 ); }
QColor Bee::colorWhite() { return QColor( 238, 238, 238 ); }
QColor Bee::colorYellow() { return QColor( 222, 222, 0 ); }
QColor Bee::colorOrange() { return QColor( 255, 207, 4 ); }

QColor Bee::invertColor( const QColor& c )
{
  int r, g, b;
  c.getRgb( &r, &g, &b );
  int i_r = 255 - r;
  int i_g = 255 - g;
  int i_b = 255 - b;

  int s_r = qAbs( r - i_r );
  int s_g = qAbs( g - i_g );
  int s_b = qAbs( b - i_b );

  if( s_r < 30 && s_g < 30 && s_b < 30 ) // gray on gray
    return QColor( 0, 0, 0 );
  else if( s_r > 230 && s_g > 230 && s_b > 230 ) // white on white
    return QColor( 255, 255, 255 );
  else
    return QColor( i_r, i_g, i_b );
}

bool Bee::isColorNear( const QColor& c1, const QColor& c2 )
{
  int r_diff = c1.red() - c2.red();
  int g_diff = c1.green() - c2.green();
  int b_diff = c1.blue() - c2.blue();

  return qAbs( r_diff ) < 30 && qAbs( g_diff ) < 30 && qAbs( b_diff ) < 30;
}

bool Bee::isColorVisibleInChat( const QColor& c )
{
  return !Bee::isColorNear( c, QColor( Settings::instance().chatBackgroundColor() ) );
}

QString Bee::beeColorsToHtmlText( const QString& txt )
{
  QString bee_txt = "";
  QString c_y = colorYellow().name();
  for( int i = 0; i < txt.size(); i++ )
    bee_txt.append( QString( "<font color=%1>%2</font>" ).arg( (i % 2 == 0) ? "#000000" : c_y ).arg( txt.at( i ) ) );
  return bee_txt;
}

QPalette Bee::darkPalette()
{
  QPalette darkPalette;
  darkPalette.setColor( QPalette::Window, colorGrey() );
  darkPalette.setColor( QPalette::WindowText, colorWhite() );
  darkPalette.setColor( QPalette::Base, colorDarkGrey() );
  darkPalette.setColor( QPalette::AlternateBase, colorDarkGrey().lighter() );
  darkPalette.setColor( QPalette::ToolTipBase, colorGrey().lighter() );
  darkPalette.setColor( QPalette::ToolTipText, colorBlack() );
  darkPalette.setColor( QPalette::Text, colorWhite() );
  darkPalette.setColor( QPalette::BrightText, Qt::yellow );
  darkPalette.setColor( QPalette::Button, colorDarkGrey() );
  darkPalette.setColor( QPalette::ButtonText, colorWhite() );
  darkPalette.setColor( QPalette::Link, colorBlue() );
  darkPalette.setColor( QPalette::LinkVisited, colorBlue().darker() );
  darkPalette.setColor( QPalette::Highlight, colorGrey() );
  darkPalette.setColor( QPalette::HighlightedText, colorBlack() );

  darkPalette.setColor( QPalette::Active, QPalette::Button, colorGrey() );
  darkPalette.setColor( QPalette::Disabled, QPalette::ButtonText, colorGrey().darker() );
  darkPalette.setColor( QPalette::Disabled, QPalette::WindowText, colorGrey().darker() );
  darkPalette.setColor( QPalette::Disabled, QPalette::Text, colorGrey().darker() );
  darkPalette.setColor( QPalette::Disabled, QPalette::Base, colorDarkGrey().lighter() );
  darkPalette.setColor( QPalette::Disabled, QPalette::AlternateBase, colorDarkGrey().lighter().lighter() );
  darkPalette.setColor( QPalette::Disabled, QPalette::Light, colorGrey().lighter() );

  return darkPalette;
}
