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

#ifdef USE_QXMPP

#include "BeeUtils.h"
#include "PluginManager.h"
#include "Protocol.h"
#include "Settings.h"
#include "UserManager.h"
#include "XmppManager.h"
#include "QXmppClient.h"
#include "QXmppMessage.h"
#include "QXmppPresence.h"
#include "QXmppRosterManager.h"
#include "QXmppUtils.h"
#include "QXmppVCardIq.h"
#include "QXmppVCardManager.h"


XmppManager::XmppManager( QObject* parent )
  : QObject( parent )
{
}

XmppClient* XmppManager::client( const QString& service_name ) const
{
  QList<XmppClient*>::const_iterator it = m_clients.begin();
  while( it != m_clients.end() )
  {
    if( (*it)->service() == service_name )
      return *it;
    ++it;
  }
  qDebug() << "XMPP> Service" << service_name << "not found";
  return 0;
}

XmppClient* XmppManager::createClient( const QString& client_service, const QString& client_icon_path )
{
  XmppClient* mp_client = client( client_service );
  if( mp_client )
    return mp_client;
  mp_client = new XmppClient( this );
  mp_client->setupManagers();
  mp_client->setService( client_service );
  mp_client->setIconPath( client_icon_path );

  connect( mp_client, SIGNAL( error( QXmppClient::Error ) ), this, SLOT( errorOccurred( QXmppClient::Error ) ) );
  connect( mp_client, SIGNAL( connected() ), this, SLOT( serverConnected() ) );
  connect( mp_client, SIGNAL( disconnected() ), this, SLOT( serverDisconnected() ) );
  connect( mp_client, SIGNAL( rosterReceived() ), this, SLOT( rosterReceived() ) );
  connect( mp_client, SIGNAL( rosterChanged( const QString& ) ), this, SLOT( rosterChanged( const QString& ) ) );
  connect( mp_client, SIGNAL( presenceChanged( const QString&, const QString& ) ), this, SLOT( presenceChanged( const QString&, const QString& ) ) );
  connect( mp_client, SIGNAL( vCardReceived( const QXmppVCardIq& ) ), this, SLOT( vCardReceived( const QXmppVCardIq& ) ) );
  connect( mp_client, SIGNAL( messageReceived( const QXmppMessage& ) ), this, SLOT( messageReceived( const QXmppMessage& ) ) );
  connect( mp_client, SIGNAL( presenceReceived( const QXmppPresence& ) ), this, SLOT( presenceReceived( const QXmppPresence& ) ) );
  connect( mp_client, SIGNAL( clientVCardReceived() ), this, SLOT( clientVCardReceived() ) );

  m_clients.append( mp_client );
  return mp_client;
}

bool XmppManager::connectToServer( const QString& service, const QString& user_name, const QString& passwd )
{
  XmppClient* mp_client = client( service );
  if( !mp_client )
  {
    Message m = Protocol::instance().systemMessage( QString( "%1 %2: service unavailable." ).arg( Bee::iconToHtml( ":/images/red-ball.png", "*E*" ), service ) );
    emit message( service, Settings::instance().localUser().bareJid(), m );
    return false;
  }

  if( mp_client->isConnected() )
  {
    makeSystemMessage( mp_client, Settings::instance().localUser().bareJid(), tr( "already connected to the server" ) );
    return false;
  }

  if( mp_client->isActive() )
  {
    makeSystemMessage( mp_client, Settings::instance().localUser().bareJid(), tr( "connection in progress to the server. Please wait" ) );
    return false;
  }

  if( user_name.isEmpty() )
  {
    makeSystemMessage( mp_client, Settings::instance().localUser().bareJid(), tr( "Username is empty. Unable to connect to the server" ) );
    return false;
  }

  if( passwd.isEmpty() )
  {
    makeSystemMessage( mp_client, Settings::instance().localUser().bareJid(), tr( "Password is empty. Unable to connect to the server" ) );
    return false;
  }

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
  {
    qDebug() << "XMPP> connection use proxy" << proxy.hostName() << proxy.port() << "with type" << proxy.type();
    mp_client->configuration().setNetworkProxy( proxy );
  }

  mp_client->configuration().setJid( user_name );
  mp_client->configuration().setPassword( passwd );
  mp_client->configuration().setResource( QString( "BeeBeep" ) );
  mp_client->configuration().setAutoReconnectionEnabled( false );

  makeSystemMessage( mp_client, Settings::instance().localUser().bareJid(), tr( "connection in progress.." ) );
  mp_client->setConnectionState( XmppClient::InProgress );
  mp_client->connectToServer( mp_client->configuration() );

  qDebug() << "XMPP> Connecting to" << mp_client->service();
  return true;
}

