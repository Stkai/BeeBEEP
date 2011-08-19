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
#include "Protocol.h"
#include "Settings.h"


void Core::downloadFile( const User& u, const FileInfo& fi )
{
  QString icon_html = Bee::iconToHtml( ":/images/download.png", "*F*" );
  dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), tr( "%1 Downloading %2 from %3." )
                         .arg( icon_html, fi.name(), Settings::instance().showUserNickname() ? u.nickname() : u.name() ),
                         DispatchToAllChatsWithUser );
  mp_fileTransfer->downloadFile( fi );
}

void Core::checkFileTransferMessage( const User& u, const FileInfo& fi, const QString& msg )
{
  QString icon_html = Bee::iconToHtml( fi.isDownload() ? ":/images/download.png" : ":/images/upload.png", "*F*" );
  dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), tr( "%1 %2 %3 %4: %5." ).arg( icon_html, fi.name(),
                         fi.isDownload() ? tr( "from") : tr( "to" ), Settings::instance().showUserNickname() ? u.nickname() : u.name(), msg ),
                         DispatchToAllChatsWithUser );
  emit fileTransferMessage( u, fi, msg );
}

bool Core::sendFile( const User& u, const QString& file_path )
{
  QString icon_html = Bee::iconToHtml( ":/images/upload.png", "*F*" );

  QFileInfo file( file_path );
  if( !file.exists() )
  {
     dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), tr( "%1 %2: file not found." ).arg( icon_html, file_path ), DispatchToAllChatsWithUser );
     return false;
  }

  Connection* c = connection( u.id() );
  if( !c )
  {
    dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), tr( "%1 Unable to send file: user is not connected." ).arg( icon_html ), DispatchToAllChatsWithUser );
    return false;
  }

  if( !mp_fileTransfer->isWorking() )
  {
    if( !mp_fileTransfer->startListener() )
    {
      dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER, tr( "%1 Unable to send file: bind address/port failed." ).arg( icon_html ), DispatchToAllChatsWithUser );
      return false;
    }
  }

  FileInfo fi = mp_fileTransfer->addFile( file );

  Settings::instance().setLastDirectorySelected( file.absoluteDir().absolutePath() );
  dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), tr( "%1 You send the file to %2: %3." )
                         .arg( icon_html, u.path(), file.fileName() ), DispatchToAllChatsWithUser );
  Message m = Protocol::instance().fileInfoToMessage( fi );
  c->sendMessage( m );

  return true;
}
