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


class Core : public QObject
{
  Q_OBJECT

public:
  explicit Core( QObject* parent = 0 );

  bool isConnected() const;
  bool start();
  void stop();

#ifdef BEEBEEP_USE_MULTICAST_DNS
  void startDnsMulticasting();
  void stopDnsMulticasting();
  bool dnsMulticastingIsActive() const;
#endif

  int fileTransferPort() const;
  inline bool hasFileTransferInProgress() const;

  bool checkSavingPaths();
  void checkBroadcastInterval();

  /* CoreUser */
  inline int connectedUsers() const;
  bool isUserConnected( VNumber ) const;
  bool areUsersConnected( const QList<VNumber>& ) const;
  void setLocalUserStatus( int );
  void setLocalUserStatusDescription( int, const QString&, bool );
  bool setLocalUserVCard( const QString&, const VCard& );
  bool createGroup( const QString&, const QList<VNumber>&, const QString& group_private_id = "" );
  bool createGroupFromChat( VNumber );
  void changeGroup( VNumber, const QString&, const QList<VNumber>& );
  bool removeGroup( VNumber );
  void toggleUserFavorite( VNumber );
  bool removeOfflineUser( VNumber );

  void loadUsersAndGroups();
  void saveUsersAndGroups();

  /* CoreChat */
  void createPrivateChat( const User& );
  int sendChatMessage( VNumber chat_id, const QString& ); // return the number of message sent (one for every user in chat)
  void showTipOfTheDay();
  void showFactOfTheDay();
  void createGroupChat( const QString& chat_name, const QList<VNumber>&, const QString& chat_private_id, bool broadcast_message );
  void createGroupChat( const Group&, bool broadcast_message );
  void changeGroupChat( VNumber chat_id, const QString& chat_name, const QList<VNumber>&, bool broadcast_message );
  void clearMessagesInChat( VNumber );
  bool removeUserFromChat( const User&, VNumber );
  bool removeChat( VNumber );
  bool readAllMessagesInChat( VNumber );
  bool saveChatMessages();

  /* CoreFileTransfer */
  bool sendFile( VNumber, const QString& file_path );
  bool downloadFile( VNumber, const FileInfo&, bool show_message );
  void refuseToDownloadFile( VNumber, const FileInfo& );
  void refuseToDownloadFolder( VNumber, const QString& );
  bool startFileTransferServer();
  void stopFileTransferServer();
  void addPathToShare( const QString&, bool );
  void removePathFromShare( const QString& );
  void sendShareBoxRequest( VNumber, const QString& );

public slots:
  void sendBroadcastMessage();
  void sendHelloToHostsInSettings();
  void sendMulticastingMessage();
  void checkNetworkInterface();
  void checkNewVersion();
  void postUsageStatistics();
  void onTickEvent( int );

  /* CoreChat */
  void sendWritingMessage( VNumber );
  void buildSavedChatList();

  /* CoreFileTransfer */
  void buildLocalShareList();
  void sendFileShareRequestToAll();
  void cancelFileTransfer( VNumber );
  void removeAllPathsFromShare();

signals:
  void chatMessage( VNumber chat_id, const ChatMessage& );
  void chatReadByUser( VNumber chat_id, VNumber user_id );
  void fileDownloadRequest( const User&, const FileInfo& );
  void folderDownloadRequest( const User&, const QString&, const QList<FileInfo>& );
  void userIsWriting( const User& );
  void userChanged( const User& );
  void fileTransferProgress( VNumber, const User&, const FileInfo&, FileSizeType );
  void fileTransferMessage( VNumber, const User&, const FileInfo&, const QString& );
  void fileShareAvailable( const User& );
  void updateChat( VNumber );
  void localShareListAvailable();
  void savedChatListAvailable();
  void updateStatus( const QString&, int );
  void updateGroup( VNumber );
  void userConnectionStatusChanged( const User& );
  void networkInterfaceIsDown();
  void networkInterfaceIsUp();
  void shareBoxAvailable( const User&, const QString&, const QList<FileInfo>& );

protected slots:
  void showBroadcasterUdpError();
  void onUpdaterJobCompleted();
  void onPostUsageStatisticsJobCompleted();

