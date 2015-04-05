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

#include "GuiAskPassword.h"
#include "Settings.h"


GuiAskPassword::GuiAskPassword( QWidget* parent )
 : QDialog( parent ), m_bgPasswordType()
{
  setupUi( this );
  setObjectName( "GuiAskPassword" );
  setWindowTitle( tr( "Chat Password - %1" ).arg( Settings::instance().programName() ) );

  m_bgPasswordType.addButton( mp_rbSelectPassaword, 0 );
  m_bgPasswordType.addButton( mp_rbUseDefaultPassword, 1 );
  m_bgPasswordType.setExclusive( true );

  connect( mp_pbOk, SIGNAL( clicked() ), this, SLOT( okPressed() ) );
  connect( mp_pbCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
  connect( &m_bgPasswordType, SIGNAL( buttonClicked( int ) ), this, SLOT( somethingChanged() ) );
  connect( mp_lePassword, SIGNAL( returnPressed() ), this, SLOT( okPressed() ) );
}

void GuiAskPassword::loadData()
{
  if( Settings::instance().useDefaultPassword() )
  {
    mp_rbUseDefaultPassword->setChecked( true );
    mp_lePassword->setText( "" );
  }
  else
  {
    mp_rbSelectPassaword->setChecked( true );
    if( Settings::instance().savePassword() )
    {
      mp_cbSavePassword->setChecked( true );
      mp_lePassword->setText( Settings::instance().passwordBeforeHash() );
    }
    else
    {
      mp_cbSavePassword->setChecked( false );
      mp_lePassword->setText( Settings::instance().defaultPassword() );
      mp_lePassword->setSelection( 0, Settings::instance().defaultPassword().size() );
    }
  }

  mp_cbAutoStart->setChecked( Settings::instance().askPasswordAtStartup() );
  somethingChanged();
}

void GuiAskPassword::somethingChanged()
{
  if( mp_rbUseDefaultPassword->isChecked() )
  {
    mp_lePassword->setEnabled( false );
    mp_cbSavePassword->setEnabled( false );
    mp_cbSavePassword->setChecked( false );
    mp_labelPasswordNote->setEnabled( false );
  }
  else
  {
    mp_lePassword->setEnabled( true );
    mp_cbSavePassword->setEnabled( true );
    mp_labelPasswordNote->setEnabled( true );
    mp_lePassword->setFocus();
  }
}

void GuiAskPassword::okPressed()
{
  if( mp_rbSelectPassaword->isChecked() )
  {
    if( mp_lePassword->text().simplified().isEmpty() )
    {
      QMessageBox::warning( this, Settings::instance().programName(), tr( "Password is empty. Please enter a valid one (spaces are removed)." ) );
      mp_lePassword->setFocus();
      return;
    }

    Settings::instance().setPassword( mp_lePassword->text().simplified() );
    Settings::instance().setSavePassword( mp_cbSavePassword->isChecked() );
    Settings::instance().setUseDefaultPassword( false );
  }
  else
  {
    Settings::instance().setUseDefaultPassword( true );
    Settings::instance().setPassword( Settings::instance().defaultPassword() );
  }

  Settings::instance().setAskPasswordAtStartup( mp_cbAutoStart->isChecked() );

  accept();
}


