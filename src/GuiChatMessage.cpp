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


QString GuiChatMessage::formatMessage( const User& u, const ChatMessage& cm, VNumber last_user_id )
{
  QString text_formatted = cm.message();
  if( cm.textColor().isValid() )
  {
    text_formatted.prepend( QString( "<font color=%1>" ).arg( cm.textColor().name() ) );
    text_formatted.append( QString( "</font>" ) );
  }

  bool append_message_to_previous = last_user_id > 0 && last_user_id == u.id();

  QString user_name = append_message_to_previous ? QString( "&nbsp;&nbsp;" ) : u.isLocal() ? QObject::tr( "You" ) : u.name();

  QString html_message = QString( "%1<font color=%2><b>%3</b>%4</font>%5" )
      .arg( Settings::instance().chatShowMessageTimestamp() ? QString( "<font color=#808080>%1</font> " ).arg( cm.timestamp().toString( "(hh:mm:ss)" ) ) : "" )
      .arg( Settings::instance().showUserColor() ? u.color() : "#000000" )
      .arg( user_name )
      .arg( append_message_to_previous ? "&nbsp;" : Settings::instance().chatCompact() ? ":&nbsp;" : ":<br />" )
      .arg( text_formatted );

  html_message += Settings::instance().chatAddNewLineToMessage() ? "<br /><br />" : "<br />";

  return html_message;
}

QString GuiChatMessage::formatSystemMessage( const ChatMessage& cm, bool force_timestamp )
{
  if( cm.message().isEmpty() )
    return QString( "" );
  QString html_message = QString( "<font color=#808080>%1 %2</font>" )
            .arg( force_timestamp || Settings::instance().chatShowMessageTimestamp() ? cm.timestamp().toString( "(hh:mm:ss) " ) : "" )
            .arg( cm.message() );
  html_message += Settings::instance().chatAddNewLineToMessage() ? "<br /><br />" : "<br />";
  return html_message;
}

QString GuiChatMessage::chatToHtml( const Chat& c, bool skip_system_message )
{
  UserList chat_users;
  QString html_text = "";
  VNumber last_message_user_id = 0;

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

      html_text += formatSystemMessage( cm, false );
      last_message_user_id = 0;
    }
    else
    {
      u = chat_users.find( cm.userId() );
      if( !u.isValid() )
      {
        u = UserManager::instance().findUser( cm.userId() );
        chat_users.set( u );
      }

      html_text += formatMessage( u, cm, Settings::instance().showMessagesGroupByUser() ? last_message_user_id : 0 );
      last_message_user_id = cm.userId();
    }
  }

  return html_text;
}

bool GuiChatMessage::messageCanBeShowedInActivity( const ChatMessage& cm )
{
  switch( cm.type() )
  {
  case ChatMessage::Header:
  case ChatMessage::System:
  case ChatMessage::Chat:
  case ChatMessage::UserStatus:
  case ChatMessage::History:
    return false;
  default:
    return true;
  }
}

bool GuiChatMessage::messageCanBeShowedInDefaultChat( const ChatMessage& cm )
{
  switch( cm.type() )
  {
  case ChatMessage::Header:
  case ChatMessage::System:
  case ChatMessage::Chat:
  case ChatMessage::UserStatus:
  case ChatMessage::History:
    return true;
  default:
    return false;
  }
}
