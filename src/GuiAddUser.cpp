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

#include "Config.h"
#include "GuiAddUser.h"
#include "Protocol.h"
#include "Settings.h"


GuiAddUser::GuiAddUser( QWidget *parent )
  : QDialog( parent ), m_users()
{
  setupUi( this );

  setWindowTitle( tr( "Add user" ) + QString( " - %1" ).arg( Settings::instance().programName() ) );
  mp_twUsers->setColumnCount( 2 );
  QStringList labels;
  labels << tr( "IP Address and port" ) << tr( "Comment" );
  mp_twUsers->setHeaderLabels( labels );
  mp_twUsers->setAlternatingRowColors( true );
  mp_twUsers->setSortingEnabled( true );
  mp_twUsers->setRootIsDecorated( false );

  connect( mp_pbAdd, SIGNAL( clicked() ), this, SLOT( addUser() ) );
  connect( mp_pbOk, SIGNAL( clicked() ), this, SLOT( saveUsers() ) );
  connect( mp_pbCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

void GuiAddUser::loadUsers()
{
  if( mp_twUsers->topLevelItemCount() > 0 )
    mp_twUsers->clear();
  if( !m_users.isEmpty() )
    m_users.clear();

  UserRecord ur;
  foreach( QString user_path, Settings::instance().userPathList() )
  {
     ur = Protocol::instance().loadUserRecord( user_path );
     if( ur.isValid() )
       addUserToList( ur );
  }

  mp_lePort->setText( QString::number( DEFAULT_LISTENER_PORT ) );
  mp_leIpAddress->setText( "" );
  mp_leComment->setText( "" );
  mp_leIpAddress->setFocus();
}

void GuiAddUser::saveUsers()
{
  QStringList sl;
  foreach( UserRecord ur, m_users )
    sl.append( Protocol::instance().saveUserRecord( ur ) );

  Settings::instance().setUserPathList( sl );
  accept();
}

void GuiAddUser::addUser()
{
  QString ip_address = mp_leIpAddress->text().simplified();
  if( ip_address.isEmpty() || QHostAddress( ip_address ).isNull() )
  {
    QMessageBox::information( this, Settings::instance().programName(), tr( "Please insert a valid IP address.") );
    mp_leIpAddress->setFocus();
    return;
  }

  bool ok = false;
  int address_port = mp_lePort->text().simplified().toInt( &ok, 10 );

  if( !ok || address_port < 1 || address_port > 65535 )
  {
    QMessageBox::information( this, Settings::instance().programName(), tr( "Please insert a valid port or use the default one %1." ).arg( DEFAULT_LISTENER_PORT ) );
    mp_lePort->setFocus();
    return;
  }

  QString user_comment = mp_leComment->text().simplified();

  UserRecord ur;
  ur.setHostAddress( QHostAddress( ip_address ) );
  ur.setHostPort( address_port );
  ur.setComment( user_comment );
  if( !addUserToList( ur ) )
  {
    QMessageBox::information( this, Settings::instance().programName(), tr( "These IP address and port are already inserted in list." ) );
    mp_leIpAddress->setFocus();
    return;
  }

  mp_leIpAddress->setText( "" );
  mp_lePort->setText( QString::number( DEFAULT_LISTENER_PORT ) );
  mp_leIpAddress->setFocus();
}

bool GuiAddUser::addUserToList( const UserRecord& ur )
{
  if( m_users.contains( ur ) )
    return false;
  else
    m_users.append( ur );

  QTreeWidgetItem* item = new QTreeWidgetItem( mp_twUsers );
  item->setText( 0, ur.hostAddressAndPort() );
  item->setIcon( 0, QIcon( ":/images/user.png" ));
  item->setText( 1, ur.comment() );

  return true;
}
