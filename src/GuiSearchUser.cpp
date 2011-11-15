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

#include "Config.h"
#include "GuiSearchUser.h"
#include "PluginManager.h"
#include "Settings.h"
#include "ui_GuiSearchUser.h"


GuiSearchUser::GuiSearchUser( QWidget *parent )
  : QDialog(parent)
{
  setupUi( this );
  setObjectName( "GuiSearchUser" );
  setWindowTitle( tr( "Search Users") );
  mp_textBroadcast->setReadOnly( false );

  connect( mp_pbOk, SIGNAL( clicked() ), this, SLOT( checkAndSearch() ) );
  connect( mp_pbCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

QString GuiSearchUser::service() const
{
  if( mp_comboService->count() > 0 )
    return mp_comboService->currentText();
  else
    return QLatin1String( "" );
}

QString GuiSearchUser::userId() const
{
  return mp_leUser->text().simplified();
}

void GuiSearchUser::loadSettings()
{
  m_addresses = Settings::instance().broadcastAddresses();
  if( m_addresses.size() > 0 )
  {
    mp_textBroadcast->setPlainText( m_addresses.join( ", " ) );
    mp_cbSaveAddresses->setChecked( true );
  }
  else
  {
    mp_textBroadcast->setPlainText( "" );
    mp_cbSaveAddresses->setChecked( false );
  }

  if( mp_comboService->count() )
    mp_comboService->clear();
  foreach( ServiceInterface* si, PluginManager::instance().services() )
  {
    if( si->isEnabled() )
      mp_comboService->addItem( si->icon(), si->name(), si->name() );
  }

  if( mp_comboService->count() == 0 )
  {
    mp_boxAddUser->hide();
    resize( width(), height() - (mp_boxAddUser->height() + contentsMargins().top() + contentsMargins().bottom()) );
  }
}

void GuiSearchUser::checkAndSearch()
{
  QString address_string = mp_textBroadcast->toPlainText().simplified();
  QStringList address_list;
  if( address_string.size() > 0 )
    address_list = address_string.split( ",", QString::SkipEmptyParts );

  m_addresses.clear();

  if( address_list.size() > 0 )
  {
    foreach( QString s, address_list )
    {
      QHostAddress host_address( s.simplified() );
      if( host_address.isNull() )
      {
        QMessageBox::warning( this, QString( "%1 - %2" ).arg( Settings::instance().programName() ).arg( tr( "Warning" ) ),
                              tr( "You have inserted an invalid host address:\n%1 is removed from the list." ).arg( s.simplified() ), QMessageBox::Ok );
        address_list.removeOne( s );
        mp_textBroadcast->setText( address_list.join( ", " ) );
        mp_textBroadcast->setFocus();
        return;
      }
      m_addresses.append( host_address.toString() );
    }
  }

  if( mp_cbSaveAddresses->isChecked() )
    Settings::instance().setBroadcastAddresses( m_addresses );
  else
    Settings::instance().setBroadcastAddresses( QStringList() );

  accept();
}
