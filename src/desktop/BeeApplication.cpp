//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2020 Marco Mastroddi
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

static void QuitAfterSignal( int sig )
{
  qWarning() << "Signal" << sig << "received by system";
  if( beeApp )
    beeApp->forceShutdown();
}

#ifdef Q_OS_LINUX
static void SleepAfterSignal( int sig )
{
  qWarning() << "Signal" << sig << "received by system";
  if( beeApp )
    beeApp->forceSleep();
}

static void WakeAfterSignal( int sig )
{
  qWarning() << "Signal" << sig << "received by system";
  if( beeApp )
    beeApp->wakeFromSleep();
}
#endif


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
  mp_sleepWatcher = Q_NULLPTR;

  m_defaultCss = styleSheet();

  mp_fsWatcher = new QFileSystemWatcher;
  m_settingsFilePath = "";
  connect( mp_fsWatcher, SIGNAL( fileChanged( const QString& ) ), this, SLOT( onFileChanged( const QString& ) ) );

  mp_networkConfigurationManager = new QNetworkConfigurationManager;
  connect( mp_networkConfigurationManager, SIGNAL( configurationAdded( const QNetworkConfiguration& ) ), this, SLOT( onNetworkConfigurationAdded( const QNetworkConfiguration& ) ) );
  connect( mp_networkConfigurationManager, SIGNAL( configurationChanged( const QNetworkConfiguration& ) ), this, SLOT( onNetworkConfigurationChanged( const QNetworkConfiguration& ) ) );
  connect( mp_networkConfigurationManager, SIGNAL( configurationRemoved( const QNetworkConfiguration& ) ), this, SLOT( onNetworkConfigurationRemoved( const QNetworkConfiguration& ) ) );
  connect( mp_networkConfigurationManager, SIGNAL( onlineStateChanged( bool ) ), this, SLOT( onNetworkMagnagerOnlineStateChanged( bool ) ) );

#if QT_VERSION >= 0x050200
  connect( this, SIGNAL( applicationStateChanged( Qt::ApplicationState ) ), this, SLOT( onApplicationStateChanged( Qt::ApplicationState ) ) );
#endif

#ifdef Q_OS_LINUX
  m_xcbConnectHasError = true;
  if( testAttribute( Qt::AA_DontShowIconsInMenus ) )
    setAttribute( Qt::AA_DontShowIconsInMenus, false );
  signal( SIGTSTP, &SleepAfterSignal );
  signal( SIGCONT, &WakeAfterSignal );
#endif
  addSleepWatcher();

  signal( SIGINT, &QuitAfterSignal );
  signal( SIGTERM, &QuitAfterSignal );
  signal( SIGABRT, &QuitAfterSignal );
}

BeeApplication::~BeeApplication()
{
  if( mp_localServer )
  {
    mp_localServer->close();
    delete mp_localServer;
  }

  if( mp_sleepWatcher )
    removeEventFilter( mp_sleepWatcher );

  clearPathsInFsWatcher();
  delete mp_fsWatcher;
  delete mp_networkConfigurationManager;

  if( mp_instance )
    mp_instance = Q_NULLPTR;
}

void BeeApplication::setSettingsFilePath( const QString& settings_file_path )
{
  if( !m_settingsFilePath.isEmpty() )
    mp_fsWatcher->removePath( m_settingsFilePath );
  m_settingsFilePath = "";
  if( !settings_file_path.isEmpty() )
  {
#if QT_VERSION >= 0x050000
    if( mp_fsWatcher->addPath( settings_file_path ) )
      m_settingsFilePath = settings_file_path;
#else
    mp_fsWatcher->addPath( settings_file_path );
    m_settingsFilePath = settings_file_path;
#endif
  }
}

void BeeApplication::clearPathsInFsWatcher()
{
  QStringList sl_paths = mp_fsWatcher->directories();
  if( !sl_paths.isEmpty() )
    mp_fsWatcher->removePaths( sl_paths );
  sl_paths = mp_fsWatcher->files();
  if( !sl_paths.isEmpty() )
    mp_fsWatcher->removePaths( sl_paths );
}

void BeeApplication::forceShutdown()
{
  qDebug() << "Shutdown forced by user";
  emit( shutdownRequest() );
  quit();
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
  qDebug() << "Network configuration manager is starting";
  QMetaObject::invokeMethod( mp_networkConfigurationManager, "updateConfigurations", Qt::QueuedConnection );
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
  if( !m_isInSleepMode )
    return;
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

  if( !obj_receiver )
    return false;
  else
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
  QProcessEnvironment pe = QProcessEnvironment::systemEnvironment();
  QString account_name = pe.value( "USERNAME", "" );
  if( account_name.isEmpty() )
    account_name = pe.value( "USER", "" );
  QString server_name = QString( "%1_%2_%3_%4" ).arg( organizationDomain(), organizationName(), applicationName(), account_name.isEmpty() ? QString( "k" ) : account_name );
  return QString( "beebeep_%1" ).arg( QString::fromLatin1( QCryptographicHash::hash( server_name.toUtf8().toBase64(), QCryptographicHash::Md5 ).toHex() ) );
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
  qDebug() << "BeeBEEP starts with instance" << qPrintable( server_name );
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

bool BeeApplication::isDesktopLocked()
{
  return m_isDesktopLocked;
}
#endif

void BeeApplication::resetStyle()
{
  setStyleSheet( m_defaultCss );
}

void BeeApplication::ignoreEvent( const QString& log_text = "" )
{
  if( !log_text.isEmpty() )
    qDebug() << log_text;
}

void BeeApplication::onFileChanged( const QString& file_path )
{
  if( file_path == m_settingsFilePath )
    qWarning() << "Settings file changed: please edit the settings file only if BeeBEEP is closed or changes will be lost";
}

#if QT_VERSION >= 0x050200
void BeeApplication::onApplicationStateChanged( Qt::ApplicationState state )
{
  if( state == Qt::ApplicationSuspended )
    forceSleep();
}
#endif

void BeeApplication::onNetworkConfigurationAdded( const QNetworkConfiguration& net_conf )
{
  qDebug() << "Network configuration is added:" << qPrintable( net_conf.name() ) << "-" << qPrintable( net_conf.identifier() ) << "-" << qPrintable( net_conf.bearerTypeName() );
  onNetworkConfigurationChanged( net_conf );
}

void BeeApplication::onNetworkConfigurationChanged( const QNetworkConfiguration& net_conf )
{
  if( !net_conf.isValid() )
  {
    qWarning() << "Network configuration" << qPrintable( net_conf.name() ) << "-" << qPrintable( net_conf.identifier() ) << "is not valid";
    return;
  }
  qDebug() << "Checking network configuration:" << qPrintable( net_conf.name() ) << "-" << qPrintable( net_conf.identifier() )
           << "- state:" << net_conf.state()
           << "- bearer:" << qPrintable( net_conf.bearerTypeName() ) << net_conf.bearerType() << net_conf.bearerTypeFamily()
           << "- purpose:" << net_conf.purpose()
           << "- type:" << net_conf.type();
}

void BeeApplication::onNetworkConfigurationRemoved( const QNetworkConfiguration& net_conf )
{
  qDebug() << "Network configuration is removed:" << qPrintable( net_conf.name() ) << "-" << qPrintable( net_conf.identifier() ) << "-" << qPrintable( net_conf.bearerTypeName() );
}

void BeeApplication::onNetworkMagnagerOnlineStateChanged( bool is_online )
{
  qDebug() << "Network configuration manager has changed state to" << (is_online ? "online" : "offline");
  if( is_online )
    wakeFromSleep();
  else
    forceSleep();
}

