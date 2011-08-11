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

#ifndef BEEBEEP_FILETRANSFERUPLOAD_H
#define BEEBEEP_FILETRANSFERUPLOAD_H

#include "FileTransferPeer.h"


class FileTransferUpload : public FileTransferPeer
{
  Q_OBJECT

public:
  explicit FileTransferUpload( VNumber, QObject *parent = 0 );

  void startTransfer( const FileInfo& );

signals:
  void fileTransferRequest( VNumber, const QByteArray& );

protected slots:
  void sendData();
  void checkData( const QByteArray& );

protected:
  void checkRequest( const QByteArray& );
  void checkSending( const QByteArray& );

};

#endif // BEEBEEP_FILETRANSFERUPLOAD_H
