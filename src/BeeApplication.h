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

#include <QApplication>
#include <QDateTime>
#include <QTimer>
#include <QLocalServer>

#ifdef Q_OS_UNIX
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
  void cleanUp();

  inline QThread* jobThread();

  static void quitAfterSignal( int );

signals:
  void enteringInIdle();
  void exitingFromIdle();
  void showUp();

protected:
  bool notify( QObject* receiver, QEvent* event );
  int idleTimeFromSystem();
  bool isScreenSaverRunning();
  QString localServerName() const;

protected slots:
  void checkIdle();
  void setIdle();
  void removeIdle();
  void slotConnectionEstablished();

private:
  int m_idleTimeout;
  QDateTime m_lastEventDateTime;
  QTimer m_timer;
  bool m_isInIdle;
  QThread* mp_jobThread;
  QLocalServer* mp_localServer;

#ifdef Q_OS_UNIX
  xcb_connection_t* mp_xcbConnection;
  xcb_screen_t* mp_xcbScreen;
  bool m_xcbConnectHasError;
#endif

};


// Inline Functions
inline QThread* BeeApplication::jobThread() { return mp_jobThread; }

#endif // BEEBEEP_APPLICATION_H
