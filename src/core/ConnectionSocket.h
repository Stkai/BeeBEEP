//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2021 Marco Mastroddi
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
// along with BeeBEEP. If not, see <http://www.gnu.org/licenses/>.
//
// Author: Marco Mastroddi <marco.mastroddi(AT)gmail.com>
//
// $Id$
//
//////////////////////////////////////////////////////////////////////

#ifndef BEEBEEP_CONNECTIONSOCKET_H
#define BEEBEEP_CONNECTIONSOCKET_H

#include "ECDH.h"
#include "NetworkAddress.h"

class Message;


class ConnectionSocket : public QTcpSocket
{
  Q_OBJECT

public:
  explicit ConnectionSocket( QObject* parent = Q_NULLPTR );

  void connectToNetworkAddress( const NetworkAddress& );
  void initSocket( qintptr, quint16 server_port );

  bool sendData( const QByteArray& );

  void flushAll();
  virtual void closeConnection();
  virtual void abortConnection();

  inline VNumber userId() const;
  inline void setUserId( VNumber );

  inline int protocolVersion() const;
  int fileTransferBufferSize() const;

  inline bool isConnected() const;
  inline bool isConnecting() const;

  inline const QDateTime& latestActivityDateTime() const;
  int activityIdle() const; // ms idle
  inline const NetworkAddress& networkAddress() const;

  void onTickEvent( int );

  inline bool isTestConnection() const;
  inline void setTestConnection( bool );

  inline bool isServerSocket() const;
  inline bool isEncrypted() const;
  inline bool isCompressed() const;

signals:
  void dataReceived( const QByteArray& );
  void authenticationRequested( const QByteArray& );
  void abortRequest();
  void pingRequest();
  void connectionTestCompleted( const QString& );

protected slots:
  qint64 readBlock();
  void sendQuestionHello();
  void onBytesWritten( qint64 );

protected:
  void sendAnswerHello( bool encryption_enabled, bool compression_enabled );
  void checkHelloMessage( const QByteArray& );
  QByteArray serializeData( const QByteArray& );
  const QByteArray& cipherKey() const;
  bool createCipherKey( const QString& other_public_key );

  bool checkConnectionTimeout( int );
  bool checkTestMessage( const Message& );

  inline bool isKeysHandshakeCompleted() const;
  void useEncryption( bool );
  void useCompression( bool );

private:
  // max block size contains lowers
  DATA_BLOCK_SIZE_32 m_blockSize;
  bool m_isHelloSent;
  VNumber m_userId;
  int m_protocolVersion;
  QString m_publicKey1;
  QString m_publicKey2;
  ECDH::Keys m_ecdhKeys;
  QByteArray m_cipherKey;

  NetworkAddress m_networkAddress;

  QDateTime m_latestActivityDateTime;
  bool m_checkConnectionTimeout;
  int m_tickCounter;
  bool m_isAborted;

  int m_datastreamVersion;
  int m_pingByteArraySize;

  bool m_isTestConnection;

  quint16 m_serverPort;

  bool m_isEncrypted;
  bool m_isCompressed;

};


// Inline Functions
inline VNumber ConnectionSocket::userId() const { return m_userId; }
inline void ConnectionSocket::setUserId( VNumber new_value ) { m_userId = new_value; }
inline int ConnectionSocket::protocolVersion() const { return m_protocolVersion; }
inline bool ConnectionSocket::isConnected() const { return isOpen() && (state() == QAbstractSocket::ConnectedState || state() == QAbstractSocket::BoundState); }
inline bool ConnectionSocket::isConnecting() const { return isOpen() && (state() == QAbstractSocket::HostLookupState || state() == QAbstractSocket::ConnectingState); }
inline const QDateTime& ConnectionSocket::latestActivityDateTime() const { return m_latestActivityDateTime; }
inline const NetworkAddress& ConnectionSocket::networkAddress() const { return m_networkAddress; }
inline bool ConnectionSocket::isTestConnection() const { return m_isTestConnection; }
inline void ConnectionSocket::setTestConnection( bool new_value ) { m_isTestConnection = new_value; }
inline bool ConnectionSocket::isServerSocket() const { return m_serverPort > 0; }
inline bool ConnectionSocket::isKeysHandshakeCompleted() const { return !m_cipherKey.isEmpty(); }
inline bool ConnectionSocket::isEncrypted() const { return m_isEncrypted; }
inline bool ConnectionSocket::isCompressed() const { return m_isCompressed; }

#endif // BEEBEEP_CONNECTIONSOCKET_H
