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
#include "XmppClient.h"
#include "User.h"
class QXmppVCardIq;


class XmppManager : public QObject
{
  Q_OBJECT

public:
  explicit XmppManager( QObject* );

  XmppClient* client( const QString& ) const;
  inline const QList<XmppClient*>& clientList() const;

  void loadClients();
  XmppClient* createClient( const QString& client_service, const QString& client_icon_path );

  bool connectToServer( const QString& service, const QString& user_name, const QString& passwd );
  void disconnectFromServer();
  void disconnectFromServer( const QString& service );

  void sendMessage( const User&, const Message& );
  void subscribeUser( const QString& service, const QString& bare_jid, bool );
  void removeUser( const User& );
  void requestVCard( const QString& service, const QString& bare_jid );
  void sendLocalUserPresence();

signals:
  void message( const QString& service, const QString& bare_jid, const Message& );
  void userChangedInRoster( const User& );
  void userSubscriptionRequest( const QString& service, const QString& bare_jid );
  void vCardReceived( const QString& service, const QString& bare_jid, const VCard& );
  void vCardAvailable( const QString& service, const QString& bare_jid );

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
  void makeSystemMessage( XmppClient*, const QString& );
  User::Status statusFromPresence( QXmppPresence::Status::Type );
  void parseChatMessage( const QString& service, const QString& bare_jid, const QXmppMessage& );
  void parseErrorMessage( const QString& service, const QString& bare_jid, const QXmppMessage& );
  void checkUserChanged( XmppClient*, const QString& );
  QString errorConditionToString( int ) const;

  void dumpMessage( const QXmppMessage& );

private:
  QList<XmppClient*> m_clients;

};


// Inline Functions
inline const QList<XmppClient*>& XmppManager::clientList() const { return m_clients; }


//inline QString XmppManager::service() const { return m_service.isEmpty() ? "Jabber" : m_service; }
//inline QString XmppManager::iconPath() const { return m_service == "GTalk" ? ":/images/gtalk.png" : ":/images/jabber.png"; }


#endif // BEEBEEP_XMPPMANAGER_H
