//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2021 Marco Mastroddi
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
// $Id: ShareDesktopJob.h 933 2017-06-21 17:56:45Z mastroddi $
//
//////////////////////////////////////////////////////////////////////

#ifndef BEEBEEP_SHAREDESKTOPDATA_H
#define BEEBEEP_SHAREDESKTOPDATA_H

#include "Config.h"


class ShareDesktopData
{
public:
  ShareDesktopData();
  ShareDesktopData( const QByteArray&, const QString&, bool, unsigned int );
  ShareDesktopData( const ShareDesktopData& );
  ShareDesktopData& operator=( const ShareDesktopData& );

  inline bool isEmpty() const;
  inline void setImageData( const QByteArray& );
  inline const QByteArray& imageData() const;
  inline void setImageType( const QString& );
  inline const QString& imageType() const;
  inline void setIsCompressed( bool );
  inline bool isCompressed() const;
  inline void setDiffColor( unsigned int );
  inline unsigned int diffColor() const;

private:
  QByteArray m_imageData;
  QString m_imageType;
  bool m_isCompressed;
  unsigned int m_diffColor;

};


// Inline functions
inline bool ShareDesktopData::isEmpty() const { return m_imageData.isEmpty(); }
inline void ShareDesktopData::setImageData( const QByteArray& new_value ) { m_imageData = new_value; }
inline const QByteArray& ShareDesktopData::imageData() const { return m_imageData; }
inline void ShareDesktopData::setImageType( const QString& new_value ) { m_imageType = new_value; }
inline const QString& ShareDesktopData::imageType() const { return m_imageType; }
inline void ShareDesktopData::setIsCompressed( bool new_value ) { m_isCompressed = new_value; }
inline bool ShareDesktopData::isCompressed() const { return m_isCompressed; }
inline void ShareDesktopData::setDiffColor( unsigned int new_value ) { m_diffColor = new_value; }
inline unsigned int ShareDesktopData::diffColor() const { return m_diffColor; }



#endif // BEEBEEP_SHAREDESKTOPDATA_H
