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

  QString s_txt1 = QString( "<b>%1</b>.<br>%2: <b>%3</b>&nbsp;&nbsp;&nbsp;%4: <b>%5</b>&nbsp;&nbsp;&nbsp;%6: <b>%7</b>" )
                     .arg( tr( "Your network parameters" ) )
                     .arg( tr( "IP Address" ) ).arg( Settings::instance().localUser().networkAddress().hostAddress().toString() )
                     .arg( tr( "Message port" ) ).arg( Settings::instance().localUser().networkAddress().hostPort() )
                     .arg( tr( "File transfer port" ) ).arg( Settings::instance().defaultFileTransferPort() );

  QString s_txt2 = tr( "Enter the host address and the port you want to test the connection.");

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
  connect( cs, SIGNAL( disconnected() ), this, SLOT( onDisconnected() ) );
  connect( cs, SIGNAL( error( QAbstractSocket::SocketError ) ), this, SLOT( onError( QAbstractSocket::SocketError ) ) );
  connect( cs, SIGNAL( connectionTestCompleted( const QString& ) ), this, SLOT( onTestCompleted( const QString& ) ) );
  m_connections.append( cs );
  addToReport( tr( "Connecting to %1" ).arg( na.toString() + QString( "..." ) ) );
  cs->connectToNetworkAddress( na );
}

void GuiNetworkTest::removeConnection( ConnectionSocket* cs, bool abort_connection )
{
  if( m_connections.removeOne( cs ) )
  {
    disconnect( cs, SIGNAL( connected() ), this, SLOT( onConnected() ) );
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

void GuiNetworkTest::onConnected()
{
  ConnectionSocket* cs = qobject_cast<ConnectionSocket*>( sender() );
  if( cs )
  {
    QString msg = tr( "Connection to %1 is active." ).arg( cs->networkAddress().toString() );
    addToReport( msg );
  }
  else
    qWarning() << "Test network has found an unkown connection socket on slot onConnected()";
}

void GuiNetworkTest::onError( QAbstractSocket::SocketError se )
{
  ConnectionSocket* cs = qobject_cast<ConnectionSocket*>( sender() );
  if( cs )
  {
    QString msg = tr( "Connection to %1 has error #%2: %3" ).arg( cs->networkAddress().toString() ).arg( static_cast<int>(se) ).arg( cs->errorString() );
    addToReport( msg );
    removeConnection( cs, true );
  }
  else
    qWarning() << "Test network has found an unkown connection socket on slot onError(" << static_cast<int>(se) << ")";

}

void GuiNetworkTest::onTestCompleted( const QString& msg )
{
  addToReport( msg );
  ConnectionSocket* cs = qobject_cast<ConnectionSocket*>( sender() );
  if( cs )
    removeConnection( cs );
  else
    qWarning() << "Test network has found an unkown connection socket on slot onTestCompleted(" << qPrintable( msg ) << ")";
}

void GuiNetworkTest::onDisconnected()
{
  ConnectionSocket* cs = qobject_cast<ConnectionSocket*>( sender() );
  if( cs )
  {
    QString msg = tr( "Connection to %1 has been closed." ).arg( cs->networkAddress().toString() );
    addToReport( msg );
    removeConnection( cs );
  }
  else
    qWarning() << "Test network has found an unkown connection socket on slot onDisconnected()";

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
