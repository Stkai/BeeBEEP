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
#include "TickManager.h"
#include <csignal>
#ifdef Q_OS_WIN
  #include <Windows.h>
#endif
#if defined( Q_OS_LINUX ) && !defined( Q_OS_ANDROID )
  // for check user inactivity time
  #include <xcb/xcb.h>
  #include <xcb/screensaver.h>
  // package libxcb-screensaver0-dev
  // package libx11-xcb-dev
#endif

BeeApplication *BeeApplication::mp_instance = Q_NULLPTR;


BeeApplication::BeeApplication( int& argc, char** argv  )
  : QApplication( argc, argv )
{
  setObjectName( "BeeApplication" );
  mp_instance = this;

  // when beebeep starts closed to tray last window is "ask password".
  // After auth "ask password" is closed and then the app quit
  // set it to true in QMainWindow closeEvent
  setQuitOnLastWindowClosed( false );

  m_idleTimeout = 0;
  m_isInIdle = false;
  mp_localServer = Q_NULLPTR;
  m_isInSleepMode = false;
  m_isDesktopLocked = false;

  mp_jobThread = new QThread();
  m_jobsInProgress = 0;
  mp_tickManager = Q_NULLPTR;

#ifdef Q_OS_LINUX
  m_xcbConnectHasError = true;
  if( testAttribute( Qt::AA_DontShowIconsInMenus ) )
    setAttribute( Qt::AA_DontShowIconsInMenus, false );
#endif
#if QT_VERSION >= 0x050600
  if( !testAttribute( Qt::AA_EnableHighDpiScaling ) )
    setAttribute( Qt::AA_EnableHighDpiScaling, true );
#endif
  addSleepWatcher();

  signal( SIGINT, &quitAfterSignal );
  signal( SIGTERM, &quitAfterSignal );
  signal( SIGABRT, &quitAfterSignal );
}

BeeApplication::~BeeApplication()
{
  if( mp_localServer )
  {
    mp_localServer->close();
    delete mp_localServer;
  }

  if( mp_instance )
    mp_instance = Q_NULLPTR;
}

void BeeApplication::forceShutdown()
{
  qDebug() << "Shutdown forced by user";
  emit( shutdownRequest() );
  quit();
}

void BeeApplication::quitAfterSignal( int sig )
{
  qWarning() << "Signal" << sig << "received by system";
  if( beeApp )
    beeApp->forceShutdown();
}

void BeeApplication::init()
{
  qDebug() << "Starting background thread and tick timer";
  mp_jobThread->start();
  mp_jobThread->setPriority( QThread::LowPriority );
  mp_tickManager = new TickManager;
  connect( mp_tickManager, SIGNAL( tickEvent( int ) ), this, SLOT( checkTicks( int ) ) );
  addJob( mp_tickManager );
  QMetaObject::invokeMethod( mp_tickManager, "startTicks", Qt::QueuedConnection );
}

void BeeApplication::forceSleep()
{
  if( m_isInSleepMode )
    return;
  qDebug() << "System goes to sleep...";
  m_isInSleepMode = true;
  emit sleepRequest();
  if( mp_tickManager->isActive() )
    QMetaObject::invokeMethod( mp_tickManager, "stopTicks", Qt::QueuedConnection );
}

void BeeApplication::wakeFromSleep()
{
  qDebug() << "System wakes up from sleep...";
  m_isInSleepMode = false;
  emit wakeUpRequest();
  if( !mp_tickManager->isActive() )
    QMetaObject::invokeMethod( mp_tickManager, "startTicks", Qt::QueuedConnection );
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

#if defined( Q_OS_LINUX ) && !defined( Q_OS_ANDROID )
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
  if( isScreenSaverRunning() || isDesktopLocked() || idleTimeFromSystem() > m_idleTimeout )
    setIdle();
  else
    removeIdle();
}

void BeeApplication::cleanUp()
{
  if( mp_tickManager )
  {
    QMetaObject::invokeMethod( mp_tickManager, "stopTicks", Qt::QueuedConnection );
    removeJob( mp_tickManager );
    mp_tickManager->deleteLater();
    mp_tickManager = Q_NULLPTR;
  }

#if defined( Q_OS_LINUX ) && !defined( Q_OS_ANDROID )
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
  screen_saver_is_running = static_cast<bool>(is_running);
#endif

#if defined( Q_OS_LINUX ) && !defined( Q_OS_ANDROID )
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
    idle_time = static_cast<int>(::GetTickCount() - idle_info.dwTime);
    idle_time = qMax( 0, idle_time / 1000 );
  }
#endif

#if defined( Q_OS_LINUX ) && !defined( Q_OS_ANDROID )
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
  idle_time = idleTimeFromMac();
#endif

  if( idle_time < 0 )
  {
    qint64 unsigned_idle_time = m_lastEventDateTime.secsTo( QDateTime::currentDateTime() );
    qint64 unsigned_idle_time_min = 0;
    idle_time = static_cast<int>( qMax( unsigned_idle_time_min, unsigned_idle_time ) );
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
  qDebug() << qPrintable( obj->objectName() ) << "moved to job thread." << m_jobsInProgress << "jobs in progress";
}

void BeeApplication::removeJob( QObject* obj )
{
  m_jobsInProgress--;
  qDebug() << qPrintable( obj->objectName() ) << "removed from job thread." << m_jobsInProgress << "jobs in progress";
}

void BeeApplication::checkTicks( int ticks )
{
  if( ticks > 31536000 )
  {
    // 1 year is passed ... it is time to close!
    qWarning() << "A year in uptime is passed. It is time to close and restart";
    QMetaObject::invokeMethod( this, "forceShutdown", Qt::QueuedConnection );
    return;
  }
  else
    emit tickEvent( ticks );
}

#if !defined( Q_OS_WIN ) && !defined( Q_OS_MAC )
void BeeApplication::addSleepWatcher()
{}
#endif

#if !defined( Q_OS_WIN )
void BeeApplication::setMainWidget( QWidget* w )
{
  mp_mainWidget = w;
}

void BeeApplication::isDesktopLocked()
{
  return m_isDesktopLocked;
}
#endif
