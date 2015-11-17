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

#ifndef BEEBEEP_SHUTDOWNMONITOR_H
#define BEEBEEP_SHUTDOWNMONITOR_H

#include "Config.h"

#if QT_VERSION >= 0x050000

class ShutdownMonitor : public QAbstractNativeEventFilter
{
public:
  bool nativeEventFilter( const QByteArray& event_type, void* event_message, long* event_result );

};

#else

class ShutdownMonitor
{
};

#endif

#endif // BEEBEEP_SHUTDOWNMONITOR_H
