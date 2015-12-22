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

#include "BeeUtils.h"
#include "HttpDownloader.h"
#include "Settings.h"


HttpDownloader::HttpDownloader( QObject* parent )
 : QObject( parent ), m_queuedUrls(), m_downloadedFilePaths()
{
  setObjectName( "HttpDownloader" );
  mp_manager = new QNetworkAccessManager( this );
  connect( mp_manager, SIGNAL( finished( QNetworkReply* ) ), this, SLOT( onReplyFinished( QNetworkReply* ) ) );
}

QString HttpDownloader::fileNameFromUrl( const QUrl& url )
{
  QString url_path = url.path();
  return QFileInfo( url_path ).fileName();
}

QString HttpDownloader::filePathFromUrl( const QUrl& url )
{
  QString file_name = fileNameFromUrl( url );
  QString file_path_tmp = Bee::convertToNativeFolderSeparator( QString( "%1/%2" ).arg( Settings::instance().downloadDirectory() ).arg( file_name ) );
  return Bee::uniqueFilePath( file_path_tmp );
}

void HttpDownloader::startDownload()
{
  if( m_queuedUrls.isEmpty() )
  {
    mp_manager->disconnect();
    emit jobFinished();
    return;
  }

  doDownload( m_queuedUrls.takeFirst() );
}

void HttpDownloader::doDownload( const QUrl& url )
{
  qDebug() << qPrintable( objectName() ) << "gets url:" << qPrintable( url.toString() );

  QList<QNetworkProxy> proxy_list = QNetworkProxyFactory::systemProxyForQuery( QNetworkProxyQuery( url ) );
  if( !proxy_list.isEmpty() )
  {
    QNetworkProxy np = proxy_list.takeFirst();
    if( np.type() != QNetworkProxy::NoProxy )
    {
      qDebug() << qPrintable( objectName() ) << "uses network proxy" << qPrintable( np.hostName() ) << ":" << np.port();
      mp_manager->setProxy( np );
    }
  }

  QNetworkRequest request;
  request.setUrl( url );
  request.setRawHeader( "User-Agent", Settings::instance().httpUserAgent().toLatin1() );

  QNetworkReply *reply = mp_manager->get( request );
  //reply->ignoreSslErrors();

  connect( reply, SIGNAL( downloadProgress( qint64, qint64 ) ), this, SLOT( onDownloadProgress( qint64, qint64 ) ) );
#ifndef QT_NO_SSL
  connect( reply, SIGNAL( sslErrors( const QList<QSslError>& ) ), SLOT( onSslErrors( const QList<QSslError>& ) ) );
#endif
}

void HttpDownloader::onDownloadProgress( qint64 bytes_received, qint64 bytes_total )
{
  QNetworkReply *reply = qobject_cast<QNetworkReply*>( sender() );
  if( !reply )
  {
    qWarning() << qPrintable( objectName() ) << "received a signal from invalid QNetworkReply instance";
    return;
  }

  if( bytes_received > 0 )
  {
#ifdef BEEBEEP_DEBUG
    QString file_name = fileNameFromUrl( reply->url() );
    qDebug() << qPrintable( file_name ) << "-> downloading" << bytes_received << "of" << bytes_total << "bytes";
#endif;
  }
}

void HttpDownloader::onReplyFinished( QNetworkReply *reply )
{
  QUrl url = reply->url();
  qDebug() << qPrintable( objectName() ) << "has finished with url:" << qPrintable( url.toString() );
  QString file_path = "";

  if( reply->error() != QNetworkReply::NoError )
  {
    qWarning() << qPrintable( url.toString() ) << "has error:" << reply->errorString();
  }
  else
  {
    file_path = filePathFromUrl( url );
    if( saveToDisk( file_path, reply ) )
      qDebug() << "Url" << url.toString() << "saved to" << qPrintable( file_path );
    else
      file_path = "";
  }

  if( !file_path.isEmpty() )
  {
    m_downloadedFilePaths.append( file_path );
    emit downloadCompleted( file_path );
  }

  reply->deleteLater();

  QTimer::singleShot( 0, this, SLOT( startDownload() ) );
}

bool HttpDownloader::saveToDisk( const QString& file_path, QIODevice *io_device )
{
  QFile file( file_path );

  if( !file.open( QIODevice::WriteOnly ) )
  {
    qWarning() << "Could not open" << file_path << "for writing:" << qPrintable( file.errorString() );
    return false;
  }

  file.write( io_device->readAll() );
  file.close();
  return true;
}

void HttpDownloader::onSslErrors( const QList<QSslError>& ssl_errors )
{
#ifndef QT_NO_SSL
  foreach( QSslError ssl_error, ssl_errors )
    qWarning() << qPrintable( objectName() ) << "has found SSL error:" << qPrintable( ssl_error.errorString() );
#else
  Q_UNUSED( ssl_errors );
#endif
}
