//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2020 Marco Mastroddi
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

#include "BeeUtils.h"
#include "GuiAddUser.h"
#include "IconManager.h"
#include "NetworkManager.h"
#include "Protocol.h"
#include "Settings.h"


GuiAddUser::GuiAddUser( QWidget *parent )
  : QDialog( parent ), m_networkAddresses()
{
  setupUi( this );
  setWindowTitle( tr( "Add users" ) + QString( " - %1" ).arg( Settings::instance().programName() ) );
  setWindowIcon( IconManager::instance().icon( "user-add.png" ) );
  Bee::removeContextHelpButton( this );

  QString s_txt1 = QString( "<b>%1</b>&nbsp;&nbsp;&nbsp;%2: <b>%3</b>&nbsp;&nbsp;&nbsp;%4: <b>%5</b>&nbsp;&nbsp;&nbsp;%6: %7" )
                     .arg( tr( "Your parameters" ) )
                     .arg( tr( "IP Address" ) ).arg( Settings::instance().localUser().networkAddress().hostAddress().toString() )
                     .arg( tr( "Port" ) ).arg( Settings::instance().localUser().networkAddress().hostPort() )
                     .arg( tr( "Subnet" ) ).arg( NetworkManager::instance().localBroadcastAddress().toString() );

  QString s_txt2 = mp_lHelp->text();

  QString s_header = QString( "%1<br><br>%2." ).arg( s_txt1 ).arg( s_txt2 );
  mp_lHelp->setText( s_header );
  mp_twUsers->setColumnCount( 2 );
  QStringList labels;
  labels << tr( "Address" ) << tr( "Comment" );
  mp_twUsers->setHeaderLabels( labels );
  mp_twUsers->setAlternatingRowColors( true );
  mp_twUsers->setSortingEnabled( true );
  mp_twUsers->setRootIsDecorated( false );
  mp_twUsers->setContextMenuPolicy( Qt::CustomContextMenu );
  mp_twUsers->setSelectionMode( QAbstractItemView::MultiSelection );
  mp_twUsers->setColumnWidth( 0, 180 );

  mp_menuContext = new QMenu( this );
  mp_menuContext->addAction( IconManager::instance().icon( "delete.png" ), tr( "Remove user path" ), this, SLOT( removeUserPath() ) );
  mp_menuContext->addSeparator();
  mp_menuContext->addAction( IconManager::instance().icon( "clear.png" ), tr( "Clear all" ), this, SLOT( removeAllUsers() ) );

  mp_pbAdd->setIcon( IconManager::instance().icon( "user-add.png" ) );

  connect( mp_pbAdd, SIGNAL( clicked() ), this, SLOT( addUser() ) );
  connect( mp_pbOk, SIGNAL( clicked() ), this, SLOT( saveUsers() ) );
  connect( mp_pbCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
  connect( mp_twUsers, SIGNAL( customContextMenuRequested( const QPoint& ) ), this, SLOT( openCustomMenu( const QPoint& ) ) );
  connect( mp_pbAddUsersAuto, SIGNAL( clicked() ), this, SLOT( addUsersAutoFromLan() ) );
}

void GuiAddUser::loadNetworkAddressesInList()
{
  if( mp_twUsers->topLevelItemCount() > 0 )
    mp_twUsers->clear();
  foreach( NetworkAddress na, m_networkAddresses )
    addNetworkAddressToList( na );
}

void GuiAddUser::loadUsers()
{
  if( !m_networkAddresses.isEmpty() )
    m_networkAddresses.clear();

  NetworkAddress na;
  foreach( QString s_network_address, Settings::instance().networkAddressList() )
  {
    na = Protocol::instance().loadNetworkAddress( s_network_address );
    if( na.isHostAddressValid() && na.isHostPortValid() )
    {
      if( !m_networkAddresses.contains( na ) )
        m_networkAddresses.append( na );
    }
  }

  loadNetworkAddressesInList();
  mp_lePort->setText( QString::number( DEFAULT_LISTENER_PORT ) );
  mp_leIpAddress->setText( "" );
  mp_leComment->setText( "" );
  mp_leIpAddress->setFocus();
}

void GuiAddUser::saveUsers()
{
  QStringList sl;
  foreach( NetworkAddress na, m_networkAddresses )
    sl.append( Protocol::instance().saveNetworkAddress( na ) );
  Settings::instance().setNetworkAddressList( sl );
  Settings::instance().save();
  accept();
}

void GuiAddUser::addUser()
{
  QString ip_address = mp_leIpAddress->text().simplified();
  if( ip_address.isEmpty() || QHostAddress( ip_address ).isNull() )
  {
    QMessageBox::information( this, Settings::instance().programName(), tr( "Please insert a valid IP Address.") );
    mp_leIpAddress->setFocus();
    return;
  }

  bool ok = false;
  quint16 address_port = mp_lePort->text().simplified().toUInt( &ok );

  if( !ok || address_port < 1 || address_port > 65534 )
  {
    QMessageBox::information( this, Settings::instance().programName(), tr( "Please insert a valid Port or use the default one %1." ).arg( DEFAULT_LISTENER_PORT ) );
    mp_lePort->setFocus();
    return;
  }

  QString user_comment = mp_leComment->text().simplified();

  if( mp_cbSplitSubnet->isChecked() )
  {
    QList<QHostAddress> ha_list = NetworkManager::instance().splitInIPv4HostAddresses( QHostAddress( ip_address ) );
    if( !ha_list.isEmpty() )
    {
      foreach( QHostAddress ha, ha_list )
      {
        NetworkAddress na( ha, address_port );
        na.setInfo( user_comment );
        if( !m_networkAddresses.contains( na ) )
        {
          m_networkAddresses.append( na );
          addNetworkAddressToList( na );
        }
      }
    }
  }
  else
  {
    NetworkAddress na( QHostAddress( ip_address ), address_port );
    na.setInfo( user_comment );
    if( m_networkAddresses.contains( na ) )
    {
      QMessageBox::information( this, Settings::instance().programName(), tr( "These IP Address and Port are already in list." ) );
      mp_leIpAddress->setFocus();
      return;
    }
    else
    {
      m_networkAddresses.append( na );
      addNetworkAddressToList( na );
    }
  }

  mp_leIpAddress->setText( "" );
  mp_lePort->setText( QString::number( Settings::instance().defaultListenerPort() ) );
  mp_leIpAddress->setFocus();
}

void GuiAddUser::addNetworkAddressToList( const NetworkAddress& na )
{
  QTreeWidgetItem* item = new QTreeWidgetItem( mp_twUsers );
  item->setText( 0, na.toString() );
  item->setIcon( 0, IconManager::instance().icon( "user.png" ));
  item->setText( 1, na.info() );
}

void GuiAddUser::openCustomMenu( const QPoint& p )
{
  QTreeWidgetItem* item = mp_twUsers->itemAt( p );
  if( !item )
    return;

  if( !item->isSelected() )
    item->setSelected( true );

  mp_menuContext->exec( QCursor::pos() );
}

bool GuiAddUser::removeUserPathFromList( const QString& user_path )
{
  QList<NetworkAddress>::iterator it = m_networkAddresses.begin();
  while( it != m_networkAddresses.end() )
  {
    if( it->toString() == user_path )
    {
      m_networkAddresses.erase( it );
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
    QMessageBox::information( this, Settings::instance().programName(), tr( "Please select an item in the list." ) );
    return;
  }

  QStringList sl;
  foreach( QTreeWidgetItem* item, items )
    sl << item->text( 0 );

  foreach( QString s, sl )
    removeUserPathFromList( s );

  loadNetworkAddressesInList();
}

void GuiAddUser::removeAllUsers()
{
  m_networkAddresses.clear();
  loadNetworkAddressesInList();
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
