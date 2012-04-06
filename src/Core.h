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

#ifndef BEEBEEP_CORE_H
#define BEEBEEP_CORE_H

#include "Chat.h"
#include "Listener.h"
#include "FileTransfer.h"
class Broadcaster;
class NetworkAccount;
class UserList;
class XmppManager;


class Core : public QObject
{
  Q_OBJECT

public:
  explicit Core( QObject* parent = 0 );

  void buildLocalShare();
  bool isConnected( bool check_also_network_service ) const;
  bool start();
  void stop();

  void addBroadcastAddress( const QHostAddress& );
  void sendBroadcastMessage();

  /* CoreUser */
  void setLocalUserStatus( int );
  void setLocalUserStatusDescription( const QString& );
  bool setUserColor( VNumber, const QString& );
  void setLocalUserVCard( const VCard& );

  /* CoreChat */
  int sendChatMessage( VNumber chat_id, const QString& ); // return the number of message sent (one for every user in chat)
  void showTipOfTheDay();

  /* CoreFileTransfer */
  bool sendFile( const User&, const QString& file_path );
  void downloadFile( const User&, const FileInfo& );
  void refuseToDownloadFile( const User&, const FileInfo& );
  bool startFileTransferServer();
  void stopFileTransferServer();

  /* CoreXmpp */
  void setXmppUserSubscription( const QString& service, const QString& user_path, bool accepted );
  bool removeXmppUser( const User& );
  bool connectToXmppServer( const NetworkAccount& );
  void disconnectFromXmppServer( const QString& service = "" ); // Empty service = All services
  bool isXmppServerConnected( const QString& ) const;

public slots:
  /* CoreChat */
  void sendWritingMessage( VNumber );

  /* CoreFileTransfer */
  void cancelFileTransfer( VNumber );

signals:
  void chatMessage( VNumber chat_id, const ChatMessage& );
  void fileDownloadRequest( const User&, const FileInfo& );
  void userIsWriting( const User& );
  void userChanged( const User& );
  void fileTransferProgress( VNumber, const User&, const FileInfo&, FileSizeType );
  void fileTransferMessage( VNumber, const User&, const FileInfo&, const QString& );
  void xmppUserSubscriptionRequest( const QString&, const QString& );
  void serviceConnected( const QString& );
  void serviceDisconnected( const QString& );
  void fileShareAvailable( const User& );

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

  /* CoreXmpp */
  void parseXmppMessage( const QString& service, const QString& bare_jid, const Message& );
  void checkXmppUser( const User& );
  void sendXmppChatMessage( const User&, const Message& );
  void checkXmppUserVCard( const QString& service, const QString& bare_jid );
  void setXmppVCard( const QString& service, const QString& bare_jid, const VCard& );
  void sendXmppUserComposing( const User& );

protected:
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

  /* CoreUser */
  void showUserStatusChanged( const User& );
  void showUserNameChanged( const User&, const QString& );
  void showUserVCardChanged( const User& );
  void sendLocalUserStatus();

  /* CoreChat */
  void createDefaultChat();
  void createPrivateChat( const User& );
  QString chatMessageToText( const UserList&, const ChatMessage& );
  bool chatHasService( const Chat&, const QString& );

  /* CoreDispatcher */
  enum DispatchType { DispatchToAll, DispatchToAllChatsWithUser, DispatchToChat, DispatchToService };
  void dispatchSystemMessage( const QString& service_name, VNumber chat_id, VNumber from_user_id, const QString& msg, DispatchType );
  void dispatchChatMessageReceived( VNumber from_user_id, const Message& m );
  void dispatchToAllChats( const ChatMessage& );
  void dispatchToAllChatsWithUser( const ChatMessage&, VNumber user_id );
  void dispatchToChat( const ChatMessage&, VNumber chat_id );
  void dispatchToService( const ChatMessage&, const QString& service_name );

  /* CoreXmpp */
  void sendLocalUserStatusToXmppServer();
  void sendLocalVCardToXmppServer();

private:
  QList<Connection*> m_connections;
  Listener* mp_listener;
  Broadcaster* mp_broadcaster;
  FileTransfer* mp_fileTransfer;
  XmppManager* mp_xmppManager;

};


#endif // BEEBEEP_CLIENT_H
