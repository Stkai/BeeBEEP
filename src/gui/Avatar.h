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
// $Id$
//
//////////////////////////////////////////////////////////////////////

#ifndef BEEBEEP_AVATAR_H
#define BEEBEEP_AVATAR_H

#include "Config.h"


class Avatar
{
public:
  Avatar();
  Avatar( const Avatar& );

  Avatar& operator=( const Avatar& );

  inline void setName( const QString& );
  inline const QString& name() const;
  inline void setColor( const QString& );
  inline const QColor& color() const;
  inline void setSize( const QSize& );
  inline const QSize& size() const;
  inline const QPixmap& pixmap() const;

  bool create();

  static QPixmap create( const QString& user_name, const QString& user_color, const QSize& icon_size );

private:
  QString m_name;
  QColor m_color;
  QSize m_size;
  QPixmap m_pixmap;

};


// Inline Functions
inline void Avatar::setName( const QString& new_value ) { m_name = new_value.simplified(); }
inline const QString& Avatar::name() const { return m_name; }
inline void Avatar::setColor( const QString& new_value ) { m_color = QColor( new_value ); }
inline const QColor& Avatar::color() const { return m_color; }
inline void Avatar::setSize( const QSize& new_value ) { m_size = new_value; }
inline const QSize& Avatar::size() const { return m_size; }
inline const QPixmap& Avatar::pixmap() const { return m_pixmap; }

#endif // BEEBEEP_AVATAR_H
