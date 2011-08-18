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
  return user_id == ID_LOCAL_USER ? chat( ID_DEFAULT_CHAT ) : chat( user_id );
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

void Core::sendChatMessage( VNumber chat_id, const QString& msg )
{
  if( !isWorking() )
  {
    dispatchSystemMessage( chat_id, m_localUser.id(), tr( "%1 Unable to send the message: you are not connected." ).arg( Bee::iconToHtml( ":/images/red-ball.png", "*X*" ) ) );
    return;
  }

  if( msg.isEmpty() )
    return;

  Message m = Protocol::instance().chatMessage( msg );
  m.setData( Settings::instance().chatFontColor() );

  if( chat_id == ID_DEFAULT_CHAT )
  {
    foreach( Connection *c, m_connections )
      c->sendMessage( m );
  }
  else
  {
    m.addFlag( Message::Private );
    Connection* c = connection( chat_id ); // user_id and chat_id is == for now...
    if( c )
    {
      c->sendMessage( m );
    }
    else
    {
      dispatchSystemMessage( chat_id, m_localUser.id(), tr( "%1 Unable to send the message." ).arg( Bee::iconToHtml( ":/images/red-ball.png", "*X*" ) ) );
      return;
    }
  }

  ChatMessage cm( m_localUser.id(), m );
  Chat c = chat( chat_id );
  c.addMessage( cm );
  emit newChatMessage( chat_id, cm );
}

void Core::sendWritingMessage( VNumber chat_id )
{
  if( !isWorking() )
    return;

  Connection* c = connection( chat_id ); // FIXME: user_id!!! (for now works)
  if( c )
  {
    qDebug() << "Sending Writing Message to" << c->peerAddress() << c->peerPort();
    c->sendMessage( Protocol::instance().writingMessage() );
  }
}



