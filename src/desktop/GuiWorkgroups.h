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

#ifndef BEEBEEP_GUIWORKGROUPS_H
#define BEEBEEP_GUIWORKGROUPS_H

#include "ui_GuiWorkgroups.h"


class GuiWorkgroups : public QDialog, private Ui::GuiWorkgroupsDialog
{
  Q_OBJECT

public:
  explicit GuiWorkgroups( QWidget *parent = 0 );

  void loadWorkgroups();

  inline const QStringList& workgroups() const;
  inline bool restartConnection() const;

protected slots:
  void addWorkgroup();
  void saveWorkgroups();
  void removeWorkgroup();
  void removeAllWorkgroups();
  void openCustomMenu( const QPoint& );

protected:
  void updateWorkgroupList();

private:
  QStringList m_workgroups;
  QMenu* mp_menuContext;
  bool m_restartConnection;

};

// Inline Functions
inline const QStringList& GuiWorkgroups::workgroups() const { return m_workgroups; }
inline bool GuiWorkgroups::restartConnection() const { return m_restartConnection; }

#endif // BEEBEEP_GUIWORKGROUPS_H
