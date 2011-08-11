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

#ifndef BEEBEEP_CONNECTIONSOCKET_H
#define BEEBEEP_CONNECTIONSOCKET_H

#include "Config.h"
#include "User.h"


class ConnectionSocket : public QTcpSocket
{
  Q_OBJECT

public:
  explicit ConnectionSocket( QObject* parent = 0 );

  bool sendData( const QByteArray& );
  inline const User& user() const;

signals:
  void dataReceived( const QByteArray& );
  void userAuthenticated();

protected slots:
  void readBlock();
  void sendHello();

protected:
  void checkHello( const QByteArray& );
  User m_user;

private:
  DATA_BLOCK_SIZE m_blockSize;
  bool m_isHelloSent;

};


// Inline Functions
inline const User& ConnectionSocket::user() const { return m_user; }

#endif // BEEBEEP_CONNECTIONSOCKET_H
