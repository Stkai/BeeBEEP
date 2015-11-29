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

#include "Emoticon.h"


Emoticon::Emoticon()
 : m_textToMatch( "" ), m_name( "" ), m_group( Emoticon::Unknown ), m_sortOrder( -1 ), m_utf8( "" )
{
}

Emoticon::Emoticon( const QString& text_to_match, const QString& emoticon_name, int emoticon_group, int sort_order )
 : m_textToMatch( text_to_match ), m_name( emoticon_name ), m_group( emoticon_group ), m_sortOrder( sort_order ),
   m_utf8( text_to_match.toUtf8() )
{
}

Emoticon::Emoticon( const Emoticon& e )
{
  (void)operator=( e );
}

Emoticon& Emoticon::operator=( const Emoticon& e )
{
  if( this != &e )
  {
    m_textToMatch = e.m_textToMatch;
    m_name = e.m_name;
    m_group = e.m_group;
    m_sortOrder = e.m_sortOrder;
    m_utf8 = e.m_utf8;
  }
  return *this;
}

QString Emoticon::groupFolder( int group_id )
{
  switch( group_id )
  {
  case Text:
    return QLatin1String( "emojis/people" );
  case People:
    return QLatin1String( "emojis/people" );
  case Objects:
    return QLatin1String( "emojis/objects" );
  case Nature:
    return QLatin1String( "emojis/nature" );
  case Places:
    return QLatin1String( "emojis/places" );
  case Symbols:
    return QLatin1String( "emojis/symbols" );
  default:
    return QLatin1String( "emoticons" );
  }
}

QIcon Emoticon::groupIcon( int group_id )
{
  switch( group_id )
  {
  case People:
    return QIcon( ":/emojis/people/1f465.png" );
  case Objects:
    return QIcon( ":/emojis/objects/1f514.png" );
  case Nature:
    return QIcon( ":/emojis/nature/1f338.png" );
  case Places:
    return QIcon( ":/emojis/places/1f698.png" );
  case Symbols:
    return QIcon( ":/emojis/symbols/1f523.png" );
  default:
    return QIcon( ":/images/emoticon.png" );
  }
}
