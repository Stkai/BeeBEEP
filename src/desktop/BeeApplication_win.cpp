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

#include "BeeApplication.h"

#if QT_VERSION < 0x050000  // qt 4.8
#include "qt_windows.h"
#endif

bool BeeApplication::winEventFilter( MSG* event_message, long* event_result )
{
  Q_UNUSED( event_result )
  if( event_message->message == WM_POWERBROADCAST )
  {
    if( event_message->wParam == PBT_APMSUSPEND )
      forceSleep();

    if( event_message->wParam == PBT_APMRESUMESUSPEND )
      wakeFromSleep();
  }

  return false; // Qt must handle event every time
}

#if QT_VERSION >= 0x050000
class BeeWindowsEventFilter : public QAbstractNativeEventFilter
{
public:
  virtual bool nativeEventFilter( const QByteArray& event_type, void *event_msg, long* event_result ) Q_DECL_OVERRIDE
  {
    if( event_type == "windows_generic_MSG" )
    {
      MSG* event_message = static_cast<MSG*>( event_msg );
      if( event_message )
      {
        BeeApplication* bee_app = (BeeApplication*)qApp;
        return bee_app->winEventFilter( event_message, event_result );
      }
    }
    return false;
  }
};

void BeeApplication::addSleepWatcher()
{
  installNativeEventFilter( new BeeWindowsEventFilter() );
}

#else
void BeeApplication::addSleepWatcher()
{}

#endif // Qt versions

