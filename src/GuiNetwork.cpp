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

#include "GuiNetwork.h"
#include "ui_GuiNetwork.h"
#include "Settings.h"


GuiNetwork::GuiNetwork( QWidget *parent )
  : QDialog( parent )
{
  setupUi( this );
  setObjectName( "GuiNetwork" );
  setWindowTitle( tr( "%1 - Network Manager" ).arg( Settings::instance().programName() ) );
  m_bgProxyType.addButton( mp_rbProxyNone, 0 );
  m_bgProxyType.addButton( mp_rbProxyAutomatic, 1 );
  m_bgProxyType.addButton( mp_rbProxyManually, 2 );
  m_bgProxyType.setExclusive( true );

  connect( &m_bgProxyType, SIGNAL( buttonClicked( int ) ), this, SLOT( proxyTypeChanged( int ) ) );
  connect( mp_cbEnableProxyAuth, SIGNAL( toggled( bool ) ), this, SLOT( proxyUseAuthToggled( bool ) ) );
  connect( mp_pbCancel, SIGNAL( clicked() ), this, SLOT( close() ) );
  connect( mp_pbOk, SIGNAL( clicked() ), this, SLOT( saveAndClose() ) );
}

void GuiNetwork::loadSettings()
{
  const QNetworkProxy& proxy = Settings::instance().networkProxy();
  switch( proxy.type() )
  {
  case QNetworkProxy::NoProxy:
    mp_rbProxyNone->setChecked( true );
    break;
  case QNetworkProxy::DefaultProxy:
    mp_rbProxyAutomatic->setChecked( true );
    break;
  default:
    mp_rbProxyManually->setChecked( true );
  }

  setProxyMenuEnabled( mp_rbProxyManually->isChecked() );

  mp_leProxyAddress->setText( proxy.hostName() );
  mp_sbProxyPort->setValue( proxy.port() );

  mp_cbEnableProxyAuth->setChecked( Settings::instance().networkProxyUseAuthentication() );
  proxyUseAuthToggled( mp_cbEnableProxyAuth->isChecked() );
  mp_leProxyUser->setText( proxy.user() );
  mp_leProxyPassword->setText( proxy.password() );

}

void GuiNetwork::saveAndClose()
{
  QNetworkProxy proxy = Settings::instance().networkProxy();
  if( m_bgProxyType.checkedId() ==  0 )
    proxy.setType( QNetworkProxy::NoProxy );
  else if( m_bgProxyType.checkedId() ==  1 )
    proxy.setType( QNetworkProxy::DefaultProxy );
  else
  {
    if( proxy.type() == QNetworkProxy::NoProxy || proxy.type() == QNetworkProxy::DefaultProxy )
      proxy.setType( QNetworkProxy::HttpProxy );
  }

  QString host_name = mp_leProxyAddress->text().trimmed();
  if( mp_rbProxyManually->isChecked() && host_name.isEmpty() )
  {
    QMessageBox::information( this, Settings::instance().programName(), tr( "Please insert the network proxy hostname or address." ) );
    mp_leProxyAddress->setFocus();
    return;
  }

  proxy.setHostName( host_name );
  proxy.setPort( mp_sbProxyPort->value() );
  QString proxy_user = mp_leProxyUser->text().trimmed();
  if( mp_cbEnableProxyAuth->isChecked() && proxy_user.isEmpty() )
  {
    QMessageBox::information( this, Settings::instance().programName(), tr( "Please insert the user for the proxy authorization." ) );
    mp_leProxyUser->setFocus();
    return;
  }
  Settings::instance().setNetworkProxyUseAuthentication( mp_cbEnableProxyAuth->isChecked() );
  proxy.setUser( proxy_user );
  proxy.setPassword( mp_leProxyPassword->text() );
  Settings::instance().setNetworkProxy( proxy );
  done( 0 );
}

void GuiNetwork::proxyTypeChanged( int )
{
  setProxyMenuEnabled( mp_rbProxyManually->isChecked() );
}

void GuiNetwork::proxyUseAuthToggled( bool value )
{
  mp_leProxyUser->setEnabled( value );
  mp_leProxyPassword->setEnabled( value );
}

void GuiNetwork::setProxyMenuEnabled( bool enable )
{
  mp_leProxyAddress->setEnabled( enable );
  mp_sbProxyPort->setEnabled( enable );
  mp_cbEnableProxyAuth->setEnabled( enable );
  if( enable && mp_cbEnableProxyAuth->isEnabled() )
    proxyUseAuthToggled( enable );
  else
    proxyUseAuthToggled( false );
}
