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

#include "BeeUtils.h"
#include "Connection.h"
#include "Core.h"
#include "FileInfo.h"
#include "Protocol.h"


void Core::parseMessage( VNumber user_id, const Message& m )
{
  qDebug() << "Parsing message received from user" << user_id;
  User u = m_users.find( user_id );
  if( !u.isValid() )
  {
    qWarning() << "Invalid user" << user_id << "found while parsing message";
    return;
  }

  switch( m.type() )
  {
  case Message::User:
    parseUserMessage( u, m );
    break;
  case Message::Chat:
    parseChatMessage( u, m );
    break;
 case Message::File:
    parseFileMessage( u, m );
    break;
  default:
    qWarning() << "Core cannot parse the message with type" << m.type();
    break;
  }
}

void Core::parseUserMessage( const User& u, const Message& m )
{
  if( m.hasFlag( Message::UserWriting ) )
  {
    qDebug() << "User" << u.path() << "is writing";
    emit userIsWriting( u );
    return;
  }
  else if( m.hasFlag( Message::UserStatus ) )
  {
    User user_with_new_status = u;
    if( Protocol::instance().changeUserStatusFromMessage( &user_with_new_status, m ) )
    {
      qDebug() << "User" << user_with_new_status.path() << "changes status to" << user_with_new_status.status() << user_with_new_status.statusDescription();
      m_users.setUser( user_with_new_status );
      setUserStatus( user_with_new_status );
    }
    else
      qWarning() << "Unable to change the status of the user" << u.path() << "because message is invalid";
  }
  else if( m.hasFlag( Message::UserVCard ) )
  {
    User user_with_new_vcard = u;
    if( Protocol::instance().changeVCardFromMessage( &user_with_new_vcard, m ) )
    {
      qDebug() << "User" << user_with_new_vcard.path() << "has new vCard";
      m_users.setUser( user_with_new_vcard );
      setUserVCard( user_with_new_vcard );
    }
    else
      qWarning() << "Unable to read vCard from the user" << u.path();
  }
  else if( m.hasFlag( Message::UserName ) )
  {
    User user_with_new_name = u;
    if( Protocol::instance().changeUserNameFromMessage( &user_with_new_name, m ) )
    {
      qDebug() << "User" << u.path() << "changes his name to" << user_with_new_name.name();
      m_users.setUser( user_with_new_name );
      setUserName( user_with_new_name, u.name() );
    }
    else
      qWarning() << "Unable to change the username of the user" << u.path() << "because message is invalid";
  }
  else
    qWarning() << "Invalid flag found in user message (in Connection)";
}

void Core::parseFileMessage( const User& u, const Message& m )
{
  FileInfo fi = Protocol::instance().fileInfoFromMessage( m );
  if( !fi.isValid() )
  {
    qWarning() << "Invalid FileInfo received from user" << u.id() << ": [" << m.data() << "]:" << m.text();
    return;
  }

  if( m.hasFlag( Message::Refused ) )
  {
    dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), tr( "%1 %2 has refused to download %3." )
                           .arg( Bee::iconToHtml( ":/images/upload.png", "*F*" ), u.path(), fi.name() ), DispatchToAllChatsWithUser );
    return;
  }

  Connection* c = connection( u.id() );
  if( !c )
  {
    qWarning() << "Connection not found for user" << u.id() << "while parsing file message";
    return;
  }
  fi.setHostAddress( c->peerAddress() );

  dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), tr( "%1 %2 is sending to you the file: %3." )
                         .arg( Bee::iconToHtml( ":/images/download.png", "*F*" ), u.path(), fi.name() ), DispatchToAllChatsWithUser );
  emit fileDownloadRequest( u, fi );
}

void Core::parseChatMessage( const User& u, const Message& m )
{
  qDebug() << "Chat message received from user" << u.path();
  dispatchChatMessageReceived( u.id(), m );
}
