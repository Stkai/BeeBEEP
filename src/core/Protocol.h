//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2020 Marco Mastroddi
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

#ifndef BEEBEEP_PROTOCOL_H
#define BEEBEEP_PROTOCOL_H

#include "Chat.h"
#include "ChatRecord.h"
#include "FileInfo.h"
#include "Group.h"
#include "Message.h"
#include "MessageRecord.h"
#include "ChatMessageData.h"
#include "User.h"
#include "UserRecord.h"
#include "UserStatusRecord.h"
#ifdef BEEBEEP_USE_SHAREDESKTOP
  #include "ShareDesktopData.h"
#endif


class Protocol
{
  // Singleton Object
  static Protocol* mp_instance;

public:
  inline int datastreamMaxVersion() const;
  inline int messageMinimumSize() const;
  QByteArray fromMessage( const Message&, int proto_version ) const;
  Message toMessage( const QByteArray&, int proto_version ) const;

  QByteArray pingMessage() const;
  QByteArray pongMessage() const;
  QByteArray broadcastMessage( const QHostAddress& ) const;
  QHostAddress hostAddressFromBroadcastMessage( const Message& ) const;
  QByteArray helloMessage( const QByteArray& public_key, bool encrypted_connection, bool data_compressed ) const;
  QByteArray testQuestionMessage( const NetworkAddress& ) const;
  bool isTestQuestionMessage( const Message& ) const;
  QByteArray testAnswerMessage( const NetworkAddress&, bool test_is_accepted, const QString& answer_msg = "Ok" ) const;
  NetworkAddress networkAddressFromTestMessage( const Message& ) const;
  Message writingMessage( const QString& chat_private_id ) const;
  bool isTestAnswerMessage( const Message& ) const;
  inline Message systemMessage( const QString& ) const;
  Message chatMessage( const Chat&, const QString& );
  Message chatReadMessage( const Chat& );
  Message groupChatRequestMessage_obsolete( const Chat&, const User& to_user );
  Message groupChatRequestMessage( const Chat&, const User& to_user );
  Message groupChatRemoveUserMessage( const Chat& );
  Message groupChatRefuseMessage( const Chat& );
  Message groupChatRefuseMessage( const ChatMessageData& );
  QStringList userPathsFromGroupRequestMessage_obsolete( const Message& ) const;
  QList<UserRecord> userRecordsFromGroupRequestMessage( const Message& ) const;
  Message fileInfoToMessage( const FileInfo&, int proto_version );
  Message fileInfoRefusedToMessage( const FileInfo&, int proto_version );
  FileInfo fileInfoFromMessage( const Message&, int proto_version );
  FileInfo fileInfo( const QFileInfo&, const QString& share_folder, bool to_share_box, const QString& chat_private_id, FileInfo::ContentType );
  QString fileInfoHash( const QFileInfo& ) const;
  QString fileInfoHashTmp( VNumber, const QString&, FileSizeType ) const;
  ChatMessageData dataFromChatMessage( const Message& ) const;
  QString chatMessageDataToString( const ChatMessageData& ) const;
  int protocolVersion( const Message& ) const;
  int datastreamVersion( const Message& ) const;
  QByteArray fileTransferBytesArrivedConfirmation( int proto_version, FileSizeType bytes_arrived_size, FileSizeType total_bytes_arrived_size, bool pause_transfer  ) const;
  bool parseFileTransferBytesArrivedConfirmation( int proto_version, const QByteArray& bytes_arrived, FileSizeType* bytes_arrived_size, FileSizeType* total_bytes_arrived_size, bool* pause_transfer ) const;
  Message createFolderMessage( const QString&, const QList<FileInfo>&, int server_port );
  QList<FileInfo> messageFolderToInfoList( const Message&, const QHostAddress&, QString* pFolderName = Q_NULLPTR ) const;
  Message folderRefusedToMessage( const QString&, const QString& );
  QStringList workgroupsFromHelloMessage( const Message& ) const;
  bool acceptConnectionFromWorkgroup( const Message& ) const;
  inline Message buzzMessage() const;

  Message userStatusMessage( int user_status, const QString& user_status_description ) const;
  bool changeUserStatusFromMessage( User*, const Message& ) const;
  Message localVCardMessage() const;
  bool changeVCardFromMessage( User*, const Message& ) const;

  static bool fileCanBeShared( const QFileInfo& );
  int countFilesCanBeSharedInPath( const QString& );
  void createFileShareListMessage( const QMultiMap<QString, FileInfo>&, int server_port );
  inline const Message& fileShareListMessage() const;
  QList<FileInfo> messageToFileShare( const Message&, const QHostAddress& ) const;
  Message fileShareRequestMessage() const;

