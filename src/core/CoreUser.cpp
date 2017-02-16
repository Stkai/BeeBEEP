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
#include "Broadcaster.h"
#include "Protocol.h"
#include "Settings.h"
#include "UserManager.h"


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
  QString sHtmlMsg = Bee::iconToHtml( ":/images/profile.png", "*N*" ) + QString( " " );

  if( u.isLocal() )
    sHtmlMsg += tr( "You have changed your nickname from %1 to %2." ).arg( old_user_name, u.name() );
  else
    sHtmlMsg += tr( "%1 has changed the nickname in %2." ).arg( old_user_name, u.name() );

  dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), sHtmlMsg, DispatchToAllChatsWithUser, ChatMessage::UserInfo );

  emit userChanged( u );
}

void Core::showUserVCardChanged( const User& u )
{
  QString sHtmlMsg = "";

  if( u.isBirthDay() )
  {
    sHtmlMsg = QString( "%1 <b>%2</b>" ).arg( Bee::iconToHtml( ":/images/birthday.png", "*!*" ),
                                        (u.isLocal() ? tr( "Happy Birthday to you!" ) : tr( "Happy Birthday to %1!" ).arg( u.name() ) ) );
    dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), sHtmlMsg, DispatchToAllChatsWithUser, ChatMessage::UserInfo );
  }

  if( !u.vCard().info().isEmpty() )
  {
    sHtmlMsg = QString( "%1 %2" ).arg( Bee::iconToHtml( ":/images/info.png", "*I*" ),
                                    (u.isLocal() ? tr( "You share this information" ) : tr( "%1 shares this information" ).arg( u.name() )) );
    sHtmlMsg += QString( ": <b>%1</b>" ).arg( u.vCard().info() );
    if( u.isLocal() )
      dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), sHtmlMsg, DispatchToChat, ChatMessage::UserInfo );
    else
      dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), sHtmlMsg, DispatchToAllChatsWithUser, ChatMessage::UserInfo );
  }
}

void Core::sendLocalUserStatus()
{
  QByteArray user_status_message = Protocol::instance().localUserStatusMessage();
  foreach( Connection *c, m_connections )
    c->sendData( user_status_message );
}

bool Core::setLocalUserVCard( const QString& user_color, const VCard& vc )
{
  User u = Settings::instance().localUser();
  bool nick_name_changed = false;
  bool color_changed = false;
  bool vc_changed = true;

  if( u.vCard().nickName() != vc.nickName() )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Local user nickname is changed";
#endif
    nick_name_changed = true;
  }

  if( u.color() != user_color )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Local user color is changed";
#endif
    u.setColor( user_color );
    color_changed = true;
  }

  if( u.vCard() == vc )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Local user vCard is changed";
#endif
    vc_changed = false;
  }
  else
    u.setVCard( vc );

  if( !nick_name_changed && !color_changed && !vc_changed )
    return false;

  Settings::instance().setLocalUser( u );
  Settings::instance().save();

  QByteArray vcard_message = Protocol::instance().localVCardMessage();
  QByteArray nick_message = Protocol::instance().localUserNameMessage();

  foreach( Connection *c, m_connections )
  {
    if( c->protoVersion() == 1 )
    {
      if( nick_name_changed )
        c->sendData( nick_message );
    }
    else
      c->sendData( vcard_message );
  }

  showUserVCardChanged( u );
  emit userChanged( u );
  return true;
}

bool Core::createGroup( const QString& group_name, const QList<VNumber>& group_members, const QString& group_private_id )
{
  if( UserManager::instance().hasGroupName( group_name ) )
  {
    qWarning() << "Unable to create group" << group_name << ": it already exists";
    return false;
  }

  Group g = Protocol::instance().createGroup( group_name, group_private_id, group_members );
  qDebug() << "Group" << g.name() << "created with id:" << g.privateId();
  addGroup( g, true );
  return true;
}

bool Core::createGroupFromChat( VNumber chat_id )
{
  Chat c = ChatManager::instance().chat( chat_id );
  if( !c.isValid() )
  {
    qWarning() << "Invalid chat found" << chat_id << "in create group from chat";
    return false;
  }

  if( !c.isGroup() )
  {
    qWarning() << "Chat" << c.id() << c.name() << "is private and can not become a group chat";
    return false;
  }

  if( createGroup( c.name(), c.usersId(), c.privateId() ) )
  {
    QString sHtmlMsg = tr( "%1 You have created group from chat: %2." ).arg( Bee::iconToHtml( ":/images/group.png", "*G*" ), c.name() );
    dispatchSystemMessage( c.id(), ID_LOCAL_USER, sHtmlMsg, DispatchToChat, ChatMessage::System );
    return true;
  }
  else
    return false;
}

void Core::addGroup( const Group& g, bool broadcast_to_members )
{
  UserManager::instance().setGroup( g );
  emit updateGroup( g.id() );

  Chat c = ChatManager::instance().findChatByPrivateId( g.privateId(), true, ID_INVALID );
  if( !c.isValid() )
    createGroupChat( g, broadcast_to_members );
}

void Core::changeGroup( VNumber group_id, const QString& group_name, const QList<VNumber>& group_members )
{
  Group g = UserManager::instance().group( group_id );
  if( !g.isValid() )
    return;

  g.setName( group_name );
  g.setUsers( group_members );
  UserManager::instance().setGroup( g );

  emit updateGroup( g.id() );

  Chat c = ChatManager::instance().findChatByPrivateId( g.privateId(), true, ID_INVALID );
  if( !c.isValid() )
  {
    qWarning() << "Chat not found for group" << g.name();
    return;
  }

  changeGroupChat( c.id(), group_name, group_members, true );
}

