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
// $Id$
//
//////////////////////////////////////////////////////////////////////

#ifndef BEEBEEP_GUILOG_H
#define BEEBEEP_GUILOG_H

#include "Config.h"
#include "ui_GuiLog.h"


class GuiLog : public QWidget, private Ui::GuiLogWidget
{
  Q_OBJECT

public:
  explicit GuiLog( QWidget *parent = 0 );
  void startCheckingLog();
  void stopCheckingLog();

protected slots:
  void refreshLog();

private:
  QTimer m_timer;

};

#endif // BEEBEEP_GUILOG_H
