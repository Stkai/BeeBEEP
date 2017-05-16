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

#ifndef BEEBEEP_GUICHATMESSAGE_H
#define BEEBEEP_GUICHATMESSAGE_H

#include "Config.h"
class Chat;
class ChatMessage;
class User;


class GuiChatMessage
{
public:
  static QString datetimestampToString( const ChatMessage&, bool show_timestamp, bool show_datestamp );
  static QString chatToHtml( const Chat&, bool skip_system_message, bool force_timestamp, bool force_datestamp );
  static QString formatMessage( const User&, const ChatMessage&, VNumber last_user_id, bool show_timestamp, bool show_datestamp, bool skip_system_message );
  static QString formatSystemMessage( const ChatMessage&, VNumber last_user_id, bool show_timestamp, bool show_datestamp );
  static bool messageCanBeShowedInActivity( const ChatMessage& );
  static bool messageCanBeShowedInDefaultChat( const ChatMessage& );

};

#endif // BEEBEEP_GUICHATMESSAGE_H
