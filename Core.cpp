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

#include "Core.h"
#include "Listener.h"
#include "PeerManager.h"
#include "Settings.h"


Core::Core( QObject* parent )
  : QObject( parent ), m_localUser( ID_LOCAL_USER ), m_users(), m_chats(), m_connections()
{
  qDebug() << "Core created";
  createLocalUser();
  createDefaultChat();

  mp_listener = new Listener( this );
  qDebug() << "Listener created";
  mp_peerManager = new PeerManager( this );
  qDebug() << "PeerManager created";
  mp_fileTransfer = new FileTransfer( this );
  qDebug() << "FileTransfer created";

  connect( mp_peerManager, SIGNAL( newPeerFound( const QHostAddress&, int ) ), this, SLOT( newPeerFound( const QHostAddress&, int ) ) );
  connect( mp_listener, SIGNAL( newConnection( Connection* ) ), this, SLOT( setNewConnection( Connection* ) ) );
  connect( mp_fileTransfer, SIGNAL( message( const User&, const FileInfo&, const QString& ) ), this, SLOT( checkFileTransfer( const User&, const FileInfo&, const QString& ) ) );
  connect( mp_fileTransfer, SIGNAL( progress( const User&, const FileInfo&, FileSizeType ) ), this, SIGNAL( transferProgress( const User&, const FileInfo&, FileSizeType ) ) );
}

void Core::start()
{
  qDebug() << "Starting" << Settings::instance().programName() << "core";
  if( !mp_listener->listen( QHostAddress::Any, m_localUser.hostPort() ) )
  {
    if( !mp_listener->listen( QHostAddress::Any ) )
    {
      dispatchSystemMessage( ID_LOCAL_USER,
                             tr( "%1 Unable to connect to %2 Network. Please check your firewall settings." )
                               .arg( Bee::iconToHtml( ":/images/red-ball.png", "*E*" ) )
                               .arg( Settings::instance().programName() ) );
      return;
    }
  }

  qDebug() << "Listener binds" << mp_listener->serverAddress().toString() << mp_listener->serverPort();
  m_localUser.setHostAddress( mp_listener->serverAddress() );
  m_localUser.setHostPort( mp_listener->serverPort() );

  if( !mp_peerManager->startBroadcasting( mp_listener->serverPort() ) )
  {
    dispatchSystemMessage( ID_LOCAL_USER,
                           tr( "%1 Unable to broadcast to %2 Network. Please check your firewall settings." )
                             .arg( Bee::iconToHtml( ":/images/red-ball.png", "*E*" ) )
                             .arg( Settings::instance().programName() ) );
    mp_listener->close();
    return;
  }

  dispatchSystemMessage( UserManager::instance().defaultChat(),
                         tr( "%1 You are connected to %2 Network." )
                         .arg( Bee::iconToHtml( ":/images/green-ball.png", "*C*" ) )
                         .arg( Settings::instance().programName() ) );

  m_localUser.setStatus( User::Online );

  if( Settings::instance().showTipsOfTheDay() )
    showTipOfTheDay();
}

void Core::stop()
{
  saveLocalUser();
  mp_peerManager->stopBroadcasting();
  mp_fileTransfer->stopListener();
  mp_listener->close();

  foreach( Connection* c, m_peers )
  {
    removeConnection( c );
  }
  m_peers.clear();
  dispatchSystemMessage( UserManager::instance().defaultChat(),
                         tr( "%1 You are disconnected.").arg( Bee::iconToHtml( ":/images/red-ball.png", "*D*" ) ) );
}

void Core::newPeerFound( const QHostAddress& sender_ip, int sender_port )
{
  if( !hasConnection( sender_ip, sender_port ) ) // Check it: before the sender port is not checked and it was passed -1
  {
    Connection *c = new Connection( this );
    setNewConnection( c );
    c->connectToHost( sender_ip, sender_port );
  }
}
