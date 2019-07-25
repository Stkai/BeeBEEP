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

#include "BeeUtils.h"
#include "ChatManager.h"
#include "Connection.h"
#include "Core.h"
#include "IconManager.h"
#include "ImageOptimizer.h"
#include "Protocol.h"
#include "Screenshot.h"
#include "ShareDesktop.h"
#include "Settings.h"
#include "UserManager.h"


bool Core::startShareDesktop( VNumber user_id )
{
  if( !Settings::instance().enableShareDesktop() )
    return false;

  User u = UserManager::instance().findUser( user_id );
  if( !u.isValid() )
  {
    qWarning() << "Invalid user" << user_id << "found in Core::startShareDesktop(...)";
    return false;
  }

  QString sHtmlMsg;
  if( u.protocolVersion() < SHARE_DESKTOP_PROTO_VERSION )
  {
    sHtmlMsg = tr( "%1 You cannot share desktop with %2." ).arg( IconManager::instance().toHtml( "desktop-share-refused.png", "*G*" ), u.name() );
    dispatchSystemMessage( ID_DEFAULT_CHAT, user_id, sHtmlMsg, DispatchToAllChatsWithUser, ChatMessage::System );
    qDebug() << "You cannot share desktop with" << qPrintable( u.path() ) << "with old protocol" << u.protocolVersion();
    emit shareDesktopUpdate( u );
    return false;
  }

  if( !mp_shareDesktop->addUserId( user_id ) )
  {
    emit shareDesktopUpdate( u );
    return false;
  }

  sHtmlMsg = tr( "%1 You start to share desktop with %2." ).arg( IconManager::instance().toHtml( "desktop-share.png", "*G*" ), u.name() );
  dispatchSystemMessage( ID_DEFAULT_CHAT, user_id, sHtmlMsg, DispatchToAllChatsWithUser, ChatMessage::System );
  qDebug() << "Start to share desktop with" << qPrintable( u.path() );
  if( !mp_shareDesktop->isActive() )
    mp_shareDesktop->start();
  emit shareDesktopUpdate( u );
  return true;
}

void Core::stopShareDesktop( VNumber user_id )
{
  if( !mp_shareDesktop->isActive() )
    return;

  if( !mp_shareDesktop->userIdList().contains( user_id ) )
    return;

  mp_shareDesktop->removeUserId( user_id );
  if( !mp_shareDesktop->hasUsers() )
    mp_shareDesktop->stop();

  User u = UserManager::instance().findUser( user_id );
  if( u.isValid() )
  {
    QString sHtmlMsg = tr( "%1 You stop to share desktop with %2." ).arg( IconManager::instance().toHtml( "desktop-share-refused.png", "*G*" ), u.name() );
    dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER, sHtmlMsg, DispatchToAllChatsWithUser, ChatMessage::System );
    sendMessageToLocalNetwork( u, Protocol::instance().shareDesktopImageDataToMessage( ShareDesktopData( "", "png", false, 0 ) ) ); // Empty image stops desktop sharing
    emit shareDesktopUpdate( u );
  }
}

void Core::stopShareDesktop()
{
  foreach( VNumber user_id, mp_shareDesktop->userIdList() )
    stopShareDesktop( user_id );
}

void Core::refuseToViewShareDesktop( VNumber from_user_id, VNumber to_user_id )
{
  if( from_user_id != ID_LOCAL_USER )
  {
    if( !mp_shareDesktop->isActive() )
      return;
  }

  User from_user = UserManager::instance().findUser( from_user_id );
  if( !from_user.isValid() )
  {
    qWarning() << "Invalid from user" << from_user_id << "found in Core::refuseToViewShareDesktop(...)";
    return;
  }

  User to_user = UserManager::instance().findUser( to_user_id );
  if( !to_user.isValid() )
  {
    qWarning() << "Invalid from user" << to_user_id << "found in Core::refuseToViewShareDesktop(...)";
    return;
  }

  if( from_user.isLocal() )
  {
    qDebug() << "You have finished to view the desktop shared by" << qPrintable( to_user.path() );
    Message m = Protocol::instance().refuseToViewDesktopShared();
    sendMessageToLocalNetwork( to_user, m );
  }
  else
  {
    qDebug() << qPrintable( from_user.path() ) << "has closed the view of your shared desktop";
    QString sHtmlMsg = tr( "%1 %2 has closed the view of your shared desktop." ).arg( IconManager::instance().toHtml( "desktop-share-refused.png", "*G*" ), from_user.name() );
    dispatchSystemMessage( ID_DEFAULT_CHAT, from_user.id(), sHtmlMsg, DispatchToAllChatsWithUser, ChatMessage::System );
    stopShareDesktop( from_user_id );
  }
}

