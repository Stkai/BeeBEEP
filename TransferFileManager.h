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
// $Id: TransferFileManager.h 28 2010-06-20 17:53:16Z mastroddi $
//
//////////////////////////////////////////////////////////////////////

#ifndef BEEBEEP_TRANSFERFILEMANAGER_H
#define BEEBEEP_TRANSFERFILEMANAGER_H

#include "Config.h"
class Message;
class User;


class TransferFileManager : public QObject
{
  Q_OBJECT

public:
  TransferFileManager( QObject* );

  void sendFile( const User&, const QFileInfo& );

signals:
  void updateProgress( const QString&, int, int );

public slots:
  void messageReceived( const User&, const Message& );

private:
  QTcpServer* mp_server;

};


#endif // BEEBEEP_TRANSFERFILEMANAGER_H
