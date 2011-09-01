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

#include "GuiVCard.h"


GuiVCard::GuiVCard( QWidget *parent )
 : QDialog( parent )
{
  setupUi( this );
  setObjectName( "GuiVCard" );

  m_bgGender.addButton( mp_rbMale, 0 );
  m_bgGender.addButton( mp_rbFemale, 1 );
  m_bgGender.setExclusive( true );
  mp_rbMale->setChecked( true );

  connect( mp_pbOk, SIGNAL( clicked() ), this, SLOT( checkData() ) );
  connect( mp_pbCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
  connect( mp_pbPhoto, SIGNAL( clicked() ), this, SLOT( changePhoto() ) );
}

void GuiVCard::setVCard( const VCard& vc )
{
  m_vCard = vc;
  loadVCard();
}

void GuiVCard::loadVCard()
{
  mp_leFirstName->setText( m_vCard.firstName() );
  mp_leLastName->setText( m_vCard.lastName() );
  QAbstractButton* ab = m_bgGender.button( m_vCard.gender() );
  if( ab )
    ab->setChecked( true );
  else
    mp_rbMale->setChecked( true );
  mp_deBirthday->setDate( m_vCard.birthday() );
  mp_leEmail->setText( m_vCard.email() );

  mp_leFirstName->setFocus();
}

void GuiVCard::changePhoto()
{

}

void GuiVCard::checkData()
{
  accept();
}
