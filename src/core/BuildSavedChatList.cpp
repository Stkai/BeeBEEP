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

#include "BuildSavedChatList.h"
#include "MessageManager.h"
#include "Protocol.h"
#include "Settings.h"


BuildSavedChatList::BuildSavedChatList( QObject *parent )
  : QObject( parent ), m_elapsedTime( 0 ), m_protocolVersion( 1 )
{
  setObjectName( "BuildSavedChatList" );
}

QString BuildSavedChatList::checkAuthCodeFromFileHeader( const QStringList& file_header, const QString& file_name )
{
  QString auth_code;
  int proto_version = 0;

  if( file_header.size() >= 3 )
  {
    bool ok = false;
    proto_version = file_header.at( 2 ).toInt( &ok );
    if( !ok )
    {
      qWarning() << "Invalid protocol version found in the header of file" << qPrintable( file_name );
      return auth_code;
    }
  }
  else
  {
    qWarning() << file_header.size() << "is invalid header size in file" << qPrintable( file_name );
    return auth_code;
  }

  if( proto_version >= SAVE_MESSAGE_AUTH_CODE_PROTO_VERSION )
  {
    if( file_header.size() >= 4 )
    {
      auth_code = file_header.at( 3 );
      if( auth_code.isEmpty() )
        qWarning() << "Empty AUTH code found in the header of file" << qPrintable( file_name );
    }
    else
      qWarning() << file_header.size() << "is invalid header size in file" << qPrintable( file_name );
  }
  else
  {
    qDebug() << "Old protocol found in file header of file" << qPrintable( file_name );
    auth_code = MessageManager::instance().savedMessagesAuthCode();
  }
  if( proto_version > m_protocolVersion )
    m_protocolVersion = proto_version;
  return auth_code;
}


void BuildSavedChatList::buildList()
{
  m_savedChatsAuthCode = QString();
  QElapsedTimer elapsed_time;
  elapsed_time.start();

  clearCacheItems();
  removePartiallyDownloadedFiles();

  QString file_name = Settings::instance().savedChatsFilePath();
  bool use_backup = false;
  QFileInfo backup_chats_file_info( Settings::instance().autoSavedChatsFilePath() );
  if( backup_chats_file_info.exists() )
  {
    QFileInfo saved_chats_file_info( Settings::instance().savedChatsFilePath() );
    if( saved_chats_file_info.exists() )
    {
      if( backup_chats_file_info.lastModified() > saved_chats_file_info.lastModified() )
      {
        qWarning() << "File" << qPrintable( Settings::instance().savedChatsFilePath() ) << "is older than backup";
        use_backup = true;
      }
    }
    else
    {
      qWarning() << "File" << qPrintable( Settings::instance().savedChatsFilePath() ) << "not found";
      use_backup = true;
    }
  }

  if( use_backup )
  {
    file_name = Settings::instance().autoSavedChatsFilePath();
    qDebug() << "Loading saved chats from backup file" << qPrintable( file_name );
  }

  QFile file( file_name );
  if( !file.open( QIODevice::ReadOnly ) )
  {
    if( Settings::instance().chatAutoSave() )
      qWarning() << "Unable to open file" << qPrintable( file_name ) << ": loading saved chats aborted";
    return;
  }

  QDataStream stream( &file );
  stream.setVersion( Settings::instance().dataStreamVersion( true ) );

  QStringList file_header;

  stream >> file_header;
  if( stream.status() == QDataStream::Ok )
  {
    m_savedChatsAuthCode = checkAuthCodeFromFileHeader( file_header, file_name );
    loadSavedChats( &stream );
  }
  else
    qWarning() << "Error reading header datastream, abort loading saved chats";
  file.close();

  loadUnsentMessages();

  m_elapsedTime = elapsed_time.elapsed();

  emit listCompleted();
}

void BuildSavedChatList::loadSavedChats( QDataStream* stream )
{
  qint32 num_of_chats = 0;
  (*stream) >> num_of_chats;

  if( stream->status() != QDataStream::Ok )
  {
    qWarning() << "Error reading number of chats in datastream, abort loading saved chats";
    return;
  }

  if( num_of_chats <= 0 )
    return;

  QString chat_name_encrypted;
  QString chat_text_encrypted;
  QString chat_name;
  QString chat_text;

  for( int i = 1; i <= num_of_chats; i++ )
  {
    (*stream) >> chat_name_encrypted;

    if( stream->status() != QDataStream::Ok )
    {
      qWarning() << "Error reading datastream, abort loading chat:" << i;
      return;
    }
    chat_name = Settings::instance().simpleDecrypt( chat_name_encrypted );

    (*stream) >> chat_text_encrypted;
    if( stream->status() != QDataStream::Ok )
    {
      qWarning() << "Error reading datastream, abort loading chat:" << qPrintable( chat_name );
      return;
    }
    chat_text = Settings::instance().simpleDecrypt( chat_text_encrypted );

    qDebug() << "Loading chat" << i << "completed:" << qPrintable( chat_name );

    if( chat_text.simplified().isEmpty() )
      qDebug() << "This saved chat is empty:" << qPrintable( chat_name );
    else
      m_savedChats.insert( chat_name, chat_text );
  }
  qDebug() << m_savedChats.size() << "saved chats found";
}

