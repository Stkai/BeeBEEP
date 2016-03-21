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
#include "NetworkManager.h"
#include "Protocol.h"
#include "Settings.h"


GuiAddUser::GuiAddUser( QWidget *parent )
  : QDialog( parent ), m_users()
{
  setupUi( this );

  setWindowTitle( tr( "Add user" ) + QString( " - %1" ).arg( Settings::instance().programName() ) );

  QString s_txt1 = mp_lHelp->text();
  QString s_txt2 = tr( "your IP is %1 in LAN %2").arg( Settings::instance().localUser().hostAddress().toString() )
                                                 .arg( NetworkManager::instance().localBroadcastAddress().toString() );
  QString s_header = QString( "%1\n(%2)" ).arg( s_txt1 ).arg( s_txt2 );
  mp_lHelp->setText( s_header );
  mp_twUsers->setColumnCount( 2 );
  QStringList labels;
  labels << tr( "Address" ) << tr( "Comment" );
  mp_twUsers->setHeaderLabels( labels );
  mp_twUsers->setAlternatingRowColors( true );
  mp_twUsers->setSortingEnabled( true );
  mp_twUsers->setRootIsDecorated( false );
  mp_twUsers->setContextMenuPolicy( Qt::CustomContextMenu);
  mp_twUsers->setSelectionMode( QAbstractItemView::MultiSelection );
  mp_twUsers->setColumnWidth( 0, 180 );

  connect( mp_pbAdd, SIGNAL( clicked() ), this, SLOT( addUser() ) );
  connect( mp_pbOk, SIGNAL( clicked() ), this, SLOT( saveUsers() ) );
  connect( mp_pbCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
  connect( mp_twUsers, SIGNAL( customContextMenuRequested( const QPoint& ) ), this, SLOT( openCustomMenu( const QPoint& ) ) );
  connect( mp_pbAddUsersAuto, SIGNAL( clicked() ), this, SLOT( addUsersAutoFromLan() ) );
}

void GuiAddUser::loadUserPathInList()
{
  if( mp_twUsers->topLevelItemCount() > 0 )
    mp_twUsers->clear();
  foreach( UserRecord ur, m_users )
    addUserToList( ur );
}

void GuiAddUser::loadUsers()
{
  if( !m_users.isEmpty() )
    m_users.clear();

  UserRecord ur;
  foreach( QString user_path, Settings::instance().userPathList() )
  {
    ur = Protocol::instance().loadUserRecord( user_path );
    if( ur.isValid() && !m_users.contains( ur ) )
      m_users.append( ur );
  }

  loadUserPathInList();
  mp_lePort->setText( QString::number( DEFAULT_LISTENER_PORT ) );
  mp_leIpAddress->setText( "" );
  mp_leComment->setText( "" );
  mp_leIpAddress->setFocus();
}

void GuiAddUser::saveUsers()
{
  QStringList sl;
  foreach( UserRecord ur, m_users )
    sl.append( Protocol::instance().saveUserRecord( ur, false ) );

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

  if( mp_cbSplitSubnet->isChecked() )
  {
    QList<QHostAddress> ha_list = NetworkManager::instance().splitBroadcastSubnetToIPv4HostAddresses( QHostAddress( ip_address ), true );
    if( !ha_list.isEmpty() )
    {
      foreach( QHostAddress ha, ha_list )
      {
        UserRecord ur;
        ur.setHostAddress( QHostAddress( ha ) );
        ur.setHostPort( address_port );
        ur.setComment( user_comment );
        if( !m_users.contains( ur ) )
        {
          m_users.append( ur );
          addUserToList( ur );
        }
      }
    }
  }
  else
  {
    UserRecord ur;
    ur.setHostAddress( QHostAddress( ip_address ) );
    ur.setHostPort( address_port );
    ur.setComment( user_comment );
    if( m_users.contains( ur ) )
    {
      QMessageBox::information( this, Settings::instance().programName(), tr( "These IP address and port are already inserted in list." ) );
      mp_leIpAddress->setFocus();
      return;
    }
    else
    {
      m_users.append( ur );
      addUserToList( ur );
    }
  }

  mp_leIpAddress->setText( "" );
  mp_lePort->setText( QString::number( Settings::instance().defaultListenerPort() ) );
  mp_leIpAddress->setFocus();
}

void GuiAddUser::addUserToList( const UserRecord& ur )
{
  QTreeWidgetItem* item = new QTreeWidgetItem( mp_twUsers );
  item->setText( 0, ur.hostAddressAndPort() );
  item->setIcon( 0, QIcon( ":/images/user.png" ));
  item->setText( 1, ur.comment() );
}

void GuiAddUser::openCustomMenu( const QPoint& p )
{
  QTreeWidgetItem* item = mp_twUsers->itemAt( p );
  if( !item )
    return;

  if( !item->isSelected() )
    item->setSelected( true );

  QMenu menu;
  menu.addAction( QIcon( ":/images/delete.png" ), tr( "Remove user path" ), this, SLOT( removeUserPath() ) );
  menu.addSeparator();
  menu.addAction( QIcon( ":/images/clear.png" ), tr( "Clear all" ), this, SLOT( removeAllUsers() ) );
  menu.exec( QCursor::pos() );
}

bool GuiAddUser::removeUserPathFromList( const QString& user_path )
{
  QList<UserRecord>::iterator it = m_users.begin();
  while( it != m_users.end() )
  {
    if( it->hostAddressAndPort() == user_path )
    {
      m_users.erase( it );
      return true;
    }

    ++it;
  }
  return false;
}

void GuiAddUser::removeUserPath()
{
  QList<QTreeWidgetItem*> items = mp_twUsers->selectedItems();
  if( items.isEmpty() )
  {
    QMessageBox::information( this, Settings::instance().programName(), tr( "Please select an user path in the list." ) );
    return;
  }

  QStringList sl;
  foreach( QTreeWidgetItem* item, items )
    sl << item->text( 0 );

  foreach( QString s, sl )
    removeUserPathFromList( s );

  loadUserPathInList();
}

void GuiAddUser::removeAllUsers()
{
  m_users.clear();
  loadUserPathInList();
}

void GuiAddUser::addUsersAutoFromLan()
{
  mp_leIpAddress->setText( NetworkManager::instance().localBroadcastAddress().toString() );
  mp_lePort->setText( QString::number( Settings::instance().defaultListenerPort() ) );
  QString last_comment = mp_leComment->text();
  mp_leComment->setText( tr( "auto added" ) );
  bool last_check = mp_cbSplitSubnet->isChecked();
  mp_cbSplitSubnet->setChecked( true );
  addUser();
  mp_cbSplitSubnet->setChecked( last_check );
  mp_leComment->setText( last_comment );
}
