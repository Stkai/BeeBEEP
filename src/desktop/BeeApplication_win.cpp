//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2021 Marco Mastroddi
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
// along with BeeBEEP. If not, see <http://www.gnu.org/licenses/>.
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
// For WTSRegisterSessionNotification and Session Change message
#include <WtsApi32.h>


bool BeeApplication::setSessionNotificationForWindow( HWND h_wnd )
{
  if( WTSRegisterSessionNotification( reinterpret_cast<HWND>( h_wnd ), NOTIFY_FOR_THIS_SESSION ) )
  {
    m_mainWindowHandle = h_wnd;
    return true;
  }
  else
  {
    qWarning() << "Unable to register session notification for main window due the error code" << QString::number( ::GetLastError() );
    return false;
  }
}

bool BeeApplication::resetSessionNotificationForWindow()
{
  if( m_mainWindowHandle )
  {
    if( !WTSUnRegisterSessionNotification( m_mainWindowHandle ) )
    {
      qWarning() << "Unable to unregister session notification for main window due the error code" << QString::number( ::GetLastError() );
      return false;
    }
    else
      m_mainWindowHandle = nullptr;
  }
  return true;
}

bool BeeApplication::winEventFilter( MSG* event_message, long* event_result )
{
  Q_UNUSED( event_result )
  if( event_message->message == WM_POWERBROADCAST )
  {
    if( event_message->wParam == PBT_APMSUSPEND )
      forceSleep();
    else if( event_message->wParam == PBT_APMRESUMESUSPEND )
      wakeFromSleep();
    else
      ignoreEvent( "" );
  }
  else if( event_message->message == WM_WTSSESSION_CHANGE )
  {
    if( event_message->wParam == WTS_SESSION_LOCK )
    {
#ifdef BEEBEEP_DEBUG
      qDebug() << "Session change detected: desktop is locked";
#endif
      if( !m_isDesktopLocked )
      {
        m_isDesktopLocked = true;
        setIdle();
      }
    }
    else if( event_message->wParam == WTS_SESSION_UNLOCK )
    {
#ifdef BEEBEEP_DEBUG
      qDebug() << "Session change detected: desktop is now unlocked";
#endif
      if( m_isDesktopLocked )
        m_isDesktopLocked = false;
    }
    else if( event_message->wParam == WTS_SESSION_LOGOFF )
    {
#ifdef BEEBEEP_DEBUG
      qDebug() << "Session change detected: logoff";
#endif
      forceShutdown();
    }
    else
      ignoreEvent( "" );
  }
  else
    ignoreEvent( "" );

  return false; // Qt must handle event every time
}

bool IsDesktopAvailable()
{
  HDESK desktop = OpenDesktop( TEXT( "Default" ), 0, false, DESKTOP_SWITCHDESKTOP );
  if( desktop )
  {
    if( SwitchDesktop( desktop ) )
    {
      CloseDesktop(desktop);
      return true;
    }
    else
    {
      CloseDesktop( desktop );
    }
  }
  return false;
}

bool BeeApplication::isDesktopLocked()
{
  // Using Win Event Filter instead of !isDesktopAvailable()
  return m_isDesktopLocked;
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
        if( beeApp )
          return beeApp->winEventFilter( event_message, event_result );
      }
    }
    return false;
  }
};

void BeeApplication::addSleepWatcher()
{
  QAbstractNativeEventFilter* anef = new BeeWindowsEventFilter();
  installNativeEventFilter( anef );
  mp_sleepWatcher = reinterpret_cast<QObject*>( anef );
}

#else
void BeeApplication::addSleepWatcher()
{}

#endif // Qt versions

