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
#include "ShutdownMonitor.h"
#include <QDebug>
#include <QEvent>
#include <QThread>
#include <QRegExp>
#include <QLocalSocket>

#include <csignal>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

#ifdef Q_OS_LINUX
// for check user inactivity time
#include <xcb/xcb.h>
#include <xcb/screensaver.h>
// package libxcb-screensaver0-dev
// package libx11-xcb-dev
#endif

#ifdef Q_OS_MAC
#include <ApplicationServices/ApplicationServices.h>
#endif


BeeApplication::BeeApplication( int& argc, char** argv  )
  : QApplication( argc, argv )
{
  setObjectName( "BeeApplication" );

  // when beebeep starts closed to tray last window is "ask password".
  // After auth "ask password" is closed and then the app quit
  // set it to true in QMainWindow closeEvent
  setQuitOnLastWindowClosed( false );

  m_idleTimeout = 0;
  m_timer.setObjectName( "BeeMainTimer" );
  m_timer.setInterval( 1000 );
  m_isInIdle = false;
  mp_localServer = 0;
  m_tickCounter = 0;

  mp_jobThread = new QThread();
  m_jobsInProgress = 0;

#ifdef Q_OS_LINUX
  m_xcbConnectHasError = true;
  if( testAttribute( Qt::AA_DontShowIconsInMenus ) )
    setAttribute( Qt::AA_DontShowIconsInMenus, false );
#endif

#if QT_VERSION >= 0x050000
  if( !testAttribute( Qt::AA_UseHighDpiPixmaps ) )
    setAttribute( Qt::AA_UseHighDpiPixmaps, true );
#endif

  connect( &m_timer, SIGNAL( timeout() ), this, SLOT( checkTick() ) );

  signal( SIGINT, &quitAfterSignal );
  signal( SIGTERM, &quitAfterSignal );
  signal( SIGABRT, &quitAfterSignal );

#if QT_VERSION >= 0x050000
  mp_shutdownMonitor = new ShutdownMonitor;
  installNativeEventFilter( mp_shutdownMonitor );
#else
  mp_shutdownMonitor = 0;
#endif
}

BeeApplication::~BeeApplication()
{
  if( mp_localServer )
  {
    mp_localServer->close();
    delete mp_localServer;
  }

  if( mp_shutdownMonitor )
  {
#if QT_VERSION >= 0x050000
    removeNativeEventFilter( mp_shutdownMonitor );
#endif
    delete mp_shutdownMonitor;
  }
}

void BeeApplication::quitAfterSignal( int sig )
{
  qWarning() << "Signal" << sig << "received. Quit and close.";
  qApp->quit();
}

void BeeApplication::init()
{
  qDebug() << "Starting background thread and tick timer";
  mp_jobThread->start();
  mp_jobThread->setPriority( QThread::LowPriority );
  m_timer.start();
}

void BeeApplication::setIdleTimeout( int new_value )
{
  if( new_value <= 0 )
  {
    qWarning() << "Unable to set idle timout to value:" << new_value;
    m_idleTimeout = 0;
    return;
  }

  m_idleTimeout = new_value * 60;

#ifdef Q_OS_LINUX
  mp_xcbConnection = xcb_connect( 0, 0 );
  m_xcbConnectHasError = xcb_connection_has_error( mp_xcbConnection ) > 0;
  if( m_xcbConnectHasError )
    qWarning() << "XCB: unable to connect to current DISPLAY. Impossible to get idle time";
  else
    mp_xcbScreen = xcb_setup_roots_iterator( xcb_get_setup( mp_xcbConnection ) ).data;
#endif
}

void BeeApplication::setIdle()
{
  if( m_isInIdle )
    return;

  m_isInIdle = true;
  emit( enteringInIdle() );
}

void BeeApplication::removeIdle()
{
  if( !m_isInIdle )
    return;

  m_isInIdle = false;
  emit( exitingFromIdle() );
}

bool BeeApplication::notify( QObject* obj_receiver, QEvent* obj_event )
{
  if( obj_event->type() == QEvent::MouseMove || obj_event->type() == QEvent::KeyPress )
  {
    m_lastEventDateTime = QDateTime::currentDateTime();
    if( m_isInIdle )
      removeIdle();
  }

  return QApplication::notify( obj_receiver, obj_event );
}

void BeeApplication::checkIdle()
{
  if( isScreenSaverRunning() || idleTimeFromSystem() > m_idleTimeout )
    setIdle();
  else
    removeIdle();
}

void BeeApplication::cleanUp()
{
  if( m_timer.isActive() )
    m_timer.stop();

#ifdef Q_OS_LINUX
  if( m_idleTimeout > 0 )
  {
 // mp_xcbScreen not need to free
 // disconnect from display and free memory
    xcb_disconnect( mp_xcbConnection );
  }
#endif

  if( mp_jobThread->isRunning() && m_jobsInProgress > 0 )
  {
    qDebug() << m_jobsInProgress << "jobs in progress. Wait 2 seconds before quit";
    mp_jobThread->wait( 2000 );
  }
  else
    qDebug() << "No jobs in progress. Quit job thread";

  mp_jobThread->quit();
  mp_jobThread->deleteLater();
}

