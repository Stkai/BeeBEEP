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

#include "MessageManager.h"
#include "Protocol.h"
#include "Settings.h"


MessageManager* MessageManager::mp_instance = Q_NULLPTR;

MessageManager::MessageManager()
  : m_messagesToSend()
{
}

void MessageManager::addMessageToSend( VNumber to_user_id, VNumber chat_id, const Message& m )
{
  MessageRecord mr( to_user_id, chat_id, m );
  m_messagesToSend.append( mr );
}

QList<MessageRecord> MessageManager::takeMessagesToSend( VNumber user_id )
{
  QList<MessageRecord> message_list;
  QList<MessageRecord>::iterator it = m_messagesToSend.begin();
  while( it != m_messagesToSend.end() )
  {
    if( (*it).toUserId() == user_id )
    {
      message_list.append( *it );
      it = m_messagesToSend.erase( it );
    }
    else
      ++it;
  }
  return message_list;
}

bool MessageManager::unsentMessagesCanBeSaved() const
{
  QFileInfo file_info( Settings::instance().unsentMessagesFilePath() );
  if( file_info.exists() )
    return file_info.isWritable();

  QFile file( Settings::instance().unsentMessagesFilePath() );
  if( file.open( QIODevice::WriteOnly ) )
  {
    file.close();
    file.remove();
    return true;
  }
  else
    return false;
}

bool MessageManager::saveUnsentMessages()
{
  QString file_name = Settings::instance().unsentMessagesFilePath();
  QFile file( file_name );
  if( !Settings::instance().chatSaveUnsentMessages() )
  {
    if( file.exists() )
    {
      qDebug() << "Unsent messages file removed:" << qPrintable( file_name );
      file.remove();
    }
    return false;
  }

  if( !file.open( QIODevice::WriteOnly ) )
  {
    qWarning() << "Unable to open file" << qPrintable( file_name ) << ": saving unsent messages aborted";
    return false;
  }

  qDebug() << "Saving unsent messages in" << qPrintable( file_name );
  QDataStream stream( &file );
  stream.setVersion( Settings::instance().dataStreamVersion( false ) );

  QStringList file_header;
  file_header << Settings::instance().programName();
  file_header << Settings::instance().version( false, false );
  file_header << QString::number( Settings::instance().protoVersion() );
  stream << file_header;
  if( stream.status() != QDataStream::Ok )
  {
    qWarning() << "Datastream error: unable to save file header";
    file.close();
    return false;
  }

  QStringList sl_smr;
  if( !m_messagesToSend.isEmpty() )
  {
    foreach( MessageRecord mr, m_messagesToSend )
    {
      QString smr = Protocol::instance().saveMessageRecord( mr );
      if( !smr.isEmpty() )
        sl_smr.append( Settings::instance().simpleEncrypt( smr ) );
    }
  }
  qint32 sl_smr_size = sl_smr.size();
  stream << sl_smr_size;
  if( stream.status() != QDataStream::Ok )
  {
    qWarning() << "Datastream error: unable to save number of unsent messages";
    file.close();
    return false;
  }
  if( sl_smr_size > 0 )
  {
    int sl_smr_counter = 0;
    foreach( QString smr, sl_smr )
    {
      sl_smr_counter++;
      stream << smr;
      if( stream.status() != QDataStream::Ok )
      {
        qWarning() << "Datastream error: unable to save unsent message" << sl_smr_counter;
        file.close();
        return false;
      }
    }
  }
  file.close();
  m_messagesToSend.clear();
  qDebug() << sl_smr_size << "unsent messages saved";
  return true;
}

void MessageManager::addMessageRecords( const QList<MessageRecord>& mr_list )
{
  if( m_messagesToSend.isEmpty() )
    m_messagesToSend = mr_list;
  else
    m_messagesToSend.append( mr_list );
}
