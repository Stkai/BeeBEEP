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

#ifndef BEEBEEP_XMPPMANAGER_H
#define BEEBEEP_XMPPMANAGER_H

#include "Config.h"
#include "Message.h"
#include "QXmppClient.h"


class XmppManager : public QObject
{
  Q_OBJECT

public:
  explicit XmppManager( QObject* );

  void connectToServer();
  void disconnectFromServer();

signals:
  void message( VNumber chat_id, const Message& m );

protected slots:
  void rosterReceived();
  void rosterChanged( const QString& );
  void errorOccurred( QXmppClient::Error );
  void presenceChanged( const QString&, const QString& );
  void messageReceived( const QXmppMessage& );

private:
  QXmppClient* mp_client;

};

#endif // BEEBEEP_XMPPMANAGER_H
