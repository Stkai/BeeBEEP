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

#include "GuiNetworkLogin.h"
#include "Settings.h"


GuiNetworkLogin::GuiNetworkLogin( QWidget* parent )
  : QDialog( parent )
{
  setupUi( this );
  setWindowTitle( tr( "Login - %1").arg( Settings::instance().programName() ) );

  connect( mp_pbLogin, SIGNAL( clicked() ), this, SLOT( doLogin() ) );
  connect( mp_pbCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

void GuiNetworkLogin::loadSettings()
{
  QString passwd = Settings::instance().networkAccountPassword();
  if( passwd.isEmpty() )
  {
    mp_lePassword->setText( "" );
    mp_cbRememberPassword->setChecked( false );
    mp_lePassword->setFocus();
  }
  else
  {
    mp_lePassword->setText( passwd );
    mp_cbRememberPassword->setChecked( true );
  }

  QString jid = Settings::instance().networkAccountUser();
  if( jid.isEmpty() )
  {
    mp_leUser->setText( "" );
    mp_cbRememberMe->setChecked( false );
    mp_leUser->setFocus();
  }
  else
  {
    mp_leUser->setText( jid );
    mp_cbRememberMe->setChecked( true );
  }

  mp_cbAutomaticConnection->setChecked( Settings::instance().autoConnectToNetworkAccount() );
}

QString GuiNetworkLogin::user() const
{
  return mp_leUser->text().trimmed();
}

QString GuiNetworkLogin::password() const
{
  return mp_lePassword->text();
}

void GuiNetworkLogin::doLogin()
{
  QString user_name = user();
  if( user_name.isEmpty() )
  {
    QMessageBox::information( this, Settings::instance().programName(), tr( "Please insert the username (JabberId)") );
    mp_leUser->setFocus();
    return;
  }

  QString user_password = password();
  if( user_password.isEmpty() )
  {
    QMessageBox::information( this, Settings::instance().programName(), tr( "Please insert the password" ) );
    mp_lePassword->setFocus();
    return;
  }

  if( !mp_cbRememberMe->isChecked() )
    user_name = "";
  if( !mp_cbRememberPassword->isChecked() )
    user_password = "";

  Settings::instance().setNetworkAccount( user_name, user_password, mp_cbAutomaticConnection->isChecked() );

  accept();
}
