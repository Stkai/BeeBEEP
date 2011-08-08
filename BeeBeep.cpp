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

#include "BeeBeep.h"
#include "BeeUtils.h"
#include "Connection.h"
#include "FileTransferServer.h"
#include "Listener.h"
#include "PeerManager.h"
#include "Protocol.h"
#include "Settings.h"


BeeBeep::BeeBeep( QObject* parent )
 : QObject( parent )
{
  mp_listener = new Listener( this );
  mp_peerManager = new PeerManager( this );
  mp_fileServer = new FileTransferServer( this );

  (void)chat( Settings::instance().defaultChatName(), true, false );

  connect( mp_peerManager, SIGNAL( newPeerFound( const QHostAddress&, int ) ), this, SLOT( newPeerFound( const QHostAddress&, int ) ) );
  connect( mp_listener, SIGNAL( newConnection( Connection* ) ), this, SLOT( newConnection( Connection* ) ) );
  connect( mp_fileServer, SIGNAL( transferMessage( const User&, const FileInfo&, const QString& ) ), this, SLOT( checkFileTransfer( const User&, const FileInfo&, const QString& ) ) );
}

bool BeeBeep::isWorking() const
{
  return mp_listener->isListening();
}

QString BeeBeep::id() const
{
  return Settings::instance().localUser().nickname();
}

void BeeBeep::start()
{
  if( !mp_listener->listen( QHostAddress::Any, LISTENER_DEFAULT_PORT ) )
  {
    if( !mp_listener->listen( QHostAddress::Any ) )
    {
      dispatchSystemMessage( Settings::instance().defaultChatName(),
                             tr( "%1 Unable to connect to %2 Network. Please check your firewall settings." )
                               .arg( Bee::iconToHtml( ":/images/red-ball.png", "*E*" ) )
                               .arg( Settings::instance().programName() ) );
      return;
    }
  }

#if defined( BEEBEEP_DEBUG )
  qDebug() << "Listen on port" << mp_listener->serverPort();
#endif
  Settings::instance().setListenerPort( mp_listener->serverPort() );
  mp_peerManager->startBroadcasting();
  dispatchSystemMessage( Settings::instance().defaultChatName(),
                         tr( "%1 You are connected to %2 Network." )
                         .arg( Bee::iconToHtml( ":/images/green-ball.png", "*C*" ) )
                         .arg( Settings::instance().programName() ) );
  emit newUser( Settings::instance().localUser() );
  setUserStatus( Settings::instance().localUser() );
}

void BeeBeep::stop()
{
  mp_fileServer->close();
  mp_listener->close();
  mp_peerManager->stopBroadcasting();
  QList<Connection*> connection_list = m_peers.values();
  foreach( Connection* c, connection_list )
  {
    removeConnection( c );
  }
  m_peers.clear();
  dispatchSystemMessage( Settings::instance().defaultChatName(),
                         tr( "%1 You are disconnected.").arg( Bee::iconToHtml( ":/images/red-ball.png", "*D*" ) ) );
}

Connection* BeeBeep::connection( const QString& chat_name )
{
  QList<Connection*> connection_list = m_peers.values();
  foreach( Connection* c, connection_list )
  {
    if( Settings::instance().chatName( c->user() ) == chat_name )
      return c;
  }
  return NULL;
}

bool BeeBeep::hasConnection( const QHostAddress& sender_ip, int sender_port ) const
{
  QList<Connection*> connection_list = m_peers.values();
  foreach( Connection *c, connection_list )
  {
    if( (sender_port == -1 || c->peerPort() == sender_port) && c->peerAddress() == sender_ip )
      return true;
  }
  return false;
}

void BeeBeep::newConnection( Connection *c )
{
  connect( c, SIGNAL( error( QAbstractSocket::SocketError ) ), this, SLOT( connectionError( QAbstractSocket::SocketError ) ) );
  connect( c, SIGNAL( disconnected() ), this, SLOT( disconnected() ) );
  connect( c, SIGNAL( readyForUse() ), this, SLOT( readyForUse() ) );
}

