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
// Author: Marco Mastroddi <marco.mastroddi(AT)gmail.com>
//
// $Id: Protocol.h 464 2015-08-18 14:55:55Z mastroddi $
//
//////////////////////////////////////////////////////////////////////

#include "BeeApplication.h"
#include "HttpDownloader.h"
#include "Settings.h"
#include "Updater.h"


Updater::Updater( QObject *parent )
 : QObject(parent)
{

}

void Updater::checkForNewVersion()
{
  QUrl url( "" );

  HttpDownloader* http_downloader = new HttpDownloader;
  http_downloader->addUrl( url );
  connect( http_downloader, SIGNAL( downloadCompleted( const QString& ) ),this, SLOT( onDownloadCompleted( const QString& ) ) );

  BeeApplication* bee_app = (BeeApplication*)qApp;
  bee_app->addJob( http_downloader );
  QMetaObject::invokeMethod( http_downloader, "startDownload", Qt::QueuedConnection );
}

void Updater::onDownloadCompleted( const QString& file_path )
{
  HttpDownloader* http_downloader = qobject_cast<HttpDownloader*>( sender() );
  if( !http_downloader )
  {
    qWarning() << "Updater received a signal from invalid HttpDownloader instance";
    return;
  }

  BeeApplication* bee_app = (BeeApplication*)qApp;
  bee_app->removeJob( http_downloader );

  qDebug() << file_path << "download completed";

  if( http_downloader->hasQueuedDownloads() )
    http_downloader->startDownload();
  else
    http_downloader->deleteLater();
}
