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
#include "TwitterService.h"
#include "Version.h"


QString TwitterService::name() const
{
  return tr( "Twitter" );
}

QString TwitterService::version() const
{
  return "0.6.1";
}

QString TwitterService::author() const
{
  return "Marco Mastroddi";
}

QString TwitterService::help() const
{
  return tr( "You can connect to the Twitter server." );
}

QIcon TwitterService::icon() const
{
  return QIcon( iconFileName() );
}

QString TwitterService::iconFileName() const
{
  return QLatin1String( ":/plugins/twitter.png" );
}

int TwitterService::priority() const
{
  return 1000;
}

QString TwitterService::coreVersion() const
{
  return QString( BEEBEEP_VERSION );
}

QString TwitterService::protocol() const
{
  return QLatin1String( "xmpp" );
}

QString TwitterService::hostAddress() const
{
  return QLatin1String( "twitter.com" );
}

int TwitterService::hostPort() const
{
  return 5222;
}

bool TwitterService::fileTransferIsEnabled() const
{
  return false;
}

QString TwitterService::userStatusOfflineIconPath() const
{
  return QLatin1String( ":/plugins/twitter-offline.png" );
}

QString TwitterService::userStatusOnlineIconPath() const
{
  return QLatin1String( ":/plugins/twitter-online.png" );
}

QString TwitterService::userStatusBusyIconPath() const
{
  return QLatin1String( ":/plugins/twitter-busy.png" );
}

QString TwitterService::userStatusAwayIconPath() const
{
  return QLatin1String( ":/plugins/twitter-away.png" );
}

TwitterService::TwitterService()
  : QObject()
{
  setEnabled( true );
  qDebug() << "Twitter service plugin loaded";
}

#if QT_VERSION >= 0x050000
Q_PLUGIN_METADATA(IID "beebeep.plugin.ServiceInterface/2.0")
#else
Q_EXPORT_PLUGIN2( beetwitter, TwitterService  )
#endif
