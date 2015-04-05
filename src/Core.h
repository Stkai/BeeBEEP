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


class Core : public QObject
{
  Q_OBJECT

public:
  explicit Core( QObject* parent = 0 );

  bool isConnected() const;
  bool start();
  void stop();

  void addBroadcastAddress( const QHostAddress& );
  void sendBroadcastMessage();

  /* CoreUser */
  inline int connectedUsers() const;
  void setLocalUserStatus( int );
  void setLocalUserStatusDescription( const QString& );
  void setLocalUserVCard( const VCard& );
  void createGroup( const QString&, const QList<VNumber>& );
  void changeGroup( VNumber, const QString&, const QList<VNumber>& );
  void removeGroup( VNumber );

  void loadGroups();
  void saveGroups();

  /* CoreChat */
  int sendChatMessage( VNumber chat_id, const QString& ); // return the number of message sent (one for every user in chat)
  void showTipOfTheDay();
  void showFactOfTheDay();
  void createGroupChat( const QString& chat_name, const QList<VNumber>&, const QString& chat_private_id, bool broadcast_message );
  void createGroupChat( const Group&, bool broadcast_message );
  void changeGroupChat( VNumber chat_id, const QString& chat_name, const QList<VNumber>&, bool broadcast_message );
  void clearMessagesInChat( VNumber );
  bool removeUserFromChat( const User&, VNumber );
  bool removeChat( VNumber );

  /* CoreFileTransfer */
  bool sendFile( const User&, const QString& file_path );
  void downloadFile( const User&, const FileInfo& );
  void refuseToDownloadFile( const User&, const FileInfo& );
  bool startFileTransferServer();
  void stopFileTransferServer();
  void addPathToShare( const QString&, bool );
  void removePathFromShare( const QString& );

public slots:
  /* CoreChat */
  void sendWritingMessage( VNumber );
  void buildSavedChatList();

  /* CoreFileTransfer */
  void buildLocalShareList();
  void sendFileShareRequestToAll();
  void cancelFileTransfer( VNumber );

signals:
  void chatMessage( VNumber chat_id, const ChatMessage& );
  void fileDownloadRequest( const User&, const FileInfo& );
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

protected slots:
  /* CoreConnection */
  void setNewConnection( Connection* );
  void newPeerFound( const QHostAddress&, int );
  void setConnectionError( QAbstractSocket::SocketError );
  void setConnectionClosed();
  void checkUserAuthentication( const Message& );

  /* CoreParser */
  void parseMessage( VNumber, const Message& );
  void parseMessage( const User&, const Message& );

  /* CoreFileTransfer */
  void checkFileTransferProgress( VNumber, VNumber, const FileInfo&, FileSizeType );
  void checkFileTransferMessage( VNumber, VNumber, const FileInfo&, const QString& );
  void validateUserForFileTransfer( VNumber, const QHostAddress&, const Message& );
  void fileTransferServerListening();
  void addListToLocalShare();

  /* CoreChat */
  void addListToSavedChats();

protected:
  void createLocalShareMessage();

  /* CoreConnection */
  Connection* connection( VNumber );
  bool hasConnection( const QHostAddress&, int ) const;
  void closeConnection( Connection* );
  void addConnectionReadyForUse( Connection* );

  /* CoreParser */
  void parseUserMessage( const User&, const Message& );
  void parseChatMessage( const User&, const Message& );
  void parseFileMessage( const User&, const Message& );
  void parseFileShareMessage( const User&, const Message& );
  void parseGroupMessage( const User&, const Message& );

  /* CoreUser */
  void showUserStatusChanged( const User& );
  void showUserNameChanged( const User&, const QString& );
  void showUserVCardChanged( const User& );
  void sendLocalUserStatus();
  void addGroup( const Group& );

  /* CoreChat */
  void createDefaultChat();
  void createPrivateChat( const User& );
  QString chatMessageToText( const UserList&, const ChatMessage& );
  bool sendMessageToLocalNetwork( const User& to_user, const Message& );
  void sendGroupChatRequestMessage( const Chat&, const UserList& );
  void sendGroupChatRefuseMessage( const Chat&, const UserList& );
  void checkGroupChatAfterUserReconnect( const User& );

  /* CoreDispatcher */
  enum DispatchType { DispatchToAll, DispatchToAllChatsWithUser, DispatchToChat };
  void dispatchSystemMessage( VNumber chat_id, VNumber from_user_id, const QString& msg, DispatchType );
  void dispatchChatMessageReceived( VNumber from_user_id, const Message& m );
  void dispatchToAllChats( const ChatMessage& );
  void dispatchToAllChatsWithUser( const ChatMessage&, VNumber user_id );
  void dispatchToChat( const ChatMessage&, VNumber chat_id );

  /* CoreFileTransfer */
  void sendFileShareListTo( VNumber user_id );
  void sendFileShareListToAll();

private:
  QList<Connection*> m_connections;
  Listener* mp_listener;
  Broadcaster* mp_broadcaster;
  FileTransfer* mp_fileTransfer;
  int m_shareListToBuild;

};

// Inline Functions
inline int Core::connectedUsers() const { return m_connections.size(); }

#endif // BEEBEEP_CLIENT_H
