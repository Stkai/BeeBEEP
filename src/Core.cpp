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

#include "BeeUtils.h"
#include "Connection.h"
#include "Core.h"
#include "Broadcaster.h"
#include "FileShare.h"
#include "Settings.h"
#include "Protocol.h"


Core::Core( QObject* parent )
 : QObject( parent ), m_connections()
{
  qDebug() << "Core created";
  createDefaultChat();

  mp_listener = new Listener( this );
  qDebug() << "Listener created";
  mp_broadcaster = new Broadcaster( this );
  qDebug() << "Broadcaster created";
  mp_fileTransfer = new FileTransfer( this );
  qDebug() << "FileTransfer created";
  m_shareListToBuild = 0;

  connect( mp_broadcaster, SIGNAL( newPeerFound( const QHostAddress&, int ) ), this, SLOT( newPeerFound( const QHostAddress&, int ) ) );
  connect( mp_listener, SIGNAL( newConnection( Connection* ) ), this, SLOT( setNewConnection( Connection* ) ) );
  connect( mp_fileTransfer, SIGNAL( listening() ), this, SLOT( fileTransferServerListening() ) );
  connect( mp_fileTransfer, SIGNAL( userConnected( VNumber, const QHostAddress&, const Message& ) ), this, SLOT( validateUserForFileTransfer( VNumber, const QHostAddress&, const Message& ) ) );
  connect( mp_fileTransfer, SIGNAL( progress( VNumber, VNumber, const FileInfo&, FileSizeType ) ), this, SLOT( checkFileTransferProgress( VNumber, VNumber, const FileInfo&, FileSizeType ) ) );
  connect( mp_fileTransfer, SIGNAL( message( VNumber, VNumber, const FileInfo&, const QString& ) ), this, SLOT( checkFileTransferMessage( VNumber, VNumber, const FileInfo&, const QString& ) ) );
}

bool Core::start()
{
  qDebug() << "Starting" << Settings::instance().programName() << "core";

  if( !mp_listener->listen( QHostAddress::Any, Settings::instance().localUser().hostPort() ) )
  {
    qDebug() << "Unable to bind" << Settings::instance().localUser().hostPort() << "port. Try to bind the first available";
    if( !mp_listener->listen( QHostAddress::Any ) )
    {
      dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER,
                             tr( "%1 Unable to connect to %2 Network. Please check your firewall settings." )
                               .arg( Bee::iconToHtml( ":/images/network-disconnected.png", "*E*" ),
                                     Settings::instance().programName() ), DispatchToChat );
      return false;
    }
  }

  qDebug() << "Listener binds" << mp_listener->serverAddress().toString() << mp_listener->serverPort();

  Settings::instance().setLocalUserHost( Settings::instance().searchLocalHostAddress(), mp_listener->serverPort() );

  if( !mp_broadcaster->startBroadcasting() )
  {
    dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER,
                           tr( "%1 Unable to broadcast to %2 Network. Please check your firewall settings." )
                             .arg( Bee::iconToHtml( ":/images/network-disconnected.png", "*E*" ),
                                   Settings::instance().programName() ), DispatchToChat );
    mp_listener->close();
    return false;
  }

  dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER,
                         tr( "%1 You are connected to %2 Network." )
                         .arg( Bee::iconToHtml( ":/images/network-connected.png", "*C*" ),
                               Settings::instance().programName() ), DispatchToAllChatsWithUser );

  if( Settings::instance().fileShare() )
    startFileTransferServer();

  if( Settings::instance().showTipsOfTheDay() )
    showTipOfTheDay();

  if( Settings::instance().localUser().status() == User::Offline )
  {
    User u = Settings::instance().localUser();
    u.setStatus( User::Online );
    Settings::instance().setLocalUser( u );
  }

  showUserStatusChanged( Settings::instance().localUser() );
  showUserVCardChanged( Settings::instance().localUser() );

  qDebug() << "Local user path:" << Settings::instance().localUser().path();

  return true;
}

void Core::stop()
{
  mp_broadcaster->stopBroadcasting();
  stopFileTransferServer();

  mp_listener->close();

  foreach( Connection* c, m_connections )
    closeConnection( c );

  m_connections.clear();

  dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER,
                         tr( "%1 You are disconnected from %2 Network.").arg( Bee::iconToHtml( ":/images/network-disconnected.png", "*D*" ),
                                                                              Settings::instance().programName() ), DispatchToAllChatsWithUser );
}

void Core::addBroadcastAddress( const QHostAddress& host_address )
{
  if( mp_broadcaster->addAddress( host_address ) )
  {
    QString sHtmlMsg = tr( "%1 Looking for the available users in %2..." )
          .arg( Bee::iconToHtml( ":/images/search.png", "*B*" ), host_address.toString() );
    dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER, sHtmlMsg, DispatchToChat );
  }
}

void Core::sendBroadcastMessage()
{
  if( isConnected() )
  {
    mp_broadcaster->sendBroadcastMessage();
    dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER,
                         tr( "%1 Broadcasting to the %2 Network..." ).arg( Bee::iconToHtml( ":/images/broadcast.png", "*B*" ),
                                                                          Settings::instance().programName() ), DispatchToChat );
  }
  else
    dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER,
                         tr( "%1 You are not connected to %2 Network." ).arg( Bee::iconToHtml( ":/images/red-ball.png", "*E*" ),
                                                                             Settings::instance().programName() ), DispatchToChat );
}

bool Core::isConnected() const
{
  return mp_listener->isListening();
}

