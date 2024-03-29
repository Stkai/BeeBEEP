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

#include "BeeUtils.h"
#include "ChatManager.h"
#include "Connection.h"
#include "Core.h"
#include "IconManager.h"
#include "MessageManager.h"
#include "Protocol.h"
#include "Settings.h"
#include "UserManager.h"


void Core::setLocalUserWorkgroups( const QStringList& wkgs )
{
  if( Settings::instance().localUser().workgroups() == wkgs )
    return;
   User u = Settings::instance().localUser();
   u.setWorkgroups( wkgs );
   Settings::instance().setLocalUser( u );
   emit userChanged( u );
}

void Core::setLocalUserStatus( int new_status )
{
  if( Settings::instance().localUser().status() == new_status )
    return;
  User u = Settings::instance().localUser();
  u.setStatus( new_status );
  u.setStatusChangedIn( QDateTime::currentDateTime() );
  Settings::instance().setLocalUser( u );
  emit userChanged( u );
  if( isConnected() )
    sendLocalUserStatus();
}

void Core::setLocalUserStatusDescription( int user_status, const QString& new_status_description, bool show_status )
{
  User u = Settings::instance().localUser();
  if( u.statusDescription() == new_status_description )
    return;

  u.setStatusDescription( new_status_description );
  Settings::instance().setLocalUser( u );

  if( !new_status_description.isEmpty() )
  {
    UserStatusRecord usr;
    usr.setStatus( user_status );
    usr.setStatusDescription( u.statusDescription() );
    QString s_status = Protocol::instance().saveUserStatusRecord( usr );
    QStringList sl_status = Settings::instance().userStatusList();
    sl_status.removeOne( s_status );
    if( sl_status.size() >= Settings::instance().maxUserStatusDescriptionInList() )
      sl_status.removeLast();
    sl_status.prepend( s_status );
    Settings::instance().setUserStatusList( sl_status );
  }

  emit userChanged( u );

  if( show_status )
  {
    if( isConnected() )
      sendLocalUserStatus();
  }
}

void Core::showUserNameChanged( const User& u, const QString& old_user_name )
{
  QString sHtmlMsg = IconManager::instance().toHtml( "profile.png", "*N*" ) + QString( " " );
  if( u.isLocal() )
    sHtmlMsg += tr( "You have changed your nickname from %1 to %2." ).arg( Bee::replaceHtmlSpecialCharacters( old_user_name ), Bee::replaceHtmlSpecialCharacters( u.name() ) );
  else
    sHtmlMsg += tr( "%1 has changed the nickname in %2." ).arg( Bee::replaceHtmlSpecialCharacters( old_user_name ), Bee::replaceHtmlSpecialCharacters( u.name() ) );
  dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), sHtmlMsg, DispatchToAllChatsWithUser, ChatMessage::UserInfo, false );
}

void Core::showUserVCardChanged( const User&, const VCard& )
{
  // Fixme: useful?
}

void Core::sendLocalUserStatus()
{
  Message user_status_message = Protocol::instance().userStatusMessage( Settings::instance().localUser().status(), Settings::instance().localUser().statusDescription() );
  foreach( Connection *c, m_connections )
    c->sendMessage( user_status_message );
}

bool Core::changeLocalUser( const QString& user_name )
{
  if( isConnected() )
  {
    qWarning() << "Unable to change user if core is connected";
    return false;
  }

  QString old_user_name = Settings::instance().localUser().name();
  if( old_user_name.toLower() == user_name.toLower() )
    qWarning() << "User changed nickname only for uppercase/lowercase characters";

  archiveAllChats();

  Settings::instance().createLocalUser( user_name );
  Settings::instance().save();
  showUserNameChanged( Settings::instance().localUser(), old_user_name );
  emit userChanged( Settings::instance().localUser() );
  return true;
}