  /* CoreConnection */
  void checkNewConnection( Connection* );
  void newPeerFound( const QHostAddress&, int );
  void setConnectionError( QAbstractSocket::SocketError );
  void setConnectionClosed();
  void checkUserAuthentication( const Message& );
  void checkUserRecord( const UserRecord& );

  /* CoreParser */
  void parseMessage( VNumber, const Message& );
  void parseMessage( const User&, const Message& );

  /* CoreFileTransfer */
  void checkFileTransferProgress( VNumber, VNumber, const FileInfo&, FileSizeType );
  void checkFileTransferMessage( VNumber, VNumber, const FileInfo&, const QString& );
  void validateUserForFileTransfer( VNumber, const QHostAddress&, const Message& );
  void fileTransferServerListening();
  void addListToLocalShare();
  void addFolderToFileTransfer();
  void sendShareBoxList();

  /* CoreChat */
  void addListToSavedChats();

protected:
  void createLocalShareMessage();
  void checkUserHostAddress( const User& );

  /* CoreConnection */
  Connection* connection( VNumber ) const;
  bool hasConnection( const QHostAddress&, int ) const;
  void closeConnection( Connection* );
  void addConnectionReadyForUse( Connection* );
  void checkOfflineMessagesForUser( const User& );
  void setupNewConnection( Connection* );

  /* CoreParser */
  void parseUserMessage( const User&, const Message& );
  void parseChatMessage( const User&, const Message& );
  void parseFileMessage( const User&, const Message& );
  void parseFileShareMessage( const User&, const Message& );
  void parseGroupMessage( const User&, const Message& );
  void parseFolderMessage( const User&, const Message& );
  void parseChatReadMessage( const User&, const Message& );
  void parseHiveMessage( const User&, const Message& );
  void parseShareBoxMessage( const User&, const Message& );

  /* CoreUser */
  void showUserStatusChanged( const User& );
  void showUserNameChanged( const User&, const QString& );
  void showUserVCardChanged( const User& );
  void sendLocalUserStatus();
  void addGroup( const Group&, bool );
  void sendLocalConnectedUsersTo( const User& );

  /* CoreChat */
  void createDefaultChat();
  QString chatMessageToText( const UserList&, const ChatMessage& );
  bool sendMessageToLocalNetwork( const User& to_user, const Message& );
  void sendGroupChatRequestMessage( const Chat&, const UserList& );
  void sendGroupChatRefuseMessage( const Chat&, const UserList& );
  void checkGroupChatAfterUserReconnect( const User& );
  void sendLocalUserHasReadChatMessage( const Chat& );

  /* CoreDispatcher */
  enum DispatchType { DispatchToAll, DispatchToAllChatsWithUser, DispatchToChat, DispatchToDefaultAndPrivateChat };
  Chat findChatFromMessageData( VNumber from_user_id, const Message& );
  void dispatchSystemMessage( VNumber chat_id, VNumber from_user_id, const QString& msg, DispatchType, ChatMessage::Type );
  void dispatchChatMessageReceived( VNumber from_user_id, const Message& );
  void dispatchChatMessageReadReceived( VNumber from_user_id, const Message& );
  void dispatchToAllChats( const ChatMessage& );
  void dispatchToAllChatsWithUser( const ChatMessage&, VNumber user_id );
  void dispatchToChat( const ChatMessage&, VNumber chat_id );
  void dispatchToDefaultAndPrivateChat( const ChatMessage&, VNumber user_id );

  /* CoreFileTransfer */
  void sendFileShareListTo( VNumber user_id );
  void sendFileShareListToAll();
  bool sendFolder( const User&, const QFileInfo& );
  void buildShareBoxFileList( const User&, const QString& );

private:
  QList<Connection*> m_connections;
  Listener* mp_listener;
  Broadcaster* mp_broadcaster;
  FileTransfer* mp_fileTransfer;
  int m_shareListToBuild;
#ifdef BEEBEEP_USE_MULTICAST_DNS
  MDnsManager* mp_mDns;
#endif

};

// Inline Functions
inline int Core::connectedUsers() const { return m_connections.size(); }
inline bool Core::hasFileTransferInProgress() const { return mp_fileTransfer->hasActivePeers(); }

#endif // BEEBEEP_CLIENT_H
