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
#include "GuiVCard.h"
#include "PluginManager.h"
#include "Settings.h"
#include "User.h"


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
}

void GuiVCard::setVCard( const User& u, VNumber chat_id )
{
  m_userId = u.id();
  m_chatId = chat_id;
  mp_lPath->setText( u.accountPath() );

  QString name_txt = QString( "<b>%1</b>" ).arg( u.vCard().hasFullName() ? u.vCard().fullName() : u.name() );

  if( !u.isLocal() )
    name_txt = QString( "<font color=""%1"">%2</font>" ).arg( u.color(), name_txt );

  mp_lName->setText( name_txt );

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
    mp_lPhoto->setPixmap( QIcon( ":/images/beebeep.png").pixmap( 96, 96 ) );

  QString user_version = "";
  if( u.version().isEmpty() )
    user_version = tr( "use old version" ).arg( u.version() );
  else if( u.version() < Settings::instance().version( false ) )
    user_version = tr( "use old %1" ).arg( u.version() );
  else if( u.version() > Settings::instance().version( false ) )
    user_version = tr( "use new %1" ).arg( u.version() );

  QString user_status = QString( "<img src='%1' width=16 height=16 border=0 /> <b>%2</b>" ).arg( Bee::userStatusIconFileName( u.status() ), Bee::userStatusToString( u.status() ) );

  if( user_version.isEmpty() )
    mp_lStatus->setText( user_status );
  else
    mp_lStatus->setText( QString( "%1&nbsp;&nbsp;&nbsp;(%2)" ).arg( user_status ).arg( user_version ) );

  if( u.isLocal() )
    mp_pbChat->setToolTip( tr( "Chat with all" ) );
  else
    mp_pbChat->setToolTip( tr( "Open chat" ) );

  if( u.isLocal() )
    mp_pbFile->hide();
  else
    mp_pbFile->show();

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
