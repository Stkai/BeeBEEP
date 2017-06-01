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
#include "GuiNetwork.h"
#include "IconManager.h"
#include "NetworkManager.h"
#include "PluginManager.h"
#include "Settings.h"


GuiNetwork::GuiNetwork( QWidget* parent )
  : QDialog( parent )
{
  setupUi( this );
  setObjectName( "GuiNetworkDialog" );
  setWindowTitle( tr( "Network" ) + QString( " - %1" ).arg( Settings::instance().programName() ));
  setWindowIcon( IconManager::instance().icon( "network.png" ) );
  Bee::removeContextHelpButton( this );

  m_restartConnection = false;
  mp_sbBroadcastInterval->setSuffix( QString( " %1" ).arg( tr( "seconds" ) ) );
  mp_sbMaxUsersToContact->setSuffix( QString( " %1" ).arg( tr( "users" ) ) );

  connect( mp_pbOk, SIGNAL( clicked() ), this, SLOT( checkAndSearch() ) );
  connect( mp_pbCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
  connect( mp_pbShowFileHosts, SIGNAL( clicked() ), this, SLOT( showFileHosts() ) );
}

void GuiNetwork::loadSettings()
{
  mp_leUdpPort->setText( QString::number( Settings::instance().defaultBroadcastPort() ) );

  if( !Settings::instance().multicastGroupAddress().isNull() )
    mp_leMulticastGroup->setText( Settings::instance().multicastGroupAddress().toString() );
  else
    mp_leMulticastGroup->setText( "" );

  QHostAddress base_host_addresses = NetworkManager::instance().localBroadcastAddress();
  if( base_host_addresses.isNull() )
    mp_leSubnet->setText( tr( "Unknown address" ) );
  else
    mp_leSubnet->setText( base_host_addresses.toString() );

  mp_pbShowFileHosts->setDisabled( Settings::instance().broadcastAddressesInFileHosts().isEmpty() );

  mp_cbEnableMDns->setChecked( Settings::instance().useMulticastDns() );

  if( Settings::instance().tickIntervalBroadcasting() > 0 )
  {
    mp_cbBroadcastInterval->setChecked( true );
    mp_sbBroadcastInterval->setValue( Settings::instance().tickIntervalBroadcasting() );
  }
  else
  {
    mp_cbBroadcastInterval->setChecked( false );
    mp_sbBroadcastInterval->setValue( 301 );
  }

  mp_sbMaxUsersToContact->setValue( Settings::instance().maxUsersToConnectInATick() );

  mp_cbPreventMultipleConnectionsFromSingleHostAddress->setChecked( Settings::instance().preventMultipleConnectionsFromSingleHostAddress() );

  mp_comboPreferredSubnet->clear();

  if( !Settings::instance().localSubnetForced().isEmpty() )
    mp_comboPreferredSubnet->addItem( Settings::instance().localSubnetForced(), Settings::instance().localSubnetForced() );
  mp_comboPreferredSubnet->addItem( tr( "none" ), "" );
  QString s_tmp;
  foreach( QHostAddress ha, NetworkManager::instance().localBroadcastAddresses() )
  {
    s_tmp = ha.toString();
    if( s_tmp != Settings::instance().localSubnetForced() )
    {
      if( mp_leSubnet->text() == s_tmp )
        mp_comboPreferredSubnet->addItem( s_tmp + QString( " (%1)" ).arg( tr( "selected by system" ) ), s_tmp );
      else
        mp_comboPreferredSubnet->addItem( s_tmp, s_tmp );
    }
  }

  m_restartConnection = false;
}

void GuiNetwork::checkAndSearch()
{
  Settings::instance().setUseMulticastDns( mp_cbEnableMDns->isChecked() );

  if( mp_cbBroadcastInterval->isChecked() )
    Settings::instance().setTickIntervalBroadcasting( qMax( 301, mp_sbBroadcastInterval->value() ) );
  else
    Settings::instance().setTickIntervalBroadcasting( 0 );

  Settings::instance().setMaxUsersToConnectInATick( mp_sbMaxUsersToContact->value() );
  Settings::instance().SetPreventMultipleConnectionsFromSingleHostAddress( mp_cbPreventMultipleConnectionsFromSingleHostAddress->isChecked() );

  QString s_preferred_subnet = mp_comboPreferredSubnet->itemData( mp_comboPreferredSubnet->currentIndex() ).toString();
  if( s_preferred_subnet != Settings::instance().localSubnetForced() )
  {
    Settings::instance().setLocalSubnetForced( s_preferred_subnet );
    m_restartConnection = true;
  }
  accept();
}

void GuiNetwork::showFileHosts()
{
  QString hosts_file_path = Settings::instance().defaultHostsFilePath( true );
  if( !QFile::exists( hosts_file_path ) )
    hosts_file_path = Settings::instance().defaultHostsFilePath( false );

  if( !Bee::showFileInGraphicalShell( hosts_file_path ) )
    QMessageBox::information( this, Settings::instance().programName(), QString( "%1\n%2" ).arg( hosts_file_path ).arg( tr( "File HOSTS not found." ) ), tr( "Ok" ) );
}
