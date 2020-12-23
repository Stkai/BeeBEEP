//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2021 Marco Mastroddi
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

#ifndef BEEBEEP_GUIPLUGINMANAGER_H
#define BEEBEEP_GUIPLUGINMANAGER_H

#include "ui_GuiPluginManager.h"
#include "Config.h"


class GuiPluginManager : public QDialog, private Ui::GuiPluginManager
{
  Q_OBJECT

public:
  explicit GuiPluginManager( QWidget *parent = Q_NULLPTR );

  void updatePlugins();
  inline bool isChanged() const;

protected slots:
  void pluginSelected( QTreeWidgetItem*, int );
  void showContextMenu( const QPoint& );
  void enableAll();
  void disableAll();
  void togglePlugin();
  void openFolder();
  void loadPlugin();
  void enableSave();

protected:
  inline bool isPluginEnabled( QTreeWidgetItem* ) const;
  inline void setPluginEnabled( QTreeWidgetItem*, bool );
  void updateItem( QTreeWidgetItem* );
  void showContextMenu( QTreeWidgetItem*, const QPoint& );

private:
  bool m_changed;

};

// Inline Functions
inline bool GuiPluginManager::isChanged() const { return m_changed; }
inline bool GuiPluginManager::isPluginEnabled( QTreeWidgetItem* item ) const { return item->data( 0, Qt::UserRole+2 ).toBool(); }
inline void GuiPluginManager::setPluginEnabled( QTreeWidgetItem* item, bool enable ) { item->setData( 0, Qt::UserRole+2, enable ); }

#endif // BEEBEEP_GUIPLUGINMANAGER_H
