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


#ifndef BEEBEEP_GUISYSTEMTRAY_H
#define BEEBEEP_GUISYSTEMTRAY_H

#include <QSystemTrayIcon>

class GuiSystemTray : public QSystemTrayIcon
{
  Q_OBJECT

public:
  enum IconStatus { Default, Message };

  explicit GuiSystemTray( QObject *parent = 0 );
  void addUnreadMessage( int );
  void setUnreadMessages( int );

public slots:
  void setDefaultIcon();
  void setMessageIcon();

protected:
  virtual void showIcon();

private:
  int m_iconStatus;
  int m_unreadMessages;

};

#endif // BEEBEEP_GUISYSTEMTRAY_H
