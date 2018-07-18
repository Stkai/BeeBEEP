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
 : m_imageTypes()
{
   m_imageTypes << QString( "jpg" ) << QString( "png" );
}

bool ImageOptimizer::imageTypeHasTransparentColor( const QString& image_type ) const
{
  return image_type != QString( "jpg" );
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
  for( int y = 0; y < img_max_h; y++ )
  {
    for( int x = 0; x < img_max_w; x++ )
    {
      c_rgb = new_image.pixel( x, y );
      if( c_rgb != old_image.pixel( x, y ) )
        diff_img.setPixel( x, y, c_rgb );
    }
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

QByteArray ImageOptimizer::saveImage( const QImage& img, const QString& image_type, int image_quality, bool use_compression, int compression_level ) const
{
  if( img.isNull() )
  {
    qWarning() << "Invalid image found in ImageOptimizer::saveImage(...)";
    return "";
  }

  QByteArray diff_img_bytes;
  QBuffer buffer( &diff_img_bytes );
  buffer.open( QIODevice::WriteOnly );
  img.save( &buffer, image_type.toLatin1().constData(), image_quality );
#ifdef BEEBEEP_DEBUG
  qDebug() << "ImageOptimizer saves image" << qPrintable( image_type ) << "(uncompressed) with size" << diff_img_bytes.size();
#endif
  if( use_compression )
  {
    diff_img_bytes = qCompress( diff_img_bytes, compression_level );
#ifdef BEEBEEP_DEBUG
    qDebug() << "ImageOptimizer compress image"<< qPrintable( image_type ) << "to size" << diff_img_bytes.size();
#endif
  }
  return diff_img_bytes;
}

QImage ImageOptimizer::loadImage( const QByteArray& img_byte_array, const QString& image_type, bool use_compression ) const
{
  QImage img;
  if( img_byte_array.isEmpty() )
    return img;
  QByteArray diff_img_bytes = use_compression ? qUncompress( img_byte_array ) :  img_byte_array;
  QBuffer buffer( &diff_img_bytes );
  buffer.open( QIODevice::ReadOnly );
  if( !img.load( &buffer, image_type.toLatin1().constData() ) )
    qWarning() << "ImageOptimizer cannot load image with size" << img_byte_array.size() << "and type" << qPrintable( image_type ) << (use_compression ? "compressed" : "not compressed");
  return img;
}

#if 0
/* Inline useful functions for Image Quantization */
static inline int pixel_distance( QRgb p1, QRgb p2 )
{
  int r1 = qRed( p1 );
  int g1 = qGreen( p1 );
  int b1 = qBlue( p1 );
  int a1 = qAlpha( p1 );

  int r2 = qRed( p2 );
  int g2 = qGreen( p2 );
  int b2 = qBlue( p2 );
  int a2 = qAlpha( p2 );

  return qAbs( r1 - r2 ) + qAbs( g1 - g2 ) + qAbs( b1 - b2 ) + qAbs( a1 - a2 );
}

static inline int closest_match( QRgb pixel, const QVector<QRgb> &clut )
{
  int idx = 0;
  int current_distance = 2147483647; // INT_MAX (signed)
  for( int i = 0; i < clut.size(); i++ )
  {
    int dist = pixel_distance( pixel, clut.at( i ) );
    if( dist < current_distance )
    {
      current_distance = dist;
      idx = i;
    }
  }
  return idx;
}
/**********************************************/
QImage ImageOptimizer::applyMedianCutAlgorithm( const QImage& img ) const
{
#ifdef BEEBEEP_DEBUG
  QElapsedTimer timer;
  timer.start();
#endif

  int max_colors = 256;
  QList<QRgb> pixels;

  // For finding color channel that has the most wide range, we need to keep their lower and upper bound.
  int lower_red = qRed( img.pixel( 0, 0 ) );
  int lower_green = qGreen( img.pixel( 0, 0 ) );
  int lower_blue = qBlue( img.pixel( 0, 0 ) );
  int upper_red   = 0;
  int upper_green = 0;
  int upper_blue  = 0;

  for( int x = 0; x < img.width(); x++ )
  {
    for( int y = 0; y < img.height(); y++ )
    {
      QRgb rgb = img.pixel( x, y ); // Get rgb data of a particular pixel
      if( !pixels.contains( rgb ) )   // If we have the same pixel, we don't need it twice or more
      {
        lower_red = qMin( lower_red, qRed( rgb ) );
        lower_green = qMin( lower_green, qGreen( rgb ) );
        lower_blue = qMin( lower_blue, qBlue( rgb ) );
        upper_red = qMax( upper_red, qRed( rgb ) );
        upper_green = qMax( upper_green, qGreen( rgb ) );
        upper_blue = qMax( upper_blue, qBlue( rgb ) );
        pixels.append( rgb );
      }
    }
  }
#ifdef BEEBEEP_DEBUG
  qDebug() << "MedianCutAlgorithm has checked the old palette in" << timer.elapsed() << "ms";
#endif

  int red = upper_red - lower_red;
  int green = upper_green - lower_green;
  int blue = upper_blue - lower_blue;
  int max = qMax( qMax( red, green ), blue );

  // We just used qSort here.
  // As comparison function, we sent a lambda function
  // that compares two rgb color according to our selected color channel.

  qSort( pixels.begin(), pixels.end(), [max,red,green,blue]( const QRgb& rgb1, const QRgb& rgb2 )
  {
    if( max == red )                       // if red is our color that has the widest range
      return qRed( rgb1 ) < qRed( rgb2 );  // just compare their red channel
    else if( max == green )                //...same
      return qGreen( rgb1 ) < qRed( rgb2 );
    else /*if (max == blue)*/
      return qBlue( rgb1 ) < qBlue( rgb2 );
  });

#ifdef BEEBEEP_DEBUG
  qDebug() << "MedianCutAlgorithm sorts colors in" << timer.elapsed() << "ms";
#endif

  // Creating color palette
  QList<QList<QRgb>> lists;
  int list_size = pixels.size() / max_colors;

  for( int i = 0; i < max_colors; i++)
  {
    QList<QRgb> list;
    for( int j = list_size * i; j < (list_size * i) + list_size; j++ )
      list.append( pixels.at( j ) );
    if( !list.isEmpty() )
      lists.append( list );
  }
#ifdef BEEBEEP_DEBUG
  qDebug() << "MedianCutAlgorithm has created new" << lists.size() << "lists of colors in" << timer.elapsed() << "ms";
#endif

  QVector<QRgb> new_palette;
  for( QList<QRgb> list: lists )
  {
    if( !list.isEmpty() )
      new_palette.append( list.at( list.size() / 2 ) );
  }
#ifdef BEEBEEP_DEBUG
  qDebug() << "MedianCutAlgorithm has created new palette with" << new_palette.size() << "colors in" << timer.elapsed() << "ms";
#endif

  QImage new_img( img.width(), img.height(), QImage::Format_ARGB32 );
  for( int x = 0; x < img.width(); x++ )
  {
    for( int y = 0; y < img.height(); ++y )
      new_img.setPixel( x, y, new_palette[ closest_match( img.pixel( x, y ), new_palette ) ] );
  }

#ifdef BEEBEEP_DEBUG
  qDebug() << "MedianCutAlgorithm has worked for" << timer.elapsed() << "ms";
#endif
  return new_img;
}
#endif
