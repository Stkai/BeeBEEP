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

#include "BeeUtils.h"
#include "GuiChatMessage.h"
#include "ChatMessage.h"
#include "EmoticonManager.h"
#include "Settings.h"
#include "Protocol.h"
#include "Settings.h"
#include "UserManager.h"


static QString textImportantPrefix()
{
  return QString( "<font color=red><b>!!</b></font> " );
}

static QString textImportantSuffix()
{
  return QString( "" );
}

QString GuiChatMessage::datetimestampToString( const ChatMessage& cm, bool show_timestamp, bool show_datestamp )
{
  QString date_time_stamp_format = "";

  if( show_datestamp || cm.timestamp().date() != QDate::currentDate() )
  {
//: Date format: do not change letters. More info in https://doc.qt.io/qt-5/qdatetime.html#toString
    date_time_stamp_format += QT_TRANSLATE_NOOP( "Date", "yyyy-MM-dd" );
  }

  if( show_timestamp )
  {
    if( !date_time_stamp_format.isEmpty() )
      date_time_stamp_format += QString( " " );
    if( Settings::instance().useMessageTimestampWithAP() )
    {
//: Time format: do not change letters. More info in https://doc.qt.io/qt-5/qdatetime.html#toString
      date_time_stamp_format += QT_TRANSLATE_NOOP( "Date", "h:mm:ss ap" );
    }
    else
    {
//: Time format: do not change letters. More info in https://doc.qt.io/qt-5/qdatetime.html#toString
      date_time_stamp_format += QT_TRANSLATE_NOOP( "Date", "hh:mm:ss" );
    }
  }

  return date_time_stamp_format.isEmpty() ? date_time_stamp_format : (Settings::instance().useMessageTimestampWithAP() ? QLocale("en_US").toString( cm.timestamp(), date_time_stamp_format ) : cm.timestamp().toString( date_time_stamp_format ));
}

QString GuiChatMessage::formatMessage( const User& u, const ChatMessage& cm, VNumber last_user_id, bool show_timestamp, bool show_datestamp,
                                       bool skip_system_message, bool show_message_group_by_user, bool use_your_name, bool use_chat_compact )
{
  QString html_message = "";
  if( cm.isImportant() )
    html_message += textImportantPrefix();

  QString text_formatted = cm.message();

  if( cm.isSourceCode() )
  {
    text_formatted.replace( QChar( '&' ), QLatin1String( "&amp;" ) );
    text_formatted.replace( QChar( '<' ), QLatin1String( "&lt;" ) );
    text_formatted.replace( QChar( '>' ), QLatin1String( "&gt;" ) );
    text_formatted.prepend( "\n<code>" );
    text_formatted.append( "\n</code>\n" );
  }

  text_formatted.replace( QString( "  " ), QLatin1String( "&nbsp;&nbsp;" ) );
  text_formatted.replace( QChar( '\t' ), QLatin1String( "&nbsp;&nbsp;&nbsp;&nbsp;" ) );
  text_formatted.replace( QChar( '\r' ), QLatin1String( "" ) );
  text_formatted.replace( QChar( '\n' ), QLatin1String( "<br>" ) );

  if( !cm.isSourceCode() )
  {
    QString text_color = (cm.textColor().isValid() && cm.textColor() != QColor( 0, 0, 0 ) && cm.textColor() != QColor( 255, 255, 255 ) ) ? cm.textColor().name() : "";
    if( !text_color.isEmpty() )
    {
      text_formatted.prepend( QString( "<font color=%1>" ).arg( text_color ) );
      text_formatted.append( QLatin1String( "</font>" ) );
    }
  }

  bool append_message_to_previous = show_message_group_by_user && last_user_id == u.id() && !cm.isImportant();

  QString user_name_to_show = Bee::userNameToShow( u, true );
  QString date_time_stamp = datetimestampToString( cm, show_timestamp, show_datestamp );
  QString html_date_time_stamp = date_time_stamp.isEmpty() ? date_time_stamp : QString( "<font color=%1>(%2)</font>" ).arg( Settings::instance().chatSystemTextColor(), date_time_stamp );
  QString user_name = append_message_to_previous ? QString( "" ) : (u.isLocal() && !use_your_name) ? QObject::tr( "You" ) : (u.isValid() ? user_name_to_show : QObject::tr( "Unknown" ));
  if( cm.isFromAutoresponder() )
  {
    append_message_to_previous = false;
    user_name = QString( "%1 from %2" ).arg( Settings::instance().autoresponderName(), user_name );
  }
  QString html_user_name = user_name.isEmpty() ? user_name : QString( "<font color=%1><b>%2</b></font>%3%4" )
                                                               .arg( Settings::instance().chatUseColoredUserNames() ? u.color() : Settings::instance().chatDefaultUserNameColor(),
                                                                     user_name,
                                                                     (Settings::instance().showTextInModeRTL() ? QString( "" ) : QString( ":" ) ),
                                                                     (use_chat_compact && !Settings::instance().showTextInModeRTL()) ? QString( " " ) : QLatin1String( "<br>" ) );

  if( Settings::instance().showTextInModeRTL() )
    html_message += QString( "%1 %2 %3" ).arg( html_user_name, html_date_time_stamp, text_formatted );
  else if( use_chat_compact )
    html_message += QString( "%1 %2 %3" ).arg( html_date_time_stamp, (html_user_name.isEmpty() ? QLatin1String( "&nbsp;&nbsp;" ) : html_user_name), text_formatted );
  else
    html_message += QString( "%1 %2 %3" ).arg( html_user_name, html_date_time_stamp, text_formatted );

  if( cm.isImportant() )
    html_message.append( textImportantSuffix() );

  if( !use_chat_compact )
  {
    if( last_user_id == ID_SYSTEM_MESSAGE )
    {
      if( !skip_system_message )
        html_message.prepend( QLatin1String( "<br>" ) );
    }
    else if( last_user_id == ID_IMPORTANT_MESSAGE || cm.isImportant() )
    {
      html_message.prepend( QLatin1String( "<br>" ) );
    }
    else
    {
      if( !append_message_to_previous )
        html_message.prepend( QLatin1String( "<br>" ) );
    }
  }

  html_message += QLatin1String( "<br>" );

  return html_message;
}

