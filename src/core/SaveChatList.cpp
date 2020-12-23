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

#include "ChatManager.h"
#include "GuiChatMessage.h"
#include "MessageManager.h"
#include "Protocol.h"
#include "SaveChatList.h"
#include "Settings.h"


SaveChatList::SaveChatList( QObject* parent )
 : QObject( parent )
{
  setObjectName( "SaveChatList" );
}

bool SaveChatList::canBeSaved()
{
  QFileInfo file_info( Settings::instance().savedChatsFilePath() );
  if( file_info.exists() )
    return file_info.isWritable();

  QFile file( Settings::instance().savedChatsFilePath() );
  if( file.open( QIODevice::WriteOnly ) )
  {
    file.close();
    file.remove();
    return true;
  }
  else
    return false;
}

bool SaveChatList::save()
{
  QString file_name = Settings::instance().savedChatsFilePath();
  bool saved = saveToFile( file_name, false );
  emit operationCompleted();
  return saved;
}

bool SaveChatList::autoSave()
{
  QString file_name = Settings::instance().autoSavedChatsFilePath();
  bool saved = saveToFile( file_name, true );
  emit operationCompleted();
#ifdef BEEBEEP_DEBUG
  qDebug() << "Autosave chat messages in file" << qPrintable( file_name ) << "->" << (saved ? "success" : "failed");
#endif
  return saved;
}

bool SaveChatList::saveToFile( const QString& file_name, bool silent_mode  )
{
  if( !Settings::instance().enableSaveData() || !Settings::instance().chatAutoSave() )
  {
    if( QFile::exists( file_name ) )
    {
      if( !silent_mode )
        qDebug() << "Chat messages are not saved because you have disabled this option";

      if( QFile::remove( file_name ) )
      {
        if( !silent_mode )
          qDebug() << "Saved chat file removed:" << qPrintable( file_name );
      }
    }
    return false;
  }

  QString file_name_tmp = file_name + ".tmp";
  QFile file( file_name_tmp );

  if( !file.open( QIODevice::WriteOnly ) )
  {
    qWarning() << "Unable to open file" << qPrintable( file_name ) << ": saving chat messages aborted";
    return false;
  }

  Settings::instance().addTemporaryFilePath( file_name_tmp );
  if( !silent_mode )
    qDebug() << "Saving chat messages in" << qPrintable( file_name );

  QDataStream stream( &file );
  stream.setVersion( Settings::instance().dataStreamVersion( false ) );

  QString auth_code = MessageManager::instance().savedMessagesAuthCode();

  QStringList file_header;
  file_header << Settings::instance().programName();
  file_header << Settings::instance().version( false, false, false );
  file_header << QString::number( Settings::instance().protocolVersion() );
  file_header << auth_code;

  bool save_ok = false;
  stream << file_header;
  if( stream.status() != QDataStream::Ok )
    qWarning() << "Datastream error: unable to save file header";
  else
    save_ok = saveChats( &stream, silent_mode );

  file.close();
  if( save_ok )
    return QFile::exists( file_name ) ? (QFile::remove( file_name ) && file.rename( file_name )) : file.rename( file_name );
  else
    return false;
}

