//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2019 Marco Mastroddi
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

#ifndef BEEBEEP_SHAREDESKTOPJOB_H
#define BEEBEEP_SHAREDESKTOPJOB_H

#include "Config.h"


class ShareDesktopJob : public QObject
{
  Q_OBJECT

public:
  explicit ShareDesktopJob( QObject* parent = Q_NULLPTR );

signals:
  void imageDataAvailable( const QByteArray&, const QString& image_type, bool use_compression, unsigned int diff_color );

public slots:
  void processNewImage(  const QImage& );

private:
  QImage m_lastImage;

};


#endif // BEEBEEP_SHAREDESKTOPJOB_H