bool Core::setLocalUserVCard( const QString& user_color, const VCard& vc )
{
  bool color_changed = user_color != Settings::instance().localUser().color();
  bool vc_changed = !(vc == Settings::instance().localUser().vCard());
  bool nickname_changed = vc_changed && vc.nickName() != Settings::instance().localUser().vCard().nickName();

  User u = Settings::instance().localUser();
  QString old_user_nickname = "";

  if( color_changed )
  {
    u.setColor( user_color );
    qDebug() << "Your user color is changed to" << qPrintable( user_color );
  }

  if( nickname_changed )
  {
    old_user_nickname = u.vCard().nickName();
    qDebug() << "Your nickname is changed from" << qPrintable( old_user_nickname ) << "to" << qPrintable( vc.nickName() );
  }

  if( vc_changed )
  {
    u.setVCard( vc );
    qDebug() << "Your profile is changed";
  }

  if( !color_changed && !vc_changed )
    return false;

  VCard old_vcard = u.vCard();
  Settings::instance().setLocalUser( u );
  Settings::instance().save();
  MessageManager::instance().generateSaveMessagesAuthCode();

  sendVCardToAllConnectedUsers();
  showUserVCardChanged( u, old_vcard );
  if( nickname_changed )
    showUserNameChanged( u, old_user_nickname );
  emit userChanged( u );
  return true;
}

void Core::loadUsersAndGroups()
{
  if( !Settings::instance().userList().isEmpty() )
  {
    qDebug() << "Loading" << Settings::instance().userList().size() << "saved users";
    User u;
    foreach( QString user_data, Settings::instance().userList() )
    {
      u = Protocol::instance().loadUser( user_data );
      if( u.isValid() )
      {
        qDebug() << "Loading user:" << qPrintable( u.path() );
        UserManager::instance().setUser( u );
        createPrivateChat( u );
        emit userChanged( u );
      }
    }
  }

  if( !Settings::instance().groupList().isEmpty() )
  {
    Group g;
    qDebug() << "Loading" << Settings::instance().groupList().size() << "saved groups";
    foreach( QString group_data, Settings::instance().groupList() )
    {
      g = Protocol::instance().loadGroup( group_data );
      if( g.isValid() )
      {
        qDebug() << "Loading group chat" << qPrintable( g.name() );
        /* check for temporary users created */
        foreach( VNumber user_id, g.usersId() )
        {
          if( user_id == ID_LOCAL_USER )
            continue;

          if( !ChatManager::instance().privateChatForUser( user_id ).isValid() )
          {
            User u_tmp = UserManager::instance().findUser( user_id );
            createPrivateChat( u_tmp );
            emit userChanged( u_tmp );
          }
        }

#ifdef BEEBEEP_DEBUG
        if( g.lastModified().isValid() )
          qDebug() << "Loag group chat" << qPrintable( g.name() ) << "with last modified date" << qPrintable( g.lastModified().toString( Qt::ISODate ) );
#endif
        if( !ChatManager::instance().findChatByPrivateId( g.privateId(), true, ID_INVALID ).isValid() )
          createGroupChat( Settings::instance().localUser(), g, false );
      }
    }
  }

  if( Settings::instance().removeInactiveUsers() )
    removeInactiveUsers();

  if( !Settings::instance().refusedChats().isEmpty() )
  {
    ChatRecord cr;
    qDebug() << "Loading" << Settings::instance().refusedChats().size() << "refused chats";
    foreach( QString chat_record_data, Settings::instance().refusedChats() )
    {
      cr = Protocol::instance().loadChatRecord( chat_record_data );
      if( cr.isValid() )
        ChatManager::instance().addToRefusedChat( cr );
    }
  }
}

void Core::saveUsersAndGroups()
{
  QStringList save_data;

  if( Settings::instance().saveUserList() )
  {
    foreach( User u, UserManager::instance().userList().toList() )
    {
      if( !u.isLocal() )
      {
        qDebug() << "Saving user" << qPrintable( u.name() );
        save_data.append( Protocol::instance().saveUser( u ) );
      }
    }
  }

  Settings::instance().setUserList( save_data );
  qDebug() << save_data.size() << "users are now stored in settings";

  if( !save_data.isEmpty() )
    save_data.clear();

  if( Settings::instance().saveGroupList() )
  {
    foreach( Chat c, ChatManager::instance().constChatList() )
    {
      if( c.isGroup() )
      {
        if( c.lastModified().isNull() )
        {
          c.setLastModifiedToNow();
          qDebug() << "Saving chat" << qPrintable( c.name() ) << "with last modified set to now";
        }
        else
          qDebug() << "Saving chat" << qPrintable( c.name() );
        save_data.append( Protocol::instance().saveGroup( c.group() ) );
      }
    }
  }

  Settings::instance().setGroupList( save_data );
  qDebug() << save_data.size() << "groups are now stored in settings";

  if( !save_data.isEmpty() )
    save_data.clear();

  if( !ChatManager::instance().refusedChats().isEmpty() )
  {
    foreach( ChatRecord cr, ChatManager::instance().refusedChats() )
    {
      save_data.append( Protocol::instance().saveChatRecord( cr ) );
      qDebug() << "Saving blocked chat" << qPrintable( cr.name() );
    }
  }

  Settings::instance().setRefusedChats( save_data );
  qDebug() << save_data.size() << "refused chats are now stored in settings";
}

