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

#ifndef BEEBEEP_CONNECTION_H
#define BEEBEEP_CONNECTION_H

#include "User.h"
class Message;


class Connection : public QTcpSocket
{
  Q_OBJECT

public:
  enum State { WaitingForHello, ReadyForUse };

  Connection( QObject *parent = 0 );

  inline const User& user() const;
  inline int id() const;

  bool sendMessage( const Message& );

public slots:
  bool sendLocalUserStatus();

signals:
  void readyForUse();
  void newMessage( const User&, const Message& );
  void newStatus( const User& );
  void isWriting( const User& );

private slots:
  void readData();
  void sendPing();
  void sendPong();
  void sendHello();

private:
  void processData();
  bool writeMessageData( const QString& );
  void parseMessage( const Message& );
  void parseHelloMessage( const Message& );
  void parseUserMessage( const Message& );

  User m_user;
  QTimer m_pingTimer;
  QTime m_pongTime;
  State m_state;
  bool m_isHelloMessageSent;

};


// Inline Functions

inline int Connection::id() const { return m_user.id(); }
inline const User& Connection::user() const { return m_user; }


#endif // BEEBEEP_CONNECTION_H
