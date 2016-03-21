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
#include "GuiVCard.h"
#include "PluginManager.h"
#include "Avatar.h"
#include "Settings.h"
#include "UserManager.h"
#include "ChatManager.h"


GuiVCard::GuiVCard( QWidget *parent )
 : QWidget( parent )
{
  setupUi( this );
  setObjectName( "GuiVCard" );

  setWindowFlags( Qt::Popup );
  setAttribute( Qt::WA_DeleteOnClose );

  connect( mp_pbChat, SIGNAL( clicked() ), this, SLOT( showPrivateChat() ) );
  connect( mp_pbFile, SIGNAL( clicked() ), this, SLOT( sendFile() ) );
  connect( mp_pbColor, SIGNAL( clicked() ), this, SLOT( changeColor() ) );
  connect( mp_pbFavorite, SIGNAL( clicked() ), this, SLOT( favoriteClicked() ) );
  connect( mp_pbRemove, SIGNAL( clicked() ), this, SLOT( removeUserClicked() ) );
}

void GuiVCard::setVCard( const User& u, VNumber chat_id, bool core_is_connected )
{
  m_userId = u.id();
  m_chatId = chat_id;
  mp_lPath->setText( u.accountPath() );

  QString name_txt = QString( "<b>%1</b>" ).arg( u.vCard().hasFullName() ? u.vCard().fullName() : u.name() );
  mp_lName->setText( QString( "<font color=""%1"">%2</font>" ).arg( u.color(), name_txt ) );

  if( u.vCard().birthday().isValid() )
    mp_lBirthday->setText( tr( "Birthday: %1" ).arg( u.vCard().birthday().toString( Qt::SystemLocaleShortDate ) ) );
  else
    mp_lBirthday->setText( "" );

  if( !u.vCard().email().isEmpty() )
    mp_lEmail->setText( u.vCard().email() );
  else
    mp_lEmail->setText( "" );

  if( !u.vCard().photo().isNull() )
    mp_lPhoto->setPixmap( u.vCard().photo() );
  else
    mp_lPhoto->setPixmap( Avatar::create( u.name(), u.color(), QSize( 96, 96 ) ) );

  if( !u.vCard().info().isEmpty() )
    mp_lPhoto->setToolTip( u.vCard().info() );
  else
    mp_lPhoto->setToolTip( u.name() );

  if( !u.vCard().phoneNumber().isEmpty() )
    mp_lPhone->setText( u.vCard().phoneNumber() );
  else
    mp_lPhone->setText( "" );

  QString user_version = "";
  if( u.version().isEmpty() )
    user_version = tr( "unknown" );
  else if( u.version() < Settings::instance().version( false ) )
    user_version = tr( "old %1" ).arg( u.version() );
  else if( u.version() > Settings::instance().version( false ) )
    user_version = tr( "new %1" ).arg( u.version() );
  else if( u.qtVersion() != Settings::instance().localUser().qtVersion() )
    user_version = QString( "v%1qt%2" ).arg( u.version(), u.qtVersion() );

  int user_current_status = core_is_connected ? u.status() : User::Offline;

  QString user_status = QString( "<img src='%1' width=16 height=16 border=0 /> <b>%2</b>" ).arg( Bee::userStatusIconFileName( user_current_status ), Bee::userStatusToString( user_current_status ) );

  if( user_version.isEmpty() )
    mp_lStatus->setText( user_status );
  else
    mp_lStatus->setText( QString( "%1&nbsp;&nbsp;&nbsp;(%2)" ).arg( user_status ).arg( user_version ) );

  if( u.isFavorite() )
  {
    mp_pbFavorite->setIcon( QIcon( ":/images/star.png" ) );
    mp_pbFavorite->setToolTip( tr( "Remove from favorites" ) );
  }
  else
  {
    mp_pbFavorite->setIcon( QIcon( ":/images/star-bn.png" ) );
    mp_pbFavorite->setToolTip( tr( "Add to favorites" ) );
  }

  if( u.isLocal() )
  {
    mp_pbChat->setToolTip( tr( "Chat with all" ) );
    mp_pbFile->hide();
    mp_pbFavorite->hide();
  }
  else
  {
    mp_pbChat->setToolTip( tr( "Open chat" ) );
    if( u.isStatusConnected() )
      mp_pbFile->show();
    else
      mp_pbFile->hide();

    if( Settings::instance().saveUserList() )
      mp_pbFavorite->show();
    else
      mp_pbFavorite->hide();
  }

  if( u.isStatusConnected() || UserManager::instance().isUserInGroups( u.id() )
          || ChatManager::instance().userIsInGroupChat( u.id() ) )
    mp_pbRemove->hide();
  else
    mp_pbRemove->show();

#ifdef BEEBEEP_DEBUG
  qDebug() << "VCard shown for the user" << u.path();
#endif
}

void GuiVCard::showPrivateChat()
{
  hide();
  emit showChat( m_chatId );
  close();
}

void GuiVCard::sendFile()
{
  hide();
  emit sendFile( m_userId );
  close();
}

void GuiVCard::changeColor()
{
  hide();
  emit changeUserColor( m_userId );
  close();
}

void GuiVCard::favoriteClicked()
{
  hide();
  emit toggleFavorite( m_userId );
  close();
}

void GuiVCard::removeUserClicked()
{
  hide();
  emit removeUser( m_userId );
  close();
}
