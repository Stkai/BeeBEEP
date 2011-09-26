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

#include "XmppClient.h"


XmppClient::XmppClient( QObject* parent )
  : QXmppClient( parent ),
    m_service( "Jabber" ), m_iconPath( ":/images/jabber.png" ),
    m_connectionState( XmppClient::Offline )
{
  connect( &(rosterManager()), SIGNAL( rosterReceived() ), this, SIGNAL( rosterReceived() ) );
  connect( &(rosterManager()), SIGNAL( rosterChanged( const QString& ) ), this, SIGNAL( rosterChanged( const QString& ) ) );
  connect( &(rosterManager()), SIGNAL( presenceChanged( const QString&, const QString& ) ), this, SIGNAL( presenceChanged( const QString&, const QString& ) ) );
  connect( &(vCardManager()), SIGNAL( vCardReceived( const QXmppVCardIq& ) ), this, SIGNAL( vCardReceived( const QXmppVCardIq& ) ) );
}
