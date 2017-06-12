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
class User;


class GuiShareDesktop : public QMainWindow
{
  Q_OBJECT

public:
  GuiShareDesktop( QWidget* parent = 0 );

  void setUser( const User& );
  inline VNumber userId() const;
  inline void setChatId( VNumber );
  inline VNumber chatId() const;

  void updatePixmap( const QPixmap& );

public slots:
  void onUserChanged( const User& );

signals:
  void shareDesktopClosed( VNumber user_id, VNumber chat_id );

protected:
  void closeEvent( QCloseEvent* );

private:
  VNumber m_userId;
  VNumber m_chatId;
  QScrollArea* mp_scrollArea;
  QLabel* mp_lView;
  QDateTime m_lastUpdate;

};


// Inline Functions
inline VNumber GuiShareDesktop::userId() const { return m_userId; }
inline void GuiShareDesktop::setChatId( VNumber new_value ) { m_chatId = new_value; }
inline VNumber GuiShareDesktop::chatId() const { return m_chatId; }

#endif // BEEBEEP_GUISHAREDESKTOP_H