void Core::toggleUserFavorite( VNumber user_id )
{
  User u = UserManager::instance().findUser( user_id );
  if( !u.isValid() )
  {
    qWarning() << "Invalid user id" << user_id << "found in toggle favorite function";
    return;
  }

  QString favorite_icon;
  QString favorite_txt;
  if( u.isFavorite() )
  {
    u.setIsFavorite( false );
    favorite_icon = IconManager::instance().toHtml( "star-bn.png", "*V*" );
    favorite_txt = tr( "is removed from favorites" );
  }
  else
  {
    u.setIsFavorite( true );
    favorite_icon = IconManager::instance().toHtml( "star.png", "*V*" );
    favorite_txt = tr( "is added to favorites" );
  }

  UserManager::instance().setUser( u );
  emit userChanged( u );
  QString sHtmlMsg = QString( "%1 %2 %3." ).arg( favorite_icon, u.name(), favorite_txt );
  dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), sHtmlMsg, DispatchToDefaultAndPrivateChat, ChatMessage::UserInfo, false );
}

bool Core::isUserConnected( VNumber user_id ) const
{
  // Is not equal to User::isStatusConnected because status is passed in hello message
  // before the authentication. Use this function in authentication and in Core.
  // Use User::isStatusConnected only in GUI
  if( user_id == ID_INVALID )
  {
    qWarning() << "Invalid ID found in Core::isUserConnected(...)";
    return isConnected();
  }

  if( user_id != ID_LOCAL_USER )
  {
    Connection* c = connection( user_id );
    if( c && c->isConnected() )
      return true;
    else
      return false;
  }
  else
    return isConnected();
}

bool Core::isUserConnected( const NetworkAddress& na ) const
{
  if( !isConnected() )
    return false;

  foreach( Connection* c, m_connections )
  {
    if( c->networkAddress() == na && (c->isConnecting() || c->isConnected()) )
      return true;
  }

  User u = UserManager::instance().findUserByNetworkAddress( na );
  if( u.isValid() )
    return isUserConnected( u.id() );
  else
    return false;
}

bool Core::areUsersConnected( const QList<VNumber>& users_id ) const
{
  if( !isConnected() )
    return false;

  foreach( VNumber user_id, users_id )
  {
    if( !isUserConnected( user_id ) )
      return false;
  }
  return true;
}

bool Core::userCanBeRemoved( const User& u )
{
  if( isUserConnected( u.id() ) )
  {
    qWarning() << "User" << qPrintable( u.name() ) << "is connected and cannot be removed from list";
    return false;
  }

  if( ChatManager::instance().userIsInGroupChat( u.id() ) )
  {
    qWarning() << "User" << qPrintable( u.name() ) << "is in a group chat and cannot be removed from list";
    return false;
  }

  return true;
}

bool Core::removeOfflineUser( VNumber user_id )
{
  User u = UserManager::instance().findUser( user_id );
  if( !u.isValid() )
    return true;

  if( !userCanBeRemoved( u ) )
    return false;

  if( UserManager::instance().removeUser( u ) )
  {
    qDebug() << "User" << qPrintable( u.name() ) << "is removed from list";
    emit userRemoved( u );
    Chat c = ChatManager::instance().privateChatForUser( u.id() );
    if( c.isValid() )
      removeChat( c.id(), true );
    return true;
  }
  else
  {
    qWarning() << "User" << qPrintable( u.name() ) << "cannot be removed from list";
    return false;
  }
}

