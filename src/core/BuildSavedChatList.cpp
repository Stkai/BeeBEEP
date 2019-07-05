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

#include "BuildSavedChatList.h"
#include "Protocol.h"
#include "Settings.h"


BuildSavedChatList::BuildSavedChatList( QObject *parent )
  : QObject( parent )
{
  setObjectName( "BuildSavedChatList" );
}

void BuildSavedChatList::buildList()
{
  QTime elapsed_time;
  elapsed_time.start();

  QString file_name = Settings::instance().savedChatsFilePath();
  QFile file( file_name );

  if( !file.open( QIODevice::ReadOnly ) )
  {
    qWarning() << "Unable to open file" << file.fileName() << ": loading saved chats aborted";
    return;
  }

  QDataStream stream( &file );
  stream.setVersion( Settings::instance().dataStreamVersion( true ) );

  QStringList file_header;

  stream >> file_header;
  if( stream.status() != QDataStream::Ok )
    qWarning() << "Error reading header datastream, abort loading saved chats";
  else
    loadSavedChats( &stream );
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
}

void BuildSavedChatList::loadUnsentMessages()
{
  m_unsentMessagesAuthCode = QString::null;
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

  if( file_header.size() < 4 )
  {
    qWarning() << file_header.size() << "is invalid header size in file" << qPrintable( file_name );
    file.close();
    return;
  }

  m_unsentMessagesAuthCode = file_header.at( 3 );
  if( m_unsentMessagesAuthCode.isEmpty() )
  {
    qWarning() << "Empty AUTH code found in the header of file" << qPrintable( file_name ) << ": loading saved unsent messages aborted";
    file.close();
    return;
  }

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


