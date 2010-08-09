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
// $Id: Connection.h 34 2010-07-13 18:45:23Z mastroddi $
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

signals:
  void readyForUse();
  void newMessage( const User&, const Message& );
  void newFileMessage( const User&, const Message& );

private slots:
  void readData();
  void sendPing();
  void sendPong();
  void sendHello();

private:
  void processData();
  bool writeMessageData( const QString& );

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
