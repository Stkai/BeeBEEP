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

  loadSavedChats( &stream );

  file.close();

  m_elapsedTime = elapsed_time.elapsed();

  emit listCompleted();
}

void BuildSavedChatList::loadSavedChats( QDataStream* stream )
{
  int num_of_chats = 0;
  (*stream) >> num_of_chats;

  if( num_of_chats <= 0 )
    return;

  QString chat_name_encrypted;
  QString chat_text_encrypted;
  QString chat_name;
  QString chat_text;

  for( int i = 0; i < num_of_chats; i++ )
  {
    (*stream) >> chat_name_encrypted;
    (*stream) >> chat_text_encrypted;

    if( stream->status() != QDataStream::Ok )
    {
      qWarning() << "Error reading datastream, abort loading chat";
      return;
    }

    chat_name = Settings::instance().simpleDecrypt( chat_name_encrypted );
    chat_text = Settings::instance().simpleDecrypt( chat_text_encrypted );

    qDebug() << "Loading chat" << chat_name;

    if( chat_text.simplified().isEmpty() )
      qDebug() << "The chat" << chat_name << "saved is empty";
    else
      m_savedChats.insert( chat_name, chat_text );
  }
}

