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
#include "ChatMessage.h"
#include "PluginManager.h"
#include "User.h"
#if QT_VERSION < 0x050000
  #ifdef Q_OS_WIN
    #include <Windows.h>
  #endif
#endif


QString Bee::userStatusIconFileName( int user_status )
{
  switch( user_status )
  {
  case User::Offline:
    return QString( ":/images/user-offline.png" );
  case User::Away:
    return QString( ":/images/user-away.png" );
  case User::Busy:
    return QString( ":/images/user-busy.png" );
  default:
    return QString( ":/images/user-online.png" );
  }
}

QString Bee::menuUserStatusIconFileName( int user_status )
{
  switch( user_status )
  {
  case User::Offline:
    return QString( ":/images/menu-user-offline.png" );
  case User::Away:
    return QString( ":/images/menu-user-away.png" );
  case User::Busy:
    return QString( ":/images/menu-user-busy.png" );
  default:
    return QString( ":/images/menu-user-online.png" );
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

QString Bee::bytesToString( FileSizeType bytes, int precision )
{
  QString suffix;
  double result = 0;
  int prec = 1;
  if( bytes > 1000000000 )
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

QString Bee::elapsedTimeToString( int time_elapsed )
{
  QTime t( 0, 0 );
  t = t.addMSecs( time_elapsed );
  QString s = "";
  if( t.hour() == 0 && t.minute() == 0 && t.second() == 0 )
    s = QString( "%1 ms" ).arg( t.msec() );
  else if( t.hour() == 0 && t.minute() == 0 )
    s = QString( "%1 s" ).arg( t.second() );
  else if( t.hour() == 0 )
    s = QString( "%1 m, %2 s" ).arg( t.minute() ).arg( t.second() );
  else
    s = QString( "%1 h, %2 m, %3 s" ).arg( t.hour() ).arg( t.minute() ).arg( t.second() );
  return s;
}

QString Bee::uniqueFilePath( const QString& file_path )
{
  int counter = 1;
  QFileInfo fi( file_path );
  QString dir_path = fi.absoluteDir().absolutePath();
  QString file_base_name = fi.completeBaseName();
  QString file_suffix = fi.suffix();
  QString new_file_name;

  while( fi.exists() )
  {
    new_file_name = QString( "%1 (%2)%3%4" ).arg( file_base_name ).arg( counter ).arg( (file_suffix.isEmpty() ? "" : ".") ).arg( file_suffix );
    fi.setFile( dir_path, new_file_name );
    counter++;

    if( counter > 98 )
    {
      qWarning() << "Unable to find a unique file name from path" << file_path << "(so overwrite the last one)";
      break;
    }
  }

  return fi.absoluteFilePath();
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
  QT_TRANSLATE_NOOP( "File", "MacOSX" )
};

QString Bee::fileTypeToString( Bee::FileType ft )
{
  if( ft < 0 || ft > Bee::NumFileType )
    ft = Bee::FileOther;
  return qApp->translate( "File", FileTypeToString[ ft ] );
}

static const char* ChatMessageTypeToString[] =
{
  QT_TRANSLATE_NOOP( "ChatMessage", "Header" ),
  QT_TRANSLATE_NOOP( "ChatMessage", "System" ),
  QT_TRANSLATE_NOOP( "ChatMessage", "Chat" ),
  QT_TRANSLATE_NOOP( "ChatMessage", "Connection" ),
  QT_TRANSLATE_NOOP( "ChatMessage", "User Status" ),
  QT_TRANSLATE_NOOP( "ChatMessage", "User Information" ),
  QT_TRANSLATE_NOOP( "ChatMessage", "File Transfer" ),
  QT_TRANSLATE_NOOP( "ChatMessage", "History" ),
  QT_TRANSLATE_NOOP( "ChatMessage", "Other" )
};

QString Bee::chatMessageTypeToString( int cmt )
{
  if( cmt < 0 || cmt > ChatMessage::NumTypes )
    cmt = ChatMessage::Other;
  return qApp->translate( "ChatMessage", ChatMessageTypeToString[ cmt ] );
}

QString Bee::dateTimeStringSuffix( const QDateTime& dt )
{
  QString s = dt.toString( "yyyy mm dd-hhmmss" );
  s.remove( QChar( ' ' ) );
  return s;
}

QString Bee::capitalizeFirstLetter( const QString& txt, bool all_chars_after_space )
{
  if( txt.isEmpty() )
    return txt;
  QString tmp = txt.toLower();
  QString capitalized = "";
  bool apply_title_case = true;
  QChar c;
  for( int i = 0; i < tmp.size(); i++ )
  {
    c = tmp.at( i );

    if( c.isSpace() )
    {
      capitalized += c;
      if( all_chars_after_space )
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

  return capitalized;
}

QColor Bee::invertColor( const QColor& c )
{
  int r, g, b;
  c.getRgb( &r, &g, &b );
  int i_r = 255 - r;
  int i_g = 255 - g;
  int i_b = 255 - b;

  int s_r = r - i_r;
  int s_g = r - i_g;
  int s_b = r - i_b;

  if( qAbs( s_r ) < 30 && qAbs( s_g ) < 30 && qAbs( s_b ) < 30 ) // gray on gray
    return QColor( 0, 0, 0 );
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
  return qApp->palette().base();
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

QPixmap Bee::convertToGrayScale( const QPixmap& pix )
{
  QImage img = pix.toImage();
  if( img.isNull() )
    return QPixmap();

  int pixels = img.width() * img.height();
  if( pixels*(int)sizeof(QRgb) <= img.byteCount() )
  {
    QRgb *data = (QRgb *)img.bits();
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

QChar Bee::naviveFolderSeparator()
{
#ifdef Q_OS_WIN
  return QChar( '\\' );
#else
  return QChar( '/' );
#endif
}

QString Bee::convertToNativeFolderSeparator( const QString& folder_path )
{
  QString folder_path_converted = folder_path;
#ifdef Q_OS_WIN
  folder_path_converted.replace( QChar( '/' ), naviveFolderSeparator() );
#else
  folder_path_converted.replace( QChar( '\\' ), naviveFolderSeparator() );
#endif
  return folder_path_converted;
}

QString Bee::folderCdUp( const QString& folder_path )
{
  QStringList sl = Bee::convertToNativeFolderSeparator( folder_path ).split( naviveFolderSeparator() );
  if( sl.isEmpty() )
    return folder_path;

  sl.removeLast();
  return sl.join( naviveFolderSeparator() );
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
  LPCWSTR to_file_name = to_path.toStdWString().c_str();

  HANDLE h_file = ::CreateFile( to_file_name, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL );

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

  const char *to_file_name = to_path.toLatin1().constData();
  ok = utime( to_file_name, &from_time_buffer ) == 0;
  if( !ok )
    qWarning() << "Function utime error" << errno << ":" << qPrintable( strerror( errno ) ) << "for file" << qPrintable( to_path );

#endif

  return ok;
}

bool Bee::showFileInGraphicalShell( const QString& file_path )
{
  QFileInfo file_info( file_path );

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

bool Bee::folderIsWriteable( const QString& folder_path )
{
  QDir folder_to_test( folder_path );
  if( !folder_to_test.exists() )
    return folder_to_test.mkpath( "." );

  QFile test_file( QString( "%1/%2" ).arg( folder_path ).arg( "beetestfile.txt" ) );
  if( test_file.open( QFile::WriteOnly ) )
  {
    test_file.close();
    test_file.remove();
    return true;
  }
  else
    return false;
}
