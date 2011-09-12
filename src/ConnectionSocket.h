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
// $Id: ConnectionSocket.h 80 2011-09-03 01:45:25Z mastroddi $
//
//////////////////////////////////////////////////////////////////////

#ifndef BEEBEEP_CONNECTIONSOCKET_H
#define BEEBEEP_CONNECTIONSOCKET_H

#include "Config.h"
class Message;


class ConnectionSocket : public QTcpSocket
{
  Q_OBJECT

public:
  explicit ConnectionSocket( QObject* parent = 0 );

  bool sendData( const QByteArray& );

  inline VNumber userId() const;
  inline void setUserId( VNumber );

  inline int protoVersion() const;

signals:
  void dataReceived( const QByteArray& );
  void authenticationRequested( const Message& );

protected slots:
  void readBlock();
  void sendHello();

protected:
  void checkHelloMessage( const QByteArray& );

private:
  DATA_BLOCK_SIZE m_blockSize;
  bool m_isHelloSent;
  VNumber m_userId;
  int m_protoVersion;

  int m_preventLoop;

};


// Inline Functions
inline VNumber ConnectionSocket::userId() const { return m_userId; }
inline void ConnectionSocket::setUserId( VNumber new_value ) { m_userId = new_value; }
inline int ConnectionSocket::protoVersion() const { return m_protoVersion; }

#endif // BEEBEEP_CONNECTIONSOCKET_H
