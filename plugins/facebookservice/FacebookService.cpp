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
#include "FacebookService.h"
#include "Version.h"


QString FacebookService::name() const
{
  return tr( "Facebook" );
}

QString FacebookService::version() const
{
  return "0.6.1";
}

QString FacebookService::author() const
{
  return "Marco Mastroddi";
}

QString FacebookService::help() const
{
  return tr( "You can connect to the Facebook chat." );
}

QIcon FacebookService::icon() const
{
  return QIcon( iconFileName() );
}

QString FacebookService::iconFileName() const
{
  return QLatin1String( ":/plugins/facebook.png" );
}

int FacebookService::priority() const
{
  return 1000;
}

QString FacebookService::coreVersion() const
{
  return QString( BEEBEEP_VERSION );
}

QString FacebookService::protocol() const
{
  return QLatin1String( "xmpp" );
}

QString FacebookService::hostAddress() const
{
  return QLatin1String( "chat.facebook.com" );
}

int FacebookService::hostPort() const
{
  return 5222;
}

bool FacebookService::fileTransferIsEnabled() const
{
  return false;
}

QString FacebookService::userStatusOfflineIconPath() const
{
  return QLatin1String( ":/plugins/facebook-offline.png" );
}

QString FacebookService::userStatusOnlineIconPath() const
{
  return QLatin1String( ":/plugins/facebook-online.png" );
}

QString FacebookService::userStatusBusyIconPath() const
{
  return QLatin1String( ":/plugins/facebook-busy.png" );
}

QString FacebookService::userStatusAwayIconPath() const
{
  return QLatin1String( ":/plugins/facebook-away.png" );
}

FacebookService::FacebookService()
  : QObject()
{
  setEnabled( true );
  qDebug() << "Facebook service plugin loaded";
}

#if QT_VERSION >= 0x050000
Q_PLUGIN_METADATA(IID "beebeep.plugin.ServiceInterface/2.0")
#else
Q_EXPORT_PLUGIN2( beefacebook, FacebookService )
#endif
