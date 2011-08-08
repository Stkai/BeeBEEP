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

#ifndef BEEBEEP_CONFIG_H
#define BEEBEEP_CONFIG_H

#include <QtCore>
#include <QtGui>
#include <QtNetwork>

const int MAX_BUFFER_SIZE = 1024000;
const int TRANSFER_TIMEOUT = 30 * 1000;
const int PONG_TIMEOUT = 84 * 1000;
const int PING_INTERVAL = 21 * 1000;
const int WRITING_MESSAGE_TIMEOUT = 3 * 1000;
const int BROADCAST_INTERVAL = 2000;
const unsigned BROADCAST_PORT = 36475;
const unsigned LISTENER_DEFAULT_PORT = 6475;
const int DATASTREAM_VERSION = QDataStream::Qt_4_0;

#define DATA_BLOCK_SIZE quint16
#define ENCRYPTED_DATA_BLOCK_SIZE 16
#define ENCRYPTION_KEYBITS 256

// Protocol
#define ID_LOCAL_USER      1
#define ID_START           100
#define ID_SYSTEM_MESSAGE  10
#define ID_BEEP_MESSAGE    11
#define ID_WRITING_MESSAGE 12
#define ID_PING_MESSAGE    13
#define ID_PONG_MESSAGE    14
#define ID_HELLO_MESSAGE   15
#define ID_STATUS_MESSAGE  16

#endif // BEEBEEP_CONFIG_H
