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

#ifndef BEEBEEP_GUIADDUDER_H
#define BEEBEEP_GUIADDUDER_H

#include "ui_GuiAddUser.h"
#include "UserRecord.h"


class GuiAddUser : public QDialog, private Ui::GuiAddUserDialog
{
  Q_OBJECT

public:
  explicit GuiAddUser( QWidget *parent = 0 );

  void loadUsers();

  inline const QList<UserRecord>& usersToAdd() const;

protected slots:
  void addUser();
  void saveUsers();
  void openCustomMenu( const QPoint& );
  void removeUserPath();
  void removeAllUsers();

protected:
  void addUserToList( const UserRecord& );
  bool removeUserPathFromList( const QString& );
  void loadUserPathInList();

private:
  QList<UserRecord> m_users;

};

// Inline Functions
inline const QList<UserRecord>& GuiAddUser::usersToAdd() const { return m_users; }

#endif // BEEBEEP_GUIADDUDER_H
