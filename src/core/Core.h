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

#ifndef BEEBEEP_CORE_H
#define BEEBEEP_CORE_H

#include "Chat.h"
#include "Listener.h"
#include "FileTransfer.h"
class Broadcaster;
class Group;
class UserList;
class UserRecord;
#ifdef BEEBEEP_USE_MULTICAST_DNS
class MDnsManager;
#endif
#ifdef BEEBEEP_USE_SHAREDESKTOP
class ShareDesktop;
class ShareDesktopData;
#endif
#ifdef BEEBEEP_USE_VOICE_CHAT
class VoicePlayer;
#endif

#define beeCore Core::instance()

class Core : public QObject
{
  Q_OBJECT

public:
  explicit Core( QObject* parent = Q_NULLPTR );
  virtual ~Core();
  static Core* instance() { return mp_instance; }

  void init();

  bool isConnected() const;
  bool start();
  void stop();
  void restart();

#ifdef BEEBEEP_USE_MULTICAST_DNS
  bool dnsMulticastingIsActive() const;
#endif

  inline bool isFileTransferActive() const;
  int fileTransferPort() const;
  inline bool hasFileTransferInProgress() const;

  QHostAddress multicastGroupAddress() const;

  bool checkSavingPaths();

  void updateUsersAddedManually();

  /* CoreUser */
  bool changeLocalUser( const QString& );
  int connectedUsers() const;
  bool isUserConnected( VNumber ) const;
  bool areUsersConnected( const QList<VNumber>& ) const;
  void setLocalUserStatus( int );
  void setLocalUserStatusDescription( int, const QString&, bool );
  bool setLocalUserVCard( const QString&, const VCard& );
  void toggleUserFavorite( VNumber );
  bool removeOfflineUser( VNumber );
  void changeUserColor( VNumber, const QString& );
  bool userCanBeRemoved( const User& );
  void regenerateLocalUserHash();
  void setLocalUserWorkgroups( const QStringList& );
  bool hasHelper() const;

  /* CoreChat */
  void createPrivateChat( const User& );
  int sendChatMessage( VNumber chat_id, const QString&, bool is_important, bool can_be_delayed, bool is_source_code ); // return the number of message sent (one for every user in chat)
  bool sendHelpMessage();
  bool sendHelpMessageToUser( VNumber );
  Chat createGroupChat( const User&, const Group&, bool broadcast_message );
  bool changeGroupChat( const User&, const Group& );
  bool clearMessagesInChat( VNumber, bool clear_history );
  bool clearSystemMessagesInChat( VNumber );
  bool removeChat( VNumber, bool save_chat_messages );
  bool readAllMessagesInChat( VNumber );
  void sendBuzzToUser( VNumber );
  void removeSavedChat( const QString& );
  void linkSavedChat( const QString& from_saved_chat_name, const QString& to_saved_chat_name, bool prepend_to_existing_saved_chat );
  bool areAllUsersConnectedInChat( VNumber ) const;

