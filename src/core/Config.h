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

#ifndef BEEBEEP_CONFIG_H
#define BEEBEEP_CONFIG_H

#include <QtCore>
#include <QtGui>
#include <QtNetwork>

#if QT_VERSION >= 0x050000
  #include <QtWidgets>
  #include <QtMultimedia>
  #include <QtPrintSupport>
#else
  typedef int qintptr;
#endif

#ifdef __arm__
  #define BEEBEEP_FOR_RASPBERRY_PI
#else
  #undef BEEBEEP_FOR_RASPBERRY_PI
#endif

// Type definition: define doesn't always work in signal slot mechanish
//#define VNumber quint64
//#define FileSizeType quint64
typedef quint64 VNumber;
typedef quint64 FileSizeType;

// Datastream QDataStream::Qt_4_0 = 7
#define DATASTREAM_VERSION_1 7

// Datastream QDataStream::Qt_4_8 = QDataStream::Qt_4_6 = 12
#define DATASTREAM_VERSION_2 12

#if QT_VERSION < 0x040600
  #define LAST_DATASTREAM_VERSION DATASTREAM_VERSION_1
#else
  #define LAST_DATASTREAM_VERSION DATASTREAM_VERSION_2
#endif

// Broadcast
const int DEFAULT_BROADCAST_PORT = 36475;
const int DEFAULT_LISTENER_PORT = 6475;
const int DEFAULT_FILE_TRANSFER_PORT = 6476;

// Connection I/O
#define DATA_BLOCK_SIZE_16 quint16
// set the limit to 65535 - sizeof( quint16 ) ... or a value near it
const DATA_BLOCK_SIZE_16 DATA_BLOCK_SIZE_16_LIMIT = 65519;
#define DATA_BLOCK_SIZE_32 quint32
//Due to compiler warning (ISO C90) the value 4294967295 of quint32 is not possibile... use qint32 - sizeof( qint32 )
const qint32 DATA_BLOCK_SIZE_32_LIMIT = 2147483615;
#define ENCRYPTED_DATA_BLOCK_SIZE 16
#define ENCRYPTION_KEYBITS 256
#define MAX_NUM_OF_LOOP_IN_CONNECTON_SOCKECT 40
const int SECURE_LEVEL_2_PROTO_VERSION = 60;
const int FILE_TRANSFER_2_PROTO_VERSION = 63;
const int HIVE_PROTO_VERSION = 64;
// Tick interval in ms
const int TICK_INTERVAL = 1000;
const int PING_INTERVAL_IDLE = 4700;
const int PING_INTERVAL_TICK = 7;
const int AUTO_BROADCAST_CHECK_TICK = 31;

// Protocol
#define ID_INVALID                0
#define ID_LOCAL_USER             1
#define ID_DEFAULT_CHAT           2
#define ID_START                  1000
#define ID_SYSTEM_MESSAGE         10
#define ID_BEEP_MESSAGE           11
#define ID_WRITING_MESSAGE        12
#define ID_PING_MESSAGE           13
#define ID_PONG_MESSAGE           14
#define ID_HELLO_MESSAGE          15
#define ID_USER_MESSAGE           16
#define ID_VCARD_MESSAGE          17
#define ID_SHARE_MESSAGE          18
#define ID_SHAREBOX_MESSAGE       19
#define ID_DOTDOT_FOLDER          20
#define ID_SHAREDESKTOP_MESSAGE   21
#define ID_BUZZ_MESSAGE           22

#endif // BEEBEEP_CONFIG_H
