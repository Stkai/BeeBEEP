//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2019 Marco Mastroddi
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

#include "BeeUtils.h"
#include "GuiShortcut.h"
#include "IconManager.h"
#include "Settings.h"
#include "ShortcutManager.h"


GuiShortcut::GuiShortcut( QWidget *parent )
  : QDialog( parent )
{
  setupUi( this );
  setWindowTitle( tr( "Shortcuts" ) + QString( " - %1" ).arg( Settings::instance().programName() ) );
  setWindowIcon( IconManager::instance().icon( "shortcut.png" ) );
  Bee::removeContextHelpButton( this );

  QStringList labels;
  labels << tr( "Key" ) << tr( "Type" ) << tr( "Action" );
  mp_twShortcuts->setHeaderLabels( labels );
  mp_twShortcuts->setAlternatingRowColors( true );
  mp_twShortcuts->setSortingEnabled( true );
  mp_twShortcuts->setRootIsDecorated( false );
  mp_twShortcuts->sortByColumn( 2, Qt::AscendingOrder );

  connect( mp_pbOk, SIGNAL( clicked() ), this, SLOT( saveShortcuts() ) );
  connect( mp_pbCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
  connect( mp_pbDefault, SIGNAL( clicked() ), this, SLOT( restoreDefault() ) );
  connect( mp_twShortcuts, SIGNAL( itemClicked( QTreeWidgetItem*, int ) ), this, SLOT( checkItemClicked( QTreeWidgetItem*, int ) ) );
}

void GuiShortcut::loadShortcuts()
{
  mp_cbUseShortcuts->setChecked( Settings::instance().useShortcuts() );

  mp_twShortcuts->clear();

  QTreeWidgetItem* item;
  for( int i = ShortcutManager::Empty; i < ShortcutManager::NumShortcut; i++ )
  {
    if( i != ShortcutManager::Empty )
    {
      item = new QTreeWidgetItem( mp_twShortcuts );
      item->setText( 0, ShortcutManager::instance().shortcutKey( i ) );
      item->setText( 1, ShortcutManager::instance().isGlobalShortcut( i ) ? tr( "Global" ) : tr( "Local" ) );
      item->setText( 2, ShortcutManager::instance().shortcutName( i ) );
      item->setData( 0, Qt::UserRole+1, i );
#ifndef BEEBEEP_USE_QXT
      if( ShortcutManager::instance().isGlobalShortcut( i ) )
        item->setDisabled( true );
#endif
    }
  }
}

void GuiShortcut::saveShortcuts()
{
  Settings::instance().setUseShortcuts( mp_cbUseShortcuts->isChecked() );
  int shortcut_type;
  QString s_key;
  QTreeWidgetItemIterator it( mp_twShortcuts );
  while( *it )
  {
    shortcut_type = (*it)->data( 0, Qt::UserRole+1 ).toInt();
    ShortcutManager::instance().setShortcut( shortcut_type, (*it)->text( 0 ).simplified() );
    ++it;
  }
  accept();
}

void GuiShortcut::restoreDefault()
{
  ShortcutManager::instance().setDefaultShortcuts();
  loadShortcuts();
}

void GuiShortcut::checkItemClicked( QTreeWidgetItem* item, int )
{
  if( !item )
    return;

  if( item->isDisabled() )
    return;

  QString shortcut_previous_key = item->text( 0 );
  if( shortcut_previous_key.isEmpty() )
  {
#ifdef Q_OS_MAC
    shortcut_previous_key = QString( "Cmd+" );
#else
    shortcut_previous_key = QString( "Ctrl+" );
#endif
  }

  bool ok = false;
  QString shortcut_key = QInputDialog::getText( this, Settings::instance().programName(),
                                                tr( "Insert shorcut for the action: %1" ).arg( item->text( 2 ).toLower() ),
                                                QLineEdit::Normal, shortcut_previous_key, &ok ).simplified();

  if( !ok )
    return;

  if( shortcut_key == shortcut_previous_key )
    return;

  if( !shortcut_key.isEmpty() )
  {
    if( shortcut_key.contains( "," ) )
    {
      QMessageBox::information( this, Settings::instance().programName(), tr( "You cannot use the comma for your shortcut." ), tr( "Ok" ) );
      return;
    }
#ifdef Q_OS_MAC
    shortcut_key.replace( QString( "Ctrl" ), QString( "Meta" ) );
    shortcut_key.replace( QString( "Cmd" ), QString( "Ctrl" ) );
#endif
    QKeySequence ks = QKeySequence::fromString( shortcut_key );
    item->setText( 0, ShortcutManager::instance().shortcutKey( ks ) );
  }
  else
    item->setText( 0, "" );
}
