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

#include "Config.h"
#include "Chat.h"
#include "Connection.h"
#include "User.h"
class Listener;
class PeerManager;
class FileTransfer;


class Core : public QObject
{
  Q_OBJECT

public:
  explicit Core( QObject* parent = 0 );

  /* CoreChat */
  Chat defaultChat();
  Chat chat( VNumber, bool read_all_messages );

signals:
  void chatMessage( VNumber chat_id, const ChatMessage& );
  void fileDownloadRequest( const User&, const FileInfo& );
  void userIsWriting( const User& );
  void userChanged( const User& );
  void transferProgress( const User&, const FileInfo&, FileSizeType );

public slots:
  /* CoreConnection */
  void setNewConnection( Connection* );

protected slots:
  /* Core */
  void newPeerFound( const QHostAddress&, int );

  /* CoreConnection */
  void setConnectionError( QAbstractSocket::SocketError );
  void setConnectionClosed();
  void checkUserAuthentication( const Message& );

  /* CoreParser */
  void parseMessage( VNumber, const Message& );

protected:
  /* CoreConnection */
  Connection* connection( VNumber );
  bool hasConnection( const QHostAddress&, int ) const;
  void closeConnection( Connection* );
  void setConnectionReadyForUse( Connection*, const User& );

  /* CoreParser */
  void parseUserMessage( const User&, const Message& );
  void parseChatMessage( const User&, const Message& );
  void parseFileMessage( const User&, const Message& );

  /* CoreUser */
  void createLocalUser();
  void saveLocalUser();
  User user( const QString& user_name, const QString& user_nick, const QHostAddress&, int ) const;
  User user( VNumber ) const;
  void setUser( const User& );

  /* CoreChat */
  void createDefaultChat();
  void createPrivateChat( const User& );
  Chat privateChatForUser( VNumber ) const;
  Chat chat( VNumber ) const;
  void setChat( const Chat& );

  /* CoreDispatcher */
  void dispatchSystemMessage( VNumber from_user_id, const QString& msg );
  void dispatchChatMessageReceived( VNumber from_user_id, const Message& m );
  void dispatchToAllChats( const ChatMessage& );

private:
  User m_localUser;
  QList<User> m_users;
  QList<Chat> m_chats;
  QList<Connection*> m_connections;
  Listener* mp_listener;
  PeerManager* mp_peerManager;
  FileTransfer* mp_fileTransfer;

};


// Inline Functions
inline Chat Core::defaultChat( bool read_all_messages ) const { return chat( ID_DEFAULT_CHAT, read_all_messages ); }


#endif // BEEBEEP_CLIENT_H
