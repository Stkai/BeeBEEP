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

#include "GuiChatMessage.h"
#include "ChatMessage.h"
#include "EmoticonManager.h"
#include "Settings.h"
#include "PluginManager.h"
#include "Protocol.h"
#include "UserManager.h"


QString GuiChatMessage::linkify( QString text )
{
  if( !text.contains( QLatin1Char( '.' ) ) )
    return text;
  text.prepend( " " ); // for matching www.miosito.it
  text.replace( QRegExp( "(((f|ht){1}tp(s:|:){1}//)[-a-zA-Z0-9@:%_\\+.,~#?&//=\\(\\)]+)" ), "<a href=\"\\1\">\\1</a>" );
  text.replace( QRegExp( "([\\s()[{}])(www.[-a-zA-Z0-9@:%_\\+.,~#?&//=\\(\\)]+)" ), "\\1<a href=\"http://\\2\">\\2</a>" );
  text.replace( QRegExp( "([_\\.0-9a-z-]+@([0-9a-z][0-9a-z-]+\\.)+[a-z]{2,3})" ), "<a href=\"mailto:\\1\">\\1</a>" );
  text.remove( 0, 1 ); // remove the space added
  qDebug() << "Linkify:" << text;
  return text;
}

QString GuiChatMessage::formatHtmlText( const QString& text )
{
  QString text_formatted = "";
  int last_semicolon_index = -1;

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
      text_formatted += QLatin1String( "<br /> " ); // space added to match url after a \n
    else if( text.at( i ) == QLatin1Char( '<' ) )
    {
      if( Settings::instance().chatUseHtmlTags() )
      {
        if( last_semicolon_index >= 0 )
          text_formatted.replace( last_semicolon_index, 1, QLatin1String( "&lt;" ) );

        last_semicolon_index = text_formatted.size();
        text_formatted += QLatin1Char( '<' );
      }
      else
        text_formatted += QLatin1String( "&lt;" );
    }
    else if( text.at( i ) == QLatin1Char( '>' ) )
    {
      if( Settings::instance().chatUseHtmlTags() )
      {
        text_formatted += QLatin1Char( '>' );
        if( last_semicolon_index >= 0 )
          last_semicolon_index = -1;
      }
      else
        text_formatted += QLatin1String( "&gt;" );
    }
    else if( text.at( i ) == QLatin1Char( '"' ) )
    {
      if( last_semicolon_index >= 0 )
        text_formatted += QLatin1Char( '"' );
      else
        text_formatted += QLatin1String( "&quot;" );
    }
    else if( text.at( i ) == QLatin1Char( '&' ) )
    {
      text_formatted += QLatin1Char( '&' ); // not &amp; for Linkify
    }
    else
      text_formatted += text.at( i );
  }

  if( last_semicolon_index >= 0 )
    text_formatted.replace( last_semicolon_index, 1, QLatin1String( "&lt;" ) );

  text_formatted.replace( QRegExp("(^|\\s|>)_(\\S+)_(<|\\s|$)"), "\\1<u>\\2</u>\\3" );
  text_formatted.replace( QRegExp("(^|\\s|>)\\*(\\S+)\\*(<|\\s|$)"), "\\1<b>\\2</b>\\3" );
  text_formatted.replace( QRegExp("(^|\\s|>)\\/(\\S+)\\/(<|\\s|$)"), "\\1<i>\\2</i>\\3" );

  if( Settings::instance().chatUseClickableLinks() )
    text_formatted = linkify( text_formatted );

  if( Settings::instance().showEmoticons() )
    text_formatted = EmoticonManager::instance().parseEmoticons( text_formatted );

  PluginManager::instance().parseText( &text_formatted, false );

  return text_formatted;
}

QString GuiChatMessage::formatMessage( const User& u, const ChatMessage& cm, VNumber last_user_id )
{
  QString text_formatted = formatHtmlText( cm.message().text() );
  ChatMessageData cm_data = Protocol::instance().dataFromChatMessage( cm.message() );
  if( cm_data.textColor().isValid() )
  {
    text_formatted.prepend( QString( "<font color=%1>" ).arg( cm_data.textColor().name() ) );
    text_formatted.append( QString( "</font>" ) );
  }

  bool append_message_to_previous = last_user_id > 0 && last_user_id == u.id();

  QString user_name = append_message_to_previous ? QString( "&nbsp;&nbsp;" ) : u.isLocal() ? QObject::tr( "You" ) : u.name();

  QString html_message = QString( "%1<font color=%2><b>%3</b>%4</font>%5" )
      .arg( Settings::instance().chatShowMessageTimestamp() ? QString( "<font color=#808080>%1</font> " ).arg( cm.message().timestamp().toString( "(hh:mm:ss)" ) ) : "" )
      .arg( Settings::instance().showUserColor() ? u.color() : "#000000" )
      .arg( user_name )
      .arg( append_message_to_previous ? "&nbsp;" : Settings::instance().chatCompact() ? ":&nbsp;" : ":<br />" )
      .arg( text_formatted );

  html_message += Settings::instance().chatAddNewLineToMessage() ? "<br /><br />" : "<br />";
  return html_message;
}

QString GuiChatMessage::formatSystemMessage( const ChatMessage& cm )
{
  QString html_message = QString( "<font color=#808080>%1 %2</font>" )
            .arg( Settings::instance().chatShowMessageTimestamp() ? cm.message().timestamp().toString( "(hh:mm:ss) " ) : "" )
            .arg( cm.message().text() );
  html_message += Settings::instance().chatAddNewLineToMessage() ? "<br /><br />" : "<br />";
  return html_message;
}

QString GuiChatMessage::chatToHtml( const Chat& c )
{
  UserList chat_users = UserManager::instance().userList().fromUsersId( c.usersId() );
  QString html_text = "";
  VNumber last_message_user_id = 0;

  foreach( ChatMessage cm, c.messages() )
  {
    if( cm.isSystem() )
    {
      html_text += formatSystemMessage( cm );
      last_message_user_id = 0;
    }
    else
    {
      html_text += formatMessage( chat_users.find( cm.userId() ), cm, Settings::instance().showMessagesGroupByUser() ? last_message_user_id : 0 );
      last_message_user_id = cm.userId();
    }
  }

  return html_text;
}


