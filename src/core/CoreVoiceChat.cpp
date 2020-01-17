//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2019 Marco Mastroddi
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

#include "IconManager.h"
#include "ChatManager.h"
#include "Core.h"
#include "Protocol.h"
#include "Settings.h"
#include "BeeUtils.h"


bool Core::sendVoiceMessageToChat( VNumber chat_id, const QString& file_path, qint64 message_duration )
{
  QString icon_html = IconManager::instance().toHtml( "red-ball.png", "*F*" );

  if( !Settings::instance().enableFileTransfer() )
  {
    dispatchSystemMessage( chat_id, ID_LOCAL_USER, tr( "%1 Unable to send voice message. File transfer is disabled." ).arg( icon_html ),
                           DispatchToChat, ChatMessage::FileTransfer, false );
    return false;
  }

  Chat c = ChatManager::instance().chat( chat_id );
  if( !c.isValid() )
  {
    qWarning() << "Unable to send voice message because invalid chat found in Core::sendVoiceMessageToChat(...)";
    return false;
  }

  QFileInfo file( file_path );
  if( !file.exists() )
  {
    dispatchSystemMessage( chat_id, ID_LOCAL_USER, tr( "%1 %2: voice message not found." ).arg( icon_html, file_path ),
                           DispatchToChat, ChatMessage::FileTransfer, false );
    return false;
  }

  FileInfo fi = mp_fileTransfer->addFile( file, "", false, c.privateId(), FileInfo::VoiceMessage, message_duration );
#ifdef BEEBEEP_DEBUG
  qDebug() << "Voice message" << fi.path() << "is added to file transfer list";
#endif

  QUrl file_url = QUrl::fromLocalFile( file_path );
  file_url.setScheme( FileInfo::urlSchemeVoiceMessage() );
  QString msg_html = QString( "[ <a href=\"%1\">%2</a> ] (%3) %4" ).arg( file_url.toString(),
                                                                    tr( "voice message" ),
                                                                    Bee::timeToString( message_duration ),
                                                                    IconManager::instance().toHtml( "voice-message.png", "*v*" ) );
  dispatchToChat( ChatMessage( ID_LOCAL_USER, msg_html, ChatMessage::Voice, true ), chat_id );
  Message m = Protocol::instance().fileInfoToMessage( fi, Settings::instance().protocolVersion() );
  sendMessageToChat( c, m );
  return true;
}


