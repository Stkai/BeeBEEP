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

#include "GuiConfig.h"
#include "GuiPresetMessageList.h"
#include "Settings.h"


GuiPresetMessageList::GuiPresetMessageList( QWidget* parent )
  : QTreeWidget( parent )
{
  setObjectName( "GuiPresetMessageList" );

  setColumnCount( 1 );
  header()->hide();
  setRootIsDecorated( true );
  setSortingEnabled( true );

  setContextMenuPolicy( Qt::CustomContextMenu );
  setMouseTracking( true );

  mp_actNew = new QAction( QIcon( ":/images/preset-message.png" ), tr( "New" ), this );
  connect( mp_actNew, SIGNAL( triggered() ), this, SLOT( onNew() ) );

  mp_actEdit = new QAction( QIcon( ":/images/preset-message-edit.png" ), tr( "Edit" ), this );
  connect( mp_actEdit, SIGNAL( triggered() ), this, SLOT( onEdit() ) );

  mp_actRename = new QAction( QIcon( ":/images/preset-message-edit.png" ), tr( "Rename" ), this );
  connect( mp_actRename, SIGNAL( triggered() ), this, SLOT( onRename() ) );

  mp_actRemove = new QAction( QIcon( ":/images/preset-message-remove.png" ), tr( "Delete" ), this );
  connect( mp_actRemove, SIGNAL( triggered() ), this, SLOT( onRemove() ) );

  connect( this, SIGNAL( customContextMenuRequested( const QPoint& ) ), this, SLOT( showContextMenu( const QPoint& ) ) );
  connect( this, SIGNAL( itemDoubleClicked( QTreeWidgetItem*, int ) ), this, SLOT( onDoubleClicked( QTreeWidgetItem*, int ) ), Qt::QueuedConnection );
}

QSize GuiPresetMessageList::sizeHint() const
{
  return QSize( BEE_DOCK_WIDGET_SIZE_HINT_WIDTH, BEE_DOCK_WIDGET_SIZE_HINT_HEIGHT );
}

void GuiPresetMessageList::loadFromSettings()
{
  if( topLevelItemCount() > 0 )
    clear();

  QMap<QString,QVariant> preset_messages = Settings::instance().presetMessages();
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

void GuiPresetMessageList::saveInSettings()
{
  QMap<QString,QVariant> preset_messages;
  QTreeWidgetItemIterator it( this );
  while( *it )
  {
    preset_messages.insert( (*it)->data( 0, Qt::UserRole+1 ).toString(), (*it)->data( 0, Qt::UserRole+2 ) );
    ++it;
  }
  Settings::instance().setPresetMessages( preset_messages );
  Settings::instance().save();
}

void GuiPresetMessageList::onDoubleClicked( QTreeWidgetItem* item, int )
{
  if( !item )
    return;

  emit presetMessageSelected( item->data( 0, Qt::UserRole+2 ).toString() );
}

void GuiPresetMessageList::showContextMenu( const QPoint& p )
{
  QMenu menu;
  QTreeWidgetItem* item = itemAt( p );
  if( item )
  {
    menu.addAction( mp_actRename );
    menu.addSeparator();
    menu.addAction( mp_actEdit );
    menu.addSeparator();
    menu.addAction( mp_actRemove );

  }
  else
    menu.addAction( mp_actNew );

  menu.exec( QCursor::pos() );
}

void GuiPresetMessageList::onNew()
{

}

void GuiPresetMessageList::onEdit()
{
}

void GuiPresetMessageList::onRename()
{

}

void GuiPresetMessageList::onRemove()
{
}
