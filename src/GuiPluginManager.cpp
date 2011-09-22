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

#include "GuiPluginManager.h"
#include "PluginManager.h"
#include "Settings.h"


GuiPluginManager::GuiPluginManager( QWidget *parent )
  : QDialog( parent )
{
  setupUi( this );
  setObjectName( "GuiPluginManager" );

  setWindowTitle( tr( "Plugin Manager - %1" ).arg( Settings::instance().programName() ) );

  m_changed = false;

  QStringList labels;
  labels << "" << tr( "Plugin" ) << tr( "Version" ) << tr( "Author" );
  mp_twPlugins->setHeaderLabels( labels );
  mp_twPlugins->setRootIsDecorated( false );
  mp_twPlugins->setSortingEnabled( false );
  mp_twPlugins->setColumnWidth( 0, 24 );

  QHeaderView* hv = mp_twPlugins->header();
  hv->setResizeMode( 0, QHeaderView::Fixed );
  hv->setResizeMode( 1, QHeaderView::Stretch );
  hv->setResizeMode( 2, QHeaderView::ResizeToContents );
  hv->setResizeMode( 3, QHeaderView::Stretch );

  connect( mp_twPlugins, SIGNAL( itemDoubleClicked( QTreeWidgetItem*, int ) ), this, SLOT( pluginSelected( QTreeWidgetItem*, int ) ) );
  connect( mp_twPlugins, SIGNAL( customContextMenuRequested ( const QPoint& ) ), this, SLOT( showContextMenu( const QPoint& ) ) );
  connect( mp_pbEnableAll, SIGNAL( clicked() ), this, SLOT( enableAll() ) );
  connect( mp_pbDisableAll, SIGNAL( clicked() ), this, SLOT( disableAll() ) );
  connect( mp_pbClose, SIGNAL( clicked() ), this, SLOT( close() ) );
}

void GuiPluginManager::showContextMenu( const QPoint& pos )
{
  QTreeWidgetItem* item = mp_twPlugins->itemAt( pos );
  if( item )
    showContextMenu( item, QCursor::pos() );
}

void GuiPluginManager::pluginSelected( QTreeWidgetItem* item, int )
{
  if( item )
    showContextMenu( item, QCursor::pos() );
}

void GuiPluginManager::showContextMenu( QTreeWidgetItem* item, const QPoint& pos )
{
  bool enable = isPluginEnabled( item );
  QIcon menu_icon = enable ? QIcon( ":/images/red-ball.png" ) : QIcon( ":/images/green-ball.png" );
  QString menu_text = enable ? tr( "Disable %1" ).arg( item->text( 1 ) ) : tr( "Enable %1" ).arg( item->text( 1 ) );

  QMenu custom_context_menu;
  custom_context_menu.addAction( menu_icon, menu_text, this, SLOT( togglePlugin() ) );
  custom_context_menu.exec( pos );
}

void GuiPluginManager::updateItem( QTreeWidgetItem* item )
{
  bool enable = isPluginEnabled( item );
  item->setIcon( 0, (enable ? QIcon( ":/images/green-ball.png" ) : QIcon( ":/images/red-ball.png" ) ) );
  item->setToolTip( 0, (enable ? tr( "%1 is enabled" ).arg( item->text( 1 ) ) : tr( "%1 is disabled" ).arg( item->text( 1 ) ) ) );
}

void GuiPluginManager::enableAll()
{
  PluginManager::instance().setPluginsEnabled( true );
  updatePlugins();
  m_changed = true;
}

void GuiPluginManager::disableAll()
{
  PluginManager::instance().setPluginsEnabled( false );
  updatePlugins();
  m_changed = true;
}

void GuiPluginManager::togglePlugin()
{
  QList<QTreeWidgetItem*> items = mp_twPlugins->selectedItems();
  if( items.isEmpty() )
  {
    QMessageBox::information( this, Settings::instance().programName(), tr( "Please select a plugin in the list." ) );
    return;
  }

  QTreeWidgetItem* item = items.first();
  if( !item  )
    return;

  bool enabled = isPluginEnabled( item );
  PluginManager::instance().setPluginEnabled( item->text( 1 ), !enabled );
  setPluginEnabled( item, !enabled );
  updateItem( item );
  m_changed = true;
}

void GuiPluginManager::updatePlugins()
{
  mp_twPlugins->clear();
  foreach( TextMarkerInterface* text_marker, PluginManager::instance().textMarkers() )
  {
    QTreeWidgetItem* item = new QTreeWidgetItem( mp_twPlugins );
    setPluginEnabled( item, text_marker->isEnabled() );
    item->setIcon( 1, text_marker->icon().isNull() ? QIcon( ":/images/plugin.png" ) : text_marker->icon() );
    item->setText( 1, text_marker->name() );
    item->setText( 2, text_marker->version() );
    item->setText( 3, text_marker->author() );
    updateItem( item );
  }
}
