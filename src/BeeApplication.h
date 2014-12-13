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
// Author: Marco Mastroddi (marco.mastroddi(AT)gmail.com)
//
// $Id: GuiChatList.h 236 2014-01-09 17:13:39Z mastroddi $
//
//////////////////////////////////////////////////////////////////////

#ifndef BEEBEEP_APPLICATION_H
#define BEEBEEP_APPLICATION_H

#include <QApplication>
#include <QDateTime>
#include <QTimer>


class BeeApplication : public QApplication
{
  Q_OBJECT

public:
  BeeApplication( int& argc, char** argv );

  void setIdleTimeout( int ); // in minutes
  void cleanUp();

signals:
  void checkIdleRequest();

protected:
  bool notify( QObject* receiver, QEvent* event );

protected slots:
  void checkIdle();

private:
  int m_idleTimeout;
  QDateTime m_lastEventDateTime;
  QTimer m_timer;

};


#endif // BEEBEEP_APPLICATION_H