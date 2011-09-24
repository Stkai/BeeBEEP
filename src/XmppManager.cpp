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
#include "QXmppPresence.h"
#include "QXmppReconnectionManager.h"
#include "QXmppRosterManager.h"
#include "QXmppUtils.h"
#include "QXmppVCardIq.h"
#include "QXmppVCardManager.h"


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
  connect( &(mp_client->vCardManager()), SIGNAL( vCardReceived( const QXmppVCardIq& ) ), this, SLOT( vCardReceived( const QXmppVCardIq& ) ) );
  connect( mp_client, SIGNAL( messageReceived( const QXmppMessage& ) ), this, SLOT( messageReceived( const QXmppMessage& ) ) );
  connect( mp_client, SIGNAL( presenceReceived( const QXmppPresence& ) ), this, SLOT( presenceReceived( const QXmppPresence& ) ) );
}

void XmppManager::connectToServer( const QString& jid, const QString& passwd )
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

  mp_client->configuration().setJid( jid );
  mp_client->configuration().setPassword( passwd );
  mp_client->configuration().setResource( QString( "BeeBeep" ) );

  if( jid.contains( "@gmail", Qt::CaseInsensitive ) )
    m_service = "GTalk";
  else
    m_service = "Jabber";

  makeSystemMessage( tr( "connection in progress.." ) );
  mp_client->connectToServer( mp_client->configuration() );

  qDebug() << "XMPP> Connecting to" << service();
}

void XmppManager::disconnectFromServer()
{
  qDebug() << "XMPP> Disconnecting from" << service();
  mp_client->disconnectFromServer();
}

void XmppManager::serverConnected()
{
  makeSystemMessage( "connected to the server" );
}

void XmppManager::serverDisconnected()
{
  makeSystemMessage( "disconnected from the server" );
  mp_client->reconnectionManager()->cancelReconnection();
}

void XmppManager::errorOccurred( QXmppClient::Error err )
{
  QString s_error;
  switch( err )
  {
  case QXmppClient::SocketError:
    s_error = tr( "socket" );
    break;
  case QXmppClient::KeepAliveError:
    s_error = tr( "keep alive" );
    break;
  case QXmppClient::XmppStreamError:
  {
    switch( mp_client->xmppStreamError() )
    {
    case QXmppStanza::Error::NotAuthorized:
      s_error = tr( "invalid user or password" );
      break;
    default:
      s_error = tr( "xmpp stream" );
    }
  }
    break;
  default:
    s_error = tr( "unknown" );
  }

  makeSystemMessage( tr( "connection error (%1)" ).arg( s_error ) );
}

void XmppManager::rosterReceived()
{
  qDebug() << "XMPP>" << service() << "has sent roster";
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
  if( bare_jid == Settings::instance().localUser().path() )
  {
    qDebug() << "XMPP> skip local user presence";
    return;
  }

  qDebug() << "XMPP> presence changed for" << bare_jid;
  QXmppPresence presence = mp_client->rosterManager().getPresence( bare_jid, jid_resource );
  User::Status status = statusFromPresence( presence.status().type() );
  QString status_desc = presence.status().statusText();
  Message m = Protocol::instance().userStatusMessage( status, status_desc );
  emit message( bare_jid, m );

  if( presence.type() == QXmppPresence::Available )
  {
    switch( presence.vCardUpdateType() )
    {
    case QXmppPresence::VCardUpdateNotReady:
      break;
    case QXmppPresence::VCardUpdateNone:
    case QXmppPresence::VCardUpdateNoPhoto:
    case QXmppPresence::VCardUpdateValidPhoto:
      qDebug() << "XMPP> vCard is available for user" << bare_jid;
      emit vCardAvailable( bare_jid );
      break;
    default:
      qDebug() << "XMPP> vCardUpdateType unknown in presence message";
    }
  }
}

void XmppManager::requestVCard( const QString& bare_jid )
{
  qDebug() << "XMPP> requesting vCard for user" << bare_jid;
  mp_client->vCardManager().requestVCard( bare_jid );
}


void XmppManager::checkPresence( const QString& bare_jid )
{
  qDebug() << "XMPP> check presence for" << bare_jid;
  QList<QXmppPresence> presence_list;
}

