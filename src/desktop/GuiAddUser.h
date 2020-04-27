//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2020 Marco Mastroddi
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
// along with BeeBEEP. If not, see <http://www.gnu.org/licenses/>.
//
// Author: Marco Mastroddi <marco.mastroddi(AT)gmail.com>
//
// $Id$
//
//////////////////////////////////////////////////////////////////////

#ifndef BEEBEEP_GUIADDUSER_H
#define BEEBEEP_GUIADDUSER_H

#include "ui_GuiAddUser.h"
#include "UserRecord.h"


class GuiAddUser : public QDialog, private Ui::GuiAddUserDialog
{
  Q_OBJECT

public:
  explicit GuiAddUser( QWidget *parent = Q_NULLPTR );

  void loadUsers();

  inline const QList<NetworkAddress>& networkAddressesToAdd() const;

protected slots:
  void addUser();
  void saveUsers();
  void openCustomMenu( const QPoint& );
  void removeUserPath();
  void removeAllUsers();
  void addUsersAutoFromLan();

protected:
  void addNetworkAddressToList( const NetworkAddress& );
  bool removeUserPathFromList( const QString& );
  void loadNetworkAddressesInList();

private:
  QList<NetworkAddress> m_networkAddresses;
  QMenu* mp_menuContext;

};

// Inline Functions
inline const QList<NetworkAddress>& GuiAddUser::networkAddressesToAdd() const { return m_networkAddresses; }

#endif // BEEBEEP_GUIADDUSER_H
