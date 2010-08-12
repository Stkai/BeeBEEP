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

#ifndef BEEBEEP_CLIENT_H
#define BEEBEEP_CLIENT_H

#include "Config.h"
#include "Chat.h"
#include "User.h"
class ChatMessage;
class Connection;
class Listener;
class Message;
class PeerManager;


class BeeBeep : public QObject
{
  Q_OBJECT

public:
  BeeBeep( QObject* parent = 0 );
  QList<User> users() const;
  User user( int ) const;
  Chat chat( const QString&, bool create_if_need, bool read_all_message );
  bool isWorking() const;
  QString id() const;
  void searchUsers( const QHostAddress& );

signals:
  void newMessage( const QString& chat_name, const ChatMessage& );
  void newUser( const User& );
  void removeUser( const User& );
  void newChat( const Chat& );

public slots:
  void start();
  void stop();
  void sendMessage( const QString&, const QString& );

private slots:
  void newConnection( Connection* );
  void newPeerFound( const QHostAddress&, int );
  void connectionError( QAbstractSocket::SocketError );
  void disconnected();
  void readyForUse();
  void dispatchMessage( const User&, const Message& );
  void dispatchSystemMessage( const QString& chat_name, const QString& sysmess );

protected:
  bool hasConnection( const QHostAddress& sender_ip, int sender_port = -1 ) const;
  void removeConnection( Connection* );
  Connection* connection( const QString& chat_name );

private:
  Listener* mp_listener;
  PeerManager* mp_peerManager;
  QMultiHash<int, Connection*> m_peers;
  QHash<QString, Chat> m_chats;

};

#endif // BEEBEEP_CLIENT_H
