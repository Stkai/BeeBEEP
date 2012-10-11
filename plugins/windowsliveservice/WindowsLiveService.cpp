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
// $Id: WindowsLiveService.cpp 136 2011-11-16 16:16:26Z mastroddi $
//
//////////////////////////////////////////////////////////////////////

#include <QtPlugin>
#include <QtDebug>
#include "WindowsLiveService.h"
#include "Version.h"


QString WindowsLiveService::name() const
{
  return tr( "WindowsLive" );
}

QString WindowsLiveService::version() const
{
  return "0.6.1";
}

QString WindowsLiveService::author() const
{
  return "Marco Mastroddi";
}

QString WindowsLiveService::help() const
{
  return tr( "You can connect to the Windows Live Messenger." );
}

QIcon WindowsLiveService::icon() const
{
  return QIcon( iconFileName() );
}

QString WindowsLiveService::iconFileName() const
{
  return QLatin1String( ":/plugins/windowslive.png" );
}

int WindowsLiveService::priority() const
{
  return 1000;
}

QString WindowsLiveService::coreVersion() const
{
  return QString( BEEBEEP_VERSION );
}

QString WindowsLiveService::protocol() const
{
  return QLatin1String( "xmpp" );
}

QString WindowsLiveService::hostAddress() const
{
  return QLatin1String( "xmpp.messenger.live.com" );
}

int WindowsLiveService::hostPort() const
{
  return 5222;
}

QString WindowsLiveService::domain() const
{
  return QLatin1String( "messenger.live.com" );
}

bool WindowsLiveService::useSASLAuthentication()
{
  return true;
}

bool WindowsLiveService::ignoreSslErrors() const
{
  return true;
}

int WindowsLiveService::streamSecurityMode() const
{
  return 2; // TLSRequired
}

int WindowsLiveService::nonSASLAuthMechanism() const
{
  return 1;
}

QString WindowsLiveService::sASLAuthMechanism() const
{
  return QLatin1String( "X-MESSENGER-OAUTH2" );
}

bool WindowsLiveService::fileTransferIsEnabled() const
{
  return false;
}

QString WindowsLiveService::userStatusOfflineIconPath() const
{
  return QLatin1String( ":/plugins/windowslive-offline.png" );
}

QString WindowsLiveService::userStatusOnlineIconPath() const
{
  return QLatin1String( ":/plugins/windowslive-online.png" );
}

QString WindowsLiveService::userStatusBusyIconPath() const
{
  return QLatin1String( ":/plugins/windowslive-busy.png" );
}

QString WindowsLiveService::userStatusAwayIconPath() const
{
  return QLatin1String( ":/plugins/windowslive-away.png" );
}

WindowsLiveService::WindowsLiveService()
  : QObject()
{
  setEnabled( true );
  qDebug() << "WindowsLive service plugin loaded";
}

Q_EXPORT_PLUGIN2( beewindowslive, WindowsLiveService )
