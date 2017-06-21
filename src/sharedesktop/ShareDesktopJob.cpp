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

#include "ImageOptimizer.h"
#include "ShareDesktopJob.h"


ShareDesktopJob::ShareDesktopJob( QObject *parent )
  : QObject( parent ), m_lastImage()
{
  setObjectName( "ShareDesktopJob" );
}

void ShareDesktopJob::processNewImage( const QImage& new_image )
{
  QByteArray diff_image_data = "";
  QRgb diff_color = qRgba( 0, 0, 0, 0 ); // transparent color
  QString image_type = "png";
  int image_quality = 100;
  bool use_compression = true;
  int compression_level = 9;

  if( !new_image.isNull() )
  {
    QImage diff_image = ImageOptimizer::instance().diffImage( m_lastImage, new_image, diff_color );
    diff_image_data = ImageOptimizer::instance().saveImage( diff_image, image_type.toLatin1().constData(), image_quality, use_compression, compression_level );
    m_lastImage = new_image;
  }
  else
    qWarning() << "Share desktop has found an invalid screen image";

  emit imageDataAvailable( diff_image_data, "png", use_compression, diff_color );
}