bool Core::shareDesktopIsActive( VNumber user_id ) const
{
  if( user_id == ID_INVALID )
    return mp_shareDesktop->isActive() && mp_shareDesktop->userIdList().size() > 0;
  else
    return mp_shareDesktop->isActive() && mp_shareDesktop->userIdList().contains( user_id );
}

void Core::onShareDesktopImageAvailable( const ShareDesktopData& sdd )
{
  if( !mp_shareDesktop->isActive() )
    return;

  if( mp_shareDesktop->userIdList().isEmpty() )
  {
    stopShareDesktop();
    return;
  }

  Message m = Protocol::instance().shareDesktopImageDataToMessage( sdd );
  foreach( VNumber user_id, mp_shareDesktop->userIdList() )
  {
    if( mp_shareDesktop->hasUserReadImage( user_id ) )
    {
      Connection* c = connection( user_id );
      if( c && c->isConnected() )
      {
        if( c->sendMessage( m ) )
        {
#ifdef BEEBEEP_DEBUG
          qDebug() << "Share desktop send image" << qPrintable( sdd.imageType() ) << "message with size" << m.text().size() << "to user" << user_id;
#endif
          mp_shareDesktop->resetUserReadImage( user_id );
        }
      }
    }
  }
}

void Core::parseShareDesktopMessage( const User& u, const Message& m )
{
  if( m.hasFlag( Message::Refused ) )
  {
 #ifdef BEEBEEP_DEBUG
    qDebug() << qPrintable( u.path() ) << "refuse your share desktop message";
 #endif
    refuseToViewShareDesktop( u.id(), ID_LOCAL_USER );
  }
  else if( m.hasFlag( Message::Request ) )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << qPrintable( u.path() ) << "has request new desktop share image";
#endif
    mp_shareDesktop->requestImageFromUser( u.id() );
  }
  else if( m.hasFlag( Message::Private ) )
  {
    if( Settings::instance().enableShareDesktop() )
    {
      ShareDesktopData sdd = Protocol::instance().imageDataFromShareDesktopMessage( m );
      QImage img = ImageOptimizer::instance().loadImage( sdd.imageData(), sdd.imageType(), sdd.isCompressed() );
      if( img.isNull() )
        qDebug() << qPrintable( u.path() ) << "has sent a NULL image and has finished to share desktop with you";
      else
        sendMessageToLocalNetwork( u, Protocol::instance().readImageFromDesktopShared() );
      emit shareDesktopImageAvailable( u, img, sdd.imageType(), sdd.diffColor() );
    }
    else
    {
      refuseToViewShareDesktop( ID_LOCAL_USER, u.id() );
      qDebug() << "You have refused to view the shared desktop because the option is disabled";
    }
  }
  else
  {
    qWarning() << "Invalid flag found in desktop share message from user" << qPrintable( u.path() );
  }
}

bool Core::sendScreenshotToChat( VNumber chat_id )
{
  Chat c = ChatManager::instance().chat( chat_id );
  if( !c.isValid() )
  {
    qWarning() << "Invalid chat" << chat_id << "found in Core::sendScreenshotToChat(...)";
    return false;
  }

  Screenshot screen_shot;
  screen_shot.grabPrimaryScreen();

  if( !screen_shot.isValid() )
  {
    qWarning() << "Invalid pixmap captured from desktop in Core::sendScreenshotToChat(...)";
    return false;
  }

  QString screenshot_format = Settings::instance().shareDesktopImageType();
  QString screenshot_initial_path = Settings::instance().cacheFolder() +
                                    QString( "/beesshottmp-%1." ).arg( Bee::dateTimeStringSuffix( QDateTime::currentDateTime() ) )
                                    + screenshot_format;
  QString file_path = Bee::uniqueFilePath( screenshot_initial_path, false );

  if( !screen_shot.save( file_path, screenshot_format.toLatin1() ) )
  {
    qWarning() << "Unable to save temporary screenshot in file" << qPrintable( file_path );
    return false;
  }

  Settings::instance().addTemporaryFilePath( file_path );
  screen_shot.reset();

  foreach( VNumber user_id, c.usersId() )
  {
    if( user_id != ID_LOCAL_USER)
      sendFile( user_id, file_path, "", false, chat_id );
  }

  return true;
}
