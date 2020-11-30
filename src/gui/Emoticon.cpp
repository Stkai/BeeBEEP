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

#include "Emoticon.h"
#include "IconManager.h"
#include "Settings.h"


Emoticon::Emoticon()
 : m_textToMatch( "" ), m_name( "" ), m_group( Emoticon::Unknown ), m_sortOrder( -1 )
{
  resetCount();
}

Emoticon::Emoticon( const QString& text_to_match, const QString& emoticon_name, int emoticon_group, int sort_order )
 : m_textToMatch( text_to_match ), m_name( emoticon_name ), m_group( emoticon_group ), m_sortOrder( sort_order )
{
  resetCount();
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
    m_count = e.m_count;
  }
  return *this;
}

QString Emoticon::sourceFolder()
{
  return Settings::instance().emoticonSourcePath().isEmpty() ? (Settings::instance().useHiResEmoticons() ? QLatin1String( ":/emojis2x" ) : QLatin1String( ":/emojis" )) : Settings::instance().emoticonSourcePath();
}

QString Emoticon::groupFolder( int group_id )
{
  switch( group_id )
  {
  case Text:
    return QLatin1String( "people" );
  case People:
    return QLatin1String( "people" );
  case Objects:
    return QLatin1String( "objects" );
  case Nature:
    return QLatin1String( "nature" );
  case Places:
    return QLatin1String( "places" );
  case Symbols:
    return QLatin1String( "symbols" );
  default:
    return QLatin1String( ":/emoticons" );
  }
}

QIcon Emoticon::groupIcon( int group_id )
{
  QString icon_file = "";
  switch( group_id )
  {
  case People:
    icon_file = QLatin1String( "1f465.png" );
    break;
  case Objects:
    icon_file = QLatin1String( "1f514.png" );
    break;
  case Nature:
    icon_file = QLatin1String( "1f338.png" );
    break;
  case Places:
    icon_file = QLatin1String( "1f698.png" );
    break;
  case Symbols:
    icon_file = QLatin1String( "1f523.png" );
    break;
  default:
    break;
  }

  return icon_file.isEmpty() ? IconManager::instance().icon( "emoticon.png" ) : QIcon( filePath( group_id, icon_file ) );
}
