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

#ifndef BEEBEEP_XMPPCLIENT_H
#define BEEBEEP_XMPPCLIENT_H

#ifdef USE_QXMPP

#include "Config.h"
#include "QXmppClient.h"
#include "QXmppTransferManager.h"
#include "User.h"
class FileInfo;
class QXmppVCardIq;


class XmppClient : public QXmppClient
{
  Q_OBJECT

public:
  enum ConnectionState { Offline, InProgress, Online };

  explicit XmppClient( QObject* );
  void setupManagers();

  inline const QString& service() const;
  inline void setService( const QString& );
  inline const QString& iconPath() const;
  inline void setIconPath( const QString& );
  inline bool isActive() const;
  inline void setConnectionState( XmppClient::ConnectionState );

  void sendFile( const QString& bare_jid, const FileInfo& );

  inline void setClientUser( const User& );
  inline const User& clientUser() const;
  inline bool isLocalUser( const QString& bare_jid ) const;

  void sendVCard( const QXmppVCardIq& );

signals:
  void rosterReceived();
  void rosterChanged( const QString& );
  void presenceChanged( const QString&, const QString& );
  void vCardReceived( const QXmppVCardIq& );
  void clientVCardReceived();

protected slots:
  void checkFileTransferRequest( QXmppTransferJob* );
  void checkFileTransferError( QXmppTransferJob::Error );
  void checkFileTransferFinished();
  void checkFileTransferProgress( qint64, qint64 );

protected:
  void setupFileTransferJob( QXmppTransferJob* );

private:
  User m_clientUser;
  QString m_iconPath;
  ConnectionState m_connectionState;

  QXmppTransferManager* mp_transferManager;

};


// Inline Functions
inline const QString& XmppClient::service() const { return m_clientUser.service(); }
inline void XmppClient::setService( const QString& new_value ) { m_clientUser.setService( new_value ); }
inline const QString& XmppClient::iconPath() const { return m_iconPath; }
inline void XmppClient::setIconPath( const QString& new_value ) { m_iconPath = new_value; }
inline bool XmppClient::isActive() const { return m_connectionState != XmppClient::Offline; }
inline void XmppClient::setConnectionState( XmppClient::ConnectionState new_value ) { m_connectionState = new_value; }
inline void XmppClient::setClientUser( const User& new_value ) { m_clientUser = new_value; }
inline const User& XmppClient::clientUser() const { return m_clientUser; }
inline bool XmppClient::isLocalUser( const QString& bare_jid ) const { return m_clientUser.bareJid() == bare_jid; }


#endif // USE_QXMPP

#endif // BEEBEEP_XMPPCLIENT_H
