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
  QString text_formatted = "";

  for( int i = 0; i < text.length(); i++ )
  {
    if( text.at( i ) == QLatin1Char( ' ' ) )
    {
      if( (i + 1) < text.length() && text.at( (i+1) ) == QLatin1Char( ' ' ) )
        text_formatted += QLatin1String( "&nbsp;" );
      else
        text_formatted += QLatin1Char( ' ' );
    }
    else if( text.at( i ) == QLatin1Char( '\n' ) )
      text_formatted += QLatin1String( "<br />" );
    else if( text.at( i ) == QLatin1Char( '<' ) )
    {
      if( (i + 1) < text.length() && text.at( (i+1) ) == QLatin1Char( '3' ) )
        text_formatted += QLatin1Char( '<' ); // save the heart emoticon
      else
        text_formatted += QLatin1String( "&lt;" );
    }
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

  return EmoticonManager::instance().parseEmoticons( text_formatted );
}

QString Bee::chatMessageToText( const ChatMessage& cm )
{
  QString s;
  if( cm.isSystem() )
    s = Private::formatSystemMessage( cm );
  else
    s = Private::formatMessage( cm );
  if( Settings::instance().chatAddNewLineToMessage() )
    s.append( "<br />" );
  s.append( "<br />" );
  return s;
}

QString Bee::Private::formatMessage( const ChatMessage& cm )
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
  QString sHtmlMessage = QString( "%1<font color='%2'><b>%3</b>%4%5</font>" )
            .arg( Settings::instance().chatShowMessageTimestamp() ? QString( "<font color=gray>%1</font> " ).arg( cm.message().timestamp().toString( "(hh:mm:ss)" ) ) : "" )
            .arg( cm.defaultColor() )
            .arg( Settings::instance().showUserNickname() ? cm.nickname() : cm.username() )
            .arg( Settings::instance().chatCompact() ? ":&nbsp;" : ":<br />" )
            .arg( text_formatted );
  return sHtmlMessage;
}

QString Bee::Private::formatSystemMessage( const ChatMessage& cm )
{
  QString sHtmlMessage = QString( "<font color=gray>%1 %2</font>" )
            .arg( Settings::instance().chatShowMessageTimestamp() ? cm.message().timestamp().toString( "(hh:mm:ss) " ) : "" )
            .arg( cm.message().text() );
  return sHtmlMessage;
}

QString Bee::chatMessagesToText( const Chat& c )
{
  QString s = "";
  foreach( ChatMessage cm, c.messages() )
  {
    s += chatMessageToText( cm );
  }
  return s;
}

QString Bee::userStatusIconFileName( int user_status )
{
  switch( user_status )
  {
  case User::Offline:
    return QString( ":/images/user-offline.png" );
  case User::Online:
    return QString( ":/images/user-online.png" );
  case User::Busy:
    return QString( ":/images/user-busy.png" );
  case User::Away:
  default:
    return QString( ":/images/user-away.png" );
  }
}

QString Bee::bytesToString( FileSizeType bytes, int precision )
{
  QString suffix;
  double result = 0;
  if( bytes > 1000000000 )
  {
    suffix = "Gb";
    result = bytes / 1000000000.0;
  }
  else if( bytes > 1000000 )
  {
    suffix = "Mb";
    result = bytes / 1000000.0;
  }
  else if( bytes > 1000 )
  {
    suffix = "kb";
    result = bytes / 1000.0;
  }
  else
  {
    suffix = "bytes";
    result = bytes;
  }
  return QString( "%1 %2").arg( result, 0, 'f', precision ).arg( suffix );
}