void XmppManager::disconnectFromServer()
{
  foreach( XmppClient* mp_client, m_clients )
  {
    if( mp_client->isActive() )
    {
      mp_client->disconnectFromServer();
      mp_client->setConnectionState( XmppClient::Offline );
    }
  }
}

void XmppManager::disconnectFromServer( const QString& service )
{
  XmppClient* mp_client = client( service );
  if( !mp_client )
    return;
  qDebug() << "XMPP> Disconnecting from" << mp_client->service();
  mp_client->disconnectFromServer();
  mp_client->setConnectionState( XmppClient::Offline );
}

void XmppManager::serverConnected()
{
  XmppClient* mp_client = qobject_cast<XmppClient*>( sender() );
  if( !mp_client )
  {
    qWarning() << "Unable to cast QObject in XmppClient in XmppManager::serverConnected";
    return;
  }
  User u = mp_client->clientUser();
  u.setBareJid( mp_client->configuration().jidBare() );
  mp_client->setClientUser( u );
  mp_client->setConnectionState( XmppClient::Online );
  makeSystemMessage( mp_client, Settings::instance().localUser().bareJid(), tr( "connected to the server with user %1" ).arg( mp_client->configuration().jidBare() ) );
  emit serviceConnected( mp_client->service() );
}

void XmppManager::serverDisconnected()
{
  XmppClient* mp_client = qobject_cast<XmppClient*>( sender() );
  if( !mp_client )
  {
    qWarning() << "Unable to cast QObject in XmppClient in XmppManager::serverDisconnected";
    return;
  }
  mp_client->setConnectionState( XmppClient::Offline );
  makeSystemMessage( mp_client, Settings::instance().localUser().bareJid(), tr( "disconnected from the server" ) );
  UserList ul = UserManager::instance().userList().serviceUserList( mp_client->service() );
  Message m;
  foreach( User u, ul.toList() )
  {
    m = Protocol::instance().userStatusMessage( User::Offline, u.statusDescription() );
    emit message( mp_client->service(), u.bareJid(), m );
  }

  emit serviceDisconnected( mp_client->service() );
}

bool XmppManager::isConnected() const
{
  foreach( XmppClient* xc, m_clients )
  {
    if( xc->isConnected() )
      return true;
  }
  return false;
}

void XmppManager::errorOccurred( QXmppClient::Error err )
{
  XmppClient* mp_client = qobject_cast<XmppClient*>( sender() );
  if( !mp_client )
  {
    qWarning() << "Unable to cast QObject in XmppClient in XmppManager::errorOccurred";
    return;
  }

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

  mp_client->setConnectionState( XmppClient::Offline );
  makeSystemMessage( mp_client, Settings::instance().localUser().bareJid(), tr( "connection error (%1)" ).arg( s_error ) );
}

void XmppManager::rosterReceived()
{
  XmppClient* mp_client = qobject_cast<XmppClient*>( sender() );
  if( !mp_client )
  {
    qWarning() << "Unable to cast QObject in XmppClient in XmppManager::rosterReceived";
    return;
  }

  qDebug() << "XMPP>" << mp_client->service() << "has sent roster";
  QStringList bare_jid_list = mp_client->rosterManager().getRosterBareJids();
  foreach( QString bare_jid, bare_jid_list )
    checkUserChanged( mp_client, bare_jid );

  requestVCard( mp_client->service(), "" );
}

void XmppManager::rosterChanged( const QString& bare_jid )
{
  qDebug() << "XMPP> user" << bare_jid << "changed in roster";
  XmppClient* mp_client = qobject_cast<XmppClient*>( sender() );
  if( !mp_client )
  {
    qWarning() << "Unable to cast QObject in XmppClient in XmppManager::rosterChanged";
    return;
  }

  QMap<QString, QXmppPresence> presences = mp_client->rosterManager().getAllPresencesForBareJid( bare_jid );
  if( presences.isEmpty() )
    return;

}

void XmppManager::checkUserChanged( XmppClient* mp_client, const QString& bare_jid )
{
  User u;
  u.setName( bare_jid );
  u.setBareJid( bare_jid );
  u.setService( mp_client->service() );
  u.setHostAddress( QHostAddress( mp_client->configuration().host() ) );
  u.setHostPort( mp_client->configuration().port() );
  emit userChangedInRoster( u );
}

