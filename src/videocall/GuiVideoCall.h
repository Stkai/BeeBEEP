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

#ifndef BEEBEEP_GUIVIDEOCALL_H
#define BEEBEEP_GUIVIDEOCALL_H

#include "Config.h"


class GuiVideoCall : public QMainWindow
{
  Q_OBJECT

public:
  GuiVideoCall( QWidget* parent = Q_NULLPTR );

  bool isVideoCallActive() const;

signals:
  void videoCallStarted();
  void videoCallClosed();

public slots:
  void startVideoCall();
  void closeVideoCall();

protected:
  void closeEvent( QCloseEvent* );


private:
  bool m_isVideoCallActive;

};


// Inline Functions
inline bool GuiVideoCall::isVideoCallActive() const { return m_isVideoCallActive; }

#endif // BEEBEEP_GUIVIDEOCALL_H
