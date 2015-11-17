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

#include "ShutdownMonitor.h"

#if QT_VERSION >= 0x050000

#ifdef Q_OS_WIN
  #include <windows.h>
#endif

bool ShutdownMonitor::nativeEventFilter( const QByteArray& event_type, void* event_message, long* event_result )
{
#ifdef Q_OS_WIN
  Q_UNUSED( event_type );
  MSG *pMsg = static_cast<MSG *>( event_message );

  if( pMsg->message == WM_QUERYENDSESSION )
  {
    qDebug() << "Windows Event END SESSION received. Quit and close";
    qApp->quit();
    *event_result = TRUE;
    return true;
  }
#else
  Q_UNUSED( event_type );
  Q_UNUSED( event_message );
  Q_UNUSED( event_result );
#endif

  return false;
}

#endif
