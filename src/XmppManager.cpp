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

#include "XmppManager.h"
#include "QXmppClient.h"
#include "QXmppRosterManager.h"



XmppManager::XmppManager( QObject* parent )
  : QObject( parent )
{
  mp_client = new QXmppClient( this );

  connect( mp_client, SIGNAL( error( QXmppClient::Error ) ), this, SLOT( errorOccurred( QXmppClient::Error ) ) );
  connect( &(mp_client->rosterManager()), SIGNAL( rosterReceived() ), this, SLOT( rosterReceived() ) );
  connect( &(mp_client->rosterManager()), SIGNAL( rosterChanged( const QString& ) ), this, SLOT( rosterChanged( const QString& ) ) );
  connect( &(mp_client->rosterManager()), SIGNAL( presenceChanged( const QString&, const QString& ) ), this, SLOT( presenceChanged( const QString&, const QString& ) ) );
  connect( &(mp_client->rosterManager()), SIGNAL( messageReceived( const QXmppMessage& ) ), this, SLOT( messageReceived( const QXmppMessage& ) ) );
}

void XmppManager::connectToServer()
{
  /*
  QNetworkProxy proxy;
  proxy.setType( QNetworkProxy::HttpProxy );
  proxy.setHostName( "10.184.160.100" );
  proxy.setPort( 8080);

  mp_client->configuration().setNetworkProxy( proxy );
  mp_client->configuration().setJid( );
  mp_client->configuration().setPassword( );
  */
}

void XmppManager::disconnectFromServer()
{

}

void XmppManager::errorOccurred( QXmppClient::Error )
{

}

void XmppManager::rosterReceived()
{

}

void XmppManager::rosterChanged( const QString& )
{

}

void XmppManager::presenceChanged( const QString&, const QString& )
{

}

void XmppManager::messageReceived( const QXmppMessage& )
{

}