void BeeBeep::newPeerFound( const QHostAddress& sender_ip, int sender_port )
{
    if( !hasConnection( sender_ip ) )
    {
      Connection *c = new Connection( this );
      newConnection( c );
      c->connectToHost( sender_ip, sender_port );
    }
}

void BeeBeep::readyForUse()
{
  Connection* c = qobject_cast<Connection*>( sender() );
  if( !c || hasConnection( c->peerAddress(), c->peerPort() ) )
    return;
  connect( c, SIGNAL( newMessage( const User&, const Message& ) ), this, SLOT( dispatchMessage( const User&, const Message& ) ) );
  connect( c, SIGNAL( newStatus( const User& ) ), this, SLOT( setUserStatus( const User& ) ) );
  connect( c, SIGNAL( isWriting( const User& ) ), this, SIGNAL( userIsWriting( const User& ) ) );
  connect( c, SIGNAL( newFileMessage( const User&, const FileInfo& ) ), this, SLOT( checkFileMessage( const User&, const FileInfo& ) ) );
  m_peers.insert( c->id(), c );
  emit newUser( c->user() );
  QString sHtmlMsg = tr( "%1 %2 has joined." ).arg( Bee::iconToHtml( ":/images/green-ball.png", "*U*" ) ).arg( Settings::instance().chatName( c->user() ) );
  dispatchSystemMessage( Settings::instance().defaultChatName(), sHtmlMsg );
  QTimer::singleShot( 500, c, SLOT( sendLocalUserStatus() ) );
}

void BeeBeep::connectionError( QAbstractSocket::SocketError )
{
  if( Connection* c = qobject_cast<Connection*>( sender() ) )
    removeConnection( c );
}

void BeeBeep::disconnected()
{
  if( Connection* c = qobject_cast<Connection*>( sender() ) )
    removeConnection( c );
}

void BeeBeep::removeConnection( Connection *c )
{
  if( m_peers.contains( c->id() ) )
  {
    m_peers.remove( c->id() );
    emit removeUser( c->user() );
    QString sHtmlMsg = tr( "%1 %2 has left." ).arg( Bee::iconToHtml( ":/images/red-ball.png", "*X*" ) ).arg( Settings::instance().chatName( c->user() ) );
    dispatchSystemMessage( Settings::instance().chatName( c->user() ), sHtmlMsg );
  }
  c->deleteLater();
}

QList<User> BeeBeep::users() const
{
  QList<User> user_list;
  user_list.append( Settings::instance().localUser() );
  QList<Connection*> connection_list = m_peers.values();
  foreach( Connection *c, connection_list )
  {
    user_list.append( c->user() );
  }
  return user_list;
}


User BeeBeep::user( int user_id ) const
{
  if( user_id == Settings::instance().localUser().id() )
    return Settings::instance().localUser();
  QList<Connection*> connection_list = m_peers.values();
  foreach( Connection* c, connection_list )
  {
    if( c->id() == user_id )
      return c->user();
  }
  return User();
}

Chat BeeBeep::chat( const QString& chat_name, bool create_if_need, bool read_all_messages )
{
  Chat c = m_chats.value( chat_name, Chat() );
  if( !c.isValid() )
  {
    if( create_if_need )
    {
      Chat new_chat;
      new_chat.setName( chat_name );
      QString sHtmlMsg = tr( "%1 Chat with %2." ).arg( Bee::iconToHtml( ":/images/chat.png", "*C*" ) ).arg( chat_name );
      ChatMessage cm( Settings::instance().localUser(), Protocol::instance().systemMessage( sHtmlMsg ) );
      new_chat.addMessage( cm );
      m_chats.insert( chat_name, new_chat );
      emit newChat( new_chat );
      return new_chat;
    }
    return c;
  }

  if( read_all_messages )
  {
    c.readAllMessages();
    m_chats.insert( c.name(), c );
  }
  return c;
}

