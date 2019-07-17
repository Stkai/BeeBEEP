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
#include "EmoticonManager.h"
#include "Settings.h"
#include "Protocol.h"
#include "Settings.h"
#include "UserManager.h"


static QString textImportantPrefix()
{
  if( Settings::instance().useOnlyTextEmoticons() )
  {
    return QString( "<font color=red><b>!!</b></font> " );
  }
  else
  {
    QString text_important_prefix = QString( "%1 " );
    QString emoticon_text = QString::fromUtf8( "‼" );
    if( !Settings::instance().useNativeEmoticons() )
    {
      Emoticon e = EmoticonManager::instance().emoticon( emoticon_text );
      int emoticon_size = 14;
      return text_important_prefix.arg( e.toHtml( emoticon_size ) );
    }
    else
      return text_important_prefix.arg( emoticon_text );
  }
}

static QString textImportantSuffix()
{
  return QString( "" );
}

QString GuiChatMessage::datetimestampToString( const ChatMessage& cm, bool show_timestamp, bool show_datestamp )
{
  QString date_time_stamp_format = "";

  if( show_datestamp || cm.timestamp().date() != QDate::currentDate() )
    date_time_stamp_format += QString( "yyyy-MM-dd" );

  if( show_timestamp )
  {
    if( !date_time_stamp_format.isEmpty() )
      date_time_stamp_format += QString( " " );
    if( Settings::instance().useMessageTimestampWithAP() )
      date_time_stamp_format += QString( "h:m:s ap" );
    else
      date_time_stamp_format += QString( "hh:mm:ss" );
  }

  return date_time_stamp_format.isEmpty() ? date_time_stamp_format : (Settings::instance().useMessageTimestampWithAP() ? QLocale("en_US").toString( cm.timestamp(), date_time_stamp_format ) : cm.timestamp().toString( date_time_stamp_format ));
}

QString GuiChatMessage::formatMessage( const User& u, const ChatMessage& cm, VNumber last_user_id, bool show_timestamp, bool show_datestamp, bool skip_system_message,
                                       bool show_message_group_by_user, bool use_your_name, bool use_chat_compact )
{
  QString html_message = "";
  if( cm.isImportant() )
    html_message += textImportantPrefix();

  QString text_formatted = cm.message();
  QString text_color = (cm.textColor().isValid() && cm.textColor() != QColor( 0, 0, 0 )) ? cm.textColor().name() : "";
  if( !text_color.isEmpty() )
  {
    text_formatted.prepend( QString( "<font color=%1>" ).arg( text_color ) );
    text_formatted.append( QLatin1String( "</font>" ) );
  }

  bool append_message_to_previous = show_message_group_by_user && last_user_id == u.id() && !cm.isImportant();

  QString date_time_stamp = datetimestampToString( cm, show_timestamp, show_datestamp );
  QString html_date_time_stamp = date_time_stamp.isEmpty() ? date_time_stamp : QString( "<font color=%1>(%2)</font>" ).arg( Settings::instance().chatSystemTextColor() ).arg( date_time_stamp );
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
                                                               .arg( (use_chat_compact && !Settings::instance().showTextInModeRTL()) ? QString( " " ) : QLatin1String( "<br>" ) );
  if( Settings::instance().showTextInModeRTL() )
    html_message += QString( "%1 %2 %3" ).arg( html_user_name ).arg( html_date_time_stamp ).arg( text_formatted );
  else if( use_chat_compact )
    html_message += QString( "%1 %2 %3" ).arg( html_date_time_stamp ).arg( html_user_name.isEmpty() ? QLatin1String( "&nbsp;&nbsp;" ) : html_user_name ).arg( text_formatted );
  else
    html_message += QString( "%1 %2 %3" ).arg( html_user_name ).arg( html_date_time_stamp ).arg( text_formatted );

  if( cm.isImportant() )
    html_message.append( textImportantSuffix() );

  if( last_user_id == ID_SYSTEM_MESSAGE )
  {
    if( !skip_system_message && !use_chat_compact )
      html_message.prepend( QLatin1String( "<br>" ) );
  }
  else if( last_user_id == ID_IMPORTANT_MESSAGE || cm.isImportant() )
  {
    html_message.prepend( QLatin1String( "<br>" ) );
  }
  else
  {
    if( !append_message_to_previous && !use_chat_compact )
      html_message.prepend( QLatin1String( "<br>" ) );
  }

  html_message += QLatin1String( "<br>" );

  return html_message;
}

QString GuiChatMessage::formatSystemMessage( const ChatMessage& cm, VNumber last_user_id, bool show_timestamp, bool show_datestamp, bool use_chat_compact )
{
  if( cm.message().isEmpty() )
    return QString( "" );

  QString date_time_stamp = cm.type() != ChatMessage::ImagePreview ? datetimestampToString( cm, show_timestamp, show_datestamp ) : QString( "" );

  QString html_message = QString( "<font color=%1>%2%3</font><br>" )
                           .arg( Settings::instance().chatSystemTextColor() )
                           .arg( date_time_stamp.isEmpty() ? date_time_stamp : QString( "(%1) " ).arg( date_time_stamp ) )
                           .arg( cm.message() );

  if( cm.isImportant() )
  {
    html_message.prepend( textImportantPrefix() );
    html_message.append( textImportantSuffix() + QLatin1String( "<br>" ) );
  }

  if( !use_chat_compact && cm.type() != ChatMessage::Other && last_user_id != ID_SYSTEM_MESSAGE )
    html_message.prepend( QLatin1String( "<br>" ) );

  return html_message;
}

QString GuiChatMessage::chatToHtml( const Chat& c, bool skip_file_transfers, bool skip_system_message, bool force_timestamp, bool force_datestamp )
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
      if( cm.isFileTransfer() || cm.isImagePreview() )
      {
        if( skip_file_transfers )
          continue;
        html_text += formatSystemMessage( cm, last_message_user_id, force_timestamp || Settings::instance().chatShowMessageTimestamp(), force_datestamp, Settings::instance().chatCompact() );
      }
      else
      {
        if( skip_system_message )
          continue;
        html_text += formatSystemMessage( cm, last_message_user_id, force_timestamp || Settings::instance().chatShowMessageTimestamp(), force_datestamp, Settings::instance().chatCompact() );
      }
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
      html_text += formatMessage( u, cm, last_message_user_id, force_timestamp || Settings::instance().chatShowMessageTimestamp(), force_datestamp, skip_system_message, false, true, Settings::instance().chatCompact() );
    }
    last_message_user_id = cm.isImportant() ? ID_IMPORTANT_MESSAGE : cm.userId();
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
