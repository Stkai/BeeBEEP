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

#ifndef BEEBEEP_SCREENSHOT_H
#define BEEBEEP_SCREENSHOT_H

#include "Config.h"


class Screenshot : public QObject
{
  Q_OBJECT

public:
  Screenshot( QObject* parent = Q_NULLPTR );

  inline void reset();
  inline bool isValid() const;

  inline const QPixmap& toPixmap() const;
  inline QImage toImage() const;

  bool grabWidget( QWidget* );
  bool grabScreen( QWidget* );

  bool save( const QString& file_name, const char* img_format = 0, int img_quality = -1 );

public slots:
  void grabPrimaryScreen();

protected:
  void checkPixelRatio();

private:
  QPixmap m_pixmap;

};

// Inline Functions
inline void Screenshot::reset() { m_pixmap = QPixmap(); }
inline bool Screenshot::isValid() const { return !m_pixmap.isNull(); }
inline const QPixmap& Screenshot::toPixmap() const { return m_pixmap; }
inline QImage Screenshot::toImage() const { return m_pixmap.toImage(); }

#endif // BEEBEEP_SCREENSHOT_H