  /* CoreFileTransfer */
  bool sendFile( VNumber user_id, const QString& file_path, const QString& share_folder, bool to_share_box, VNumber chat_id );
  int sendFilesFromChat( VNumber chat_id, const QStringList& file_path_list );
  bool downloadFile( VNumber, const FileInfo&, bool show_message );
  void refuseToDownloadFile( VNumber, const FileInfo& );
  void refuseToDownloadFolder( VNumber, const QString& folder_name, const QString& chat_private_id );
  bool startFileTransferServer();
  void stopFileTransferServer();
  void addPathToShare( const QString& );
  void removePathFromShare( const QString& );
  void sendShareBoxRequest( VNumber, const QString&, bool create_folder );
  void downloadFromShareBox( VNumber from_user_id, const FileInfo&, const QString& to_path );
  void uploadToShareBox( VNumber to_user_id, const FileInfo&, const QString& to_path );
  bool resumeFileTransfer( VNumber user_id, const FileInfo& );

#ifdef BEEBEEP_USE_SHAREDESKTOP
  /* CoreShareDesktop */
  bool startShareDesktop( VNumber user_id );
  void stopShareDesktop( VNumber user_id );
  void stopShareDesktop();
  void refuseToViewShareDesktop( VNumber from_user_id, VNumber to_user_id );
  bool shareDesktopIsActive( VNumber user_id ) const;
#endif

#ifdef BEEBEEP_USE_VOICE_CHAT
  /* CoreVoiceChat.cpp */
  bool sendVoiceMessageToChat( VNumber chat_id, const QString& file_path, qint64 message_duration );
  inline VoicePlayer* voicePlayer();
#endif

signals:
  void connected();
  void disconnected();
  void userChanged( const User& );
  void userIsWriting( const User&, VNumber );
  void userRemoved( const User& );
  void chatChanged( const Chat& );
  void chatRemoved( const Chat& );
  void newChatMessage( const Chat&, const ChatMessage& );
  void chatReadByUser( const Chat&, const User& );
  void offlineMessageSentToUser( const User& );
  void fileDownloadRequest( const User&, const FileInfo& );
  void folderDownloadRequest( const User&, const QString&, const QList<FileInfo>& );
  void fileTransferProgress( VNumber, const User&, const FileInfo&, FileSizeType, qint64 );
  void fileTransferMessage( VNumber, const User&, const FileInfo&, const QString&, FileTransferPeer::TransferState );
  void fileShareAvailable( const User& );
  void localShareListAvailable();
  void savedChatListAvailable();
  void userConnectionStatusChanged( const User& );
  void networkInterfaceIsDown();
  void networkInterfaceIsUp();
  void shareBoxAvailable( const User&, const QString&, const QList<FileInfo>& );
  void shareBoxUnavailable( const User&, const QString& );
  void localUserIsBuzzedBy( const User&, VNumber );
  void helpRequestFrom( const User&, VNumber );
  void helpAnswerFrom( const User&, VNumber );
  void newSystemStatusMessage( const QString&, int );
  void newsAvailable( const QString& );
#ifdef BEEBEEP_USE_SHAREDESKTOP
  void shareDesktopImageAvailable( const User&, const QImage&, const QString&, QRgb );
  void shareDesktopUpdate( const User& );
#endif
#ifdef BEEBEEP_USE_MULTICAST_DNS
  void multicastDnsChanged();
#endif

public slots:
  void sendBroadcastMessage();
  void checkNetworkInterface();
  void checkNewVersion();
  void postUsageStatistics();
  void onTickEvent( int );
#ifdef BEEBEEP_USE_MULTICAST_DNS
  void startDnsMulticasting();
  void stopDnsMulticasting();
  void sendDnsMulticastingMessage();
#endif

  /* CoreConnection */
  void checkConnectionPorts();
  void checkFirewall();
  void updateNetworkConfiguration( const QNetworkConfiguration& );

  /* CoreChat */
  void sendWritingMessage( VNumber );
  void buildSavedChatList();

  /* CoreFileTransfer */
  void buildLocalShareList();
  void sendFileShareRequestToAll();
  void cancelFileTransfer( VNumber );
  void pauseFileTransfer( VNumber );
  void removeAllPathsFromShare();

#ifdef BEEBEEP_USE_SHAREDESKTOP
  /* CoreShareDesktop */
  bool sendScreenshotToChat( VNumber chat_id );
#endif

protected slots:
  bool restartConnection();
  void onUpdaterJobCompleted();
  void onPostUsageStatisticsJobCompleted();
#ifdef BEEBEEP_USE_MULTICAST_DNS
  void onMulticastDnsServiceRegistered();
#endif

  /* CoreUser */
  void saveUsersAndGroups();

  /* CoreConnection */
  void checkNewConnection( qintptr );
  void newPeerFound( const QHostAddress&, int );
  void setConnectionError( QAbstractSocket::SocketError );
  void setConnectionClosed();
  void checkUserAuthentication( const QByteArray& );
  void checkNetworkAddress( const NetworkAddress& );

  /* CoreParser */
  void parseMessage( VNumber, const Message& );
  void parseMessage( const User&, const Message& );

  /* CoreFileTransfer */
  void checkFileTransferProgress( VNumber, VNumber, const FileInfo&, FileSizeType, qint64 );
  void checkFileTransferMessage( VNumber, VNumber, const FileInfo&, const QString&, FileTransferPeer::TransferState );
  void onFileTransferServerListening();
  void addListToLocalShare();
  void addFolderToFileTransfer();
  void sendShareBoxList();
#ifdef BEEBEEP_USE_SHAREDESKTOP
  void onShareDesktopImageAvailable( const ShareDesktopData& );
#endif
  /* CoreChat */
  void addListToSavedChats();
  void autoSaveChatMessages();
  void autoSaveChatMessagesCompleted();

protected:
  void loadUsersAndGroups();
  void createLocalShareMessage();
  void showMessage( const QString&, int ms_to_show );

  /* CoreConnection */
  Connection* connection( VNumber user_id ) const;
  bool hasConnection( const QHostAddress&, int ) const;
  void closeConnection( Connection* );
  void addConnectionReadyForUse( Connection*, const User& );
  int checkOfflineMessagesForUser( const User&, bool on_new_connection );
  void setupNewConnection( Connection* );
  Connection* createConnection();

