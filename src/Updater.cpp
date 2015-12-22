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
// $Id$
//
//////////////////////////////////////////////////////////////////////

#include "HttpDownloader.h"
#include "Settings.h"
#include "Updater.h"


Updater::Updater( QObject *parent )
 : QObject(parent), m_versionAvailable( "" ), m_downloadUrl( "" )
{
}

void Updater::checkForNewVersion()
{
  m_versionAvailable = "";
  m_downloadUrl = "";
  QUrl url( Settings::instance().updaterWebSite() );

  HttpDownloader* http_downloader = new HttpDownloader( this );
  connect( http_downloader, SIGNAL( downloadCompleted( const QString& ) ),this, SLOT( onDownloadCompleted( const QString& ) ), Qt::QueuedConnection );
  connect( http_downloader, SIGNAL( jobFinished() ), this, SIGNAL( jobCompleted() ) );

  http_downloader->addUrl( url );

  QTimer::singleShot( 0, http_downloader, SLOT( startDownload() ) );
}

void Updater::onDownloadCompleted( const QString& file_path )
{
  HttpDownloader* http_downloader = qobject_cast<HttpDownloader*>( sender() );
  if( !http_downloader )
  {
    qWarning() << "Updater received a signal from invalid HttpDownloader instance";
    return;
  }

  qDebug() << file_path << "download completed";
  Settings::instance().addTemporaryFilePath( file_path );

  QSettings sets( file_path, QSettings::IniFormat );
  if( !sets.allKeys().isEmpty() )
  {
    sets.beginGroup( Settings::instance().operatingSystem( false ) );
    m_versionAvailable = sets.value( "CurrentVersion", "" ).toString();
    m_downloadUrl = sets.value( "DownloadUrl", "" ).toString();
    sets.endGroup();
  }

  if( m_versionAvailable.isEmpty() )
    qWarning() << file_path << "is not valid to check new version";

  if( !m_downloadUrl.isEmpty() )
  {
    QUrl url = QUrl::fromUserInput( m_downloadUrl );
    m_downloadUrl = url.toString();
  }
}