bool BeeApplication::isScreenSaverRunning()
{
  bool screen_saver_is_running = false;

#ifdef Q_OS_WIN
  BOOL is_running = FALSE;
  SystemParametersInfo( SPI_GETSCREENSAVERRUNNING, 0, &is_running, 0 );
  screen_saver_is_running = (bool)is_running;
#endif

#ifdef Q_OS_LINUX
  if( !m_xcbConnectHasError )
  {
    xcb_screensaver_query_info_cookie_t xcbCookie;
    xcb_screensaver_query_info_reply_t* xcbInfo;

    xcbCookie = xcb_screensaver_query_info( mp_xcbConnection, mp_xcbScreen->root );
    xcbInfo = xcb_screensaver_query_info_reply( mp_xcbConnection, xcbCookie, 0 );
    if( xcbInfo )
    {
      screen_saver_is_running = xcbInfo->state == XCB_SCREENSAVER_STATE_ON || xcbInfo->state == XCB_SCREENSAVER_STATE_CYCLE;
      free( xcbInfo );
    }
  }
#endif

  return screen_saver_is_running;
}

int BeeApplication::idleTimeFromSystem()
{
  int idle_time = -1;

#ifdef Q_OS_WIN
  LASTINPUTINFO idle_info;
  idle_info.cbSize = sizeof( LASTINPUTINFO );
  if( ::GetLastInputInfo( &idle_info ) )
  {
    idle_time = ::GetTickCount() - idle_info.dwTime;
    idle_time = qMax( 0, idle_time / 1000 );
  }
#endif

#ifdef Q_OS_LINUX
  /*
  // gcc -o getIdleTime getIdleTime.c -lXss
#include <X11/extensions/scrnsaver.h>
#include <stdio.h>

int main(void) {
  Display *dpy = XOpenDisplay(NULL);

  if (!dpy) {
    return(1);
  }

  XScreenSaverInfo *info = XScreenSaverAllocInfo();
  XScreenSaverQueryInfo(dpy, DefaultRootWindow(dpy), info);
  printf("%u", info->idle);

  return(0);
}
*/

  if( !m_xcbConnectHasError )
  {
    xcb_screensaver_query_info_cookie_t xcbCookie;
    xcb_screensaver_query_info_reply_t* xcbInfo;

    xcbCookie = xcb_screensaver_query_info( mp_xcbConnection, mp_xcbScreen->root);
    xcbInfo = xcb_screensaver_query_info_reply( mp_xcbConnection, xcbCookie, 0 );
    if( xcbInfo )
    {
      idle_time = qMax( 0, (int)xcbInfo->ms_since_user_input / 1000 );
      free ( xcbInfo );
    }
  }
#endif

#ifdef Q_OS_MAC
  CFTimeInterval macx_idle_secs = CGEventSourceSecondsSinceLastEventType( kCGEventSourceStateHIDSystemState, kCGAnyInputEventType );
  idle_time = static_cast<int>( qMax( 0.0, macx_idle_secs ) );
#endif

  if( idle_time < 0 )
  {
    quint64 unsigned_idle_time = m_lastEventDateTime.secsTo( QDateTime::currentDateTime() );
    idle_time = static_cast<int>( qMax( (quint64)0, unsigned_idle_time ) );
  }

  return idle_time;
}

void BeeApplication::slotConnectionEstablished()
{
  qDebug() << "New instance is called by user. Show the main window";
  QLocalSocket* local_socket = mp_localServer->nextPendingConnection();
  local_socket->close();
  local_socket->deleteLater();
  emit showUp();
}

QString BeeApplication::localServerName() const
{
  QString server_name = QString( "%1_%2_%3" ).arg( organizationDomain(), organizationName(), applicationName() );
  server_name.replace( QRegExp("[^\\w\\-. ]"), "" );
  return server_name;
}

void BeeApplication::preventMultipleInstances()
{
  if( mp_localServer )
    return;
  mp_localServer = new QLocalServer();
  QString server_name = localServerName();
  mp_localServer->removeServer( server_name );
  mp_localServer->listen( server_name );
  QObject::connect( mp_localServer, SIGNAL( newConnection() ), this, SLOT( slotConnectionEstablished() ) );
}

bool BeeApplication::otherInstanceExists()
{
  // Attempt to connect to the LocalServer
  QLocalSocket local_socket;
  local_socket.connectToServer( localServerName() );
  if( local_socket.waitForConnected( 2000 ) )
  {
    qDebug() << "Another instance of" << localServerName() << "exists";
    local_socket.close();
    return true;
  }

  return false;
}

void BeeApplication::addJob( QObject* obj )
{
  obj->moveToThread( mp_jobThread );
  m_jobsInProgress++;
  qDebug() << obj->objectName() << "moved to job thread." << m_jobsInProgress << "jobs in progress";
}

void BeeApplication::removeJob( QObject* obj )
{
  m_jobsInProgress--;
  qDebug() << obj->objectName() << "removed from job thread." << m_jobsInProgress << "jobs in progress";
}

void BeeApplication::checkTick()
{
  m_tickCounter++;

  if( m_tickCounter > 31536000 )
  {
    // 1 year is passed ... it is time to close!
    qWarning() << "A year in uptime is passed. It is time to close and restart";
    m_timer.stop();
    QTimer::singleShot( 0, this, SLOT( quit() ) );
    return;
  }
  else
    emit tickEvent( m_tickCounter );
}
