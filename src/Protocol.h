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

#ifndef BEEBEEP_PROTOCOL_H
#define BEEBEEP_PROTOCOL_H

#include "Chat.h"
#include "FileInfo.h"
#include "Group.h"
#include "Message.h"
#include "ChatMessageData.h"
#include "User.h"
#include "UserRecord.h"
#include "UserStatusRecord.h"


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
  QByteArray helloMessage( const QString& cipher_key_tmp ) const;
  inline const QByteArray& writingMessage() const;
  inline Message systemMessage( const QString& ) const;
  Message chatMessage( const Chat&, const QString& );
  Message chatReadMessage( const Chat& );
  Message groupChatRequestMessage( const Chat&, const User& to_user );
  Message groupChatRefuseMessage( const Chat& );
  QStringList userPathsFromGroupRequestMessage( const Message& ) const;
  Message fileInfoToMessage( const FileInfo& );
  Message fileInfoRefusedToMessage( const FileInfo& );
  FileInfo fileInfoFromMessage( const Message& );
  FileInfo fileInfo( const QFileInfo&, const QString& );
  QString fileInfoHash( const QFileInfo& ) const;
  QString fileInfoHashTmp( VNumber, const QString&, FileSizeType ) const;
  ChatMessageData dataFromChatMessage( const Message& );
  QString chatMessageDataToString( const ChatMessageData& );
  int protoVersion( const Message& ) const;
  QString publicKey( const Message& ) const;
  QByteArray createCipherKey( const QString&, const QString& ) const;
  QByteArray bytesArrivedConfirmation( int ) const;
  Message createFolderMessage( const QString&, const QList<FileInfo>&, int server_port );
  QList<FileInfo> messageFolderToInfoList( const Message&, const QHostAddress&, QString* pFolderName = 0 ) const;
  Message folderRefusedToMessage( const QString& );
  QStringList workgroupsFromHelloMessage( const Message& ) const;
  bool acceptConnectionFromWorkgroup( const Message& ) const;

  Message userStatusMessage( int user_status, const QString& user_status_description ) const;
  QByteArray localUserStatusMessage() const;
  bool changeUserStatusFromMessage( User*, const Message& ) const;
  QByteArray localUserNameMessage() const;
  bool changeUserNameFromMessage( User*, const Message& ) const;
  QByteArray localVCardMessage() const;
  bool changeVCardFromMessage( User*, const Message& ) const;

  static bool fileCanBeShared( const QFileInfo& );
  int countFilesCanBeSharedInPath( const QString& );
  void createFileShareListMessage( const QMultiMap<QString, FileInfo>&, int server_port );
  inline const QByteArray& fileShareListMessage() const;
  QList<FileInfo> messageToFileShare( const Message&, const QHostAddress& ) const;
  inline const QByteArray& fileShareRequestMessage() const;

  User createUser( const Message&, const QHostAddress& );
  User createTemporaryUser( const QString& user_path, const QString& account_name );
  User createTemporaryUser( const QString& user_name, const QString& user_account_name, const QHostAddress& user_address, int user_port );
  Chat createChat( const QList<VNumber>& user_list );
  Group createGroup( const QString& group_name, const QString& group_private_id, const QList<VNumber>& user_list );
  QString saveGroup( const Group& ) const;
  Group loadGroup( const QString& );

  QString saveUser( const User& ) const;
  User loadUser( const QString& );
  QString saveUserRecord( const UserRecord& ) const;
  UserRecord loadUserRecord( const QString& ) const;
  QString saveUserStatusRecord( const UserStatusRecord& ) const;
  UserStatusRecord loadUserStatusRecord( const QString& ) const;

  inline VNumber currentId() const;
  inline VNumber newId();
  QString newMd5Id();

  QString linkifyText( QString );
  QString formatHtmlText( const QString& );

  QByteArray encryptByteArray( const QByteArray& text_to_encrypt, const QByteArray& cipher_key ) const;
  QByteArray decryptByteArray( const QByteArray& text_to_decrypt, const QByteArray& cipher_key ) const;

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

  QList<QByteArray> splitByteArray( const QByteArray&, int ) const;

private:
  VNumber m_id;
  QByteArray m_writingMessage;
  QByteArray m_fileShareListMessage;
  QByteArray m_fileShareRequestMessage;

};

// Inline Functions
inline VNumber Protocol::currentId() const { return m_id; }
inline VNumber Protocol::newId() { return ++m_id; }
inline int Protocol::messageMinimumSize() const { return 10; }
inline const QByteArray& Protocol::writingMessage() const { return m_writingMessage; }
inline Message Protocol::systemMessage( const QString& msg_txt ) const { return Message( Message::System, ID_SYSTEM_MESSAGE, msg_txt ); }
inline const QByteArray& Protocol::fileShareListMessage() const { return m_fileShareListMessage; }
inline const QByteArray& Protocol::fileShareRequestMessage() const { return m_fileShareRequestMessage; }

#endif // BEEBEEP_PROTOCOL_H
