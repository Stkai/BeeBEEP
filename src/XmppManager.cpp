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

#include "Protocol.h"
#include "Settings.h"
#include "XmppManager.h"
#include "QXmppClient.h"
#include "QXmppMessage.h"
#include "QXmppRosterManager.h"
#include "QXmppUtils.h"


XmppManager::XmppManager( QObject* parent )
  : QObject( parent )
{
  mp_client = new QXmppClient( this );

  connect( mp_client, SIGNAL( error( QXmppClient::Error ) ), this, SLOT( errorOccurred( QXmppClient::Error ) ) );
  connect( mp_client, SIGNAL( connected() ), this, SLOT( serverConnected() ) );
  connect( mp_client, SIGNAL( disconnected() ), this, SLOT( serverDisconnected() ) );
  connect( &(mp_client->rosterManager()), SIGNAL( rosterReceived() ), this, SLOT( rosterReceived() ) );
  connect( &(mp_client->rosterManager()), SIGNAL( rosterChanged( const QString& ) ), this, SLOT( rosterChanged( const QString& ) ) );
  connect( &(mp_client->rosterManager()), SIGNAL( presenceChanged( const QString&, const QString& ) ), this, SLOT( presenceChanged( const QString&, const QString& ) ) );
  connect( mp_client, SIGNAL( messageReceived( const QXmppMessage& ) ), this, SLOT( messageReceived( const QXmppMessage& ) ) );
}

void XmppManager::connectToServer()
{
  QNetworkProxy proxy = Settings::instance().networkProxy();

  if( proxy.type() == QNetworkProxy::DefaultProxy )
  {
    QList<QNetworkProxy> proxy_list = QNetworkProxyFactory::systemProxyForQuery();
    if( !proxy_list.isEmpty() )
      proxy = proxy_list.first();
    else
      proxy = QNetworkProxy( QNetworkProxy::NoProxy );
  }

  if( proxy.type() != QNetworkProxy::NoProxy && proxy.type() != QNetworkProxy::DefaultProxy )
    mp_client->configuration().setNetworkProxy( proxy );

  mp_client->configuration().setJid( "beebeep.test@gmail.com" );
  mp_client->configuration().setPassword( "TesT12345" );
  mp_client->configuration().setResource( "beebeep" );

  mp_client->connectToServer( mp_client->configuration() );
  qDebug() << "XMPP> Connecting to" << mp_client->configuration().host();

}

void XmppManager::disconnectFromServer()
{
  qDebug() << "XMPP> Disconnecting from" << mp_client->configuration().host();
  mp_client->disconnectFromServer();
}

void XmppManager::serverConnected()
{
  qDebug() << "XMPP>" << mp_client->configuration().domain() << "server is connected";
}

void XmppManager::serverDisconnected()
{
  qDebug() << "XMPP>" << mp_client->configuration().domain() << "server is disconnected";
}

void XmppManager::errorOccurred( QXmppClient::Error err )
{
  QString s_error;
  switch( err )
  {
  case QXmppClient::SocketError:
    s_error = tr( "socket error" );
    break;
  case QXmppClient::KeepAliveError:
    s_error = tr( "keep alive error" );
    break;
  case QXmppClient::XmppStreamError:
    s_error = tr( "xmpp stream error" );
    break;
  default:
    s_error = tr( "unknown error" );
  }

  systemMessage( tr( "%1: %2 occurred." ).arg( mp_client->configuration().domain(), s_error ) );
}

void XmppManager::rosterReceived()
{
  qDebug() << "XMPP>" << mp_client->configuration().domain() << "has sent roster";
  QStringList bare_jid_list = mp_client->rosterManager().getRosterBareJids();
  foreach( QString bare_jid, bare_jid_list )
    rosterChanged( bare_jid );
}

void XmppManager::rosterChanged( const QString& bare_jid )
{
  qDebug() << "XMPP> user" << bare_jid << "changed in roster";
  User u;
  u.setPath( bare_jid );
  u.setName( mp_client->rosterManager().getRosterEntry( bare_jid ).name() );
  if( u.name().isEmpty() )
    u.setName( bare_jid );
  u.setHostAddress( QHostAddress( mp_client->configuration().host() ) );
  u.setHostPort( mp_client->configuration().port() );
  emit userChangedInRoster( u );
}

