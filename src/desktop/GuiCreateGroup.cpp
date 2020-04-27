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

#include "BeeUtils.h"
#include "ChatManager.h"
#include "GuiCreateGroup.h"
#include "IconManager.h"
#include "Settings.h"
#include "UserManager.h"


GuiCreateGroup::GuiCreateGroup( QWidget *parent )
  : QDialog( parent ), m_group(), m_leaveGroup( false )
{
  setupUi( this );
  setObjectName( "GuiCreateGroup" );
  Bee::removeContextHelpButton( this );
  setWindowIcon( IconManager::instance().icon( "group-create.png" ) );

  QStringList labels;
  labels << tr( "Users" );
  mp_twUsers->setHeaderLabels( labels );

  mp_twUsers->sortItems( 0, Qt::AscendingOrder );
  mp_twUsers->setAlternatingRowColors( true );
  mp_twUsers->setRootIsDecorated( false );
  mp_twUsers->setSelectionMode( QAbstractItemView::MultiSelection );

  mp_pbLeave->setIcon( IconManager::instance().icon( "group-remove" ) );

  connect( mp_pbOk, SIGNAL( clicked() ), this, SLOT( checkAndClose() ) );
  connect( mp_pbCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
  connect( mp_pbLeave, SIGNAL( clicked() ), this, SLOT( leaveGroupAndClose() ) );
}

void GuiCreateGroup::init( const Group& g )
{
  m_group = g;
}

void GuiCreateGroup::loadData()
{
  if( m_group.isValid() )
  {
    mp_leName->setText( m_group.name() );
    setWindowTitle( QString( "%1 - %2" ).arg( tr( "Edit group chat" ) ).arg( Settings::instance().programName() ) );
    mp_pbLeave->setEnabled( true );
  }
  else
  {
    mp_leName->setText( "" );
    setWindowTitle( QString( "%1 - %2" ).arg( tr( "Create new group chat" ) ).arg( Settings::instance().programName() ) );
    mp_pbLeave->setEnabled( false );
  }

  if( mp_twUsers->topLevelItemCount() > 0 )
    mp_twUsers->clear();

  QTreeWidgetItem* item;
  foreach( User u, UserManager::instance().userList().toList() )
  {
    item = new QTreeWidgetItem( mp_twUsers );
    item->setIcon( 0, Bee::userStatusIcon( u.status() ) );
    item->setText( 0, u.name() );
    item->setData( 0, Qt::UserRole+1, u.id() );
    item->setFlags( item->flags() | Qt::ItemIsUserCheckable );
    item->setCheckState( 0, Qt::Checked );

    if( m_group.hasUser( u.id() ) )
    {
      item->setCheckState( 0, Qt::Checked );
      item->setDisabled( !Settings::instance().canRemoveMembersFromGroup() );
    }
    else
    {
      item->setCheckState( 0, Qt::Unchecked );
      item->setDisabled( !Settings::instance().canAddMembersToGroup() );
    }
  }
}

void GuiCreateGroup::checkAndClose()
{
  QList<QTreeWidgetItem*> item_list;
  QTreeWidgetItemIterator it( mp_twUsers, QTreeWidgetItemIterator::Checked );
  while( *it )
  {
    item_list.append( *it );
    ++it;
  }

  if( item_list.isEmpty() )
  {
    QMessageBox::information( this, Settings::instance().programName(), tr( "Please select at least one member for the group." ) );
    return;
  }

  QString group_name_tmp = mp_leName->text().simplified();

  if( group_name_tmp.isEmpty() )
  {
    QMessageBox::information( this, Settings::instance().programName(), tr( "Please insert a group name." ) );
    mp_leName->setFocus();
    return;
  }

  if( group_name_tmp != m_group.name() )
  {
    if( ChatManager::instance().hasName( group_name_tmp ) )
    {
      QMessageBox::information( this, Settings::instance().programName(), tr( "%1 already exists as group name or chat name.\nPlease select a different name." ).arg( group_name_tmp ) );
      mp_leName->setFocus();
      return;
    }
  }

  m_group.setName( group_name_tmp );

  QList<VNumber> selected_users_id;
  selected_users_id.append( ID_LOCAL_USER );

  VNumber user_id = 0;
  foreach( QTreeWidgetItem* item, item_list )
  {
    user_id = Bee::qVariantToVNumber( item->data( 0, Qt::UserRole+1 ) );
    if( !selected_users_id.contains( user_id ) )
      selected_users_id.append( user_id );
  }

  m_group.setUsers( selected_users_id );
  accept();
}

void GuiCreateGroup::leaveGroupAndClose()
{
  m_leaveGroup = true;
  accept();
}
