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
#include "GuiNetworkTest.h"
#include "IconManager.h"
#include "NetworkManager.h"
#include "Settings.h"


GuiNetworkTest::GuiNetworkTest( QWidget *parent )
  : QDialog( parent ), m_connections()
{
  setupUi( this );
  setWindowTitle( tr( "Network test" ) + QString( " - %1" ).arg( Settings::instance().programName() ) );
  setWindowIcon( IconManager::instance().icon( "network-test.png" ) );
  Bee::removeContextHelpButton( this );

  QString s_txt1 = QString( "<b>%1</b>&nbsp;&nbsp;&nbsp;%2: <b>%3</b>&nbsp;&nbsp;&nbsp;%4: <b>%5</b>&nbsp;&nbsp;&nbsp;%6: %7" )
                     .arg( tr( "Your parameters" ) )
                     .arg( tr( "IP Address" ) ).arg( Settings::instance().localUser().networkAddress().hostAddress().toString() )
                     .arg( tr( "Port" ) ).arg( Settings::instance().localUser().networkAddress().hostPort() )
                     .arg( tr( "Subnet" ) ).arg( NetworkManager::instance().localBroadcastAddress().toString() );

  QString s_txt2 = mp_lHelp->text();

  QString s_header = QString( "%1<br><br>%2." ).arg( s_txt1 ).arg( s_txt2 );
  mp_lHelp->setText( s_header );

  connect( mp_pbTest, SIGNAL( clicked() ), this, SLOT( startTest() ) );
  connect( mp_pbClose, SIGNAL( clicked() ), this, SLOT( closeTest() ) );
  connect( mp_pbClear, SIGNAL( clicked() ), this, SLOT( clearReport() ) );

}

void GuiNetworkTest::showUp()
{
  Bee::showUp( this );
}

void GuiNetworkTest::startTest()
{
  QHostAddress host_address = QHostAddress( mp_leIpAddress->text().simplified() );
  if( host_address.isNull() )
  {
    QMessageBox::warning( this, Settings::instance().programName(), tr( "The host address entered is not valid." ), tr( "Ok" ) );
    mp_leIpAddress->setFocus();
    return;
  }

  bool ok = false;
  int host_port = mp_lePort->text().simplified().toInt( &ok );
  if( !ok || host_port < 1 || host_port > MAX_SOCKET_PORT )
  {
    QMessageBox::warning( this, Settings::instance().programName(), tr( "The port entered is not valid." ), tr( "Ok" ) );
    mp_leIpAddress->setFocus();
    return;
  }

  NetworkAddress na( host_address, static_cast<quint16>(host_port) );
  ConnectionSocket* cs = new ConnectionSocket( this );
  cs->setTestConnection( true );
  connect( cs, SIGNAL( connected() ), this, SLOT( onConnected() ) );
  connect( cs, SIGNAL( hostFound() ), this, SLOT( onHostFound() ) );
  connect( cs, SIGNAL( disconnected() ), this, SLOT( onDisconnected() ) );
  connect( cs, SIGNAL( error( QAbstractSocket::SocketError ) ), this, SLOT( onError( QAbstractSocket::SocketError ) ) );
  connect( cs, SIGNAL( connectionTestCompleted( const QString& ) ), this, SLOT( onTestCompleted( const QString& ) ) );
  m_connections.append( cs );
  cs->connectToNetworkAddress( na );
}

void GuiNetworkTest::removeConnection( ConnectionSocket* cs, bool abort_connection )
{
  if( m_connections.removeOne( cs ) )
  {
    disconnect( cs, SIGNAL( connected() ), this, SLOT( onConnected() ) );
    disconnect( cs, SIGNAL( hostFound() ), this, SLOT( onHostFound() ) );
    disconnect( cs, SIGNAL( disconnected() ), this, SLOT( onDisconnected() ) );
    disconnect( cs, SIGNAL( error( QAbstractSocket::SocketError ) ), this, SLOT( onError( QAbstractSocket::SocketError ) ) );
    disconnect( cs, SIGNAL( connectionTestCompleted( const QString& ) ), this, SLOT( onTestCompleted( const QString& ) ) );
    if( abort_connection )
      cs->abortConnection();
    else
      cs->closeConnection();
    cs->deleteLater();
  }
}

void GuiNetworkTest::onHostFound()
{
  QString msg;
  ConnectionSocket* cs = qobject_cast<ConnectionSocket*>( sender() );
  if( cs )
    msg = tr( "Connection to %1 has found host." ).arg( cs->networkAddress().toString() );
  else
    msg = tr( "Unknown connection has found host." );
  addToReport( msg );
}

void GuiNetworkTest::onConnected()
{
  QString msg;
  ConnectionSocket* cs = qobject_cast<ConnectionSocket*>( sender() );
  if( cs )
    msg = tr( "Connection to %1 is active." ).arg( cs->networkAddress().toString() );
  else
    msg = tr( "Unknown connection is active." );
  addToReport( msg );
}

void GuiNetworkTest::onError( QAbstractSocket::SocketError se )
{
  QString msg;
  ConnectionSocket* cs = qobject_cast<ConnectionSocket*>( sender() );
  if( cs )
  {
    msg = tr( "Connection to %1 has error #%2." ).arg( cs->networkAddress().toString() ).arg( static_cast<int>(se) );
    removeConnection( cs, true );
  }
  else
    msg = tr( "Unknown connection has error #%2." ).arg( static_cast<int>(se) );
  addToReport( msg );
}

void GuiNetworkTest::onTestCompleted( const QString& msg )
{
  addToReport( msg );
  ConnectionSocket* cs = qobject_cast<ConnectionSocket*>( sender() );
  if( cs )
    removeConnection( cs );
}

void GuiNetworkTest::onDisconnected()
{
  QString msg;
  ConnectionSocket* cs = qobject_cast<ConnectionSocket*>( sender() );
  if( cs )
  {
    msg = tr( "Connection to %1 has been disconnected." ).arg( cs->networkAddress().toString() );
    removeConnection( cs );
  }
  else
    msg = tr( "Unknown connection has been disconnected." );
  addToReport( msg );
}

void GuiNetworkTest::addToReport( const QString& msg )
{
  mp_teReport->append( msg );
}

void GuiNetworkTest::closeTest()
{
  done( 1 );
}

void GuiNetworkTest::clearReport()
{
  mp_teReport->clear();
}

void GuiNetworkTest::closePendingConnections()
{
#ifdef BEEBEEP_DEBUG
  qDebug() << "GuiNetworkTest closing" << m_connections.size() << "pending connections";
#endif
  foreach( ConnectionSocket* cs, m_connections )
  {
    if( cs->isConnecting() || cs->isConnected() )
      cs->abortConnection();
  }
}

void GuiNetworkTest::closeEvent( QCloseEvent* e )
{
  closePendingConnections();
  QDialog::closeEvent( e );
}

void GuiNetworkTest::onTickEvent( int ticks )
{
  foreach( ConnectionSocket* cs, m_connections )
  {
    if( cs->isConnecting() || cs->isConnected() )
      cs->onTickEvent( ticks );
  }
}
