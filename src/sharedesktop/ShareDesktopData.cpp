//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2020 Marco Mastroddi
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

#include "ShareDesktopData.h"

ShareDesktopData::ShareDesktopData()
 : m_imageData( "" ), m_imageType( "jpg" ), m_isCompressed( true ), m_diffColor( qRgba( 0, 0, 0, 0 ) )
{

}

ShareDesktopData::ShareDesktopData( const QByteArray& image_data, const QString& image_type,
                                    bool is_compressed, unsigned int diff_color )
 : m_imageData( image_data ), m_imageType( image_type ), m_isCompressed( is_compressed ), m_diffColor( diff_color )
{
}

ShareDesktopData::ShareDesktopData( const ShareDesktopData& sdd )
{
  (void)operator=( sdd );
}

ShareDesktopData& ShareDesktopData::operator=( const ShareDesktopData& sdd )
{
  if( this != &sdd )
  {
    m_imageData = sdd.m_imageData;
    m_imageType = sdd.m_imageType;
    m_isCompressed = sdd.m_isCompressed;
    m_diffColor = sdd.m_diffColor;
  }
  return *this;
}
