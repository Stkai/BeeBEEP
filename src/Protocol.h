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
// Author: Marco Mastroddi (marco.mastroddi(AT)gmail.com)
//
// $Id$
//
//////////////////////////////////////////////////////////////////////

#ifndef BEEBEEP_PROTOCOL_H
#define BEEBEEP_PROTOCOL_H

#include "Chat.h"
#include "FileInfo.h"
#include "Message.h"
#include "ChatMessageData.h"
#include "User.h"


class Protocol
{
  // Singleton Object
  static Protocol* mp_instance;

public:
  inline int messageMinimumSize() const;
  QByteArray fromMessage( const Message& ) const;
  Message toMessage( const QByteArray& ) const;

  QByteArray pingMessage() const;
  QByteArray pongMessage() const;
  QByteArray broadcastMessage() const;
  QByteArray helloMessage() const;
  inline const QByteArray& writingMessage() const;
  inline Message systemMessage( const QString& ) const;
  inline Message chatMessage( const QString& );
  Message fileInfoToMessage( const FileInfo& );
  Message fileInfoRefusedToMessage( const FileInfo& );
  FileInfo fileInfoFromMessage( const Message& );
  FileInfo fileInfo( const QFileInfo& );
  ChatMessageData dataFromChatMessage( const Message& );
  QString chatMessageDataToString( const ChatMessageData& );
  int protoVersion( const Message& ) const;

  Message userStatusMessage( int user_status, const QString& user_status_description ) const;
  QByteArray localUserStatusMessage() const;
  bool changeUserStatusFromMessage( User*, const Message& ) const;
  QByteArray localUserNameMessage() const;
  bool changeUserNameFromMessage( User*, const Message& ) const;
  QByteArray localVCardMessage() const;
  bool changeVCardFromMessage( User*, const Message& ) const;

  void createFileShareListMessage( const QMultiMap<QString, FileInfo>&, int server_port );
  inline const QByteArray& fileShareListMessage() const;
  QList<FileInfo> messageToFileShare( const Message&, const QHostAddress& ) const;
  inline const QByteArray& fileShareRequestMessage() const;

  User createUser( const Message&, const QHostAddress& );
  Chat createChat( const QList<VNumber>& user_list );

  inline VNumber newId();
  QString newMd5Id() const;

  QByteArray encryptByteArray( const QByteArray& ) const;
  QByteArray decryptByteArray( const QByteArray& ) const;

  static QString simpleEncrypt( const QString& );
  static QString simpleDecrypt( const QString& );

  static Protocol& instance()
  {
    if( !mp_instance )
      mp_instance = new Protocol();
    return *mp_instance;
  }

  static void close()
  {
    if( mp_instance )
    {
      delete mp_instance;
      mp_instance = NULL;
    }
  }

protected:
  Protocol();
  QString messageHeader( Message::Type ) const;
  Message::Type messageType( const QString& ) const;

  QString pixmapToString( const QPixmap& ) const;
  QPixmap stringToPixmap( const QString& ) const;

private:
  VNumber m_id;
  QByteArray m_writingMessage;
  QByteArray m_fileShareListMessage;
  QByteArray m_fileShareRequestMessage;

};


// Inline Functions
inline VNumber Protocol::newId() { return ++m_id; }
inline int Protocol::messageMinimumSize() const { return 10; }
inline const QByteArray& Protocol::writingMessage() const { return m_writingMessage; }
inline Message Protocol::chatMessage( const QString& msg_txt ) { return Message( Message::Chat, newId(), msg_txt ); }
inline Message Protocol::systemMessage( const QString& msg_txt ) const { return Message( Message::System, ID_SYSTEM_MESSAGE, msg_txt ); }
inline const QByteArray& Protocol::fileShareListMessage() const { return m_fileShareListMessage; }
inline const QByteArray& Protocol::fileShareRequestMessage() const { return m_fileShareRequestMessage; }

#endif // BEEBEEP_PROTOCOL_H
