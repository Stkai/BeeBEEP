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
#include "NetworkManager.h"
#include "PluginManager.h"
#include "Settings.h"


GuiSearchUser::GuiSearchUser( QWidget *parent )
  : QDialog(parent)
{
  setupUi( this );
  setObjectName( "GuiSearchUser" );
  setWindowTitle( tr( "Search for users" ) + QString( " - %1" ).arg( Settings::instance().programName() ));

  connect( mp_pbOk, SIGNAL( clicked() ), this, SLOT( checkAndSearch() ) );
  connect( mp_pbCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

void GuiSearchUser::loadSettings()
{
  QHostAddress base_host_addresses = NetworkManager::instance().localBroadcastAddress();
  if( base_host_addresses.isNull() )
    mp_leSubnet->setText( tr( "Unknown address" ) );
  else
    mp_leSubnet->setText( base_host_addresses.toString() );

  mp_leUdpPort->setText( QString::number( Settings::instance().defaultBroadcastPort() ) );

  QStringList sl_tmp = Settings::instance().broadcastAddressesInFileHosts();
  if( sl_tmp.size() > 0 )
    mp_teAddressesInHosts->setPlainText( sl_tmp.join( ", " ) );
  else
    mp_teAddressesInHosts->setPlainText( tr( "File is empty" ) );

  sl_tmp = Settings::instance().broadcastAddressesInSettings();
  sl_tmp.removeDuplicates();
  if( sl_tmp.size() > 0 )
    mp_teAddressesInSettings->setPlainText( sl_tmp.join( ", " ) );
  else
    mp_teAddressesInSettings->setPlainText( "" );

  mp_cbParseAddresses->setChecked( Settings::instance().parseBroadcastAddresses() );
  mp_cbAutoAddSubnet->setChecked( Settings::instance().addExternalSubnetAutomatically() );
  mp_cbEnableMDns->setChecked( Settings::instance().useMulticastDns() );

  sl_tmp = Settings::instance().workgroups();
  if( sl_tmp.size() > 0 )
    mp_leWorkgroups->setText( sl_tmp.join( ", " ) );
  else
    mp_leWorkgroups->setText( "" );

  mp_cbAcceptConnectionsOnlyFromWorkgroups->setChecked( Settings::instance().acceptConnectionsOnlyFromWorkgroups() );

  mp_leWorkgroups->setFocus();
}

void GuiSearchUser::checkAndSearch()
{
  QString string_tmp = mp_teAddressesInSettings->toPlainText().simplified();
  QStringList sl_tmp;
  if( string_tmp.size() > 0 )
    sl_tmp = string_tmp.split( ",", QString::SkipEmptyParts );

  if( !sl_tmp.isEmpty() )
  {
    QStringList sl_addresses;
    foreach( QString s, sl_tmp )
    {
      QHostAddress host_address( s.simplified() );
      if( host_address.isNull() )
      {
        QMessageBox::warning( this, QString( "%1 - %2" ).arg( Settings::instance().programName() ).arg( tr( "Warning" ) ),
                              tr( "You have inserted an invalid host address:\n%1 is removed from the list." ).arg( s.simplified() ), tr( "Ok" ) );
        sl_tmp.removeOne( s );
        mp_teAddressesInSettings->setPlainText( sl_tmp.join( ", " ) );
        mp_teAddressesInSettings->setFocus();
        return;
      }
      else
        sl_addresses.append( s.trimmed() );
    }

    Settings::instance().setBroadcastAddressesInSettings( sl_addresses );
  }
  else
    Settings::instance().setBroadcastAddressesInSettings( QStringList() );

  string_tmp = mp_leWorkgroups->text().simplified();
  QStringList sl_workgroups;
  if( string_tmp.size() > 0 )
  {
    sl_tmp = string_tmp.split( ",", QString::SkipEmptyParts );
    foreach( QString s, sl_tmp )
      sl_workgroups.append( s.simplified() );
  }

  Settings::instance().setWorkgroups( sl_workgroups );
  Settings::instance().setAcceptConnectionsOnlyFromWorkgroups( mp_cbAcceptConnectionsOnlyFromWorkgroups->isChecked() );

  Settings::instance().setParseBroadcastAddresses( mp_cbParseAddresses->isChecked() );
  Settings::instance().setAddExternalSubnetAutomatically( mp_cbAutoAddSubnet->isChecked() );
  Settings::instance().setUseMulticastDns( mp_cbEnableMDns->isChecked() );

  accept();
}
