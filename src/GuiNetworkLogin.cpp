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
#include "PluginManager.h"
#include "Settings.h"


GuiNetworkLogin::GuiNetworkLogin( QWidget* parent )
  : QDialog( parent )
{
  setupUi( this );
  setWindowTitle( tr( "Login - %1").arg( Settings::instance().programName() ) );

  connect( mp_pbLogin, SIGNAL( clicked() ), this, SLOT( doLogin() ) );
  connect( mp_pbClose, SIGNAL( clicked() ), this, SLOT( reject() ) );
  connect( mp_pbSave, SIGNAL( clicked() ) , this, SLOT( doSave() ) );

  connect( mp_comboService, SIGNAL( currentIndexChanged( int ) ), this, SLOT( somethingChanged() ) );
  connect( mp_leUser, SIGNAL( textEdited( const QString& ) ), this, SLOT( somethingChanged() ) );
  connect( mp_lePassword, SIGNAL( textEdited( const QString& ) ), this, SLOT( somethingChanged() ) );
  connect( mp_cbRememberMe, SIGNAL( stateChanged( int ) ), this, SLOT( somethingChanged() ) );
  connect( mp_cbRememberPassword, SIGNAL( stateChanged( int ) ), this, SLOT( somethingChanged() ) );
  connect( mp_cbAutomaticConnection, SIGNAL( stateChanged( int ) ), this, SLOT( somethingChanged() ) );
}

void GuiNetworkLogin::setNetworkAccount( const NetworkAccount& na, const QString& network_service, bool is_connected )
{
  int i = 0;
  int current_index = 0;
  foreach( ServiceInterface* si, PluginManager::instance().services() )
  {
    mp_comboService->insertItem( i, si->icon(), si->name() );
    if( na.isValid() )
    {
      if( si->name() == na.service() )
        current_index = i;
    }
    else
    {
      if( si->name() == network_service )
        current_index = i;
    }
    i++;
  }

  if( current_index > 0 )
    mp_comboService->setCurrentIndex( current_index );

  if( na.saveUser() )
    mp_leUser->setText( na.user() );
  else
    mp_leUser->setText( "" );

  if( na.savePassword() )
    mp_lePassword->setText( na.password() );
  else
    mp_lePassword->setText( "" );

  mp_cbRememberMe->setChecked( na.saveUser() );
  mp_cbRememberPassword->setChecked( na.savePassword() );
  mp_cbAutomaticConnection->setChecked( na.autoConnect() );

  if( mp_leUser->text().trimmed().isEmpty() )
    mp_leUser->setFocus();
  else if( mp_lePassword->text().trimmed().isEmpty() )
    mp_lePassword->setFocus();
  else
    mp_pbLogin->setFocus();

  if( is_connected )
    mp_pbLogin->setText( tr( "Disconnect" ) );
  else
    mp_pbLogin->setText( tr( "Connect" ) );

  mp_pbSave->setEnabled( false );
}

bool GuiNetworkLogin::loadDataFromForm( bool perform_check )
{
  QString user_name = mp_leUser->text().trimmed();
  if( perform_check && user_name.isEmpty() )
  {
    QMessageBox::information( this, Settings::instance().programName(), tr( "Please insert the username (JabberId)") );
    mp_leUser->setFocus();
    return false;
  }

  QString user_password = mp_lePassword->text().trimmed();
  if( perform_check && user_password.isEmpty() )
  {
    QMessageBox::information( this, Settings::instance().programName(), tr( "Please insert the password" ) );
    mp_lePassword->setFocus();
    return false;
  }

  m_account.setService( mp_comboService->currentText() );
  m_account.setUser( user_name );
  m_account.setPassword( user_password );
  m_account.setSaveUser( mp_cbRememberMe->isChecked() );
  m_account.setSavePassword( mp_cbRememberPassword->isChecked() );
  m_account.setAutoConnect( mp_cbAutomaticConnection->isChecked() );

  return true;
}

void GuiNetworkLogin::doLogin()
{
  if( !loadDataFromForm( true ) )
    return;
  accept();
}

void GuiNetworkLogin::somethingChanged()
{
  mp_pbSave->setEnabled( true );
}

void GuiNetworkLogin::doSave()
{
  if( !loadDataFromForm( false ) )
    return;
  Settings::instance().setNetworkAccount( m_account );
  mp_pbSave->setEnabled( false );
}

