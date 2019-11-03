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

#include "Core.h"
#include "BeeUtils.h"
#include "Config.h"
#include "GuiWorkgroups.h"
#include "IconManager.h"
#include "Settings.h"


GuiWorkgroups::GuiWorkgroups( QWidget *parent )
  : QDialog( parent ), m_workgroups(), m_restartConnection( false )
{
  setupUi( this );
  setWindowTitle( tr( "Your workgroups" ) + QString( " - %1" ).arg( Settings::instance().programName() ) );
  setWindowIcon( IconManager::instance().icon( "workgroup.png" ) );
  Bee::removeContextHelpButton( this );

  mp_twWorkgroups->setColumnCount( 1 );
  QStringList labels;
  labels << tr( "Workgroups" );
  mp_twWorkgroups->setHeaderLabels( labels );
  mp_twWorkgroups->setAlternatingRowColors( true );
  mp_twWorkgroups->setSortingEnabled( true );
  mp_twWorkgroups->setRootIsDecorated( false );
  mp_twWorkgroups->setContextMenuPolicy( Qt::CustomContextMenu );
  mp_twWorkgroups->setSelectionMode( QAbstractItemView::MultiSelection );
  mp_twWorkgroups->sortByColumn( 0, Qt::AscendingOrder );

  mp_menuContext = new QMenu( this );
  mp_menuContext->addAction( IconManager::instance().icon( "delete.png" ), tr( "Remove workgroup" ), this, SLOT( removeWorkgroup() ) );
  mp_menuContext->addSeparator();
  mp_menuContext->addAction( IconManager::instance().icon( "clear.png" ), tr( "Clear all" ), this, SLOT( removeAllWorkgroups() ) );

  mp_pbAdd->setIcon( IconManager::instance().icon( "add.png" ) );

  connect( mp_pbAdd, SIGNAL( clicked() ), this, SLOT( addWorkgroup() ) );
  connect( mp_pbOk, SIGNAL( clicked() ), this, SLOT( saveWorkgroups() ) );
  connect( mp_pbCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
  connect( mp_twWorkgroups, SIGNAL( customContextMenuRequested( const QPoint& ) ), this, SLOT( openCustomMenu( const QPoint& ) ) );
}

void GuiWorkgroups::updateWorkgroupList()
{
  if( mp_twWorkgroups->topLevelItemCount() > 0 )
    mp_twWorkgroups->clear();
  foreach( QString s_workgroup, m_workgroups )
  {
    QTreeWidgetItem* item = new QTreeWidgetItem( mp_twWorkgroups );
    item->setText( 0, Bee::capitalizeFirstLetter( s_workgroup, false, false ) );
    item->setIcon( 0, IconManager::instance().icon( "workgroup.png" ) );
    item->setToolTip( 0, tr( "Right click to open menu" ) );
  }
}

void GuiWorkgroups::loadWorkgroups()
{
  mp_cbAcceptOnlyWorkgroups->setChecked( Settings::instance().acceptConnectionsOnlyFromWorkgroups() );
  m_workgroups = Settings::instance().localUser().workgroups();
  m_workgroups.sort();
  m_restartConnection = false;
  updateWorkgroupList();
}

void GuiWorkgroups::saveWorkgroups()
{
  if( mp_cbAcceptOnlyWorkgroups->isChecked() != Settings::instance().acceptConnectionsOnlyFromWorkgroups() ||
      !Bee::areStringListEqual( m_workgroups, Settings::instance().localUser().workgroups() ) )
  {
    m_restartConnection = true;
    Settings::instance().setAcceptConnectionsOnlyFromWorkgroups( mp_cbAcceptOnlyWorkgroups->isChecked() );
    beeCore->setLocalUserWorkgroups( m_workgroups );
    Settings::instance().save();
  }
  accept();
}

void GuiWorkgroups::addWorkgroup()
{
  QString s_workgroup = mp_leWorkgroup->text().simplified();

  if( s_workgroup.isEmpty() )
  {
    QMessageBox::information( this, Settings::instance().programName(), tr( "Please insert a workgroup.") );
    mp_leWorkgroup->setFocus();
    return;
  }

  if( m_workgroups.contains( s_workgroup, Qt::CaseInsensitive ) )
  {
    QMessageBox::information( this, Settings::instance().programName(), tr( "This workgroup is already in list." ) );
    mp_leWorkgroup->setFocus();
    return;
  }

  m_workgroups.append( s_workgroup );
  m_workgroups.sort();
  mp_leWorkgroup->clear();
  updateWorkgroupList();
  mp_leWorkgroup->setFocus();
}

void GuiWorkgroups::openCustomMenu( const QPoint& p )
{
  QTreeWidgetItem* item = mp_twWorkgroups->itemAt( p );
  if( !item )
    return;

  if( !item->isSelected() )
    item->setSelected( true );

  mp_menuContext->exec( QCursor::pos() );
}

void GuiWorkgroups::removeWorkgroup()
{
  QList<QTreeWidgetItem*> items = mp_twWorkgroups->selectedItems();
  if( items.isEmpty() )
  {
    QMessageBox::information( this, Settings::instance().programName(), tr( "Please select an item in the list." ) );
    return;
  }

  QStringList workgroups_to_remove;
  foreach( QTreeWidgetItem* item, items )
    workgroups_to_remove.append( item->text( 0 ) );

  if( QMessageBox::question( this, Settings::instance().programName(), tr( "Don't you want to be part of these workgroups anymore?" ) + QString( "\n" ) + workgroups_to_remove.join( ", " ),
                             tr( "Yes, remove me" ), tr( "Cancel" ), QString::null, 1, 1 ) != 0 )
    return;

  foreach( QString s, workgroups_to_remove )
  {
    QString wg_to_remove = s.toLower();
    QStringList::iterator it = m_workgroups.begin();
    while( it != m_workgroups.end() )
    {
      if( it->toLower() == wg_to_remove )
      {
        m_workgroups.erase( it );
        break;
      }
      ++it;
    }
  }
  updateWorkgroupList();
}

void GuiWorkgroups::removeAllWorkgroups()
{
  m_workgroups.clear();
  updateWorkgroupList();
}
