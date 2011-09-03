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
#include "UserList.h"
class Broadcaster;


class Core : public QObject
{
  Q_OBJECT

public:
  explicit Core( QObject* parent = 0 );

  inline bool isConnected() const;
  bool start();
  void stop();

  inline const UserList& users() const;

  /* CoreUser */
  void searchUsers( const QHostAddress& );
  void setLocalUserStatus( int );
  void setLocalUserStatusDescription( const QString& );
  bool setUserColor( VNumber, const QString& );
  void setLocalUserVCard( const VCard& );

  /* CoreChat */
  int sendChatMessage( VNumber chat_id, const QString& ); // return the number of message sent (one for every user in chat)
  inline Chat defaultChat( bool read_all_messages  );
  Chat chat( VNumber, bool read_all_messages );
  Chat privateChatForUser( VNumber ) const;
  void showTipOfTheDay();
  QString chatMessageToText( const ChatMessage& );
  QString chatMessagesToText( const Chat& );
  QString chatUsers( const Chat&, const QString& user_separator );

  /* CoreFileTransfer */
  bool sendFile( const User&, const QString& file_path );
  void downloadFile( const User&, const FileInfo& );
  void refuseToDownloadFile( const User&, const FileInfo& );

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

protected slots:
  /* CoreConnection */
  void setNewConnection( Connection* );
  void newPeerFound( const QHostAddress&, int );
  void setConnectionError( QAbstractSocket::SocketError );
  void setConnectionClosed();
  void checkUserAuthentication( const Message& );

  /* CoreParser */
  void parseMessage( VNumber, const Message& );

  /* CoreFileTransfer */
  void checkFileTransferProgress( VNumber, VNumber, const FileInfo&, FileSizeType );
  void checkFileTransferMessage( VNumber, VNumber, const FileInfo&, const QString& );
  void validateUserForFileTransfer( VNumber, const QHostAddress&, const Message& );

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

  /* CoreUser */
  void showUserStatusChanged( const User& );
  void showUserNameChanged( const User&, const QString& );
  void showUserVCardChanged( const User& );
  void sendLocalUserStatus();

  /* CoreChat */
  void createDefaultChat();
  void createPrivateChat( const User& );
  Chat chat( VNumber ) const;
  void setChat( const Chat& );
  QString chatMessageToText( const UserList&, const ChatMessage& );

  /* CoreDispatcher */
  enum DispatchType { DispatchToAll, DispatchToAllChatsWithUser, DispatchToChat };
  void dispatchSystemMessage( VNumber chat_id, VNumber from_user_id, const QString& msg, DispatchType );
  void dispatchChatMessageReceived( VNumber from_user_id, const Message& m );
  void dispatchToAllChats( const ChatMessage& );
  void dispatchToAllChatsWithUser( const ChatMessage&, VNumber user_id );
  void dispatchToChat( const ChatMessage&, VNumber chat_id );

private:
  UserList m_users;
  QList<Chat> m_chats;
  QList<Connection*> m_connections;
  Listener* mp_listener;
  Broadcaster* mp_broadcaster;
  FileTransfer* mp_fileTransfer;

};


// Inline Functions
inline Chat Core::defaultChat( bool read_all_messages ) { return chat( ID_DEFAULT_CHAT, read_all_messages ); }
inline bool Core::isConnected() const { return mp_listener->isListening(); }
inline const UserList& Core::users() const { return m_users; }

#endif // BEEBEEP_CLIENT_H
