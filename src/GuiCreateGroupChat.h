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

#ifndef BEEBEEP_GUICREATEGROUPCHAT_H
#define BEEBEEP_GUICREATEGROUPCHAT_H

#include "ui_GuiCreateGroupChat.h"
#include "Config.h"
class Chat;


class GuiCreateGroupChat : public QDialog, private Ui::GuiCreateGroupChat
{
    Q_OBJECT
    
public:
  explicit GuiCreateGroupChat( QWidget *parent = 0 );

  void setGroupChat( const Chat& );
  inline const QList<VNumber>& groupUsersId() const;
  inline const QString& groupName() const;

protected slots:
  void updateGroupChat();

private:
  QList<VNumber> m_groupUsersId;
  QString m_groupName;

};


// Inline Functions
inline const QList<VNumber>& GuiCreateGroupChat::groupUsersId() const { return m_groupUsersId; }
inline const QString& GuiCreateGroupChat::groupName() const { return m_groupName; }

#endif // BEEBEEP_GUICREATEGROUPCHAT_H