void Core::sendLocalConnectedUsersTo( const User& to_user )
{
  if( connectedUsers() < 2 )
    return;

  Message msg_to_send;
  QList<UserRecord> user_record_list;
  UserRecord ur;

  if( to_user.protocolVersion() >= HIVE_PROTO_VERSION )
  {
    foreach( User u, UserManager::instance().userList().toList() )
    {
      if( u.isLocal() || u == to_user )
        continue;

      if( isUserConnected( u.id() ) )
      {
        ur.setNetworkAddress( u.networkAddress() );
        user_record_list.append( ur );
      }
    }

    msg_to_send = Protocol::instance().userRecordListToHiveMessage( user_record_list );
    if( sendMessageToLocalNetwork( to_user, msg_to_send ) )
      qDebug() << "Hive protocol sends" << user_record_list.size() << "connected users to" << qPrintable( to_user.path() );
    else
      qWarning() << "Hive protocol is unable to send" << user_record_list.size() << "connected users to" << qPrintable( to_user.path() );
  }

  qDebug() << "Hive protocol shares new user" <<  qPrintable( to_user.path() ) << "to other" << user_record_list.size() << "connected users";
  user_record_list.clear();
  ur.setNetworkAddress( to_user.networkAddress() );
  user_record_list.append( ur );
  msg_to_send = Protocol::instance().userRecordListToHiveMessage( user_record_list );
  foreach( User u, UserManager::instance().userList().toList() )
  {
    if( u.isLocal() || u == to_user )
      continue;

    if( u.protocolVersion() >= HIVE_PROTO_VERSION && isUserConnected( u.id() ) )
    {
      if( !sendMessageToLocalNetwork( u, msg_to_send ) )
        qWarning() << "Hive protocol is unable to send" << qPrintable( to_user.path() ) << "to connected user" << qPrintable( u.path() );
    }
  }
}

void Core::changeUserColor( VNumber user_id, const QString& user_color )
{
  User u = UserManager::instance().findUser( user_id );
  if( !u.isValid() )
  {
    qWarning() << "Invalid user" << user_id << "found in Core::changeUserColor(...)";
    return;
  }

  u.setColor( user_color );
  UserManager::instance().setUser( u );
  if( u.isLocal() )
    sendVCardToAllConnectedUsers();
  emit userChanged( u );
}

void Core::removeInactiveUsers()
{
  QDateTime dt_today = QDateTime::currentDateTime();
  UserList ul_users = UserManager::instance().userList();
  qint64 inactivity_days = 0;
  foreach( User u, ul_users.toList() )
  {
    if( u.isLocal() )
      continue;
    if( u.lastConnection().isValid() )
    {
      inactivity_days = u.lastConnection().daysTo( dt_today );
      if( inactivity_days > Settings::instance().maxDaysOfUserInactivity() )
      {
        qDebug() << "Removing user" << qPrintable( u.name() ) << "due to inactivity of" << inactivity_days << "days";
        removeOfflineUser( u.id() );
      }
    }
    else
    {
      u.setLastConnection( dt_today );
      UserManager::instance().setUser( u );
    }
  }
}

void Core::regenerateLocalUserHash()
{
  User u = Settings::instance().localUser();
  u.setHash( Settings::instance().createLocalUserHash() );
  Settings::instance().setLocalUser( u );
  MessageManager::instance().generateSaveMessagesAuthCode();
}

void Core::sendVCardToAllConnectedUsers()
{
  if( !isConnected() || m_connections.isEmpty() )
    return;
  int count = 0;
  foreach( Connection* c, m_connections )
  {
    Message m = Protocol::instance().localVCardMessage( c->protocolVersion() );
    if( count < Settings::instance().maxUsersToConnectInATick() )
    {
      if( c->sendMessage( m ) )
        count++;
    }
    else
      MessageManager::instance().addMessageToSend( c->userId(), ID_INVALID, m );
  }
}

bool Core::hasHelper() const
{
  UserList ul = UserManager::instance().helpers();
  if( ul.isEmpty() )
    return false;
  foreach( User u, ul.toList() )
  {
    if( isUserConnected( u.id() ) )
      return true;
  }
  return false;
}
