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
#ifdef Q_OS_MAC
#include <ApplicationServices/ApplicationServices.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <mach/mach_port.h>
#include <mach/mach_interface.h>
#include <mach/mach_init.h>
#include <IOKit/pwr_mgt/IOPMLib.h>
#include <IOKit/IOMessage.h>

io_connect_t  mac_io_root_port; // a reference to the Root Power Domain IOService

int BeeApplication::idleTimeFromMac()
{
  CFTimeInterval macx_idle_secs = CGEventSourceSecondsSinceLastEventType( kCGEventSourceStateHIDSystemState, kCGAnyInputEventType );
  int idle_time = static_cast<int>( qMax( 0.0, macx_idle_secs ) );
  return idle_time;
}

void BeeMacSleepCallBack( void * ref_con, io_service_t io_srv, natural_t message_type, void* message_argument )
{
  Q_UNUSED( ref_con )
  Q_UNUSED( io_srv )

  switch ( message_type )
  {
  case kIOMessageCanSystemSleep:
    /* Idle sleep is about to kick in. This message will not be sent for forced sleep.
       Applications have a chance to prevent sleep by calling IOCancelPowerChange.
       Most applications should not prevent idle sleep.
       Power Management waits up to 30 seconds for you to either allow or deny idle
       sleep. If you don't acknowledge this power change by calling either
       IOAllowPowerChange or IOCancelPowerChange, the system will wait 30
       seconds then go to sleep.
    */

    //Uncomment to cancel idle sleep
    //IOCancelPowerChange( root_port, (long)messageArgument );
    // we will allow idle sleep
    IOAllowPowerChange( mac_io_root_port, (long)message_argument );
    break;

  case kIOMessageSystemWillSleep:
    /* The system WILL go to sleep. If you do not call IOAllowPowerChange or
       IOCancelPowerChange to acknowledge this message, sleep will be
       delayed by 30 seconds.

       NOTE: If you call IOCancelPowerChange to deny sleep it returns
             kIOReturnSuccess, however the system WILL still go to sleep.
    */
    {
      IOAllowPowerChange( mac_io_root_port, (long)message_argument );
      if( beeApp )
        beeApp->forceSleep();
    }
    break;

  case kIOMessageSystemWillPowerOn:
     //System has started the wake up process...
     break;

  case kIOMessageSystemHasPoweredOn:
     //System has finished waking up...
     {
       if( beeApp )
         beeApp->wakeFromSleep();
     }
     break;

  default:
    // do nothing
    break;
  }
}

void BeeApplication::addSleepWatcher()
{
  // notification port allocated by IORegisterForSystemPower
  static IONotificationPortRef  notify_port_ref;
  // notifier object, used to deregister later
  static io_object_t            notifier_object;
  // this parameter is passed to the callback
  static void*                  ref_con;

  // register to receive system sleep notifications
  mac_io_root_port = IORegisterForSystemPower( ref_con, &notify_port_ref, BeeMacSleepCallBack, &notifier_object );
  if( mac_io_root_port == 0 )
  {
    qWarning() << "Unable to register callback for system power";
    return;
  }

  // add the notification port to the application runloop
  CFRunLoopAddSource( CFRunLoopGetCurrent(), IONotificationPortGetRunLoopSource( notify_port_ref ), kCFRunLoopCommonModes );
}
#endif

