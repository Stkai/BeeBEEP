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

#include "FileDialog.h"
#include "GuiPluginManager.h"
#include "PluginManager.h"
#include "Settings.h"


GuiPluginManager::GuiPluginManager( QWidget *parent )
  : QDialog( parent )
{
  setupUi( this );
  setObjectName( "GuiPluginManager" );
  setWindowTitle( tr( "Plugin Manager - %1" ).arg( Settings::instance().programName() ) );
  setWindowIcon( QIcon( ":/images/plugin.png" ) );

  m_changed = false;

  QStringList labels;
  labels << "" << "" << tr( "Plugin" ) << tr( "Version" ) << tr( "Author" );
  mp_twPlugins->setHeaderLabels( labels );
  mp_twPlugins->setRootIsDecorated( true );
  mp_twPlugins->setSortingEnabled( false );
  mp_twPlugins->setColumnWidth( 0, 18 );
  mp_twPlugins->setColumnWidth( 1, 18 );

  QHeaderView* hv = mp_twPlugins->header();
#if QT_VERSION >= 0x050000
  hv->setSectionResizeMode( 0, QHeaderView::Fixed );
  hv->setSectionResizeMode( 1, QHeaderView::Fixed );
  hv->setSectionResizeMode( 2, QHeaderView::Stretch );
  hv->setSectionResizeMode( 3, QHeaderView::ResizeToContents );
  hv->setSectionResizeMode( 4, QHeaderView::Stretch );
#else
  hv->setResizeMode( 0, QHeaderView::Fixed );
  hv->setResizeMode( 1, QHeaderView::Fixed );
  hv->setResizeMode( 2, QHeaderView::Stretch );
  hv->setResizeMode( 3, QHeaderView::ResizeToContents );
  hv->setResizeMode( 4, QHeaderView::Stretch );
#endif

  mp_leFolder->setText( Settings::instance().pluginPath() );
  mp_pbLoad->setEnabled( false );

  connect( mp_twPlugins, SIGNAL( itemDoubleClicked( QTreeWidgetItem*, int ) ), this, SLOT( pluginSelected( QTreeWidgetItem*, int ) ) );
  connect( mp_twPlugins, SIGNAL( customContextMenuRequested ( const QPoint& ) ), this, SLOT( showContextMenu( const QPoint& ) ) );
  connect( mp_pbEnableAll, SIGNAL( clicked() ), this, SLOT( enableAll() ) );
  connect( mp_pbDisableAll, SIGNAL( clicked() ), this, SLOT( disableAll() ) );
  connect( mp_pbClose, SIGNAL( clicked() ), this, SLOT( close() ) );
  connect( mp_pbFolder, SIGNAL( clicked() ), this, SLOT( openFolder() ) );
  connect( mp_pbLoad, SIGNAL( clicked() ), this, SLOT( loadPlugin() ) );
  connect( mp_leFolder, SIGNAL( textChanged( QString ) ), this, SLOT( enableSave() ) );
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
  if( item->data( 0, Qt::UserRole+1 ).toInt() <= 0 )
    return;

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
  item->setIcon( 1, (enable ? QIcon( ":/images/green-ball.png" ) : QIcon( ":/images/red-ball.png" ) ) );
  item->setToolTip( 1, (enable ? tr( "%1 is enabled" ).arg( item->text( 1 ) ) : tr( "%1 is disabled" ).arg( item->text( 1 ) ) ) );
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
  PluginManager::instance().setPluginEnabled( item->text( 2 ), !enabled );
  setPluginEnabled( item, !enabled );
  updateItem( item );
  m_changed = true;
}

void GuiPluginManager::updatePlugins()
{
  mp_twPlugins->clear();
  QTreeWidgetItem* item;

  if( PluginManager::instance().textMarkers().size() > 0 )
  {
    QTreeWidgetItem* text_marker_root_item = new QTreeWidgetItem( mp_twPlugins );
    text_marker_root_item->setText( 2, tr( "Text Markers") );
    text_marker_root_item->setData( 0, Qt::UserRole+1, 0 );

    foreach( TextMarkerInterface* text_marker, PluginManager::instance().textMarkers() )
    {
      item = new QTreeWidgetItem( text_marker_root_item );
      setPluginEnabled( item, text_marker->isEnabled() );
      item->setIcon( 2, text_marker->icon().isNull() ? QIcon( ":/images/plugin.png" ) : text_marker->icon() );
      item->setText( 2, text_marker->name() );
      item->setText( 3, text_marker->version() );
      item->setText( 4, text_marker->author() );
      item->setData( 0, Qt::UserRole+1, 1 );
      updateItem( item );
    }
  }

  mp_twPlugins->expandAll();
}

void GuiPluginManager::openFolder()
{
  QString plugin_path = FileDialog::getExistingDirectory( this, tr( "%1 - Select the plugin folder" )
                                                                 .arg( Settings::instance().programName() ),
                                                                 Settings::instance().pluginPath() );
  if( plugin_path.isEmpty() )
    return;

  mp_leFolder->setText( plugin_path );
  enableSave();
}

void GuiPluginManager::loadPlugin()
{
  QString dir_path = mp_leFolder->text().simplified();
  QDir dp( dir_path );
  if( !dp.exists() )
  {
    QMessageBox::warning( this, Settings::instance().programName(), tr( "Folder %1 not found." ).arg( dir_path ) );
    return;
  }

  Settings::instance().setPluginPath( dir_path );
  PluginManager::instance().loadPlugins();
  updatePlugins();
  m_changed = true;
  mp_pbLoad->setEnabled( false );
}

void GuiPluginManager::enableSave()
{
  mp_pbLoad->setEnabled( true );
}
