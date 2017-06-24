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

#ifndef BEEBEEP_APPLICATION_H
#define BEEBEEP_APPLICATION_H

#include "Config.h"
class TickManager;

#ifdef Q_OS_LINUX
struct xcb_connection_t;
struct xcb_screen_t;
#endif


class BeeApplication : public QApplication
{
  Q_OBJECT

public:
  BeeApplication( int& argc, char** argv );
  ~BeeApplication();

  bool otherInstanceExists();
  void preventMultipleInstances();

  void init();
  void setIdleTimeout( int ); // in minutes
  inline int idleTimeout() const;
  void cleanUp();

  void addJob( QObject* );
  void removeJob( QObject* );

  static void quitAfterSignal( int );

  void forceSleep();
  inline bool isInSleepMode() const;
  void wakeFromSleep();

#ifdef Q_OS_WIN
  bool winEventFilter( MSG*, long* );
#endif

signals:
  void enteringInIdle();
  void exitingFromIdle();
  void showUp();
  void tickEvent( int );
  void shutdownRequest();
  void sleepRequest();
  void wakeUpRequest();

public slots:
  void checkIdle();
  void forceShutdown();

protected:
  bool notify( QObject* receiver, QEvent* event );

  int idleTimeFromSystem();
#ifdef Q_OS_MAC
  int idleTimeFromMac();
#endif
  bool isScreenSaverRunning();
  QString localServerName() const;
  void addSleepWatcher();

protected slots:
  void checkTicks( int );
  void setIdle();
  void removeIdle();
  void slotConnectionEstablished();

private:
  int m_idleTimeout;
  QDateTime m_lastEventDateTime;
  bool m_isInIdle;
  QThread* mp_jobThread;
  QLocalServer* mp_localServer;
  int m_jobsInProgress;

#ifdef Q_OS_LINUX
  xcb_connection_t* mp_xcbConnection;
  xcb_screen_t* mp_xcbScreen;
  bool m_xcbConnectHasError;
#endif

  bool m_isInSleepMode;

  TickManager* mp_tickManager;

};

// Inline Functions
inline int BeeApplication::idleTimeout() const { return m_idleTimeout; }
inline bool BeeApplication::isInSleepMode() const { return m_isInSleepMode; }

#endif // BEEBEEP_APPLICATION_H