void XmppManager::presenceChanged( const QString& bare_jid, const QString& jid_resource )
{
  qDebug() << "XMPP> presence changed for" << bare_jid;
  QXmppPresence presence = mp_client->rosterManager().getPresence( bare_jid, jid_resource );
  User::Status status = statusFromPresence( presence.status().type() );
  QString status_desc = presence.status().statusText();
  Message m = Protocol::instance().userStatusMessage( status, status_desc );
  emit message( bare_jid, m );
}

void XmppManager::messageReceived( const QXmppMessage& xmpp_msg )
{
  QString user_path = jidToBareJid( xmpp_msg.from() );
  qDebug() << "XMPP> message received from" << user_path;
  dumpMessage( xmpp_msg );

  switch( xmpp_msg.type() )
  {
  //case QXmppMessage::Normal:
  //case QXmppMessage::GroupChat:
  //case QXmppMessage::Headline:
  case QXmppMessage::Chat:
    parseChatMessage( user_path, xmpp_msg );
    break;
  case QXmppMessage::Error:
    parseErrorMessage( user_path, xmpp_msg );
    break;
  default:
    qWarning() << "XMPP> cannot handle message type:" << xmpp_msg.type();
  }
}

void XmppManager::systemMessage( const QString& txt )
{
  qDebug() << mp_client->configuration().domain() << "send system message:" << txt;
  Message m = Protocol::instance().systemMessage( txt );
  emit message( Settings::instance().localUser().path(), m );
}

User::Status XmppManager::statusFromPresence( QXmppPresence::Status::Type xmpp_presence_status_type )
{
  switch( xmpp_presence_status_type )
  {
  case QXmppPresence::Status::Offline:
  case QXmppPresence::Status::Invisible:
    return User::Offline;
  case QXmppPresence::Status::Online:
  case QXmppPresence::Status::Chat:
    return User::Online;
  case QXmppPresence::Status::Away:
  case QXmppPresence::Status::XA:
    return User::Away;
  case QXmppPresence::Status::DND:
    return User::Busy;
  default:
    qWarning() << "XMPP> unable to convert presence status" << xmpp_presence_status_type << "to user status";
    return User::Offline;
  }
}

void XmppManager::sendMessage( const QString& user_path, const Message& m )
{
  if( m.type() == Message::Chat )
  {
    mp_client->sendMessage( user_path, m.text() );
  }
}

void XmppManager::parseChatMessage( const QString& user_path, const QXmppMessage& xmpp_msg )
{
  if( xmpp_msg.body().isEmpty() )
    return;
  Message m = Protocol::instance().chatMessage( xmpp_msg.body() );
  m.addFlag( Message::Private );
  m.setTimestamp( xmpp_msg.stamp() );
  m.setData( QColor( 0, 0, 0 ).name() );
  qDebug() << "XMPP> chat message from" << user_path << "with body:" << m.text();
  emit message( user_path, m );
}

void XmppManager::parseErrorMessage( const QString& user_path, const QXmppMessage& xmpp_msg )
{
  qDebug() << "XMPP> error" << xmpp_msg.error().condition() << "in message from" << user_path;
  Message m = Protocol::instance().systemMessage( tr( "Error occurred (#%1)" ).arg( xmpp_msg.error().condition() ) );
  emit message( user_path, m );
}

void XmppManager::dumpMessage( const QXmppMessage& xmpp_msg )
{
  qDebug() << "XMPP> Message Begin";
  qDebug() << "XMPP> Body:" << xmpp_msg.body();
  qDebug() << "XMPP> Stamp:" << xmpp_msg.stamp().toString( Qt::ISODate );
  qDebug() << "XMPP> State:" << (int)xmpp_msg.state();
  qDebug() << "XMPP> Subject:" << xmpp_msg.subject();
  qDebug() << "XMPP> Thread:" << xmpp_msg.thread();
  qDebug() << "XMPP> Type:" << (int)xmpp_msg.type();
  qDebug() << "XMPP> To:" << xmpp_msg.to();
  qDebug() << "XMPP> From:" << xmpp_msg.from();
  qDebug() << "XMPP> Id:" << xmpp_msg.id();
  qDebug() << "XMPP> Lang:" << xmpp_msg.lang();
  qDebug() << "XMPP> Error code:" << xmpp_msg.error().code();
  qDebug() << "XMPP> Error text:" << xmpp_msg.error().text();
  qDebug() << "XMPP> Error condition:" << (int)xmpp_msg.error().condition();
  qDebug() << "XMPP> Error type:" << (int)xmpp_msg.error().type();
  qDebug() << "XMPP> Message End";
}