void BeeBeep::sendMessage( const QString& chat_name, const QString& msg )
{
  if( !isWorking() )
  {
    dispatchSystemMessage( chat_name, tr( "%1 Unable to send the message: you are not connected." ).arg( Bee::iconToHtml( ":/images/red-ball.png", "*X*" ) ) );
    return;
  }

  if( msg.isEmpty() )
    return;

  Message m = Protocol::instance().chatMessage( msg );
  m.setData( Settings::instance().chatFontColor() );
  if( chat_name == Settings::instance().defaultChatName() )
  {
    QList<Connection*> connections = m_peers.values();
    foreach( Connection *c, connections )
    {
      c->sendMessage( m );
    }
  }
  else
  {
    m.addFlag( Message::Private );
    Connection* c = connection( chat_name );
    if( c )
    {
      c->sendMessage( m );
    }
    else
    {
      dispatchSystemMessage( chat_name, tr( "%1 Unable to send the message." ).arg( Bee::iconToHtml( ":/images/red-ball.png", "*X*" ) ) );
      return;
    }
  }

  ChatMessage cm( Settings::instance().localUser(), m );
  Chat c = chat( chat_name, true, false );
  c.addMessage( cm );
  m_chats.insert( c.name(), c );
  emit newMessage( chat_name, cm );
}

void BeeBeep::sendWritingMessage( const QString& chat_name )
{
  if( !isWorking() )
    return;
  Connection* c = connection( chat_name );
  if( c )
  {
#if defined( BEEBEEP_DEBUG )
    qDebug() << "Sending Writing Message";
#endif
    c->sendMessage( Protocol::instance().writingMessage() );
  }
}

void BeeBeep::sendUserStatus()
{
  Message m = Protocol::instance().userStatusToMessage( Settings::instance().localUser() );
  QList<Connection*> connections = m_peers.values();
  foreach( Connection *c, connections )
  {
    c->sendMessage( m );
  }
}

void BeeBeep::dispatchMessage( const User& u, const Message& m )
{
  Chat c = m.hasFlag( Message::Private ) ? chat( Settings::instance().chatName( u ), true, false ) : chat( Settings::instance().defaultChatName(), false, false );
  ChatMessage cm( u, m );
  c.addMessage( cm );
  c.addUnreadMessage();
  c.setLastMessageTimestamp( m.timestamp() );
  m_chats.insert( c.name(), c );
  emit newMessage( c.name(), cm );
}

void BeeBeep::dispatchSystemMessage( const QString& chat_name, const QString& msg )
{
  Chat c = chat( Settings::instance().defaultChatName(), false, false );
  Message m = Protocol::instance().systemMessage( msg );
  ChatMessage cm( Settings::instance().localUser(), m );
  c.addMessage( cm );
  m_chats.insert( c.name(), c );
  emit newMessage( c.name(), cm );
  if( chat_name == Settings::instance().defaultChatName() )
    return;

  c = chat( chat_name, false, false );
  if( c.isValid() )
  {
    c.addMessage( cm );
    m_chats.insert( c.name(), c );
    emit newMessage( c.name(), cm );
  }
}

void BeeBeep::searchUsers( const QHostAddress& host_address )
{
  mp_peerManager->sendDatagramToHost( host_address );
  QString sHtmlMsg = tr( "%1 Sending Beep to %2..." ).arg( Bee::iconToHtml( ":/images/search.png", "*b*" ) ).arg( host_address.toString() );
  dispatchSystemMessage( Settings::instance().defaultChatName(), sHtmlMsg );
}

void BeeBeep::setLocalUserStatus( int new_status )
{
  if( Settings::instance().localUser().status() == new_status )
    return;

  User u = Settings::instance().localUser();
  u.setStatus( new_status );
  Settings::instance().setLocalUser( u );
  sendUserStatus();
  setUserStatus( u );
}