bool Core::removeGroup( VNumber group_id )
{
  Group g = UserManager::instance().group( group_id );
  if( !g.isValid() )
  {
    qWarning() << "Invalid group id" << group_id << "found in remove group";
    return false;
  }

  Chat c = ChatManager::instance().findChatByPrivateId( g.privateId(), true, ID_INVALID );
  if( c.isValid() )
    removeChat( c.id() );

  if( UserManager::instance().removeGroup( group_id ) )
  {
    QString sHtmlMsg = tr( "%1 You have deleted group: %2." ).arg( Bee::iconToHtml( ":/images/group-remove.png", "*G*" ), g.name() );
    dispatchSystemMessage( ID_DEFAULT_CHAT, ID_LOCAL_USER, sHtmlMsg, DispatchToChat, ChatMessage::Other );
    return true;
  }
  else
    return false;
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
        qDebug() << "Loading user:" << u.path();
        UserManager::instance().setUser( u );
        createPrivateChat( u );
      }
    }
  }

  if( !Settings::instance().groupList().isEmpty() )
  {
    Group g;
    foreach( QString group_data, Settings::instance().groupList() )
    {
      g = Protocol::instance().loadGroup( group_data );
      if( g.isValid() )
        addGroup( g, false );
    }
  }

  QList<Group> group_list = Protocol::instance().loadGroupsFromFile();
  if( !group_list.isEmpty() )
  {
    foreach( Group g, group_list )
    {
      if( g.isValid() )
        addGroup( g, false );
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
        save_data.append( Protocol::instance().saveUser( u ) );
    }
  }

  Settings::instance().setUserList( save_data );
  qDebug() << save_data.size() << "users are now stored in settings";

  if( !save_data.isEmpty() )
    save_data.clear();

  if( !UserManager::instance().groups().isEmpty() )
  {
    foreach( Group g, UserManager::instance().groups() )
    {
      save_data.append( Protocol::instance().saveGroup( g ) );
    }
  }

  Settings::instance().setGroupList( save_data );
  qDebug() << save_data.size() << "groups are now stored in settings";
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
    favorite_icon = Bee::iconToHtml( ":/images/star-bn.png", "*V*" );
    favorite_txt = tr( "is removed from favorites" );
  }
  else
  {
    u.setIsFavorite( true );
    favorite_icon = Bee::iconToHtml( ":/images/star.png", "*V*" );
    favorite_txt = tr( "is added to favorites" );
  }

  UserManager::instance().setUser( u );

  QString sHtmlMsg = QString( "%1 %2 %3." ).arg( favorite_icon, u.name(), favorite_txt );
  dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), sHtmlMsg, DispatchToDefaultAndPrivateChat, ChatMessage::UserInfo );
}

bool Core::isUserConnected( VNumber user_id ) const
{
  // Is not equal to User::isStatusConnected because status is passed in hello message
  // before the authentication. Use this function in authentication and in Core.
  // Use User::isStatusConnected only in GUI
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

bool Core::areUsersConnected( const QList<VNumber>& users_id ) const
{
  foreach( VNumber user_id, users_id )
  {
    if( !isUserConnected( user_id ) )
      return false;
  }
  return true;
}

bool Core::removeOfflineUser( VNumber user_id )
{
  User u = UserManager::instance().findUser( user_id );
  if( !u.isValid() )
  {
    qWarning() << "User" << user_id << "is already removed from list";
    return false;
  }

  if( isUserConnected( u.id() ) )
  {
    qWarning() << "User" << u.path() << "is connected and cannot be removed from list";
    return false;
  }

  if( UserManager::instance().isUserInGroups( u.id() ) )
  {
    qWarning() << "User" << u.path() << "is in some groups and cannot be removed from list";
    return false;
  }

  if( ChatManager::instance().userIsInGroupChat( u.id() ) )
  {
    qWarning() << "User" << u.path() << "is in a group chat and cannot be removed from list";
    return false;
  }

  if( UserManager::instance().removeUser( u ) )
  {
    qDebug() << "User" << u.path() << "is removed from list";
    return true;
  }
  else
  {
    qWarning() << "User" << u.path() << "cannot be removed from list";
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

    msg_to_send = Protocol::instance().userRecordListToMessage( user_record_list );
    if( sendMessageToLocalNetwork( to_user, msg_to_send ) )
    {
#ifdef BEEBEEP_DEBUG
      qDebug() << "Hive protocol sends" << user_record_list.size() << "connected users to" << qPrintable( to_user.path() );
#endif
      return;
    }
    else
      qWarning() << "Hive protocol is unable to send" << user_record_list.size() << "connected users to" << qPrintable( to_user.path() );
  }

#ifdef BEEBEEP_DEBUG
  qDebug() << "Hive protocol tries to send" <<  qPrintable( to_user.path() ) << "to" << user_record_list.size() << "connected users";
#endif

  user_record_list.clear();
  ur.setNetworkAddress( to_user.networkAddress() );
  user_record_list.append( ur );
  msg_to_send = Protocol::instance().userRecordListToMessage( user_record_list );
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
  userChanged( u );
}
