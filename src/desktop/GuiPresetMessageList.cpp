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

#include "GuiConfig.h"
#include "GuiPresetMessageList.h"
#include "IconManager.h"
#include "Settings.h"


GuiPresetMessageList::GuiPresetMessageList( QWidget* parent )
  : QTreeWidget( parent )
{
  setObjectName( "GuiPresetMessageList" );

  setColumnCount( 1 );
  header()->hide();
  setRootIsDecorated( false );
  setSortingEnabled( true );

  setContextMenuPolicy( Qt::CustomContextMenu );
  setMouseTracking( true );
  setSelectionMode( QAbstractItemView::SingleSelection );
  setAlternatingRowColors( true );

  mp_actNew = new QAction( IconManager::instance().icon( "preset-message.png" ), tr( "New" ), this );
  mp_actEdit = new QAction( IconManager::instance().icon( "preset-message-edit.png" ), tr( "Edit" ), this );
  mp_actRename = new QAction( IconManager::instance().icon( "preset-message-edit.png" ), tr( "Rename" ), this );
  mp_actRemove = new QAction( IconManager::instance().icon( "preset-message-remove.png" ), tr( "Delete" ), this );

  setToolTip( tr( "Right click on panel to create a new preset message" ) );

  connect( this, SIGNAL( customContextMenuRequested( const QPoint& ) ), this, SLOT( showContextMenu( const QPoint& ) ) );
  connect( this, SIGNAL( itemDoubleClicked( QTreeWidgetItem*, int ) ), this, SLOT( onDoubleClicked( QTreeWidgetItem*, int ) ), Qt::QueuedConnection );
}

QSize GuiPresetMessageList::sizeHint() const
{
  return QSize( BEE_DOCK_WIDGET_SIZE_HINT_WIDTH, BEE_DOCK_WIDGET_SIZE_HINT_HEIGHT_SMALL );
}

void GuiPresetMessageList::loadFromSettings()
{
  if( topLevelItemCount() > 0 )
    clear();

  const QMap<QString,QVariant>& preset_messages = Settings::instance().presetMessages();
  if( preset_messages.isEmpty() )
    return;

  QMap<QString,QVariant>::const_iterator it = preset_messages.constBegin();
  while( it != preset_messages.constEnd() )
  {
    QTreeWidgetItem* item = new QTreeWidgetItem( this );
    if( it.key().isEmpty() )
      item->setText( 0, it.value().toString() );
    else
      item->setText( 0, it.key() );
    item->setData( 0, Qt::UserRole+1, it.key() );
    item->setData( 0, Qt::UserRole+2, it.value() );
    item->setToolTip( 0, it.value().toString() );
    item->setTextAlignment( 0, Qt::AlignCenter );
    ++it;
  }
}

void GuiPresetMessageList::onDoubleClicked( QTreeWidgetItem* item, int )
{
  if( item )
    emit presetMessageSelected( item->data( 0, Qt::UserRole+2 ).toString() );
}

void GuiPresetMessageList::showContextMenu( const QPoint& p )
{
  QMenu* menu = new QMenu( this );
  QTreeWidgetItem* item = itemAt( p );
  if( item )
  {
    menu->addAction( mp_actRename );
    menu->addSeparator();
    menu->addAction( mp_actEdit );
    menu->addSeparator();
    menu->addAction( mp_actRemove );
  }
  else
  {
    menu->addAction( mp_actNew );
    clearSelection();
  }

  QAction* ret_act = menu->exec( QCursor::pos() );
  if( !ret_act )
    return;

  if( ret_act == mp_actRename )
    onRename( item );
  else if( ret_act == mp_actEdit )
    onEdit( item );
  else if( ret_act == mp_actRemove )
    onRemove( item );
  else
    onNew();
}

void GuiPresetMessageList::savePreset( const QString& preset_key, const QString& preset_value )
{
  QMap<QString,QVariant> preset_messages = Settings::instance().presetMessages();
  preset_messages.insert( preset_key, preset_value );
  Settings::instance().setPresetMessages( preset_messages );
}

void GuiPresetMessageList::removePreset( const QString& preset_key )
{
  QMap<QString,QVariant> preset_messages = Settings::instance().presetMessages();
  preset_messages.remove( preset_key );
  Settings::instance().setPresetMessages( preset_messages );
}

void GuiPresetMessageList::onNew()
{
#if QT_VERSION >= 0x050200
  QString new_preset = QInputDialog::getMultiLineText( this, Settings::instance().programName(),
                                                       tr( "Please insert your new preset message." ),
                                                       QString( "" ) );
#else
  QString new_preset = QInputDialog::getText( this, Settings::instance().programName(),
                                              tr( "Please insert your new preset message." ),
                                              QLineEdit::Normal, QString( "" ) );
#endif

  if( new_preset.simplified().isEmpty() )
    return;

  savePreset( new_preset, new_preset );
  loadFromSettings();
  Settings::instance().save();
}

void GuiPresetMessageList::onEdit( QTreeWidgetItem* item )
{
  if( !item )
    return;
#if QT_VERSION >= 0x050200
  QString edit_preset = QInputDialog::getMultiLineText( this, Settings::instance().programName(),
                                                        tr( "Edit your preset message." ),
                                                        item->data( 0, Qt::UserRole+2 ).toString() );
#else
  QString edit_preset = QInputDialog::getText( this, Settings::instance().programName(),
                                               tr( "Edit your preset message." ),
                                               QLineEdit::Normal, item->data( 0, Qt::UserRole+2 ).toString() );
#endif

  if( edit_preset.simplified().isEmpty() )
    return;

  savePreset( item->data( 0, Qt::UserRole+1 ).toString(), edit_preset );
  loadFromSettings();
  Settings::instance().save();
}

void GuiPresetMessageList::onRename( QTreeWidgetItem* item )
{
  if( !item )
    return;
  QString edit_preset_key = QInputDialog::getText( this, Settings::instance().programName(),
                                                          tr( "Edit the name of your preset message." ), QLineEdit::Normal,
                                                          item->data( 0, Qt::UserRole+1 ).toString() );

  if( edit_preset_key.simplified().isEmpty() )
    return;

  removePreset( item->data( 0, Qt::UserRole+1 ).toString() );
  savePreset( edit_preset_key.simplified(), item->data( 0, Qt::UserRole+2 ).toString() );
  loadFromSettings();
  Settings::instance().save();
}

void GuiPresetMessageList::onRemove( QTreeWidgetItem* item )
{
  if( !item )
    return;
  if( QMessageBox::question( this, Settings::instance().programName(), tr( "Do you want to delete this preset?" ),
                             tr( "Yes" ), tr( "No" ), QString(), 1, 1 ) != 0 )
    return;

  removePreset( item->data( 0, Qt::UserRole+1 ).toString() );
  loadFromSettings();
  Settings::instance().save();
}
