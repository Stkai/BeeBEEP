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

#include "Screenshot.h"


Screenshot::Screenshot( QObject* parent )
 : QObject( parent ), m_pixmap()
{
  setObjectName( "Screenshot" );
}

bool Screenshot::grabWidget( QWidget* w )
{
  reset();
  if( !w )
    return false;
 #if QT_VERSION >= 0x050000
  const QWindow *window = w->windowHandle();
  if( window )
    m_pixmap = window->screen()->grabWindow( w->winId() );
 #else
  m_pixmap = QPixmap::grabWindow( w->winId(), 0, 0, w->width(), w->height() );
 #endif
  checkPixelRatio();
  return isValid();
}

void Screenshot::grabPrimaryScreen()
{
#if QT_VERSION >= 0x050000
  reset();
  QScreen* primary_screen = QApplication::primaryScreen();
  if( primary_screen )
  {
    m_pixmap = primary_screen->grabWindow( 0 );
    checkPixelRatio();
  }
#else
  grabWidget( QApplication::desktop() );
#endif
}

void Screenshot::checkPixelRatio()
{
#if QT_VERSION >= 0x050000
  if( isValid() )
  {
    qreal device_pixel_ratio = qApp->devicePixelRatio();
    if( device_pixel_ratio > 1.0 )
    {
      m_pixmap.setDevicePixelRatio( device_pixel_ratio );
      m_pixmap = m_pixmap.scaled( QApplication::desktop()->width(), QApplication::desktop()->height(), Qt::KeepAspectRatio );
    }
  }
#endif
}

bool Screenshot::save( const QString& file_name, const char* img_format, int img_quality )
{
  return m_pixmap.save( file_name, img_format, img_quality );
}
