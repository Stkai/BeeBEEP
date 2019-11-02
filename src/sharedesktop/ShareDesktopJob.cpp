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

#include "ImageOptimizer.h"
#include "Settings.h"
#include "ShareDesktopJob.h"


ShareDesktopJob::ShareDesktopJob( QObject *parent )
  : QObject( parent ), m_lastImage()
{
  setObjectName( "ShareDesktopJob" );
}

void ShareDesktopJob::processNewImage( const QImage& new_image )
{
  QByteArray image_data = "";
  QRgb diff_color = qRgba( 0, 0, 0, 0 ); // transparent color
  QString image_type = Settings::instance().shareDesktopImageType();
  int image_quality = Settings::instance().shareDesktopImageQuality();
  bool use_compression = true;
  int compression_level = 9;

#ifdef BEEBEEP_DEBUG
  qDebug() << "Processing new share desktop image with type" << qPrintable( image_type ) << "and quality" << image_quality;
#endif
  if( !new_image.isNull() )
  {
    if( ImageOptimizer::instance().imageTypeHasTransparentColor( image_type ) )
    {
      QImage diff_image = ImageOptimizer::instance().diffImage( m_lastImage, new_image, diff_color );
      image_data = ImageOptimizer::instance().saveImage( diff_image, image_type, image_quality, use_compression, compression_level );
      m_lastImage = new_image;
    }
    else
      image_data = ImageOptimizer::instance().saveImage( new_image, image_type, image_quality, use_compression, compression_level );
  }
  else
    qWarning() << "Share desktop has found an invalid screen image";

  emit imageDataAvailable( image_data, image_type, use_compression, diff_color );
}
