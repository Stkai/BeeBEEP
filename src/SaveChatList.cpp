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

#include "ChatManager.h"
#include "GuiChatMessage.h"
#include "SaveChatList.h"
#include "Protocol.h"
#include "Settings.h"


SaveChatList::SaveChatList( QObject* parent )
 : QObject( parent )
{
  setObjectName( "SaveChatList" );
}

void SaveChatList::save()
{
  QString file_name = QDir::toNativeSeparators( QString( "%1/%2" ).arg( Settings::instance().chatSaveDirectory() ).arg( "beebeep.dat" ) );
  QFile file( file_name );
  if( !Settings::instance().chatAutoSave() )
  {
    if( file.exists() )
    {
      qDebug() << "Saved chat file removed:" << file_name;
      file.remove();
    }
    return;
  }

  if( !file.open( QIODevice::WriteOnly ) )
  {
    qWarning() << "Unable to open file" << file.fileName() << ": saving chat messages aborted";
    return;
  }

  QDataStream stream( &file );
  stream.setVersion( Settings::instance().dataStreamVersion( false ) );

  QStringList file_header;
  file_header << Settings::instance().programName();
  file_header << Settings::instance().version( false );
  file_header << QString::number( Settings::instance().protoVersion() );

  stream << file_header;

  saveChats( &stream );

  file.close();
}

void SaveChatList::saveChats( QDataStream* stream )
{
  if( Settings::instance().chatMaxLineSaved() <= 0 )
  {
    (*stream) << 0;
    return;
  }

  qint32 num_of_chats = ChatManager::instance().constChatList().count();
  quint64 file_pos = stream->device()->pos();
 (*stream) << num_of_chats;

  QString chat_footer = QString( "<font color=gray><b>*** %1 %2 ***</b></font><br />" ).arg( QObject::tr( "Saved in" ) ).arg( QDateTime::currentDateTime().toString( Qt::SystemLocaleShortDate ) );
  QStringList chat_lines;
  QString chat_name_encrypted;
  QString chat_text_encrypted;
  qint32 chat_counter = 0;

  foreach( Chat c, ChatManager::instance().constChatList() )
  {
    if( c.isEmpty() )
      continue;
    qDebug() << "Saving chat" << c.name();
    chat_counter++;
    chat_name_encrypted = Protocol::instance().simpleEncryptDecrypt( c.name() );
    (*stream) << chat_name_encrypted;
    QString html_text = GuiChatMessage::chatToHtml( c, true );

    if( !html_text.simplified().isEmpty() )
    {
      html_text.prepend( QString( "<font color=gray><b>*** %1 %2 ***</b></font><br />" ).arg( QObject::tr( "Started in" ) ).arg( c.dateTimeStarted().toString( Qt::SystemLocaleShortDate ) ) );
      html_text.append( chat_footer );
    }

    if( ChatManager::instance().chatHasSavedText( c.name() ) )
    {
      html_text.prepend( ChatManager::instance().chatSavedText( c.name() ) );
      ChatManager::instance().removeSavedTextFromChat( c.name() );
    }

    if( html_text.simplified().isEmpty() )
    {
      (*stream) << QString( "" );
      continue;
    }

    chat_lines = html_text.split( "<br />", QString::SkipEmptyParts );
    if( chat_lines.size() > Settings::instance().chatMaxLineSaved() )
    {
      qWarning() << "Chat exceeds line size limit with" << chat_lines.size();
      while( chat_lines.size() > Settings::instance().chatMaxLineSaved() )
        chat_lines.removeFirst();
      html_text = chat_lines.join( "<br />" );
      html_text.append( "<br />" ); // SkipEmptyParts remove the last one too
    }

    chat_text_encrypted = Protocol::instance().simpleEncryptDecrypt( html_text );
    (*stream) << chat_text_encrypted;
  }

  QMap<QString, QString>::const_iterator it = ChatManager::instance().constHistoryMap().constBegin();
  while( it !=  ChatManager::instance().constHistoryMap().constEnd() )
  {
    qDebug() << "Saving stored chat" << it.key();
    chat_counter++;
    chat_name_encrypted = Protocol::instance().simpleEncryptDecrypt( it.key() );
    (*stream) << chat_name_encrypted;
    chat_text_encrypted = Protocol::instance().simpleEncryptDecrypt( it.value() );
    (*stream) << chat_text_encrypted;
    ++it;
  }

  if( chat_counter != num_of_chats )
  {
    stream->device()->seek( file_pos );
    (*stream) << chat_counter;
  }
}

