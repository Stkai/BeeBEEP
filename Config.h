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
const int PONG_TIMEOUT = 90 * 1000;
const int PING_INTERVAL = 9 * 1000;
const int WRITING_MESSAGE_TIMEOUT = 3 * 1000;
const int BROADCAST_INTERVAL = 2000;
const unsigned BROADCAST_PORT = 45000;

#endif // BEEBEEP_CONFIG_H
