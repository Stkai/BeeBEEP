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

#ifndef BEEBEEP_GANALYTICS_H
#define BEEBEEP_GANALYTICS_H

#include "Config.h"


class GAnalytics: public QObject
{
  Q_OBJECT

public:
  explicit GAnalytics( QObject* parent = Q_NULLPTR );

public slots:
  void doPost();

signals:
  void jobFinished();

protected slots:
  void onReplyFinished( QNetworkReply* );
  void onSslErrors( const QList<QSslError>& );

private:
  QNetworkAccessManager* mp_manager;

};

#endif // BEEBEEP_DOWNLOADER_H