  /* CoreParser */
  void parseUserMessage( const User&, const Message& );
  void parseChatMessage( const User&, const Message& );
  void parseFileMessage( const User&, const Message& );
  void parseFileShareMessage( const User&, const Message& );
  void parseGroupMessage( const User&, const Message& );
  void parseFolderMessage( const User&, const Message& );
  void parseChatReadMessage( const User&, const Message& );
  void parseReceivedMessage( const User&, const Message& );
  void parseHiveMessage( const User&, const Message& );
  void parseShareBoxMessage( const User&, const Message& );
  void parseBuzzMessage( const User&, const Message& );
#ifdef BEEBEEP_USE_SHAREDESKTOP
  void parseShareDesktopMessage( const User&, const Message& );
#endif
  void parseHelpMessage( const User&, const Message& );

  /* CoreUser */
  void showUserNameChanged( const User&, const QString& old_user_name );
  void showUserVCardChanged( const User&, const VCard& old_vcard );
  void sendLocalUserStatus();
  void addGroup( const Group& );
  void sendLocalConnectedUsersTo( const User& );
  bool isUserConnected( const NetworkAddress& ) const; // to prevent multiple connections in Core::newPeerFound(...)
  void removeInactiveUsers();
  void sendVCardToAllConnectedUsers();

  /* CoreChat */
  void createDefaultChat();
  bool sendMessageToLocalNetwork( const User& to_user, const Message& );
  void sendGroupChatRequestMessage( const Chat&, const UserList&, const User& );
  void sendRefuseMessageToGroupChat( const Chat& );
  int checkGroupChatAfterUserReconnect( const User& );
  void sendLocalUserHasReadChatMessage( const Chat& );
  void addChatHeader( Chat* );
  bool removeUserFromGroupChat( const User&, const QString& chat_private_id );
  bool removeLocalUserFromGroupChatByOther( const User& other_user, const QString& chat_private_id );
  int archiveAllChats();
  bool sendChatAutoResponderMessageToUser( const Chat&, const QString& msg, VNumber user_id );
  int sendMessageToChat( const Chat&, const Message& );

  /* CoreDispatcher */
  enum DispatchType { DispatchToAll, DispatchToAllChatsWithUser, DispatchToChat, DispatchToDefaultAndPrivateChat };
  Chat findChatFromMessageData( VNumber from_user_id, const Message& );
  void dispatchSystemMessage( VNumber chat_id, VNumber from_user_id, const QString& msg, DispatchType, ChatMessage::Type, bool can_be_saved );
  bool dispatchChatMessageReceived( VNumber from_user_id, const Message& );
  void dispatchToAllChats( const ChatMessage& );
  void dispatchToAllChatsWithUser( const ChatMessage&, VNumber user_id );
  void dispatchToChat( const ChatMessage&, VNumber chat_id );
  void dispatchToDefaultAndPrivateChat( const ChatMessage&, VNumber user_id );

  /* CoreFileTransfer */
  bool sendFileToUser( const User&, const QString& file_path, const QString& share_folder, bool to_share_box, const Chat& );
  bool showFilesUploadPreviewInChat( const Chat&, const QStringList& file_paths );
  bool showFileUploadPreviewInChat( const Chat&, const QString& file_path );
  void sendFileShareListTo( VNumber user_id );
  void sendFileShareListToAll();
  bool sendFolder( const User&, const QFileInfo&, const QString& chat_private_id );
  void buildShareBoxFileList( const User&, const QString&, bool create_folder );

private:
  static Core* mp_instance;
  QList<Connection*> m_connections;
  Listener* mp_listener;
  Broadcaster* mp_broadcaster;
  FileTransfer* mp_fileTransfer;
  int m_shareListToBuild;
#ifdef BEEBEEP_USE_MULTICAST_DNS
  MDnsManager* mp_mDns;
#endif
#ifdef BEEBEEP_USE_SHAREDESKTOP
  ShareDesktop* mp_shareDesktop;
#endif
#ifdef BEEBEEP_USE_VOICE_CHAT
  VoicePlayer* mp_voicePlayer;
#endif

};

// Inline Functions
inline bool Core::hasFileTransferInProgress() const { return isConnected() && mp_fileTransfer->hasActivePeers(); }
inline bool Core::isFileTransferActive() const { return isConnected() && mp_fileTransfer->isActive(); }
#ifdef BEEBEEP_USE_VOICE_CHAT
inline VoicePlayer* Core::voicePlayer() { return mp_voicePlayer; }
#endif

#endif // BEEBEEP_CLIENT_H
