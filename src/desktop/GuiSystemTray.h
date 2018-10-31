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

#ifndef BEEBEEP_GUISYSTEMTRAY_H
#define BEEBEEP_GUISYSTEMTRAY_H

#include "Config.h"


class GuiSystemTray : public QSystemTrayIcon
{
  Q_OBJECT

public:
  enum IconStatus { Default, Message };

  explicit GuiSystemTray( QObject *parent = Q_NULLPTR );

  void showNewFileArrived( VNumber, const QString&, bool );
  void showNewMessageArrived( VNumber, const QString&, bool );
  void showUserStatusChanged( VNumber, const QString& );
  void setUnreadMessages( VNumber, int );
  inline VNumber chatId() const;
  inline void resetChatId();

  inline bool iconStatusIsMessage() const;

public slots:
  void setDefaultIcon();
  void setMessageIcon();
  void onTickEvent( int );

protected:
  void showMessageInTray( const QString&, bool );

private:
  int m_iconStatus;
  VNumber m_chatId;

};


// Inline Functions
inline VNumber GuiSystemTray::chatId() const { return m_chatId; }
inline void GuiSystemTray::resetChatId() { m_chatId = ID_INVALID; }
inline bool GuiSystemTray::iconStatusIsMessage() const { return m_iconStatus == GuiSystemTray::Message; }

#endif // BEEBEEP_GUISYSTEMTRAY_H
