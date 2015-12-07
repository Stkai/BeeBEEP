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

#ifndef BEEBEEP_CONNECTIONSOCKET_H
#define BEEBEEP_CONNECTIONSOCKET_H

#include "Config.h"
class Message;


class ConnectionSocket : public QTcpSocket
{
  Q_OBJECT

public:
  explicit ConnectionSocket( QObject* parent = 0 );

  void connectToNetworkAddress( const QHostAddress&, int );
  void initSocket( qintptr );

  void flushAll();
  void closeConnection();
  void abortConnection();

  bool sendData( const QByteArray& );

  inline VNumber userId() const;
  inline void setUserId( VNumber );

  inline int protoVersion() const;
  int fileTransferBufferSize() const;

  inline bool isConnected() const;
  inline bool isConnecting() const;

  inline const QDateTime& latestActivityDateTime() const;
  int activityIdle() const;

  inline const QString& hostAndPort() const;

signals:
  void dataReceived( const QByteArray& );
  void authenticationRequested( const Message& );
  void abortRequest();
  void tickEvent( int );

protected slots:
  void readBlock();
  void sendQuestionHello();
  void checkConnectionTimeout( int );
  void timerEvent( QTimerEvent* );

protected:
  bool startTimerTick();
  void stopTimerTick();
  void sendAnswerHello();
  void checkHelloMessage( const QByteArray& );
  QByteArray serializeData( const QByteArray& );
  const QByteArray& cipherKey() const;
  bool createCipherKey( const QString& );

private:
  // max block size contains lowers
  DATA_BLOCK_SIZE_32 m_blockSize;
  bool m_isHelloSent;
  VNumber m_userId;
  int m_protoVersion;
  QByteArray m_cipherKey;
  QString m_publicKey1;
  QString m_publicKey2;

  QString m_hostAndPort;
  QDateTime m_latestActivityDateTime;
  int m_timerTickId;
  int m_tickCounter;
  bool m_isAborted;

};


// Inline Functions
inline VNumber ConnectionSocket::userId() const { return m_userId; }
inline void ConnectionSocket::setUserId( VNumber new_value ) { m_userId = new_value; }
inline int ConnectionSocket::protoVersion() const { return m_protoVersion; }
inline bool ConnectionSocket::isConnected() const { return isOpen() && state() >= QAbstractSocket::HostLookupState && state() <= QAbstractSocket::ConnectedState; }
inline bool ConnectionSocket::isConnecting() const { return state() == QAbstractSocket::HostLookupState || state() == QAbstractSocket::ConnectingState; }
inline const QDateTime& ConnectionSocket::latestActivityDateTime() const { return m_latestActivityDateTime; }
  inline const QString& ConnectionSocket::hostAndPort() const { return m_hostAndPort; }

#endif // BEEBEEP_CONNECTIONSOCKET_H