  User recognizeUser( const User&, int user_recognition_method ) const;
  User recognizeUser( const UserRecord&, int user_recognition_method ) const;
  User createUser( const Message&, const QHostAddress& );
  User createTemporaryUser( const UserRecord& );
  Chat createDefaultChat();
  Chat createPrivateChat( const User& );
  Chat createChat( const Group&, Group::ChatType );
  QString saveGroup( const Group& ) const;
  Group loadGroup( const QString& );

  QString saveUser( const User& ) const;
  User loadUser( const QString& );
  QString saveUserRecord( const UserRecord&, bool add_extras ) const;
  UserRecord loadUserRecord( const QString& ) const;
  QString saveUserStatusRecord( const UserStatusRecord& ) const;
  UserStatusRecord loadUserStatusRecord( const QString& ) const;
  Message userRecordListToHiveMessage( const QList<UserRecord>& );
  QList<UserRecord> hiveMessageToUserRecordList( const Message& ) const;

  QString saveMessageRecord( const MessageRecord& ) const;
  MessageRecord loadMessageRecord( const QString& ) const;

  QString saveNetworkAddress( const NetworkAddress& ) const;
  NetworkAddress loadNetworkAddress( const QString& ) const;

  Message shareBoxRequestPathList( const QString&, bool set_create_flag );
  Message refuseToShareBoxPath( const QString&, bool set_create_flag );
  Message acceptToShareBoxPath( const QString&, const QList<FileInfo>&, int );
  QString folderNameFromShareBoxMessage( const Message& ) const;
  QList<FileInfo> messageToShareBoxFileList( const Message&, const QHostAddress& ) const;

#ifdef BEEBEEP_USE_SHAREDESKTOP
  Message refuseToViewDesktopShared() const;
  Message readImageFromDesktopShared() const;
  Message shareDesktopImageDataToMessage( const ShareDesktopData& ) const;
  ShareDesktopData imageDataFromShareDesktopMessage( const Message& ) const;
#endif

#ifdef BEEBEEP_USE_VOICE_CHAT

#endif

  inline VNumber currentId() const;
  inline VNumber maxId() const;
  inline VNumber newId();
  QString newMd5Id();

  QString linkifyText( QString );
  QString formatHtmlText( const QString& );

  QString saveChatRecord( const ChatRecord& ) const;
  ChatRecord loadChatRecord( const QString& ) const;

  QByteArray generatePrivateKey() const;
  QByteArray generatePublicKey( const QByteArray& private_key ) const;
  QByteArray publicKey( const Message& ) const;
  QByteArray generateSharedKey( const QByteArray& private_key, const QByteArray& public_key, int key_exchange_method, int data_stream_version ) const;

  QByteArray encryptByteArray( const QByteArray& text_to_encrypt, const QByteArray& cipher_key, int proto_version ) const;
  QByteArray decryptByteArray( const QByteArray& text_to_decrypt, const QByteArray& cipher_key, int proto_version ) const;

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
      mp_instance = Q_NULLPTR;
    }
  }

protected:
  Protocol();
  QString messageHeader( Message::Type ) const;
  Message::Type messageType( const QString& ) const;

  QString pixmapToString( const QPixmap& ) const;
  QPixmap stringToPixmap( const QString& ) const;

  QByteArray generateECDHRandomPrivateKey() const;
  QByteArray generateECDHPublicKey( const QByteArray& private_key ) const;
  QByteArray generateECDHSharedCipherKey( const QByteArray& private_key, const QByteArray& other_public_key ) const;

  QList<QByteArray> splitByteArray( const QByteArray&, int ) const;
  void hexToUnsignedChar( const QByteArray&, unsigned char* out_string, unsigned int len_out_string ) const;

private:
  VNumber m_id;
  int m_datastreamMaxVersion;
  Message m_fileShareListMessage;

};

// Inline Functions
inline VNumber Protocol::currentId() const { return m_id; }
inline VNumber Protocol::newId() { return ++m_id; }
inline VNumber Protocol::maxId() const { return 18446744073709000000u; }
inline int Protocol::messageMinimumSize() const { return 10; }
inline Message Protocol::systemMessage( const QString& msg_txt ) const { return Message( Message::System, ID_SYSTEM_MESSAGE, msg_txt ); }
inline Message Protocol::buzzMessage() const { return Message( Message::Buzz, ID_BUZZ_MESSAGE, QLatin1String( "*" ) ); }
inline int Protocol::datastreamMaxVersion() const { return m_datastreamMaxVersion; }
inline const Message& Protocol::fileShareListMessage() const { return m_fileShareListMessage; }

#endif // BEEBEEP_PROTOCOL_H
