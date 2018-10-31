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

#ifndef BEEBEEP_UPDATER_H
#define BEEBEEP_UPDATER_H

#include "Config.h"

class Updater : public QObject
{
  Q_OBJECT

public:
  explicit Updater( QObject *parent = Q_NULLPTR );

  inline const QString& versionAvailable() const;
  inline const QString& downloadUrl() const;
  inline const QString& news() const;

signals:
  void newVersionAvailable( const QString& );
  void jobCompleted();

public slots:
  void checkForNewVersion();

protected slots:
  void onDownloadCompleted( const QString& );

private:
  QString m_versionAvailable;
  QString m_downloadUrl;
  QString m_news;

};

// Inline Functions
inline const QString& Updater::versionAvailable() const { return m_versionAvailable; }
inline const QString& Updater::downloadUrl() const { return m_downloadUrl; }
inline const QString& Updater::news() const { return m_news; }

#endif // BEEBEEP_UPDATER_H
