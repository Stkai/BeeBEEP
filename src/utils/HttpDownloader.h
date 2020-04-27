//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2020 Marco Mastroddi
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
// along with BeeBEEP. If not, see <http://www.gnu.org/licenses/>.
//
// Author: Marco Mastroddi <marco.mastroddi(AT)gmail.com>
//
// $Id$
//
//////////////////////////////////////////////////////////////////////

#ifndef BEEBEEP_DOWNLOADER_H
#define BEEBEEP_DOWNLOADER_H

#include "Config.h"
class QSslError;


class HttpDownloader: public QObject
{
  Q_OBJECT

public:
  explicit HttpDownloader( QObject* parent = Q_NULLPTR );

  inline void addUrl( const QUrl& );
  inline const QStringList& downloadedFilePaths() const;
  inline void setOverwriteExistingFiles( bool );
  void cleanUp();

public slots:
  void startDownload();

signals:
  void downloadCompleted( const QString& );
  void jobFinished();

protected slots:
  void onReplyFinished( QNetworkReply* );
  void onSslErrors( const QList<QSslError>& );
  void onDownloadProgress( qint64, qint64 );

protected:
  QString filePathFromFileName( const QString& );
  QString filePathFromUrl( const QUrl& );
  QString fileNameFromUrl( const QUrl& );
  void doDownload( const QUrl& );
  bool saveToDisk( const QString&, QIODevice* );

private:
  QNetworkAccessManager* mp_manager;
  QList<QUrl> m_queuedUrls;
  QStringList m_downloadedFilePaths;
  bool m_overwriteExistingFiles;

};

// Inline Functions
inline void HttpDownloader::addUrl( const QUrl& new_value ) { m_queuedUrls.append( new_value ); }
inline const QStringList& HttpDownloader::downloadedFilePaths() const { return m_downloadedFilePaths; }
inline void HttpDownloader::setOverwriteExistingFiles( bool new_value ) { m_overwriteExistingFiles = new_value; }

#endif // BEEBEEP_DOWNLOADER_H
