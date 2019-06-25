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

#ifndef BEEBEEP_GUINETWORKTEST_H
#define BEEBEEP_GUINETWORKTEST_H

#include "ui_GuiNetworkTest.h"
#include "ConnectionSocket.h"


class GuiNetworkTest : public QDialog, private Ui::GuiNetworkTestDialog
{
  Q_OBJECT

public:
  explicit GuiNetworkTest( QWidget *parent = Q_NULLPTR );

  void onTickEvent( int );
  void showUp();

protected slots:
  void startTest();
  void onError( QAbstractSocket::SocketError );
  void onConnected();
  void onDisconnected();
  void onTestCompleted( const QString& );
  void closeTest();
  void clearReport();

protected:
  void addToReport( const QString& );
  void closePendingConnections();
  void closeEvent( QCloseEvent* );
  void removeConnection( ConnectionSocket*, bool abort_connection = false );

private:
  QList<ConnectionSocket*> m_connections;

};


#endif // BEEBEEP_GUINETWORKTEST_H
