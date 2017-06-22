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

#ifndef BEEBEEP_IMAGEOPTIMIZER_H
#define BEEBEEP_IMAGEOPTIMIZER_H

#include "Config.h"


class ImageOptimizer
{
// Singleton Object
  static ImageOptimizer* mp_instance;

public:
  QImage diffImage( const QImage& old_image, const QImage& new_image, QRgb diff_color = qRgba( 0, 0, 0, 0 ) ) const;
  QImage mergeImage( const QImage& old_image, const QImage& new_image, QRgb diff_color = qRgba( 0, 0, 0, 0 ) ) const;

  QByteArray saveImage( const QImage&, const QString& image_type = "png", int image_quality = -1, bool use_compression = true, int compression_level = -1 ) const;
  QImage loadImage( const QByteArray&, const QString& image_type = "png", bool use_compression = true ) const;

  //QImage applyMedianCutAlgorithm( const QImage& ) const; // too slow: 32s for a single image

  bool imageTypeHasTransparentColor( const QString& ) const;
  inline const QStringList& imageTypes() const;

  static ImageOptimizer& instance()
  {
    if( !mp_instance )
      mp_instance = new ImageOptimizer();
    return *mp_instance;
  }

  static void close()
  {
    if( mp_instance )
    {
      delete mp_instance;
      mp_instance = NULL;
    }
  }

protected:
  ImageOptimizer();

private:
  QStringList m_imageTypes;

};


// Inline Functions
inline const QStringList& ImageOptimizer::imageTypes() const { return m_imageTypes; }

#endif // BEEBEEP_IMAGEOPTIMIZER_H