void XmppManager::messageReceived( const QXmppMessage& xmpp_msg )
{
  QString user_path = jidToBareJid( xmpp_msg.from() );
  qDebug() << "XMPP> message received from" << user_path;
  dumpMessage( xmpp_msg );

  switch( xmpp_msg.type() )
  {
  //case QXmppMessage::GroupChat:
  //case QXmppMessage::Headline:
  case QXmppMessage::Normal:  // offline messages
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

void XmppManager::makeSystemMessage( const QString& txt )
{
  QString msg = QString( "%1: %2." ).arg( service(), txt );
  qDebug() << "XMPP>" << msg;
  Message m = Protocol::instance().systemMessage( msg );
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

QString XmppManager::errorConditionToString( int condition ) const
{
  switch( condition )
  {
  case QXmppStanza::Error::BadRequest:
    return tr( "bad request" );
  case QXmppStanza::Error::Conflict:
    return tr( "conflict" );
  case QXmppStanza::Error::FeatureNotImplemented:
    return tr( "feature not implemented" );
  case QXmppStanza::Error::Forbidden:
    return tr( "forbidden" );
  case QXmppStanza::Error::Gone:
    return tr( "gone" );
  case QXmppStanza::Error::InternalServerError:
    return tr( "internal server error" );
  case QXmppStanza::Error::ItemNotFound:
    return tr( "item not found" );
  case QXmppStanza::Error::JidMalformed:
    return tr( "jid malformed" );
  case QXmppStanza::Error::NotAcceptable:
    return tr( "not acceptable" );
  case QXmppStanza::Error::NotAllowed:
    return tr( "not allowed" );
  case QXmppStanza::Error::NotAuthorized:
    return tr( "not authorized" );
  case QXmppStanza::Error::PaymentRequired:
    return tr( "payment required" );
  case QXmppStanza::Error::RecipientUnavailable:
    return tr( "recipient unavailable" );
  case QXmppStanza::Error::Redirect:
    return tr( "redirect" );
  case QXmppStanza::Error::RegistrationRequired:
    return tr( "registration required" );
  case QXmppStanza::Error::RemoteServerNotFound:
    return tr( "remote server not found" );
  case QXmppStanza::Error::RemoteServerTimeout:
    return tr( "remote server timeout" );
  case QXmppStanza::Error::ResourceConstraint:
    return tr( "resource constraint" );
  case QXmppStanza::Error::ServiceUnavailable:
    return tr( "service unavailable" );
  case QXmppStanza::Error::SubscriptionRequired:
    return tr( "subscription required" );
  case QXmppStanza::Error::UndefinedCondition:
    return tr( "undefined condition" );
  case QXmppStanza::Error::UnexpectedRequest:
    return tr( "unexpected request" );
  default:
    return tr( "unknown" );
  }
}

void XmppManager::parseErrorMessage( const QString& user_path, const QXmppMessage& xmpp_msg )
{
  QString s_error = errorConditionToString( xmpp_msg.error().condition() );
  qDebug() << "XMPP> error" << s_error << "in message from" << user_path;
  Message m = Protocol::instance().systemMessage( tr( "%1: error occurred (%2)" ).arg( service(), s_error ) );
  emit message( user_path, m );
}

void XmppManager::subscribeUser( const QString& jid, bool accepted )
{
  if( accepted )
  {
    makeSystemMessage( tr( "adding %1 to the contact list").arg( jid ) );
    QXmppPresence subscribed;
    subscribed.setTo( jid );
    subscribed.setType( QXmppPresence::Subscribed );
    mp_client->sendPacket( subscribed );

    // reciprocal subscription
    QXmppPresence subscribe;
    subscribe.setTo( jid );
    subscribe.setType( QXmppPresence::Subscribe );
    mp_client->sendPacket( subscribe );
  }
  else
  {
    makeSystemMessage( tr( "%1's request is rejected").arg( jid ) );
    QXmppPresence unsubscribed;
    unsubscribed.setTo( jid );
    unsubscribed.setType( QXmppPresence::Unsubscribed );
    mp_client->sendPacket( unsubscribed );
  }
}

void XmppManager::removeUser( const QString& jid )
{
  makeSystemMessage( tr( "removing %1 from the contact list").arg( jid ) );
  QXmppRosterIq remove;
  remove.setType( QXmppIq::Set );
  QXmppRosterIq::Item item_remove;
  item_remove.setSubscriptionType( QXmppRosterIq::Item::Remove );
  item_remove.setBareJid( jid );
  remove.addItem( item_remove );
  mp_client->sendPacket( remove );
}

void XmppManager::sendLocalUserPresence()
{
  QXmppPresence presence = mp_client->clientPresence();

  const User& u = Settings::instance().localUser();

  if( u.isConnected() )
    presence.setType( QXmppPresence::Available );
  else
    presence.setType( QXmppPresence::Unavailable );

  switch( u.status() )
  {
  case User::Busy:
    presence.status().setType( QXmppPresence::Status::DND );
    break;
  case User::Away:
    presence.status().setType( QXmppPresence::Status::Away );
    break;
  default:
    presence.status().setType( QXmppPresence::Status::Online );
  }

  presence.status().setStatusText( u.statusDescription() );

  mp_client->setClientPresence( presence );

}

void XmppManager::presenceReceived( const QXmppPresence& presence )
{
  QString from = presence.from();
  QString msg = "";
  bool subscribe_request = false;
  bool subscription_accepted = false;

  switch( presence.type() )
  {
  case QXmppPresence::Subscribe:
    msg = tr( "%1 wants to subscribe to your contact list" );
    subscribe_request = true;
    break;
  case QXmppPresence::Subscribed:
    msg = tr( "%1 has accepted your subscription" );
    subscription_accepted = true;
    break;
  case QXmppPresence::Unsubscribe:
    ///< The sender is unsubscribing from another entity's presence.
    break;
  case QXmppPresence::Unsubscribed:
    msg = tr( "%1 has reject your subscription" );
    break;
  default:
   break;
 }

 if( msg.isEmpty() )
   return;

 makeSystemMessage( msg.arg( from ) );

 if( subscribe_request )
   emit userSubscriptionRequest( from );

 if( subscription_accepted )
   rosterChanged( from );
}

void XmppManager::vCardReceived( const QXmppVCardIq& vciq )
{
  QString bare_jid = vciq.from();
  qDebug() << "XMPP> vCard received from user" << bare_jid;
  qDebug() << "XMPP> vCard nickname" << vciq.nickName();
  qDebug() << "XMPP> vCard firstname" << vciq.firstName();
  qDebug() << "XMPP> vCard middlename" << vciq.middleName();
  qDebug() << "XMPP> vCard lastname" << vciq.lastName();
  qDebug() << "XMPP> vCard fullname" << vciq.fullName();
  qDebug() << "XMPP> vCard email" << vciq.email();
  qDebug() << "XMPP> vCard birthday" << vciq.birthday().toString( Qt::ISODate );

  VCard vc;
  vc.setNickName( vciq.nickName() );
  vc.setFirstName( vciq.firstName() );
  vc.setLastName( vciq.lastName() );
  vc.setFullName( vciq.fullName() );
  vc.setEmail( vciq.email() );
  vc.setBirthday( vciq.birthday() );
  if( vc.nickName().isEmpty() )
  {
    if( vc.hasFullName() )
      vc.setNickName( vc.fullName() );
    else
      vc.setNickName( bare_jid );
  }

  if( !vciq.photo().isEmpty() )
  {
    QImage img;
    QByteArray bytes = vciq.photo();
    QBuffer buffer( &bytes );
    buffer.open( QIODevice::ReadOnly );
    QImageReader img_reader( &buffer, vciq.photoType().toUtf8() );
    img_reader.setAutoDetectImageFormat( true );
    img_reader.setScaledSize( QSize( 96, 96 ) );
    if( img_reader.read( &img ) )
    {
      QPixmap pix = QPixmap::fromImage( img );
      vc.setPhoto( pix );
      qDebug() << "XMPP> vCard avatar image type" << vciq.photoType() << "received";
    }
    else
      qWarning() << "XMPP> unable to load avatar image type" << vciq.photoType();

  }
  emit vCardReceived( bare_jid, vc );
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