void XmppManager::presenceChanged( const QString& bare_jid, const QString& jid_resource )
{
  XmppClient* mp_client = qobject_cast<XmppClient*>( sender() );
  if( !mp_client )
  {
    qWarning() << "Unable to cast QObject in XmppClient in XmppManager::presenceChanged";
    return;
  }

  qDebug() << "XMPP> presence received for" << bare_jid;

  QXmppPresence presence = mp_client->rosterManager().getPresence( bare_jid, jid_resource );
  User::Status status;

  if( presence.type() == QXmppPresence::Available )
    status = statusFromPresence( presence.availableStatusType() );
  else if( presence.type() == QXmppPresence::Unavailable )
    status = User::Offline;
  else
    return;

  QString status_desc = presence.statusText();

  if( mp_client->isLocalUser( bare_jid ) )
  {
    User u = mp_client->clientUser();
    u.setStatus( status );
    u.setStatusDescription( status_desc );
    mp_client->setClientUser( u );
    qDebug() << "XMPP> local user presence received from" << mp_client->service() << "... saved";
    return;
  }

  checkUserChanged( mp_client, bare_jid );

  Message m = Protocol::instance().userStatusMessage( status, status_desc );
  emit message( mp_client->service(), bare_jid, m );

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
      emit vCardAvailable( mp_client->service(), bare_jid );
      break;
    default:
      qDebug() << "XMPP> vCardUpdateType unknown in presence message";
    }
  }
}

void XmppManager::requestVCard( const QString& service, const QString& bare_jid )
{
  XmppClient* mp_client = client( service );
  if( !mp_client )
    return;
  qDebug() << "XMPP> requesting vCard for user" << bare_jid;

  if( bare_jid.isEmpty() || mp_client->isLocalUser( bare_jid ) )
  {
    if( !mp_client->vCardManager().isClientVCardReceived() )
    {
      qDebug() << "XMPP> client vCard requested from" << service;
      mp_client->vCardManager().requestClientVCard();
    }
  }
  else
    mp_client->vCardManager().requestVCard( bare_jid );
}

void XmppManager::messageReceived( const QXmppMessage& xmpp_msg )
{
  XmppClient* mp_client = qobject_cast<XmppClient*>( sender() );
  if( !mp_client )
  {
    qWarning() << "Unable to cast QObject in XmppClient in XmppManager::messageReceived";
    return;
  }

  QString bare_jid = QXmppUtils::jidToBareJid( xmpp_msg.from() );
  qDebug() << "XMPP> message received from" << bare_jid;
  dumpMessage( xmpp_msg );

  switch( xmpp_msg.type() )
  {
  //case QXmppMessage::GroupChat:
  //case QXmppMessage::Headline:
  case QXmppMessage::Normal:  // offline messages
  case QXmppMessage::Chat:
    parseChatMessage( mp_client, bare_jid, xmpp_msg );
    break;
  case QXmppMessage::Error:
    parseErrorMessage( mp_client, bare_jid, xmpp_msg );
    break;
  default:
    qWarning() << "XMPP> cannot handle message type:" << xmpp_msg.type();
  }
}

void XmppManager::makeSystemMessage( XmppClient* mp_client, const QString& bare_jid, const QString& txt )
{
  QString msg = QString( "%1: %2." ).arg( mp_client->service(), txt );
  qDebug() << "XMPP>" << qPrintable( msg );
  Message m = Protocol::instance().systemMessage( QString( "%1 %2" ).arg( Bee::iconToHtml( mp_client->iconPath(), "*@*" ), msg ) );
  emit message( mp_client->service(), bare_jid, m );
}

User::Status XmppManager::statusFromPresence( QXmppPresence::AvailableStatusType xmpp_presence_status_type )
{
  switch( xmpp_presence_status_type )
  {
  case QXmppPresence::Online:
  case QXmppPresence::Chat:
    return User::Online;
  case QXmppPresence::Away:
  case QXmppPresence::XA:
    return User::Away;
  case QXmppPresence::DND:
    return User::Busy;
  default:
    qWarning() << "XMPP> unable to convert presence status" << xmpp_presence_status_type << "to user status";
    return User::Offline;
  }
}