bool SaveChatList::saveChats( QDataStream* stream, bool silent_mode )
{
  qint32 num_saved_chats = 0;
  qint64 file_pos = stream->device()->pos();
  (*stream) << num_saved_chats; // we set it to zero until save is not completed
  if( stream->status() != QDataStream::Ok )
  {
    qWarning() << "Datastream error: unable to save number of chats";
    return false;
  }

  QStringList chat_lines;
  QString chat_name_encrypted;
  QString chat_text_encrypted;
  QStringList chat_name_saved_list;
  QString html_text;

  foreach( Chat c, ChatManager::instance().constChatList() )
  {
    if( c.isEmpty() )
      continue;

    html_text = "";

    if( ChatManager::instance().chatHasSavedText( c.name() ) )
      html_text.append( ChatManager::instance().chatSavedText( c.name(), Settings::instance().chatMaxLineSaved() ) );
    html_text.append( GuiChatMessage::chatToHtml( c, !Settings::instance().chatSaveFileTransfers(),
                                                    !Settings::instance().chatSaveSystemMessages(), true, true, true, Settings::instance().useCompactDataSaving() ) );
    if( html_text.simplified().isEmpty() )
    {
      if( !silent_mode )
        qDebug() << "Skip saving empty chat:" << qPrintable( c.name() );
      continue;
    }
    if( !silent_mode )
      qDebug() << "Saving chat:" << qPrintable( c.name() );
    num_saved_chats++;
    chat_name_saved_list << c.name();

    chat_name_encrypted = Settings::instance().simpleEncrypt( c.name() );
    (*stream) << chat_name_encrypted;
    if( stream->status() != QDataStream::Ok )
    {
      qWarning() << "Datastream error: unable to save chat" << qPrintable( c.name() );
      return false;
    }

    chat_lines = html_text.split( "<br>", QString::SkipEmptyParts, Qt::CaseInsensitive );
    if( chat_lines.size() > Settings::instance().chatMaxLineSaved() )
    {
      if( !silent_mode )
        qWarning() << "Chat exceeds line size limit with" << chat_lines.size();
      while( chat_lines.size() > Settings::instance().chatMaxLineSaved() )
        chat_lines.removeFirst();
    }
    html_text = chat_lines.join( "<br>" );
    html_text.append( "<br>" ); // SkipEmptyParts remove the last one too

    chat_text_encrypted = Settings::instance().simpleEncrypt( html_text );
    (*stream) << chat_text_encrypted;
    if( stream->status() != QDataStream::Ok )
    {
      qWarning() << "Datastream error: unable to save messages of chat" << qPrintable( c.name() );
      return false;
    }
  }

  QMap<QString, QString>::const_iterator it = ChatManager::instance().constHistoryMap().constBegin();
  while( it !=  ChatManager::instance().constHistoryMap().constEnd() )
  {
    if( !chat_name_saved_list.contains( it.key() ) )
    {
      if( !silent_mode )
        qDebug() << "Saving history for chat:" << qPrintable( it.key() );
      num_saved_chats++;
      chat_name_encrypted = Settings::instance().simpleEncrypt( it.key() );
      (*stream) << chat_name_encrypted;
      if( stream->status() != QDataStream::Ok )
      {
        qWarning() << "Datastream error: unable to save history name" << qPrintable( it.key() );
        return false;
      }
      html_text = it.value();
      chat_lines = html_text.split( "<br>", QString::SkipEmptyParts, Qt::CaseInsensitive );
      if( chat_lines.size() > Settings::instance().chatMaxLineSaved() )
      {
        if( !silent_mode )
          qWarning() << "History exceeds line size limit with" << chat_lines.size();
        while( chat_lines.size() > Settings::instance().chatMaxLineSaved() )
          chat_lines.removeFirst();
      }
      html_text = chat_lines.join( "<br>" );
      html_text.append( "<br>" ); // SkipEmptyParts remove the last one too
      chat_text_encrypted = Settings::instance().simpleEncrypt( html_text );
      (*stream) << chat_text_encrypted;
      if( stream->status() != QDataStream::Ok )
      {
        qWarning() << "Datastream error: unable to save history messages" << qPrintable( it.key() );
        return false;
      }
    }
    else
    {
      if( !silent_mode )
        qDebug() << "Skip saving history for previous saved chat" << qPrintable( it.key() );
    }
    ++it;
  }

  if( num_saved_chats > 0 )
  {
    stream->device()->seek( file_pos );
    (*stream) << num_saved_chats;
    if( stream->status() != QDataStream::Ok )
    {
      qWarning() << "Datastream error: unable to save number of chats (final)";
      return false;
    }
  }

  ChatManager::instance().setChatMessagesSaved();
  if( !silent_mode )
    qDebug() << num_saved_chats << "chat saved";
  return true;
}

