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


ImageOptimizer* ImageOptimizer::mp_instance = NULL;


ImageOptimizer::ImageOptimizer()
{
}

QImage ImageOptimizer::diffImage( const QImage& old_image, const QImage& new_image, QRgb diff_color ) const
{
  if( old_image.isNull() )
    return new_image;

  int img_max_w = qMin( old_image.width(), new_image.width() );
  int img_max_h = qMin( old_image.height(), new_image.height() );

  QImage diff_img( QSize( img_max_w, img_max_h ), QImage::Format_ARGB32 );
  diff_img.fill( diff_color );
  QRgb c_rgb;
  bool image_is_changed = false;

  for( int y = 0; y < img_max_h; y++ )
  {
    for( int x = 0; x < img_max_w; x++ )
    {
      c_rgb = new_image.pixel( x, y );
      if( c_rgb != old_image.pixel( x, y ) )
      {
        diff_img.setPixel( x, y, c_rgb );
        if( !image_is_changed )
          image_is_changed = true;
      }
    }
  }

  if( !image_is_changed )
  {

  }
  return diff_img;
}

QImage ImageOptimizer::mergeImage( const QImage& old_image, const QImage& new_image, QRgb diff_color ) const
{
  if( old_image.isNull() )
    return new_image;

  if( new_image.isNull() )
    return old_image;

  int img_max_w = qMin( old_image.width(), new_image.width() );
  int img_max_h = qMin( old_image.height(), new_image.height() );

  QImage merged_img( QSize( img_max_w, img_max_h ), QImage::Format_ARGB32 );
  QRgb c_rgb;

  for( int y = 0; y < img_max_h; y++ )
  {
    for( int x = 0; x < img_max_w; x++ )
    {
      c_rgb = new_image.pixel( x, y );
      if( c_rgb != diff_color )
        merged_img.setPixel( x, y, c_rgb );
      else
        merged_img.setPixel( x, y, old_image.pixel( x, y ) );
    }
  }

  return merged_img;
}

QByteArray ImageOptimizer::saveImage( const QImage& img, const char* image_type, int image_quality, bool use_compression, int compression_level ) const
{
  if( img.isNull() )
  {
    qWarning() << "Invalid image found in ImageOptimizer::saveImage(...)";
    return "";
  }

  QByteArray diff_img_bytes;
  QBuffer buffer( &diff_img_bytes );
  buffer.open( QIODevice::WriteOnly );
  img.save( &buffer, image_type, image_quality );
#ifdef BEEBEEP_DEBUG
  qDebug() << "ImageOptimizer saves image (uncompressed) with size" << diff_img_bytes.size();
#endif
  if( use_compression )
  {
    diff_img_bytes = qCompress( diff_img_bytes, compression_level );
#ifdef BEEBEEP_DEBUG
    qDebug() << "ImageOptimizer saves image (compressed) with size" << diff_img_bytes.size();
#endif
  }
  return diff_img_bytes;
}

QImage ImageOptimizer::loadImage( const QByteArray& img_byte_array, const char* image_type, bool use_compression ) const
{
  QImage img;
  if( img_byte_array.isEmpty() )
    return img;
  QByteArray diff_img_bytes = use_compression ? qUncompress( img_byte_array ) :  img_byte_array;
  QBuffer buffer( &diff_img_bytes );
  buffer.open( QIODevice::ReadOnly );
  img.load( &buffer, image_type );
  return img;
}
