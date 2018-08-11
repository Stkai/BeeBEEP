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

#include "GuiChatMessage.h"
#include "ChatMessage.h"
#include "Settings.h"
#include "Protocol.h"
#include "UserManager.h"


QString GuiChatMessage::datetimestampToString( const ChatMessage& cm, bool show_timestamp, bool show_datestamp )
{
  QString date_time_stamp_format = "";

  if( show_datestamp || cm.timestamp().date() != QDate::currentDate() )
    date_time_stamp_format += QString( "yyyy-MM-dd" );

  if( show_timestamp )
  {
    if( !date_time_stamp_format.isEmpty() )
      date_time_stamp_format += QString( " " );
    date_time_stamp_format += QString( "hh:mm:ss" );
  }

  return date_time_stamp_format.isEmpty() ? date_time_stamp_format : cm.timestamp().toString( date_time_stamp_format );
}

QString GuiChatMessage::formatMessage( const User& u, const ChatMessage& cm, VNumber last_user_id, bool show_timestamp, bool show_datestamp, bool skip_system_message,
                                       bool show_message_group_by_user, bool use_your_name, bool use_chat_compact )
{
  QString text_formatted = cm.message();
  if( cm.textColor().isValid() )
  {
    text_formatted.prepend( QString( "<font color=%1>" ).arg( cm.textColor().name() ) );
    text_formatted.append( QLatin1String( "</font>" ) );
  }

  bool append_message_to_previous = show_message_group_by_user && last_user_id == u.id();

  QString date_time_stamp = datetimestampToString( cm, show_timestamp, show_datestamp );
  QString html_date_time_stamp = date_time_stamp.isEmpty() ? date_time_stamp : QString( "<font color=#808080>(%1)</font>" ).arg( date_time_stamp );
  QString user_name = append_message_to_previous ? QString( "" ) : (u.isLocal() && !use_your_name) ? QObject::tr( "You" ) : (u.isValid() ? u.name() : QObject::tr( "Unknown" ));
  if( cm.isFromAutoresponder() )
  {
    append_message_to_previous = false;
    user_name = QString( "%1 from %2" ).arg( Settings::instance().autoresponderName(), user_name );
  }
  QString html_user_name = user_name.isEmpty() ? user_name : QString( "<font color=%1><b>%2</b></font>%3%4" )
                                                               .arg( Settings::instance().chatUseColoredUserNames() ? u.color() : Settings::instance().chatDefaultUserNameColor() )
                                                               .arg( user_name )
                                                               .arg( Settings::instance().showTextInModeRTL() ? QString( "" ) : QString( ":" ) )
                                                               .arg( (use_chat_compact && !Settings::instance().showTextInModeRTL()) ? QString( " " ) : QLatin1String( "<br />" ) );

  QString html_message;

  if( Settings::instance().showTextInModeRTL() )
    html_message = QString( "%1 %2 %3" ).arg( html_user_name ).arg( html_date_time_stamp ).arg( text_formatted );
  else if( use_chat_compact )
    html_message = QString( "%1 %2 %3" ).arg( html_date_time_stamp ).arg( html_user_name.isEmpty() ? QLatin1String( "&nbsp;&nbsp;" ) : html_user_name ).arg( text_formatted );
  else
    html_message = QString( "%1 %2 %3" ).arg( html_user_name ).arg( html_date_time_stamp ).arg( text_formatted );

  if( last_user_id == ID_SYSTEM_MESSAGE )
  {
    if( !skip_system_message )
      html_message.prepend( QLatin1String( "<br />" ) );
  }
  else
  {
    if( !append_message_to_previous && !use_chat_compact )
      html_message.prepend( QLatin1String( "<br />" ) );
  }

  html_message += QLatin1String( "<br />" );

  return html_message;
}

QString GuiChatMessage::formatSystemMessage( const ChatMessage& cm, VNumber last_user_id, bool show_timestamp, bool show_datestamp )
{
  if( cm.message().isEmpty() )
    return QString( "" );

  QString date_time_stamp = cm.type() != ChatMessage::ImagePreview ? datetimestampToString( cm, show_timestamp, show_datestamp ) : QString( "" );

  QString html_message = QString( "<font color=#808080>%1%2</font><br />" )
                           .arg( date_time_stamp.isEmpty() ? date_time_stamp : QString( "(%1) " ).arg( date_time_stamp ) )
                           .arg( cm.message() );

  if( cm.type() != ChatMessage::Other && last_user_id != ID_SYSTEM_MESSAGE )
    html_message.prepend( QLatin1String( "<br />" ) );

  return html_message;
}

QString GuiChatMessage::chatToHtml( const Chat& c, bool skip_system_message, bool force_timestamp, bool force_datestamp )
{
  UserList chat_users;
  QString html_text = "";
  VNumber last_message_user_id = skip_system_message ? 0 : ID_SYSTEM_MESSAGE;

  if( c.isDefault() )
    chat_users = UserManager::instance().userList();
  else
    chat_users = UserManager::instance().userList().fromUsersId( c.usersId() );

  User u;

  foreach( ChatMessage cm, c.messages() )
  {
    if( cm.isFromSystem() )
    {
      if( skip_system_message )
        continue;
      html_text += formatSystemMessage( cm, last_message_user_id, force_timestamp || Settings::instance().chatShowMessageTimestamp(), force_datestamp );
    }
    else
    {
      u = chat_users.find( cm.userId() );
      if( !u.isValid() )
      {
        u = UserManager::instance().findUser( cm.userId() );
        if( u.isValid() )
          chat_users.set( u );
      }
      html_text += formatMessage( u, cm, last_message_user_id, force_timestamp || Settings::instance().chatShowMessageTimestamp(), force_datestamp, skip_system_message, false, true, true );
    }

    last_message_user_id = cm.userId();
  }

  return html_text;
}

bool GuiChatMessage::messageCanBeShowedInActivity( const ChatMessage& cm )
{
  return !messageCanBeShowedInDefaultChat( cm );
}

bool GuiChatMessage::messageCanBeShowedInDefaultChat( const ChatMessage& cm )
{
  switch( cm.type() )
  {
  case ChatMessage::Header:
  case ChatMessage::Chat:
  case ChatMessage::History:
  case ChatMessage::Other:
  case ChatMessage::ImagePreview:
  case ChatMessage::Autoresponder:
    return true;
  default:
    return false;
  }
}
