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
#include "FileInfo.h"


void Core::parseMessage( VNumber user_id, const Message& m )
{
  User u = user( user_id );
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
  if( m.hasFlag( Message::Writing ) )
  {
    qDebug() << "User" << m_userId << "is writing";
    emit isWriting( u );
    return;
  }
  else if( m.hasFlag( Message::Status ) )
  {
    User user_with_new_status = u;
    if( Protocol::instance().changeUserStatusFromMessage( &user_with_new_status, m ) )
    {
      qDebug() << "User" << user_with_new_status.path() << "changes status to" << user_with_new_status.status() << user_with_new_status.statusDescription();
      setUser( user_with_new_status );
      emit userChanged( user_with_new_status );
    }
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

  Connection* c = connection( u.id() );
  if( !c )
  {
    qWarning() << "Connection not found for user" << u.id() << "while parsing file message";
    return;
  }
  fi.setHostAddress( c->peerAddress() );
  QString icon_html = Bee::iconToHtml( fi.isDownload() ? ":/images/download.png" : ":/images/upload.png", "*F*" );
  dispatchSystemMessage( u.id(), tr( "%1 %2 is sending to you the file: %3." ).arg( icon_html, Settings::instance().showUserNickname() ? u.nickname() : u.name(), fi.name() ) );
  emit fileDownloadRequest( u, fi );
}

void Core::parseChatMessage( const User& u, const Message& m )
{
  dispatchChatMessageReceived( u.id(), m );
}