void BuildSavedChatList::loadUnsentMessages()
{
  m_unsentMessagesAuthCode = QString();
  QString file_name = Settings::instance().unsentMessagesFilePath();
  QFile file( file_name );
  if( !file.open( QIODevice::ReadOnly ) )
  {
    qWarning() << "Unable to open file" << qPrintable( file_name ) << ": loading saved unsent messages aborted";
    return;
  }

  QDataStream stream( &file );
  stream.setVersion( Settings::instance().dataStreamVersion( true ) );

  QStringList file_header;
  stream >> file_header;
  if( stream.status() != QDataStream::Ok )
  {
    qWarning() << "Error reading header datastream, abort loading unsent messages";
    file.close();
    return;
  }

  m_unsentMessagesAuthCode = checkAuthCodeFromFileHeader( file_header, file_name );

  qint32 num_of_unsent_messages = 0;
  stream >> num_of_unsent_messages;
  if( stream.status() != QDataStream::Ok )
  {
    qWarning() << "Error reading number of unsent messages datastream, abort loading unsent messages";
    file.close();
    return;
  }

  if( num_of_unsent_messages > 0 )
  {
    for( int i = 1; i <= num_of_unsent_messages; i++ )
    {
      QString smr;
      stream >> smr;
      if( stream.status() != QDataStream::Ok )
      {
        qWarning() << "Error reading datastream, abort loading unsent message:" << i;
        break;
      }

      QString decoded_smr = Settings::instance().simpleDecrypt( smr );
      MessageRecord mr = Protocol::instance().loadMessageRecord( decoded_smr );
      if( !mr.isValid() )
      {
        qWarning() << "Error reading saved unsent message:" << i;
        continue;
      }

      m_unsentMessages.append( mr );
    }
    qDebug() << "Loading" << m_unsentMessages.size() << "unsent messages from" << qPrintable( file_name ) << "completed";
  }
  else
    qDebug() << "0 saved unsent messaged found in" << qPrintable( file_name );

  file.close();
}

void BuildSavedChatList::clearCacheItems()
{
  int clear_days = Settings::instance().clearCacheAfterDays();
  if( clear_days < 0 )
  {
    qDebug() << "Automatic cache cleaning is disabled";
    return;
  }

  QString cache_folder = Settings::instance().cacheFolder();
  QDir cache_dir( cache_folder );
  if( !cache_dir.exists() )
    return;
  if( !cache_dir.isReadable() )
  {
    qWarning() << "Cache folder path" << qPrintable( cache_folder ) << "is not readable";
    return;
  }
  qDebug() << "Cleaning cache items from path" << qPrintable( cache_folder );
  cache_dir.setFilter( QDir::Files | QDir::NoDotAndDotDot );
  cache_dir.setSorting( QDir::Time | QDir::Reversed );
  QDateTime today = QDateTime::currentDateTime();
  int item_counter = 0;
  QList<QFileInfo> fi_list = cache_dir.entryInfoList();
  foreach( QFileInfo fi, fi_list )
  {
    if( fi.suffix() == Settings::instance().partiallyDownloadedFileExtension() )
      continue;
    int fi_days = static_cast<int>( qAbs( fi.lastModified().daysTo( today ) ) );
 #ifdef BEEBEEP_DEBUG
    qDebug() << "Cache file" << qPrintable( fi.fileName() ) << "is" << fi_days << "days old";
 #endif
    if( clear_days == 0 || fi_days > clear_days )
    {
      if( cache_dir.remove( fi.fileName() ) )
      {
        item_counter++;
        qDebug() << "Removed file from cache:" << qPrintable( fi.fileName() ) << "(last modified" << qPrintable( fi.lastModified().toString( Qt::ISODate ) ) << ")";
      }
      else
        qWarning() << "Unable to remove file from cache:" << qPrintable( fi.fileName() ) << "(last modified" << qPrintable( fi.lastModified().toString( Qt::ISODate ) ) << ")";
    }
  }
  qDebug() << "Cleaned" << item_counter << "items from cache";
}

void BuildSavedChatList::removePartiallyDownloadedFiles()
{
  int clear_days = Settings::instance().removePartiallyDownloadedFilesAfterDays();
  if( clear_days < 0 )
  {
    qDebug() << "Automatic remove of partially downloaded files is disabled";
    return;
  }

  QString cache_folder = Settings::instance().cacheFolder();
  QDir cache_dir( cache_folder );
  if( !cache_dir.exists() )
    return;
  if( !cache_dir.isReadable() )
  {
    qWarning() << "Cache folder path" << qPrintable( cache_folder ) << "is not readable";
    return;
  }
  qDebug() << "Cleaning partially downloaded files from path" << qPrintable( cache_folder );
  cache_dir.setFilter( QDir::Files | QDir::NoDotAndDotDot );
  cache_dir.setSorting( QDir::Time | QDir::Reversed );
  QDateTime today = QDateTime::currentDateTime();
  int item_counter = 0;
  QList<QFileInfo> fi_list = cache_dir.entryInfoList();
  foreach( QFileInfo fi, fi_list )
  {
    if( fi.suffix() != Settings::instance().partiallyDownloadedFileExtension() )
      continue;
    if( !fi.isFile() )
      continue;
    int fi_days = static_cast<int>( qAbs( fi.lastModified().daysTo( today ) ) );
 #ifdef BEEBEEP_DEBUG
    qDebug() << "Partially downloaded file" << qPrintable( fi.fileName() ) << "is" << fi_days << "days old";
 #endif
    if( clear_days == 0 || fi_days > clear_days )
    {
      if( cache_dir.remove( fi.fileName() ) )
      {
        item_counter++;
        qDebug() << "Removed partially downloaded from cache:" << qPrintable( fi.fileName() ) << "(last modified" << qPrintable( fi.lastModified().toString( Qt::ISODate ) ) << ")";
      }
      else
        qWarning() << "Unable to remove partially downloaded from cache:" << qPrintable( fi.fileName() ) << "(last modified" << qPrintable( fi.lastModified().toString( Qt::ISODate ) ) << ")";
    }
  }
  qDebug() << "Cleaned" << item_counter << "partially downloaded files from cache";
}
