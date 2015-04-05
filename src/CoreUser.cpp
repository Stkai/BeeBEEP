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
  Settings::instance().setLocalUser( u );
  showUserStatusChanged( u );
  if( isConnected() )
    sendLocalUserStatus();
}

void Core::setLocalUserStatusDescription( const QString& new_status_description )
{
  if( Settings::instance().localUser().statusDescription() == new_status_description )
    return;
  User u = Settings::instance().localUser();
  u.setStatusDescription( new_status_description );
  Settings::instance().setLocalUser( u );
  showUserStatusChanged( u );
  if( isConnected() )
    sendLocalUserStatus();
}

void Core::showUserStatusChanged( const User& u )
{
  emit userChanged( u );

  if( !isConnected() )
    return;

  QString sHtmlMsg = Bee::iconToHtml( Bee::userStatusIconFileName( u.status() ), "*S*" ) + QString( " " );
  if( u.isLocal() )
    sHtmlMsg += tr( "You are" );
  else
    sHtmlMsg += tr( "%1 is" ).arg( u.name() );
   sHtmlMsg += QString( " %1%2." ).arg( Bee::userStatusToString( u.status() ) )
                            .arg( (u.statusDescription().isEmpty() || u.status() == User::Offline) ? "" : QString( ": %1").arg( u.statusDescription() ) );
  dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), sHtmlMsg, DispatchToAllChatsWithUser );
}

void Core::showUserNameChanged( const User& u, const QString& old_user_name )
{
  QString sHtmlMsg = Bee::iconToHtml( ":/images/profile.png", "*N*" ) + QString( " " );
  if( u.isLocal() )
    sHtmlMsg += tr( "You have changed your nickname from %1 to %2." ).arg( old_user_name, u.name() );
  else
    sHtmlMsg += tr( "%1 has changed the nickname in %2." ).arg( old_user_name, u.name() );

  dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), sHtmlMsg, DispatchToAllChatsWithUser );
  emit userChanged( u );
}

void Core::showUserVCardChanged( const User& u )
{
  QString sHtmlMsg = "";
  if( !u.isLocal() )
  {
    sHtmlMsg += QString( "%1 %2" ).arg( Bee::iconToHtml( ":/images/profile.png", "*V*" ),
                                        tr( "The %1's profile has been received." ).arg( u.name() ) );
  }

  if( u.isBirthDay() )
  {
    if( !sHtmlMsg.isEmpty() )
      sHtmlMsg += QString( "<br />" );

    sHtmlMsg += QString( "%1 <b>%2</b>" ).arg( Bee::iconToHtml( ":/images/birthday.png", "*!*" ),
                                        (u.isLocal() ? tr( "Happy Birthday to you!" ) : tr( "Happy Birthday to %1!" ).arg( u.name() ) ) );
  }

  if( !u.vCard().info().isEmpty() )
  {
    if( !sHtmlMsg.isEmpty() )
      sHtmlMsg += QString( "<br />" );
    sHtmlMsg += QString( "%1 %2" ).arg( Bee::iconToHtml( ":/images/info.png", "*I*" ),
                                    (u.isLocal() ? tr( "You share this information" ) : tr( "%1 shares this information" ).arg( u.name() )) );
    sHtmlMsg += QString( ":<br />%1" ).arg( u.vCard().info() );
  }

  if( sHtmlMsg.isEmpty() )
    return;

  dispatchSystemMessage( ID_DEFAULT_CHAT, u.id(), sHtmlMsg, DispatchToAllChatsWithUser );
  emit userChanged( u );
}

void Core::sendLocalUserStatus()
{
  QByteArray user_status_message = Protocol::instance().localUserStatusMessage();
  foreach( Connection *c, m_connections )
    c->sendData( user_status_message );
}

void Core::setLocalUserVCard( const VCard& vc )
{
  User u = Settings::instance().localUser();
  bool nick_name_changed = u.vCard().nickName() != vc.nickName();
  u.setVCard( vc );
  Settings::instance().setLocalUser( u );

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
}

void Core::createGroup( const QString& group_name, const QList<VNumber>& group_members )
{
  Group g = Protocol::instance().createGroup( group_name, group_members );
  addGroup( g );
}

void Core::addGroup( const Group& g )
{
  UserManager::instance().setGroup( g );
  emit updateGroup( g.id() );
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

  Chat c = ChatManager::instance().findGroupChatByPrivateId( g.privateId() );
  if( !c.isValid() )
  {
    qWarning() << "Chat not found for group" << g.name();
    return;
  }

  changeGroupChat( c.id(), group_name, group_members, true );
}

void Core::removeGroup( VNumber group_id )
{
  if( UserManager::instance().removeGroup( group_id ) )
  {
    emit updateGroup( group_id );
  }
}

void Core::loadGroups()
{
  QStringList group_list = Settings::instance().groupList();
  if( group_list.isEmpty() )
    return;

  Group g;
  foreach( QString group_data, group_list )
  {
    g = Protocol::instance().loadGroup( group_data );
    if( g.isValid() )
      addGroup( g );
  }
}

void Core::saveGroups()
{
  QStringList group_save_data;
  if( !UserManager::instance().groups().isEmpty() )
  {
    foreach( Group g, UserManager::instance().groups() )
    {
      group_save_data.append( Protocol::instance().saveGroup( g ) );
    }
  }
  Settings::instance().setGroupList( group_save_data );
}
