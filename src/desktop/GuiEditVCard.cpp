//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2019 Marco Mastroddi
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
// along with BeeBEEP. If not, see <http://www.gnu.org/licenses/>.
//
// Author: Marco Mastroddi <marco.mastroddi(AT)gmail.com>
//
// $Id$
//
//////////////////////////////////////////////////////////////////////

#include "Avatar.h"
#include "BeeUtils.h"
#include "FileDialog.h"
#include "GuiEditVCard.h"
#include "IconManager.h"
#include "Settings.h"
#include "UserManager.h"


GuiEditVCard::GuiEditVCard( QWidget *parent )
  : QDialog( parent ), m_vCard(), m_userColor( "#000000" ), m_regenerateUserHash( false )
{
  setupUi( this );
  setObjectName( "GuiEditVCard" );
  setWindowTitle( tr( "Edit your profile" ) + QString( " - %1" ).arg( Settings::instance().programName() ) );
  setWindowIcon( IconManager::instance().icon( "profile-edit.png" ) );
  Bee::removeContextHelpButton( this );

  mp_pbChangePhoto->setIcon( IconManager::instance().icon( "add.png" ) );
  mp_pbRemovePhoto->setIcon( IconManager::instance().icon( "delete.png" ) );
  mp_pbColor->setIcon( IconManager::instance().icon( "font-color.png" ) );

  connect( mp_pbOk, SIGNAL( clicked() ), this, SLOT( onOkClicked() ) );
  connect( mp_pbCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
  connect( mp_pbChangePhoto, SIGNAL( clicked() ), this, SLOT( changePhoto() ) );
  connect( mp_pbRemovePhoto, SIGNAL( clicked() ), this, SLOT( removePhoto() ) );
  connect( mp_pbColor, SIGNAL( clicked() ), this, SLOT( changeUserColor() ) );
  connect( mp_pbRegenerateHash, SIGNAL( clicked() ), this, SLOT( regenerateHash() ) );
}

QString GuiEditVCard::currentAvatarName() const
{
  QString current_avatar_name = "";
  if( Settings::instance().useUserFullName() )
    current_avatar_name = QString( "%1 %2" ).arg( mp_leFirstName->text().simplified() ).arg( mp_leLastName->text().simplified() ).trimmed();

  if( current_avatar_name.isEmpty() )
    current_avatar_name = mp_leNickname->text().simplified();
  return current_avatar_name.isEmpty() ? QLatin1String( "??" ) : current_avatar_name;
}

void GuiEditVCard::setUserColor( const QString& new_value )
{
  m_userColor = new_value;
  QColor c( m_userColor );
  QPalette palette = mp_leNickname->palette();
  palette.setColor( QPalette::Text, c );
  mp_leNickname->setPalette( palette );
  palette.setColor( QPalette::Foreground, c );
  mp_lNickname->setPalette( palette );
  if( m_vCard.photo().isNull() )
    mp_lPhoto->setPixmap( Avatar::create( currentAvatarName(), m_userColor, QSize( 96, 96 ) ) );
}

void GuiEditVCard::setUser( const User& u )
{
  m_vCard = u.vCard();
  setUserColor( u.color() );
  m_regenerateUserHash = false;
  loadVCard();
}

void GuiEditVCard::loadVCard()
{
  mp_leNickname->setText( m_vCard.nickName() );
  if( Settings::instance().allowEditNickname() )
  {
    mp_leNickname->setEnabled( true  );
    mp_leNickname->setReadOnly( false );
    mp_leNickname->setToolTip( "" );
  }
  else
  {
    mp_leNickname->setEnabled( false  );
    mp_leNickname->setReadOnly( true  );
    mp_leNickname->setToolTip( tr( "Disabled by system administrator" ) );
  }
  mp_leFirstName->setText( m_vCard.firstName() );
  mp_leLastName->setText( m_vCard.lastName() );

  if( m_vCard.birthday().isValid() )
    mp_deBirthday->setDate( m_vCard.birthday() );
  else
    mp_deBirthday->setDate( QDate( 1900, 1, 1 ) );

  mp_leEmail->setText( m_vCard.email() );

  if( m_vCard.photo().isNull() )
    mp_lPhoto->setPixmap( Avatar::create( currentAvatarName(), m_userColor, QSize( 96, 96 ) ) );
  else
    mp_lPhoto->setPixmap( m_vCard.photo() );

  mp_lePhone->setText( m_vCard.phoneNumber() );

  mp_teInfo->setPlainText( m_vCard.info() );

  mp_leNickname->setFocus();
}

void GuiEditVCard::changePhoto()
{
  QList<QByteArray> supported_formats = QImageReader::supportedImageFormats();
  QString supported_formats_string = "";

  foreach( QByteArray sf, supported_formats )
  {
    supported_formats_string += QString( " *." );
    supported_formats_string.append( sf );
  }

  QString photo_path = FileDialog::getOpenFileName( true, this, tr( "%1 - Select your profile photo" ).arg( Settings::instance().programName() ),
                                                     Settings::instance().lastDirectorySelected(), tr( "Images" ) + QString( " (%1)" ).arg( supported_formats_string.simplified() ) );
  if( photo_path.isNull() || photo_path.isEmpty() )
    return;

  Settings::instance().setLastDirectorySelectedFromFile( photo_path );

  QImage img;
  QImageReader img_reader( photo_path );
  img_reader.setAutoDetectImageFormat( true );
  if( !img_reader.read( &img ) )
  {
    QMessageBox::warning( this, Settings::instance().programName(), tr( "Unable to load image %1." ).arg( photo_path ), tr( "Ok" ) );
    qWarning() << "Can not load profile picture. Format supported:" << supported_formats_string;
    return;
  }

  if( img.width() != img.height() )
    QMessageBox::information( this, Settings::instance().programName(), tr( "It is preferable to use square images to avoid display problems." ), tr( "Ok" ) );
  QPixmap pix = QPixmap::fromImage( img.scaled( 96, 96, Qt::KeepAspectRatio ) );
  if( !pix.isNull() )
  {
    mp_lPhoto->setPixmap( pix );
    m_vCard.setPhoto( pix );
  }
}

void GuiEditVCard::removePhoto()
{
  mp_lPhoto->setPixmap( Avatar::create( currentAvatarName(), m_userColor, QSize( 96, 96 ) ) );
  m_vCard.setPhoto( QPixmap() );
}

void GuiEditVCard::changeUserColor()
{
  QColor c = QColorDialog::getColor( QColor( Settings::instance().chatFontColor() ), this );
  if( c.isValid() )
    setUserColor( c.name() );
}

bool GuiEditVCard::checkLineEdit( QLineEdit* ple, const QString& msg )
{
  QString s = ple->text().simplified();
  if( s.isEmpty() )
  {
    QMessageBox::information( this, Settings::instance().programName(), msg, tr( "Ok" ) );
    ple->setFocus();
    return false;
  }
  return true;
}

bool GuiEditVCard::checkData()
{
  if( !checkLineEdit( mp_leNickname, tr( "Please insert your nickname." ) ) )
    return false;

  QString user_nickname = mp_leNickname->text().simplified();
  User u = UserManager::instance().findUserByNickname( user_nickname );
  if( u.isValid() && !u.isLocal() )
  {
    QMessageBox::warning( this, Settings::instance().programName(),
                          tr( "The nickname '%1' is already in use by the user %2." )
                            .arg( user_nickname ).arg( u.path() ) );
    mp_leNickname->setFocus();
    return false;
  }

  return true;
}

void GuiEditVCard::saveVCard()
{
  m_vCard.setNickName( mp_leNickname->text().simplified() );
  m_vCard.setFirstName( mp_leFirstName->text().simplified() );
  m_vCard.setLastName( mp_leLastName->text().simplified() );
  if( mp_deBirthday->date() != QDate( 1900, 1, 1 ) )
    m_vCard.setBirthday( mp_deBirthday->date() );
  else
    m_vCard.setBirthday( QDate() );
  m_vCard.setEmail( mp_leEmail->text().simplified() );
  m_vCard.setPhoneNumber( mp_lePhone->text().simplified() );
  m_vCard.setInfo( mp_teInfo->toPlainText().trimmed() );
}

void GuiEditVCard::onOkClicked()
{
  if( !checkData() )
    return;
  saveVCard();
  accept();
}

void GuiEditVCard::regenerateHash()
{
  if( !checkData() )
    return;

  if( QMessageBox::question( this, Settings::instance().programName(),
                 tr( "Be careful, by changing the hash code you may no longer be a member of some groups and you will have to get invited again. Do you really want to regenerate your hash code?" ),
                 tr( "Yes" ), tr( "No" ), QString::null, 1, 1 ) == 0 )
  {
    m_regenerateUserHash = true;
    saveVCard();
    accept();
  }
}