void XmppManager::sendMessage( const User& u, const Message& m )
{
  XmppClient* mp_client = client( u.service() );
  if( !mp_client )
    return;

  if( !mp_client->isConnected() )
  {
    makeSystemMessage( mp_client, u.bareJid(), tr( "is not connected. Unable to send the message" ) );
    return;
  }

  if( m.type() == Message::Chat )
    mp_client->sendMessage( u.bareJid(), m.text() );
}

void XmppManager::parseChatMessage( XmppClient* mp_client, const QString& bare_jid, const QXmppMessage& xmpp_msg )
{
  Message m;
  if( xmpp_msg.state() == QXmppMessage::Composing )
  {
    m = Protocol::instance().userStatusMessage( User::Online, "" );
    m.addFlag( Message::UserWriting );
  }
  else
  {
    if( xmpp_msg.body().isEmpty() )
    {
      qDebug() << "XMPP> body message is empty... skip it";
      return;
    }
    m = Protocol::instance().chatMessage( xmpp_msg.body() );
    m.setData( QColor( 0, 0, 0 ).name() );
  }

  m.addFlag( Message::Private );
  m.setTimestamp( xmpp_msg.stamp() );

  qDebug() << "XMPP> message received from" << bare_jid << "with body:" << m.text();
  emit message( mp_client->service(), bare_jid, m );
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

void XmppManager::parseErrorMessage( XmppClient* mp_client, const QString& bare_jid, const QXmppMessage& xmpp_msg )
{
  QString s_error = errorConditionToString( xmpp_msg.error().condition() );
  qDebug() << "XMPP> error" << s_error << "in message from" << bare_jid;
  makeSystemMessage( mp_client, bare_jid, tr( "error occurred (%1)" ).arg( s_error ) );
}

void XmppManager::subscribeUser( const QString& service, const QString& bare_jid, bool accepted )
{
  XmppClient* mp_client = client( service );
  if( !mp_client )
    return;

  if( !mp_client->isConnected() )
  {
    makeSystemMessage( mp_client, bare_jid, tr( "is not connected. Unable to add %1 to the contact list").arg( bare_jid ) );
    return;
  }

  if( accepted )
  {
    makeSystemMessage( mp_client, bare_jid, tr( "adding %1 to the contact list").arg( bare_jid ) );
    QXmppPresence subscribed;
    subscribed.setTo( bare_jid );
    subscribed.setType( QXmppPresence::Subscribed );
    mp_client->sendPacket( subscribed );

    // reciprocal subscription
    QXmppPresence subscribe;
    subscribe.setTo( bare_jid );
    subscribe.setType( QXmppPresence::Subscribe );
    mp_client->sendPacket( subscribe );
  }
  else
  {
    makeSystemMessage( mp_client, bare_jid, tr( "%1's request is rejected").arg( bare_jid ) );
    QXmppPresence unsubscribed;
    unsubscribed.setTo( bare_jid );
    unsubscribed.setType( QXmppPresence::Unsubscribed );
    mp_client->sendPacket( unsubscribed );
  }
}

void XmppManager::removeUser( const User& u )
{
  XmppClient* mp_client = client( u.service() );
  if( !mp_client )
    return;

  if( !mp_client->isConnected() )
  {
    makeSystemMessage( mp_client, u.bareJid(), tr( "is not connected. Unable to remove %1 from the contact list").arg( u.bareJid() ) );
    return;
  }

  makeSystemMessage( mp_client, u.bareJid(), tr( "removing %1 from the contact list").arg( u.bareJid() ) );
  QXmppRosterIq remove;
  remove.setType( QXmppIq::Set );
  QXmppRosterIq::Item item_remove;
  item_remove.setSubscriptionType( QXmppRosterIq::Item::Remove );
  item_remove.setBareJid( u.bareJid() );
  remove.addItem( item_remove );
  mp_client->sendPacket( remove );
}

void XmppManager::sendLocalUserPresence()
{
  const User& u = Settings::instance().localUser();
  QXmppPresence::Type presence_type;
  QXmppPresence::AvailableStatusType presence_status_type;

  if( u.isConnected() )
    presence_type = QXmppPresence::Available;
  else
    presence_type = QXmppPresence::Unavailable;

  switch( u.status() )
  {
  case User::Busy:
    presence_status_type = QXmppPresence::DND;
    break;
  case User::Away:
    presence_status_type = QXmppPresence::Away;
    break;
  default:
    presence_status_type = QXmppPresence::Online;
  }

  foreach( XmppClient* mp_client, m_clients )
  {
    if( mp_client->isConnected() )
    {
      QXmppPresence presence = mp_client->clientPresence();
      presence.setType( presence_type );
      presence.setAvailableStatusType( presence_status_type );
      presence.setStatusText( u.statusDescription() );
      mp_client->setClientPresence( presence );
    }
  }
}

void XmppManager::presenceReceived( const QXmppPresence& presence )
{
  XmppClient* mp_client = qobject_cast<XmppClient*>( sender() );
  if( !mp_client )
  {
    qWarning() << "Unable to cast QObject in XmppClient in XmppManager::presenceReceived";
    return;
  }

  QString bare_jid = QXmppUtils::jidToBareJid( presence.from() );
  if( mp_client->isLocalUser( bare_jid ) )
  {
    qDebug() << "XMPP> presence from local user received... skip it";
    return;
  }

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

 makeSystemMessage( mp_client, bare_jid, msg.arg( bare_jid ) );

 if( subscribe_request )
   emit userSubscriptionRequest( mp_client->service(), bare_jid );

 if( subscription_accepted )
   checkUserChanged( mp_client, bare_jid );
}

void XmppManager::vCardReceived( const QXmppVCardIq& vciq )
{
  XmppClient* mp_client = qobject_cast<XmppClient*>( sender() );
  if( !mp_client )
  {
    qWarning() << "Unable to cast QObject in XmppClient in XmppManager::vCardReceived";
    return;
  }

  QString bare_jid = QXmppUtils::jidToBareJid( vciq.from() );

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

  if( mp_client->isLocalUser( bare_jid ) )
  {
    User u = mp_client->clientUser();
    u.setVCard( vc );
    mp_client->setClientUser( u );
    qDebug() << "XMPP> vCard received from local user" << bare_jid << "... saved";
    return;
  }

  emit vCardReceived( mp_client->service(), bare_jid, vc );
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

void XmppManager::loadClients()
{
  foreach( ServiceInterface* si, PluginManager::instance().services() )
  {
    if( si->protocol() == "xmpp" )
    {
      XmppClient* mp_client = createClient( si->name(), si->iconFileName() );
      mp_client->configuration().setHost( si->hostAddress() );
      mp_client->configuration().setPort( si->hostPort() );
      qDebug() << "XMPP> Service" << mp_client->service() << "created";
    }
  }
}

bool XmppManager::sendFile( const User& u, const FileInfo& fi )
{
  XmppClient* mp_client = client( u.service() );
  if( !mp_client )
    return false;
  if( !mp_client->isConnected() )
    return false;
  mp_client->sendFile( u.bareJid(), fi );
  return true;
}

void XmppManager::sendComposingMessage( const User& u )
{
  XmppClient* mp_client = client( u.service() );
  if( !mp_client )
    return;
  if( !mp_client->isConnected() )
    return;
  QXmppMessage xmpp_message( mp_client->configuration().jid(), u.bareJid(), "", "" );
  xmpp_message.setType( QXmppMessage::Chat );
  xmpp_message.setState( QXmppMessage::Composing );
  mp_client->sendPacket( xmpp_message );
}

void XmppManager::clientVCardReceived()
{
  XmppClient* mp_client = qobject_cast<XmppClient*>( sender() );
  if( !mp_client )
  {
    qWarning() << "Unable to cast QObject in XmppClient in XmppManager::clientVCardReceived";
    return;
  }

  qDebug() << "XMPP> client vCard received from" << mp_client->service();
}

void XmppManager::sendLocalUserVCard()
{
  VCard vc = Settings::instance().localUser().vCard();
  QXmppVCardIq vciq;
  vciq.setType( QXmppIq::Set );

  vciq.setNickName( vc.nickName() );
  vciq.setFirstName( vc.firstName() );
  vciq.setLastName( vc.lastName() );
  vciq.setFullName( vc.fullName() );
  vciq.setEmail( vc.email() );
  vciq.setBirthday( vc.birthday() );

  if( !vc.photo().isNull() )
  {
    QByteArray ba;
    QBuffer buffer( &ba );
    if( buffer.open( QIODevice::WriteOnly ) )
    {
      if( vc.photo().save( &buffer, "PNG" ) )
      {
        vciq.setPhoto( ba );
        vciq.setPhotoType( "PNG" );
      }
    }
  }

  foreach( XmppClient* mp_client, m_clients )
    mp_client->sendVCard( vciq );
}

#endif // USE_QXMPP