void BeeBeep::setLocalUserStatusDescription( const QString& new_status_description )
{
  if( Settings::instance().localUser().statusDescription() == new_status_description )
    return;

  User u = Settings::instance().localUser();
  u.setStatusDescription( new_status_description );
  Settings::instance().setLocalUser( u );
  sendUserStatus();
  setUserStatus( u );
}

void BeeBeep::setUserStatus( const User& u )
{
  QString sHtmlMsg = Bee::iconToHtml( Bee::userStatusIconFileName( u.status() ), "*S*" ) + QString( " " );
  if( Settings::instance().localUser() == u )
    sHtmlMsg += tr( "You are" );
  else
    sHtmlMsg += (Settings::instance().showUserNickname() ? u.nickname() : u.name()) + QString( " " ) + tr( "is" );

   sHtmlMsg += QString( " %2%3." ).arg( userStatusToString( u.status() ) )
                            .arg( u.statusDescription().isEmpty() ? "" : QString( ": %1").arg( u.statusDescription() ) );

  dispatchSystemMessage( Settings::instance().chatName( u ), sHtmlMsg );
  emit( userNewStatus( u ) );
}

static const char* UserStatusToString[] =
{
  QT_TRANSLATE_NOOP( "BeeBeep", "offline" ),
  QT_TRANSLATE_NOOP( "BeeBeep", "online" ),
  QT_TRANSLATE_NOOP( "BeeBeep", "busy" ),
  QT_TRANSLATE_NOOP( "BeeBeep", "away" )
};

QString BeeBeep::userStatusToString( int user_status )
{
  if( user_status < 0 || user_status >= User::NumStatus )
    return "";
  else
    return tr( UserStatusToString[ user_status ] );
}

bool BeeBeep::sendFile( const QString& chat_name, const QString& file_path )
{
  QString icon_html = Bee::iconToHtml( ":/images/send-file.png", "*F*" );

  QFileInfo file( file_path );
  if( !file.exists() )
  {
     dispatchSystemMessage( chat_name, tr( "%1 %2: file not found." ).arg( icon_html ).arg( file_path ) );
     return false;
  }

  Connection* c = connection( chat_name );
  if( !c )
  {
    dispatchSystemMessage( chat_name, tr( "%1 Unable to send file: user is not connected." ).arg( icon_html ) );
    return false;
  }

  Settings::instance().setLastDirectorySelected( file.absoluteDir().absolutePath() );
  dispatchSystemMessage( chat_name, tr( "%1 You are sending %2..." ).arg( icon_html ).arg( file.fileName() ) );

  if( !mp_fileServer->isListening() )
    mp_fileServer->listen( QHostAddress::Any );

  FileInfo fi( FileInfo::Upload );
  fi.setName( file.fileName() );
  fi.setPath( file.absoluteFilePath() );
  fi.setSize( file.size() );
  fi.setHostAddress( mp_fileServer->serverAddress() );
  fi.setHostPort( mp_fileServer->serverPort() );
  fi.setPassword( Settings::instance().hash( "test_download" ).toUtf8() );

  mp_fileServer->uploadFile( c->user(), fi );
  Message m = Protocol::instance().fileInfoToMessage( fi );
  c->sendMessage( m );
  return true;
}

void BeeBeep::checkFileMessage( const User& u, const FileInfo& fi )
{
  QString icon_html = Bee::iconToHtml( ":/images/send-file.png", "*F*" );
  dispatchSystemMessage( Settings::instance().chatName( u ), tr( "%1 File request arrived: %2." ).arg( icon_html ).arg( fi.name() ) );

  //FIXME
  QString path = QString( "E:/" ) +  fi.name();

  FileInfo file_info = fi;
  file_info.setPath( path );
  mp_fileServer->downloadFile( u, file_info );
}

void BeeBeep::checkFileTransfer( const User& u, const FileInfo& fi, const QString& msg )
{
  QString icon_html = Bee::iconToHtml( ":/images/send-file.png", "*F*" );
  dispatchSystemMessage( Settings::instance().chatName( u ), tr( "%1 %2: %3." ).arg( icon_html ).arg( fi.name() ).arg( msg ) );
}
