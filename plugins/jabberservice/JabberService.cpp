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

#include <QtPlugin>
#include <QtDebug>
#include "JabberService.h"
#include "Version.h"


QString JabberService::name() const
{
  return tr( "Jabber" );
}

QString JabberService::version() const
{
  return "0.6.1";
}

QString JabberService::author() const
{
  return "Marco Mastroddi";
}

QString JabberService::help() const
{
  return tr( "You can connect to the Jabber.org server." );
}

QIcon JabberService::icon() const
{
  return QIcon( iconFileName() );
}

QString JabberService::iconFileName() const
{
  return QLatin1String( ":/plugins/jabber.png" );
}

int JabberService::priority() const
{
  return 1000;
}

QString JabberService::coreVersion() const
{
  return QString( BEEBEEP_VERSION );
}

QString JabberService::protocol() const
{
  return QLatin1String( "xmpp" );
}

QString JabberService::hostAddress() const
{
  return QLatin1String( "jabber.org" );
}

int JabberService::hostPort() const
{
  return 5222;
}

QString JabberService::domain() const
{
  return QLatin1String( "jabber.org" );
}

bool JabberService::useSASLAuthentication()
{
  return true;
}

bool JabberService::ignoreSslErrors() const
{
  return true;
}

int JabberService::streamSecurityMode() const
{
  return 0;
}

int JabberService::nonSASLAuthMechanism() const
{
  return 1;
}

int JabberService::sASLAuthMechanism() const
{
  return 1;
}

bool JabberService::fileTransferIsEnabled() const
{
  return true;
}

QString JabberService::userStatusOfflineIconPath() const
{
  return QLatin1String( ":/plugins/jabber-offline.png" );
}

QString JabberService::userStatusOnlineIconPath() const
{
  return QLatin1String( ":/plugins/jabber-online.png" );
}

QString JabberService::userStatusBusyIconPath() const
{
  return QLatin1String( ":/plugins/jabber-busy.png" );
}

QString JabberService::userStatusAwayIconPath() const
{
  return QLatin1String( ":/plugins/jabber-away.png" );
}

JabberService::JabberService()
  : QObject()
{
  setEnabled( true );
  qDebug() << "Jabber service plugin loaded";
}

Q_EXPORT_PLUGIN2( beejabber, JabberService )
