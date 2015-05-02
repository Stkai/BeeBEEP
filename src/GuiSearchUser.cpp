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
#include "GuiSearchUser.h"
#include "PluginManager.h"
#include "Settings.h"


GuiSearchUser::GuiSearchUser( QWidget *parent )
  : QDialog(parent)
{
  setupUi( this );
  setObjectName( "GuiSearchUser" );
  setWindowTitle( tr( "Configure network" ) );

  connect( mp_pbOk, SIGNAL( clicked() ), this, SLOT( checkAndSearch() ) );
  connect( mp_pbCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

void GuiSearchUser::loadSettings()
{
  QHostAddress base_host_addresses = Settings::instance().baseBroadcastAddress();
  if( base_host_addresses.isNull() )
    mp_leSubnet->setText( tr( "Unknown address" ) );
  else
    mp_leSubnet->setText( base_host_addresses.toString() );

  mp_leUdpPort->setText( QString::number( Settings::instance().broadcastPort() ) );

  QStringList sl_addresses = Settings::instance().broadcastAddressesInFileHosts();
  if( sl_addresses.size() > 0 )
    mp_teAddressesInHosts->setPlainText( sl_addresses.join( ", " ) );
  else
    mp_teAddressesInHosts->setPlainText( tr( "File is empty" ) );

  sl_addresses = Settings::instance().broadcastAddressesInSettings();
  if( sl_addresses.size() > 0 )
    mp_teAddressesInSettings->setPlainText( sl_addresses.join( ", " ) );
  else
    mp_teAddressesInSettings->setPlainText( "" );

  mp_cbParseAddresses->setChecked( Settings::instance().parseBroadcastAddresses() );

  mp_teAddressesInSettings->setFocus();
}

void GuiSearchUser::checkAndSearch()
{
  QString address_string = mp_teAddressesInSettings->toPlainText().simplified();
  QStringList address_list;
  if( address_string.size() > 0 )
    address_list = address_string.split( ",", QString::SkipEmptyParts );

  if( !address_list.isEmpty() )
  {
    foreach( QString s, address_list )
    {
      QHostAddress host_address( s.simplified() );
      if( host_address.isNull() )
      {
        QMessageBox::warning( this, QString( "%1 - %2" ).arg( Settings::instance().programName() ).arg( tr( "Warning" ) ),
                              tr( "You have inserted an invalid host address:\n%1 is removed from the list." ).arg( s.simplified() ), QMessageBox::Ok );
        address_list.removeOne( s );
        mp_teAddressesInSettings->setPlainText( address_list.join( ", " ) );
        mp_teAddressesInSettings->setFocus();
        return;
      }
    }

    Settings::instance().setBroadcastAddressesInSettings( address_list );
  }
  else
    Settings::instance().setBroadcastAddressesInSettings( QStringList() );

  Settings::instance().setParseBroadcastAddresses( mp_cbParseAddresses->isChecked() );

  accept();
}

