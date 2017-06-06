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

#ifndef BEEBEEP_GUISHAREDESKTOP_H
#define BEEBEEP_GUISHAREDESKTOP_H

#include "Config.h"
#include "ui_GuiShareDesktop.h"
class User;


class GuiShareDesktop : public QWidget, private Ui::GuiShareDesktopWidget
{
  Q_OBJECT

public:
  GuiShareDesktop( QWidget* parent = 0 );

  void setOwner( const User& );
  inline VNumber ownerId() const;

  void updatePixmap( const QPixmap& );

public slots:
  void onUserChanged( const User& );

signals:
  void shareDesktopClosed( VNumber );

protected:
  void closeEvent( QCloseEvent* );

private:
  VNumber m_ownerId;
  QPixmap m_lastImage;

};


// Inline Functions
inline VNumber GuiShareDesktop::ownerId() const { return m_ownerId; }


#endif // BEEBEEP_GUISHAREDESKTOP_H