QString GuiChatMessage::formatSystemMessage( const ChatMessage& cm, VNumber last_user_id, bool show_timestamp, bool show_datestamp, bool use_chat_compact )
{
  if( cm.message().isEmpty() )
    return QString( "" );

  QString date_time_stamp = cm.type() != ChatMessage::ImagePreview ? datetimestampToString( cm, show_timestamp, show_datestamp ) : QString( "" );

  QString html_message = QString( "<font color=%1>%2%3</font>" )
                           .arg( Settings::instance().chatSystemTextColor(),
                                 date_time_stamp.isEmpty() ? date_time_stamp : QString( "(%1) " ).arg( date_time_stamp ),
                                 cm.message() );
  if( cm.isImportant() )
  {
    html_message.prepend( textImportantPrefix() );
    html_message.append( textImportantSuffix() );
  }

  html_message.append( QLatin1String( "<br>" ) );

  if( !use_chat_compact )
  {
    if( cm.type() != ChatMessage::Other && last_user_id != ID_SYSTEM_MESSAGE )
    {
      html_message.prepend( QLatin1String( "<br>" ) );
    }
    else
    {
      if( cm.isImportant() )
        html_message.prepend( QLatin1String( "<br>" ) );
    }
  }

  return html_message;
}

QString GuiChatMessage::chatToHtml( const Chat& c, bool skip_file_transfers, bool skip_system_message, bool force_timestamp, bool force_datestamp, bool use_chat_compact, bool skip_cannot_be_saved_messages )
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
        if( skip_cannot_be_saved_messages && !cm.canBeSaved() )
          continue;
        html_text += formatSystemMessage( cm, last_message_user_id, force_timestamp || Settings::instance().chatShowMessageTimestamp(), force_datestamp, use_chat_compact );
      }
      else
      {
        if( skip_system_message )
          continue;
        if( skip_cannot_be_saved_messages && !cm.canBeSaved() )
          continue;
        html_text += formatSystemMessage( cm, last_message_user_id, force_timestamp || Settings::instance().chatShowMessageTimestamp(), force_datestamp, use_chat_compact );
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
      html_text += formatMessage( u, cm, last_message_user_id, force_timestamp || Settings::instance().chatShowMessageTimestamp(), force_datestamp, skip_system_message, false, true, use_chat_compact );
    }
    last_message_user_id = cm.isImportant() ? ID_IMPORTANT_MESSAGE : cm.userId();
  }

  return html_text;
}
