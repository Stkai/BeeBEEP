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
// $Id: Core.cpp 80 2011-09-03 01:45:25Z mastroddi $
//
//////////////////////////////////////////////////////////////////////

#include "BeeUtils.h"
#include "Core.h"
#include "Broadcaster.h"
#include "Settings.h"
#include "Protocol.h"


Core::Core( QObject* parent )
  : QObject( parent ), m_users(), m_chats(), m_connections()
{
  qDebug() << "Core created";
  createDefaultChat();

  mp_listener = new Listener( this );
  qDebug() << "Listener created";
  mp_broadcaster = new Broadcaster( this );
  qDebug() << "Broadcaster created";
  mp_fileTransfer = new FileTransfer( this );
  qDebug() << "FileTransfer created";

  connect( mp_broadcaster, SIGNAL( newPeerFound( const QHostAddress&, int ) ), this, SLOT( newPeerFound( const QHostAddress&, int ) ) );
  connect( mp_listener, SIGNAL( newConnection( Connection* ) ), this, SLOT( setNewConnection( Connection* ) ) );
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
                               .arg( Bee::iconToHtml( ":/images/red-ball.png", "*E*" ),
                                     Settings::instance().programName() ), DispatchToChat );
      return false;
    }
  }

  qDebug() << "Listener binds" << mp_listener->serverAddress().toString() << mp_listener->serverPort();
  Settings::instance().setLocalUserHost( mp_listener->serverAddress(), mp_listener->serverPort() );

  if( !mp_broadcaster->startBroadcasting() )
  {
    dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER,
                           tr( "%1 Unable to broadcast to %2 Network. Please check your firewall settings." )
                             .arg( Bee::iconToHtml( ":/images/red-ball.png", "*E*" ),
                                   Settings::instance().programName() ), DispatchToChat );
    mp_listener->close();
    return false;
  }

  dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER,
                         tr( "%1 You are connected to %2 Network." )
                         .arg( Bee::iconToHtml( ":/images/green-ball.png", "*C*" ),
                               Settings::instance().programName() ), DispatchToAllChatsWithUser );

  showUserStatusChanged( Settings::instance().localUser() );

  if( Settings::instance().showTipsOfTheDay() )
    showTipOfTheDay();
  return true;
}

void Core::stop()
{
  mp_broadcaster->stopBroadcasting();
  mp_fileTransfer->stopListener();
  mp_listener->close();

  foreach( Connection* c, m_connections )
    closeConnection( c );

  m_connections.clear();

  dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER,
                         tr( "%1 You are disconnected from %2 Network.").arg( Bee::iconToHtml( ":/images/red-ball.png", "*D*" ),
                                                                              Settings::instance().programName() ), DispatchToAllChatsWithUser );
}
