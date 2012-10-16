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
#include "GTalkService.h"
#include "Version.h"


QString GTalkService::name() const
{
  return tr( "GTalk" );
}

QString GTalkService::version() const
{
  return "0.6.1";
}

QString GTalkService::author() const
{
  return "Marco Mastroddi";
}

QString GTalkService::help() const
{
  return tr( "You can connect to the Google chat." );
}

QIcon GTalkService::icon() const
{
  return QIcon( iconFileName() );
}

QString GTalkService::iconFileName() const
{
  return QLatin1String( ":/plugins/gtalk.png" );
}

int GTalkService::priority() const
{
  return 1000;
}

QString GTalkService::coreVersion() const
{
  return QString( BEEBEEP_VERSION );
}

QString GTalkService::protocol() const
{
  return QLatin1String( "xmpp" );
}

QString GTalkService::hostAddress() const
{
  return QLatin1String( "talk.google.com" );
}

int GTalkService::hostPort() const
{
  return 5222;
}

bool GTalkService::fileTransferIsEnabled() const
{
  return false;
}

QString GTalkService::userStatusOfflineIconPath() const
{
  return QLatin1String( ":/plugins/gtalk-offline.png" );
}

QString GTalkService::userStatusOnlineIconPath() const
{
  return QLatin1String( ":/plugins/gtalk-online.png" );
}

QString GTalkService::userStatusBusyIconPath() const
{
  return QLatin1String( ":/plugins/gtalk-busy.png" );
}

QString GTalkService::userStatusAwayIconPath() const
{
  return QLatin1String( ":/plugins/gtalk-away.png" );
}

GTalkService::GTalkService()
  : QObject()
{
  setEnabled( true );
  qDebug() << "GTalk service plugin loaded";
}

Q_EXPORT_PLUGIN2( beegtalk, GTalkService )
