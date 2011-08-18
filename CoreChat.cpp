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

#include "Core.h"
#include "BeeUtils.h"


void Core::createDefaultChat()
{
  qDebug() << "Creating default chat";
  Chat c;
  c.setId( ID_DEFAULT_CHAT );
  QString sHtmlMsg = tr( "%1 Chat with all people." ).arg( Bee::iconToHtml( ":/images/chat.png", "*C*" ) );
  ChatMessage cm( u.id(), Protocol::instance().systemMessage( sHtmlMsg ) );
  c.addMessage( cm );
  setChat( c );

}

void Core::createPrivateChat( const User& u )
{
  qDebug() << "Creating private chat room for user" << u.path();
  Chat c;
  c.setId( u.id() );
  QString sHtmlMsg = tr( "%1 Chat with %2." ).arg( Bee::iconToHtml( ":/images/chat.png", "*C*" ), u.path() );
  ChatMessage cm( u.id(), Protocol::instance().systemMessage( sHtmlMsg ) );
  c.addMessage( cm );
  setChat( c );
}

Chat Core::chat( const QString& chat_name, bool create_if_need, bool read_all_messages )
{
  Chat c = m_chats.value( chat_name, Chat() );
  if( !c.isValid() )
  {
    if( create_if_need )
    {

    }
    return c;
  }

  if( read_all_messages )
  {
    c.readAllMessages();
    m_chats.insert( c.name(), c );
  }
  return c;
}

Chat Core::privateChatForUser( VNumber user_id )
{
  return chat( user_id );
}

Chat Core::chat( VNumber chat_id ) const
{
  QList<Chat>::const_iterator it = m_chats.begin();
  while( it != m_chats.end() )
  {
    if( chat_id == (*it).id() )
      return *it;
    ++it;
  }
  return Chat();
}

void Core::setChat( const Chat& c )
{
  QList<Chat>::iterator it = m_chats.begin();
  while( it != m_chats.end() )
  {
    if( (*it).id() == c.id() )
    {
      (*it) = c;
      return;
    }
    ++it
  }
  m_chats.append( c );
}



