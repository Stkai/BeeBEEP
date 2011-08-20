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

#include "Connection.h"
#include "Core.h"
#include "EmoticonManager.h"
#include "BeeUtils.h"
#include "Protocol.h"
#include "Random.h"
#include "Settings.h"
#include "Tips.h"


void Core::createDefaultChat()
{
  qDebug() << "Creating default chat";
  Chat c;
  c.setId( ID_DEFAULT_CHAT );
  c.addUser( ID_LOCAL_USER );
  QString sHtmlMsg = tr( "%1 Chat with all people." ).arg( Bee::iconToHtml( ":/images/chat.png", "*C*" ) );
  ChatMessage cm( ID_LOCAL_USER, Protocol::instance().systemMessage( sHtmlMsg ) );
  c.addMessage( cm );
  setChat( c );
}

void Core::createPrivateChat( const User& u )
{
  qDebug() << "Creating private chat room for user" << u.path();
  QList<VNumber> user_list;
  user_list.append( u.id() );
  Chat c = Protocol::instance().createChat( user_list );
  QString sHtmlMsg = tr( "%1 Chat with %2." ).arg( Bee::iconToHtml( ":/images/chat.png", "*C*" ), u.path() );
  ChatMessage cm( u.id(), Protocol::instance().systemMessage( sHtmlMsg ) );
  c.addMessage( cm );
  setChat( c );
}

Chat Core::chat( VNumber chat_id, bool read_all_messages )
{
  QList<Chat>::iterator it = m_chats.begin();
  while( it != m_chats.end() )
  {
    if( chat_id == (*it).id() )
    {
      if( read_all_messages )
        (*it).readAllMessages();
      return *it;
    }
    ++it;
  }
  return Chat();
}

Chat Core::privateChatForUser( VNumber user_id ) const
{
  if( user_id == ID_LOCAL_USER )
    return chat( ID_DEFAULT_CHAT );
  QList<Chat>::const_iterator it = m_chats.begin();
  while( it != m_chats.end() )
  {
    if( (*it).isPrivateForUser( user_id ) )
      return *it;
    ++it;
  }
  return Chat();
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
    ++it;
  }
  m_chats.append( c );
}

int Core::sendChatMessage( VNumber chat_id, const QString& msg )
{
  if( !isConnected() )
  {
    dispatchSystemMessage( chat_id, ID_LOCAL_USER, tr( "%1 Unable to send the message: you are not connected." )
                           .arg( Bee::iconToHtml( ":/images/red-ball.png", "*X*" ) ), DispatchToChat );
    return 0;
  }

  if( msg.isEmpty() )
    return 0;

  Message m = Protocol::instance().chatMessage( msg );
  m.setData( Settings::instance().chatFontColor() );

  int messages_sent = 0;

  if( chat_id == ID_DEFAULT_CHAT )
  {
    foreach( Connection *c, m_connections )
    {
      if( !c->sendMessage( m ) )
        dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER, tr( "%1 Unable to send the message to %2." )
                               .arg( Bee::iconToHtml( ":/images/red-ball.png", "*X*" ), user( c->userId() ).path() ), DispatchToChat );
      else
        messages_sent += 1;
    }
  }
  else
  {
    m.addFlag( Message::Private );
    Chat from_chat = chat( chat_id );
    QList<VNumber> user_list = from_chat.usersId();
    foreach( VNumber user_id, user_list )
    {
      if( user_id == ID_LOCAL_USER )
        continue;
      Connection* c = connection( user_id );
      if( c && c->sendMessage( m ) )
        messages_sent += 1;
      else
        dispatchSystemMessage( chat_id, ID_LOCAL_USER, tr( "%1 Unable to send the message to %2." )
                               .arg( Bee::iconToHtml( ":/images/red-ball.png", "*X*" ), user( user_id ).path() ), DispatchToChat );
    }
  }

  ChatMessage cm( ID_LOCAL_USER, m );
  dispatchToChat( cm, chat_id );
  return messages_sent;
}

void Core::sendWritingMessage( VNumber chat_id )
{
  if( !isConnected() )
    return;

  Chat from_chat = chat( chat_id );
  QList<VNumber> user_list = from_chat.usersId();
  foreach( VNumber user_id, user_list )
  {
    if( user_id == ID_LOCAL_USER )
      continue;
    Connection* c = connection( user_id );
    if( c )
    {
      qDebug() << "Sending Writing Message to" << c->peerAddress() << c->peerPort();
      c->sendData( Protocol::instance().writingMessage() );
    }
  }
}

void Core::showTipOfTheDay()
{
  QString tip_of_the_day = QString( "%1 %2" ).arg( Bee::iconToHtml( ":/images/tip.png", "*T*" ),
                                                   qApp->translate( "Tips", BeeBeepTips[ Random::number( 0, (BeeBeepTipsSize-1) ) ] ) );
  dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER, tip_of_the_day, DispatchToChat );
}

namespace
{


QString FormatHtmlText( QString text )
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

QString FormatMessage( const User& u, const ChatMessage& cm )
{
  QString text_formatted = FormatHtmlText( cm.message().text() );
  if( cm.message().data().size() > 0 )
  {
    QColor c( cm.message().data() );
    if( c.isValid() )
    {
      text_formatted.prepend( QString( "<font color=%1>" ).arg( c.name() ) );
      text_formatted.append( QString( "</font>" ) );
    }
  }
  QString sHtmlMessage = QString( "%1<font color=%2><b>%3</b>%4%5</font>" )
            .arg( Settings::instance().chatShowMessageTimestamp() ? QString( "<font color=gray>%1</font> " ).arg( cm.message().timestamp().toString( "(hh:mm:ss)" ) ) : "" )
            .arg( u.color() )
            .arg( Settings::instance().showUserNickname() ? u.nickname() : u.name() )
            .arg( Settings::instance().chatCompact() ? ":&nbsp;" : ":<br />" )
            .arg( text_formatted );
  return sHtmlMessage;
}

QString FormatSystemMessage( const ChatMessage& cm )
{
  QString sHtmlMessage = QString( "<font color=gray>%1 %2</font>" )
            .arg( Settings::instance().chatShowMessageTimestamp() ? cm.message().timestamp().toString( "(hh:mm:ss) " ) : "" )
            .arg( cm.message().text() );
  return sHtmlMessage;
}

}

QString Core::chatMessageToText( const ChatMessage& cm )
{
  QString s;
  if( cm.isSystem() )
  {
    s = FormatSystemMessage( cm );
  }
  else
  {
    User u = user( cm.userId() );
    s = FormatMessage( u, cm );
  }
  if( Settings::instance().chatAddNewLineToMessage() )
    s.append( "<br />" );
  s.append( "<br />" );
  return s;
}

QString Core::chatMessagesToText( const Chat& c )
{
  QString s = "";
  foreach( ChatMessage cm, c.messages() )
  {
    s += chatMessageToText( cm );
  }
  return s;
}

