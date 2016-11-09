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

#ifndef BEEBEEP_CONNECTION_H
#define BEEBEEP_CONNECTION_H

#include "ConnectionSocket.h"
class Message;


class Connection : public ConnectionSocket
{
  Q_OBJECT

public:
  explicit Connection( QObject *parent = 0 );

  bool sendMessage( const Message& );
  void setReadyForUse( VNumber );
  inline bool isReadyForUse() const;

signals:
  void newMessage( VNumber, const Message& );

protected slots:
  void parseData( const QByteArray& );
  void sendPing();
  void sendPong();
  void onTickEvent( int );

};

// Inline Functions
inline bool Connection::isReadyForUse() const { return userId() != ID_INVALID; }

#endif // BEEBEEP_CONNECTION_H
