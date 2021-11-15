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

#include "ChatMessage.h"
#include "Message.h"
#include "Protocol.h"


ChatMessage::ChatMessage()
  : m_userId( ID_INVALID ), m_message( "" ), m_timestamp(), m_textColor(),
    m_type( ChatMessage::Other ), m_isImportant( false ), m_canBeSaved( false )
{
}

ChatMessage::ChatMessage( const ChatMessage& cm )
{
  (void)operator=( cm );
}

ChatMessage::ChatMessage( VNumber user_id, const Message& m, ChatMessage::Type cmt, bool can_be_saved )
  : m_userId( user_id ), m_message( "" ), m_timestamp(), m_textColor(), m_type( cmt ),
    m_isImportant( false ), m_canBeSaved( can_be_saved )
{
  fromMessage( m );
}

ChatMessage::ChatMessage( VNumber user_id, const QString& msg, ChatMessage::Type cmt, bool can_be_saved )
  : m_userId( user_id ), m_message( msg ), m_timestamp( QDateTime::currentDateTime() ), m_textColor(), m_type( cmt ),
    m_isImportant( false ), m_canBeSaved( can_be_saved )
{
}

ChatMessage& ChatMessage::operator=( const ChatMessage& cm )
{
  if( this != &cm )
  {
    m_userId = cm.m_userId;
    m_message = cm.m_message;
    m_timestamp = cm.m_timestamp;
    m_textColor = cm.m_textColor;
    m_type = cm.m_type;
    m_isImportant = cm.m_isImportant;
    m_canBeSaved = cm.m_canBeSaved;
  }
  return *this;
}

void ChatMessage::fromMessage( const Message& m )
{
  if( m.type() != Message::System )
  {
    m_message = m.isSourceCode() ? m.text().trimmed() : Protocol::instance().formatHtmlText( m.text() );
    if( m_message.endsWith( "<br>" ) )
      m_message.chop( 4 );
  }
  else
    m_message = m.text();
  m_timestamp = m.timestamp();
  if( !m.isSourceCode() )
  {
    ChatMessageData cm_data = Protocol::instance().dataFromChatMessage( m );
    if( cm_data.textColor().isValid() )
      m_textColor = cm_data.textColor();
  }
  m_isImportant = m.hasFlag( Message::Important );
}

bool ChatMessage::isChatActivity() const
{
  switch( m_type )
  {
  case ChatMessage::Header:
  case ChatMessage::Chat:
  case ChatMessage::FileTransfer:
  case ChatMessage::History:
  case ChatMessage::Other:
  case ChatMessage::ImagePreview:
  case ChatMessage::Autoresponder:
  case ChatMessage::Voice:
    return true;
  default:
    return false;
  }
}

bool ChatMessage::isSystemActivity() const
{
  return isFromSystem() && !isChatActivity();
}


