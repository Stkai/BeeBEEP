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

#ifndef BEEBEEP_SELECTITEMS_H
#define BEEBEEP_SELECTITEMS_H

#include "Config.h"
#include "ui_GuiSelectItems.h"


class GuiSelectItems : public QDialog, private Ui::GuiSelectItemsWidget
{
  Q_OBJECT

public:
  GuiSelectItems( QWidget* );

  inline QTreeWidget* treeWidget() const;

protected:
  virtual void resetList();
  virtual void updateList();
  virtual void sortList();

protected slots:
  void onOkClicked();
  void onCancelClicked();
  void onClearClicked();
  void searchText( const QString& );
  void onClearFilterClicked();

private:
  QString m_textToSearch;

};


// Inline functions
inline QTreeWidget* GuiSelectItems::treeWidget() const { return mp_tw; }

#endif // BEEBEEP_SELECTITEMS_H
