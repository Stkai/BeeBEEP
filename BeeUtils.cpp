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
// Author: Marco Mastroddi (marco.mastroddi(AT)gmail.com)
//
// $Id$
//
//////////////////////////////////////////////////////////////////////

#include "BeeUtils.h"
#include "Chat.h"
#include "EmoticonManager.h"
#include "ChatMessage.h"
#include "Settings.h"


QString Bee::Private::formatHtmlText( QString text )
{
  text.replace( "<3", "-heart-emoticon-" ); // save the heart emoticon
  QString text_formatted = "";

  for( int i = 0; i < text.length(); i++ )
  {
    if( text.at( i ) == QLatin1Char( '<' ) )
      text_formatted += QLatin1String( "&lt;" );
    else if(text.at( i ) == QLatin1Char( '>' ) )
      text_formatted += QLatin1String( "&gt;" );
    else if(text.at( i ) == QLatin1Char( '"' ) )
      text_formatted += QLatin1String( "&quot;" );
    else if(text.at( i ) == QLatin1Char( '&' ) )
      text_formatted += QLatin1String( "&amp;" );
    else
      text_formatted += text.at( i );
  }

  text_formatted.replace( QRegExp("(^|\\s|>)_(\\S+)_(<|\\s|$)"), "\\1<u>\\2</u>\\3" );
  text_formatted.replace( QRegExp("(^|\\s|>)\\*(\\S+)\\*(<|\\s|$)"), "\\1<b>\\2</b>\\3" );
  text_formatted.replace( QRegExp("(^|\\s|>)\\/(\\S+)\\/(<|\\s|$)"), "\\1<i>\\2</i>\\3" );

  text_formatted.replace( "-heart-emoticon-", "<3" ); // save the heart emoticon

  text = EmoticonManager::instance().parseEmoticons( text_formatted );
  return text.replace( "\n", "<br />" );
}

QString Bee::formatMessage( const ChatMessage& cm )
{
  QString text_formatted = Private::formatHtmlText( cm.message().text() );
  if( cm.message().data().size() > 0 )
  {
    QColor c( cm.message().data() );
    if( c.isValid() )
    {
      text_formatted.prepend( QString( "<font color='%1'>" ).arg( c.name() ) );
      text_formatted.append( QString( "</font>" ) );
    }
  }
  QString sHtmlMessage = QString( "%1<b>%2</b>%3%4" )
            .arg( Settings::instance().chatShowMessageTimestamp() ? cm.message().timestamp().toString( "(hh:mm:ss) " ) : "" )
            .arg( Settings::instance().showUserNickname() ? cm.nickname() : cm.username() )
            .arg( Settings::instance().chatCompact() ? ":&nbsp;" : ":<br />" )
            .arg( text_formatted );
  if( Settings::instance().chatAddNewLineToMessage() )
    sHtmlMessage.prepend( "<br />" );
  return sHtmlMessage;
}

QString Bee::formatSystemMessage( const ChatMessage& cm )
{
  QString sHtmlMessage = QString( "%1<font color=gray>%2</font>" )
            .arg( Settings::instance().chatShowMessageTimestamp() ? cm.message().timestamp().toString( "(hh:mm:ss) " ) : "" )
            .arg( cm.message().text() );
  if( Settings::instance().chatAddNewLineToMessage() )
    sHtmlMessage.prepend( "<br />" );
  return sHtmlMessage;
}

QString Bee::chatMessagesToText( const Chat& c )
{
  QString s = "";
  foreach( ChatMessage cm, c.messages() )
  {
    if( cm.isSystem() )
      s += formatSystemMessage( cm );
    else
      s += formatMessage( cm );
    s += "<br />";
  }
  return s;
}

QString Bee::userStatusToString( int user_status )
{
  switch( user_status )
  {
  case User::Offline:
    return QT_TR_NOOP( "offline" );
  case User::Online:
    return QT_TR_NOOP( "online" );
  case User::Busy:
    return QT_TR_NOOP( "busy" );
  case User::Away:
    return QT_TR_NOOP( "away" );
  default:
    return "";
  }
}
