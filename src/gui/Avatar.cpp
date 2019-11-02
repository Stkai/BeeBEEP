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

#include "Avatar.h"
#include "BeeUtils.h"
#include "IconManager.h"


Avatar::Avatar()
  : m_name(), m_color(), m_size(), m_pixmap()
{}

Avatar::Avatar( const Avatar& av )
{
  (void)operator=( av );
}

Avatar& Avatar::operator=( const Avatar& av )
{
  if( this != &av )
  {
    m_name = av.m_name;
    m_color = av.m_color;
    m_size = av.m_size;
    m_pixmap = av.m_pixmap;
  }
  return *this;
}

bool Avatar::create()
{
  if( m_name.isEmpty() )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Unable to create avatar: name is empty";
#endif
    return false;
  }

  if( !m_color.isValid() )
    m_color = QColor( Qt::gray );

  if( m_size.isNull() || m_size.isEmpty() )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Unable to create avatar: size is null or empty";
#endif
    return false;
  }

  if( m_size.width() < 17 || m_size.height() < 17 )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Unable to create avatar: size is to small" << m_size.width() << m_size.height();
#endif
    return false;
  }

  QString converted_name = "";
  for( int i = 0; i < m_name.size(); i++ )
  {
    QChar c = m_name.at( i );
    if( c.isLetterOrNumber() || c.isSpace() )
      converted_name += c;
  }

  if( converted_name.isEmpty() )
    converted_name = "x x";

  QStringList sl_name = converted_name.simplified().split( " ", QString::SkipEmptyParts );
  if( sl_name.isEmpty() )
    sl_name << "x" << "x";

  QString text_to_write;
  if( sl_name.size() == 1 )
  {
    text_to_write = sl_name.first();

    if( text_to_write.size() > 2 )
      text_to_write.truncate( 2 );

    text_to_write = Bee::capitalizeFirstLetter( text_to_write, false );
  }
  else
  {
    text_to_write = sl_name.first().at( 0 ).toUpper();
    sl_name.removeFirst();
    text_to_write += sl_name.first().at( 0 ).toUpper() ;
  }

  QFont f( QApplication::font().family(), qMin( 128, m_size.height() ) );
  f.setStyleHint( QFont::TypeWriter );
  f.setBold( true );
  QFontMetrics fm( f );
  int w_min_max_size = m_size.width() >= 24 ? 8 : m_size.width() >= 16 ? 4 : 3;
  int w_max_size = m_size.width() - qMax( w_min_max_size, static_cast<int>(m_size.width() / 6) );

  while( fm.width( text_to_write ) > w_max_size )
  {
    f.setPointSize( f.pointSize() - 2 );
    fm = QFontMetrics( f );
    if( f.pointSize() < 6 )
    {
#ifdef BEEBEEP_DEBUG
      qDebug() << "Unable to create avatar: font is too small for this size" << m_size.width() << m_size.height();
#endif
      return false;
    }
  }

  int border_x = (m_size.width() - fm.width( text_to_write )) / 2;
  int border_y = (m_size.height() - fm.height()) / 2;

  QPixmap pix( m_size );
  pix.fill( m_color );
  QPainter p( &pix );
  p.setFont( f );
  p.setRenderHint( QPainter::TextAntialiasing );

  QColor pen_color = Bee::isColorNear( m_color, Qt::white ) ? Bee::invertColor( m_color ) : Qt::white;
  QPen pen( pen_color );
  p.setPen( pen );
  p.drawText( QRect( border_x, border_y, m_size.width() - border_x, m_size.height() - border_y ), text_to_write );

  m_pixmap = pix;

  return true;
}

QPixmap Avatar::create( const QString& user_name, const QString& user_color, const QSize& icon_size )
{
  Avatar av;
  av.setName( user_name );
  av.setColor( user_color );
  av.setSize( icon_size );

  if( av.create() )
    return av.pixmap();
  else
    return IconManager::instance().icon( "beebeep.png" ).pixmap( icon_size );
}
