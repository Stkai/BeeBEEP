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
#include "GuiWizard.h"
#include "IconManager.h"
#include "Settings.h"


GuiWizard::GuiWizard( QWidget *parent )
  : QDialog( parent )
{
  setupUi( this );
  setWindowTitle( Settings::instance().programName() );
  Bee::removeContextHelpButton( this );

  mp_lPix->setPixmap( IconManager::instance().icon( "beebeep.png" ).pixmap( QSize( 128, 128 ) ) );
  mp_lWelcome->setText( tr( "Welcome to <b>%1 Network</b>." ).arg( Settings::instance().programName() ) );
  m_userName = "";
  connect( mp_pbChangeUser, SIGNAL( clicked() ), this, SLOT( saveSettings() ) );
  connect( mp_pbCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

void GuiWizard::loadSettings()
{
  mp_lAccount->setText( QString( "%1: %2\n\n%3" ).arg( tr( "Your system account is" ) ).arg( Settings::instance().localUser().accountName() ).arg( tr( "Press ok to continue or change your username.") ) );

  if( Settings::instance().allowEditNickname() )
  {
    mp_leName->setEnabled( true );
    mp_leName->setReadOnly( false );
    mp_leName->setToolTip( "" );
    if( Settings::instance().localUser().name() == Settings::instance().localUser().accountName() )
    {
      QString display_name = Settings::instance().localUser().name();
      display_name.replace( QChar( '.' ), QChar( ' ' ) );
      display_name.replace( QChar( '_' ), QChar( ' ' ) );
      mp_leName->setText( Bee::capitalizeFirstLetter( display_name, true ) );
    }
    else
      mp_leName->setText( Settings::instance().localUser().name() );
  }
  else
  {
    mp_leName->setEnabled( false );
    mp_leName->setReadOnly( true );
    mp_leName->setToolTip( tr( "Disabled by system administrator" ) );
    mp_leName->setText( Settings::instance().localUser().name() );
  }
}

void GuiWizard::saveSettings()
{
  QString user_name = mp_leName->text().simplified();
  if( user_name.isEmpty() )
  {
    QMessageBox::information( this, Settings::instance().programName(), tr( "Your nickname can not be empty." ) );
    mp_leName->setFocus();
    return;
  }

  m_userName = user_name;
  accept();
}

