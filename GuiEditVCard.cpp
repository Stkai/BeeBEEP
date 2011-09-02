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

#include "GuiEditVCard.h"
#include "Settings.h"


GuiEditVCard::GuiEditVCard( QWidget *parent )
  : QDialog( parent ), m_user()
{
  setupUi( this );
  setObjectName( "GuiEditVCard" );

  m_bgGender.addButton( mp_rbMale, 0 );
  m_bgGender.addButton( mp_rbFemale, 1 );
  m_bgGender.setExclusive( true );
  mp_rbMale->setChecked( true );

  connect( mp_pbOk, SIGNAL( clicked() ), this, SLOT( checkData() ) );
  connect( mp_pbCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
  connect( mp_pbPhoto, SIGNAL( clicked() ), this, SLOT( changePhoto() ) );
}

void GuiEditVCard::setVCard( const VCard& vc )
{
  m_vCard = vc;
  loadVCard();
}

void GuiEditVCard::loadVCard()
{
  mp_leNickname->setText( m_vCard.nickName() );
  mp_leFirstName->setText( m_vCard.firstName() );
  mp_leLastName->setText( m_vCard.lastName() );
  QAbstractButton* ab = m_bgGender.button( m_vCard.gender() );
  if( ab )
    ab->setChecked( true );
  else
    mp_rbMale->setChecked( true );
  if( m_vCard.birthday().isValid() )
    mp_deBirthday->setDate( m_vCard.birthday() );
  else
    mp_deBirthday->setDate( QDate( 1900, 1, 1 ) );
  mp_leEmail->setText( m_vCard.email() );

  if( m_vCard.photo().isNull() )
    mp_lPhoto->setPixmap( QIcon( ":/images/beebeep.png" ).pixmap( 96, 96 ) );
  else
    mp_lPhoto->setPixmap( m_vCard.photo() );

  mp_leNickname->setFocus();
}

void GuiEditVCard::changePhoto()
{
  QString photo_path = QFileDialog::getOpenFileName( this, tr( "%1 - Select your profile photo" ).arg( Settings::instance().programName() ),
                                                     Settings::instance().lastDirectorySelected(), tr( "Images (*.png *.xpm *.jpg *.jpeg)" ) );
  if( photo_path.isNull() || photo_path.isEmpty() )
    return;

  QImage img;
  QImageReader img_reader( photo_path );
  img_reader.setAutoDetectImageFormat( true );
  img_reader.setScaledSize( QSize( 96, 96 ) );
  if( !img_reader.read( &img ) )
  {
    QMessageBox::warning( this, Settings::instance().programName(), tr( "Unable to load image %1." ).arg( photo_path ), QMessageBox::Ok );
    return;
  }
  QPixmap pix = QPixmap::fromImage( img );
  mp_lPhoto->setPixmap( pix );
  m_vCard.setPhoto( pix );
}

bool GuiEditVCard::checkLineEdit( QLineEdit* ple, const QString& msg )
{
  QString s = ple->text().simplified();
  if( s.isEmpty() )
  {
    QMessageBox::information( this, Settings::instance().programName(), msg, QMessageBox::Ok );
    ple->setFocus();
    return false;
  }
  return true;
}

void GuiEditVCard::checkData()
{
  if( !checkLineEdit( mp_leNickname, tr( "Please insert your nickname." ) ) )
    return;

  m_vCard.setNickName( mp_leNickname->text().simplified() );
  m_vCard.setFirstName( mp_leFirstName->text().simplified() );
  m_vCard.setLastName( mp_leLastName->text().simplified() );
  if( m_bgGender.checkedId() == 1 )
    m_vCard.setGender( VCard::Female );
  else
    m_vCard.setGender( VCard::Male );
  if( mp_deBirthday->date() != QDate( 1900, 1, 1 ) )
    m_vCard.setBirthday( mp_deBirthday->date() );
  m_vCard.setEmail( mp_leEmail->text().simplified() );

  accept();
}
