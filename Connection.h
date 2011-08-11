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

#include "ConnectionSocket.h"
#include "User.h"
class Message;
class FileInfo;


class Connection : public ConnectionSocket
{
  Q_OBJECT

public:
  explicit Connection( QObject *parent = 0 );

  inline const User& user() const;
  inline VNumber id() const;

  bool sendMessage( const Message& );

public slots:
  bool sendLocalUserStatus();

signals:
  void readyForUse();
  void newMessage( const User&, const Message& );
  void newStatus( const User& );
  void isWriting( const User& );
  void newFileMessage( const User&, const FileInfo& );

private slots:
  void parseData( const QByteArray& );
  void sendPing();
  void sendPong();
  void setReadyForUse();

private:
  void parseUserMessage( const Message& );
  void parseFileMessage( const Message& );

  QTimer m_pingTimer;
  QTime m_pongTime;

};


// Inline Functions

inline VNumber Connection::id() const { return m_user.id(); }
inline const User& Connection::user() const { return m_user; }


#endif // BEEBEEP_CONNECTION_H
