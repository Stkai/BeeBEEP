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

#ifndef BEEBEEP_XMPPMANAGER_H
#define BEEBEEP_XMPPMANAGER_H

#include "Config.h"
#include "Message.h"
#include "QXmppClient.h"
#include "User.h"
class QXmppVCardIq;


class XmppManager : public QObject
{
  Q_OBJECT

public:
  explicit XmppManager( QObject* );

  void connectToServer( const QString& jid, const QString& passwd );
  void disconnectFromServer();
  inline bool isConnected() const;

  inline QString service() const;
  inline QString iconPath() const;

  void sendMessage( const QString&, const Message& );
  void subscribeUser( const QString&, bool );
  void removeUser( const QString& );
  void checkPresence( const QString& );
  void requestVCard( const QString& );
  void sendLocalUserPresence();

signals:
  void message( const QString&, const Message& );
  void userChangedInRoster( const User& );
  void userSubscriptionRequest( const QString& );
  void vCardReceived( const QString&, const VCard& );
  void vCardAvailable( const QString& );

protected slots:
  void serverConnected();
  void serverDisconnected();
  void rosterReceived();
  void rosterChanged( const QString& );
  void errorOccurred( QXmppClient::Error );
  void presenceChanged( const QString&, const QString& );
  void messageReceived( const QXmppMessage& );
  void presenceReceived( const QXmppPresence& );
  void vCardReceived( const QXmppVCardIq& );

protected:
  void makeSystemMessage( const QString& );
  User::Status statusFromPresence( QXmppPresence::Status::Type );
  void parseChatMessage( const QString&, const QXmppMessage& );
  void parseErrorMessage( const QString&, const QXmppMessage& );
  QString errorConditionToString( int ) const;

private:
  void dumpMessage( const QXmppMessage& );

private:
  QXmppClient* mp_client;
  QString m_service;

};


// Inline Functions
inline bool XmppManager::isConnected() const { return mp_client->isConnected(); }
inline QString XmppManager::service() const { return m_service.isEmpty() ? "Jabber" : m_service; }
inline QString XmppManager::iconPath() const { return m_service == "GTalk" ? ":/images/gtalk.png" : ":/images/jabber.png"; }


#endif // BEEBEEP_XMPPMANAGER_H
