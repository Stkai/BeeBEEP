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

#include "MessageManager.h"
#include "Protocol.h"
#include "SaveChatList.h"
#include "Settings.h"


MessageManager* MessageManager::mp_instance = Q_NULLPTR;

MessageManager::MessageManager()
  : m_messagesToSend()
{
}

void MessageManager::addMessageToSend( VNumber to_user_id, VNumber chat_id, const Message& m )
{
  addMessageRecord( MessageRecord( to_user_id, chat_id, m ) );
}

QList<MessageRecord> MessageManager::takeMessagesToSendToUserId( VNumber user_id, bool also_sent_messages )
{
  QList<MessageRecord> message_list;
  if( also_sent_messages )
  {
    QList<MessageRecord>::iterator it = m_sentMessages.begin();
    while( it != m_sentMessages.end() )
    {
      if( (*it).toUserId() == user_id )
      {
        message_list.append( *it );
        it = m_sentMessages.erase( it );
      }
      else
        ++it;
    }
  }

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
  if( !message_list.isEmpty() )
  {
    qSort( message_list );
    saveUnsentMessages( true );
  }
  return message_list;
}

int MessageManager::countMessagesToSendToUserId( VNumber user_id )
{
  int counter = 0;
  foreach( MessageRecord mr, m_messagesToSend )
  {
    if( mr.toUserId() == user_id )
      counter++;
  }
  return counter;
}

int MessageManager::countMessagesToSendInChatId( VNumber chat_id )
{
  int counter = 0;
  foreach( MessageRecord mr, m_messagesToSend )
  {
    if( mr.chatId() == chat_id )
      counter++;
  }
  return counter;
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

bool MessageManager::saveUnsentMessages( bool silent_mode )
{
  QString file_name = Settings::instance().unsentMessagesFilePath();
  QFile file( file_name );
  if( !Settings::instance().enableSaveData() || !Settings::instance().chatSaveUnsentMessages() )
  {
    if( !silent_mode )
      qDebug() << "Unsent chat messages are not saved because you have disabled this option";
    if( file.exists() )
    {
      if( !silent_mode )
        qDebug() << "Unsent messages file removed:" << qPrintable( file_name );
      file.remove();
    }
    return false;
  }

  if( !Settings::instance().saveUserList() || !Settings::instance().saveGroupList() )
  {
    qWarning() << "Saving unsent messages fails because 'Save users' and 'Save groups' options are disabled";
    return false;
  }

  if( !file.open( QIODevice::WriteOnly ) )
  {
    qWarning() << "Unable to open file" << qPrintable( file_name ) << ": saving unsent messages aborted";
    return false;
  }

  if( !silent_mode )
    qDebug() << "Saving unsent messages in" << qPrintable( file_name );
  QDataStream stream( &file );
  stream.setVersion( Settings::instance().dataStreamVersion( false ) );

  QStringList file_header;
  file_header << Settings::instance().programName();
  file_header << Settings::instance().version( false, false, false );
  file_header << QString::number( Settings::instance().protocolVersion() );
  file_header << m_savedMessagesAuthCode;
  stream << file_header;
  if( stream.status() != QDataStream::Ok )
  {
    qWarning() << "Datastream error: unable to save file header in" << qPrintable( file_name );
    file.close();
    return false;
  }

  QStringList sl_smr;
  QList<MessageRecord> mr_list;
  foreach( MessageRecord mr, m_sentMessages )
    mr_list.append( mr );
  foreach( MessageRecord mr, m_messagesToSend )
    mr_list.append( mr );
  if( !mr_list.isEmpty() )
  {
    foreach( MessageRecord mr, mr_list )
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
    qWarning() << "Datastream error: unable to save number of unsent messages in" << qPrintable( file_name );
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
        qWarning() << "Datastream error: unable to save unsent message" << sl_smr_counter << "in" << qPrintable( file_name );
        file.close();
        return false;
      }
    }
  }
  file.close();
  if( !silent_mode )
    qDebug() << sl_smr_size << "unsent messages saved";
  return true;
}

void MessageManager::addMessageRecord( const MessageRecord& mr )
{
  m_messagesToSend.append( mr );
  saveUnsentMessages( true );
}

void MessageManager::addMessageRecords( const QList<MessageRecord>& mr_list )
{
  if( mr_list.isEmpty() )
    return;
  if( m_messagesToSend.isEmpty() )
    m_messagesToSend = mr_list;
  else
    m_messagesToSend.append( mr_list );
  saveUnsentMessages( true );
}

void MessageManager::loadSavedMessagesAuthCode()
{
  QString s = Settings::instance().saveMessagesTimestamp().toString( Qt::ISODate );
  s += Settings::instance().localUser().name();
  QByteArray ba = s.toUtf8().toBase64();
  QByteArray auth_code = QCryptographicHash::hash( ba, QCryptographicHash::Sha1 );
  m_savedMessagesAuthCode = QString::fromLatin1( auth_code.toHex() );
}

void MessageManager::generateSaveMessagesAuthCode()
{
  Settings::instance().setSaveMessagesTimestamp( QDateTime::currentDateTime() );
  Settings::instance().save();
  loadSavedMessagesAuthCode();
}

bool MessageManager::chatMessageCanBeSaved() const
{
  return SaveChatList::canBeSaved();
}

bool MessageManager::saveMessages( bool save_unsent_messages_also )
{
  bool unsent_chat_messages_saved;
  generateSaveMessagesAuthCode();
  if( save_unsent_messages_also )
    unsent_chat_messages_saved = saveUnsentMessages( false );
  else
    unsent_chat_messages_saved = true;
  SaveChatList scl;
  return scl.save() && unsent_chat_messages_saved;
}

void MessageManager::addSentMessage( VNumber to_user_id, VNumber chat_id, const Message& m )
{
  m_sentMessages.append( MessageRecord( to_user_id, chat_id, m ) );
}

bool MessageManager::setMessageReceived( VNumber msg_id )
{
  QList<MessageRecord>::iterator it = m_sentMessages.begin();
  while( it != m_sentMessages.end() )
  {
    if( (*it).message().id() == msg_id )
    {
#ifdef BEEBEEP_DEBUG
      qDebug() << "Message manager sets" << msg_id << "as message received from user" << (*it).toUserId();
#endif
      m_sentMessages.erase( it );
      saveUnsentMessages( true );
      return true;
    }
    ++it;
  }
  return false;
}

bool MessageManager::hasMessageNotReceivedYet( VNumber user_id ) const
{
  QList<MessageRecord>::const_iterator it = m_sentMessages.begin();
  while( it != m_sentMessages.end() )
  {
    if( (*it).toUserId() == user_id )
    {

      if( (*it).message().timestamp().msecsTo( QDateTime::currentDateTime() ) > Settings::instance().messageNotReceivedTimeout() )
        return true;
    }
    ++it;
  }
  return false;
}
