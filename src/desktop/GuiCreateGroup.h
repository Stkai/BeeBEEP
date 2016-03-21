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

#ifndef BEEBEEP_GUICREATEGROUP_H
#define BEEBEEP_GUICREATEGROUP_H

#include "ui_GuiCreateGroup.h"
#include "Config.h"


class GuiCreateGroup : public QDialog, private Ui::GuiCreateGroup
{
  Q_OBJECT

public:
  explicit GuiCreateGroup( QWidget *parent = 0 );

  void init( const QString&, const QList<VNumber>& );
  void loadData( bool is_group );

  inline const QString& selectedName() const;
  inline const QList<VNumber>& selectedUsersId() const;

protected slots:
  void checkAndClose();

private:
  QString m_selectedName;
  QList<VNumber> m_selectedUsersId;

};


// Inline Functions
inline const QString& GuiCreateGroup::selectedName() const { return m_selectedName; }
inline const QList<VNumber>& GuiCreateGroup::selectedUsersId() const { return m_selectedUsersId; }


#endif // BEEBEEP_GUICREATEGROUP_H
