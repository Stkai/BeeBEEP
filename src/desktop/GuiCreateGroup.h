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

#ifndef BEEBEEP_GUICREATEGROUP_H
#define BEEBEEP_GUICREATEGROUP_H

#include "ui_GuiCreateGroup.h"
#include "Group.h"


class GuiCreateGroup : public QDialog, private Ui::GuiCreateGroup
{
  Q_OBJECT

public:
  explicit GuiCreateGroup( QWidget *parent = Q_NULLPTR );

  void init( const Group& );
  void loadData();

  inline const Group& group() const;
  inline bool leaveGroup() const;

protected slots:
  void checkAndClose();
  void leaveGroupAndClose();

private:
  Group m_group;
  bool m_leaveGroup;

};


// Inline Functions
inline const Group& GuiCreateGroup::group() const { return m_group; }
inline bool GuiCreateGroup::leaveGroup() const { return m_leaveGroup; }

#endif // BEEBEEP_GUICREATEGROUP_H
